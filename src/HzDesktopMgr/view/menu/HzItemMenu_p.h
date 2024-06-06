#pragma once

#include <wil/resource.h>

#include <QObject>

#include "HzItemMenu.h"
#include "common/QtpqReimp.h"


class MenuHelper
{
public:

	static inline void appendMenuItem(HMENU menu, UINT id);

	static inline void insertMenuItem(HMENU menu, UINT id, UINT item);

	static inline void appendSeparator(HMENU menu);

	static inline void addSubMenuItem(HMENU menu, UINT id, wil::unique_hmenu subMenu);
	
	static inline void insertSubMenuItem(HMENU menu, UINT id, wil::unique_hmenu subMenu, UINT item);
};

class HzDesktopBlankMenuPrivate : public HzDesktopPrivate
{
	Q_OBJECT

	HZQ_DECLARE_PUBLIC(HzDesktopBlankMenu)

public:
	HzDesktopBlankMenuPrivate();
	~HzDesktopBlankMenuPrivate();

	void updateMenu(HMENU menu);

private:
	wil::unique_hmenu buildViewsMenu();
};
