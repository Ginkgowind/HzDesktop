#pragma once

#include <QMenu>
#include <memory>

#include "config/HzDesktopParam.h"

class HzItemMenu : public QMenu
{
	Q_OBJECT

public:
	explicit HzItemMenu(QWidget* parent = nullptr);
	~HzItemMenu();

	static HzItemMenu& instance();

	void showMenu(const QStringList& itemList);

signals:
	void onOpen();
	void onCopy();
	void onCut();
	void onDelete();
	void onRename();

private:
	void onExplorerMenu();

private:
	QAction* m_copyAct;
	QAction* m_cutAct;
	QAction* m_deleteAct;
	QAction* m_renameAct;

	QStringList m_selectedItemList;
	QPoint m_showPos;
};

class HzDesktopBlankMenu : public QMenu
{
	Q_OBJECT

public:
	explicit HzDesktopBlankMenu(QObject* parent);
	~HzDesktopBlankMenu();

signals:
	void onSetIconSizeMode(IconSizeMode mode);

	void onSetItemSortRole(CustomRoles role);

	void onHide();

	void refreshDesktopItemsSignal();

private:

	void switchAutoArrangeIcons();

	void setSortMode();

	void OnRefresh();
};
