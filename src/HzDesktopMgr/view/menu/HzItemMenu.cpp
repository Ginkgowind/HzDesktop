#include <QDebug>

#include "HzItemMenu.h"
#include "windows/UiOperation.h"

HzItemMenu::HzItemMenu(QWidget* parent)
	: QMenu(parent)
{
	addAction(tr("Open"), this, &HzItemMenu::onOpen);
	m_openWithAct = addAction(tr("Open method"), this, &HzItemMenu::onOpenWith);
	addSeparator();
	m_copyAct = addAction(tr("Copy"), this, &HzItemMenu::onCopy);
	m_cutAct = addAction(tr("Cut"), this, &HzItemMenu::onCut);
	addSeparator();
	m_deleteAct = addAction(tr("Delete"), this, &HzItemMenu::onDelete);
	m_renameAct = addAction(tr("Rename"), this, &HzItemMenu::onRename);
	addSeparator();
	addAction(tr("Explorer menu"), this, &HzItemMenu::onExplorerMenu);
}

HzItemMenu::~HzItemMenu()
{}

HzItemMenu& HzItemMenu::instance()
{
	static HzItemMenu ins;
	return ins;
}

void HzItemMenu::showMenu(
	QWidget* parentWidget,
	const QStringList& itemList
)
{
	if (itemList.empty()) {

	}
	else if (parentWidget) {
		// 部分数据存起来，因为打开文件管理器菜单时会用到
		m_showPos = QCursor::pos();
		m_parentWidget = parentWidget;
		m_selectedItemList = itemList;
		exec(m_showPos);
	}
}

void HzItemMenu::onOpen()
{
}

void HzItemMenu::onOpenWith()
{
}

void HzItemMenu::onCopy()
{
}

void HzItemMenu::onCut()
{
}

void HzItemMenu::onDelete()
{
}

void HzItemMenu::onRename()
{
}

void HzItemMenu::onExplorerMenu()
{
	HZ::showContentMenuWin10(
		m_parentWidget->winId(),
		m_selectedItemList,
		m_showPos.x(),
		m_showPos.y()
	);
}
//
//switch (m_menuShowStyle)
//{
//case HzStyle:
//
//	break;
//case Win10Style:
//	//HZ::showContentMenuWin10(
//	//	winId(),
//	//	m_itemModel->filePath(indexList[0]),
//	//	QCursor::pos().x(),
//	//	QCursor::pos().y()
//	//);
//	break;
//case Win11Style:
//	break;
//default:
//	break;
//}

HzDesktopBlankMenu::HzDesktopBlankMenu(QObject* parent)
{
	QMenu* viewModeSubMenu = addMenu(tr("View"));
	viewModeSubMenu->addAction(tr("Large icons"), this, &HzDesktopBlankMenu::setViewMode);
	viewModeSubMenu->addAction(tr("Medium icons"), this, &HzDesktopBlankMenu::setViewMode);
	viewModeSubMenu->addAction(tr("Small icons"), this, &HzDesktopBlankMenu::setViewMode);
	viewModeSubMenu->addSeparator();
	viewModeSubMenu->addAction(tr("Auto arrange icons"), this, &HzDesktopBlankMenu::switchAutoArrangeIcons);
	viewModeSubMenu->addSeparator();
	viewModeSubMenu->addAction(tr("Show desktop icons"), this, &HzDesktopBlankMenu::switchShowDesktopIcons);

	QMenu* sortBySubMenu = addMenu(tr("Sort by"));
	sortBySubMenu->addAction(tr("Large icons"), this, &HzDesktopBlankMenu::setSortMode);
	sortBySubMenu->addAction(tr("Medium icons"), this, &HzDesktopBlankMenu::setSortMode);
	sortBySubMenu->addAction(tr("Small icons"), this, &HzDesktopBlankMenu::setSortMode);

	addAction(tr("Refresh"), this, &HzDesktopBlankMenu::OnRefresh);
}

HzDesktopBlankMenu::~HzDesktopBlankMenu()
{
}

void HzDesktopBlankMenu::setViewMode()
{
	QAction* action = (QAction*)sender();
	qDebug() << action->text();
}

void HzDesktopBlankMenu::switchAutoArrangeIcons()
{
}

void HzDesktopBlankMenu::switchShowDesktopIcons()
{
}

void HzDesktopBlankMenu::setSortMode()
{
}

void HzDesktopBlankMenu::OnRefresh()
{
	qDebug() << "refresh";
}
