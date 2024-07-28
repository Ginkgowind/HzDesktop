#include <strsafe.h>
#include <QtWinExtras/QtWin>
#include <wil/com.h>

#include "HzDrag_p.h"

HzDragPrivate::HzDragPrivate()
{
    //CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_INPROC, IID_PPV_ARGS(&_pdth));
}

HzDragPrivate::~HzDragPrivate()
{
}

HRESULT HzDragPrivate::GetDataObject(IShellItemArray* psiaItems, SHDRAGIMAGE& image, IDataObject** ppdtobj)
{
    HZQ_Q(HzDrag);

    HRESULT hr = S_OK;

    do 
    {
        // 实测不需要下面这里获取，直接从psiaItems获取也是可以的
		hr = HzDataObject_CreateInstance(IID_PPV_ARGS(ppdtobj));
		if (FAILED(hr)) {
			break;
		}

        wil::com_ptr_nothrow<IDropTargetHelper> pdth;
        hr = CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_INPROC, IID_PPV_ARGS(&pdth));
		if (FAILED(hr)) {
			break;
		}

        wil::com_ptr_nothrow<IDragSourceHelper2> pdsh;
        hr = pdth->QueryInterface(IID_PPV_ARGS(&pdsh));
		if (FAILED(hr)) {
			break;
		}

        hr = psiaItems->BindToHandler(NULL, BHID_DataObject, IID_PPV_ARGS(ppdtobj));
		if (FAILED(hr)) {
			break;
		}

		// enable drop tips
		pdsh->SetFlags(DSH_ALLOWDROPDESCRIPTIONTEXT);

		// note that InitializeFromBitmap() takes ownership of the hbmp
			// so we should not free it by calling DeleteObject()
		pdsh->InitializeFromBitmap(&image, *ppdtobj);

    } while (false);

    return hr;
}

HRESULT HzDragPrivate::HzDataObject_CreateInstance(REFIID riid, void** ppv)
{
    *ppv = NULL;
    HzDataObject* p = new (std::nothrow) HzDataObject();
    HRESULT hr = p ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hr)) {
        hr = p->QueryInterface(riid, ppv);
        p->Release();
    }

    return hr;
}

WCHAR const c_szText[] = L"Clipboard Contents";

STDMETHODIMP HzDataObject::GetData(FORMATETC* pformatetcIn, STGMEDIUM* pmedium)
{
    ZeroMemory(pmedium, sizeof(*pmedium));

    HRESULT hr = DATA_E_FORMATETC;
    if (pformatetcIn->cfFormat == CF_UNICODETEXT)
    {
        if (pformatetcIn->tymed & TYMED_HGLOBAL)
        {
            UINT cb = sizeof(c_szText[0]) * (lstrlen(c_szText) + 1);
            HGLOBAL h = GlobalAlloc(GPTR, cb);
            hr = h ? S_OK : E_OUTOFMEMORY;
            if (SUCCEEDED(hr))
            {
                StringCbCopy((PWSTR)h, cb, c_szText);
                pmedium->hGlobal = h;
                pmedium->tymed = TYMED_HGLOBAL;
            }
        }
    }
    else if (SUCCEEDED(_EnsureShellDataObject()))
    {
        hr = _pdtobjShell->GetData(pformatetcIn, pmedium);
    }
    return hr;
}

IFACEMETHODIMP HzDataObject::SetData(FORMATETC* pformatetc, STGMEDIUM* pmedium, BOOL fRelease)
{
    HRESULT hr = _EnsureShellDataObject();
    if (SUCCEEDED(hr))
    {
        hr = _pdtobjShell->SetData(pformatetc, pmedium, fRelease);
    }
    return hr;
}

STDMETHODIMP HzDataObject::QueryGetData(FORMATETC* pformatetc)
{
    HRESULT hr = S_FALSE;
    if (pformatetc->cfFormat == CF_UNICODETEXT)
    {
        hr = S_OK;
    }
    else if (SUCCEEDED(_EnsureShellDataObject()))
    {
        hr = _pdtobjShell->QueryGetData(pformatetc);
    }
    return hr;
}

STDMETHODIMP HzDataObject::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc)
{
    *ppenumFormatEtc = NULL;
    HRESULT hr = E_NOTIMPL;
    if (dwDirection == DATADIR_GET)
    {
        FORMATETC rgfmtetc[] =
        {
            // the order here defines the accuarcy of rendering
            { CF_UNICODETEXT,          NULL, 0,  0, TYMED_HGLOBAL },
        };
        hr = SHCreateStdEnumFmtEtc(ARRAYSIZE(rgfmtetc), rgfmtetc, ppenumFormatEtc);
    }
    return hr;
}