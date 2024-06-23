#include <QMouseEvent>
#include <QMenu>
#include <QPainter>

#include "DesktopBackgroundView.h"

DesktopBackgroundView::DesktopBackgroundView()
	: m_menu(nullptr)
{
	//setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);

	//setAttribute(Qt::WA_TranslucentBackground, true);

	// TODO 单独设置下面这个为什么会变黑
	//setStyleSheet("background: transparent;");
}

DesktopBackgroundView::~DesktopBackgroundView()
{
}

void DesktopBackgroundView::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	painter.setPen(Qt::transparent);
	painter.setBrush(QColor(0, 0, 0, 1));
	painter.drawRect(rect());

	QWidget::paintEvent(event);
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

// TODO 这里也画一块透明的