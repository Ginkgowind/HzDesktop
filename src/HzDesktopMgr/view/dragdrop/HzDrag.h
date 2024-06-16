#pragma once

#include <QObject>

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
};
