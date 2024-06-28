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

	void handleDelete();

	void handleRename();

	void handleSelectAll();

	// TODO 目前测试是menu先触发，如果后续发现时序不一定时需要做修改
	void handleMenuNewFile();
	void handleFileCreated(const QModelIndex& index);

	QMap<Qt::DropAction, QPixmap> m_dragCursorMap;

	bool m_bNewFileByMenu;
};
