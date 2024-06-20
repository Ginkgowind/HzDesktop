#include <windows.h>
#include <shlobj.h>
#include <objbase.h>
#include <shlwapi.h>
#include <objidl.h>
#include <wil/com.h>

#include <QWidget>
#include <QtWinExtras/QtWin>

#include "HzDrag.h"
#include "HzDrag_p.h"

enum class DragType
{
	LeftClick,
	RightClick
};

class DropSource : public IDropSource
{
public:

	DropSource(DragType dragType);

	HRESULT		__stdcall	QueryInterface(REFIID iid, void** ppvObject);
	ULONG		__stdcall	AddRef();
	ULONG		__stdcall	Release();

	HRESULT _stdcall	QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState);
	HRESULT _stdcall	GiveFeedback(DWORD dwEffect);

private:

	LONG		m_lRefCount;
	DragType	m_DragType;
};

HRESULT CreateDropSource(IDropSource** ppDropSource, DragType dragType)
{
	if (ppDropSource == NULL)
	{
		return E_FAIL;
	}

	*ppDropSource = new DropSource(dragType);

	return S_OK;
}

DropSource::DropSource(DragType dragType)
{
	m_lRefCount = 1;
	m_DragType = dragType;
}

/* IUnknown interface members. */
HRESULT __stdcall DropSource::QueryInterface(REFIID iid, void** ppvObject)
{
	*ppvObject = NULL;

	if (iid == IID_IDropSource ||
		iid == IID_IUnknown)
	{
		*ppvObject = this;
	}

	if (*ppvObject)
	{
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG __stdcall DropSource::AddRef()
{
	return InterlockedIncrement(&m_lRefCount);
}

ULONG __stdcall DropSource::Release()
{
	LONG lCount = InterlockedDecrement(&m_lRefCount);

	if (lCount == 0)
	{
		delete this;
		return 0;
	}

	return lCount;
}


HRESULT _stdcall DropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
{
	DWORD dwStopButton = 0;

	if (m_DragType == DragType::LeftClick)
	{
		if ((grfKeyState & MK_LBUTTON) == 0)
		{
			return DRAGDROP_S_DROP;
		}

		dwStopButton = MK_RBUTTON;
	}
	else if (m_DragType == DragType::RightClick)
	{
		if ((grfKeyState & MK_RBUTTON) == 0)
		{
			return DRAGDROP_S_DROP;
		}

		dwStopButton = MK_LBUTTON;
	}

	if (fEscapePressed == TRUE || grfKeyState & dwStopButton)
	{
		return DRAGDROP_S_CANCEL;
	}

	return S_OK;
}

HRESULT _stdcall DropSource::GiveFeedback(DWORD dwEffect)
{
	UNREFERENCED_PARAMETER(dwEffect);

	return DRAGDROP_S_USEDEFAULTCURSORS;
}



HzDrag::HzDrag(QObject *parent)
	: QObject(parent)
{}

HzDrag::~HzDrag()
{
}

void HzDrag::setItemPaths(const QStringList pathList)
{
	m_pathList = pathList;
}

void HzDrag::setPixmap(const QPixmap& pixmap)
{
	m_pixmap = pixmap;
}

Qt::DropAction HzDrag::exec(Qt::DropActions supportedActions)
{
	DWORD finalEffect = DROPEFFECT_NONE;
	std::vector<LPCITEMIDLIST> selectedItemPidls;

	do 
	{
		HRESULT hRet = S_OK;
		for (auto& path : m_pathList) {
			LPITEMIDLIST pidl = NULL;
			path.replace('/', '\\');
			hRet = SHParseDisplayName(path.toStdWString().c_str(), nullptr, &pidl, 0, nullptr);
			if (FAILED(hRet)) {
				continue;
			}
			selectedItemPidls.push_back(pidl);
		}

		wil::com_ptr_nothrow<IDataObject> dataObject;
		wil::com_ptr_nothrow<IShellItemArray> shellItemArray;
		hRet = SHCreateShellItemArrayFromIDLists(static_cast<UINT>(selectedItemPidls.size()),
			selectedItemPidls.data(), &shellItemArray);
		if (FAILED(hRet)) {
			break;
		}

		hRet = shellItemArray->BindToHandler(nullptr, BHID_DataObject, IID_PPV_ARGS(&dataObject));
		if (FAILED(hRet)) {
			break;
		}

		POINT				pt = { 0,0 };
		IDragSourceHelper* pDragSourceHelper = nullptr;
		CoCreateInstance(CLSID_DragDropHelper, nullptr, CLSCTX_ALL,
			IID_PPV_ARGS(&pDragSourceHelper));
		//hRet = pDragSourceHelper->InitializeFromWindow(HWND(0x123), &pt, dataObject.get());
		SHDRAGIMAGE image;
		image.sizeDragImage = { m_pixmap.width(), m_pixmap.height() };
		image.hbmpDragImage = QtWin::toHBITMAP(m_pixmap);
		hRet = pDragSourceHelper->InitializeFromBitmap(&image, dataObject.get());

		IDropSource* pDropSource = nullptr;
		hRet = CreateDropSource(&pDropSource, DragType::LeftClick);

		//hRet = SHDoDragDrop(
		//	NULL, 
		//	dataObject.get(), nullptr,
		//	supportedActions, &finalEffect);
		hRet = DoDragDrop(
			dataObject.get(), pDropSource,
			supportedActions, &finalEffect);
		if (FAILED(hRet)) {
			break;
		}
	} while (false);
	
	for (auto pidl : selectedItemPidls) {
		CoTaskMemFree((LPVOID)pidl);
	}

	deleteLater();

	return Qt::DropAction(finalEffect);
}
