#include <windows.h>

#include "HzDesktopMgr.h"

HzDesktopMgr::HzDesktopMgr()
{
    CoInitialize(NULL);

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