#include <QDebug>
#include <QDateTime>

#include "HzDesktopItemModel.h"
#include "HzDesktopItemModel_p.h"
#include "config/HzDesktopParam.h"

HzDesktopItemModel::HzDesktopItemModel(QObject *parent)
	: QStandardItemModel(parent)
	, HzDesktopPublic(new HzDesktopItemModelPrivate())
{
	HZQ_D(HzDesktopItemModel);

	d->init();
}

HzDesktopItemModel::~HzDesktopItemModel()
{
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

void HzDesktopItemModel::refreshItems()
{
	qDebug() << "model refresh";
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
		//item->setData(true, ItemIsPlaceholder);
		appendRow(item);
	}

	// ��row��ʼ����Ԫ��
	for (const auto& item : items) {
		insertRow(row++, item);
	}
}
