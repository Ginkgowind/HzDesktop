#pragma once

#include <QObject>

#include "HzItemMenu.h"
#include "common/QtpqReimp.h"


class HzDesktopBlankMenuPrivate : public HzDesktopPrivate
{
	Q_OBJECT

	HZQ_DECLARE_PUBLIC(HzDesktopBlankMenu)

public:
	HzDesktopBlankMenuPrivate();
	~HzDesktopBlankMenuPrivate();

	QList<QAction*> getBackgroundShellActions();

	QList<QAction*> getBackgroundShellExActions();

	QList<QAction*> getDesktopBackgroundActions();
};
