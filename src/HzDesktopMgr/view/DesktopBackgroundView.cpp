#include <QMouseEvent>
#include <QMenu>
#include <QPainter>
#include <QDebug>

#include <Windows.h>

#include "DesktopBackgroundView.h"

DesktopBackgroundView::DesktopBackgroundView()
	: m_menu(nullptr)
{
	// 以此实现只影响当前类而不影响子类
	setObjectName("bkgview");
	setStyleSheet("#bkgview {background: transparent;}");

	inplace();
}

DesktopBackgroundView::~DesktopBackgroundView()
{
}

//void DesktopBackgroundView::paintEvent(QPaintEvent* event)
//{
//	//QPainter painter(this);
//	//painter.setCompositionMode(QPainter::CompositionMode_Clear);
//	//painter.fillRect(this->rect(), Qt::transparent);
//
//	QPainter painter(this);
//
//	painter.setPen(Qt::transparent);
//	painter.setBrush(QColor(0, 0, 0, 1));
//	painter.drawRect(rect());
//
//	QWidget::paintEvent(event);
//}

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

void DesktopBackgroundView::inplace() 
{
	// 接入到图标层
	HWND background = NULL;
	HWND worker = NULL;

	// 循环查找WorkerW窗口
	do {
		worker = FindWindowExA(NULL, worker, "WorkerW", NULL);
		if (worker != NULL) {
			qDebug() << "Find WokerW";
			// 尝试找到SHELLDLL_DefView窗口
			HWND shelldlldefview = FindWindowExA(worker, NULL, "SHELLDLL_DefView", NULL);
			if (shelldlldefview != NULL) {
				qDebug() << "Find SHELLDLL_DefView";
				// 检查SHELLDLL_DefView的父窗口是否为当前的WorkerW窗口
				HWND parent = GetParent(shelldlldefview);
				if (parent != NULL) {
					qDebug() << "Find SHELLDLL_DefView's Parent";
					if (parent == worker) {
						qDebug() << "Right!";

						// 找到了正确的WorkerW窗口
						background = shelldlldefview;
						break; // 结束循环
					}
				}
			}
		}
	} while (worker != NULL);

	// 如果找到了正确的WorkerW窗口，设置父窗口
	if (background == NULL) {
		HWND pPM = FindWindowA("Progman", "Program Manager");
		if (pPM != NULL) {
			qDebug() << "Find Program Manager";
			// 尝试找到SHELLDLL_DefView窗口
			HWND shelldlldefview = FindWindowExA(pPM, NULL, "SHELLDLL_DefView", NULL);
			if (shelldlldefview != NULL) {
				qDebug() << "Find SHELLDLL_DefView";
				// 检查SHELLDLL_DefView的父窗口是否为当前的WorkerW窗口
				HWND parent = GetParent(shelldlldefview);
				if (parent != NULL) {
					qDebug() << "Find SHELLDLL_DefView's Parent";
					if (parent == pPM) {
						qDebug() << "Right!";
						// 找到了正确的WorkerW窗口
						background = shelldlldefview;// 结束循环
					}
				}
			}
		}
	}


	if (background != NULL) {
		SetParent((HWND)winId(), background);
		SetWindowPos((HWND)winId(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		SetWindowPos((HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		SetFocus((HWND)winId());
	}
	else {
		qDebug() << "Unable to find proper Program manager,Inplacing failed";
	}
}