#include <QDebug>

#include "HzDesktopItemModel.h"
#include "HzDesktopItemModel_p.h"

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

void HzDesktopItemModel::refreshItems()
{
	qDebug() << "model refresh";
}

void HzDesktopItemModel::insertItems(int row, const QList<QStandardItem*>& items)
{
	// �Ӳ���λ�ÿ�ʼ�����������뼸��Ԫ�ؾ�ɾ������ռλԪ��
	int insertCnt = items.size();
	for (int i = row; i < rowCount(); ) {
		int test = rowCount();
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

QString HzDesktopItemModel::filePath(const QModelIndex& index) const
{
	// TODO ���ʹ��proxymodel����Ӱ��˴�data��index->model()==this���ж�
	return data(index, FilePathRole).toString();
}
