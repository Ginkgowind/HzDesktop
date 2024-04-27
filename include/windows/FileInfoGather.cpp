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
			///> ��ʱ��ʼ����Ŀ¼�����ļ�����ʹû�в��ҵ�ҲӦ�÷��سɹ���ֻ�Ƿ��ص���һ�����б�
			///> ����ʧ�����������������⣺�޺�ģʽɾ��������һ����ݷ�ʽ������û�п�ݷ�ʽ�˵��´˺�������ʧ�ܣ��Ӷ�û��������������ݿ�
			bRet = true;
			strGatherFile = AppendPathConstW(strGatherDirPath, strGatherFileName);

			//> ��ʼ����Ŀ¼�µ�ָ���ļ�
			HANDLE hFile = ::FindFirstFileW(strGatherFile.c_str(), &wFindData);
			if (INVALID_HANDLE_VALUE == hFile)
			{
				//LOG_WARN_W("FindFirstFile path:%s failed. err=%d", strGatherFile.c_str(), ::GetLastError());
				break;
			}

			do
			{
				//> ���� . �� .. Ŀ¼
				if (0 == lstrcmpW(wFindData.cFileName, L".") ||
					0 == lstrcmpW(wFindData.cFileName, L".."))
				{
					continue;
				}

				if (wcsicmp(wFindData.cFileName, L"$Recycle.bin") == 0)
				{
					continue;
				}
				//> ��ǰ�ļ��������ļ��У�������Ҫ�����Ƿ�ݹ����
				if ((wFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					if (bRecursionGather)
					{
						//> �ݹ�������ļ����ļ�������ļ�������Ŀ���ļ���
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