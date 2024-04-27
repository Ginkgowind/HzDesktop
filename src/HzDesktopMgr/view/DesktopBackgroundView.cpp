#include <QScreen>
#include <QGuiApplication>

#include "DesktopBackgroundView.h"

DesktopBackgroundView::DesktopBackgroundView()
{
	setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
	setAcceptDrops(true);
	//设置为桌面去除掉任务栏的矩形
	QRect availableRect = QGuiApplication::primaryScreen()->availableGeometry();
	setGeometry(availableRect);
	setGeometry(0, 0, 1200, 800);

	setAttribute(Qt::WA_TranslucentBackground, true);

	//setGeometry(600, 0, 1200, 800);
}

DesktopBackgroundView::~DesktopBackgroundView()
{
}
