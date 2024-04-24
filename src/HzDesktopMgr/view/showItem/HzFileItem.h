#pragma once

#include <QStandardItem>
#include <QDateTime>

class HzFileItem  : public QStandardItem
{
public:
	HzFileItem();
	~HzFileItem();

	QString     m_showName;
	QString     m_path;

	QDateTime   m_modifyDate;
	QString     m_modifyDateShowStr;

	qint64      m_llSize;
	QString     m_llSizeShowStr;

	bool        m_isDir;
};
