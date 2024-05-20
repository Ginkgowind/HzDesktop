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
	// TODO ���ʹ��proxymodel����Ӱ��˴�data��index->model()==this���ж�
	return data(index, FilePathRole).toString();
}
