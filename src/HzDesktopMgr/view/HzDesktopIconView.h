#pragma once

#include <QMimeData>

#include "HzDragDrogWindow.h"
#include "showItem/HzDesktopItemModel.h"
#include "menu/HzItemMenu.h"
#include "common/QtpqReimp.h"

QT_BEGIN_NAMESPACE
class QStandardItemModel;
class QDrag;
QT_END_NAMESPACE

class HzItemDelegate;
class HzDesktopIconViewPrivate;

class HzDesktopIconView  
	: public HzDragDrogWindow
	, public HzDesktopPublic
{
	Q_OBJECT

public:
	HzDesktopIconView(const std::wstring& dirPath, QWidget* parent);
	~HzDesktopIconView();

	const HzDesktopParam& getParam() const {
		return m_param;
	}

private:
	void initSignalAndSlot();

signals:
	void onExternalDrop(const QString& oriFilePath, QStandardItem* item);;

protected:
	virtual void initParam();

	virtual QRect visualRect(const QModelIndex& index) const;

	virtual QModelIndex indexAt(const QPoint& point) const;

	virtual QModelIndex moveCursor(CursorAction cursorAction,
		Qt::KeyboardModifiers modifiers);

	virtual void setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command);
	
	virtual QRegion visualRegionForSelection(const QItemSelection& selection) const;

	virtual bool isIndexHidden(const QModelIndex& index) const;

	// 桌面无需实现
	virtual int horizontalOffset() const { return 0; }
	virtual int verticalOffset() const { return 0; }
	virtual void scrollTo(const QModelIndex& index, ScrollHint hint = EnsureVisible) {}

	void resizeEvent(QResizeEvent* event) override;

	bool viewportEvent(QEvent* event) override;

	void mousePressEvent(QMouseEvent* event) override;

	void mouseMoveEvent(QMouseEvent* event) override;

	void mouseReleaseEvent(QMouseEvent* event) override;

	void mouseDoubleClickEvent(QMouseEvent* event) override;

	void startDrag(Qt::DropActions supportedActions) override;

	void dragEnterEvent(QDragEnterEvent* event) override;

	void dragMoveEvent(QDragMoveEvent* event) override;

	void dragLeaveEvent(QDragLeaveEvent* event) override;

	void dropEvent(QDropEvent* e) override;

	void contextMenuEvent(QContextMenuEvent* event) override;

	void paintEvent(QPaintEvent* e) override;

	void focusOutEvent(QFocusEvent* event) override;

	//void doItemsLayout() override;

	bool edit(const QModelIndex& index, EditTrigger trigger, QEvent* event) override;

	DropTargetInfo getCurrentDropTarget() override;

	bool filterDragDrop(const DropTargetInfo& targetInfo, bool bIsDrop) override;

private:
	QStringList getSelectedPaths();

	void handleLayoutChanged();

	void handleInternalDrop(QDropEvent* e);

	void handleExternalDrop(QDropEvent* e);

	void handleSetIconSizeMode(IconSizeMode mode);

	void handleEnableAutoArrange();

	void handleSetItemSortRole(int role);

	void handleSetItemSortOrder(Qt::SortOrder order);

private:

	int getInsertRow(const QPoint& pos);

	QModelIndexList intersectingSet(const QRect& area) const;

	QItemSelection getSelectionFromRect(const QRect& rect) const;

private:

	HzDesktopItemModel* m_itemModel;
	HzItemDelegate* m_itemDelegate;

	HzItemMenu* m_itemMenu;
	HzDesktopBkgMenu* m_desktopBlankMenu;

	HzDesktopParam m_param;

	int m_maxViewRow;
	int m_maxViewColumn;

	QRect m_elasticBand;

	// 插入的位置
	int m_insertRow = -1;

	QPoint m_pressedPos;
	QPersistentModelIndex m_hoverIndex;
	QPersistentModelIndex m_singleCheckedIndex;
	QItemSelectionModel::SelectionFlag m_ctrlDragSelectionFlag;

private:
	HZQ_DECLARE_PRIVATE(HzDesktopIconView)
	Q_DISABLE_COPY(HzDesktopIconView)
};
