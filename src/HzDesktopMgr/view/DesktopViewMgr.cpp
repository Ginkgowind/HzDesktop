#include "DesktopViewMgr.h"

DesktopViewMgr::DesktopViewMgr()
{
	m_bkgView = new DesktopBackgroundView();

	m_iconView = new HzIconView(m_bkgView);

	m_systemTray = new HzSystemTray(m_bkgView);

	m_bkgView->show();
}

DesktopViewMgr::~DesktopViewMgr()
{
}
