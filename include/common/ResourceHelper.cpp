#include "ResourceHelper.h"

#define MAX_MENU_TEXT_LEN	64

namespace ResourceHelper
{
	std::wstring LoadStringFromRC(HINSTANCE resourceInstance, UINT stringId)
	{
		TCHAR str[MAX_MENU_TEXT_LEN] = { 0 };
		int numCharacters =
			LoadString(resourceInstance, stringId, str, MAX_MENU_TEXT_LEN);

		if (numCharacters == 0) {
			return L"";
		}

		// TODO ���������������2��������
		return std::wstring(str, numCharacters);
	}
}