#include "HzItemSortProxyModel.h"
#include "HzDesktopItemModel.h"
#include "config/HzDesktopParam.h"

HzItemSortProxyModel::HzItemSortProxyModel(QObject *parent)
	: QSortFilterProxyModel(parent)
{}

HzItemSortProxyModel::~HzItemSortProxyModel()
{}

//bool HzItemSortProxyModel::lessThan(const QModelIndex & source_left, const QModelIndex & source_right) const
//{
//	return QSortFilterProxyModel::lessThan(source_left, source_right);
//
//	//QVariant l = (source_left.model() ? source_left.model()->data(source_left, sortRole()) : QVariant());
//	//QVariant r = (source_right.model() ? source_right.model()->data(source_right, sortRole()) : QVariant());
//
//	//return l < r;
//}

QString HzItemSortProxyModel::name(const QModelIndex& index) const
{
	return data(index, CustomRoles::FileNameRole).toString();
}

qint64 HzItemSortProxyModel::size(const QModelIndex& index) const
{
	return data(index, CustomRoles::FileSizeRole).toLongLong();
}

QString HzItemSortProxyModel::type(const QModelIndex& index) const
{
	return data(index, CustomRoles::FileTypeRole).toString();
}

QDateTime HzItemSortProxyModel::lastModified(const QModelIndex& index) const
{
	return data(index, CustomRoles::FileLastModifiedRole).toDateTime();
}

QString HzItemSortProxyModel::filePath(const QModelIndex& index) const
{
	return data(index, CustomRoles::FilePathRole).toString();
}