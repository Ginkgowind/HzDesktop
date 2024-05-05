#include "HzItemMenu.h"
#include "windows/UiOperation.h"

HzItemMenu::HzItemMenu(QWidget* parent)
	: QMenu(parent)
{
	//m_openAct = new QAction(tr("打开"), this);
	//m_openWithAct = new QAction(tr("打开方式"), this);
	//m_copyAct = new QAction(tr("复制"), this);
	//m_cutAct = new QAction(tr("剪切"), this);
	//m_deleteAct = new QAction(tr("删除"), this);
	//m_renameAct = new QAction(tr("重命名"), this);
	//m_explorerMenuAct = new QAction(tr("资源管理器菜单"), this);
	m_openAct = new QAction(tr("Open"), this);
	m_openWithAct = new QAction(tr("Open method"), this);
	m_copyAct = new QAction(tr("Copy"), this);
	m_cutAct = new QAction(tr("Cut"), this);
	m_deleteAct = new QAction(tr("Delete"), this);
	m_renameAct = new QAction(tr("Rename"), this);
	m_explorerMenuAct = new QAction(tr("Explorer menu"), this);

	addAction(m_openAct);
	addAction(m_openWithAct);
	addSeparator();
	addAction(m_copyAct);
	addAction(m_cutAct);
	addSeparator();
	addAction(m_deleteAct);
	addAction(m_renameAct);
	addSeparator();
	addAction(m_explorerMenuAct);

	connect(m_openAct, &QAction::triggered,
		this, &HzItemMenu::onOpen);
	connect(m_openWithAct, &QAction::triggered,
		this, &HzItemMenu::onOpenWith);
	connect(m_copyAct, &QAction::triggered,
		this, &HzItemMenu::onCopy);
	connect(m_cutAct, &QAction::triggered,
		this, &HzItemMenu::onCut);
	connect(m_deleteAct, &QAction::triggered,
		this, &HzItemMenu::onDelete);
	connect(m_renameAct, &QAction::triggered,
		this, &HzItemMenu::onRename);
	connect(m_explorerMenuAct, &QAction::triggered,
		this, &HzItemMenu::onExplorerMenu);
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
	if (!parentWidget || itemList.empty()) {
		return;
	}

	// 部分数据存起来，因为打开文件管理器菜单时会用到
	m_showPos = QCursor::pos();
	m_parentWidget = parentWidget;
	m_selectedItemList = itemList;
	exec(m_showPos);
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