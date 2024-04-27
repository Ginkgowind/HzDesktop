#include "StringConv.h"

namespace HZ
{
	char* UnicodeToUtf8(IN LPCWSTR pwstr, OUT UINT* pcbChar /*= NULL*/)
	{
		_ASSERT(pwstr != NULL);
		int cbSize = WideCharToMultiByte(CP_UTF8, NULL, pwstr, (int)wcslen(pwstr), NULL, 0, NULL, NULL);
		char* pstr = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cbSize + 1); // null terminated
		if (pstr)
		{
			WideCharToMultiByte(CP_UTF8, NULL, pwstr, (int)wcslen(pwstr), pstr, cbSize, NULL, NULL);
			pstr[cbSize] = 0;
			if (pcbChar)
			{
				*pcbChar = cbSize + 1;
			}
		}
		return pstr;
	}

	WCHAR* Utf8ToUnicode(IN LPCSTR pstr, OUT UINT* pcbWideChar /*= NULL*/)
	{
		_ASSERT(pstr != NULL);
		int nLen = MultiByteToWideChar(CP_UTF8, NULL, pstr, (int)strlen(pstr), NULL, 0);
		WCHAR* pwstr = (WCHAR*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (nLen + 1) * sizeof(WCHAR)); // null terminated
		if (pwstr)
		{
			MultiByteToWideChar(CP_UTF8, NULL, pstr, (int)strlen(pstr), pwstr, nLen);
			pwstr[nLen] = 0;
			if (pcbWideChar)
			{
				*pcbWideChar = (nLen + 1) * sizeof(WCHAR);
			}
		}
		return pwstr;
	}

	std::string UnicodeToUTF8(const std::wstring& wstr)
	{
		std::string strRet;
		LPSTR pszStr = NULL;
		do
		{
			if (wstr.empty())
			{
				break;
			}

			pszStr = UnicodeToUtf8(wstr.c_str());
			if (pszStr == NULL)
			{
				break;
			}

			strRet.assign(pszStr);
		} while (false);

		if (pszStr)
		{
			FreeStr(pszStr);
			pszStr = NULL;
		}

		return strRet;
	}

	std::wstring UTF8ToUnicode(const std::string& srcStr)
	{
		std::wstring strRet;
		LPWSTR pwszStr = NULL;
		do
		{
			if (srcStr.empty())
			{
				break;
			}

			pwszStr = Utf8ToUnicode(srcStr.c_str());
			if (pwszStr == NULL)
			{
				break;
			}

			strRet.assign(pwszStr);
		} while (false);

		if (pwszStr)
		{
			FreeStr(pwszStr);
			pwszStr = NULL;
		}

		return strRet;
	}

	void FreeStr(LPSTR& lpStrMem)
	{
		if (lpStrMem != NULL &&
			HeapFree(GetProcessHeap(), 0, lpStrMem))
		{
			lpStrMem = NULL;
		}
	}

	void FreeStr(LPWSTR& lpwStrMem)
	{
		if (lpwStrMem != NULL &&
			HeapFree(GetProcessHeap(), 0, lpwStrMem))
		{
			lpwStrMem = NULL;
		}
	}
}