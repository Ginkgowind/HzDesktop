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
	// �����������Ϊ���࣬�ֱ�ʵ������͸���
	void initFilesInfo();

	void initContextMenu();

private:
	QSortFilterProxyModel* m_itemProxyModel;
	QStandardItemModel* m_itemModel;
	MenuShowStyle m_menuShowStyle;
	QMenu* m_hzStyleMenu;
	PathFilesMgr* m_pathFilesMgr;
};
