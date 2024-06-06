#include <QDebug>
#include <QUrl>
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <ShlObj.h>
#include <wil/resource.h>
#include <wil/com.h>

#include "HzItemMenu.h"
#include "HzItemMenu_p.h"
#include "windows/UiOperation.h"

static const int MIN_SHELL_MENU_ID = 1;
static const int MAX_SHELL_MENU_ID = 1000;

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

HzDesktopBlankMenu::HzDesktopBlankMenu(QWidget* parent, HzDesktopParam* param)
	: QMenu(parent)
	, HzDesktopPublic(new HzDesktopBlankMenuPrivate())
	, m_param(param)
{
	initViewSubMenu();

	initSortSubMenu();

	addAction(tr("Refresh"), this, [this]() {emit refreshDesktop(); });
}

HzDesktopBlankMenu::~HzDesktopBlankMenu()
{
}

void HzDesktopBlankMenu::showMenu()
{
	HZQ_D(HzDesktopBlankMenu);

	wil::unique_hmenu menu(CreatePopupMenu());
	HWND hwnd = reinterpret_cast<HWND>(qobject_cast<QWidget*>(parent())->winId());

	IShellFolder* pDesktopShell = nullptr;
	HRESULT hr = SHGetDesktopFolder(&pDesktopShell);

	wil::com_ptr_nothrow<IContextMenu> contextMenu;
	hr = pDesktopShell->CreateViewObject(hwnd, IID_PPV_ARGS(&contextMenu));

	contextMenu->QueryContextMenu(menu.get(), 0,
		MIN_SHELL_MENU_ID, MAX_SHELL_MENU_ID, CMF_NORMAL);

	// 添加自定义内容
	d->updateMenu(menu.get());

	QPoint pos = QCursor::pos();
	UINT cmd = TrackPopupMenu(menu.get(), TPM_LEFTALIGN | TPM_RETURNCMD,
		pos.x(), pos.y(), 0, hwnd, nullptr);

	if (cmd == 0) {
		return;
	}
	else if (cmd >= MIN_SHELL_MENU_ID && cmd <= MAX_SHELL_MENU_ID)
	{
		CMINVOKECOMMANDINFO commandInfo = {};
		commandInfo.cbSize = sizeof(commandInfo);
		commandInfo.fMask = 0;
		commandInfo.hwnd = hwnd;
		commandInfo.lpVerb = reinterpret_cast<LPCSTR>(MAKEINTRESOURCE(cmd - MIN_SHELL_MENU_ID));
		commandInfo.lpDirectory = m_param->dirPath.toStdString().c_str();
		commandInfo.nShow = SW_SHOWNORMAL;
		contextMenu->InvokeCommand(&commandInfo);
	}
}

void HzDesktopBlankMenu::initViewSubMenu()
{
	QActionGroup* viewModeGroup = new QActionGroup(this);
	QMenu* viewModeSubMenu = addMenu(tr("View"));

	QAction* largeIcon = viewModeSubMenu->addAction(
		tr("Large icons"), [this]() {emit onSetIconSizeMode(LargeIcon); });
	QAction* mediumIcon = viewModeSubMenu->addAction(
		tr("Medium icons"), [this]() {emit onSetIconSizeMode(MediumIcon); });
	QAction* smallIcon = viewModeSubMenu->addAction(
		tr("Small icons"), [this]() {emit onSetIconSizeMode(SmallIcon); });

	viewModeGroup->addAction(largeIcon)->setCheckable(true);
	viewModeGroup->addAction(mediumIcon)->setCheckable(true);
	viewModeGroup->addAction(smallIcon)->setCheckable(true);

	largeIcon->setChecked(m_param->iconMode == LargeIcon);
	mediumIcon->setChecked(m_param->iconMode == MediumIcon);
	smallIcon->setChecked(m_param->iconMode == SmallIcon);

	viewModeSubMenu->addSeparator();

	QAction* autoArrange = viewModeSubMenu->addAction(tr("Auto arrange icons"),
		[this]() {emit switchAutoArrangeStatus(); });
	autoArrange->setCheckable(true);
	autoArrange->setChecked(m_param->bAutoArrange);

	viewModeSubMenu->addSeparator();

	QAction* enableDoubleClick = viewModeSubMenu->addAction(tr("Double click hide desktop"),
		[this]() {emit switchDoubleClickStatus(); });
	enableDoubleClick->setCheckable(true);
	enableDoubleClick->setChecked(m_param->bEnableDoubleClick);

	// 这个要保持选中状态
	QAction* action = viewModeSubMenu->addAction(tr("Show desktop"));
	action->setCheckable(true);
	action->setChecked(true);
	connect(action, &QAction::triggered,
		[this, action]() {emit onHide(); action->setChecked(true); });
}

void HzDesktopBlankMenu::initSortSubMenu()
{
	QActionGroup* sortModeGroup = new QActionGroup(this);
	QMenu* sortModeSubMenu = addMenu(tr("Sort by"));
	QAction* sortByName = sortModeSubMenu->addAction(tr("Name"),
		[this]() {emit onSetItemSortRole(FileNameRole); });
	QAction* sortBySize = sortModeSubMenu->addAction(tr("Size"),
		[this]() {emit onSetItemSortRole(FileSizeRole); });
	QAction* sortByType = sortModeSubMenu->addAction(tr("ItemType"),
		[this]() {emit onSetItemSortRole(FileTypeRole); });
	QAction* sortByTime = sortModeSubMenu->addAction(tr("DateModified"),
		[this]() {emit onSetItemSortRole(FileLastModifiedRole); });

	sortModeGroup->addAction(sortByName)->setCheckable(true);
	sortModeGroup->addAction(sortBySize)->setCheckable(true);
	sortModeGroup->addAction(sortByType)->setCheckable(true);
	sortModeGroup->addAction(sortByTime)->setCheckable(true);

	if (m_param->bAutoArrange) {
		sortByName->setChecked(m_param->sortRole == FileNameRole);
		sortBySize->setChecked(m_param->sortRole == FileSizeRole);
		sortByType->setChecked(m_param->sortRole == FileTypeRole);
		sortByTime->setChecked(m_param->sortRole == FileLastModifiedRole);
	}
}