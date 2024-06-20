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

	Qt::DropAction exec(Qt::DropActions supportedActions);

private:
	QStringList m_pathList;

	QPixmap m_pixmap;
};
