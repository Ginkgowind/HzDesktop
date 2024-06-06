#include "ResourceHelper.h"

#define MAX_MENU_TEXT_LEN	64

namespace ResourceHelper
{
	std::string LoadString(HINSTANCE resourceInstance, UINT stringId)
	{
		CHAR str[MAX_MENU_TEXT_LEN] = { 0 };
		int numCharacters =
			LoadString(resourceInstance, stringId, str, MAX_MENU_TEXT_LEN);

		if (numCharacters == 0) {
			return "";
		}

		// TODO ���������������2��������
		return std::string(str, numCharacters);
	}
}