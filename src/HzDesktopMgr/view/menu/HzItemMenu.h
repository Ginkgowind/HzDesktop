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

class HzItemMenuPrivate;
class HzDesktopBkgMenuPrivate;

class HzMenuBase : public QMenu
{
public:
	HzMenuBase(QWidget* parent);

public:
	LRESULT ParentWindowSubclass(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	wil::com_ptr_nothrow<IContextMenu> m_contextMenu;
};

class HzItemMenu
	: public HzMenuBase
	, public HzDesktopPublic
{
	Q_OBJECT

public:
	explicit HzItemMenu(QWidget* parent = nullptr);
	~HzItemMenu();

	static HzItemMenu& instance();

	void showMenu(const QStringList& itemList);

	// TODO 这些函数为什么不能设置为内联被外部调用？
	void handleCopy(const QStringList& itemList);

	void handleCut(const QStringList& itemList);

	void handleDelete(const QStringList& itemList);

	void handleCopyCommon(const QStringList& itemList);

	void handleCutCommon(const QStringList& itemList);

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

private:
	HZQ_DECLARE_PRIVATE(HzItemMenu)
	Q_DISABLE_COPY(HzItemMenu)
};

class HzDesktopBkgMenu 
	: public HzMenuBase
	, public HzDesktopPublic
{
	Q_OBJECT

public:
	HzDesktopBkgMenu(QWidget* parent, HzDesktopParam* param);
	~HzDesktopBkgMenu();

	void showMenu();

	void handleCustomMenuItem(UINT cmd);

	void hideSortStatus();

	inline void handlePaste();

	inline void handlePasteShortcut();

private:
	void initViewSubMenu();

	void initSortSubMenu();

	inline void setItemSortRole(int role);

	inline void setItemSortOrder(Qt::SortOrder order);

signals:
	void onSetIconSizeMode(IconSizeMode mode);

	void enableAutoArrange();

	void onHide();

	void onSetItemSortRole(int role);

	void onSetItemSortOrder(Qt::SortOrder order);

	void refreshDesktop();

	void onNewFile();

private:

	HzDesktopParam* m_param;

	bool m_showSortStatus;

private:
	HZQ_DECLARE_PRIVATE(HzDesktopBkgMenu)
	Q_DISABLE_COPY(HzDesktopBkgMenu)
};
