// HzDesktopMenuExt.cpp: CHzDesktopMenuExt 的实现

#include <strsafe.h>

#include "pch.h"
#include "resource.h"
#include "HzDesktopMenuExt.h"
#include "common/CommonTools.h"
#include "common/MenuHelper.h"

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

	bool bAlreadyRunning = HZ::isHzDesktopRunning();
	UINT idCmd = idCmdFirst;
	UINT uIndexMenu = indexMenu;

	HZ::insertSeparator(hmenu, uIndexMenu++);
	if (!bAlreadyRunning) {
		// 启动HZ桌面整理
		HZ::insertMenuItem(hmenu, IDM_LAUNCH_HZDESKTOP, uIndexMenu++);
	}
	else {
		// 一键桌面整理
		// 整理功能 - 新建桌面格子 | 新建文件夹映射 | 设置中心
		// 退出桌面整理
		HZ::insertMenuItem(hmenu, IDM_ONECLICK_MANAGE, uIndexMenu++);

		HZ::insertMenuItem(hmenu, IDM_EXIT_HZDESKTOP, uIndexMenu++);
	}
	HZ::insertSeparator(hmenu, uIndexMenu++);

	TweakMenu(hmenu);

	return MAKE_HRESULT(SEVERITY_SUCCESS, 0, static_cast <USHORT>(CMD_SHOW_DEMO_ABOUT + 1));
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
		hr = StringCchCopyW(reinterpret_cast<PWSTR>(pszName), cchName, L"A Demo Context Menu");
	}
	else if (uFlags & GCS_HELPTEXT)
	{
		hr = StringCchCopyA(pszName, cchName, "A Demo Context Menu");
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