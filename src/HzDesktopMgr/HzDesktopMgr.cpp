#include <windows.h>

#include "HzDesktopMgr.h"

HzDesktopMgr::HzDesktopMgr()
{
    CoInitialize(NULL);

	// ��ʼ��Common Controls������ʹ��ͼ���б��������
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