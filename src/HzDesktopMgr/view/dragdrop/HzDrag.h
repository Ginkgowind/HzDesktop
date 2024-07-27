#pragma once

#include <QObject>
#include <QPixmap>

#include "common/QtpqReimp.h"

class HzDragPrivate;

class HzDrag  
	: public QObject
	, public HzDesktopPublic
{
	Q_OBJECT

public:
	HzDrag(QObject *parent);
	~HzDrag();

	void setItemPaths(const QStringList pathList);

	void setPixmap(const QPixmap& pixmap);

	void setHotSpot(const QPoint& hotspot);

	Qt::DropAction exec(Qt::DropActions supportedActions);

	static QObject* source() {
		return s_source;
	}

private:
	QStringList m_pathList;

	QPixmap m_pixmap;

	QPoint m_hotSpot;

	static QObject* s_source;

private:
	HZQ_DECLARE_PRIVATE(HzDrag)
	Q_DISABLE_COPY(HzDrag)
};
