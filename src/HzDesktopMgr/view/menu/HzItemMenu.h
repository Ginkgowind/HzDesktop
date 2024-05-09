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
	QAction* m_openWithAct;
	QAction* m_copyAct;
	QAction* m_cutAct;
	QAction* m_deleteAct;
	QAction* m_renameAct;

	QStringList m_selectedItemList;
	QPoint m_showPos;
	QWidget* m_parentWidget;
};

class HzDesktopBlankMenu : public QMenu
{
	Q_OBJECT

public:
	explicit HzDesktopBlankMenu(QObject* parent);
	~HzDesktopBlankMenu();

private:
	void setViewMode();

	void switchAutoArrangeIcons();

	void switchShowDesktopIcons();

	void setSortMode();

	void OnRefresh();
};
