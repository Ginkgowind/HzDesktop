#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

class DesktopBackgroundView
	: public QWidget
{
	Q_OBJECT

public:
	DesktopBackgroundView();

	~DesktopBackgroundView();

signals:
	void onShowDesktopView();

protected:

	void mouseDoubleClickEvent(QMouseEvent* event) override;

	void contextMenuEvent(QContextMenuEvent* event) override;

private:

private:
	QMenu* m_menu;
};
