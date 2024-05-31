#pragma once

#include <QSortFilterProxyModel>

class HzItemSortProxyModel  : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	HzItemSortProxyModel(QObject *parent);
	~HzItemSortProxyModel();

	QString name(const QModelIndex& index) const;
	qint64 size(const QModelIndex& index) const;
	QString type(const QModelIndex& index) const;
	QDateTime lastModified(const QModelIndex& index) const;
	QString filePath(const QModelIndex& index) const;

protected:
	//virtual bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const override;
};
