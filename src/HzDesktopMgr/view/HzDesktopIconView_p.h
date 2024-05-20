#pragma once

#include <QObject>

#include "common/QtpqReimp.h"
#include "HzDesktopIconView.h"

class HzDesktopIconViewPrivate : public HzDesktopPrivate
{
	Q_OBJECT

public:
	HzDesktopIconViewPrivate();
	~HzDesktopIconViewPrivate();

private:
	friend class HzDesktopIconView;
};
