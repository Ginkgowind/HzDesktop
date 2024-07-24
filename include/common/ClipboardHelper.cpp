#include "ClipboardHelper.h"

#include <wil/com.h>

std::string ReadBinaryDataFromGlobal(HGLOBAL global)
{
	wil::unique_hglobal_locked mem(global);

	if (!mem) {
		return "";
	}

	auto size = GlobalSize(mem.get());

	if (size == 0) {
		return "";
	}

	return std::string(static_cast<const char*>(mem.get()), size);
}

HRESULT GetBlobData_tmpname(IDataObject* dataObject, CLIPFORMAT format, std::string& outputData)
{
	HRESULT hr = S_OK;

	FORMATETC ftc = { format, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

	wil::unique_stg_medium stg;
	hr = dataObject->GetData(&ftc, &stg);
	if (FAILED(hr)) {
		return hr;
	}

	outputData = ReadBinaryDataFromGlobal(stg.hGlobal);
	if (outputData.empty()) {
		return E_FAIL;
	}

	return S_OK;
}

bool CanShellPasteDataObject(PCIDLIST_ABSOLUTE destination, IDataObject* dataObject,
	PasteType pasteType)
{
	wil::com_ptr_nothrow<IShellFolder> shellFolder;
	HRESULT hr = SHBindToObject(nullptr, destination, nullptr, IID_PPV_ARGS(&shellFolder));

	if (FAILED(hr)) {
		return false;
	}

	wil::com_ptr_nothrow<IDropTarget> dropTarget;
	hr = shellFolder->CreateViewObject(nullptr, IID_PPV_ARGS(&dropTarget));

	if (FAILED(hr)) {
		return false;
	}

	DWORD allowedEffects = DROPEFFECT_NONE;

	switch (pasteType)
	{
	case PasteType::Normal:
		allowedEffects = DROPEFFECT_COPY | DROPEFFECT_MOVE;
		break;

	case PasteType::Shortcut:
		allowedEffects = DROPEFFECT_LINK;
		break;
	}

	// The value below indicates whether the source prefers a copy or a move and will be used to try
	// and determine whether a paste operation would succeed. This is important for folders like the
	// recycle bin, where it is possible to paste, though only if the items are moved.
	DWORD preferredEffect;
	hr = GetBlobData(dataObject,
		static_cast<CLIPFORMAT>(RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT)), preferredEffect);

	if (FAILED(hr)) {
		// If no preferred effect is set, assume that the source prefers a copy.
		preferredEffect = DROPEFFECT_COPY | DROPEFFECT_LINK;
	}

	allowedEffects &= preferredEffect;

	// Internally, a paste is a simulated drop, so this should give a reliable indication of whether
	// such a drop would succeed.
	// Note that this can cause the COM modal loop to run and PeekMessage() to be called. In that
	// case, certain window messages, like WM_CLOSE, can be dispatched.
	DWORD finalEffect = allowedEffects;
	hr = dropTarget->DragEnter(dataObject, MK_LBUTTON, { 0, 0 }, &finalEffect);

	if (FAILED(hr) || finalEffect == DROPEFFECT_NONE) {
		return false;
	}

	return true;
}


bool CanPasteInDirectory(PCIDLIST_ABSOLUTE pidl, PasteType pasteType)
{
	wil::com_ptr_nothrow<IDataObject> clipboardObject;
	HRESULT hr = OleGetClipboard(&clipboardObject);

	if (FAILED(hr)) {
		return false;
	}

	return CanShellPasteDataObject(pidl, clipboardObject.get(), pasteType);
}
