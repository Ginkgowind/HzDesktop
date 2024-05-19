#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>
#include <QUrl>
#include <QMenu>
#include <QDebug>
#include <QPainter>

#include <QFileSystemModel>

#include "HzDesktopIconView.h"
#include "showItem/HzItemDelegate.h"
#include "menu/HzItemMenu.h"
#include "showItem/HzFileItem.h"
#include "windows/UiOperation.h"
#include "windows/tools.h"

#define MAX_ICON_SIZE			108
#define MEDIUM_ICON_SIZE		90
#define MIN_ICON_SIZE			72
#define ICON_MARGIN				5		// icon与周围边界的距离，固定不变

#define TEXT_MAX_HEIGHT			40

#define ITEM_X_SPACE        5         // item之间的X方向的间隙
#define ITEM_Y_SPACE        20        // item之间的Y方向的间隙

HzDesktopIconView::HzDesktopIconView(QWidget *parent)
	: QAbstractItemView(parent)
	//: QListView(parent)
	, m_menuShowStyle(Win10Style)
{
	setFixedSize(1200, 800);

	setIconSize({MEDIUM_ICON_SIZE, MEDIUM_ICON_SIZE});

	//m_desktopBlankMenu = new HzDesktopBlankMenu(this);

	//m_itemProxyModel = new QSortFilterProxyModel(this);
	//m_itemModel = new HzDesktopItemModel(m_itemProxyModel);
	//m_itemProxyModel->setSourceModel(m_itemModel);
	//setModel(m_itemProxyModel);

	m_itemModel = new HzDesktopItemModel(this);
	setModel(m_itemModel);

	m_itemDelegate = new HzItemDelegate(this);
	m_itemDelegate->setUiParam(
		QSize(MEDIUM_ICON_SIZE, MEDIUM_ICON_SIZE),
		ICON_MARGIN);
	setItemDelegate(m_itemDelegate);

	//setDropIndicatorShown(true); // 显示拖放位置指示器

	//setStyleSheet("QListView {background-color: transparent;}");
	//setDragEnabled(true);
	//setAcceptDrops(true);
	setDragEnabled(false);
	setAcceptDrops(false);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	viewport()->setAttribute(Qt::WA_Hover, true);

	setEditTriggers(EditKeyPressed);

	//connect(m_desktopBlankMenu, &HzDesktopBlankMenu::refreshDesktopItemsSignal,
	//	m_itemModel, &HzDesktopItemModel::refreshItems);

	updateGridSize();
}

HzDesktopIconView::~HzDesktopIconView()
{

}

QRect HzDesktopIconView::visualRect(const QModelIndex& index) const
{
	QPoint posIndex = index.data(HzDesktopItemModel::PosIndex2DRole).toPoint();

	return QRect(
		posIndex.x() * m_gridSize.width(),
		posIndex.y() * m_gridSize.height(),
		2 * ICON_MARGIN + iconSize().width(),
		2 * ICON_MARGIN + iconSize().height() + TEXT_MAX_HEIGHT
	);
}

QModelIndex HzDesktopIconView::indexAt(const QPoint& point) const
{
	QModelIndex retIndex;

	int rowCount = model()->rowCount();
	for (int i = 0; i < rowCount; ++i) {
		QModelIndex index = model()->index(i, 0);
		if (visualRect(index).contains(point)) {
			retIndex = index;
			break;
		}
	}

	return retIndex;
}

QModelIndex HzDesktopIconView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
	return QModelIndex();
}

void HzDesktopIconView::setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command)
{
	QItemSelection selection;

	if (rect.width() == 1 && rect.height() == 1) {
		const QVector<QModelIndex> intersectVector = intersectingSet(rect);
		QModelIndex tl;
		if (!intersectVector.isEmpty())
			tl = intersectVector.last(); // special case for mouse press; only select the top item
		if (tl.isValid())
			selection.select(tl, tl);
	}
	else {
		if (state() == DragSelectingState) { // visual selection mode (rubberband selection)
			selection = getSelection(rect);
		}
		else {
			int a = 1;
		}
		// TODO 处理 logical selection mode (key and mouse click selection)
	}

	selectionModel()->select(selection, command);
}

QRegion HzDesktopIconView::visualRegionForSelection(const QItemSelection& selection) const
{
	QRegion selectionRegion;
	const QRect& viewportRect = viewport()->rect();
	for (const auto& elem : selection) {
		if (!elem.isValid())
			continue;
		QModelIndex parent = elem.topLeft().parent();
		//we only display the children of the root in a listview
		//we're not interested in the other model indexes
		if (parent != rootIndex())
			continue;
		int t = elem.topLeft().row();
		int b = elem.bottomRight().row();

		 // in non-static mode, we have to go through all selected items
		for (int r = t; r <= b; ++r) {
			const QRect& rect = visualRect(model()->index(r, 0, parent));
			if (viewportRect.intersects(rect))
				selectionRegion += rect;
		}
		//else { // in static mode, we can optimize a bit
		//	while (t <= b && d->isHidden(t)) ++t;
		//	while (b >= t && d->isHidden(b)) --b;
		//	const QModelIndex top = d->model->index(t, c, parent);
		//	const QModelIndex bottom = d->model->index(b, c, parent);
		//	QRect rect(visualRect(top).topLeft(),
		//		visualRect(bottom).bottomRight());
		//	if (viewportRect.intersects(rect))
		//		selectionRegion += rect;
		//}
	}

	return selectionRegion;
}

bool HzDesktopIconView::viewportEvent(QEvent* event)
{
	switch (event->type()) {
	case QEvent::HoverMove:
	case QEvent::HoverEnter:
		m_hoverIndex = indexAt(static_cast<QHoverEvent*>(event)->pos());
		if (rootIndex().isValid()) {
			int a = 1;
		}
		break;
	case QEvent::HoverLeave:
		m_hoverIndex = QModelIndex();
		break;

	default:
		break;
	}

	return QAbstractItemView::viewportEvent(event);
}

void HzDesktopIconView::mousePressEvent(QMouseEvent* e)
{
	QAbstractItemView::mousePressEvent(e);

	m_pressedPos = e->pos();
}

void HzDesktopIconView::mouseMoveEvent(QMouseEvent* e)
{
	//QListView::mouseMoveEvent(e);
	QAbstractItemView::mouseMoveEvent(e);

	if (state() == DragSelectingState) {
		QRect rect(m_pressedPos, e->pos());
		rect = rect.normalized();
		viewport()->update(rect.united(m_elasticBand));
		m_elasticBand = rect;
	}
}

void HzDesktopIconView::mouseReleaseEvent(QMouseEvent* e)
{
	QAbstractItemView::mouseReleaseEvent(e);

	if (m_elasticBand.isValid()) {
		viewport()->update(m_elasticBand);
		m_elasticBand = QRect();
	}
}


//void HzDesktopIconView::startDrag(Qt::DropActions supportedActions)
//{
//	QMimeData* dragMimeData = HZ::multiDrag(getSelectedPaths());
//
//	QDrag* drag = new QDrag(this);
//	drag->setMimeData(dragMimeData);
//	//drag->setPixmap(item->icon().pixmap(iconSize()));
//	drag->setPixmap(QPixmap(":/HzDesktopMgr/view/qrc/test/heart.png"));
//	 //开始拖放操作
//	Qt::DropAction dropAction = drag->exec(supportedActions);
//	if (dropAction == Qt::MoveAction) {
//		//currentFilePath.clear();
//	}
//
//}

//void HzDesktopIconView::dragEnterEvent(QDragEnterEvent* event)
//{
//	event->setDropAction(Qt::MoveAction);
//	event->accept();
//}

//void HzDesktopIconView::dropEvent(QDropEvent* e)
//{
//	auto strList = e->mimeData()->formats();
//	qDebug() << strList;
//}

void HzDesktopIconView::contextMenuEvent(QContextMenuEvent* event)
{
	QStringList selectedPathList = getSelectedPaths();

	if (selectedPathList.empty()) {
		m_desktopBlankMenu->exec(QCursor::pos());
	}
	else {
		HzItemMenu::instance().showMenu(this, selectedPathList);
	}

	// TODO 有作用吗
	event->accept();
}

void HzDesktopIconView::paintEvent(QPaintEvent* e)
{
	QPainter painter(viewport());

	QStyleOptionViewItem option = QAbstractItemView::viewOptions();
	const QVector<QModelIndex> toBeRendered = intersectingSet(e->rect());
	const QModelIndex current = currentIndex();
	const QModelIndex hover = m_hoverIndex;
	const bool focus = (hasFocus() || viewport()->hasFocus()) && current.isValid();
	const QStyle::State state = option.state;
	const QAbstractItemView::State viewState = this->state();
	const bool enabled = (state & QStyle::State_Enabled) != 0;

	//e->rect() TODO 利用这个rect来决定要绘制哪些modelindex
	QVector<QModelIndex>::const_iterator end = toBeRendered.constEnd();
	for (QVector<QModelIndex>::const_iterator it = toBeRendered.constBegin(); it != end; ++it) {
		option.rect = visualRect(*it);
		option.state = state;
		if (selectionModel() && selectionModel()->isSelected(*it))
			option.state |= QStyle::State_Selected;
		if (enabled) {
			QPalette::ColorGroup cg;
			if ((model()->flags(*it) & Qt::ItemIsEnabled) == 0) {
				option.state &= ~QStyle::State_Enabled;
				cg = QPalette::Disabled;
			}
			else {
				cg = QPalette::Normal;
			}
			option.palette.setCurrentColorGroup(cg);
		}

		if (focus && current == *it) {
			option.state |= QStyle::State_HasFocus;
			if (viewState == EditingState)
				option.state |= QStyle::State_Editing;
		}
		option.state.setFlag(QStyle::State_MouseOver, *it == hover);

		itemDelegate()->paint(&painter, option, *it);
	}

	if (m_elasticBand.isValid()) {
		QStyleOptionRubberBand opt;
		opt.initFrom(this);
		opt.shape = QRubberBand::Rectangle;
		opt.opaque = false;
		opt.rect = m_elasticBand;
		painter.save();
		style()->drawControl(QStyle::CE_RubberBand, &opt, &painter);
		painter.restore();
	}
}

//void HzDesktopIconView::doItemsLayout()
//{
//}

QStringList HzDesktopIconView::getSelectedPaths()
{
	QStringList pathList;

	// TODO 了解此处直接调用函数与复制变量，有什么区别？
	QModelIndexList indexList = selectedIndexes();
	for (const QModelIndex& index : indexList) {
		pathList.append(m_itemModel->filePath(index));
	}
		
	return pathList;
}

void HzDesktopIconView::initItemsPos()
{
	int rowCount = model()->rowCount();
	for (int i = 0; i < rowCount; ++i) {
		QModelIndex index = model()->index(i, 0); // 获取指定行列的索引
		QPoint posIndex = index.data(HzDesktopItemModel::PosIndex2DRole).toPoint();
		//setPositionForIndex({ posIndex.x() * 120, posIndex.y() * 140 }, index);
	}
}

void HzDesktopIconView::updateGridSize()
{
	m_gridSize = QSize(MEDIUM_ICON_SIZE, MEDIUM_ICON_SIZE)
		+ QSize(ICON_MARGIN, ICON_MARGIN)
		+ QSize(0, TEXT_MAX_HEIGHT)
		+ QSize(ITEM_X_SPACE, ITEM_Y_SPACE);
}

QVector<QModelIndex> HzDesktopIconView::intersectingSet(const QRect& area) const
{
	QVector<QModelIndex> ret;

	int rowCount = model()->rowCount();
	for (int i = 0; i < rowCount; ++i) {
		QModelIndex index = model()->index(i, 0);
		if (area.intersects(visualRect(index))) {
			ret.push_back(index);
		}
	}

	return ret;
}

QItemSelection HzDesktopIconView::getSelection(const QRect& rect) const
{
	QItemSelection selection;
	QModelIndex tl, br;
	// TODO 这里为什么要normalized
	const QVector<QModelIndex> intersectVector = intersectingSet(rect.normalized());
	qDebug() << intersectVector.size();
	QVector<QModelIndex>::const_iterator it = intersectVector.begin();
	qDebug() << rect << visualRect(*it);
	for (; it != intersectVector.end(); ++it) {
		if (!tl.isValid() && !br.isValid()) {
			tl = br = *it;
		}
		else if ((*it).row() == (tl.row() - 1)) {
			tl = *it; // expand current range
		}
		else if ((*it).row() == (br.row() + 1)) {
			br = (*it); // expand current range
		}
		else {
			selection.select(tl, br); // select current range
			tl = br = *it; // start new range
		}
	}

	if (tl.isValid() && br.isValid())
		selection.select(tl, br);
	else if (tl.isValid())
		selection.select(tl, tl);
	else if (br.isValid())
		selection.select(br, br);

	return selection;
}