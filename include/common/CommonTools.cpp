#include "CommonTools.h"
#include "StringConv.h"

#define SYSTEM_FILE_PATH_SEPARATOR                      "\\"
#define SYSTEM_FILE_PATH_SEPARATOR_W                    L"\\"

static bool IsFilePathSeparator(char c)
{
	return SYSTEM_FILE_PATH_SEPARATOR[0] == c;
}

static bool IsFilePathSeparatorW(wchar_t c)
{
	return SYSTEM_FILE_PATH_SEPARATOR_W[0] == c;
}

namespace HZ
{
	std::string& AppendPath(std::string& parentPath, const std::string& subPath)
	{

		bool bHasEndPathSeparator = false;
		bool bHasStartPathSeparator = false;

		size_t parentPathLen = parentPath.length();
		size_t subPathLen = subPath.length();

		if (parentPathLen > 0)
		{
			if (IsFilePathSeparator(parentPath[parentPathLen - 1]))
			{
				bHasEndPathSeparator = true;
			}
		}

		if (subPathLen > 0)
		{
			if (IsFilePathSeparator(subPath[0]))
			{
				bHasStartPathSeparator = true;
			}
		}

		if (!bHasEndPathSeparator && !bHasStartPathSeparator)
		{
			// ·���м�û�зָ���,���һ��
			parentPath.append(SYSTEM_FILE_PATH_SEPARATOR);
			parentPath.append(subPath);
		}
		else if (bHasEndPathSeparator && bHasStartPathSeparator)
		{
			// ����·�����зָ���,ȥ��һ��; ע��ֻ������һ��,����ж��Ҳֻ������һ��
			parentPath.append(subPath, 1, subPathLen - 1);
		}
		else
		{
			// �зָ��, �����
			parentPath.append(subPath);
		}

		return parentPath;
	}

	std::string AppendPathConst(const std::string& parentPath, const std::string& subPath)
	{
		std::string strRetPath = parentPath;
		strRetPath = AppendPath(strRetPath, subPath);

		return strRetPath;
	}

	std::wstring& AppendPathW(std::wstring& parentPath, const std::wstring& subPath)
	{
		bool bHasEndPathSeparator = false;
		bool bHasStartPathSeparator = false;

		size_t parentPathLen = parentPath.length();
		size_t subPathLen = subPath.length();

		if (parentPathLen > 0)
		{
			if (IsFilePathSeparatorW(parentPath[parentPathLen - 1]))
			{
				bHasEndPathSeparator = true;
			}
		}

		if (subPathLen > 0)
		{
			if (IsFilePathSeparatorW(subPath[0]))
			{
				bHasStartPathSeparator = true;
			}
		}

		if (!bHasEndPathSeparator && !bHasStartPathSeparator)
		{
			// ·���м�û�зָ���,���һ��
			parentPath.append(SYSTEM_FILE_PATH_SEPARATOR_W);
			parentPath.append(subPath);
		}
		else if (bHasEndPathSeparator && bHasStartPathSeparator)
		{
			// ����·�����зָ���,ȥ��һ��; ע��ֻ������һ��,����ж��Ҳֻ������һ��
			parentPath.append(subPath, 1, subPathLen - 1);
		}
		else
		{
			// �зָ��, �����
			parentPath.append(subPath);
		}

		return parentPath;
	}

	std::wstring AppendPathConstW(const std::wstring& parentPath, const std::wstring& subPath)
	{
		std::wstring strRetPath = parentPath;
		strRetPath = AppendPathW(strRetPath, subPath);

		return strRetPath;
	}

	QString formatFileSize(qint64 size)
	{
		if (size >= 1024 * 1024 * 1024) { // ���ڵ���GB
			return QString("%1 GB").arg((double)size / (1024.0 * 1024.0 * 1024.0), 0, 'f', 2);
		}
		else if (size >= 1024 * 1024) { // ���ڵ���MB
			return QString("%1 MB").arg((double)size / (1024.0 * 1024.0), 0, 'f', 2);
		}
		else if (size >= 1024) { // ���ڵ���KB
			return QString("%1 KB").arg((double)size / 1024.0, 0, 'f', 2);
		}
		else { // С��KB
			return QString("%1 B").arg(size);
		}
	}
}