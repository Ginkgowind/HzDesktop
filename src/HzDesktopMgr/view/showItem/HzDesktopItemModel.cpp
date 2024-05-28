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
	// 从插入位置开始向后遍历，插入几个元素就删除几个占位元素
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

QString HzDesktopItemModel::filePath(const QModelIndex& index) const
{
	// TODO 如果使用proxymodel，会影响此处data里index->model()==this的判断
	return data(index, FilePathRole).toString();
}
