#pragma once

#include <string>
#include <QString>

namespace HZ
{
	std::string& AppendPath(
		std::string& parentPath,
		const std::string& subPath
	);

	std::string AppendPathConst(
		const std::string& parentPath,
		const std::string& subPath
	);

	std::wstring& AppendPathW(
		std::wstring& parentPath,
		const std::wstring& subPath
	);

	std::wstring AppendPathConstW(
		const std::wstring& parentPath,
		const std::wstring& subPath
	);

	QString formatFileSize(qint64 size);
}