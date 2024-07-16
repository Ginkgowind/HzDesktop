#pragma once
/* �Զ���modelʵ�ֶ���������Ŀ¼�Լ�ϵͳͼ��ļ�� */
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

	void handleExternalDrop(const QModelIndex& index);

	// ���Զ�����ռλ���Ĳ��뺯��
	void insertItems(int row, const QList<QStandardItem*>& items);

	QString name(const QModelIndex& index) const;
	qint64 size(const QModelIndex& index) const;
	QString type(const QModelIndex& index) const;
	QDateTime lastModified(const QModelIndex& index) const;
	QString filePath(const QModelIndex& index) const;

protected:
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

	// ��Ϊû��ʹ��QDrag���������漸����������ʵ��
	//QStringList mimeTypes() const override;
	//QMimeData* mimeData(const QModelIndexList& indexes) const override;
	//bool dropMimeData(const QMimeData* data, Qt::DropAction action,
	//	int row, int column, const QModelIndex& parent) override;
	//Qt::DropActions supportedDropActions() const override;
	//return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;

signals:
	void onFileCreated(const QModelIndex& index);

private:
	HzDesktopParam* m_param;

private:
	HZQ_DECLARE_PRIVATE(HzDesktopItemModel)
	Q_DISABLE_COPY(HzDesktopItemModel)
};