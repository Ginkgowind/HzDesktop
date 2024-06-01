#pragma once

#include <QObject>

#include "common/QtpqReimp.h"
#include "HzDesktopIconView.h"

class HzDesktopIconViewPrivate : public HzDesktopPrivate
{
	Q_OBJECT

	HZQ_DECLARE_PUBLIC(HzDesktopIconView)

public:
	HzDesktopIconViewPrivate();
	~HzDesktopIconViewPrivate();

private:
	void handleOpen();

	void handleCopy();

	void handleCut();

	void handlePaste();

	void handleDelete();

	void handleRename();
};
