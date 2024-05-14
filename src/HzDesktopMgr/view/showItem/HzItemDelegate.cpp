#include <QPainter>

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
	//option.rect 就是 sizeHint 的值

	paintBackground(painter, option);


	//const QIcon& icon = item->icon();
	//int showX = option.rect.left() + m_iconShowRC.left();
	//int showY = option.rect.top() + m_iconShowRC.top();

	//QPixmap pixmap = icon.pixmap(m_iconShowRC.size());

	//// 部分文件的图标最大只包含了48x48的，但是Qt读取时会自动合成一个256x256的图片，并且将48x48绘制在256大小图片的左上角
	//// 此时缩放后导致显示在左上角一个很小的图片，这里规避处理进行矫正
	//UemQHelper::correctPixmapIfIsInvalid(pixmap, icon);

	//QPixmap scaledPixmap = pixmap.scaled(
	//	m_iconShowRC.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

	//painter->drawPixmap(showX, showY, m_iconShowRC.width(), m_iconShowRC.height(), scaledPixmap);

}

QWidget* HzItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return nullptr;
}

void HzItemDelegate::paintBackground(
	QPainter* painter,
	const QStyleOptionViewItem& option
) const
{
	setBackgroundPainter(painter, option);

	painter->drawRect(option.rect);
}

void HzItemDelegate::paintIcon()
{

}

void HzItemDelegate::painText()
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
