#include "CommonTools.h"
#include "StringConv.h"

namespace HZ
{
	bool isHzDesktopRunning()
	{
		wil::unique_mutex hMutex;
		hMutex.reset(::CreateMutexW(NULL, FALSE, HZDESKTOP_MUTEX_NAME));

		if (::GetLastError() == ERROR_ALREADY_EXISTS ||
			::GetLastError() == ERROR_ACCESS_DENIED) {
			return true;
		}

		return false;
	}
}