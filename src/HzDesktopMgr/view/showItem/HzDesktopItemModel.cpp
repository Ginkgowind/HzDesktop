#include <QDebug>
#include <QDateTime>
#include <QUrl>
#include <QMimeData>

#include <Windows.h>
#include <shlwapi.h>
#include <shlobj_core.h>
#include <shobjidl_core.h>
#include <wil/resource.h>

#include "HzDesktopItemModel.h"
#include "HzDesktopItemModel_p.h"
#include "config/HzDesktopParam.h"

HzDesktopItemModel::HzDesktopItemModel(QObject *parent, HzDesktopParam* param)
	: QStandardItemModel(parent)
	, m_param(param)
	, HzDesktopPublic(new HzDesktopItemModelPrivate())
{
	HZQ_D(HzDesktopItemModel);

	d->init();
}

HzDesktopItemModel::~HzDesktopItemModel()
{
}

void HzDesktopItemModel::sortItemsLayout()
{
	removeAllDisableItem();

	setSortRole(m_param->sortRole);

	if (m_param->bAutoArrange) {
		sort(0, m_param->sortOrder);
	}
}

void HzDesktopItemModel::removeAllDisableItem()
{
	for (int i = 0; i < rowCount(); ) {
		if (!item(i)->isEnabled()) {
			removeRow(i);
		}
		else {
			i++;
		}
	}
}

void HzDesktopItemModel::refreshItems()
{
	HZQ_D(HzDesktopItemModel);

	//for (int i = 0; i < rowCount(); ) {
	//	removeRow(i);
	//}

	//removeRows(0, rowCount());

	//d->m_systemItemWatcher.refreshSystemAppsInfo();

	//d->m_fileItemWatcher.refreshFileItem();
}

void HzDesktopItemModel::insertItems(int row, const QList<QStandardItem*>& items)
{
	// 从插入位置开始向后遍历，插入几个元素就删除几个占位元素
	int insertCnt = items.size();
	for (int i = row; i < rowCount(); ) {
		if (!item(i)->isEnabled()) {
			if (insertCnt > 0) {
				removeRow(i);
				insertCnt--;
			}

			if (insertCnt == 0) {
				break;
			}
		}
		else {
			i++;
		}
	}

	// 如果rowCount不够，就补充占位元素
	for (int i = rowCount(); i < row; i++) {
		QStandardItem* item = new QStandardItem();
		item->setEnabled(false);
		//item->setData(true, ItemIsPlaceholder);
		appendRow(item);
	}

	// 从row开始插入元素
	for (const auto& item : items) {
		insertRow(row++, item);
	}
}

QString HzDesktopItemModel::name(const QModelIndex& index) const
{
	return data(index, CustomRoles::FileNameRole).toString();
}

qint64 HzDesktopItemModel::size(const QModelIndex& index) const
{
	return data(index, CustomRoles::FileSizeRole).toLongLong();
}

QString HzDesktopItemModel::type(const QModelIndex& index) const
{
	return data(index, CustomRoles::FileTypeRole).toString();
}

QDateTime HzDesktopItemModel::lastModified(const QModelIndex& index) const
{
	return data(index, CustomRoles::FileLastModifiedRole).toDateTime();
}

QString HzDesktopItemModel::filePath(const QModelIndex& index) const
{
	return data(index, CustomRoles::FilePathRole).toString();
}

QStringList HzDesktopItemModel::mimeTypes() const
{
	return QStringList()
		<< QLatin1String("text/uri-list")
		<< QLatin1String("application/x-qt-windows-mime;value=\"Shell IDList Array\"");
}

QMimeData* HzDesktopItemModel::mimeData(const QModelIndexList& indexes) const
{
	typedef wil::unique_cotaskmem_ptr<LPITEMIDLIST> LPITEMIDLIST_PTR;

	QList<QUrl> urls;
	QByteArray idls;

	std::vector<LPITEMIDLIST_PTR> idvec;
	std::vector<LPCITEMIDLIST> idChildvec;

	for (auto it = indexes.begin(); it != indexes.end(); ++it) {
		urls << QUrl::fromLocalFile(filePath(*it));
	}

	QMimeData* mimeData = new QMimeData();
	mimeData->setUrls(urls);
	mimeData->setData(
		"application/x-qt-windows-mime;value=\"Shell IDList Array\"",
		"not empty");

	return mimeData;
}

bool HzDesktopItemModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
	return true;
}

Qt::DropActions HzDesktopItemModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}
