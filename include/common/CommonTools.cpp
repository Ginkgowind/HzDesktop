#include <wil/com.h>

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

	bool launchHzDesktop()
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		if (!CreateProcessW(
			L"D:\\Documents\\Codes\\CppDemos\\HzDesktop\\build\\Release\\HzDesktopMgr.exe",
			NULL, // 命令行
			NULL,    // 进程属性
			NULL,    // 线程属性
			FALSE,   // 不继承句柄
			0,       // 创建标志
			NULL,    // 使用父进程的环境块
			NULL,    // 使用父进程的当前目录
			&si,     // STARTUPINFO结构
			&pi))    // PROCESS_INFORMATION结构
		{
			return false;
		}

		return true;
	}
}