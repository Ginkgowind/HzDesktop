#pragma once

#include <string>
#include <atomic>
#include <vector>
#include <mutex>
#include <windows.h>

#define  STR_LNK_SUFFIX         L".lnk"
#define  STR_LIB_MS_SUFFIX      L".library-ms"
#define  STR_PNG_SUFFIX         L".png"
#define  STR_JPG_SUFFIX         L".jpg"
#define  STR_JPEG_SUFFIX        L".jpeg"
#define  STR_BMP_SUFFIX         L".bmp"
#define  STR_ICO_SUFFIX         L".ico"

namespace HZ
{
	class FileInfo
	{
	public:
		FileInfo(DWORD dwFileAttributes = 0, FILETIME ftCreationTime = { 0 }, FILETIME ftLastAccessTime = { 0 },
			FILETIME ftLastWriteTime = { 0 }, DWORD nFileSizeHigh = 0, DWORD nFileSizeLow = 0,
			const std::wstring& strFileName = L"", const std::wstring& strFileAbsName = L"");

		DWORD m_dwFileAttributes;
		FILETIME m_ftCreationTime;
		FILETIME m_ftLastAccessTime;
		FILETIME m_ftLastWriteTime;
		DWORD m_nFileSizeHigh;
		DWORD m_nFileSizeLow;
		std::wstring m_strFileName;
		std::wstring m_strFileAbsName;
		std::wstring m_strSuffixName;
	};

	/**
	 * @brief 收集文件信息类
	 */
	class FileInfoGather
	{
	public:
		static bool GatherFilesInfo(
			const std::wstring& strGatherDirPath,
			std::vector<FileInfo>& fileInfoList,
			const std::wstring& strGatherFileName = L"*",
			bool bRecursionGather = false,
			bool bNeedFilterDirectory = false
		);
	};

	/**
	 * @brief 获取文件后缀名称，带.  如：'.txt'
	 * @param strFileName 文件名称
	 * @return 返回获取到的后缀名，失败返回空字符串
	 */
	std::wstring GetFileSuffix(const std::wstring& strFileName);

	/**
	 * @brief 指定文件是否为文件夹
	 * @param strFileName 文件绝对路径
	 * @return 是文件夹返回true，否则返回false
	 */
	bool IsDirectory(const std::wstring& strFilePath);

}