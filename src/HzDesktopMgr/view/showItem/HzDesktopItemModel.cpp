#include <QDebug>
#include <QDateTime>
#include <QUrl>
#include <QTimer>
#include <QMimeData>

#include <Windows.h>
#include <shlwapi.h>
#include <shlobj_core.h>
#include <shobjidl_core.h>
#include <wil/resource.h>

#include "HzDesktopItemModel.h"
#include "HzDesktopItemModel_p.h"
#include "config/HzDesktopParam.h"
#include "windows/UiOperation.h"

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

	clear();

	// ��ʱ���ٶ�ȡ
	QTimer::singleShot(10, this, [d]() {
		d->m_systemItemWatcher.refreshSystemAppsInfo();
		d->m_fileItemWatcher.refreshFileItem();
	});
}

void HzDesktopItemModel::handleExternalDrop(const QString& oriFilePath, QStandardItem* item)
{
	SHFILEINFOW shFileInfo = { 0 };
	SHGetFileInfoW(oriFilePath.toStdWString().c_str(), 0, &shFileInfo, sizeof(SHFILEINFO),
		SHGFI_DISPLAYNAME | SHGFI_USEFILEATTRIBUTES);

	QIcon itemIcon = HZ::getUltimateIcon(oriFilePath);
	QString displayName = QFileInfo(QString::fromStdWString(shFileInfo.szDisplayName)).fileName();

	item->setIcon(itemIcon);
	item->setText(displayName);
}

void HzDesktopItemModel::insertItems(int row, const QList<QStandardItem*>& items)
{
	// �Ӳ���λ�ÿ�ʼ�����������뼸��Ԫ�ؾ�ɾ������ռλԪ��
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

	// ���rowCount�������Ͳ���ռλԪ��
	for (int i = rowCount(); i < row; i++) {
		QStandardItem* item = new QStandardItem();
		item->setEnabled(false);
		appendRow(item);
	}

	// ��row��ʼ����Ԫ��
	for (const auto& item : items) {
		insertRow(row++, item);
	}
}

QString HzDesktopItemModel::name(const QModelIndex& index) const
{
	return data(index, Qt::DisplayRole).toString();
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

bool HzDesktopItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		QString oldPath = filePath(index);
		ItemHelper::setDisplayName(oldPath, value.toString());
		removePixmapCache(oldPath);
	}

	return QStandardItemModel::setData(index, value, role);
}
