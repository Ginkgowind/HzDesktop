#include <QPainter>
#include <QDebug>
#include <QAbstractItemView>

#include "HzItemDelegate.h"
#include "HzItemTextEditor.h"
#include "../HzDesktopIconView.h"
#include "HzDesktopItemModel.h"
#include "windows/UiOperation.h"

#define FONT_PIXEL_SIZE	16

#define POLYGON 4   //等腰三角形直角边长
#define WIDTH 1     //分隔符粗细的一半

static int s_textFlags = Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap;

HzItemDelegate::HzItemDelegate(QObject* parent, HzDesktopParam* param)
	: QStyledItemDelegate(parent)
	, m_painter(new QPainter)
	, m_metrics(nullptr)
	, m_param(param)
{
	QFont itemFont("Microsoft YaHei");
	itemFont.setPixelSize(16);

	m_painter->setFont(itemFont);

	m_metrics = new QFontMetrics(itemFont);
}

HzItemDelegate::~HzItemDelegate()
{
	delete m_painter;
}

QSize HzItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	HzDesktopIconView* itemView =
		qobject_cast<HzDesktopIconView*>(option.styleObject);

	QStandardItem* item = getItemFromOption(option, index);
	if (!item || !itemView) {
		return {0, 0};
	}

	auto& param = itemView->getParam();

	QSize sizeRet = param.iconSize + 2 * param.iconMargin;

	constexpr int customDeltaHeight = 6;
	int textHeight = m_metrics->boundingRect(0, 0, sizeRet.width(), sizeRet.height(),
		s_textFlags, item->text()).height() - customDeltaHeight;


	sizeRet += {0, textHeight};

	// TODO 这里要结合计算文本行数，最多两行

	return sizeRet;
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
	// 索引设置为 图标大小枚举+路径
	const QString& key = QString::number(m_param->iconMode) + item->data(CustomRoles::FilePathRole).toString();

	if (!QPixmapCache::find(key, &showPixmap)) {
		showPixmap = paintIconText(option, itemView->getParam(), item);
		QPixmapCache::insert(key, showPixmap);
	}

	painter->drawPixmap(option.rect, showPixmap);

	painter->restore();
}

QWidget* HzItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return new HzItemTextEditor(parent);
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
	QPixmap retPixmap(option.rect.size());
	retPixmap.fill(Qt::transparent);

	 //切换绘制目标
	m_painter->begin(&retPixmap);


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

	// 绘制显示名字
	//m_painter->setPen(Qt::white);

	QRect textShowRC(
		QPoint(0, param.iconSize.height() + 2 * param.iconMargin.height()),
		QPoint(option.rect.width(), option.rect.height())
	);

	m_painter->drawText(
		textShowRC,
		s_textFlags,
		item->text()
	);

	// 绘制结束
	m_painter->end();

	return retPixmap;
}