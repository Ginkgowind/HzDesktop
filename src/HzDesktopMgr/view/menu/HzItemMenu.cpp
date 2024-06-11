#include <QDebug>
#include <QDir>
#include <QUrl>
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <windows.h>
#include <shellapi.h>
#include <functional>
#include <memory>

#include "HzItemMenu.h"
#include "HzItemMenu_p.h"
#include "windows/UiOperation.h"
#include "windows/WindowSubclassWrapper.h"
#include "resource.h"

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

	hr = pDesktopShell->CreateViewObject(hwnd, IID_PPV_ARGS(&m_contextMenu));

	m_contextMenu->QueryContextMenu(menu.get(), 0,
		MIN_SHELL_MENU_ID, MAX_SHELL_MENU_ID, CMF_NORMAL);

	// 添加自定义内容
	d->updateMenu(menu.get());

	//WindowSubclassWrapper(hwnd, std::bind(&HzDesktopBlankMenuPrivate::ParentWindowSubclass, d));

	auto subclass = std::make_unique<WindowSubclassWrapper>(hwnd,
		std::bind_front(&HzDesktopBlankMenuPrivate::ParentWindowSubclass, d));

	QPoint pos = QCursor::pos();
	UINT cmd = TrackPopupMenu(menu.get(), TPM_LEFTALIGN | TPM_RETURNCMD,
		pos.x(), pos.y(), 0, hwnd, nullptr);

	subclass.reset();

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
		commandInfo.lpDirectory = StrDupA(QDir::toNativeSeparators(m_param->dirPath).toStdString().c_str());
		commandInfo.nShow = SW_SHOWNORMAL;
		m_contextMenu->InvokeCommand(&commandInfo);
	}
	else {
		HandleCustomMenuItem(cmd);
	}
}

void HzDesktopBlankMenu::HandleCustomMenuItem(UINT cmd)
{
	switch (cmd)
	{
	case IDM_VIEW_LARGE_ICON:
		emit onSetIconSizeMode(LargeIcon);
		break;
	case IDM_VIEW_MEDIUM_ICON:
		emit onSetIconSizeMode(MediumIcon);
		break;
	case IDM_VIEW_SMALL_ICON:
		emit onSetIconSizeMode(SmallIcon);
		break;
	case IDM_VIEW_AUTO_ARRANGE:
		emit switchAutoArrangeStatus();
		break;
	case IDM_VIEW_DOUBLE_CLICK:
		emit switchDoubleClickStatus();
		break;
	case IDM_VIEW_SHOW_DESKTOP:
		emit onHide();
		break;
	case IDM_VIEW_LNK_ARROW:

		break;

	default:
		break;
	}
}

void HzDesktopBlankMenu::initViewSubMenu()
{
	
}

void HzDesktopBlankMenu::initSortSubMenu()
{
	
}