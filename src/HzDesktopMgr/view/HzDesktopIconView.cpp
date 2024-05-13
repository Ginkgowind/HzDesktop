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

#define CUSTOM_ITEM_SIZE		108
#define MAX_ICON_SIZE			108
#define MEDIUM_ICON_SIZE		90
#define MIN_ICON_SIZE			72
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
	m_itemDelegate->setItemSize(
		QSize(CUSTOM_ITEM_SIZE, CUSTOM_ITEM_SIZE));
	setItemDelegate(m_itemDelegate);

	// ͼ��ģʽ
	setViewMode(QListView::IconMode);
	//setIconSize(QSize(MAX_ICON_SIZE, MAX_ICON_SIZE));

	setGridSize(QSize(CUSTOM_ITEM_SIZE + ITEM_MIN_X_SPACE, CUSTOM_ITEM_SIZE + ITEM_MIN_Y_SPACE));
	setUniformItemSizes(true);
	setMovement(QListView::Snap); // ����ͼ���ƶ�ģʽΪ���뵽����
	setDropIndicatorShown(true); // ��ʾ�Ϸ�λ��ָʾ��

	//setWordWrap(true);
	setWrapping(true);

	//setStyleSheet("QListView {background-color: transparent;}");
	setDragEnabled(true);
	setAcceptDrops(true);
	setFlow(QListView::TopToBottom);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	connect(m_desktopBlankMenu, &HzDesktopBlankMenu::refreshDesktopItemsSignal,
		m_itemModel, &HzDesktopItemModel::refreshItems);
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
