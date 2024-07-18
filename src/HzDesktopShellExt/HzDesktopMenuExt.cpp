// HzDesktopMenuExt.cpp: CHzDesktopMenuExt 的实现

#include "pch.h"
#include "HzDesktopMenuExt.h"


// CHzDesktopMenuExt

#include <strsafe.h>

// CHzDesktopMenuExt

STDMETHODIMP CHzDesktopMenuExt::Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject* pdtobj, HKEY hkeyProgID)
{
	FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stg = { TYMED_HGLOBAL };
	HDROP      hDrop;

	//空白处右键，pdtobj是空指针
	if (pdtobj == nullptr)
	{
		return E_INVALIDARG;
	}

	// 在数据对象内查找CF_HDROP类型数据。
	// 如果没有数据，返回一个错误（“无效参数”）给Explorer。
	if (FAILED(pdtobj->GetData(&fmt, &stg)))
	{
		return E_INVALIDARG;
	}

	// 取得指向实际数据的指针。
	hDrop = (HDROP)GlobalLock(stg.hGlobal);

	// 确保非NULL
	if (NULL == hDrop)
	{
		return E_INVALIDARG;
	}

	//选中文件个数
	UINT uNumFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

	if (0 == uNumFiles)
	{
		GlobalUnlock(stg.hGlobal);
		ReleaseStgMedium(&stg);
		return E_INVALIDARG;
	}

	// 获取选中的文件
	for (UINT index = 0; index < uNumFiles; index++)
	{
		WCHAR szFile[MAX_PATH + 1];
		if (0 == DragQueryFile(hDrop, index, szFile, MAX_PATH))
			continue;

		m_filepaths.push_back(szFile);
	}

	GlobalUnlock(stg.hGlobal);
	ReleaseStgMedium(&stg);
	return S_OK;
}



STDMETHODIMP CHzDesktopMenuExt::QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
	// 如果标识包含了 CMF_DEFAULTONLY，那么，什么都不做
	if (uFlags & CMF_DEFAULTONLY || m_filepaths.size() <= 0)
	{
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
	}

	UINT idCmd = idCmdFirst;
	UINT uIndexMenu = indexMenu;

	InsertMenu(hmenu, uIndexMenu++, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);//分割线

	// sub menus
	HMENU submenu = ::CreatePopupMenu();
	UINT uIndexSubMenu = 0;

	MENUITEMINFO submif{};
	submif.cbSize = sizeof(submif);
	submif.fType = MFT_STRING;
	submif.hbmpItem = NULL;
	submif.fMask = MIIM_STRING | MIIM_ID | MIIM_FTYPE;

	if (m_filepaths.size() == 1)
	{
		submif.wID = idCmd + CMD_SHOW_FILEPATH;
		submif.dwTypeData = L"示例-文件路径";
		submif.cch = wcslen(submif.dwTypeData);
	}
	else
	{
		submif.wID = idCmd + CMD_SHOW_FILEPATHS;
		submif.dwTypeData = L"示例-多文件路径";
		submif.cch = wcslen(submif.dwTypeData);
	}
	InsertMenuItem(submenu, uIndexSubMenu++, TRUE, &submif);

	// secord sub menuitem
	MENUITEMINFO secordmif{};
	secordmif.cbSize = sizeof(secordmif);
	secordmif.fType = MFT_STRING;
	secordmif.hbmpItem = NULL;
	secordmif.fMask = MIIM_STRING | MIIM_ID | MIIM_FTYPE;
	secordmif.wID = idCmd + CMD_SHOW_DEMO_ABOUT;
	secordmif.dwTypeData = L"示例菜单-关于";
	secordmif.cch = wcslen(secordmif.dwTypeData);
	InsertMenuItem(submenu, uIndexSubMenu++, TRUE, &secordmif);

	// root menuitem
	MENUITEMINFO rootmif{};
	rootmif.cbSize = sizeof(rootmif);
	rootmif.fType = MFT_STRING;
	rootmif.wID = idCmd + CMD_ROOT;
	rootmif.dwTypeData = L"HZ菜单扩展";
	rootmif.hbmpItem = NULL;
	rootmif.fMask = MIIM_SUBMENU | MIIM_STRING | MIIM_ID | MIIM_FTYPE | MIIM_DATA;
	rootmif.hSubMenu = submenu;
	InsertMenuItem(hmenu, uIndexMenu++, TRUE, &rootmif);

	InsertMenu(hmenu, uIndexMenu++, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);//分割线 
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