#include <QPainter>
#include <QAbstractItemView>
#include <QStandardItemModel>

#include "HzItemDelegate.h"
#include "windows/UiOperation.h"

#define POLYGON 4   //等腰三角形直角边长
#define WIDTH 1     //分隔符粗细的一半

HzItemDelegate::HzItemDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{}

HzItemDelegate::~HzItemDelegate()
{}

void HzItemDelegate::setUiParam(
	const QSize& iconSize,
	const int iconMargin,
	const int oneLineHeight
)
{
	m_iconSize = iconSize;
	m_iconMargin = iconMargin;
	m_oneLineHeight = oneLineHeight;
}

QSize HzItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// TODO 这里要结合计算文本行数，最多两行

	return m_iconSize
		+ 2 * QSize(m_iconMargin, m_iconMargin)
		+ QSize(0, m_oneLineHeight);
}

void HzItemDelegate::paint(
	QPainter * painter, 
	const QStyleOptionViewItem& option, 
	const QModelIndex& index
) const
{
	QStandardItem* item = getItemFromParam(option, index);
	if (!item) {
		return;
	}

	//option.rect 就是 sizeHint 的值

	paintBackground(painter, option);

	paintIcon(painter, option, item);

	paintText(painter, option, item);
}

QWidget* HzItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return nullptr;
}

QStandardItem* HzItemDelegate::getItemFromParam(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QStandardItem* item = nullptr;

	do 
	{
		QAbstractItemView* itemView = 
			qobject_cast<QAbstractItemView*>(option.styleObject);
		if (!itemView) {
			break;
		}
		
		QStandardItemModel* itemModel = 
			qobject_cast<QStandardItemModel*>(itemView->model());
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
	setBackgroundPainter(painter, option);

	painter->drawRect(option.rect);
}

void HzItemDelegate::paintIcon(
	QPainter* painter,
	const QStyleOptionViewItem& option,
	QStandardItem* item
) const
{
	const QIcon& itemIcon = item->icon();
	QPoint showPos = option.rect.topLeft() + QPoint(m_iconMargin, m_iconMargin);

	//QPixmap scaledPixmap = pixmap.scaled(
	//	m_iconShowRC.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

	painter->drawPixmap({ showPos , m_iconSize }, itemIcon.pixmap(m_iconSize));
}

void HzItemDelegate::paintText(
	QPainter* painter,
	const QStyleOptionViewItem& option,
	QStandardItem* item
) const
{
}

void HzItemDelegate::setBackgroundPainter(
	QPainter* painter, 
	const QStyleOptionViewItem& option
) const
{
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setPen(Qt::NoPen);

	if (option.state.testFlag(QStyle::State_Selected)
		&& option.state.testFlag(QStyle::State_MouseOver))
	{
		painter->setBrush(QBrush(Qt::red));
	}
	else if (option.state.testFlag(QStyle::State_Selected))
	{
		painter->setBrush(QBrush(Qt::magenta));
	}
	else if (option.state.testFlag(QStyle::State_MouseOver))
	{
		painter->setBrush(QBrush(Qt::yellow));
	}
	else
	{
		painter->setBrush(Qt::transparent);
	}
}
