#pragma once

#include <string>
#include <QString>
#include <windows.h>

namespace HZ
{
    bool deleteFile(const std::wstring& strFilePath, bool bIsDir, bool bRemove = false, bool bIsShowUI = false);

    bool copyFile(const QString& strOldFilePath, const QString& strNewFilePath, bool bIsShowUI = false);

    bool moveFile(const QString& strOriFilePath, const QString& strNewFilePath, bool bIsShowUI = false);

    bool renameFile(const std::wstring& strOldFileName, const std::wstring& strNewFileName, bool bIsShowUI = false);
}