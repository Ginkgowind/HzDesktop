#pragma once

#include <QListView>
#include <QMimeData>
#include <memory>

#include "showItem/HzDesktopItemModel.h"

QT_BEGIN_NAMESPACE
class QSortFilterProxyModel;
class QStandardItemModel;
class QFileSystemModel;
QT_END_NAMESPACE

enum MenuShowStyle
{
	HzStyle,
	Win10Style,
	Win11Style
};

class HzDesktopIconView  
	: public QListView
	, public std::enable_shared_from_this<HzDesktopIconView>
{
	Q_OBJECT

public:
	HzDesktopIconView(QWidget *parent);
	~HzDesktopIconView();

protected:
	void startDrag(Qt::DropActions supportedActions) override;

	void dragEnterEvent(QDragEnterEvent* event) override;

	//void dropEvent(QDropEvent* e) override;

	void contextMenuEvent(QContextMenuEvent* event) override;

private:
	QStringList getSelectedPaths();

private:
	QSortFilterProxyModel* m_itemProxyModel;
	HzDesktopItemModel* m_itemModel;
	MenuShowStyle m_menuShowStyle;
};
