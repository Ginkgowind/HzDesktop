#include <QDebug>

#include "HzDrag_p.h"

QWindowsDragCursorWindow::QWindowsDragCursorWindow(QWindow* parent)
    : QRasterWindow(parent)
{
    QSurfaceFormat windowFormat = format();
    windowFormat.setAlphaBufferSize(8);
    setFormat(windowFormat);
    setObjectName(QStringLiteral("QWindowsDragCursorWindow"));
    setFlags(Qt::Popup | Qt::NoDropShadowWindowHint
        | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint
        | Qt::WindowDoesNotAcceptFocus | Qt::WindowTransparentForInput);
}

void QWindowsDragCursorWindow::setPixmap(const QPixmap& p)
{
    if (p.cacheKey() == m_pixmap.cacheKey())
        return;
    const QSize oldSize = m_pixmap.size();
    QSize newSize = p.size();
    //qCDebug(lcQpaMime) << __FUNCTION__ << p.cacheKey() << newSize;
    m_pixmap = p;
    if (oldSize != newSize) {
        const qreal pixDevicePixelRatio = p.devicePixelRatio();
        if (pixDevicePixelRatio > 1.0 && qFuzzyCompare(pixDevicePixelRatio, devicePixelRatio()))
            newSize /= qRound(pixDevicePixelRatio);
        resize(newSize);
    }
    if (isVisible())
        update();
}

QWindowsOleDropSource::QWindowsOleDropSource(QWindowsDrag* drag)
    : m_mode(QWindowsCursor::cursorState() != QWindowsCursor::State::Suppressed ? MouseDrag : TouchDrag)
    , m_drag(drag)
    , m_windowUnderMouse(QWindowsContext::instance()->windowUnderMouse())
    , m_currentButtons(Qt::NoButton)
    , m_touchDragWindow(nullptr)
{
    //qCDebug(lcQpaMime) << __FUNCTION__ << m_mode;
}

QWindowsOleDropSource::~QWindowsOleDropSource()
{
    m_cursors.clear();
    delete m_touchDragWindow;
    //qCDebug(lcQpaMime) << __FUNCTION__;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug d, const QWindowsOleDropSource::CursorEntry& e)
{
    d << "CursorEntry:" << e.pixmap.size() << '#' << e.cacheKey
        << "HCURSOR" << e.cursor->handle() << "hotspot:" << e.hotSpot;
    return d;
}
#endif // !QT_NO_DEBUG_STREAM

/*!
    \brief Blend custom pixmap with cursors.
*/

void QWindowsOleDropSource::createCursors()
{
    const QDrag* drag = m_drag->currentDrag();
    const QPixmap pixmap = drag->pixmap();
    const bool hasPixmap = !pixmap.isNull();

    // Find screen for drag. Could be obtained from QDrag::source(), but that might be a QWidget.
    const QPlatformScreen* platformScreen = QWindowsContext::instance()->screenManager().screenAtDp(QWindowsCursor::mousePosition());
    if (!platformScreen) {
        if (const QScreen* primaryScreen = QGuiApplication::primaryScreen())
            platformScreen = primaryScreen->handle();
    }
    Q_ASSERT(platformScreen);
    QPlatformCursor* platformCursor = platformScreen->cursor();

    if (GetSystemMetrics(SM_REMOTESESSION) != 0) {
        /* Workaround for RDP issues with large cursors.
         * Touch drag window seems to work just fine...
         * 96 pixel is a 'large' mouse cursor, according to RDP spec */
        const int rdpLargeCursor = qRound(qreal(96) / QHighDpiScaling::factor(platformScreen));
        if (pixmap.width() > rdpLargeCursor || pixmap.height() > rdpLargeCursor)
            m_mode = TouchDrag;
    }

    qreal pixmapScaleFactor = 1;
    qreal hotSpotScaleFactor = 1;
    if (m_mode != TouchDrag) { // Touch drag: pixmap is shown in a separate QWindow, which will be scaled.)
        hotSpotScaleFactor = QHighDpiScaling::factor(platformScreen);
        pixmapScaleFactor = hotSpotScaleFactor / pixmap.devicePixelRatio();
    }
    QPixmap scaledPixmap = qFuzzyCompare(pixmapScaleFactor, 1.0)
        ? pixmap
        : pixmap.scaled((QSizeF(pixmap.size()) * pixmapScaleFactor).toSize(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);
    scaledPixmap.setDevicePixelRatio(1);

    Qt::DropAction actions[] = { Qt::MoveAction, Qt::CopyAction, Qt::LinkAction, Qt::IgnoreAction };
    int actionCount = int(sizeof(actions) / sizeof(actions[0]));
    if (!hasPixmap)
        --actionCount; // No Qt::IgnoreAction unless pixmap
    const QPoint hotSpot = qFuzzyCompare(hotSpotScaleFactor, 1.0)
        ? drag->hotSpot()
        : (QPointF(drag->hotSpot()) * hotSpotScaleFactor).toPoint();
    for (int cnum = 0; cnum < actionCount; ++cnum) {
        const Qt::DropAction action = actions[cnum];
        QPixmap cursorPixmap = drag->dragCursor(action);
        if (cursorPixmap.isNull() && platformCursor)
            cursorPixmap = static_cast<QWindowsCursor*>(platformCursor)->dragDefaultCursor(action);
        const qint64 cacheKey = cursorPixmap.cacheKey();
        const auto it = m_cursors.find(action);
        if (it != m_cursors.end() && it.value().cacheKey == cacheKey)
            continue;
        if (cursorPixmap.isNull()) {
            qWarning("%s: Unable to obtain drag cursor for %d.", __FUNCTION__, action);
            continue;
        }

        QPoint newHotSpot(0, 0);
        QPixmap newPixmap = cursorPixmap;

        if (hasPixmap) {
            const int x1 = qMin(-hotSpot.x(), 0);
            const int x2 = qMax(scaledPixmap.width() - hotSpot.x(), cursorPixmap.width());
            const int y1 = qMin(-hotSpot.y(), 0);
            const int y2 = qMax(scaledPixmap.height() - hotSpot.y(), cursorPixmap.height());
            QPixmap newCursor(x2 - x1 + 1, y2 - y1 + 1);
            newCursor.fill(Qt::transparent);
            QPainter p(&newCursor);
            const QPoint pmDest = QPoint(qMax(0, -hotSpot.x()), qMax(0, -hotSpot.y()));
            p.drawPixmap(pmDest, scaledPixmap);
            p.drawPixmap(qMax(0, hotSpot.x()), qMax(0, hotSpot.y()), cursorPixmap);
            newPixmap = newCursor;
            newHotSpot = QPoint(qMax(0, hotSpot.x()), qMax(0, hotSpot.y()));
        }

        if (const HCURSOR sysCursor = QWindowsCursor::createPixmapCursor(newPixmap, newHotSpot)) {
            const CursorEntry entry(newPixmap, cacheKey, CursorHandlePtr(new CursorHandle(sysCursor)), newHotSpot);
            if (it == m_cursors.end())
                m_cursors.insert(action, entry);
            else
                it.value() = entry;
        }
    }
//#ifndef QT_NO_DEBUG_OUTPUT
//    if (lcQpaMime().isDebugEnabled())
//        qCDebug(lcQpaMime) << __FUNCTION__ << "pixmap" << pixmap.size() << m_cursors.size() << "cursors:\n" << m_cursors;
//#endif // !QT_NO_DEBUG_OUTPUT
}

/*!
    \brief Check for cancel.
*/

QT_ENSURE_STACK_ALIGNED_FOR_SSE STDMETHODIMP
QWindowsOleDropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
{
    Qt::MouseButtons buttons = toQtMouseButtons(grfKeyState);

    SCODE result = S_OK;
    if (fEscapePressed || QWindowsDrag::isCanceled()) {
        result = DRAGDROP_S_CANCEL;
        buttons = Qt::NoButton;
    }
    else {
        if (buttons && !m_currentButtons) {
            m_currentButtons = buttons;
        }
        else if (!(m_currentButtons & buttons)) { // Button changed: Complete Drop operation.
            result = DRAGDROP_S_DROP;
        }
    }

    switch (result) {
    case DRAGDROP_S_DROP:
    case DRAGDROP_S_CANCEL:
        if (!m_windowUnderMouse.isNull() && m_mode != TouchDrag && fEscapePressed == FALSE
            && buttons != lastButtons) {
            // QTBUG 66447: Synthesize a mouse release to the window under mouse at
            // start of the DnD operation as Windows does not send any.
            const QPoint globalPos = QWindowsCursor::mousePosition();
            const QPoint localPos = m_windowUnderMouse->handle()->mapFromGlobal(globalPos);
            QWindowSystemInterface::handleMouseEvent(m_windowUnderMouse.data(),
                QPointF(localPos), QPointF(globalPos),
                QWindowsMouseHandler::queryMouseButtons(),
                Qt::LeftButton, QEvent::MouseButtonRelease);
        }
        m_currentButtons = Qt::NoButton;
        break;

    default:
        QGuiApplication::processEvents();
        break;
    }

    if (QWindowsContext::verbose > 1 || result != S_OK) {
        //qCDebug(lcQpaMime) << __FUNCTION__ << "fEscapePressed=" << fEscapePressed
        //    << "grfKeyState=" << grfKeyState << "buttons" << m_currentButtons
        //    << "returns 0x" << Qt::hex << int(result) << Qt::dec;
    }
    return ResultFromScode(result);
}

/*!
    \brief Give feedback: Change cursor accoding to action.
*/

QT_ENSURE_STACK_ALIGNED_FOR_SSE STDMETHODIMP
QWindowsOleDropSource::GiveFeedback(DWORD dwEffect)
{
    const Qt::DropAction action = translateToQDragDropAction(dwEffect);
    m_drag->updateAction(action);

    const qint64 currentCacheKey = m_drag->currentDrag()->dragCursor(action).cacheKey();
    auto it = m_cursors.constFind(action);
    // If a custom drag cursor is set, check its cache key to detect changes.
    if (it == m_cursors.constEnd() || (currentCacheKey && currentCacheKey != it.value().cacheKey)) {
        createCursors();
        it = m_cursors.constFind(action);
    }

    if (it != m_cursors.constEnd()) {
        const CursorEntry& e = it.value();
        switch (m_mode) {
        case MouseDrag:
            SetCursor(e.cursor->handle());
            break;
        case TouchDrag:
            // "Touch drag" with an unsuppressed cursor may happen with RDP (see createCursors())
            if (QWindowsCursor::cursorState() != QWindowsCursor::State::Suppressed)
                SetCursor(nullptr);
            if (!m_touchDragWindow)
                m_touchDragWindow = new QWindowsDragCursorWindow;
            m_touchDragWindow->setPixmap(e.pixmap);
            m_touchDragWindow->setFramePosition(QCursor::pos() - e.hotSpot);
            if (!m_touchDragWindow->isVisible())
                m_touchDragWindow->show();
            break;
        }
        return ResultFromScode(S_OK);
    }

    return ResultFromScode(DRAGDROP_S_USEDEFAULTCURSORS);
}