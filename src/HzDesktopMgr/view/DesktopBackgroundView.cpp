#include <QScreen>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QMenu>

#include "DesktopBackgroundView.h"

DesktopBackgroundView::DesktopBackgroundView()
	: m_menu(nullptr)
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

void DesktopBackgroundView::mouseDoubleClickEvent(QMouseEvent* event)
{
	emit onShowDesktopView();

	QWidget::mouseDoubleClickEvent(event);
}

void DesktopBackgroundView::contextMenuEvent(QContextMenuEvent* event)
{
	if (!m_menu) {
		m_menu = new QMenu(this);
		m_menu->addAction(tr("Show Desktop"), this,
			[this]() {emit onShowDesktopView(); });
	}

	m_menu->exec(QCursor::pos());
}

// TODO ����Ҳ��һ��͸����