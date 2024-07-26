#include <Windows.h>
#include <shellapi.h>

#include "MenuHelper.h"
#include "ResourceHelper.h"

MenuHelper::MenuHelper(HINSTANCE resInstance)
	: m_resInstance(resInstance)
{
}

void MenuHelper::appendMenuItem(HMENU menu, UINT id, UINT uIcon)
{
	insertMenuItem(menu, GetMenuItemCount(menu), id, uIcon);
}

void MenuHelper::insertMenuItem(HMENU menu, UINT item, UINT id, UINT uIcon)
{
	MENUITEMINFO menuItemInfo = {};
	menuItemInfo.cbSize = sizeof(menuItemInfo);
	menuItemInfo.fMask = MIIM_ID | MIIM_STRING;
	menuItemInfo.wID = id;

	// 查看是否含有ico资源
	if (uIcon > 0) {
		menuItemInfo.fMask |= MIIM_BITMAP;
		menuItemInfo.hbmpItem = m_bitmapUtils.LoadAndResizeFirstIcon(
			m_resInstance, uIcon, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));
	}

	std::wstring text = ResourceHelper::LoadStringFromRC(m_resInstance, id);
	menuItemInfo.dwTypeData = const_cast<LPWSTR>(text.c_str());

	InsertMenuItem(menu, item, TRUE, &menuItemInfo);
}

void MenuHelper::insertMenuItem2(HMENU menu, UINT item, UINT id, UINT strId, UINT uIcon)
{
	MENUITEMINFO menuItemInfo = {};
	menuItemInfo.cbSize = sizeof(menuItemInfo);
	menuItemInfo.fMask = MIIM_ID | MIIM_STRING;
	menuItemInfo.wID = id;

	// 查看是否含有ico资源
	if (uIcon > 0) {
		menuItemInfo.fMask |= MIIM_BITMAP;
		menuItemInfo.hbmpItem = m_bitmapUtils.LoadAndResizeFirstIcon(
			m_resInstance, uIcon, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));
	}

	std::wstring text = ResourceHelper::LoadStringFromRC(m_resInstance, strId);
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

void MenuHelper::addSubMenuItem(HMENU menu, HMENU subMenu, UINT id, UINT uIcon)
{
	insertSubMenuItem(menu, id, GetMenuItemCount(menu), subMenu, uIcon);
}

void MenuHelper::insertSubMenuItem(HMENU menu, UINT id, UINT item, HMENU subMenu, UINT uIcon)
{
	std::wstring text = ResourceHelper::LoadStringFromRC(m_resInstance, id);

	MENUITEMINFO menuItemInfo = {};
	menuItemInfo.cbSize = sizeof(menuItemInfo);
	menuItemInfo.fMask = MIIM_ID | MIIM_STRING | MIIM_SUBMENU;
	menuItemInfo.wID = id;
	menuItemInfo.dwTypeData = const_cast<LPWSTR>(text.c_str());
	menuItemInfo.hSubMenu = subMenu;

	// 查看是否含有ico资源
	if (uIcon > 0) {
		menuItemInfo.fMask |= MIIM_BITMAP;
		menuItemInfo.hbmpItem = m_bitmapUtils.LoadAndResizeFirstIcon(
			m_resInstance, uIcon, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));
	}

	InsertMenuItem(menu, item, TRUE, &menuItemInfo);
}

void MenuHelper::CheckItem(HMENU hMenu, UINT itemID, bool bCheck)
{
	UINT state = bCheck ? MF_CHECKED : MF_UNCHECKED;
	CheckMenuItem(hMenu, itemID, state);
}

void MenuHelper::EnableItem(HMENU hMenu, UINT itemID, bool bEnable)
{
	UINT state = bEnable ? MF_ENABLED : MF_DISABLED;
	EnableMenuItem(hMenu, itemID, state);
}