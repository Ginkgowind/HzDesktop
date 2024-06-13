#pragma once
/* 自定义model实现对两个桌面目录以及系统图标的监控 */
#include <QStandardItemModel>

#include "common/QtpqReimp.h"

class HzDesktopParam;
class HzDesktopItemModel;
class HzDesktopItemModelPrivate;

class HzDesktopItemModel
	: public QStandardItemModel
	, public HzDesktopPublic
{
	Q_OBJECT

public:
	
	HzDesktopItemModel(QObject *parent, HzDesktopParam* param);
	~HzDesktopItemModel();

	void sortItemsLayout();

	void removeAllDisableItem();

	void refreshItems();

	// 会自动消除占位符的插入函数
	void insertItems(int row, const QList<QStandardItem*>& items);

public:

	QString name(const QModelIndex& index) const;
	qint64 size(const QModelIndex& index) const;
	QString type(const QModelIndex& index) const;
	QDateTime lastModified(const QModelIndex& index) const;
	QString filePath(const QModelIndex& index) const;

private:
	HzDesktopParam* m_param;

private:
	HZQ_DECLARE_PRIVATE(HzDesktopItemModel)
	Q_DISABLE_COPY(HzDesktopItemModel)
};