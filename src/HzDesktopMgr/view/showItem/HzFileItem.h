#pragma once

#include <QStandardItem>
#include <QDateTime>

class HzFileItem  : public QStandardItem
{
public:
	HzFileItem();

	~HzFileItem();

private:
	QIcon IOGetFileIcon(const QString& strPath);

protected:
	//void 

public:
	QString     m_showName;
	QString     m_filePath;

	QDateTime   m_modifyDate;

	qint64      m_llSize;
	QString     m_llSizeShowStr;

	bool        m_isDir;
};
