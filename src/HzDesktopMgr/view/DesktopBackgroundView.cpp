#include <QScreen>
#include <QGuiApplication>

#include "DesktopBackgroundView.h"

DesktopBackgroundView::DesktopBackgroundView()
{
	//setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
	// ����Ϊ����ȥ�����������ľ���
	//QRect availableRect = QGuiApplication::primaryScreen()->availableGeometry();
	//setGeometry(availableRect);
	//setGeometry(0, 0, 1200, 800);

	setAttribute(Qt::WA_TranslucentBackground, true);
	// TODO ���������������Ϊʲô����
	//setStyleSheet("background: transparent;");

	setGeometry(500, 500, 1200, 800);
}

DesktopBackgroundView::~DesktopBackgroundView()
{
}

// TODO ����Ҳ��һ��͸����