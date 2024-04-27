#pragma once

#include <QListView>

#include "control/PathFilesMgr.h"

QT_BEGIN_NAMESPACE
class QSortFilterProxyModel;
class QStandardItemModel;
QT_END_NAMESPACE

enum MenuShowStyle
{
	HzStyle,
	Win10Style,
	Win11Style
};

class HzIconView  : public QListView
{
	Q_OBJECT

public:
	HzIconView(QWidget *parent);
	~HzIconView();

protected:
	void startDrag(Qt::DropActions supportedActions) override;

	void contextMenuEvent(QContextMenuEvent* event) override;

private:
	// 后续将此类改为基类，分别实现桌面和格子
	void initFilesInfo();

	void initContextMenu();

private:
	QSortFilterProxyModel* m_itemProxyModel;
	QStandardItemModel* m_itemModel;
	MenuShowStyle m_menuShowStyle;
	QMenu* m_hzStyleMenu;
	PathFilesMgr* m_pathFilesMgr;
};
