#pragma once

#include "view/DesktopViewMgr.h"

class HzDesktopMgr
{
public:
    HzDesktopMgr();
    ~HzDesktopMgr();

private:
    DesktopViewMgr* m_desktopViewMgr = nullptr;
};
