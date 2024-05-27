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
		FilePathRole = Qt::UserRole + 1,
		FileNameRole = Qt::UserRole + 2,
		PosIndex2DRole = Qt::UserRole + 3
	};

	HzDesktopItemModel(QObject *parent);
	~HzDesktopItemModel();

public:
	void refreshItems();

public:
	QString filePath(const QModelIndex& index) const;

protected:

private:
	HZQ_DECLARE_PRIVATE(HzDesktopItemModel)
	Q_DISABLE_COPY(HzDesktopItemModel)
};