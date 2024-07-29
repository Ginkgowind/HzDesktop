#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QDesktopServices>
#include <QMouseEvent>
#include <QApplication>
#include <QShortcut>
#include <QClipboard>
#include <QMimeData>
#include <QPainter>
#include <QDrag>
#include <QMenu>
#include <QUrl>
#include <QDir>
#include <QDebug>

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsDropShadowEffect>

#include <winrt/base.h>

#include "HzDesktopIconView.h"
#include "HzDesktopIconView_p.h"
#include "showItem/HzItemDelegate.h"
#include "showItem/HzFileItem.h"
#include "dragdrop/HzDrag.h"
#include "dragdrop/HzWindowsMimeIdl.h"
#include "windows/UiOperation.h"
#include "windows/tools.h"
#include "windows/FileUtils.h"

HzDesktopIconView::HzDesktopIconView(const std::wstring& dirPath, QWidget* parent)
	: HzDragDrogWindow(dirPath, parent)
	, HzDesktopPublic(new HzDesktopIconViewPrivate())
	, m_ctrlDragSelectionFlag(QItemSelectionModel::NoUpdate)
{
	resize(parent->size());

	// TODO 后续把m_param指针传给各个成员的方式应该不好，等做完了再尝试优化
	// TODO 已有const HzDesktopParam& getParam() const 函数，后续结合weak_ptr给用起来
	initParam();

	m_desktopBlankMenu = new HzDesktopBkgMenu(this, &m_param);

	m_itemModel = new HzDesktopItemModel(this, &m_param);
	setModel(m_itemModel);

	m_itemDelegate = new HzItemDelegate(this, &m_param);
	setItemDelegate(m_itemDelegate);

	m_itemMenu = new HzItemMenu(this);

	//setDropIndicatorShown(true); // 显示拖放位置指示器

	setStyleSheet("QAbstractItemView {background-color: transparent;}");

	setDragEnabled(true);
	setAcceptDrops(true);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	viewport()->setAttribute(Qt::WA_Hover, true);

	setEditTriggers(EditKeyPressed);

	initSignalAndSlot();

	handleLayoutChanged();

	new HzWindowsMimeIdl();

	setFont(m_param.font);

	// TODO 优化下这里的逻辑，为什么放在父类构造函数里就会崩溃？
	InitializeDragDropHelper(reinterpret_cast<HWND>(winId()));
}

HzDesktopIconView::~HzDesktopIconView()
{

}

void HzDesktopIconView::initSignalAndSlot()
{
	HZQ_D(HzDesktopIconView);

	// 复制，默认为Ctrl + C
	connect(new QShortcut(QKeySequence::Copy, this), &QShortcut::activated,
		[this]() {m_itemMenu->handleCopy(getSelectedPaths()); });

	// 剪切， 默认为Ctrl + X
	connect(new QShortcut(QKeySequence::Cut, this), &QShortcut::activated,
		[this]() {m_itemMenu->handleCut(getSelectedPaths()); });

	// 粘贴， 默认为Ctrl + V
	connect(new QShortcut(QKeySequence::Paste, this), &QShortcut::activated,
		m_desktopBlankMenu, &HzDesktopBkgMenu::handlePaste);

	// 全选， 默认为Ctrl + A
	connect(new QShortcut(QKeySequence::SelectAll, this), &QShortcut::activated,
		d, &HzDesktopIconViewPrivate::handleSelectAll);

	// 删除， 默认为Delete
	connect(new QShortcut(QKeySequence::Delete, this), &QShortcut::activated,
		[this]() {m_itemMenu->handleDelete(getSelectedPaths()); });
	connect(new QShortcut(QKeySequence("Ctrl+D"), this), &QShortcut::activated,
		[this]() {m_itemMenu->handleDelete(getSelectedPaths()); });

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
	connect(m_itemMenu, &HzItemMenu::onRename, d, &HzDesktopIconViewPrivate::handleRename);

	// 空白处右键菜单
	connect(m_desktopBlankMenu, &HzDesktopBkgMenu::onSetIconSizeMode,
		this, &HzDesktopIconView::handleSetIconSizeMode);
	connect(m_desktopBlankMenu, &HzDesktopBkgMenu::onSetItemSortRole,
		this, &HzDesktopIconView::handleSetItemSortRole);
	connect(m_desktopBlankMenu, &HzDesktopBkgMenu::onSetItemSortOrder,
		this, &HzDesktopIconView::handleSetItemSortOrder);
	connect(m_desktopBlankMenu, &HzDesktopBkgMenu::enableAutoArrange,
		this, &HzDesktopIconView::handleEnableAutoArrange);
	connect(m_desktopBlankMenu, &HzDesktopBkgMenu::onHide, [this]() {setVisible(false); });

	connect(m_desktopBlankMenu, &HzDesktopBkgMenu::refreshDesktop,
		m_itemModel, &HzDesktopItemModel::refreshItems);

	// item一有变化时清理menu的排序状态
	connect(m_itemModel, &HzDesktopItemModel::itemChanged,
		[this](QStandardItem* item) {m_desktopBlankMenu->hideSortStatus(); });

	connect(m_itemModel, &QAbstractItemModel::rowsInserted,
		[this](const QModelIndex& parent, int first, int last) {
			m_desktopBlankMenu->hideSortStatus();
			viewport()->update();
		});

	connect(m_itemModel, &QAbstractItemModel::rowsRemoved,
		[this](const QModelIndex& parent, int first, int last) {
			m_desktopBlankMenu->hideSortStatus();
			viewport()->update();
		});

	connect(this, &QAbstractItemView::clicked, [this](const QModelIndex& index) {
		m_singleCheckedIndex = index;
		update(index);
		qDebug()
			<< "click -> " << index.row();
		});

	connect(this, &HzDesktopIconView::onExternalDrop,
		m_itemModel, &HzDesktopItemModel::handleExternalDrop);

	//connect(this, &QAbstractItemView::activated, [this](const QModelIndex& index) {
	//	m_singleCheckedIndex = index;
	//	update(index);
	//	qDebug()
	//		<< index.row();
	//	});

	connect(m_desktopBlankMenu, &HzDesktopBkgMenu::onNewFile,
		d, &HzDesktopIconViewPrivate::handleMenuNewFile);

	connect(m_itemModel, &HzDesktopItemModel::onFileCreated,
		d, &HzDesktopIconViewPrivate::handleFileCreated);
}

void HzDesktopIconView::initParam()
{
	m_param.dirPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation).replace('/', '\\');
}

QRect HzDesktopIconView::visualRect(const QModelIndex& index) const
{
	// TODO 处理横向时的逻辑
	int posIndexX = index.row() / m_maxViewRow;
	int posIndexY = index.row() % m_maxViewRow;

	QPoint showPos(
		posIndexX * m_param.gridSize.width(),
		posIndexY * m_param.gridSize.height()
	);

	QStyleOptionViewItem option = viewOptions();
	option.rect = QRect(QPoint(0, 0), m_param.iconSize + 2 * m_param.iconMargin);
	// TODO 下面这一行==有崩溃 other是0x7
	option.state.setFlag(QStyle::State_On, index == m_singleCheckedIndex);

	return QRect(showPos, m_itemDelegate->sizeHint(option, index));
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
	// TODO 这里使用itemFromIndex会不会有问题？不过前面已经判断了isValid，应该没问题
	return !index.isValid() || !m_itemModel->itemFromIndex(index)->isEnabled();
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

	QModelIndex index = indexAt(m_pressedPos);
	// TODO 所有单独使用index.isValid()的地方，都要检查一下是否与isIndexHidden冲突？
	if (index.isValid()) {
		QItemSelectionModel::SelectionFlags command = selectionCommand(index, e);
		if (command.testFlag(QItemSelectionModel::Toggle)) {
			command &= ~QItemSelectionModel::Toggle;
			m_ctrlDragSelectionFlag = selectionModel()->isSelected(index) ? QItemSelectionModel::Deselect : QItemSelectionModel::Select;
			command |= m_ctrlDragSelectionFlag;
		}
	}

	if (m_singleCheckedIndex.isValid() && m_singleCheckedIndex != index) {
		QRect preCheckedRect = visualRect(m_singleCheckedIndex);
		removePixmapCache(m_itemModel->filePath(m_singleCheckedIndex));
		m_singleCheckedIndex = QModelIndex();
		// TODO QAbstractItemView::update(const QModelIndex &index) 里就是调用的viewPort的update，
		// 需要确认下有没有考虑了viewPort的offset
		viewport()->update(preCheckedRect);
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
	// 这段逻辑要放在父类处理新的state之前
	QModelIndex index = indexAt(m_pressedPos);
	if (index.isValid() && isIndexHidden(index) && state() != DragSelectingState) {
		clearSelection();
	}

	QAbstractItemView::mouseReleaseEvent(e);

	if (state() != EditingState) {
		setState(NoState);
	}

	if (m_elasticBand.isValid()) {
		viewport()->update(m_elasticBand);
		m_elasticBand = QRect();
	}

	qDebug() << "curr " << currentIndex().row();
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

void HzDesktopIconView::startDrag(Qt::DropActions supportedActions)
{
	HZQ_D(HzDesktopIconView);

	if (isIndexHidden(indexAt(m_pressedPos))) {
		return;
	}

	m_draggedTargetInfo = getCurrentDropTarget();

	QRect rect;
	QModelIndexList indexes = selectedIndexes();
	QPixmap pixmap = d->renderToPixmap(indexes, &rect);
	HzDrag* drag = new HzDrag(this);
	drag->setItemPaths(getSelectedPaths());
	drag->setPixmap(pixmap);
	drag->setHotSpot(m_pressedPos - rect.topLeft());
	Qt::DropAction dropAction = drag->exec(supportedActions);

	if (dropAction == Qt::MoveAction) {

	}
}

void HzDesktopIconView::dragEnterEvent(QDragEnterEvent* e)
{
	//qDebug() << e->mimeData()->urls()[0];

	if (HzDrag::source() == this) {

	}
	else {

	}
}

void HzDesktopIconView::dragMoveEvent(QDragMoveEvent* e)
{
	QAbstractItemView::dragMoveEvent(e);

	const QPoint pos = e->pos();	// TODO 后续再统一处理offset

	m_hoverIndex = indexAt(pos);
	
	if (!m_hoverIndex.isValid() || isIndexHidden(m_hoverIndex) || selectedIndexes().contains(m_hoverIndex)) {
		// 如果不在已显示的图标上、或显示了但同时也是拖拽图标的一员，就根据鼠标位置计算出当前鼠标所处的网格
		m_insertRow = getInsertRow(pos);

		// 如果是自动排序并且插入位置超出了最大row，就认为无效
		if (m_param.bAutoArrange && m_insertRow >= m_itemModel->rowCount()) {
			m_insertRow = -1;
		}
	}
	else {
		m_insertRow = -1;
	}

	qDebug()
		<< m_hoverIndex.row()
		<< m_insertRow;
}

void HzDesktopIconView::dragLeaveEvent(QDragLeaveEvent* e)
{
	QAbstractItemView::dragLeaveEvent(e);

	m_insertRow = -1;
	//qDebug() << e->proposedAction() << e->pos() << m_hoverIndex.row();
}

void HzDesktopIconView::dropEvent(QDropEvent* e)
{
	if (m_insertRow < 0) {
		return;
	}

	if (HzDrag::source() == this) {
		handleInternalDrop(e);
	}
	else {
		handleExternalDrop(e);
	}

	m_insertRow = -1;

	setState(NoState);

	// 删除末尾的占位格；有路径的也不能删，因为是从外部拖入的预备格
	for (int i = m_itemModel->rowCount() - 1; i >= 0; i--) {
		QModelIndex index = m_itemModel->index(i, 0);
		if (!isIndexHidden(index) || !m_itemModel->filePath(index).isEmpty()) {
			break;
		}

		m_itemModel->removeRow(i);
	}

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

	if (this->state() == EditingState) {
		int a = 1;
	}

	QStyleOptionViewItem option = QAbstractItemView::viewOptions();
	QStyleOptionViewItem singleCheckedOption;
	// TODO 这里e->rect()怎么每次都是最大大小
	const QModelIndexList toBeRendered = intersectingSet(e->rect());
	const QModelIndex current = currentIndex();
	const QModelIndex hover = m_hoverIndex;
	//const bool focus = (hasFocus() || viewport()->hasFocus()) && current.isValid();
	// TODO 看看focus是不是在editor上
	const bool focus = true;
	const QStyle::State state = option.state;
	const QAbstractItemView::State viewState = this->state();
	const bool enabled = (state & QStyle::State_Enabled) != 0;

	if (this->state() == EditingState) {
		int a = 1;
	}

	QModelIndexList::const_iterator end = toBeRendered.constEnd();
	for (QModelIndexList::const_iterator it = toBeRendered.constBegin(); it != end; ++it) {
		if (isIndexHidden(*it)) {
			continue;
		}
		
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

		option.state.setFlag(QStyle::State_MouseOver, *it == hover);

		if (focus && current == *it) {
			option.state |= QStyle::State_HasFocus;
			if (viewState == EditingState) {
				option.state |= QStyle::State_Editing;
			}
		}

		option.state.setFlag(QStyle::State_On, false);
		if (*it == m_singleCheckedIndex) {
			option.state.setFlag(QStyle::State_On, true);
			singleCheckedOption = option;
			continue;
		}

		itemDelegate()->paint(&painter, option, *it);
	}

	// 选中的item要最后绘制，以使其在Zorder是最上层
	if (singleCheckedOption.state.testFlag(QStyle::State_On)) {
		removePixmapCache(m_itemModel->filePath(current));
		itemDelegate()->paint(&painter, singleCheckedOption, current);
	}

	// 仅在当前插入位置是有图标显示的网格时，才显示指示线
	if (m_insertRow >= 0 && !isIndexHidden(m_itemModel->index(m_insertRow, 0))) {
		painter.save();
		// 计算出插入线的位置
		QPoint TMP_DELTA(0, -10);
		QRect rect = visualRect(model()->index(m_insertRow, 0));
		painter.setPen(QPen(Qt::white, 3));
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

bool HzDesktopIconView::edit(const QModelIndex& index, EditTrigger trigger, QEvent* event)
{
	if (QAbstractItemView::edit(index, trigger, event)) {
		// 重绘该index使文字消失
		// TODO 这里面要修改索引了，这里用的是枚举，插入时又用的大小
		removePixmapCache(m_itemModel->filePath(index));
		update(index);
		return true;
	}

	return false;
}

IFACEMETHODIMP HzDesktopIconView::QueryInterface(REFIID riid, void** ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(HzDesktopIconView, IDropTarget),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) HzDesktopIconView::AddRef()
{
	return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) HzDesktopIconView::Release()
{
	long cRef = InterlockedDecrement(&_cRef);
	if (!cRef)
		delete this;
	return cRef;
}

DropTargetInfo HzDesktopIconView::getCurrentDropTarget()
{
	DropTargetInfo targetInfo = { FileOrFolder };
	QPoint pos = mapFromGlobal(QCursor::pos());

	QModelIndex index = indexAt(pos);
	if (isIndexHidden(index)) {
		targetInfo.info = m_param.dirPath;
	}
	else {
		targetInfo.info = m_itemModel->filePath(index);
	}

	return targetInfo;
}

bool HzDesktopIconView::filterThisDrag(const DropTargetInfo& targetInfo)
{
	bool bInternalMove =
		(targetInfo.type == FileOrFolder) &&
		(targetInfo.info.toString() == m_param.dirPath);

	bool bSameItem = (targetInfo == m_draggedTargetInfo);

	return bInternalMove || bSameItem;
}

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
	QItemSelection selection;

	if (m_insertRow < 0 || indexAt(m_pressedPos).row() == m_insertRow) {
		return;
	}

	// 自动排序时按照鼠标位置挨个插入进去，否则每一个都根据各自位置进行插入
	if (m_param.bAutoArrange) {
		// 按照row从大到小进行排序，从而先删除大的
		qSort(indexList.begin(), indexList.end(), [](const QModelIndex& index1, const QModelIndex& index2) {
			return index1.row() > index2.row(); });

		int insertRow = m_insertRow;
		QList<QStandardItem*> dropItems;
		for (const QModelIndex& index : indexList) {
			dropItems.push_back(m_itemModel->takeItem(index.row()));
			m_itemModel->removeRow(index.row());

			// 修正insertRow
			if (index.row() <= m_insertRow) {
				insertRow--;
			}
		}

		//auto it = std::upper_bound(indexList.rbegin(), indexList.rend(), indexAt(m_pressedPos));
		//int c = std::distance(indexList.rbegin(), it);
		//insertRow -= std::distance(it, indexList.rend());

		// TODO 拖拽到自身下一格会有问题，和拖到超出最大格，都会有问题
		// 仿照下面的方式
		for (auto it = dropItems.rbegin(); it < dropItems.rend(); it++, insertRow++) {
			m_itemModel->insertRow(insertRow, *it);
			selection.append(QItemSelectionRange(m_itemModel->index(insertRow, 0)));
		}
	}
	else {
		// 按照row从小到大进行排序，从而先插入小的
		qSort(indexList.begin(), indexList.end(), [](const QModelIndex& index1, const QModelIndex& index2) {
			return index1.row() < index2.row(); });

		// 计算所有Item的插入位置，只将插入位置有效的保留下来，其余的从中剔除
		QRect dragStartIndexRect = visualRect(indexAt(m_pressedPos));
		QMap<QStandardItem*, int> insertInfoMap;
		QList<QPersistentModelIndex> invalidDropIndexes;

		// 先将拖拽的所有item disable，防止影响位置判定，后续会恢复
		for (auto& index : indexList) {
			m_itemModel->itemFromIndex(index)->setEnabled(false);
		}

		for (auto& index : indexList) {
			QPoint delta = visualRect(index).topLeft() - dragStartIndexRect.topLeft();
			int insertRow = getInsertRow(e->pos() + delta);
			if (insertRow >= 0) {
				QStandardItem* item = m_itemModel->takeItem(index.row());
				item->setEnabled(true);
				// take之后原先位置创建一个disabled的item
				m_itemModel->itemFromIndex(index)->setEnabled(false);
				insertInfoMap[item] = insertRow;
			}
			else {
				m_itemModel->itemFromIndex(index)->setEnabled(true);
				invalidDropIndexes.push_back(index);
			}
		}

		// TODO 为什么QMap不支持如下迭代方式
		for (auto& [item, insertRow] : insertInfoMap.toStdMap()) {
			m_itemModel->insertItems(insertRow, { item });
			selection.append(QItemSelectionRange(m_itemModel->index(insertRow, 0)));
		}


		for (auto& index : invalidDropIndexes) {
			selection.append(QItemSelectionRange(index));
		}
	}

	// drop结束后重新将拖动的item设置为选中状态
	selectionModel()->select(selection, QItemSelectionModel::Select);

	viewport()->update(viewport()->rect());
}

void HzDesktopIconView::handleExternalDrop(QDropEvent* e)
{
	QItemSelection selection;
	int insertRow = m_insertRow;

	// TODO 测试e->mimeData()->urls()直接放在 : 之后是否相当于单次读取的临时变量
	for (const QUrl& url : e->mimeData()->urls()) {
		QString oriFilePath = url.toLocalFile();

		QString targetFilePath = QDir(m_param.dirPath).absoluteFilePath(QFileInfo(oriFilePath).fileName());
		QStandardItem* item = new QStandardItem();
		item->setData(targetFilePath, CustomRoles::FilePathRole);
		m_itemModel->insertItems(insertRow++, { item });
		emit onExternalDrop(oriFilePath, item);
		selection.append(QItemSelectionRange(m_itemModel->index(insertRow, 0)));

		HZ::moveFile(oriFilePath, targetFilePath, true);
		//if (e->dropAction() == Qt::CopyAction) {
		//	HZ::copyFile(oriFilePath, targetFilePath, true);
		//}
		//else if (e->dropAction() == Qt::MoveAction) {
		//	HZ::moveFile(oriFilePath, targetFilePath, true);
		//}
	}

	// drop结束后重新将拖动的item设置为选中状态
	selectionModel()->select(selection, QItemSelectionModel::Select);
}

void HzDesktopIconView::handleSetIconSizeMode(IconSizeMode mode)
{
	m_param.setIconSizeMode(mode);

	handleLayoutChanged();

	updateEditorGeometries();

	viewport()->update();
}

void HzDesktopIconView::handleEnableAutoArrange()
{
	m_itemModel->removeAllDisableItem();

	// TODO 显示排序菜单的标识
	m_itemModel->setSortRole(m_param.sortRole);
	m_itemModel->sort(0, m_param.sortOrder);
}

void HzDesktopIconView::handleSetItemSortRole(int role)
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
	// TODO 限制在有效网格内？
	if (!geometry().contains(pos, true)) {
		// 代表插入位置无效
		return -1;
	}

	int row = pos.x() / m_param.gridSize.width() * m_maxViewRow
		+ pos.y() / m_param.gridSize.height();

	// 在有图标显示的网格上时，才会按照如下计算偏移
	if (!isIndexHidden(m_itemModel->index(row, 0))
		&& pos.y() % m_param.gridSize.height() > m_param.gridSize.height() / 2) {
		row += 1;
	}

	return row;
}

QModelIndexList HzDesktopIconView::intersectingSet(const QRect& area) const
{
	QModelIndexList ret;

	int rowCount = model()->rowCount();
	for (int i = 0; i < rowCount; ++i) {
		// TODO 这里有崩溃
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