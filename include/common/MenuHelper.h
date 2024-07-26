#pragma once

#include <shobjidl_core.h>

#include "windows/IconBitmapUtils.h"

class MenuHelper
{
public:
	MenuHelper(HINSTANCE resInstance);

	void appendMenuItem(HMENU menu, UINT id, UINT uIcon = 0);

	void insertMenuItem(HMENU menu, UINT item, UINT id, UINT uIcon = 0);

	void insertMenuItem2(HMENU menu, UINT item, UINT id, UINT strId, UINT uIcon = 0);

	void appendSeparator(HMENU menu);

	void insertSeparator(HMENU menu, UINT item);

	void addSubMenuItem(HMENU menu, HMENU subMenu, UINT id, UINT uIcon = 0);

	void insertSubMenuItem(HMENU menu, UINT id, UINT item, HMENU subMenu, UINT uIcon = 0);

	void CheckItem(HMENU hMenu, UINT itemID, bool bCheck);

	void EnableItem(HMENU hMenu, UINT itemID, bool bEnable);

private:

	HINSTANCE m_resInstance;

	IconBitmapUtils m_bitmapUtils;
};