#pragma once
/* �Զ���modelʵ�ֶ���������Ŀ¼�Լ�ϵͳͼ��ļ�� */
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
	: public QStandardItemModel
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