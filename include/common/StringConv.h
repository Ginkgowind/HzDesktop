#pragma once

#include <windows.h>
#include <string>
#include <algorithm>
#include <functional>
#include <iterator> 

namespace HZ
{
    inline void string_toupper(std::string& src)
    {
		std::transform(src.begin(), src.end(),
			std::back_insert_iterator<std::string>(src), ::toupper);
    }

    inline void string_tolower(std::string& src)
    {
		std::transform(src.begin(), src.end(),
			std::back_insert_iterator<std::string>(src), ::tolower);
    }


    inline void string_toupper(std::wstring& src)
    {
		std::transform(src.begin(), src.end(),
			std::back_insert_iterator<std::wstring>(src), ::toupper);
    }

    inline void string_tolower(std::wstring& src)
    {
		std::transform(src.begin(), src.end(),
			std::back_insert_iterator<std::wstring>(src), ::tolower);
    }


    /*
     *  unicode to utf8 需要调用者回收内存 FreeStr()
     *  pcbChar - 转换后的字符串字节数 in bytes
     */
    char* UnicodeToUtf8(IN LPCWSTR pwstr, OUT UINT* pcbChar = NULL);

    /*
     *  utf8 to unicode 需要调用者回收内存 FreeStr()
     *  pcbWideChar - 转换后的字符串字节数 in bytes
     */
    WCHAR* Utf8ToUnicode(IN LPCSTR pstr, OUT UINT* pcbWideChar = NULL);

    /**
     * @brief
     * 转换为窄字符串
     * @param wstr
     * @return std::string
     * @attention
     */
    std::string UnicodeToUTF8(const std::wstring& wstr);

    /**
     * @brief
     * 转换为宽字符串
     * @param str
     * @return std::wstring
     * @attention
     */
    std::wstring UTF8ToUnicode(const std::string& str);

    /*
     *    清理由该类转换的字符串的内存(char*)
     */
    void FreeStr(LPSTR& lpStrMem);

    /*
     *  清理由该类转换的字符串的内存(WCHAR*)
     */
    void FreeStr(LPWSTR& lpwStrMem);
}