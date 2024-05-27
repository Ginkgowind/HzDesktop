#include <QScreen>
#include <QGuiApplication>

#include "DesktopBackgroundView.h"

DesktopBackgroundView::DesktopBackgroundView()
{
	setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
	setAcceptDrops(true);
	// ����Ϊ����ȥ�����������ľ���
	QRect availableRect = QGuiApplication::primaryScreen()->availableGeometry();
	setGeometry(availableRect);
	setGeometry(0, 0, 1200, 800);

	//setAttribute(Qt::WA_TranslucentBackground, true);
	//setAttribute(Qt::WA_TransparentForMouseEvents, false);
	setStyleSheet("background: transparent;");

	setGeometry(600, 600, 1200, 800);
}

DesktopBackgroundView::~DesktopBackgroundView()
{
}
