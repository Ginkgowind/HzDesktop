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

class DropSource : public IDropSource
{
public:

	DropSource();

	HRESULT		__stdcall	QueryInterface(REFIID iid, void** ppvObject);
	ULONG		__stdcall	AddRef();
	ULONG		__stdcall	Release();

	HRESULT _stdcall	QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState);
	HRESULT _stdcall	GiveFeedback(DWORD dwEffect);

private:

	LONG		m_lRefCount;
};

DropSource::DropSource()
{
	m_lRefCount = 1;
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

	if ((grfKeyState & MK_LBUTTON) == 0)
	{
		return DRAGDROP_S_DROP;
	}

	dwStopButton = MK_RBUTTON;

	if (fEscapePressed == TRUE || grfKeyState & dwStopButton)
	{
		return DRAGDROP_S_CANCEL;
	}

	return S_OK;
}

HRESULT _stdcall DropSource::GiveFeedback(DWORD dwEffect)
{
	UNREFERENCED_PARAMETER(dwEffect);

	return S_OK;
	return DRAGDROP_S_USEDEFAULTCURSORS;
}

QObject* HzDrag::s_source = nullptr;

HzDrag::HzDrag(QObject *parent)
	: QObject(parent)
{
	s_source = parent;
}

HzDrag::~HzDrag()
{
	s_source = nullptr;
}

void HzDrag::setItemPaths(const QStringList pathList)
{
	m_pathList = pathList;
}

void HzDrag::setPixmap(const QPixmap& pixmap)
{
	m_pixmap = pixmap;
}

void HzDrag::setHotSpot(const QPoint& hotspot)
{
	m_hotSpot = hotspot;
}

Qt::DropAction HzDrag::exec(Qt::DropActions supportedActions)
{
	DWORD finalEffect = DROPEFFECT_NONE;
	std::vector<LPCITEMIDLIST> selectedItemPidls;

	do 
	{
		HRESULT hr = S_OK;
		for (auto& path : m_pathList) {
			LPITEMIDLIST pidl = NULL;
			path.replace('/', '\\');
			hr = SHParseDisplayName(path.toStdWString().c_str(), nullptr, &pidl, 0, nullptr);
			if (FAILED(hr)) {
				continue;
			}
			selectedItemPidls.push_back(pidl);
		}

		wil::com_ptr_nothrow<IDataObject> dataObject;
		wil::com_ptr_nothrow<IShellItemArray> shellItemArray;
		hr = SHCreateShellItemArrayFromIDLists(static_cast<UINT>(selectedItemPidls.size()),
			selectedItemPidls.data(), &shellItemArray);
		if (FAILED(hr)) {
			break;
		}

		hr = shellItemArray->BindToHandler(nullptr, BHID_DataObject, IID_PPV_ARGS(&dataObject));
		if (FAILED(hr)) {
			break;
		}

		IDragSourceHelper* pDragSourceHelper = nullptr;
		CoCreateInstance(CLSID_DragDropHelper, nullptr, CLSCTX_ALL,
			IID_PPV_ARGS(&pDragSourceHelper));

		SHDRAGIMAGE image;
		image.sizeDragImage = { m_pixmap.width(), m_pixmap.height() };
		image.hbmpDragImage = QtWin::toHBITMAP(m_pixmap);
		image.crColorKey = 0x00FFFFFF;	// TODO ÐÞ¸Ä
		image.ptOffset = { m_hotSpot.x(), m_hotSpot.y() };
		hr = pDragSourceHelper->InitializeFromBitmap(&image, dataObject.get());

		IDropSource* pDropSource = new DropSource();

		hr = DoDragDrop(
			dataObject.get(), pDropSource,
			supportedActions, &finalEffect);
		if (FAILED(hr)) {
			break;
		}
	} while (false);
	
	for (auto pidl : selectedItemPidls) {
		CoTaskMemFree((LPVOID)pidl);
	}

	deleteLater();

	return Qt::DropAction(finalEffect);
}
