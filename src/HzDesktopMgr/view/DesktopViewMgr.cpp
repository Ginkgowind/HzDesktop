#include <QScreen>
#include <QGuiApplication>

#include "DesktopViewMgr.h"

DesktopViewMgr::DesktopViewMgr()
{
	/*设置为桌面去除掉任务栏的矩形*/
	QRect availableRect = QGuiApplication::primaryScreen()->availableGeometry();
	
	m_bkgView = new DesktopBackgroundView();
	m_bkgView->setGeometry(availableRect);
	//m_bkgView->setGeometry(200, 200, 1200, 800);

	m_desktopIconView = new HzDesktopIconView(m_bkgView);

	m_systemTray = new HzSystemTray(m_bkgView);

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
