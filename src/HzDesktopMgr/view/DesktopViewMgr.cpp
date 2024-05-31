#include <QScreen>
#include <QGuiApplication>

#include "DesktopViewMgr.h"

DesktopViewMgr::DesktopViewMgr()
{
	/*����Ϊ����ȥ�����������ľ���*/
	QRect availableRect = QGuiApplication::primaryScreen()->availableGeometry();
	
	m_bkgView = new DesktopBackgroundView();
	m_bkgView->setGeometry(availableRect);

	m_desktopIconView = new HzDesktopIconView(m_bkgView);
	m_desktopIconView->setFixedSize(availableRect.size());

	m_systemTray = new HzSystemTray(m_desktopIconView);

	connect(m_bkgView, &DesktopBackgroundView::onShowDesktopView,
		[this]() {m_desktopIconView->setVisible(true); });

	// TODO Ϊʲô������������
	//connect(m_bkgView, &DesktopBackgroundView::onShowDesktopView,
	//	[m_desktopIconView]() {m_desktopIconView->setVisible(true); });


	m_bkgView->show();
}

DesktopViewMgr::~DesktopViewMgr()
{
}
