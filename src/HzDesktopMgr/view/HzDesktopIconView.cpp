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
#define ICON_MARGIN				5		// icon与周围边界的距离，固定不变

#define ITEM_MIN_X_SPACE        10         // item之间的X方向的最小间隙
#define ITEM_MIN_Y_SPACE        20        // item之间的Y方向的最小间隙

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

	// 图标模式，自己指定delegate的话，什么模式倒不重要了
	setViewMode(QListView::IconMode);

	// TODO 通过计算得出gridsize
	//setGridSize(QSize(120, 140));
	//setUniformItemSizes(true);  设置这个之后所有图标的可选范围是相同的，故不设置
	setMovement(QListView::Free);
	setDropIndicatorShown(true); // 显示拖放位置指示器

	//setWordWrap(true);
	//setWrapping(true);

	//setStyleSheet("QListView {background-color: transparent;}");
	setDragEnabled(true);
	setAcceptDrops(true);
	setFlow(QListView::TopToBottom);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	connect(m_desktopBlankMenu, &HzDesktopBlankMenu::refreshDesktopItemsSignal,
		m_itemModel, &HzDesktopItemModel::refreshItems);

	// TODO 这里设置了，但是在paint的时候又无效了，为什么？
	// 推测是doItemsLayout导致的？
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
//	 //开始拖放操作
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

	// TODO 有作用吗
	event->accept();
}

void HzDesktopIconView::paintEvent(QPaintEvent* e)
{
	// TODO 要等待执行完成，并且仅初始化一次。
	// 但是事件循环还是不堵塞为好，还是看看是哪里给清空了吧
	//initItemsPos();

	QListView::paintEvent(e);
}

void HzDesktopIconView::doItemsLayout()
{
	// TODO 此函数为空时，就不会调用到delegate的paint，为什么？
	//QListView::doItemsLayout();
}

QStringList HzDesktopIconView::getSelectedPaths()
{
	QStringList pathList;

	// TODO 了解此处直接调用函数与复制变量，有什么区别？
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
		QModelIndex index = model()->index(i, 0); // 获取指定行列的索引
		QPoint posIndex = index.data(HzDesktopItemModel::PosIndex2DRole).toPoint();
		setPositionForIndex({ posIndex.x() * 120, posIndex.y() * 140 }, index);
	}
}
