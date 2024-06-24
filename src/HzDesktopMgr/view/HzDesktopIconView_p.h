#pragma once

#include <QObject>

#include "common/QtpqReimp.h"
#include "HzDesktopIconView.h"

struct QItemViewPaintPair {
	QRect rect;
	QModelIndex index;
};
typedef QVector<QItemViewPaintPair> QItemViewPaintPairs;

class HzDesktopIconViewPrivate : public HzDesktopPrivate
{
	Q_OBJECT

	HZQ_DECLARE_PUBLIC(HzDesktopIconView)

public:
	HzDesktopIconViewPrivate();
	~HzDesktopIconViewPrivate();

	void initDragCursors();

	QPixmap renderToPixmap(const QModelIndexList& indexes, QRect* r) const;

	QItemViewPaintPairs draggablePaintPairs(const QModelIndexList& indexes, QRect* r) const;

	void handleOpen();

	void handleCopy();

	void handleCut();

	void handlePaste();

	void handleDelete();

	void handleRename();

	void handleSelectAll();

	QMap<Qt::DropAction, QPixmap> m_dragCursorMap;
};
