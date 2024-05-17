#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>
#include <QUrl>
#include <QMenu>
#include <QDebug>

#include <QFileSystemModel>

#include "HzDesktopIconView.h"
#include "showItem/HzItemDelegate.h"
#include "menu/HzItemMenu.h"
#include "showItem/HzFileItem.h"
#include "windows/UiOperation.h"
#include "windows/tools.h"

#define MAX_ICON_SIZE			108
#define MEDIUM_ICON_SIZE		90
#define MIN_ICON_SIZE			72
#define ICON_MARGIN				5		// icon����Χ�߽�ľ��룬�̶�����

#define ITEM_MIN_X_SPACE        10         // item֮���X�������С��϶
#define ITEM_MIN_Y_SPACE        20        // item֮���Y�������С��϶

HzDesktopIconView::HzDesktopIconView(QWidget *parent)
	: QListView(parent)
	, m_menuShowStyle(Win10Style)
{
	setFixedSize(1200, 800);

	m_desktopBlankMenu = new HzDesktopBlankMenu(this);

	//m_itemProxyModel = new QSortFilterProxyModel(this);
	//m_itemModel = new HzDesktopItemModel(m_itemProxyModel);
	//m_itemProxyModel->setSourceModel(m_itemModel);
	//setModel(m_itemProxyModel);

	m_itemModel = new HzDesktopItemModel(this);
	setModel(m_itemModel);

	m_itemDelegate = new HzItemDelegate(this);
	m_itemDelegate->setUiParam(
		QSize(MEDIUM_ICON_SIZE, MEDIUM_ICON_SIZE),
		ICON_MARGIN);
	setItemDelegate(m_itemDelegate);

	// ͼ��ģʽ���Լ�ָ��delegate�Ļ���ʲôģʽ������Ҫ��
	setViewMode(QListView::IconMode);

	// TODO ͨ������ó�gridsize
	//setGridSize(QSize(120, 140));
	//setUniformItemSizes(true);  �������֮������ͼ��Ŀ�ѡ��Χ����ͬ�ģ��ʲ�����
	setMovement(QListView::Free);
	setDropIndicatorShown(true); // ��ʾ�Ϸ�λ��ָʾ��

	//setWordWrap(true);
	//setWrapping(true);

	//setStyleSheet("QListView {background-color: transparent;}");
	setDragEnabled(true);
	setAcceptDrops(true);
	setFlow(QListView::TopToBottom);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	connect(m_desktopBlankMenu, &HzDesktopBlankMenu::refreshDesktopItemsSignal,
		m_itemModel, &HzDesktopItemModel::refreshItems);

	// TODO ���������ˣ�������paint��ʱ������Ч�ˣ�Ϊʲô��
	// �Ʋ���doItemsLayout���µģ�
	initItemsPos();
}

HzDesktopIconView::~HzDesktopIconView()
{

}


//void HzDesktopIconView::startDrag(Qt::DropActions supportedActions)
//{
//	QMimeData* dragMimeData = HZ::multiDrag(getSelectedPaths());
//
//	QDrag* drag = new QDrag(this);
//	drag->setMimeData(dragMimeData);
//	//drag->setPixmap(item->icon().pixmap(iconSize()));
//	drag->setPixmap(QPixmap(":/HzDesktopMgr/view/qrc/test/heart.png"));
//	 //��ʼ�ϷŲ���
//	Qt::DropAction dropAction = drag->exec(supportedActions);
//	if (dropAction == Qt::MoveAction) {
//		//currentFilePath.clear();
//	}
//
//}

//void HzDesktopIconView::dragEnterEvent(QDragEnterEvent* event)
//{
//	event->setDropAction(Qt::MoveAction);
//	event->accept();
//}

//void HzDesktopIconView::dropEvent(QDropEvent* e)
//{
//	auto strList = e->mimeData()->formats();
//	qDebug() << strList;
//}

void HzDesktopIconView::contextMenuEvent(QContextMenuEvent* event)
{
	//initItemsPos();

	QStringList selectedPathList = getSelectedPaths();

	if (selectedPathList.empty()) {
		m_desktopBlankMenu->exec(QCursor::pos());
	}
	else {
		HzItemMenu::instance().showMenu(this, selectedPathList);
	}

	// TODO ��������
	event->accept();
}

void HzDesktopIconView::paintEvent(QPaintEvent* e)
{
	// TODO Ҫ�ȴ�ִ����ɣ����ҽ���ʼ��һ�Ρ�
	// �����¼�ѭ�����ǲ�����Ϊ�ã����ǿ��������������˰�
	//initItemsPos();

	QListView::paintEvent(e);
}

void HzDesktopIconView::doItemsLayout()
{
	// TODO �˺���Ϊ��ʱ���Ͳ�����õ�delegate��paint��Ϊʲô��
	//QListView::doItemsLayout();
}

QStringList HzDesktopIconView::getSelectedPaths()
{
	QStringList pathList;

	// TODO �˽�˴�ֱ�ӵ��ú����븴�Ʊ�������ʲô����
	QModelIndexList indexList = selectedIndexes();
	for (const QModelIndex& index : indexList) {
		pathList.append(m_itemModel->filePath(index));
	}
		
	return pathList;
}

void HzDesktopIconView::initItemsPos()
{
	int rowCount = model()->rowCount();
	for (int i = 0; i < rowCount; ++i) {
		QModelIndex index = model()->index(i, 0); // ��ȡָ�����е�����
		QPoint posIndex = index.data(HzDesktopItemModel::PosIndex2DRole).toPoint();
		setPositionForIndex({ posIndex.x() * 120, posIndex.y() * 140 }, index);
	}
}
