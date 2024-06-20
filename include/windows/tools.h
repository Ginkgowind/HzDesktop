#pragma once

#include <windows.h>
#include <string>
#include <QString>
#include <QSysInfo>

namespace HZ
{
	bool isSystem64Bit();

	int getSystemVersion();

	QString getPublicDesktopPath();
}

