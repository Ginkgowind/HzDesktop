#include "DesktopViewMgr.h"

DesktopViewMgr::DesktopViewMgr()
{
	m_bkgView = new DesktopBackgroundView();

	m_desktopIconView = new HzDesktopIconView(m_bkgView);

	m_systemTray = new HzSystemTray(m_desktopIconView);

	connect(m_bkgView, &DesktopBackgroundView::onShowDesktopView,
		[this]() {m_desktopIconView->setVisible(true); });

	// TODO 为什么下面这样不行
	//connect(m_bkgView, &DesktopBackgroundView::onShowDesktopView,
	//	[m_desktopIconView]() {m_desktopIconView->setVisible(true); });


	m_bkgView->show();
}

DesktopViewMgr::~DesktopViewMgr()
{
}
