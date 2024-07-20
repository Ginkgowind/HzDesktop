#include <Windows.h>
#include <shellapi.h>
#include <wil/com.h>

#include <QUrl>
#include <QProcess>
#include <QSettings>
#include <QDesktopServices>

#include "resource.h"
#include "HzItemMenu_p.h"
#include "windows/UiOperation.h"
#include "windows/tools.h"
#include "common/ResourceHelper.h"
#include "common/MenuHelper.h"

std::vector<PITEMID_CHILD> HzItemMenuPrivate::getDesktopPidcFromPaths(const QStringList& paths)
{
	std::vector<PITEMID_CHILD> pidlItems;

	do
	{
		if (paths.empty()) {
			break;
		}

		wil::com_ptr<IShellFolder> pShellFolder;
		HRESULT hr = SHGetDesktopFolder(&pShellFolder);
		if (FAILED(hr)) {
			break;
		}

		wil::com_ptr<IEnumIDList> pEnumIDList;
		hr = pShellFolder->EnumObjects(NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &pEnumIDList);
		if (FAILED(hr)) {
			break;
		}

		ULONG uFetched = 1;
		wil::unique_cotaskmem_ptr<ITEMID_CHILD> pidlItem;

		QStringList tmpPaths;
		for (QString path : paths) {
			tmpPaths.push_back(path.replace('/', '\\'));
		}
		while (pEnumIDList->Next(1, wil::out_param(pidlItem), &uFetched) == S_OK
			&& (uFetched == 1))
		{
			STRRET str;
			hr = pShellFolder->GetDisplayNameOf(pidlItem.get(), SHGDN_FORPARSING, &str);

			TCHAR szFilePath[MAX_PATH];
			StrRetToBufW(&str, pidlItem.get(), szFilePath, MAX_PATH);

			int index = tmpPaths.indexOf(QString::fromStdWString(szFilePath));
			if (index >= 0) {
				pidlItems.push_back(ILCloneChild(pidlItem.get()));
				tmpPaths.removeAt(index);

				if (tmpPaths.empty()) {
					break;
				}
			}
		}
	} while (false);

	return pidlItems;
}

void HzItemMenuPrivate::executeActionFromContextMenu(const QStringList& pathList, const std::string& action)
{
	std::vector<LPITEMIDLIST> idChildvec;

	do
	{
		if (pathList.empty()) {
			break;
		}

		idChildvec = getDesktopPidcFromPaths(pathList);
		if (idChildvec.empty()) {
			break;
		}

		wil::com_ptr<IShellFolder> pDesktop;
		HRESULT hr = SHGetDesktopFolder(&pDesktop);
		if (FAILED(hr)) {
			break;
		}

		wil::com_ptr<IContextMenu> pContextMenu;
		hr = pDesktop->GetUIObjectOf(nullptr, (UINT)idChildvec.size(),
			const_cast<LPCITEMIDLIST*>(idChildvec.data()),
			IID_IContextMenu, nullptr, (void**)&pContextMenu);
		if (FAILED(hr)) {
			break;
		}

		CMINVOKECOMMANDINFO commandInfo;
		commandInfo.cbSize = sizeof(CMINVOKECOMMANDINFO);
		commandInfo.fMask = 0;
		commandInfo.hwnd = NULL;
		// TODO这里应该是要资源释放的，注意别的地方都是
		commandInfo.lpVerb = action.c_str();
		commandInfo.lpParameters = nullptr;
		commandInfo.lpDirectory = nullptr;
		commandInfo.nShow = SW_SHOWNORMAL;
		hr = pContextMenu->InvokeCommand(&commandInfo);
		if (FAILED(hr)) {
			break;
		}
	} while (false);

	for (auto& pidl : idChildvec) {
		ILFree(pidl);
	}
}

// TODO 处理private部分的资源释放
HzDesktopBkgMenuPrivate::HzDesktopBkgMenuPrivate()
{

}

HzDesktopBkgMenuPrivate::~HzDesktopBkgMenuPrivate()
{}

void HzDesktopBkgMenuPrivate::updateMenu(HMENU menu)
{
	UINT position = 0;

	// TODO 这里是不是不需要传智能指针，直接返回句柄就可以了，反正也不报错还省得move
	auto viewMenu = buildViewMenu();
	HZ::insertSubMenuItem(menu, IDS_VIEW_BKG_MENU, std::move(viewMenu), position++);

	auto sortMenu = buildSortMenu();
	HZ::insertSubMenuItem(menu, IDS_SORT_BKG_MENU, std::move(sortMenu), position++);

	HZ::insertMenuItem(menu, IDM_REFRESH, position++);
	HZ::insertSeparator(menu, position++);

	HZ::insertMenuItem(menu, IDM_PASTE, position++);
	HZ::insertMenuItem(menu, IDM_PASTE_SHORTCUT, position++);
	HZ::insertSeparator(menu, position++);
}

wil::unique_hmenu HzDesktopBkgMenuPrivate::buildViewMenu()
{
	HZQ_Q(HzDesktopBkgMenu);

	HMENU viewMenu = CreatePopupMenu();

	HZ::appendMenuItem(viewMenu, IDM_VIEW_LARGE_ICON);
	HZ::appendMenuItem(viewMenu, IDM_VIEW_MEDIUM_ICON);
	HZ::appendMenuItem(viewMenu, IDM_VIEW_SMALL_ICON);
	HZ::appendSeparator(viewMenu);
	HZ::appendMenuItem(viewMenu, IDM_VIEW_AUTO_ARRANGE);
	HZ::appendSeparator(viewMenu);
	HZ::appendMenuItem(viewMenu, IDM_VIEW_DOUBLE_CLICK);
	HZ::appendMenuItem(viewMenu, IDM_VIEW_SHOW_DESKTOP);
	HZ::appendSeparator(viewMenu);
	HZ::appendMenuItem(viewMenu, IDM_VIEW_LNK_ARROW);

	HZ::CheckItem(viewMenu, IDM_VIEW_LARGE_ICON, q->m_param->iconMode == LargeIcon);
	HZ::CheckItem(viewMenu, IDM_VIEW_MEDIUM_ICON, q->m_param->iconMode == MediumIcon);
	HZ::CheckItem(viewMenu, IDM_VIEW_SMALL_ICON, q->m_param->iconMode == SmallIcon);
	HZ::CheckItem(viewMenu, IDM_VIEW_AUTO_ARRANGE, q->m_param->bAutoArrange);
	HZ::CheckItem(viewMenu, IDM_VIEW_DOUBLE_CLICK, q->m_param->bEnableDoubleClick);
	HZ::CheckItem(viewMenu, IDM_VIEW_SHOW_DESKTOP, true);
	HZ::CheckItem(viewMenu, IDM_VIEW_LNK_ARROW, q->m_param->bShowLnkArrow);

	return wil::unique_hmenu(viewMenu);
}

wil::unique_hmenu HzDesktopBkgMenuPrivate::buildSortMenu()
{
	HZQ_Q(HzDesktopBkgMenu);

	HMENU sortMenu = CreatePopupMenu();

	HZ::appendMenuItem(sortMenu, IDM_SORT_BY_NAME);
	HZ::appendMenuItem(sortMenu, IDM_SORT_BY_SIZE);
	HZ::appendMenuItem(sortMenu, IDM_SORT_BY_TYPE);
	HZ::appendMenuItem(sortMenu, IDM_SORT_BY_TIME);
	HZ::appendSeparator(sortMenu);
	HZ::appendMenuItem(sortMenu, IDM_SORT_ASCENDING);
	HZ::appendMenuItem(sortMenu, IDM_SORT_DESCENDING);

	if (q->m_showSortStatus) {
		HZ::CheckItem(sortMenu, IDM_SORT_BY_NAME, q->m_param->sortRole == Qt::DisplayRole);
		HZ::CheckItem(sortMenu, IDM_SORT_BY_SIZE, q->m_param->sortRole == FileSizeRole);
		HZ::CheckItem(sortMenu, IDM_SORT_BY_TYPE, q->m_param->sortRole == FileTypeRole);
		HZ::CheckItem(sortMenu, IDM_SORT_BY_TIME, q->m_param->sortRole == FileLastModifiedRole);

		HZ::CheckItem(sortMenu, IDM_SORT_ASCENDING, q->m_param->sortOrder == Qt::AscendingOrder);
		HZ::CheckItem(sortMenu, IDM_SORT_DESCENDING, q->m_param->sortOrder == Qt::DescendingOrder);
	}

	return wil::unique_hmenu(sortMenu);
}

QVector<UINT> HzDesktopBkgMenuPrivate::getNewFileCmdsVec(HMENU hMenu)
{
	HZQ_Q(HzDesktopBkgMenu);

	QVector<UINT> cmds;

	HMENU hNewSubMenu = NULL;

	int itemCount = GetMenuItemCount(hMenu);
	for (int i = 0; i < itemCount; ++i) {
		MENUITEMINFO mii;
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_ID | MIIM_SUBMENU; // 设置需要获取的信息
		mii.dwTypeData = NULL; // 不需要获取菜单项文本

		if (!GetMenuItemInfoW(hMenu, i, TRUE, &mii) || mii.wID < MIN_SHELL_MENU_ID || MAX_SHELL_MENU_ID < mii.wID) {
			continue;
		}

		CHAR szCommandString[MAX_PATH];
		HRESULT hr = q->m_contextMenu->GetCommandString(mii.wID - MIN_SHELL_MENU_ID, GCS_VERBA, NULL, szCommandString, sizeof(szCommandString));
		if (SUCCEEDED(hr)) {
			if (strcmp(szCommandString, "New") == 0) {
				hNewSubMenu = mii.hSubMenu;
				break;
			}
		}
		else {
			CHAR szVerb[MAX_PATH];
			int nResult = GetMenuStringA(hMenu, mii.wID, szVerb, MAX_PATH, MF_BYCOMMAND);
			if (nResult > 0 && strcmp(szVerb, "Ne&w") == 0) {
				hNewSubMenu = mii.hSubMenu;
				break;
			}
		}
	}

	if (!hNewSubMenu) {
		return cmds;
	}

	// 再遍历New子菜单
	itemCount = GetMenuItemCount(hNewSubMenu);
	for (int i = 0; i < itemCount; ++i) {
		MENUITEMINFO mii;
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_ID; // 设置需要获取的信息
		mii.dwTypeData = NULL; // 不需要获取菜单项文本

		if (!GetMenuItemInfoW(hNewSubMenu, i, TRUE, &mii) || mii.wID < MIN_SHELL_MENU_ID || MAX_SHELL_MENU_ID < mii.wID) {
			continue;
		}

		CHAR szCommandString[MAX_PATH];
		if (SUCCEEDED(q->m_contextMenu->GetCommandString(mii.wID - MIN_SHELL_MENU_ID, GCS_VERBA, NULL, szCommandString, sizeof(szCommandString)))) {
			if (strcmp(szCommandString, "NewLink") == 0) {
				continue;
			}
		}

		cmds.push_back(mii.wID);
	}

	return cmds;
}

void HzDesktopBkgMenuPrivate::executeActionFromContextMenu(const QString& path, const std::string& action)
{
	LPITEMIDLIST idl = nullptr;

	do
	{
		wil::com_ptr<IShellFolder> pDesktop;
		HRESULT hr = SHGetDesktopFolder(&pDesktop);
		if (FAILED(hr)) {
			break;
		}

		hr = SHParseDisplayName(path.toStdWString().c_str(), nullptr, &idl, 0, nullptr);
		if (FAILED(hr)) {
			break;
		}

		wil::com_ptr<IShellFolder> pShellFolder;
		hr = pDesktop->BindToObject(idl, nullptr, IID_PPV_ARGS(&pShellFolder));
		if (FAILED(hr)) {
			break;
		}

		wil::com_ptr<IContextMenu> pContextMenu;
		hr = pShellFolder->CreateViewObject(nullptr, IID_PPV_ARGS(&pContextMenu));
		if (FAILED(hr)) {
			break;
		}

		CMINVOKECOMMANDINFO commandInfo;
		commandInfo.cbSize = sizeof(CMINVOKECOMMANDINFO);
		commandInfo.fMask = 0;
		commandInfo.hwnd = NULL;
		// TODO这里应该是要资源释放的，注意别的地方都是
		commandInfo.lpVerb = action.c_str();
		commandInfo.lpParameters = nullptr;
		commandInfo.lpDirectory = nullptr;
		commandInfo.nShow = SW_SHOWNORMAL;
		hr = pContextMenu->InvokeCommand(&commandInfo);
		if (FAILED(hr)) {
			break;
		}
	} while (false);

	if (idl) {
		CoTaskMemFree(idl);
	}
}