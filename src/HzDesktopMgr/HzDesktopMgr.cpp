#include <windows.h>

#include "HzDesktopMgr.h"

HzDesktopMgr::HzDesktopMgr()
{
    CoInitialize(NULL);
	// TODO �Ƿ���Ҫ��
	//CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

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