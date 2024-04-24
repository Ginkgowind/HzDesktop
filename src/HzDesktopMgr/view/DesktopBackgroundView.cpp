#include <QScreen>

#include "DesktopBackgroundView.h"

DesktopBackgroundView::DesktopBackgroundView()
{
	//setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
	//setAcceptDrops(true);
	////设置为桌面去除掉任务栏的矩形
	//QRect availableRect = QGuiApplication::primaryScreen()->availableGeometry();
	//setGeometry(availableRect);
	setGeometry(800, 800, 800, 600);

	//setAttribute(Qt::WA_TranslucentBackground, true);
}

DesktopBackgroundView::~DesktopBackgroundView()
{
}
