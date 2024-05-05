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
#include "menu/HzItemMenu.h"
#include "showItem/HzFileItem.h"
#include "windows/UiOperation.h"
#include "windows/tools.h"

#define MAX_ICON_SIZE			108
#define MEDIUM_ICON_SIZE		90
#define MIN_ICON_SIZE			72
#define ITEM_MIN_X_SPACE        5         // item之间的X方向的最小间隙
#define ITEM_MIN_Y_SPACE        30        // item之间的Y方向的最小间隙

HzDesktopIconView::HzDesktopIconView(QWidget *parent)
	: QListView(parent)
	, m_menuShowStyle(Win10Style)
{
	setFixedSize(1200, 800);

	m_itemProxyModel = new QSortFilterProxyModel(this);
	m_itemModel = new HzDesktopItemModel(m_itemProxyModel);
	m_itemProxyModel->setSourceModel(m_itemModel);

	setModel(m_itemProxyModel);

	// 图标模式
	setViewMode(QListView::IconMode);
	setIconSize(QSize(MAX_ICON_SIZE, MAX_ICON_SIZE));

	// 设置网格间的间距为0，由网格大小控制间距
	setSpacing(0);
	setGridSize(QSize(MAX_ICON_SIZE + ITEM_MIN_X_SPACE, MAX_ICON_SIZE + ITEM_MIN_Y_SPACE));
	setMovement(QListView::Snap); // 设置图标移动模式为对齐到网格
	setDropIndicatorShown(true); // 显示拖放位置指示器

	//setStyleSheet("QListView {background-color: transparent;}");
	setDragEnabled(true);
	setAcceptDrops(true);
	setFlow(QListView::TopToBottom);
	//setSelectionMode(QAbstractItemView::SingleSelection);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
}

HzDesktopIconView::~HzDesktopIconView()
{

}


void HzDesktopIconView::startDrag(Qt::DropActions supportedActions)
{
	m_testData = HZ::multiDrag(getSelectedPaths());

	QDrag* drag = new QDrag(this);
	//drag->setMimeData(mimeData);
	drag->setMimeData(m_testData);
	//drag->setPixmap(item->icon().pixmap(iconSize()));
	drag->setPixmap(QPixmap(":/HzDesktopMgr/view/qrc/test/heart.png"));
	 //开始拖放操作
	Qt::DropAction dropAction = drag->exec(supportedActions);
	if (dropAction == Qt::MoveAction) {
		//currentFilePath.clear();
	}

}

void HzDesktopIconView::dragEnterEvent(QDragEnterEvent* event)
{
	event->setDropAction(Qt::MoveAction);
	event->accept();

	auto strList = event->mimeData()->formats();
	//for (int i = 0; i < strList.size(); i++) {
	for (int i = 0; i < 2; i++) {
		auto data = event->mimeData()->data(strList[i]);
		//if (i >= 2) {
		//	//qDebug() << strList[i] << " : " << data;
		//	continue;
		//}
			
		m_testData->setData(strList[i], data);

		
	}
}

//void HzDesktopIconView::dropEvent(QDropEvent* e)
//{
//	auto strList = e->mimeData()->formats();
//	qDebug() << strList;
//}

void HzDesktopIconView::contextMenuEvent(QContextMenuEvent* event)
{
	HzItemMenu::instance().showMenu(this, getSelectedPaths());
}

QStringList HzDesktopIconView::getSelectedPaths()
{
	QStringList pathList;

	// TODO 此处直接调用函数与复制变量，有什么区别？
	QModelIndexList indexList = selectedIndexes();
	for (const QModelIndex& index : selectedIndexes()) {
		if (!pathList.contains(m_itemModel->filePath(index))) {
			pathList.append(m_itemModel->filePath(index));
		}
	}
		
	return pathList;
}
