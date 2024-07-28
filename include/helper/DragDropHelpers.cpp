#include <shlwapi.h>
#include <shlobj.h>

#include "DragDropHelpers.h"

HRESULT CreateItemFromObject(IUnknown* punk, REFIID riid, void** ppv)
{
    *ppv = NULL;

    PIDLIST_ABSOLUTE pidl;
    HRESULT hr = SHGetIDListFromObject(punk, &pidl);
    if (SUCCEEDED(hr))
    {
        hr = SHCreateItemFromIDList(pidl, riid, ppv);
        ILFree(pidl);
    }
    else
    {
        // perhaps the input is from IE and if so we can construct an item from the URL
        IDataObject* pdo;
        hr = punk->QueryInterface(IID_PPV_ARGS(&pdo));
        if (SUCCEEDED(hr))
        {
            static CLIPFORMAT g_cfURL = 0;

            FORMATETC fmte = { GetClipboardFormat(&g_cfURL, CFSTR_SHELLURL), NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
            STGMEDIUM medium;
            hr = pdo->GetData(&fmte, &medium);
            if (SUCCEEDED(hr))
            {
                PCSTR pszURL = (PCSTR)GlobalLock(medium.hGlobal);
                if (pszURL)
                {
                    WCHAR szURL[2048];
                    SHAnsiToUnicode(pszURL, szURL, ARRAYSIZE(szURL));
                    hr = SHCreateItemFromParsingName(szURL, NULL, riid, ppv);
                    GlobalUnlock(medium.hGlobal);
                }
                ReleaseStgMedium(&medium);
            }
            pdo->Release();
        }
    }

    return hr;
}

HRESULT HzDragDropWindow::OnDragInWindow(IDataObject* dataObject, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect)
{
	DropTargetInfo targetInfo = getCurrentDropTarget();
	if (m_previousInterface && m_previousInterface->targetInfo != targetInfo
		&& m_previousInterface->pDropTarget && m_previousInterface->bInitialised) {
		m_previousInterface->pDropTarget->DragLeave();
		m_previousInterface.reset();
	}

	DropTargetInterface dropTargetInterface = getDropTargetInterface(targetInfo);
    m_previousInterface = dropTargetInterface;

    if (!dropTargetInterface.pDropTarget) {
        return S_FALSE;
    }

	if (dropTargetInterface.bInitialised) {
        dropTargetInterface.pDropTarget->DragOver(grfKeyState, ptl, pdwEffect);
	}
    else {
        dropTargetInterface.pDropTarget->DragEnter(dataObject, grfKeyState, ptl, pdwEffect);
    }

    dropTargetInterface.bInitialised = true;

    return S_OK;
}

DropTargetInterface HzDragDropWindow::getDropTargetInterface(const DropTargetInfo& targetInfo)
{
    if (m_previousInterface && m_previousInterface->targetInfo == targetInfo) {
        return *m_previousInterface;
    }

    wil::com_ptr_nothrow<IDropTarget> pDropTarget;
	if (targetInfo.type == FileOrFolder) {
		pDropTarget = GetDropTargetForPath(targetInfo.info.toString().toStdWString());
	}

    return { targetInfo, pDropTarget, false };
}

wil::com_ptr_nothrow<IDropTarget> HzDragDropWindow::GetDropTargetForPath(const std::wstring& path)
{
    wil::com_ptr_nothrow<IDropTarget> pDropTarget;

    do
    {
        PIDLIST_ABSOLUTE pidl = nullptr;
        HRESULT hr = SHParseDisplayName(path.c_str(), nullptr, &pidl, 0, nullptr);
        if (FAILED(hr)) {
            break;
        }

        wil::com_ptr_nothrow<IShellFolder> parent;
        PCITEMID_CHILD child;
        hr = SHBindToParent(pidl, IID_PPV_ARGS(&parent), &child);
        if (FAILED(hr)) {
            break;
        }

        hr = parent->GetUIObjectOf(NULL, 1, &child, __uuidof(*pDropTarget),
            nullptr, IID_PPV_ARGS_Helper(&pDropTarget));
        if (FAILED(hr)) {
            break;
        }

    } while (false);

    return pDropTarget;
}
