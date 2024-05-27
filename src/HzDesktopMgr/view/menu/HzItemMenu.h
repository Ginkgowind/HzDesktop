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

	void showMenu(const QStringList& itemList);

signals:
	void onOpen();
	void onCopy();
	void onCut();
	void onDelete();
	void onRename();

private:
	void onExplorerMenu();

private:
	QAction* m_copyAct;
	QAction* m_cutAct;
	QAction* m_deleteAct;
	QAction* m_renameAct;

	QStringList m_selectedItemList;
	QPoint m_showPos;
};

class HzDesktopBlankMenu : public QMenu
{
	Q_OBJECT

public:
	explicit HzDesktopBlankMenu(QObject* parent);
	~HzDesktopBlankMenu();

signals:
	void refreshDesktopItemsSignal();

private:
	void setViewMode();

	void switchAutoArrangeIcons();

	void switchShowDesktopIcons();

	void setSortMode();

	void OnRefresh();
};
