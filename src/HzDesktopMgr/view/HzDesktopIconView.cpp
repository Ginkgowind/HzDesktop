#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QDesktopServices>
#include <QMouseEvent>
#include <QApplication>
#include <QShortcut>
#include <QMimeData>
#include <QPainter>
#include <QDrag>
#include <QMenu>
#include <QUrl>
#include <QDebug>

#include <winrt/base.h>

#include "HzDesktopIconView.h"
#include "HzDesktopIconView_p.h"
#include "showItem/HzItemDelegate.h"
#include "showItem/HzFileItem.h"
#include "dragdrop/HzDrag.h"
#include "dragdrop/HzWindowsMimeIdl.h"
#include "windows/UiOperation.h"
#include "windows/tools.h"

#define TEXT_MAX_HEIGHT			40

HzDesktopIconView::HzDesktopIconView(QWidget *parent)
	: QAbstractItemView(parent)
	, HzDesktopPublic(new HzDesktopIconViewPrivate())
	, m_ctrlDragSelectionFlag(QItemSelectionModel::NoUpdate)
{
	initParam();

	m_desktopBlankMenu = new HzDesktopBlankMenu(this, &m_param);

	m_itemModel = new HzDesktopItemModel(this, &m_param);
	setModel(m_itemModel);

	m_itemDelegate = new HzItemDelegate(this, &m_param);
	setItemDelegate(m_itemDelegate);

	m_itemMenu = new HzItemMenu(this);

	//setDropIndicatorShown(true); // 显示拖放位置指示器

	setStyleSheet("QAbstractItemView {background-color: transparent;}");
	setAttribute(Qt::WA_TranslucentBackground, true);
	//setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);

	setDragEnabled(true);
	setAcceptDrops(true);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	viewport()->setAttribute(Qt::WA_Hover, true);

	setEditTriggers(EditKeyPressed);

	initSignalAndSlot();

	handleLayoutChanged();

	new HzWindowsMimeIdl();
}

HzDesktopIconView::~HzDesktopIconView()
{

}

void HzDesktopIconView::initSignalAndSlot()
{
	HZQ_D(HzDesktopIconView);

	// 复制，默认为Ctrl + C
	connect(new QShortcut(QKeySequence::Copy, this), &QShortcut::activated, 
		d, &HzDesktopIconViewPrivate::handleCopy);

	// 剪切， 默认为Ctrl + X
	connect(new QShortcut(QKeySequence::Cut, this), &QShortcut::activated,
		d, &HzDesktopIconViewPrivate::handleCut);

	// 粘贴， 默认为Ctrl + V
	connect(new QShortcut(QKeySequence::Paste, this), &QShortcut::activated,
		d, &HzDesktopIconViewPrivate::handlePaste);

	// 全选， 默认为Ctrl + A
	/*QShortcut* pSelectAllShorcut = new QShortcut(QKeySequence(QKeySequence::SelectAll),
		this, SLOT(handleSelectAll()), nullptr, Qt::WidgetWithChildrenShortcut);*/

	// 删除， 默认为Delete
	connect(new QShortcut(QKeySequence::Delete, this), &QShortcut::activated,
		d, &HzDesktopIconViewPrivate::handleDelete);
	connect(new QShortcut(QKeySequence("Ctrl+D"), this), &QShortcut::activated,
		d, &HzDesktopIconViewPrivate::handleDelete);

	// 刷新，默认为F5
	//new QShortcut(QKeySequence(QKeySequence::Refresh),
	//	this, SLOT(handleRefreshFile()), nullptr, Qt::WidgetWithChildrenShortcut);

	// 重命名，F2
	connect(new QShortcut(QKeySequence(Qt::Key_F2), this), &QShortcut::activated,
		d, &HzDesktopIconViewPrivate::handleRename);

	// 打开选中文件， Enter
	connect(new QShortcut(QKeySequence(Qt::Key_Return), this), &QShortcut::activated,
		d, &HzDesktopIconViewPrivate::handleOpen);
	connect(new QShortcut(QKeySequence(Qt::Key_Enter), this), &QShortcut::activated,
		d, &HzDesktopIconViewPrivate::handleOpen);

	// item右键菜单
	connect(m_itemMenu, &HzItemMenu::onOpen, d, &HzDesktopIconViewPrivate::handleOpen);
	connect(m_itemMenu, &HzItemMenu::onCopy, d, &HzDesktopIconViewPrivate::handleCopy);
	connect(m_itemMenu, &HzItemMenu::onCut, d, &HzDesktopIconViewPrivate::handleCut);
	connect(m_itemMenu, &HzItemMenu::onDelete, d, &HzDesktopIconViewPrivate::handleDelete);
	connect(m_itemMenu, &HzItemMenu::onRename, d, &HzDesktopIconViewPrivate::handleRename);

	// 空白处右键菜单
	connect(m_desktopBlankMenu, &HzDesktopBlankMenu::onSetIconSizeMode,
		this, &HzDesktopIconView::handleSetIconSizeMode);
	connect(m_desktopBlankMenu, &HzDesktopBlankMenu::onSetItemSortRole,
		this, &HzDesktopIconView::handleSetItemSortRole);
	connect(m_desktopBlankMenu, &HzDesktopBlankMenu::onSetItemSortOrder,
		this, &HzDesktopIconView::handleSetItemSortOrder);
	connect(m_desktopBlankMenu, &HzDesktopBlankMenu::enableAutoArrange,
		this, &HzDesktopIconView::handleEnableAutoArrange);
	connect(m_desktopBlankMenu, &HzDesktopBlankMenu::onHide, [this]() {setVisible(false); });

	connect(m_desktopBlankMenu, &HzDesktopBlankMenu::refreshDesktop,
		m_itemModel, &HzDesktopItemModel::refreshItems);

	// TODO item一有变化时清理menu的排序状态
	connect(m_itemModel, &HzDesktopItemModel::itemChanged,
		[this](QStandardItem* item) {m_desktopBlankMenu->hideSortStatus(); });
}

void HzDesktopIconView::initParam()
{
	m_param.dirPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
}

QRect HzDesktopIconView::visualRect(const QModelIndex& index) const
{
	// TODO 处理横向时的逻辑
	int posIndexX = index.row() / m_maxViewRow;
	int posIndexY = index.row() % m_maxViewRow;

	return QRect(
		posIndexX * m_param.gridSize.width(),
		posIndexY * m_param.gridSize.height(),
		2 * m_param.iconMargin.width() + m_param.iconSize.width(),
		2 * m_param.iconMargin.height() + m_param.iconSize.height() + TEXT_MAX_HEIGHT
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
		const QModelIndexList intersectVector = intersectingSet(rect);
		QModelIndex tl;
		if (!intersectVector.isEmpty())
			tl = intersectVector.last(); // special case for mouse press; only select the top item
		if (tl.isValid())
			selection.select(tl, tl);
	}
	else {
		if (state() == DragSelectingState) { // visual selection mode (rubberband selection)
			selection = getSelectionFromRect(rect);
		}
		else {
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

bool HzDesktopIconView::isIndexHidden(const QModelIndex& index) const
{
	return !m_itemModel->itemFromIndex(index)->isEnabled();
}

void HzDesktopIconView::resizeEvent(QResizeEvent* event)
{
	handleLayoutChanged();

	QAbstractItemView::resizeEvent(event);
}

bool HzDesktopIconView::viewportEvent(QEvent* event)
{
	switch (event->type()) {
	case QEvent::HoverMove:
	case QEvent::HoverEnter:
		m_hoverIndex = indexAt(static_cast<QHoverEvent*>(event)->pos());
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
	QPersistentModelIndex index = indexAt(m_pressedPos);
	QItemSelectionModel::SelectionFlags command = selectionCommand(index, e);

	if (index.isValid()) {
		if (command.testFlag(QItemSelectionModel::Toggle)) {
			command &= ~QItemSelectionModel::Toggle;
			m_ctrlDragSelectionFlag = selectionModel()->isSelected(index) ? QItemSelectionModel::Deselect : QItemSelectionModel::Select;
			command |= m_ctrlDragSelectionFlag;
		}
	}
}

void HzDesktopIconView::mouseMoveEvent(QMouseEvent* e)
{
	State preState = state();

	QAbstractItemView::mouseMoveEvent(e);

	// dropEvent之后总是会又触发一次mouseMoveEvent，所以这里要设置过滤条件
	bool bFilterDragSelecting = (state() == DraggingState) ||
		(preState == DraggingState && (m_pressedPos - e->pos()).manhattanLength() > QApplication::startDragDistance());

	if ((e->buttons() & Qt::LeftButton) && !bFilterDragSelecting) {
		// 参考 QAbstractItemView 实现状态设置与框选逻辑
		setState(DragSelectingState);

		QPersistentModelIndex index = indexAt(e->pos());
		QItemSelectionModel::SelectionFlags command = selectionCommand(index, e);
		if (m_ctrlDragSelectionFlag != QItemSelectionModel::NoUpdate && command.testFlag(QItemSelectionModel::Toggle)) {
			command &= ~QItemSelectionModel::Toggle;
			command |= m_ctrlDragSelectionFlag;
		}

		// Do the normalize ourselves, since QRect::normalized() is flawed
		QRect selectionRect = QRect(m_pressedPos, e->pos());
		setSelection(selectionRect, command);

		// 参考QListView 实现框选框绘制
		QRect rect(m_pressedPos, e->pos());
		rect = rect.normalized();
		viewport()->update(rect.united(m_elasticBand));
		m_elasticBand = rect;
	}
}

void HzDesktopIconView::mouseReleaseEvent(QMouseEvent* e)
{
	QAbstractItemView::mouseReleaseEvent(e);

	if (state() != EditingState) {
		setState(NoState);
	}

	if (m_elasticBand.isValid()) {
		viewport()->update(m_elasticBand);
		m_elasticBand = QRect();
	}
}

void HzDesktopIconView::mouseDoubleClickEvent(QMouseEvent* event)
{
	HZQ_D(HzDesktopIconView);

	if (selectedIndexes().empty()) {
		// 隐藏桌面
		if (m_param.bEnableDoubleClick) {
			setVisible(false);
		}
	}
	else {
		d->handleOpen();
	}
}

//void HzDesktopIconView::startDrag(Qt::DropActions supportedActions)
//{
//	HZQ_D(HzDesktopIconView);
//
//	QMimeData* dragMimeData = HZ::multiDrag(getSelectedPaths());
//
//	QDrag* drag = new QDrag(this);
//	drag->setMimeData(dragMimeData);
//	//drag->setPixmap(item->icon().pixmap(iconSize()));
//	//drag->setPixmap(QPixmap(":/HzDesktopMgr/view/qrc/test/heart.png"));
//	 //开始拖放操作
//	Qt::DropAction dropAction = drag->exec(supportedActions);
//	if (dropAction == Qt::MoveAction) {
//		//currentFilePath.clear();
//	}
//
//	//QRect rect;
//	//QPixmap pixmap = d->renderToPixmap(selectedIndexes(), &rect);
//	//HzDrag* drag = new HzDrag(this);
//	//drag->setItemPaths(getSelectedPaths());
//	//drag->setPixmap(pixmap);
//	//Qt::DropAction dropAction = drag->exec(supportedActions);
//
//}

void HzDesktopIconView::dragEnterEvent(QDragEnterEvent* e)
{
	e->accept();

	if (e->source() == this) {

	}
	else {

	}
}

void HzDesktopIconView::dragMoveEvent(QDragMoveEvent* e)
{
	QAbstractItemView::dragMoveEvent(e);
	
	HWND hWndTarget = ::WindowFromPoint({200, 250});
	if (hWndTarget)
	{
		TCHAR szTitle[256];
		if (::GetWindowText(hWndTarget, szTitle, _countof(szTitle)))
		{
			// szTitle现在包含了目标窗口的标题
			//_tprintf(TEXT("拖放到 '%s'\n"), szTitle);
			int a = 1;
		}
	}

	e->accept();

	m_hoverIndex = indexAt(e->pos());

	qDebug() << e->proposedAction() << e->pos() << m_hoverIndex.row();

	const QPoint pos = e->pos() - QPoint(0, 0);
	if (indexAt(pos).isValid()) {
		// TODO 非自动排序时要处理
	}
	else {
		// 计算出当前鼠标所处的网格
		m_insertRow = getInsertRow(pos);
	}
}

void HzDesktopIconView::dragLeaveEvent(QDragLeaveEvent* e)
{
	QAbstractItemView::dragLeaveEvent(e);

	e->accept();
}

void HzDesktopIconView::dropEvent(QDropEvent* e)
{
	if (e->source() == this) {
		handleInternalDrop(e);
	}
	else {
		e->accept();
		//QAbstractItemView::dropEvent(e);
		//handleExternalDrop(e);
	}

	m_insertRow = -1;
	

	setState(NoState);

	// TODO 为什么下面的函数用内联会找不到定义，难道是因为两边都有inline？
	// TODO 根据是否有变化来判断是否取消显示状态
	m_desktopBlankMenu->hideSortStatus();
}

void HzDesktopIconView::contextMenuEvent(QContextMenuEvent* event)
{
	QStringList selectedPathList = getSelectedPaths();

	if (selectedPathList.empty()) {
		//m_desktopBlankMenu->exec(QCursor::pos());
		m_desktopBlankMenu->showMenu();
	}
	else {
		m_itemMenu->showMenu(selectedPathList);
	}
}

void HzDesktopIconView::paintEvent(QPaintEvent* e)
{
	QPainter painter(viewport());

	// TODO 绘制一个透明内容，为什么不绘制就会穿透
	painter.save();
	painter.setPen(Qt::transparent);
	painter.setBrush(QColor(0, 0, 0, 1));
	painter.drawRect(viewport()->rect());
	painter.restore();

	QStyleOptionViewItem option = QAbstractItemView::viewOptions();
	const QModelIndexList toBeRendered = intersectingSet(e->rect());
	const QModelIndex current = currentIndex();
	const QModelIndex hover = m_hoverIndex;
	const bool focus = (hasFocus() || viewport()->hasFocus()) && current.isValid();
	const QStyle::State state = option.state;
	const QAbstractItemView::State viewState = this->state();
	const bool enabled = (state & QStyle::State_Enabled) != 0;

	//e->rect() TODO 利用这个rect来决定要绘制哪些modelindex 以此来优化性能
	QModelIndexList::const_iterator end = toBeRendered.constEnd();
	for (QModelIndexList::const_iterator it = toBeRendered.constBegin(); it != end; ++it) {
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

	if (m_insertRow >= 0) {
		painter.save();
		// 计算出插入线的位置
		QPoint TMP_DELTA(0, -5);
		QRect rect = visualRect(model()->index(m_insertRow, 0));
		painter.setPen(QPen(Qt::blue, 2));
		painter.drawLine(rect.topLeft() + TMP_DELTA, rect.topRight() + TMP_DELTA);
		painter.restore();
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

void HzDesktopIconView::handleLayoutChanged()
{
	m_maxViewRow = height() / m_param.gridSize.height();
	m_maxViewColumn = width() / m_param.gridSize.width();
}

void HzDesktopIconView::handleInternalDrop(QDropEvent* e)
{
	QModelIndexList indexList = selectedIndexes();

	if (m_insertRow < 0) {
		return;
	}

	int insertRow = m_insertRow;

	// 自动排序时按照鼠标位置挨个插入进去，否则每一个都根据各自位置进行插入
	if (m_param.bAutoArrange) {
		// 按照row从大到小进行排序，从而先删除大的
		qSort(indexList.begin(), indexList.end(), [](const QModelIndex& index1, const QModelIndex& index2) {
			return index1.row() > index2.row(); });

		QList<QStandardItem*> dropItems;
		for (const QModelIndex& index : indexList) {
			dropItems.push_back(m_itemModel->takeItem(index.row()));
			m_itemModel->removeRow(index.row());
		}

		// TODO 拖拽到自身下一格会有问题
		for (auto it = dropItems.rbegin(); it < dropItems.rend(); it++) {
			m_itemModel->insertRow(insertRow++, *it);
		}
	}
	else {
		// 按照row从小到大进行排序，从而先插入小的
		qSort(indexList.begin(), indexList.end(), [](const QModelIndex& index1, const QModelIndex& index2) {
			return index1.row() < index2.row(); });

		QRect dragStartIndexRect = visualRect(indexAt(m_pressedPos));
		//QMap<QPersistentModelIndex, QRect> indexRectMap;
		QMap<QModelIndex, QRect> indexRectMap;
		for (auto& index : indexList) {
			indexRectMap[index] = visualRect(index);
		}

		// TODO 为什么QMap不支持如下迭代方式
		for (auto& [index, rect] : indexRectMap.toStdMap()) {
			QStandardItem* item = m_itemModel->takeItem(index.row());
			// TODO 再细致了解itemFromIndex和item的区别，以及root和parent
			m_itemModel->itemFromIndex(index)->setEnabled(false);

			QPoint delta = rect.topLeft() - dragStartIndexRect.topLeft();
			int nInsertRow = getInsertRow(e->pos() + delta);
			m_itemModel->insertItems(nInsertRow, { item });
		}
	}

	viewport()->update(viewport()->rect());
}

void HzDesktopIconView::handleExternalDrop(QDropEvent* e)
{
}

void HzDesktopIconView::handleSetIconSizeMode(IconSizeMode mode)
{
	handleLayoutChanged();

	viewport()->update();
}

void HzDesktopIconView::handleEnableAutoArrange()
{
	m_itemModel->removeAllDisableItem();

	m_itemModel->sort(0, m_param.sortOrder);
}

void HzDesktopIconView::handleSetItemSortRole(CustomRoles role)
{
	// 这里没有使用QSortFilterProxyModel，因为桌面在排序之后还能进行拖动图标，
	// 但是QSortFilterProxyModel仅仅是对原始model的映射，不能理想地添加数据

	// 触发排序时，先删除掉所有的占位item
	m_itemModel->removeAllDisableItem();

	m_itemModel->setSortRole(role);
	m_itemModel->sort(0, m_param.sortOrder);
}

void HzDesktopIconView::handleSetItemSortOrder(Qt::SortOrder order)
{
	// 触发排序时，先删除掉所有的占位item
	m_itemModel->removeAllDisableItem();

	m_itemModel->sort(0, m_param.sortOrder);
}

int HzDesktopIconView::getInsertRow(const QPoint& pos)
{
	int row = pos.x() / m_param.gridSize.width() * m_maxViewRow
		+ pos.y() / m_param.gridSize.height();
	if (pos.y() % m_param.gridSize.height() > m_param.gridSize.height() / 2) {
		row += 1;
	}

	return row;
}

QModelIndexList HzDesktopIconView::intersectingSet(const QRect& area) const
{
	QModelIndexList ret;

	int rowCount = model()->rowCount();
	for (int i = 0; i < rowCount; ++i) {
		QModelIndex index = model()->index(i, 0);
		if (area.intersects(visualRect(index))) {
			ret.push_back(index);
		}
	}

	return ret;
}

QItemSelection HzDesktopIconView::getSelectionFromRect(const QRect& rect) const
{
	QItemSelection selection;
	QModelIndex tl, br;
	// TODO 这里为什么要normalized
	const QModelIndexList intersectVector = intersectingSet(rect.normalized());
	QModelIndexList::const_iterator it = intersectVector.begin();
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