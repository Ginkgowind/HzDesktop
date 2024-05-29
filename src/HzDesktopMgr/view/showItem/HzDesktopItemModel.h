#pragma once
/* �Զ���modelʵ�ֶ���������Ŀ¼�Լ�ϵͳͼ��ļ�� */
#include <QStandardItemModel>

#include "common/QtpqReimp.h"

class HzDesktopItemModel;
class HzDesktopItemModelPrivate;

class HzDesktopItemModel
	: public QStandardItemModel
	, public HzDesktopPublic
{
	Q_OBJECT

public:
	enum Roles {
		FilePathRole = Qt::UserRole + 1
	};

	HzDesktopItemModel(QObject *parent);
	~HzDesktopItemModel();

public:
	void refreshItems();

	// ���Զ�����ռλ���Ĳ��뺯��
	void insertItems(int row, const QList<QStandardItem*>& items);

public:
	QString filePath(const QModelIndex& index) const;

protected:

private:
	HZQ_DECLARE_PRIVATE(HzDesktopItemModel)
	Q_DISABLE_COPY(HzDesktopItemModel)
};