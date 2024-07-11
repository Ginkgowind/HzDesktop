#include <QPainter>
#include <QFileInfo>
#include <QDebug>
#include <QAbstractItemView>
#include <QHBoxLayout>

#include "HzItemDelegate.h"
#include "HzItemTextEditor.h"
#include "../HzDesktopIconView.h"
#include "HzDesktopItemModel.h"
#include "windows/UiOperation.h"

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
	textHeight = getLimitedTextHeight(option.font, textHeight);
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
#define CUSTOM_COLOR 41, 144, 255
	if (option.state.testFlag(QStyle::State_Selected)
		&& option.state.testFlag(QStyle::State_MouseOver)) {
		painter->setBrush(QColor(CUSTOM_COLOR, 140));
	}
	else if (option.state.testFlag(QStyle::State_Selected)) {
		painter->setBrush(QColor(CUSTOM_COLOR, 110));
	}
	else if (option.state.testFlag(QStyle::State_MouseOver)) {
		painter->setBrush(QColor(CUSTOM_COLOR, 60));
	}
	else {
		painter->setBrush(Qt::transparent);
	}

	painter->drawRect(option.rect);
}

void HzItemDelegate::paintText(
	QPainter* painter,
	const QStyleOptionViewItem& option,
	QStandardItem* item
) const
{
	// 非选中状态最大显示两行，选中时会展开全部

	//do
//{
//	if (0 >= elideWidth)
//	{
//		// 不需要截断显示
//		break;
//	}

//	// 计算文本长度是否超长，替换为省略号
//	QFontMetrics fontMetric(m_textFont);
//	int needWidth = fontMetric.horizontalAdvance(text);
//	if (needWidth < elideWidth)
//	{
//		// 能够正常显示
//		break;
//	}

//	// TODO 这里计算由于会自动换行，因此可能还是存在刚好有一个字符不显示的情况
//	QString ellipsis = "...";
//	int ellipsis_width = fontMetric.boundingRect(ellipsis).width();
//	showText = fontMetric.elidedText(
//		text, Qt::ElideRight, elideWidth - ellipsis_width);

//} while (0);
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

	if (option.state.testFlag(QStyle::State_Editing)) {
		int a = 1;
	}

	if (!option.state.testFlag(QStyle::State_Editing)) {
		// 绘制显示名字
		//m_painter->setPen(Qt::white);
		m_painter->setFont(option.font);

		QRectF textLimitRC(
			QPoint(0, option.rect.width()),
			QPoint(option.rect.width(), option.rect.height())
		);

		QRect textShowRC = m_painter->boundingRect(textLimitRC, item->text(), m_textOption).toRect();

		m_painter->drawText(
			textShowRC,
			item->text(),
			m_textOption
		);
	}

	// 绘制结束
	m_painter->end();

	return retPixmap;
}

inline int HzItemDelegate::getLimitedTextHeight(const QFont& font, int height) const
{
	// 文本显示最多两行
#define MAX_TEXT_SHOW_LINE 2

	QFontMetrics fontMetrics(font);

	return qMin(height, MAX_TEXT_SHOW_LINE * fontMetrics.lineSpacing());;
}