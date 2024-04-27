#include <shlwapi.h>

#include "FileInfoGather.h"
#include "common/StringConv.h"
#include "common/CommonTools.h"

namespace HZ
{
	FileInfo::FileInfo(
		DWORD dwFileAttributes, FILETIME ftCreationTime, FILETIME ftLastAccessTime,
		FILETIME ftLastWriteTime, DWORD nFileSizeHigh, DWORD nFileSizeLow,
		const std::wstring& strFileName, const std::wstring& strFileAbsName) :
		m_dwFileAttributes(dwFileAttributes),
		m_ftCreationTime(ftCreationTime),
		m_ftLastAccessTime(ftLastAccessTime),
		m_ftLastWriteTime(ftLastWriteTime),
		m_nFileSizeHigh(nFileSizeHigh),
		m_nFileSizeLow(nFileSizeLow),
		m_strFileName(strFileName),
		m_strFileAbsName(strFileAbsName),
		m_strSuffixName(GetFileSuffix(strFileName))
	{

	}

	bool FileInfoGather::GatherFilesInfo(
		const std::wstring& strGatherDirPath, 
		std::vector<FileInfo>& fileInfoList, 
		const std::wstring& strGatherFileName,
		bool bRecursionGather,
		bool bNeedFilterDirectory
	)
	{
		bool bRet = false;

		do
		{
			WIN32_FIND_DATAW wFindData = { 0 };
			std::wstring strGatherFile;

			if (strGatherDirPath.empty() || strGatherFileName.empty())
			{
				//LOG_ERROR_W("Input param is empty.");
				break;
			}
			///> 此时开始遍历目录查找文件，即使没有查找到也应该返回成功，只是返回的是一个空列表
			///> 返回失败在这个场景会出问题：无痕模式删除面板最后一个快捷方式后，由于没有快捷方式了导致此函数返回失败，从而没有清除个人域数据库
			bRet = true;
			strGatherFile = AppendPathConstW(strGatherDirPath, strGatherFileName);

			//> 开始遍历目录下的指定文件
			HANDLE hFile = ::FindFirstFileW(strGatherFile.c_str(), &wFindData);
			if (INVALID_HANDLE_VALUE == hFile)
			{
				//LOG_WARN_W("FindFirstFile path:%s failed. err=%d", strGatherFile.c_str(), ::GetLastError());
				break;
			}

			do
			{
				//> 过滤 . 和 .. 目录
				if (0 == lstrcmpW(wFindData.cFileName, L".") ||
					0 == lstrcmpW(wFindData.cFileName, L".."))
				{
					continue;
				}

				if (wcsicmp(wFindData.cFileName, L"$Recycle.bin") == 0)
				{
					continue;
				}
				//> 当前文件属性是文件夹，根据需要决定是否递归遍历
				if ((wFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					if (bRecursionGather)
					{
						//> 递归遍历子文件夹文件夹里的文件拷贝到目的文件夹
						std::wstring strNewDir = AppendPathConstW(strGatherDirPath, wFindData.cFileName);
						(void)GatherFilesInfo(strNewDir, fileInfoList, strGatherFileName, bRecursionGather, bNeedFilterDirectory);
					}

					if (bNeedFilterDirectory)
					{
						continue;
					}
				}

				fileInfoList.emplace_back(
					FileInfo(wFindData.dwFileAttributes, wFindData.ftCreationTime,
						wFindData.ftLastAccessTime, wFindData.ftLastWriteTime,
						wFindData.nFileSizeHigh, wFindData.nFileSizeLow,
						wFindData.cFileName, AppendPathConstW(strGatherDirPath, wFindData.cFileName)));
			} while (::FindNextFileW(hFile, &wFindData));

			if (INVALID_HANDLE_VALUE != hFile)
			{
				::FindClose(hFile);
				hFile = INVALID_HANDLE_VALUE;
			}
		} while (false);

		return bRet;
	}

	std::wstring GetFileSuffix(const std::wstring& strFileName)
	{
		std::wstring strSuffixName;

		if (!strFileName.empty())
		{
			size_t index = strFileName.find_last_of(L'.');
			if (index != strFileName.npos)
			{
				strSuffixName = strFileName.substr(index);
			}
		}

		HZ::string_tolower(strSuffixName);
		return strSuffixName;
	}

	bool IsDirectory(const std::wstring& strFilePath)
	{
		bool bRet = false;
		DWORD dwFileAttr = INVALID_FILE_ATTRIBUTES;

		do
		{
			if (strFilePath.empty() || !::PathFileExistsW(strFilePath.c_str()))
			{
				//LOG_ERROR_W("Input file path is invliad or not exist.");
				break;
			}

			dwFileAttr = ::GetFileAttributesW(strFilePath.c_str());

			if (INVALID_FILE_ATTRIBUTES == dwFileAttr)
			{
				//LOG_ERROR_W("Get file attribute failed. File path:%s, error:%d",
				//	strFilePath.c_str(), ::GetLastError());
				break;
			}

			bRet = (dwFileAttr & FILE_ATTRIBUTE_DIRECTORY);
		} while (false);

		return bRet;
	}
}