#include <Windows.h>
#include <shellapi.h>
#include <wil/com.h>

#include "resource.h"
#include "MenuHelper.h"
#include "common/ResourceHelper.h"

static HMODULE s_resInstance = GetModuleHandle(nullptr);

namespace HZ
{
	void appendMenuItem(HMENU menu, UINT id)
	{
		insertMenuItem(menu, id, GetMenuItemCount(menu));
	}

	void insertMenuItem(HMENU menu, UINT id, UINT item)
	{
		std::wstring text = ResourceHelper::LoadStringFromRC(s_resInstance, id);
		UINT fMask = MIIM_ID | MIIM_STRING;
		//if (hBitmap) { 获取成功就加此标志位
		//	fMask |= MIIM_BITMAP;
		//}

		MENUITEMINFO menuItemInfo = {};
		menuItemInfo.cbSize = sizeof(menuItemInfo);
		menuItemInfo.fMask = fMask;
		menuItemInfo.wID = id;
		//menuItemInfo.hbmpItem = hBitmap;
		menuItemInfo.dwTypeData = const_cast<LPWSTR>(text.c_str());

		InsertMenuItem(menu, item, TRUE, &menuItemInfo);
	}

	void appendSeparator(HMENU menu)
	{
		insertSeparator(menu, GetMenuItemCount(menu));
	}

	void insertSeparator(HMENU menu, UINT item)
	{
		MENUITEMINFO menuItemInfo = {};
		menuItemInfo.cbSize = sizeof(menuItemInfo);
		menuItemInfo.fMask = MIIM_FTYPE;
		menuItemInfo.fType = MFT_SEPARATOR;
		InsertMenuItem(menu, item, TRUE, &menuItemInfo);
	}


	void addSubMenuItem(HMENU menu, UINT id, wil::unique_hmenu subMenu)
	{
		insertSubMenuItem(menu, id, std::move(subMenu), GetMenuItemCount(menu));
	}

	void insertSubMenuItem(HMENU menu, UINT id, wil::unique_hmenu subMenu, UINT item)
	{
		std::wstring text = ResourceHelper::LoadStringFromRC(s_resInstance, id);

		MENUITEMINFO menuItemInfo = {};
		menuItemInfo.cbSize = sizeof(menuItemInfo);
		menuItemInfo.fMask = MIIM_ID | MIIM_STRING | MIIM_SUBMENU;
		menuItemInfo.wID = id;
		menuItemInfo.dwTypeData = const_cast<LPWSTR>(text.c_str());
		menuItemInfo.hSubMenu = subMenu.release();
		InsertMenuItem(menu, item, TRUE, &menuItemInfo);
	}

	void CheckItem(HMENU hMenu, UINT itemID, bool bCheck)
	{
		UINT state = bCheck ? MF_CHECKED : MF_UNCHECKED;
		CheckMenuItem(hMenu, itemID, state);
	}

}