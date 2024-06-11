#pragma once

#include <windows.h>
#include <string>

namespace ResourceHelper
{

	std::wstring LoadStringFromRC(HINSTANCE resourceInstance, UINT stringId);

}