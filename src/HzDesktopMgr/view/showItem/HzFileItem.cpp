#include <QObject>

#include "HzFileItem.h"

HzFileItem::HzFileItem()
{
	m_showName = "testApp";
	m_isDir = false;

	setIcon(QIcon(":/HzDesktopMgr/view/qrc/test/bad.png"));

	setText(m_showName);

	setData("new item", Qt::DisplayRole);
}

HzFileItem::~HzFileItem()
{
}
