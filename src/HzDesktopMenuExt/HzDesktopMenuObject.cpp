// HzDesktopMenuObject.cpp: CHzDesktopMenuObject 的实现

// CHzDesktopMenuObject

#include "pch.h"
#include "resource.h"
#include "HzDesktopMenuObject.h"
#include "common/CommonTools.h"
#include "common/MenuHelper.h"
#include "common/ResourceHelper.h"

#include <strsafe.h>

extern HINSTANCE g_hInsDll;

// CHzDesktopMenuObject

STDMETHODIMP CHzDesktopMenuObject::Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject* pdtobj, HKEY hkeyProgID)
{
    // 判断是否为顶层桌面，注意不能是桌面文件夹
    if (pidlFolder->mkid.cb) {
        return E_INVALIDARG;
    }

    if (HZ::isHzDesktopRunning()) {
        return E_INVALIDARG;
    }

    return S_OK;
}

STDMETHODIMP CHzDesktopMenuObject::QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
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
    helper.insertMenuItem2(hmenu, uIndexMenu++, 
        idCmdFirst + IDM_LAUNCH_HZDESKTOP, IDM_LAUNCH_HZDESKTOP, IDI_ICON_LAUNCH);
    helper.insertSeparator(hmenu, uIndexMenu++);

    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, static_cast <USHORT>(IDM_LAUNCH_HZDESKTOP + 1));
}

STDMETHODIMP CHzDesktopMenuObject::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi)
{
    UINT cmd = LOWORD(lpcmi->lpVerb);

    switch (cmd)
    {
    case IDM_LAUNCH_HZDESKTOP:
        (void)HZ::launchHzDesktop();
        break;
    default:
        return E_INVALIDARG;
    }

    return S_OK;
}

STDMETHODIMP CHzDesktopMenuObject::GetCommandString(UINT_PTR  idCommand,
    UINT uFlags,
    UINT* pReserved,
    PSTR pszName,
    UINT cchName)
{
    HRESULT hr = E_INVALIDARG;

    if (idCommand == IDM_LAUNCH_HZDESKTOP) {
        switch (uFlags)
        {
        case GCS_VERBA:
            hr = StringCchCopyA(pszName, cchName, "HzDesktop");
            break;

        case GCS_VERBW:
            // GCS_VERBW is an optional feature that enables a caller
            // to discover the canonical name for the verb that is passed in
            // through idCommand.
            hr = StringCchCopyW(reinterpret_cast<PWSTR>(pszName), cchName, L"HzDesktop");
            break;
        }
    }

    return hr;
}
