#include <QMenu>
#include <QAction>
#include <QWidget>
#include <QCoreApplication>

#include "HzSystemTray.h"

// �˴�Ӧ�����ø�����
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

	// TODO �����˸��ؼ�ȴ��Ҫ�������ã�Ϊʲô��
	show();
}

HzSystemTray::~HzSystemTray()
{
}
