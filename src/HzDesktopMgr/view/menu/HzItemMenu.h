#pragma once

#include <QMenu>
#include <memory>

#include "common/QtpqReimp.h"
#include "config/HzDesktopParam.h"

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

private:
	void initViewSubMenu();

	void initSortSubMenu();

signals:
	void onSetIconSizeMode(IconSizeMode mode);

	void onSetItemSortRole(CustomRoles role);

	void switchAutoArrangeStatus();

	void switchDoubleClickStatus();

	void onHide();

	void refreshDesktop();

private:

	HzDesktopParam* m_param;

private:
	HZQ_DECLARE_PRIVATE(HzDesktopBlankMenu)
	Q_DISABLE_COPY(HzDesktopBlankMenu)
};
