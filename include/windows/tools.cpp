#include <shlobj_core.h>

#include "tools.h"

namespace HZ
{
	std::wstring getUserDesktopPath()
	{
		WCHAR path[MAX_PATH] = { 0 };
		HRESULT hr = SHGetFolderPathW(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, path);
		if (!SUCCEEDED(hr))
		{
			//LOG_ERROR_W("SHGetFolderPathW index(%d) failed hr(0x%x), err(%d).", i, hr, GetLastError());
			return L"";
		}

		return path;
	}
	std::wstring getPublicDesktopPath()
	{
		WCHAR path[MAX_PATH] = { 0 };
		HRESULT hr = SHGetFolderPathW(NULL, CSIDL_COMMON_DESKTOPDIRECTORY, NULL, 0, path);
		if (!SUCCEEDED(hr))
		{
			//LOG_ERROR_W("SHGetFolderPathW index(%d) failed hr(0x%x), err(%d).", i, hr, GetLastError());
			return L"";
		}

		return path;
	}
}