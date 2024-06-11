#include "HzItemSortProxyModel.h"
#include "HzDesktopItemModel.h"
#include "config/HzDesktopParam.h"

HzItemSortProxyModel::HzItemSortProxyModel(QObject *parent, HzDesktopParam* param)
	: QSortFilterProxyModel(parent)
	, m_param(param)
{
	connect(this, &QSortFilterProxyModel::sourceModelChanged, [this]() {
		HzDesktopItemModel* itemModel = qobject_cast<HzDesktopItemModel*>(sourceModel());
		if (!itemModel) {
			return;
		}

		itemModel->removeAllDisableItem();

		setSortRole(m_param->sortRole);
		setDynamicSortFilter(false);

		if (m_param->bAutoArrange) {
			sort(0, m_param->sortOrder);
		}
	});
}

HzItemSortProxyModel::~HzItemSortProxyModel()
{}

QModelIndexList HzItemSortProxyModel::mapListToSource(const QModelIndexList & proxyIndexList)
{
	QModelIndexList sourceIndexList;

	for (const QModelIndex& index : proxyIndexList) {
		sourceIndexList.push_back(mapToSource(index));
	}

	return sourceIndexList;
}

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