#pragma once

#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>

#include "HzDrag.h"
#include "helper/DragDropHelpers.h"

// 仅适用于桌面
class HzDragPrivate 
    : public HzDesktopPrivate
    , public HzDragDropHelper
{
	HZQ_DECLARE_PUBLIC(HzDrag)

public:
	HRESULT GetDataObject(IShellItemArray* psiaItems, SHDRAGIMAGE& image, IDataObject** ppdtobj);

    HRESULT CDataObject_CreateInstance(REFIID riid, void** ppv);

    virtual HRESULT OnDrop(IShellItemArray* psia, DWORD /* grfKeyState */)
    {
        // TODO 
        return S_OK;
        //HRESULT hr = _CopyShellItemArray(psia, &_psiaDrop);
        //if (SUCCEEDED(hr))
        //{
        //    _BindUI();
        //}
        //return hr;
    }
};

class HzDataObject : public IDataObject
{
public:
    HzDataObject() : _cRef(1), _pdtobjShell(NULL)
    {
    }

    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv)
    {
        static const QITAB qit[] = {
            QITABENT(HzDataObject, IDataObject),
            { 0 },
        };
        return QISearch(this, qit, riid, ppv);
    }

    IFACEMETHODIMP_(ULONG) AddRef()
    {
        return InterlockedIncrement(&_cRef);
    }

    IFACEMETHODIMP_(ULONG) Release()
    {
        long cRef = InterlockedDecrement(&_cRef);
        if (0 == cRef)
        {
            delete this;
        }
        return cRef;
    }

    // IDataObject
    IFACEMETHODIMP GetData(FORMATETC* pformatetcIn, STGMEDIUM* pmedium);

    IFACEMETHODIMP GetDataHere(FORMATETC* /* pformatetc */, STGMEDIUM* /* pmedium */)
    {
        return E_NOTIMPL;
    }

    IFACEMETHODIMP QueryGetData(FORMATETC* pformatetc);

    IFACEMETHODIMP GetCanonicalFormatEtc(FORMATETC* pformatetcIn, FORMATETC* pFormatetcOut)
    {
        *pFormatetcOut = *pformatetcIn;
        pFormatetcOut->ptd = NULL;
        return DATA_S_SAMEFORMATETC;
    }
    IFACEMETHODIMP SetData(FORMATETC* pformatetc, STGMEDIUM* pmedium, BOOL fRelease);
    IFACEMETHODIMP EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc);

    IFACEMETHODIMP DAdvise(FORMATETC* /* pformatetc */, DWORD /* advf */, IAdviseSink* /* pAdvSnk */, DWORD* /* pdwConnection */)
    {
        return E_NOTIMPL;
    }

    IFACEMETHODIMP DUnadvise(DWORD /* dwConnection */)
    {
        return E_NOTIMPL;
    }

    IFACEMETHODIMP EnumDAdvise(IEnumSTATDATA** /* ppenumAdvise */)
    {
        return E_NOTIMPL;
    }

private:
    ~HzDataObject()
    {
        if (_pdtobjShell) {
            _pdtobjShell->Release();
        }
    }

    HRESULT _EnsureShellDataObject()
    {
        // the shell data object imptlements ::SetData() in a way that will store any format
        // this code delegates to that implementation to avoid having to implement ::SetData()
        return _pdtobjShell ? S_OK : SHCreateDataObject(NULL, 0, NULL, NULL, IID_PPV_ARGS(&_pdtobjShell));
    }

    long _cRef;

    IDataObject* _pdtobjShell;
};