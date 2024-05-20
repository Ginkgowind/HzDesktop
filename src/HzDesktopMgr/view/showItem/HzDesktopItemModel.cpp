#include <QDebug>

#include "HzDesktopItemModel.h"
#include "HzDesktopItemModel_p.h"

HzDesktopItemModel::HzDesktopItemModel(QObject *parent)
	: QStandardItemModel(parent)
	, hzd_ptr(new HzDesktopItemModelPrivate())
{
	hzd_ptr->hzq_ptr = this;
	
	hzd_func()->init();
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
