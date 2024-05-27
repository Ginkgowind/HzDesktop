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

QString HzDesktopItemModel::filePath(const QModelIndex& index) const
{
	// TODO 如果使用proxymodel，会影响此处data里index->model()==this的判断
	return data(index, FilePathRole).toString();
}
