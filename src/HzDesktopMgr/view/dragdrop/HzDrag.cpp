#include <windows.h>
#include <shlobj.h>
#include <objbase.h>
#include <shlwapi.h>
#include <objidl.h>
#include <wil/com.h>

#include <QWidget>
#include <QtWinExtras/QtWin>

#include "HzDrag_p.h"

QObject* HzDrag::s_source = nullptr;

HzDrag::HzDrag(QObject *parent)
	: QObject(parent)
	, HzDesktopPublic(new HzDragPrivate())
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
	HZQ_D(HzDrag);

	DWORD finalEffect = DROPEFFECT_NONE;
	std::vector<LPCITEMIDLIST> selectedItemPidls;

	do 
	{
		HRESULT hr = S_OK;
		for (auto& path : m_pathList) {
			LPITEMIDLIST pidl = NULL;
			hr = SHParseDisplayName(path.toStdWString().c_str(), nullptr, &pidl, 0, nullptr);
			if (FAILED(hr)) {
				continue;
			}
			selectedItemPidls.push_back(pidl);
		}

		wil::com_ptr_nothrow<IShellItemArray> shellItemArray;
		hr = SHCreateShellItemArrayFromIDLists(static_cast<UINT>(selectedItemPidls.size()),
			selectedItemPidls.data(), &shellItemArray);
		if (FAILED(hr)) {
			break;
		}

		SHDRAGIMAGE image;
		image.sizeDragImage = { m_pixmap.width(), m_pixmap.height() };
		image.hbmpDragImage = QtWin::toHBITMAP(m_pixmap, QtWin::HBitmapAlpha);
		image.ptOffset = { m_hotSpot.x(), m_hotSpot.y() };

		wil::com_ptr_nothrow<IDataObject> dataObject;
		hr = d->GetDataObject(shellItemArray.get(), image, wil::out_param(dataObject));
		if (FAILED(hr)) {
			break;
		}

		hr = SHDoDragDrop(
			NULL, dataObject.get(), NULL,
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
