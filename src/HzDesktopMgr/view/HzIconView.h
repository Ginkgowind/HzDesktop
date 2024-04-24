#pragma once

#include <QListView>

QT_BEGIN_NAMESPACE
class QSortFilterProxyModel;
class QStandardItemModel;
QT_END_NAMESPACE

class HzIconView  : public QListView
{
	Q_OBJECT

public:
	HzIconView(QWidget *parent);
	~HzIconView();

private:
	QSortFilterProxyModel* m_itemProxyModel;
	QStandardItemModel* m_itemModel;
};
