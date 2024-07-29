#include <QScreen>
#include <QStandardPaths>
#include <QGuiApplication>

#include "DesktopViewMgr.h"

DesktopViewMgr::DesktopViewMgr()
{
	/*����Ϊ����ȥ�����������ľ���*/
	QRect availableRect = QGuiApplication::primaryScreen()->availableGeometry();
	
	m_bkgView = new DesktopBackgroundView();
	m_bkgView->setGeometry(availableRect);
	//m_bkgView->setGeometry(200, 200, 1200, 800);

	std::wstring desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)
		.replace('/', '\\').toStdWString();
	m_desktopIconView = new HzDesktopIconView(desktopPath, m_bkgView);

	m_systemTray = new HzSystemTray(m_bkgView);

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
