#include <windows.h>
#include <shlobj.h>
#include <objbase.h>
#include <shlwapi.h>
#include <objidl.h>
#include <wil/com.h>

#include <QWidget>

#include "HzDrag.h"
#include "HzDrag_p.h"

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
		hRet = pDragSourceHelper->InitializeFromWindow(HWND(0x123), &pt, dataObject.get());

		hRet = SHDoDragDrop(
			NULL, 
			dataObject.get(), nullptr,
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
