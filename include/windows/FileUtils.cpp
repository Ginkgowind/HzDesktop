#include <shlwapi.h>

#include "FileUtils.h"
#include "common/CommonTools.h"

namespace HZ
{
	bool deleteFile(const std::wstring& strFilePath, bool bIsDir, bool bRemove, bool bIsShowUI)
	{
        /////> �ļ�ɾ�����ļ���ɾ����������SHFileOperation
        //FILEOP_FLAGS dwOpFlags = 0;
        //if (!bRemove)
        //{
        //    ///> ת�Ƶ�����վ
        //    dwOpFlags = FOF_ALLOWUNDO;
        //}

        //if (!bIsShowUI)
        //{
        //    ///> ���Ҫ�þ�Ĭִ�в�������Ҫʹ��FOF_NO_UI
        //    dwOpFlags |= FOF_NO_UI;
        //}

        //WCHAR szDelDir[SUPPORT_MAX_PATH_LEN + 2] = { 0 };
        //::StringCchCopyNW(szDelDir, SUPPORT_MAX_PATH_LEN, strFilePath.c_str(), strFilePath.length());

        //SHFILEOPSTRUCTW fileOp = { 0 };
        //fileOp.hwnd = NULL;
        //fileOp.wFunc = FO_DELETE; ///> �ļ�ɾ������
        //fileOp.pFrom = szDelDir;
        //fileOp.pTo = NULL;
        //fileOp.fFlags = dwOpFlags;
        //fileOp.hNameMappings = NULL;
        //fileOp.lpszProgressTitle = L"�ļ�ɾ��";

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
            ///> ���Ҫ�þ�Ĭִ�в�������Ҫʹ��FOF_NO_UI
            dwOpFlags |= FOF_NO_UI;
        }

        std::wstring wstrOldFilePath = QString(oldFilePath).replace('/', '\\').toStdWString();
        std::wstring wstrNewFilePath = QString(newFilePath).replace('/', '\\').toStdWString();

        SHFILEOPSTRUCTW fileOp = { 0 };
        fileOp.hwnd = NULL;
        fileOp.wFunc = FO_COPY; ///> �ļ����Ʋ���
        fileOp.pFrom = wstrOldFilePath.c_str();
        fileOp.pTo = wstrNewFilePath.c_str();
        fileOp.hNameMappings = NULL;
        fileOp.lpszProgressTitle = L"hz copy file";
        fileOp.fFlags = dwOpFlags;

        //��ʼ����
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
        fileOp.wFunc = FO_MOVE; ///> �ļ�����������
        fileOp.pFrom = wstrOriFilePath.c_str();
        fileOp.pTo = wstrNewFilePath.c_str();
        fileOp.hNameMappings = NULL;
        fileOp.lpszProgressTitle = L"hz move file";
        fileOp.fFlags = dwOpFlags;

        int nResult = ::SHFileOperationW(&fileOp);
        // ��������ʧ�ܣ����Ҳ����û�ȡ���������ŷ���ʧ��
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