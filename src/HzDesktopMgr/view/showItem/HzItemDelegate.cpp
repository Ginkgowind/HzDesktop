#include <QPainter>
#include <QDebug>
#include <QAbstractItemView>

#include "HzItemDelegate.h"
#include "HzItemTextEditor.h"
#include "../HzDesktopIconView.h"
#include "HzDesktopItemModel.h"
#include "windows/UiOperation.h"

#define FONT_PIXEL_SIZE	16

#define POLYGON 4   //����������ֱ�Ǳ߳�
#define WIDTH 1     //�ָ�����ϸ��һ��

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

	// TODO ����Ҫ��ϼ����ı��������������

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
	//option.rect ���� sizeHint ��ֵ
	paintBackground(painter, option);

	QPixmap showPixmap;
	// ��������Ϊ ͼ���Сö��+·��
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
	// ��ѡ��״̬�����ʾ���У�ѡ��ʱ��չ��ȫ��

	//do
//{
//	if (0 >= elideWidth)
//	{
//		// ����Ҫ�ض���ʾ
//		break;
//	}

//	// �����ı������Ƿ񳬳����滻Ϊʡ�Ժ�
//	QFontMetrics fontMetric(m_textFont);
//	int needWidth = fontMetric.horizontalAdvance(text);
//	if (needWidth < elideWidth)
//	{
//		// �ܹ�������ʾ
//		break;
//	}

//	// TODO ����������ڻ��Զ����У���˿��ܻ��Ǵ��ڸպ���һ���ַ�����ʾ�����
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

	 //�л�����Ŀ��
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

	// ����icon
	m_painter->drawPixmap(iconPos, iconPixmap);

	// ͼ���С�ͻ��Ʊ߿�
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

	// ������ʾ����
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

	// ���ƽ���
	m_painter->end();

	return retPixmap;
}