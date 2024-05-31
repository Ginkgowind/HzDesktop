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
	addAction(tr("Open"), this, [this]() {emit onOpen(); });
	addSeparator();
	m_copyAct = addAction(tr("Copy"), this, [this]() {emit onCopy(); });
	m_cutAct = addAction(tr("Cut"), this, [this]() {emit onCut(); });
	addSeparator();
	m_deleteAct = addAction(tr("Delete"), this, [this]() {emit onDelete(); });
	m_renameAct = addAction(tr("Rename"), this, [this]() {emit onRename(); }); 
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

void HzItemMenu::showMenu(const QStringList& itemList)
{
	if (itemList.empty()) {

	}
	else if (parent()) {
		// 部分数据存起来，因为打开文件管理器菜单时会用到
		m_showPos = QCursor::pos();
		m_selectedItemList = itemList;
		exec(m_showPos);
	}
}

void HzItemMenu::onExplorerMenu()
{
	HZ::showContentMenuWin10(
		qobject_cast<QWidget*>(parent())->winId(),
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
	viewModeSubMenu->addAction(tr("Large icons"), [this]() {emit onSetIconSizeMode(Large); });
	viewModeSubMenu->addAction(tr("Medium icons"), [this]() {emit onSetIconSizeMode(Medium); });
	viewModeSubMenu->addAction(tr("Small icons"), [this]() {emit onSetIconSizeMode(Small); });
	viewModeSubMenu->addSeparator();
	viewModeSubMenu->addAction(tr("Auto arrange icons"), this, &HzDesktopBlankMenu::switchAutoArrangeIcons);
	viewModeSubMenu->addSeparator();
	viewModeSubMenu->addAction(tr("Show desktop icons"), [this]() {emit onHide(); });

	QMenu* sortBySubMenu = addMenu(tr("Sort by"));
	sortBySubMenu->addAction(tr("Name"), [this]() {emit onSetItemSortRole(FileNameRole); });
	sortBySubMenu->addAction(tr("Size"), [this]() {emit onSetItemSortRole(FileSizeRole); });
	sortBySubMenu->addAction(tr("ItemType"), [this]() {emit onSetItemSortRole(FileTypeRole); });
	sortBySubMenu->addAction(tr("DateModified"), [this]() {emit onSetItemSortRole(FileLastModifiedRole); });

	addAction(tr("Refresh"), this, &HzDesktopBlankMenu::OnRefresh);
}

HzDesktopBlankMenu::~HzDesktopBlankMenu()
{
}

void HzDesktopBlankMenu::switchAutoArrangeIcons()
{
}

void HzDesktopBlankMenu::OnRefresh()
{
	emit refreshDesktopItemsSignal();
}
