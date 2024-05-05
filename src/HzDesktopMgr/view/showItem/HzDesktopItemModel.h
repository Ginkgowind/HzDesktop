#pragma once
/* �Զ���modelʵ�ֶ���������Ŀ¼�Լ�ϵͳͼ��ļ�� */
//#include <QAbstractItemModel>
#include <QStandardItemModel>

class HzDesktopItemModel;
class HzDesktopItemModelPrivate;

/*
 * QtԴ�����p��qָ�룬��QObject�Ĺ��캯������Ҫ��
 * QObjectPrivate�����࣬�����಻�ǹ����ģ��ʴ˴��Լ�ʵ��һ��
 */
class HzDesktopItemDataMgr : public QObject
{
	Q_DISABLE_COPY(HzDesktopItemDataMgr)
public:
	HzDesktopItemDataMgr() = default;
	virtual ~HzDesktopItemDataMgr() = 0;
	HzDesktopItemModel* hzq_ptr;
};

class HzDesktopItemModel  
	//: public QAbstractItemModel
	: public QStandardItemModel
{
	Q_OBJECT

public:
	HzDesktopItemModel(QObject *parent);
	~HzDesktopItemModel();

public:
	QString filePath(const QModelIndex& index) const;
	inline QString fileName(const QModelIndex& index) const;
	inline QIcon fileIcon(const QModelIndex& index) const;

protected:

	virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

	virtual QModelIndex parent(const QModelIndex& child) const override;

	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;

	virtual QVariant data(const QModelIndex& index, 
		int role = Qt::DisplayRole) const override;

private:
	QScopedPointer<HzDesktopItemDataMgr> hzd_ptr;
	Q_DISABLE_COPY(HzDesktopItemModel)

	inline HzDesktopItemModelPrivate* hzd_func() {
		return reinterpret_cast<HzDesktopItemModelPrivate*>(qGetPtrHelper(hzd_ptr));
	}
	inline const HzDesktopItemModelPrivate* hzd_func() const {
		return reinterpret_cast<HzDesktopItemModelPrivate*>(qGetPtrHelper(hzd_ptr));
	}
	friend class HzDesktopItemModelPrivate;

};