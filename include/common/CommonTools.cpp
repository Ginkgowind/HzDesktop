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
			NULL, // ������
			NULL,    // ��������
			NULL,    // �߳�����
			FALSE,   // ���̳о��
			0,       // ������־
			NULL,    // ʹ�ø����̵Ļ�����
			NULL,    // ʹ�ø����̵ĵ�ǰĿ¼
			&si,     // STARTUPINFO�ṹ
			&pi))    // PROCESS_INFORMATION�ṹ
		{
			return false;
		}

		return true;
	}
}