// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#include <QVariant>
#include <ShlObj_core.h>
#include <strsafe.h>
#include <commoncontrols.h>
#include <wil/com.h>

enum DropTargetType {
    FileOrFolder = 0,
    GridView
};

struct DropTargetInfo {
    DropTargetType type;
    QVariant info;

    bool operator==(const DropTargetInfo& other) const {
        return type == other.type && info == other.info;
    }
};

struct DropTargetInterface {
    DropTargetInfo targetInfo;
	wil::com_ptr_nothrow<IDropTarget> pDropTarget;
	bool bInitialised;
};

// declare a static CLIPFORMAT and pass that that by ref as the first param

__inline CLIPFORMAT GetClipboardFormat(CLIPFORMAT *pcf, PCWSTR pszForamt)
{
    if (*pcf == 0)
    {
        *pcf = (CLIPFORMAT)RegisterClipboardFormat(pszForamt);
    }
    return *pcf;
}

__inline HRESULT SetBlob(IDataObject *pdtobj, CLIPFORMAT cf, const void *pvBlob, UINT cbBlob)
{
    void *pv = GlobalAlloc(GPTR, cbBlob);
    HRESULT hr = pv ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hr))
    {
        CopyMemory(pv, pvBlob, cbBlob);

        FORMATETC fmte = {cf, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};

        // The STGMEDIUM structure is used to define how to handle a global memory transfer.
        // This structure includes a flag, tymed, which indicates the medium
        // to be used, and a union comprising pointers and a handle for getting whichever
        // medium is specified in tymed.
        STGMEDIUM medium = {};
        medium.tymed = TYMED_HGLOBAL;
        medium.hGlobal = pv;

        hr = pdtobj->SetData(&fmte, &medium, TRUE);
        if (FAILED(hr))
        {
            GlobalFree(pv);
        }
    }
    return hr;
}

__inline void SetDropTip(IDataObject *pdtobj, DROPIMAGETYPE type, PCWSTR pszMsg, PCWSTR pszInsert)
{
    DROPDESCRIPTION dd = { type };
    StringCchCopyW(dd.szMessage, ARRAYSIZE(dd.szMessage), pszMsg);
    StringCchCopyW(dd.szInsert, ARRAYSIZE(dd.szInsert), pszInsert ? pszInsert : L"");

    static CLIPFORMAT s_cfDropDescription = 0;
    SetBlob(pdtobj, GetClipboardFormat(&s_cfDropDescription, CFSTR_DROPDESCRIPTION), &dd, sizeof(dd));
}

__inline void ClearDropTip(IDataObject *pdtobj)
{
    if (pdtobj) {
        SetDropTip(pdtobj, DROPIMAGE_INVALID, L"", NULL);
    }
}

// helper to convert a data object with HIDA format or folder into a shell item
// note: if the data object contains more than one item this function will fail
// if you want to operate on the full selection use SHCreateShellItemArrayFromDataObject

HRESULT CreateItemFromObject(IUnknown* punk, REFIID riid, void** ppv);

// encapsulation of the shell drag drop presentation and Drop handling functionality
// this provides the following features 1) drag images, 2) drop tips,
// 3) ints OLE and registers drop target, 4) conversion of the data object item into shell items
//
// to use this
//      1) have the object that represents the main window of your app derive
//         from HzDragDropHelper exposing it as public.
//         "class CAppClass : public HzDragDropHelper"
//      2) add IDropTarget to the QueryInterface() implementation of your class
//         that is a COM object itself
//      3) in your WM_INITDIALOG handling call
//         InitializeDragDropHelper(_hdlg, DROPIMAGE_LINK, NULL) passing
//         the dialog window and drop tip template and type
//      4) in your WM_DESTROY handler add a call to TerminateDragDropHelper(). note not
//         doing this will lead to a leak of your object so this is important
//      5) add the delaration and implementation of OnDrop() to your class, this
//         gets called when the drop happens

class HzDragDropInterface 
    : public IDropTarget
{
public:
    HzDragDropInterface(const std::wstring& dirpath) 
        : _pdth(NULL)
        , m_pdtobj(NULL)
        , _hwndRegistered(NULL)
        , _hrOleInit(OleInitialize(0))
    {
        CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_INPROC, IID_PPV_ARGS(&_pdth));
    }

    ~HzDragDropInterface()
    {
        if (_pdth) {
            _pdth->Release();
        }
        if (SUCCEEDED(_hrOleInit)) {
            OleUninitialize();
        }
    }

	void InitializeDragDropHelper(HWND hwnd)
	{
		if (SUCCEEDED(RegisterDragDrop(hwnd, this))) {
            _hwndRegistered = hwnd;
		}
	}

    HRESULT GetDragDropHelper(REFIID riid, void **ppv)
    {
        *ppv = NULL;
        return _pdth ? _pdth->QueryInterface(riid, ppv) : E_NOINTERFACE;
    }

    // direct access to the data object, if you don't want to use the shell item array
    IDataObject *GetDataObject()
    {
        return m_pdtobj;
    }

    // IDropTarget
    IFACEMETHODIMP DragEnter(IDataObject* pdtobj, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect) override;

    IFACEMETHODIMP DragOver(DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect) override;

    IFACEMETHODIMP DragLeave() override;

    IFACEMETHODIMP Drop(IDataObject* pdtobj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override;

    HRESULT OnDragInWindow(IDataObject* dataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);

    DropTargetInterface getDropTargetInterface(const DropTargetInfo& targetInfo);

    wil::com_ptr_nothrow<IDropTarget> GetDropTargetForPath(const std::wstring& path);

    void resetDropState();

protected:
    // client provides
	virtual DropTargetInfo getCurrentDropTarget() = 0;

    virtual bool filterDragDrop(const DropTargetInfo& targetInfo, bool bIsDrop) = 0;

    IDropTargetHelper *_pdth;
    HRESULT _hrOleInit;

    HWND _hwndRegistered;
    IDataObject* m_pdtobj;
    // TODO 详细了解std::optional用法
    std::optional<DropTargetInterface> m_previousInterface;
    DropTargetInfo m_draggedTargetInfo;
};
