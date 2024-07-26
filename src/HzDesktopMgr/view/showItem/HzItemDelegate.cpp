#include <QPainter>
#include <QFileInfo>
#include <QDebug>
#include <QAbstractItemView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsDropShadowEffect>

#include "HzItemDelegate.h"
#include "HzItemTextEditor.h"
#include "../HzDesktopIconView.h"
#include "HzDesktopItemModel.h"
#include "windows/UiOperation.h"

#define TEXT_BLUR_RADIUS		2
#define TEXT_BLUR_WIDTH_DELTA	4
#define TEXT_BLUR_DRAW_COUNT	3

HzItemDelegate::HzItemDelegate(QObject* parent, HzDesktopParam* param)
	: QStyledItemDelegate(parent)
	, m_painter(new QPainter)
{
	m_textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
	m_textOption.setAlignment(Qt::AlignHCenter | Qt::AlignTop);
}

HzItemDelegate::~HzItemDelegate()
{
	delete m_painter;
}

QSize HzItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QSize sizeRet(option.rect.size());
	QRectF textLimitRC(QPoint(0, 0), sizeRet);
	QPixmap pixmap(option.rect.size());

	m_painter->begin(&pixmap);
	m_painter->setFont(option.font);
	int textHeight = (int)m_painter->boundingRect(textLimitRC,
		index.data(Qt::DisplayRole).toString(), m_textOption).height();
	if (!option.state.testFlag(QStyle::State_On)) {
		textHeight = qMin(textHeight,
			MAX_TEXT_SHOW_LINE * QFontMetrics(option.font).lineSpacing());
	}
	m_painter->end();

	return sizeRet + QSize(0, textHeight);
}

void HzItemDelegate::paint(
	QPainter * painter, 
	const QStyleOptionViewItem& option, 
	const QModelIndex& index
) const
{
	HzDesktopIconView* itemView =
		qobject_cast<HzDesktopIconView*>(option.styleObject);
	QStandardItem* item = getItemFromOption(option, index);
	if (!itemView || !item || !item->isEnabled()) {
		return;
	}
	
	painter->save();
	//option.rect 就是 sizeHint 的值
	paintBackground(painter, option);

	QPixmap showPixmap;
	// 索引设置为 图标大小+路径，以便能够区分不同大小的缓存
	// TODO先这样传吧，后续优化param传递方式
	const QString key = QString::number(itemView->getParam().iconMode) + item->data(CustomRoles::FilePathRole).toString();

	if (!QPixmapCache::find(key, &showPixmap)) {
		showPixmap = paintIconText(option, itemView->getParam(), item);
		QPixmapCache::insert(key, showPixmap);
	}

	painter->drawPixmap(option.rect, showPixmap);

	painter->restore();
}

QWidget* HzItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	HzItemTextEditor* editor = new HzItemTextEditor(parent, option, m_textOption);

	return editor;
}

void HzItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	QStyledItemDelegate::setEditorData(editor, index);
	QFileInfo fileInfo(index.data(CustomRoles::FilePathRole).toString());
	HzItemTextEditor* textEditor = qobject_cast<HzItemTextEditor*>(editor);
	// 设置最小高度为一行
	textEditor->setMinimumHeight(QFontMetrics(editor->font()).lineSpacing() + EDITOR_Y_ADJUSTMENT);

	if (fileInfo.isDir()) {
		textEditor->selectAll();
		return;
	}

	int suffixPos = fileInfo.fileName().lastIndexOf('.');
	if (suffixPos == -1) {
		// 没有后缀也全选
		textEditor->selectAll();
		return;
	}

	// 选择后缀前的所有文本
	QTextCursor cursor = textEditor->textCursor();
	cursor.setPosition(0);
	cursor.setPosition(suffixPos, QTextCursor::KeepAnchor);
	textEditor->setTextCursor(cursor);
}

void HzItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// 图标显示区域为正方形
	int sideLength = option.rect.width();
	QPoint middleTop = option.rect.topLeft() + QPoint{ sideLength / 2, sideLength };

	HzItemTextEditor* hzEditor = qobject_cast<HzItemTextEditor*>(editor);
	hzEditor->updateGeometryParam(middleTop, sideLength);
}

bool HzItemDelegate::eventFilter(QObject* object, QEvent* event)
{
	// 看源码可知 do not filter enter / return / tab / backtab for QTextEdit or QPlainTextEdit
	// 所以此处自行实现

	// TODO 了解Qt对键鼠事件的分发，类似于js中冒泡之类的
	QWidget* editor = qobject_cast<QWidget*>(object);
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
		switch (keyEvent->key()) {
		case Qt::Key_Enter:
		case Qt::Key_Return:
			emit commitData(editor);
			emit closeEditor(editor, QAbstractItemDelegate::SubmitModelCache);
			return true;
		}
	}

	return QStyledItemDelegate::eventFilter(object, event);
}

QStandardItem* HzItemDelegate::getItemFromOption(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QStandardItem* item = nullptr;

	do 
	{
		QAbstractItemView* itemView = 
			qobject_cast<QAbstractItemView*>(option.styleObject);
		if (!itemView) {
			break;
		}
		
		HzDesktopItemModel* itemModel =
			qobject_cast<HzDesktopItemModel*>(itemView->model());
		if (!itemModel) {
			break;
		}

		item = itemModel->itemFromIndex(index);
	} while (false);

	return item;
}

void HzItemDelegate::paintBackground(
	QPainter* painter,
	const QStyleOptionViewItem& option
) const
{
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setPen(Qt::NoPen);

	//255, 255, 255,
//#define CUSTOM_COLOR 41, 144, 255
#define CUSTOM_COLOR 255, 255, 255
	if (option.state.testFlag(QStyle::State_Selected)
		&& option.state.testFlag(QStyle::State_MouseOver)) {
		painter->setBrush(QColor(CUSTOM_COLOR, 180));
	}
	else if (option.state.testFlag(QStyle::State_Selected)) {
		painter->setBrush(QColor(CUSTOM_COLOR, 140));
	}
	else if (option.state.testFlag(QStyle::State_MouseOver)) {
		painter->setBrush(QColor(CUSTOM_COLOR, 80));
	}
	else {
		painter->setBrush(Qt::transparent);
	}

	painter->drawRect(option.rect);
}

QPixmap HzItemDelegate::paintIconText(
	const QStyleOptionViewItem& option,
	const HzDesktopParam& param,
	QStandardItem* item
) const
{
	// 这里的size好像不用改
	QPixmap retPixmap(option.rect.size());
	retPixmap.fill(Qt::transparent);

	 //切换绘制目标
	m_painter->begin(&retPixmap);

	// 取可能的最大图标
	QPixmap iconPixmap = item->icon().pixmap(MAX_ICON_SIZE, MAX_ICON_SIZE);
	QPoint iconPos(param.iconMargin.width(), param.iconMargin.height());
	if (iconPixmap.size().width() < param.iconSize.width() &&
		iconPixmap.size().height() < param.iconSize.height()) {
		iconPos += QPoint(
			(param.iconSize.width() - iconPixmap.size().width()) / 2,
			(param.iconSize.height() - iconPixmap.size().height()) / 2
		);
	}
	else {
		iconPixmap = iconPixmap.scaled(param.iconSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	}

	// 绘制icon
	m_painter->drawPixmap(iconPos, iconPixmap);

	// 图标过小就绘制边框
	if (!iconPixmap.size().isEmpty() &&
		iconPixmap.size().width() < param.iconSize.width() * 2 / 3 &&
		iconPixmap.size().height() < param.iconSize.height() * 2 / 3) {
		m_painter->save();
		m_painter->setBrush(Qt::NoBrush);
		m_painter->setPen(Qt::white);
		m_painter->drawRect(
			param.iconMargin.width(), param.iconMargin.width(),
			param.iconSize.width(), param.iconSize.height()
		);
		m_painter->restore();
	}

	if (!option.state.testFlag(QStyle::State_Editing)) {
		// 绘制显示名字
		m_painter->setFont(option.font);
		m_painter->setPen(Qt::white);

		QRectF textLimitRC(
			QPoint(0, option.rect.width()),
			QPoint(option.rect.width(), option.rect.height())
		);

		QString displayText = item->text();
		QRect textShowRC = m_painter->boundingRect(textLimitRC, displayText, m_textOption).toRect();
		int maxHeight = MAX_TEXT_SHOW_LINE * QFontMetrics(option.font).lineSpacing();
		// TODO 下面嵌套太多，优化
		if (!option.state.testFlag(QStyle::State_On)) {
			// 实现名称过长时的省略显示
			if (textShowRC.height() > maxHeight) {
				while (displayText.size() > 0) {
					displayText.chop(1);
					textShowRC = m_painter->boundingRect(textLimitRC, displayText + "...", m_textOption).toRect();
					if (textShowRC.height() <= maxHeight) {
						displayText += "...";
						break;
					}
				}
			}
		}

		// 宽度略微加宽
		textShowRC.setWidth(textShowRC.width() + TEXT_BLUR_WIDTH_DELTA);

		// 创建一个临时的 QImage，并绘制文本
		QImage tempImage(textShowRC.size(), QImage::Format_ARGB32);
		tempImage.fill(Qt::transparent);
		QPainter tempPainter(&tempImage);
		tempPainter.setFont(option.font);
		tempPainter.setPen(Qt::black);
		tempPainter.drawText(QRect(QPoint(0, 0), textShowRC.size()), displayText, m_textOption);

		// 创建模糊效果
		QGraphicsScene scene;
		QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(tempImage));
		QGraphicsBlurEffect* blurEffect = new QGraphicsBlurEffect();
		blurEffect->setBlurRadius(TEXT_BLUR_RADIUS);
		pixmapItem->setGraphicsEffect(blurEffect);
		scene.addItem(pixmapItem);

		// 渲染场景到另一个 QImage
		QImage blurTextPixmap(textShowRC.size(), QImage::Format_ARGB32);
		blurTextPixmap.fill(Qt::transparent);
		QPainter imagePainter(&blurTextPixmap);
		scene.render(&imagePainter);

		// 将模糊后的 QImage 绘制到窗口，多绘制几次增加颜色深度
		for (int i = 0; i < TEXT_BLUR_DRAW_COUNT; i++) {
			m_painter->drawImage(textShowRC.topLeft(), blurTextPixmap);
		}

		// 再绘制真正的文本
		m_painter->drawText(textShowRC, displayText, m_textOption);
	}

	// 绘制结束
	m_painter->end();

	return retPixmap;
}