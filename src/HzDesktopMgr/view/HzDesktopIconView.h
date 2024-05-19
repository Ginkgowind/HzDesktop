#pragma once

#include <QListView>
#include <QMimeData>
#include <memory>

#include "showItem/HzDesktopItemModel.h"

QT_BEGIN_NAMESPACE
class QSortFilterProxyModel;
class QStandardItemModel;
class QFileSystemModel;
QT_END_NAMESPACE

class HzDesktopBlankMenu;
class HzItemDelegate;

enum MenuShowStyle
{
	HzStyle,
	Win10Style,
	Win11Style
};

class HzDesktopIconView  
	: public QAbstractItemView
	//: public QListView
{
	Q_OBJECT

public:
	HzDesktopIconView(QWidget *parent);
	~HzDesktopIconView();

protected:
	virtual QRect visualRect(const QModelIndex& index) const;

	virtual QModelIndex indexAt(const QPoint& point) const;

	virtual QModelIndex moveCursor(CursorAction cursorAction,
		Qt::KeyboardModifiers modifiers);

	virtual void setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command);
	
	virtual QRegion visualRegionForSelection(const QItemSelection& selection) const;

	// 桌面无需实现
	virtual int horizontalOffset() const { return 0; }
	virtual int verticalOffset() const { return 0; }
	virtual void scrollTo(const QModelIndex& index, ScrollHint hint = EnsureVisible) {}
	virtual bool isIndexHidden(const QModelIndex& index) const { return false; }

	bool viewportEvent(QEvent* event) override;

	void mousePressEvent(QMouseEvent* event) override;

	void mouseMoveEvent(QMouseEvent* event) override;

	void mouseReleaseEvent(QMouseEvent* event) override;

	//void startDrag(Qt::DropActions supportedActions) override;

	//void dragEnterEvent(QDragEnterEvent* event) override;

	//void dropEvent(QDropEvent* e) override;

	void contextMenuEvent(QContextMenuEvent* event) override;

	void paintEvent(QPaintEvent* e) override;

	//void doItemsLayout() override;

private:
	QStringList getSelectedPaths();

private:
	void initItemsPos();

	void updateGridSize();

	QVector<QModelIndex> intersectingSet(const QRect& area) const;

	QItemSelection getSelection(const QRect& rect) const;

	QSortFilterProxyModel* m_itemProxyModel;
	HzDesktopItemModel* m_itemModel;
	HzItemDelegate* m_itemDelegate;

	MenuShowStyle m_menuShowStyle;
	HzDesktopBlankMenu* m_desktopBlankMenu;

	QSize m_gridSize;

private:
	QRect m_elasticBand;

	// QAbstractItemView的下列数据未暴露，故自己实现一个
	QPoint m_pressedPos;
	QPersistentModelIndex m_hoverIndex;
};
