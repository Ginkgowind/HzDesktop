#pragma once

#include <QMenu>
#include <memory>

class HzItemMenu : public QMenu
{
	Q_OBJECT

public:
	explicit HzItemMenu(QWidget* parent = nullptr);
	~HzItemMenu();

	static HzItemMenu& instance();

	void showMenu(
		QWidget* parentWidget,
		const QStringList& itemList
	);

	void onOpen();
	void onOpenWith();
	void onCopy();
	void onCut();
	void onDelete();
	void onRename();
	void onExplorerMenu();

private:
	QAction* m_openAct;
	QAction* m_openWithAct;
	QAction* m_copyAct;
	QAction* m_cutAct;
	QAction* m_deleteAct;
	QAction* m_renameAct;
	QAction* m_explorerMenuAct;

	QStringList m_selectedItemList;
	QPoint m_showPos;
	QWidget* m_parentWidget;
};

//class HzGridBlankMenu : public QMenu
//{
//	Q_OBJECT
//
//public:
//	explicit HzGridBlankMenu(QObject* parent);
//	~HzGridBlankMenu();
//};
