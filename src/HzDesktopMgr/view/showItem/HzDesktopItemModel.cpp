#include "HzDesktopItemModel_p.h"
//#include "HzDesktopItemModel.h"

HzDesktopItemDataMgr::~HzDesktopItemDataMgr()
{
}

HzDesktopItemModel::HzDesktopItemModel(QObject *parent)
	//: QAbstractItemModel(parent)
	: QStandardItemModel(parent)
	, hzd_ptr(new HzDesktopItemModelPrivate())
{
	hzd_ptr->hzq_ptr = this;
	
	hzd_func()->init();
}

HzDesktopItemModel::~HzDesktopItemModel()
{
}

QString HzDesktopItemModel::filePath(const QModelIndex& aindex) const
{
	const QModelIndex& bindex = hzd_func()->translateRealIndex(aindex);
	// 根据内部指针来确定是哪个子模型
	HzDesktopItemModelPrivate::ItemType itemType
		= static_cast<HzDesktopItemModelPrivate::ItemType>(bindex.internalId());

	switch (itemType) {
	case HzDesktopItemModelPrivate::SystemAppItem:
		return hzd_func()->getSystemAppInfoList().at(bindex.row()).clsidPath;
	case HzDesktopItemModelPrivate::NormalFileItem:
		return hzd_func()->getFileItemInfoList().at(bindex.row()).filePath();
	}

	return QString();
}

inline QString HzDesktopItemModel::fileName(const QModelIndex& aindex) const
{
	const QModelIndex& bindex = hzd_func()->translateRealIndex(aindex);
	// 根据内部指针来确定是哪个子模型
	HzDesktopItemModelPrivate::ItemType itemType
		= static_cast<HzDesktopItemModelPrivate::ItemType>(bindex.internalId());

	switch (itemType) {
	case HzDesktopItemModelPrivate::SystemAppItem:
		return hzd_func()->getSystemAppInfoList().at(bindex.row()).showName;
	case HzDesktopItemModelPrivate::NormalFileItem:
		const QFileInfo& fileInfo = hzd_func()->getFileItemInfoList().at(bindex.row());
		if (fileInfo.isShortcut()) {
			return fileInfo.baseName();
		}
		else {
			return fileInfo.fileName();
		}
	}

	return QString();
}

inline QIcon HzDesktopItemModel::fileIcon(const QModelIndex& aindex) const
{
	const QModelIndex& bindex = hzd_func()->translateRealIndex(aindex);
	// 根据内部指针来确定是哪个子模型
	HzDesktopItemModelPrivate::ItemType itemType
		= static_cast<HzDesktopItemModelPrivate::ItemType>(bindex.internalId());

	switch (itemType) {
	case HzDesktopItemModelPrivate::SystemAppItem:
		return hzd_func()->getSystemAppInfoList().at(bindex.row()).showIcon;
	case HzDesktopItemModelPrivate::NormalFileItem:
		const QFileInfo& fileInfo = hzd_func()->getFileItemInfoList().at(bindex.row());
		if (fileInfo.isShortcut()) {
			QFileInfo targetFileInfo(fileInfo.symLinkTarget());
			return QFileIconProvider().icon(targetFileInfo);
		}
		else {
			return QFileIconProvider().icon(fileInfo);
		}
	}

	return QIcon();
}

QModelIndex HzDesktopItemModel::index(int row, int column, const QModelIndex& parent /*= QModelIndex()*/) const
{
	return createIndex(row, column);
}

QModelIndex HzDesktopItemModel::parent(const QModelIndex& child) const
{
	// 桌面视图的项没有parent
	return QModelIndex();
}

int HzDesktopItemModel::rowCount(const QModelIndex& parent) const
{
	return hzd_func()->getSystemAppInfoList().size()
		+ hzd_func()->getFileItemInfoList().size();
}

int HzDesktopItemModel::columnCount(const QModelIndex& parent) const
{
	// QFileSystemModel有4列
	return HzDesktopItemModelPrivate::NumColumns;
}

QVariant HzDesktopItemModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	switch (role) {
	case Qt::EditRole:
	case Qt::DisplayRole:
		return fileName(index);
	case Qt::DecorationRole:
		return fileIcon(index);
	}

	return QVariant();
}

