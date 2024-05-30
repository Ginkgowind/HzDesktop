#pragma once

#include "DesktopBackgroundView.h"
#include "HzDesktopIconView.h"
#include "HzSystemTray.h"

class DesktopViewMgr : public QObject
{
	Q_OBJECT

public:
	DesktopViewMgr();

	~DesktopViewMgr();

private:
	DesktopBackgroundView* m_bkgView = nullptr;
	HzDesktopIconView* m_desktopIconView = nullptr;
	HzSystemTray* m_systemTray = nullptr;
};
