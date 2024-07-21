#pragma once

#include <wil/resource.h>
#include <shobjidl_core.h>

class MenuHelper
{
public:
	MenuHelper(HINSTANCE resInstance);

	void appendMenuItem(HMENU menu, UINT id);

	void insertMenuItem(HMENU menu, UINT id, UINT item);

	void appendSeparator(HMENU menu);

	void insertSeparator(HMENU menu, UINT item);

	void addSubMenuItem(HMENU menu, UINT id, HMENU subMenu);

	void insertSubMenuItem(HMENU menu, UINT id, HMENU subMenu, UINT item);

	void CheckItem(HMENU hMenu, UINT itemID, bool bCheck);

private:

	HINSTANCE m_resInstance;
};