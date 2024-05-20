#pragma once
/* 自定义model实现对两个桌面目录以及系统图标的监控 */
#include <QStandardItemModel>

#include "common/QtpqReimp.h"

class HzDesktopItemModel;
class HzDesktopItemModelPrivate;

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
	QScopedPointer<HzDesktopPrivate> hzd_ptr;
	Q_DISABLE_COPY(HzDesktopItemModel)

	inline HzDesktopItemModelPrivate* hzd_func() {
		return reinterpret_cast<HzDesktopItemModelPrivate*>(qGetPtrHelper(hzd_ptr));
	}
	inline const HzDesktopItemModelPrivate* hzd_func() const {
		return reinterpret_cast<HzDesktopItemModelPrivate*>(qGetPtrHelper(hzd_ptr));
	}
	friend class HzDesktopItemModelPrivate;

};