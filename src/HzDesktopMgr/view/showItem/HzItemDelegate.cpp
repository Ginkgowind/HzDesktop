#include <QPainter>
#include <QAbstractItemView>

#include "HzItemDelegate.h"
#include "../HzDesktopIconView.h"
#include "HzDesktopItemModel.h"
#include "HzItemSortProxyModel.h"
#include "windows/UiOperation.h"

#define FONT_PIXEL_SIZE	16

#define POLYGON 4   //等腰三角形直角边长
#define WIDTH 1     //分隔符粗细的一半

static int s_textFlags = Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap;

HzItemDelegate::HzItemDelegate(QObject* parent)
	: QStyledItemDelegate(parent)
	, m_painter(new QPainter)
	, m_metrics(nullptr)
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
	const QString& path = item->data(CustomRoles::FilePathRole).toString();
	if (!m_showPixmapCache.find(path, &showPixmap)) {
		showPixmap = paintIconText(option, itemView->getParam(), item);
		m_showPixmapCache.insert(path, showPixmap);
	}

	painter->drawPixmap(option.rect, showPixmap);

	painter->restore();
}

//QWidget* HzItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
//{
//	return nullptr;
//}

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
		
		HzItemSortProxyModel* itemProxyModel = 
			qobject_cast<HzItemSortProxyModel*>(itemView->model());
		if (!itemProxyModel) {
			break;
		}

		HzDesktopItemModel* itemModel =
			qobject_cast<HzDesktopItemModel*>(itemProxyModel->sourceModel());
		if (!itemModel) {
			break;
		}

		item = itemModel->itemFromIndex(itemProxyModel->mapToSource(index));
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

	// 绘制icon
	QRect iconShowRC(
		QPoint(param.iconMargin.width(), param.iconMargin.height()),
		param.iconSize
	);
	m_painter->drawPixmap(
		iconShowRC,
		item->icon().pixmap(param.iconSize)
	);

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