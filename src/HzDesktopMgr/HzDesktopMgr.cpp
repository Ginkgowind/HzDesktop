#include <windows.h>

#include "HzDesktopMgr.h"

HzDesktopMgr::HzDesktopMgr()
{
    CoInitialize(NULL);
    m_desktopViewMgr = new DesktopViewMgr();

    CoUninitialize();
}

HzDesktopMgr::~HzDesktopMgr()
{
}