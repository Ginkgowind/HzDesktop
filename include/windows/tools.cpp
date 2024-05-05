#include <shlobj_core.h>

#include "tools.h"

namespace HZ
{
	QString getPublicDesktopPath()
	{
		char path[MAX_PATH] = { 0 };
		HRESULT hr = SHGetFolderPathA(NULL, CSIDL_COMMON_DESKTOPDIRECTORY, NULL, 0, path);
		if (FAILED(hr))
		{
			//LOG_ERROR_W("SHGetFolderPathW index(%d) failed hr(0x%x), err(%d).", i, hr, GetLastError());
			return "";
		}

		return path;
	}
}