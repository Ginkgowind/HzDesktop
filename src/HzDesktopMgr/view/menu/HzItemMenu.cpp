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

HzMenuBase::HzMenuBase(QWidget* parent)
	: QMenu(parent)
{
}

LRESULT HzMenuBase::ParentWindowSubclass(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_MEASUREITEM:
	case WM_DRAWITEM:
	case WM_INITMENUPOPUP:
	case WM_MENUCHAR:
		// wParam is 0 if this item was sent by a menu.
		if ((msg == WM_MEASUREITEM || msg == WM_DRAWITEM) && wParam != 0)
		{
			break;
		}

		if (!m_contextMenu)
		{
			break;
		}

		if (auto contextMenu3 = m_contextMenu.try_query<IContextMenu3>())
		{
			LRESULT result;
			HRESULT hr = contextMenu3->HandleMenuMsg2(msg, wParam, lParam, &result);

			if (SUCCEEDED(hr))
			{
				return result;
			}
		}
		else if (auto contextMenu2 = m_contextMenu.try_query<IContextMenu2>())
		{
			contextMenu2->HandleMenuMsg(msg, wParam, lParam);
		}
		break;
	}

	return DefSubclassProc(hwnd, msg, wParam, lParam);
}


HzItemMenu::HzItemMenu(QWidget* parent)
	: HzMenuBase(parent)
	, HzDesktopPublic(new HzItemMenuPrivate())
{
	addAction(tr("Open"), this, [this]() {emit onOpen(); });
	addSeparator();
	m_copyAct = addAction(tr("Copy"), this, [this]() {handleCopy(m_selectedItemList); });
	m_cutAct = addAction(tr("Cut"), this, [this]() {handleCut(m_selectedItemList); });
	addSeparator();
	m_deleteAct = addAction(tr("Delete"), this, [this]() {handleDelete(m_selectedItemList); });
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
		return;
	}

	// 部分数据存起来，因为打开文件管理器菜单时会用到
	m_showPos = QCursor::pos();
	m_selectedItemList = itemList;
	exec(m_showPos);
}

void HzItemMenu::handleCopy(const QStringList& itemList)
{
	HZQ_D(HzItemMenu);

	d->executeActionFromContextMenu(itemList, "copy");
}

void HzItemMenu::handleCut(const QStringList& itemList)
{
	HZQ_D(HzItemMenu);

	d->executeActionFromContextMenu(itemList, "cut");
}

void HzItemMenu::handleDelete(const QStringList& itemList)
{
	HZQ_D(HzItemMenu);

	d->executeActionFromContextMenu(itemList, "delete");
}

// TODO 后续再考虑支持win11风格菜单
void HzItemMenu::onExplorerMenu()
{
	HZQ_D(HzItemMenu);

	std::vector<LPITEMIDLIST> idChildvec;

	do
	{
		if (m_selectedItemList.empty()) {
			break;
		}

		idChildvec = d->getDesktopPidcFromPaths(m_selectedItemList);
		if (idChildvec.empty()) {
			break;
		}

		wil::com_ptr<IShellFolder> pShellFolder;
		HRESULT hr = SHGetDesktopFolder(&pShellFolder);
		if (FAILED(hr)) {
			break;
		}

		HWND hwnd = reinterpret_cast<HWND>(qobject_cast<QWidget*>(parent())->winId());
		hr = pShellFolder->GetUIObjectOf(hwnd, (UINT)idChildvec.size(),
			const_cast<LPCITEMIDLIST*>(idChildvec.data()),
			IID_IContextMenu, nullptr, (void**)&m_contextMenu);
		if (FAILED(hr)) {
			break;
		}

		wil::unique_hmenu hMenu(CreatePopupMenu());
		hr = m_contextMenu->QueryContextMenu(hMenu.get(), 0, 1, 0x7FFF, CMF_NORMAL);
		if (FAILED(hr)) {
			break;
		}

		auto subclass = std::make_unique<WindowSubclassWrapper>(hwnd,
			std::bind_front(&HzMenuBase::ParentWindowSubclass, this));

		int iCmd = TrackPopupMenuEx(hMenu.get(), TPM_RETURNCMD, 
			m_showPos.x(), m_showPos.y(), hwnd, nullptr);

		subclass.reset();

		if (iCmd > 0) {   //执行菜单命令
			CMINVOKECOMMANDINFOEX info = { 0 };
			info.cbSize = sizeof(info);
			info.fMask = CMIC_MASK_UNICODE;
			info.hwnd = hwnd;
			info.lpVerb = MAKEINTRESOURCEA(iCmd - 1);
			info.lpVerbW = MAKEINTRESOURCEW(iCmd - 1);
			info.nShow = SW_SHOWNORMAL;
			m_contextMenu->InvokeCommand((LPCMINVOKECOMMANDINFO)&info);
		}
	} while (false);

	for (auto& pidl : idChildvec) {
		ILFree(pidl);
	}
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

HzDesktopBkgMenu::HzDesktopBkgMenu(QWidget* parent, HzDesktopParam* param)
	: HzMenuBase(parent)
	, HzDesktopPublic(new HzDesktopBkgMenuPrivate())
	, m_param(param)
	, m_showSortStatus(false)
{
}

HzDesktopBkgMenu::~HzDesktopBkgMenu()
{
}

void HzDesktopBkgMenu::showMenu()
{
	HZQ_D(HzDesktopBkgMenu);

	wil::unique_hmenu menu(CreatePopupMenu());
	HWND hwnd = reinterpret_cast<HWND>(qobject_cast<QWidget*>(parent())->winId());

	IShellFolder* pDesktopShell = nullptr;
	HRESULT hr = SHGetDesktopFolder(&pDesktopShell);

	hr = pDesktopShell->CreateViewObject(hwnd, IID_PPV_ARGS(&m_contextMenu));

	m_contextMenu->QueryContextMenu(menu.get(), 0,
		MIN_SHELL_MENU_ID, MAX_SHELL_MENU_ID, CMF_NORMAL);

	// 添加自定义内容
	d->updateMenu(menu.get());

	auto subclass = std::make_unique<WindowSubclassWrapper>(hwnd,
		std::bind_front(&HzMenuBase::ParentWindowSubclass, this));

	QPoint pos = QCursor::pos();
	UINT cmd = TrackPopupMenu(menu.get(), TPM_LEFTALIGN | TPM_RETURNCMD,
		pos.x(), pos.y(), 0, hwnd, nullptr);

	subclass.reset();

	if (cmd == 0) {
		return;
	}
	else if (cmd >= MIN_SHELL_MENU_ID && cmd <= MAX_SHELL_MENU_ID)
	{
		std::string workDir = QDir::toNativeSeparators(m_param->dirPath).toStdString();
		CMINVOKECOMMANDINFO commandInfo = {};
		commandInfo.cbSize = sizeof(commandInfo);
		commandInfo.fMask = 0;
		commandInfo.hwnd = hwnd;
		commandInfo.lpVerb = reinterpret_cast<LPCSTR>(MAKEINTRESOURCE(cmd - MIN_SHELL_MENU_ID));
		commandInfo.lpDirectory = workDir.c_str();
		commandInfo.nShow = SW_SHOWNORMAL;
		m_contextMenu->InvokeCommand(&commandInfo);

		// 是否选择了新建文件或文件夹
		QVector<UINT> newFileCmds = d->getNewFileCmdsVec(menu.get());
		if (newFileCmds.contains(cmd)) {
			emit onNewFile();
		}
	}
	else {
		handleCustomMenuItem(cmd);
	}
}

void HzDesktopBkgMenu::handleCustomMenuItem(UINT cmd)
{
	HZQ_D(HzDesktopBkgMenu);

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
		if (!m_param->bAutoArrange) {
			emit enableAutoArrange();
		}
		m_param->bAutoArrange = !m_param->bAutoArrange;
		break;
	case IDM_VIEW_DOUBLE_CLICK:
		m_param->bEnableDoubleClick = !m_param->bEnableDoubleClick;
		break;
	case IDM_VIEW_SHOW_DESKTOP:
		emit onHide();
		break;
	case IDM_VIEW_LNK_ARROW:

		break;
	case IDM_SORT_BY_NAME:
		setItemSortRole(Qt::DisplayRole);
		break;
	case IDM_SORT_BY_SIZE:
		setItemSortRole(FileSizeRole);
		break;
	case IDM_SORT_BY_TYPE:
		setItemSortRole(FileTypeRole);
		break;
	case IDM_SORT_BY_TIME:
		setItemSortRole(FileLastModifiedRole);
		break;
	case IDM_SORT_ASCENDING:
		setItemSortOrder(Qt::AscendingOrder);
		break;
	case IDM_SORT_DESCENDING:
		setItemSortOrder(Qt::DescendingOrder);
		break;
	case IDM_REFRESH:
		emit refreshDesktop();
		break;
	case IDM_PASTE:
		// TODO 根据是否有合法复制内容处理下面这两个菜单项的是否启用
		handlePaste();
		break;
	case IDM_PASTE_SHORTCUT:
		handlePasteShortcut();
		break;

	default:
		break;
	}
}

void HzDesktopBkgMenu::hideSortStatus()
{
	m_showSortStatus = false;
}

inline void HzDesktopBkgMenu::handlePaste()
{
	HZQ_D(HzDesktopBkgMenu);

	d->executeActionFromContextMenu(m_param->dirPath, "paste");
}

inline void HzDesktopBkgMenu::handlePasteShortcut()
{
	HZQ_D(HzDesktopBkgMenu);

	d->executeActionFromContextMenu(m_param->dirPath, "pastelink");
}

void HzDesktopBkgMenu::initViewSubMenu()
{
	
}

void HzDesktopBkgMenu::initSortSubMenu()
{
	
}

inline void HzDesktopBkgMenu::setItemSortRole(int role)
{
	m_param->sortRole = role;
	m_showSortStatus = true;
	emit onSetItemSortRole(role);
}

inline void HzDesktopBkgMenu::setItemSortOrder(Qt::SortOrder order)
{
	m_param->sortOrder = order;
	m_showSortStatus = true;
	emit onSetItemSortOrder(order);
}
