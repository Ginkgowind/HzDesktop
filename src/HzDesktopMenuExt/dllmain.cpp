// dllmain.cpp: DllMain 的实现。

#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "HzDesktopMenuExt_i.h"
#include "dllmain.h"

CHzDesktopMenuExtModule _AtlModule;

HINSTANCE g_hInsDll = NULL;

// DLL 入口点
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	g_hInsDll = hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved);
}
