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
	// TODO 判断是否安装了HzDesktop，安装了才返回OK，否则返回E_INVALIDARG

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
	bool bAlreadyRunning = HZ::isHzDesktopRunning();
	MenuHelper helper(g_hInsDll);

	// TODO 下面这些宏值调整到7w+
	helper.insertSeparator(hmenu, uIndexMenu++);
	if (!bAlreadyRunning) {
		// 启动HZ桌面整理
		std::wstring text = ResourceHelper::LoadStringFromRC(g_hInsDll, IDM_LAUNCH_HZDESKTOP);
		helper.insertMenuItem(hmenu, IDM_LAUNCH_HZDESKTOP, uIndexMenu++);
	}
	else {
		// 一键桌面整理
		// 整理功能 - 新建桌面格子 | 新建文件夹映射 | 设置中心
		// 退出桌面整理
		helper.insertMenuItem(hmenu, IDM_ONECLICK_MANAGE, uIndexMenu++);

		helper.insertMenuItem(hmenu, IDM_EXIT_HZDESKTOP, uIndexMenu++);
	}
	helper.insertSeparator(hmenu, uIndexMenu++);

	// root menuitem
	//MENUITEMINFO rootmif{};
	//rootmif.cbSize = sizeof(rootmif);
	//rootmif.fType = MFT_STRING;
	//rootmif.wID = idCmd + CMD_ROOT;
	//rootmif.dwTypeData = L"HZ菜单扩展111";
	//rootmif.hbmpItem = NULL;
	//rootmif.fMask = MIIM_STRING | MIIM_ID | MIIM_FTYPE | MIIM_DATA;
	//InsertMenuItem(hmenu, uIndexMenu++, TRUE, &rootmif);

	//TweakMenu(hmenu);

	//return MAKE_HRESULT(SEVERITY_SUCCESS, 0, static_cast <USHORT>(CMD_ROOT + 1));

	//TweakMenu(hmenu);

	return MAKE_HRESULT(SEVERITY_SUCCESS, 0, static_cast <USHORT>(IDS_MAX_VALUE + 1));
}



STDMETHODIMP CHzDesktopMenuExt::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi)
{
	if (m_filepaths.size() <= 0)     return E_INVALIDARG;

	BOOL fEx = FALSE;
	BOOL fUnicode = FALSE;

	if (lpcmi->cbSize == sizeof(CMINVOKECOMMANDINFOEX))
	{
		fEx = TRUE;
		if ((lpcmi->fMask & CMIC_MASK_UNICODE))
		{
			fUnicode = TRUE;
		}
	}

	UINT cmd = LOWORD(lpcmi->lpVerb);
	switch (cmd)
	{
	case CMD_SHOW_FILEPATH:
	{
		std::wstring text = L" 选中一个文件:  " + m_filepaths[0];
		MessageBox(lpcmi->hwnd,
			text.c_str(),
			L"Demo Context Menu",
			MB_OK | MB_ICONINFORMATION);
		break;
	}
	case CMD_SHOW_FILEPATHS:
	{
		std::wstring text = L" 选中多个文件:  \n";
		for (int i = 0; i < m_filepaths.size(); i++)
		{
			text += m_filepaths[i] + L"\n";
		}
		MessageBox(lpcmi->hwnd,
			text.c_str(),
			L"Demo Context Menu",
			MB_OK | MB_ICONINFORMATION);
		break;
	}
	case CMD_SHOW_DEMO_ABOUT:
	{
		MessageBox(lpcmi->hwnd,
			L" A demo of context menu! \n I'm ohh!",
			L"Demo Context Menu",
			MB_OK | MB_ICONINFORMATION);
		break;
	}
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

void CHzDesktopMenuExt::TweakMenu(HMENU hMenu)
{
	MENUINFO MenuInfo = {};
	MenuInfo.cbSize = sizeof(MenuInfo);
	MenuInfo.fMask = MIM_STYLE | MIM_APPLYTOSUBMENUS;
	MenuInfo.dwStyle = MNS_CHECKORBMP;
	SetMenuInfo(hMenu, &MenuInfo);
}