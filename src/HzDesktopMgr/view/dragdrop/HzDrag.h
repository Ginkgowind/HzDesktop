#pragma once

#include <QObject>
#include <QPixmap>

class HzDrag  : public QObject
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
};
