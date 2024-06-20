#include <shlobj_core.h>

#include "tools.h"

namespace HZ
{
	bool isSystem64Bit()
	{
		// ������ֻ�ṩ64λ
		return (sizeof(void*) == 8);
	}

	int getSystemVersion()
	{
        int version = 0;

        OSVERSIONINFOEX osvi;
        ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
        osvi.dwMajorVersion = 10; // ����Ҫ���� Windows �汾

        // ������������
        DWORDLONG dwlConditionMask = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);

        // ͨ�� VerifyVersionInfo �������汾
        if (VerifyVersionInfo(&osvi, VER_MAJORVERSION, dwlConditionMask))
        {
            version = 10; // Win10
        }
        else
        {
            osvi.dwMajorVersion = 11; // ����Ҫ���� Windows �汾Ϊ 11
            if (VerifyVersionInfo(&osvi, VER_MAJORVERSION, dwlConditionMask))
            {
                version = 11; // Win11
            }
        }

        return version;
	}

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