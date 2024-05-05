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
#define ITEM_MIN_X_SPACE        5         // item֮���X�������С��϶
#define ITEM_MIN_Y_SPACE        30        // item֮���Y�������С��϶

HzDesktopIconView::HzDesktopIconView(QWidget *parent)
	: QListView(parent)
	, m_menuShowStyle(Win10Style)
{
	setFixedSize(1200, 800);

	m_itemProxyModel = new QSortFilterProxyModel(this);
	m_itemModel = new HzDesktopItemModel(m_itemProxyModel);
	m_itemProxyModel->setSourceModel(m_itemModel);

	setModel(m_itemProxyModel);

	// ͼ��ģʽ
	setViewMode(QListView::IconMode);
	setIconSize(QSize(MAX_ICON_SIZE, MAX_ICON_SIZE));

	// ���������ļ��Ϊ0���������С���Ƽ��
	setSpacing(0);
	setGridSize(QSize(MAX_ICON_SIZE + ITEM_MIN_X_SPACE, MAX_ICON_SIZE + ITEM_MIN_Y_SPACE));
	setMovement(QListView::Snap); // ����ͼ���ƶ�ģʽΪ���뵽����
	setDropIndicatorShown(true); // ��ʾ�Ϸ�λ��ָʾ��

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
	 //��ʼ�ϷŲ���
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

	// TODO �˴�ֱ�ӵ��ú����븴�Ʊ�������ʲô����
	QModelIndexList indexList = selectedIndexes();
	for (const QModelIndex& index : selectedIndexes()) {
		if (!pathList.contains(m_itemModel->filePath(index))) {
			pathList.append(m_itemModel->filePath(index));
		}
	}
		
	return pathList;
}
