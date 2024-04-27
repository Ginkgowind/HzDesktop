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
     *  unicode to utf8 ��Ҫ�����߻����ڴ� FreeStr()
     *  pcbChar - ת������ַ����ֽ��� in bytes
     */
    char* UnicodeToUtf8(IN LPCWSTR pwstr, OUT UINT* pcbChar = NULL);

    /*
     *  utf8 to unicode ��Ҫ�����߻����ڴ� FreeStr()
     *  pcbWideChar - ת������ַ����ֽ��� in bytes
     */
    WCHAR* Utf8ToUnicode(IN LPCSTR pstr, OUT UINT* pcbWideChar = NULL);

    /**
     * @brief
     * ת��Ϊխ�ַ���
     * @param wstr
     * @return std::string
     * @attention
     */
    std::string UnicodeToUTF8(const std::wstring& wstr);

    /**
     * @brief
     * ת��Ϊ���ַ���
     * @param str
     * @return std::wstring
     * @attention
     */
    std::wstring UTF8ToUnicode(const std::string& str);

    /*
     *    �����ɸ���ת�����ַ������ڴ�(char*)
     */
    void FreeStr(LPSTR& lpStrMem);

    /*
     *  �����ɸ���ת�����ַ������ڴ�(WCHAR*)
     */
    void FreeStr(LPWSTR& lpwStrMem);
}