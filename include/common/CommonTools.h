#pragma once

#include <string>

#include "spdlog/spdlog.h"

#define HZDESKTOP_MUTEX_NAME               L"Global\\HzDesktop_Mutex_{F36BDF43-8995-4A3C-9F25-9FE9940538E1}"

namespace HZ
{
	bool isHzDesktopRunning();

	bool launchHzDesktop();
}