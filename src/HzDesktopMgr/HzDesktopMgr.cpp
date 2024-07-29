#include <windows.h>

#include "HzDesktopMgr.h"

HzDesktopMgr::HzDesktopMgr()
{
    CoInitialize(NULL);
	// TODO 是否需要？
	//CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	// 初始化Common Controls，这是使用图像列表所必需的
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex);

    m_desktopViewMgr = new DesktopViewMgr();

    CoUninitialize();
}

HzDesktopMgr::~HzDesktopMgr()
{
}