#include <QScreen>
#include <QGuiApplication>

#include "DesktopBackgroundView.h"

DesktopBackgroundView::DesktopBackgroundView()
{
	//setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
	// 设置为桌面去除掉任务栏的矩形
	//QRect availableRect = QGuiApplication::primaryScreen()->availableGeometry();
	//setGeometry(availableRect);
	//setGeometry(0, 0, 1200, 800);

	setAttribute(Qt::WA_TranslucentBackground, true);
	// TODO 单独设置下面这个为什么会变黑
	//setStyleSheet("background: transparent;");

	setGeometry(500, 500, 1200, 800);
}

DesktopBackgroundView::~DesktopBackgroundView()
{
}

// TODO 这里也画一块透明的