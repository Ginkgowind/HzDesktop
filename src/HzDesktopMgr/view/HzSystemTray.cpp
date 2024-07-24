#include <QMenu>
#include <QAction>
#include <QWidget>
#include <QCoreApplication>

#include "HzSystemTray.h"

// 此处应该利用父对象
HzSystemTray::HzSystemTray(QWidget* parent)
	: QSystemTrayIcon(parent)
{

	//QAction *minimizeAction = new QAction(tr("Mi&nimize"), this);
	//connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);

	//QAction* maximizeAction = new QAction(tr("Ma&ximize"), this);
	//connect(maximizeAction, &QAction::triggered, this, &QWidget::showMaximized);

	//QAction* restoreAction = new QAction(tr("&Restore"), this);
	//connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

	QAction* quitAction = new QAction(tr("&Quit"), this);
	connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

	QMenu* trayIconMenu = new QMenu(parent);
	trayIconMenu->addAction(quitAction);

	setContextMenu(trayIconMenu);
	setIcon(QIcon(":/main/view/res/common/HzDesktop.ico"));

	// TODO 设置了父控件却还要主动调用，为什么？
	show();
}

HzSystemTray::~HzSystemTray()
{
}
