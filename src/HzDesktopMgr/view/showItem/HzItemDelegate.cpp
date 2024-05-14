#include <QPainter>

#include "HzItemDelegate.h"
#include "windows/UiOperation.h"

#define POLYGON 4   //����������ֱ�Ǳ߳�
#define WIDTH 1     //�ָ�����ϸ��һ��

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
	// TODO ����Ҫ��ϼ����ı��������������

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
	//option.rect ���� sizeHint ��ֵ

	paintBackground(painter, option);


	//const QIcon& icon = item->icon();
	//int showX = option.rect.left() + m_iconShowRC.left();
	//int showY = option.rect.top() + m_iconShowRC.top();

	//QPixmap pixmap = icon.pixmap(m_iconShowRC.size());

	//// �����ļ���ͼ�����ֻ������48x48�ģ�����Qt��ȡʱ���Զ��ϳ�һ��256x256��ͼƬ�����ҽ�48x48������256��СͼƬ�����Ͻ�
	//// ��ʱ���ź�����ʾ�����Ͻ�һ����С��ͼƬ�������ܴ�����н���
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
