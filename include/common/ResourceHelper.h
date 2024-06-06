#pragma once

#include <windows.h>
#include <string>

namespace ResourceHelper
{

	std::string LoadString(HINSTANCE resourceInstance, UINT stringId);

}