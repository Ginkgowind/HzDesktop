#pragma once

#include <wil/resource.h>
#include <shobjidl_core.h>

namespace HZ
{
	void appendMenuItem(HMENU menu, UINT id);

	void insertMenuItem(HMENU menu, UINT id, UINT item);

	void appendSeparator(HMENU menu);

	void insertSeparator(HMENU menu, UINT item);

	void addSubMenuItem(HMENU menu, UINT id, wil::unique_hmenu subMenu);
	
	void insertSubMenuItem(HMENU menu, UINT id, wil::unique_hmenu subMenu, UINT item);

	void CheckItem(HMENU hMenu, UINT itemID, bool bCheck);
};