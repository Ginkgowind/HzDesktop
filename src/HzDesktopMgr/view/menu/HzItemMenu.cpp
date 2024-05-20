#include <QDebug>
#include <QUrl>
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>

#include "HzItemMenu.h"
#include "windows/UiOperation.h"

HzItemMenu::HzItemMenu(QWidget* parent)
	: QMenu(parent)
{
	addAction(tr("Open"), this, &HzItemMenu::onOpen);
	m_openWithAct = addAction(tr("Open method"), this, &HzItemMenu::onOpenWith);
	addSeparator();
	m_copyAct = addAction(tr("Copy"), this, &HzItemMenu::onCopy);
	m_cutAct = addAction(tr("Cut"), this, &HzItemMenu::onCut);
	addSeparator();
	m_deleteAct = addAction(tr("Delete"), this, &HzItemMenu::onDelete);
	m_renameAct = addAction(tr("Rename"), this, &HzItemMenu::onRename);
	addSeparator();
	addAction(tr("Explorer menu"), this, &HzItemMenu::onExplorerMenu);
}

HzItemMenu::~HzItemMenu()
{}

HzItemMenu& HzItemMenu::instance()
{
	static HzItemMenu ins;
	return ins;
}

void HzItemMenu::showMenu(
	QWidget* parentWidget,
	const QStringList& itemList
)
{
	if (itemList.empty()) {

	}
	else if (parentWidget) {
		// 部分数据存起来，因为打开文件管理器菜单时会用到
		m_showPos = QCursor::pos();
		m_parentWidget = parentWidget;
		m_selectedItemList = itemList;
		exec(m_showPos);
	}
}

void HzItemMenu::onOpen()
{
	for (const QString& path : m_selectedItemList) {
		QDesktopServices::openUrl(QUrl::fromLocalFile(path));
	}
}

void HzItemMenu::onOpenWith()
{
}

void HzItemMenu::onCopy()
{
	QMimeData* mimeData = new QMimeData;
	QList<QUrl> urls;
	for (QString path : m_selectedItemList) {
		urls.append(QUrl::fromLocalFile(path));
	}
	mimeData->setUrls(urls);
	int dropEffect = 5; // 2 for cut and 5 for copy
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream.setByteOrder(QDataStream::LittleEndian);
	stream << dropEffect;
	mimeData->setData("Preferred DropEffect", data);
	QApplication::clipboard()->setMimeData(mimeData);
}

void HzItemMenu::onCut()
{
	QMimeData* mimeData = new QMimeData;
	QList<QUrl> urls;
	for (QString path : m_selectedItemList) {
		urls.append(QUrl::fromLocalFile(path));
	}
	mimeData->setUrls(urls);
	int dropEffect = 2; // 2 for cut and 5 for copy
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream.setByteOrder(QDataStream::LittleEndian);
	stream << dropEffect;
	mimeData->setData("Preferred DropEffect", data);
	QApplication::clipboard()->setMimeData(mimeData);
}

void HzItemMenu::onDelete()
{
	for (QString path : m_selectedItemList) {
		FILEOP_FLAGS dwOpFlags = FOF_ALLOWUNDO | FOF_NO_UI;

		SHFILEOPSTRUCTA fileOp = { 0 };
		fileOp.hwnd = NULL;
		fileOp.wFunc = FO_DELETE; ///> 文件删除操作
		fileOp.pFrom = StrDupA(path.toStdString().c_str());
		fileOp.pTo = NULL;
		fileOp.fFlags = dwOpFlags;
		fileOp.hNameMappings = NULL;
		fileOp.lpszProgressTitle = "hz delete file";

		SHFileOperationA(&fileOp);
	}
}

void HzItemMenu::onRename()
{
}

void HzItemMenu::onExplorerMenu()
{
	HZ::showContentMenuWin10(
		m_parentWidget->winId(),
		m_selectedItemList,
		m_showPos.x(),
		m_showPos.y()
	);
}
//
//switch (m_menuShowStyle)
//{
//case HzStyle:
//
//	break;
//case Win10Style:
//	//HZ::showContentMenuWin10(
//	//	winId(),
//	//	m_itemModel->filePath(indexList[0]),
//	//	QCursor::pos().x(),
//	//	QCursor::pos().y()
//	//);
//	break;
//case Win11Style:
//	break;
//default:
//	break;
//}

HzDesktopBlankMenu::HzDesktopBlankMenu(QObject* parent)
{
	QMenu* viewModeSubMenu = addMenu(tr("View"));
	viewModeSubMenu->addAction(tr("Large icons"), this, &HzDesktopBlankMenu::setViewMode);
	viewModeSubMenu->addAction(tr("Medium icons"), this, &HzDesktopBlankMenu::setViewMode);
	viewModeSubMenu->addAction(tr("Small icons"), this, &HzDesktopBlankMenu::setViewMode);
	viewModeSubMenu->addSeparator();
	viewModeSubMenu->addAction(tr("Auto arrange icons"), this, &HzDesktopBlankMenu::switchAutoArrangeIcons);
	viewModeSubMenu->addSeparator();
	viewModeSubMenu->addAction(tr("Show desktop icons"), this, &HzDesktopBlankMenu::switchShowDesktopIcons);

	QMenu* sortBySubMenu = addMenu(tr("Sort by"));
	sortBySubMenu->addAction(tr("Large icons"), this, &HzDesktopBlankMenu::setSortMode);
	sortBySubMenu->addAction(tr("Medium icons"), this, &HzDesktopBlankMenu::setSortMode);
	sortBySubMenu->addAction(tr("Small icons"), this, &HzDesktopBlankMenu::setSortMode);

	addAction(tr("Refresh"), this, &HzDesktopBlankMenu::OnRefresh);
}

HzDesktopBlankMenu::~HzDesktopBlankMenu()
{
}

void HzDesktopBlankMenu::setViewMode()
{
	QAction* action = (QAction*)sender();
	qDebug() << action->text();
}

void HzDesktopBlankMenu::switchAutoArrangeIcons()
{
}

void HzDesktopBlankMenu::switchShowDesktopIcons()
{
}

void HzDesktopBlankMenu::setSortMode()
{
}

void HzDesktopBlankMenu::OnRefresh()
{
	emit refreshDesktopItemsSignal();
}
