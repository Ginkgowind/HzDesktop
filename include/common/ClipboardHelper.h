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

std::string ReadBinaryDataFromGlobal(HGLOBAL global)
{
	wil::unique_hglobal_locked mem(global);

	if (!mem)
	{
		return "";
	}

	auto size = GlobalSize(mem.get());

	if (size == 0)
	{
		return "";
	}

	return std::string(static_cast<const char*>(mem.get()), size);
}

HRESULT GetBlobData(IDataObject* dataObject, CLIPFORMAT format, std::string& outputData)
{
	HRESULT hr = S_OK;

	FORMATETC ftc = { format, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	
	wil::unique_stg_medium stg;
	hr = dataObject->GetData(&ftc, &stg);
	if (FAILED(hr)) {
		return hr;
	}

	outputData = ReadBinaryDataFromGlobal(stg.hGlobal);
	if (outputData.empty())
	{
		return E_FAIL;
	}

	return S_OK;
}

template <typename T>
	requires std::is_trivially_copyable_v<T>&& std::is_trivially_constructible_v<T>
HRESULT GetBlobData(IDataObject* dataObject, CLIPFORMAT format, T& outputData)
{
	std::string binaryData;
	RETURN_IF_FAILED(GetBlobData(dataObject, format, binaryData));

	// The size of the data that's retrieved is determined by GlobalSize(). As indicated by the
	// documentation for that function, the returned size can be larger than the size requested when
	// the memory was allocated. If the size is smaller than the size of the target type, however,
	// something has gone wrong and it doesn't make sense to try and use the data.
	if (binaryData.size() < sizeof(T))
	{
		return E_FAIL;
	}

	std::memcpy(&outputData, binaryData.data(), sizeof(T));

	return S_OK;
}

bool CanShellPasteDataObject(PCIDLIST_ABSOLUTE destination, IDataObject* dataObject,
	PasteType pasteType);
