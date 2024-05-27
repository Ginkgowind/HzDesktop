#include "DesktopViewMgr.h"

DesktopViewMgr::DesktopViewMgr()
{
	//m_bkgView = new DesktopBackgroundView();

	m_desktopIconView = new HzDesktopIconView(m_bkgView);

	m_systemTray = new HzSystemTray(m_desktopIconView);

	m_desktopIconView->show();

	//m_bkgView->show();
}

DesktopViewMgr::~DesktopViewMgr()
{
}
