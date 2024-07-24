#pragma once

#include <windows.h>
#include <objidl.h>
#include <shlobj_core.h>
#include <type_traits>
#include <string>

#include <wil/resource.h>

enum class PasteType
{
	Normal,
	Shortcut
};

std::string ReadBinaryDataFromGlobal(HGLOBAL global);

// TODO �ĺ�������
HRESULT GetBlobData_tmpname(IDataObject* dataObject, CLIPFORMAT format, std::string& outputData);

template <typename T>
	requires std::is_trivially_copyable_v<T>&& std::is_trivially_constructible_v<T>
HRESULT GetBlobData(IDataObject* dataObject, CLIPFORMAT format, T& outputData)
{
	std::string binaryData;
	RETURN_IF_FAILED(GetBlobData_tmpname(dataObject, format, binaryData));

	// The size of the data that's retrieved is determined by GlobalSize(). As indicated by the
	// documentation for that function, the returned size can be larger than the size requested when
	// the memory was allocated. If the size is smaller than the size of the target type, however,
	// something has gone wrong and it doesn't make sense to try and use the data.
	if (binaryData.size() < sizeof(T)) {
		return E_FAIL;
	}

	std::memcpy(&outputData, binaryData.data(), sizeof(T));

	return S_OK;
}

bool CanShellPasteDataObject(PCIDLIST_ABSOLUTE destination, IDataObject* dataObject,
	PasteType pasteType);

bool CanPasteInDirectory(PCIDLIST_ABSOLUTE pidl, PasteType pasteType);
