#include <shlwapi.h>

#include "FileUtils.h"
#include "common/CommonTools.h"

namespace HZ
{
	bool deleteFile(const std::wstring& strFilePath, bool bIsDir, bool bRemove, bool bIsShowUI)
	{
        /////> 文件删除和文件夹删除都可以用SHFileOperation
        //FILEOP_FLAGS dwOpFlags = 0;
        //if (!bRemove)
        //{
        //    ///> 转移到回收站
        //    dwOpFlags = FOF_ALLOWUNDO;
        //}

        //if (!bIsShowUI)
        //{
        //    ///> 如果要用静默执行操作，需要使用FOF_NO_UI
        //    dwOpFlags |= FOF_NO_UI;
        //}

        //WCHAR szDelDir[SUPPORT_MAX_PATH_LEN + 2] = { 0 };
        //::StringCchCopyNW(szDelDir, SUPPORT_MAX_PATH_LEN, strFilePath.c_str(), strFilePath.length());

        //SHFILEOPSTRUCTW fileOp = { 0 };
        //fileOp.hwnd = NULL;
        //fileOp.wFunc = FO_DELETE; ///> 文件删除操作
        //fileOp.pFrom = szDelDir;
        //fileOp.pTo = NULL;
        //fileOp.fFlags = dwOpFlags;
        //fileOp.hNameMappings = NULL;
        //fileOp.lpszProgressTitle = L"文件删除";

        //int nResult = ::SHFileOperation(&fileOp);
        //if (0 != nResult)
        //{
        //    LOG_ERROR_W("SHFileOperation failed, filepath: %s, ret:%d", szDelDir, nResult);
        //    return false;
        //}

        return true;
	}

	bool copyFile(const QString& oldFilePath, const QString& newFilePath, bool bIsShowUI)
	{
        FILEOP_FLAGS dwOpFlags = 0;
        if (!bIsShowUI) {
            ///> 如果要用静默执行操作，需要使用FOF_NO_UI
            dwOpFlags |= FOF_NO_UI;
        }

        std::wstring wstrOldFilePath = QString(oldFilePath).replace('/', '\\').toStdWString();
        std::wstring wstrNewFilePath = QString(newFilePath).replace('/', '\\').toStdWString();

        SHFILEOPSTRUCTW fileOp = { 0 };
        fileOp.hwnd = NULL;
        fileOp.wFunc = FO_COPY; ///> 文件复制操作
        fileOp.pFrom = wstrOldFilePath.c_str();
        fileOp.pTo = wstrNewFilePath.c_str();
        fileOp.hNameMappings = NULL;
        fileOp.lpszProgressTitle = L"hz copy file";
        fileOp.fFlags = dwOpFlags;

        //开始拷贝
        int nResult = SHFileOperation(&fileOp);
        if (0 != nResult && !fileOp.fAnyOperationsAborted) {
            //LOG_ERROR_W("SHFileOperation failed, strOldFilePath: %s,strNewFilePath: %s", strOldFilePath.c_str(), strNewFilePath.c_str());
            return false;
        }

        return true;
	}

	bool moveFile(const QString& oriFilePath, const QString& newFilePath, bool bIsShowUI)
	{
        FILEOP_FLAGS dwOpFlags = 0;
        if (!bIsShowUI) {
            dwOpFlags = FOF_NO_UI;
        }

        std::wstring wstrOriFilePath = QString(oriFilePath).replace('/', '\\').toStdWString();
        std::wstring wstrNewFilePath = QString(newFilePath).replace('/', '\\').toStdWString();

        SHFILEOPSTRUCTW fileOp = { 0 };
        fileOp.hwnd = NULL;
        fileOp.wFunc = FO_MOVE; ///> 文件重命名操作
        fileOp.pFrom = wstrOriFilePath.c_str();
        fileOp.pTo = wstrNewFilePath.c_str();
        fileOp.hNameMappings = NULL;
        fileOp.lpszProgressTitle = L"hz move file";
        fileOp.fFlags = dwOpFlags;

        int nResult = ::SHFileOperationW(&fileOp);
        // 函数调用失败，并且不是用户取消操作，才返回失败
        if (0 != nResult && !fileOp.fAnyOperationsAborted) {
            //LOG_ERROR_W("move failed, origin filepath: %s, new file path:%s, ret:%d",
            //    szOriFile, szNewFile, nResult);
            return false;
        }

        return true;
	}

	bool renameFile(const std::wstring& strOldFileName, const std::wstring& strNewFileName, bool bIsShowUI)
	{
		return false;
	}
}