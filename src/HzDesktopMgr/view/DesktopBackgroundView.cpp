#include <QScreen>

#include "DesktopBackgroundView.h"

DesktopBackgroundView::DesktopBackgroundView()
{
	//setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
	//setAcceptDrops(true);
	////����Ϊ����ȥ�����������ľ���
	//QRect availableRect = QGuiApplication::primaryScreen()->availableGeometry();
	//setGeometry(availableRect);
	setGeometry(800, 800, 800, 600);

	//setAttribute(Qt::WA_TranslucentBackground, true);
}

DesktopBackgroundView::~DesktopBackgroundView()
{
}
