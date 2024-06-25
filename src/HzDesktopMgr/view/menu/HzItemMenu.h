#pragma once

#include <QMenu>
#include <memory>
#include <shlwapi.h>
#include <ShlObj.h>
#include <wil/resource.h>
#include <wil/com.h>

#include "common/QtpqReimp.h"
#include "config/HzDesktopParam.h"

#define MIN_SHELL_MENU_ID 1
#define MAX_SHELL_MENU_ID 1000

class HzDesktopBlankMenuPrivate;

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

class HzDesktopBlankMenu 
	: public QMenu
	, public HzDesktopPublic
{
	Q_OBJECT

public:
	HzDesktopBlankMenu(QWidget* parent, HzDesktopParam* param);
	~HzDesktopBlankMenu();

	void showMenu();

	void handleCustomMenuItem(UINT cmd);

	void hideSortStatus();

private:
	void initViewSubMenu();

	void initSortSubMenu();

	inline void setItemSortRole(CustomRoles role);

	inline void setItemSortOrder(Qt::SortOrder order);

signals:
	void onSetIconSizeMode(IconSizeMode mode);

	void enableAutoArrange();

	void onHide();

	void onSetItemSortRole(CustomRoles role);

	void onSetItemSortOrder(Qt::SortOrder order);

	void refreshDesktop();

	void onNewFile();

private:

	wil::com_ptr_nothrow<IContextMenu> m_contextMenu;

	HzDesktopParam* m_param;

	bool m_showSortStatus;

private:
	HZQ_DECLARE_PRIVATE(HzDesktopBlankMenu)
	Q_DISABLE_COPY(HzDesktopBlankMenu)
};
