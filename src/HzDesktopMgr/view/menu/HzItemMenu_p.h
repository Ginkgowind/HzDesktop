#pragma once

#include <wil/resource.h>
#include <shobjidl_core.h>

#include <QObject>

#include "HzItemMenu.h"
#include "common/QtpqReimp.h"


class MenuHelper
{
public:

	static inline void appendMenuItem(HMENU menu, UINT id);

	static inline void insertMenuItem(HMENU menu, UINT id, UINT item);

	static inline void appendSeparator(HMENU menu);

	static inline void insertSeparator(HMENU menu, UINT item);

	static inline void addSubMenuItem(HMENU menu, UINT id, wil::unique_hmenu subMenu);
	
	static inline void insertSubMenuItem(HMENU menu, UINT id, wil::unique_hmenu subMenu, UINT item);

	static inline void CheckItem(HMENU hMenu, UINT itemID, bool bCheck);

	void executeActionFromContextMenu(const QString& path, const QStringList& childPaths, const QString& action);
};

class HzItemMenuPrivate : public HzDesktopPrivate
{
	Q_OBJECT

		HZQ_DECLARE_PUBLIC(HzItemMenu)

public:

};

class HzDesktopBkgMenuPrivate : public HzDesktopPrivate
{
	Q_OBJECT

	HZQ_DECLARE_PUBLIC(HzDesktopBkgMenu)

public:
	HzDesktopBkgMenuPrivate();
	~HzDesktopBkgMenuPrivate();

	void updateMenu(HMENU menu);


	LRESULT ParentWindowSubclass(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	wil::unique_hmenu buildViewMenu();

	wil::unique_hmenu buildSortMenu();

	QVector<UINT> getNewFileCmdsVec(HMENU hMenu);

	void executeActionFromContextMenu(const QString& path, const QString& action);
};
