#include <QObject>
#include <QFileIconProvider>

#include "HzFileItem.h"

HzFileItem::HzFileItem()
{
	m_showName = "testApp";
	m_filePath = "C:\\Users\\Ginkgo\\Desktop\\QQ.lnk";
	m_isDir = false;

	QFileIconProvider iconProvider;
	QIcon itemIcon = IOGetFileIcon(m_filePath);
	setIcon(itemIcon);

	setData("new item", Qt::DisplayRole);

	//setText(m_showName);
	//setToolTip("testapp");

	setDragEnabled(true);
	setDropEnabled(true);
}

HzFileItem::~HzFileItem()
{
}

QIcon HzFileItem::IOGetFileIcon(const QString& strPath)
{
	QFileInfo fileInfo(strPath);
	if (fileInfo.isShortcut())
	{
		QFileInfo targetFileInfo(fileInfo.symLinkTarget());
		return QFileIconProvider().icon(targetFileInfo);
	}
	else
	{
		return QFileIconProvider().icon(fileInfo);
	}
}