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

static HMODULE s_resInstance = GetModuleHandle(nullptr);

inline void MenuHelper::appendMenuItem(HMENU menu, UINT id)
{
	insertMenuItem(menu, id, GetMenuItemCount(menu));
}

inline void MenuHelper::insertMenuItem(HMENU menu, UINT id, UINT item)
{
	std::string text = ResourceHelper::LoadString(s_resInstance, id);
	UINT fMask = MIIM_ID | MIIM_STRING;
	//if (hBitmap) { 获取成功就加此标志位
	//	fMask |= MIIM_BITMAP;
	//}

	MENUITEMINFO menuItemInfo = {};
	menuItemInfo.cbSize = sizeof(menuItemInfo);
	menuItemInfo.fMask = fMask;
	menuItemInfo.wID = id;
	//menuItemInfo.hbmpItem = hBitmap;
	menuItemInfo.dwTypeData = const_cast<LPSTR>(text.c_str());

	InsertMenuItem(menu, item, TRUE, &menuItemInfo);
}

inline void MenuHelper::appendSeparator(HMENU menu)
{
	insertSeparator(menu, GetMenuItemCount(menu));
}

inline void MenuHelper::insertSeparator(HMENU menu, UINT item)
{
	MENUITEMINFO menuItemInfo = {};
	menuItemInfo.cbSize = sizeof(menuItemInfo);
	menuItemInfo.fMask = MIIM_FTYPE;
	menuItemInfo.fType = MFT_SEPARATOR;
	InsertMenuItem(menu, item, TRUE, &menuItemInfo);
}


inline void MenuHelper::addSubMenuItem(HMENU menu, UINT id, wil::unique_hmenu subMenu)
{
	insertSubMenuItem(menu, id, std::move(subMenu), GetMenuItemCount(menu));
}

inline void MenuHelper::insertSubMenuItem(HMENU menu, UINT id, wil::unique_hmenu subMenu, UINT item)
{
	std::string text = ResourceHelper::LoadString(s_resInstance, id);

	MENUITEMINFO menuItemInfo = {};
	menuItemInfo.cbSize = sizeof(menuItemInfo);
	menuItemInfo.fMask = MIIM_ID | MIIM_STRING | MIIM_SUBMENU;
	menuItemInfo.wID = id;
	menuItemInfo.dwTypeData = const_cast<LPSTR>(text.c_str());
	menuItemInfo.hSubMenu = subMenu.release();
	InsertMenuItem(menu, item, TRUE, &menuItemInfo);
}

// TODO 处理private部分的资源释放
HzDesktopBlankMenuPrivate::HzDesktopBlankMenuPrivate()
{

}

HzDesktopBlankMenuPrivate::~HzDesktopBlankMenuPrivate()
{}

void HzDesktopBlankMenuPrivate::updateMenu(HMENU menu)
{
	UINT position = 0;

	// TODO 为什么用unique
	auto viewMenu = buildViewsMenu();
	MenuHelper::insertSubMenuItem(menu, IDS_VIEW_BKG_MENU, std::move(viewMenu), position++);
	MenuHelper::insertSeparator(menu, position++);
}

LRESULT HzDesktopBlankMenuPrivate::ParentWindowSubclass(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HZQ_Q(HzDesktopBlankMenu);

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

		if (!q->m_contextMenu)
		{
			break;
		}

		if (auto contextMenu3 = q->m_contextMenu.try_query<IContextMenu3>())
		{
			LRESULT result;
			HRESULT hr = contextMenu3->HandleMenuMsg2(msg, wParam, lParam, &result);

			if (SUCCEEDED(hr))
			{
				return result;
			}
		}
		else if (auto contextMenu2 = q->m_contextMenu.try_query<IContextMenu2>())
		{
			contextMenu2->HandleMenuMsg(msg, wParam, lParam);
		}
		break;
	}

	return DefSubclassProc(hwnd, msg, wParam, lParam);
}


wil::unique_hmenu HzDesktopBlankMenuPrivate::buildViewsMenu()
{
	HMENU viewMenu = CreatePopupMenu();

	MenuHelper::appendMenuItem(viewMenu, IDM_VIEW_LARGE_ICON);
	MenuHelper::appendMenuItem(viewMenu, IDM_VIEW_MEDIUM_ICON);
	MenuHelper::appendMenuItem(viewMenu, IDM_VIEW_SMALL_ICON);
	MenuHelper::appendSeparator(viewMenu);
	MenuHelper::appendMenuItem(viewMenu, IDM_VIEW_AUTO_ARRANGE);
	MenuHelper::appendSeparator(viewMenu);
	MenuHelper::appendMenuItem(viewMenu, IDM_VIEW_DOUBLE_CLICK);
	MenuHelper::appendMenuItem(viewMenu, IDM_VIEW_SHOW_DESKTOP);
	MenuHelper::appendSeparator(viewMenu);
	MenuHelper::appendMenuItem(viewMenu, IDM_VIEW_LNK_ARROW);

	return wil::unique_hmenu(viewMenu);
}