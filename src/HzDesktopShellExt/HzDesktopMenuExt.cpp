// HzDesktopMenuExt.cpp: CHzDesktopMenuExt 的实现

#include <strsafe.h>

#include "pch.h"
#include "resource.h"
#include "HzDesktopMenuExt.h"
#include "common/CommonTools.h"
#include "common/MenuHelper.h"
#include "common/ResourceHelper.h"

extern HINSTANCE g_hInsDll;

// CHzDesktopMenuExt

STDMETHODIMP CHzDesktopMenuExt::Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject* pdtobj, HKEY hkeyProgID)
{
	// TODO 判断是否为顶层桌面，注意不能是桌面文件夹

	if (HZ::isHzDesktopRunning()) {
		return E_INVALIDARG;
	}

	return S_OK;
}

STDMETHODIMP CHzDesktopMenuExt::QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
	// 如果标识包含了 CMF_DEFAULTONLY，那么，什么都不做。 
	// TODO 为什么？
	if (uFlags & CMF_DEFAULTONLY) {
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
	}

	UINT idCmd = idCmdFirst;
	UINT uIndexMenu = indexMenu;
	MenuHelper helper(g_hInsDll);

	helper.insertSeparator(hmenu, uIndexMenu++);
	helper.insertMenuItem(hmenu, IDM_LAUNCH_HZDESKTOP, uIndexMenu++);
	helper.insertSeparator(hmenu, uIndexMenu++);

	return MAKE_HRESULT(SEVERITY_SUCCESS, 0, static_cast <USHORT>(IDM_LAUNCH_HZDESKTOP + 1));
}



STDMETHODIMP CHzDesktopMenuExt::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi)
{
	UINT cmd = LOWORD(lpcmi->lpVerb);
	switch (cmd)
	{
	case IDM_LAUNCH_HZDESKTOP:
		MessageBoxW(lpcmi->hwnd,
			L"xxx，启动！",
			L"Demo Context Menu",
			MB_OK | MB_ICONINFORMATION);
		break;
	default:
		return E_INVALIDARG;
	}

	return S_OK;
}


STDMETHODIMP CHzDesktopMenuExt::GetCommandString(UINT_PTR  idCommand,
	UINT uFlags,
	UINT* pReserved,
	PSTR pszName,
	UINT cchName)
{
	HRESULT hr = E_INVALIDARG;

	if (uFlags & GCS_HELPTEXTW)
	{
		// Only useful for pre-Vista versions of Windows that 
		// have a Status bar.
		//hr = StringCchCopyW(reinterpret_cast<PWSTR>(pszName), cchName, L"A Demo Context Menu");
	}
	else if (uFlags & GCS_HELPTEXT)
	{
		//hr = StringCchCopyA(pszName, cchName, "A Demo Context Menu");
	}
	return hr;
}
