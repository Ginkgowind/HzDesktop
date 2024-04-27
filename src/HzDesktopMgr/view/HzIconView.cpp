#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>
#include <QUrl>
#include <QMenu>
#include <QDebug>

#include "HzIconView.h"
#include "showItem/HzFileItem.h"
#include "windows/UiOperation.h"
#include "windows/tools.h"

#define MAX_ICON_SIZE			108
#define MEDIUM_ICON_SIZE		90
#define MIN_ICON_SIZE			72
#define ITEM_MIN_X_SPACE        5         // item֮���X�������С��϶
#define ITEM_MIN_Y_SPACE        30        // item֮���Y�������С��϶

HzIconView::HzIconView(QWidget *parent)
	: QListView(parent)
	, m_menuShowStyle(Win10Style)
{
	setFixedSize(1000, 700);

	m_itemProxyModel = new QSortFilterProxyModel(this);
	m_itemModel = new QStandardItemModel(m_itemProxyModel);
	m_itemProxyModel->setSourceModel(m_itemModel);

	setModel(m_itemProxyModel);

	// ͼ��ģʽ
	setViewMode(QListView::IconMode);
	setIconSize(QSize(MAX_ICON_SIZE, MAX_ICON_SIZE));

	// ���������ļ��Ϊ0���������С���Ƽ��
	setSpacing(0);
	setGridSize(QSize(MAX_ICON_SIZE + ITEM_MIN_X_SPACE, MAX_ICON_SIZE + ITEM_MIN_Y_SPACE));

	setStyleSheet("QListView {background-color: transparent;}");
	setAcceptDrops(true);
	setFlow(QListView::TopToBottom);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	for (int i = 0; i < 6; i++) {
		HzFileItem* newItem = new HzFileItem();
		m_itemModel->appendRow(newItem);
	}

	initFilesInfo();

	initContextMenu();


	//connect(this, &QListView::clicked, this, [=](const QModelIndex& index) {
	//	qDebug() << index.row() << " " << index.column();
	//	});
}

HzIconView::~HzIconView()
{

}

void HzIconView::startDrag(Qt::DropActions supportedActions)
{
	// ����index��ȡ��·��ͼ��ȵ�
	QModelIndex index = currentIndex();
	//QStandardItem* p = m_itemModel->itemFromIndex(index);
	QStandardItem* p = m_itemModel->item(index.row());
	HzFileItem* item = dynamic_cast<HzFileItem*>(p);

	QMimeData* mimeData = new QMimeData;
	mimeData->setUrls(QList<QUrl>() << QUrl::fromLocalFile(item->m_filePath));

	QDrag* drag = new QDrag(this);
	drag->setMimeData(mimeData);
	drag->setPixmap(item->icon().pixmap(iconSize()));
	//drag->setPixmap(QPixmap(":/HzDesktopMgr/view/qrc/test/heart.png"));
	// ��ʼ�ϷŲ���
	Qt::DropAction dropAction = drag->exec(supportedActions);
	//if (dropAction == Qt::MoveAction) {
	//	//currentFilePath.clear();
	//}
}

void HzIconView::contextMenuEvent(QContextMenuEvent* event)
{
	switch (m_menuShowStyle)
	{
	case HzStyle:
		m_hzStyleMenu->exec(QCursor::pos());
		break;
	case Win10Style:
		HZ::showContentMenuWin10(
			winId(),
			HzFileItem().m_filePath,
			QCursor::pos().x(),
			QCursor::pos().y()
		);
		break;
	case Win11Style:
		break;
	default:
		break;
	}
}

void HzIconView::initFilesInfo()
{
	std::wstring userDesktopPath = HZ::getUserDesktopPath();
	std::wstring publicDesktopPath = HZ::getPublicDesktopPath();

	std::vector<std::wstring> observeDirPaths = { userDesktopPath , publicDesktopPath };
	m_pathFilesMgr = new PathFilesMgr(observeDirPaths);
	m_pathFilesMgr->startWork();
}

void HzIconView::initContextMenu()
{
	m_hzStyleMenu = new QMenu(this);
	QAction* start = new QAction(tr("start"), this);
	QAction* stop = new QAction(tr("stop"), this);
	QAction* delete_step = new QAction(tr("delete"), this);
	m_hzStyleMenu->addAction(start);
	m_hzStyleMenu->addAction(stop);
	m_hzStyleMenu->addAction(delete_step);
}
