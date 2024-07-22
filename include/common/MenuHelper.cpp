#include <Windows.h>
#include <shellapi.h>

#include "MenuHelper.h"
#include "ResourceHelper.h"

MenuHelper::MenuHelper(HINSTANCE resInstance)
	: m_resInstance(resInstance)
{
}

void MenuHelper::appendMenuItem(HMENU menu, UINT id)
{
	insertMenuItem(menu, id, GetMenuItemCount(menu));
}

void MenuHelper::insertMenuItem(HMENU menu, UINT id, UINT item)
{
	std::wstring text = ResourceHelper::LoadStringFromRC(m_resInstance, id);
	UINT fMask = MIIM_ID | MIIM_STRING;
	//if (hBitmap) { 获取成功就加此标志位
	//	fMask |= MIIM_BITMAP;
	//}

	// 查看是否含有ico资源


	MENUITEMINFO menuItemInfo = {};
	menuItemInfo.cbSize = sizeof(menuItemInfo);
	menuItemInfo.fMask = fMask;
	menuItemInfo.wID = id;
	//menuItemInfo.hbmpItem = hBitmap;
	menuItemInfo.dwTypeData = const_cast<LPWSTR>(text.c_str());

	InsertMenuItem(menu, item, TRUE, &menuItemInfo);
}

void MenuHelper::appendSeparator(HMENU menu)
{
	insertSeparator(menu, GetMenuItemCount(menu));
}

void MenuHelper::insertSeparator(HMENU menu, UINT item)
{
	MENUITEMINFO menuItemInfo = {};
	menuItemInfo.cbSize = sizeof(menuItemInfo);
	menuItemInfo.fMask = MIIM_FTYPE;
	menuItemInfo.fType = MFT_SEPARATOR;
	InsertMenuItem(menu, item, TRUE, &menuItemInfo);
}


void MenuHelper::addSubMenuItem(HMENU menu, UINT id, HMENU subMenu)
{
	insertSubMenuItem(menu, id, subMenu, GetMenuItemCount(menu));
}

void MenuHelper::insertSubMenuItem(HMENU menu, UINT id, HMENU subMenu, UINT item)
{
	std::wstring text = ResourceHelper::LoadStringFromRC(m_resInstance, id);

	MENUITEMINFO menuItemInfo = {};
	menuItemInfo.cbSize = sizeof(menuItemInfo);
	menuItemInfo.fMask = MIIM_ID | MIIM_STRING | MIIM_SUBMENU;
	menuItemInfo.wID = id;
	menuItemInfo.dwTypeData = const_cast<LPWSTR>(text.c_str());
	menuItemInfo.hSubMenu = subMenu;
	InsertMenuItem(menu, item, TRUE, &menuItemInfo);
}

void MenuHelper::CheckItem(HMENU hMenu, UINT itemID, bool bCheck)
{
	UINT state = bCheck ? MF_CHECKED : MF_UNCHECKED;
	CheckMenuItem(hMenu, itemID, state);
}