#pragma once

#include <QRasterWindow>
#include <QPainter>

#include <shlobj.h>

#include "HzDrag.h"
#include "windows/QWindowsComBase.h"

/*!
    \class QWindowsDragCursorWindow
    \brief A toplevel window showing the drag icon in case of touch drag.

    \sa QWindowsOleDropSource
    \internal
    \ingroup qt-lighthouse-win
*/

class QWindowsDragCursorWindow : public QRasterWindow
{
public:
    explicit QWindowsDragCursorWindow(QWindow* parent = nullptr);

    void setPixmap(const QPixmap& p);

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        painter.drawPixmap(0, 0, m_pixmap);
    }

private:
    QPixmap m_pixmap;
};

class QWindowsOleDropSource : public QWindowsComBase<IDropSource>
{
public:
    enum Mode {
        MouseDrag,
        TouchDrag // Mouse cursor suppressed, use window as cursor.
    };

    explicit QWindowsOleDropSource(HzDrag* drag);
    ~QWindowsOleDropSource() override;

    void createCursors();

    // IDropSource methods
    STDMETHOD(QueryContinueDrag)(BOOL fEscapePressed, DWORD grfKeyState);
    STDMETHOD(GiveFeedback)(DWORD dwEffect);

private:
    struct CursorEntry {
        CursorEntry() : cacheKey(0) {}
        CursorEntry(const QPixmap& p, qint64 cK, const CursorHandlePtr& c, const QPoint& h) :
            pixmap(p), cacheKey(cK), cursor(c), hotSpot(h) {}

        QPixmap pixmap;
        qint64 cacheKey; // Cache key of cursor
        CursorHandlePtr cursor;
        QPoint hotSpot;
    };

    typedef QMap<Qt::DropAction, CursorEntry> ActionCursorMap;

    Mode m_mode;
    QWindowsDrag* m_drag;
    QPointer<QWindow> m_windowUnderMouse;
    Qt::MouseButtons m_currentButtons;
    ActionCursorMap m_cursors;
    QWindowsDragCursorWindow* m_touchDragWindow;

#ifndef QT_NO_DEBUG_STREAM
    friend QDebug operator<<(QDebug, const QWindowsOleDropSource::CursorEntry&);
#endif
};