#pragma once

#include "DesktopBackgroundView.h"
#include "HzIconView.h"
#include "HzSystemTray.h"

class DesktopViewMgr
{
public:
	DesktopViewMgr();

	~DesktopViewMgr();

private:
	DesktopBackgroundView* m_bkgView = nullptr;
	HzIconView* m_iconView = nullptr;
	HzSystemTray* m_systemTray = nullptr;
};

