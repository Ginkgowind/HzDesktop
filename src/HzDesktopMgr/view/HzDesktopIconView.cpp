#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>
#include <QUrl>
#include <QMenu>
#include <QDebug>
#include <QPainter>
#include <QApplication>
#include <QClipboard>
#include <QShortcut>
#include <QDesktopServices>
#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>

#include <QFileSystemModel>

#include "HzDesktopIconView.h"
#include "HzDesktopIconView_p.h"
#include "showItem/HzItemDelegate.h"
#include "showItem/HzFileItem.h"
#include "windows/UiOperation.h"
#include "windows/tools.h"

#define TEXT_MAX_HEIGHT			40

HzDesktopIconView::HzDesktopIconView(QWidget *parent)
	: QAbstractItemView(parent)
	, HzDesktopPublic(new HzDesktopIconViewPrivate())
	, m_ctrlDragSelectionFlag(QItemSelectionModel::NoUpdate)
{
	setFixedSize(1200, 800);

	m_desktopBlankMenu = new HzDesktopBlankMenu(this);

	//m_itemProxyModel = new QSortFilterProxyModel(this);
	//m_itemModel = new HzDesktopItemModel(m_itemProxyModel);
	//m_itemProxyModel->setSourceModel(m_itemModel);
	//setModel(m_itemProxyModel);

	m_itemModel = new HzDesktopItemModel(this);
	setModel(m_itemModel);

	m_itemDelegate = new HzItemDelegate(this);
	setItemDelegate(m_itemDelegate);

	m_itemMenu = new HzItemMenu(this);

	//setDropIndicatorShown(true); // 显示拖放位置指示器

	//setStyleSheet("QAbstractItemView {background-color: transparent;}");
	//setAttribute(Qt::WA_TranslucentBackground, true);
	//setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);

	setDragEnabled(true);
	setAcceptDrops(true);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	viewport()->setAttribute(Qt::WA_Hover, true);

	setEditTriggers(EditKeyPressed);

	//connect(m_desktopBlankMenu, &HzDesktopBlankMenu::refreshDesktopItemsSignal,
	//	m_itemModel, &HzDesktopItemModel::refreshItems);

	initSignalAndSlot();

	handleLayoutChanged();
}

HzDesktopIconView::~HzDesktopIconView()
{

}

void HzDesktopIconView::initSignalAndSlot()
{
	// TODO 为什么使用SLOT宏时，函数必须要用slots来声明？

	// 复制，默认为Ctrl + C
	new QShortcut(QKeySequence(QKeySequence::Copy), this,
		SLOT(onCopy()), nullptr, Qt::WidgetWithChildrenShortcut);

	// 粘贴， 默认为Ctrl + V
	new QShortcut(QKeySequence::Paste, this, 
		SLOT(onPaste()), nullptr, Qt::WindowShortcut);

	// 剪切， 默认为Ctrl + X
	new QShortcut(QKeySequence(QKeySequence::Cut), this, 
		SLOT(onCut()), nullptr, Qt::WidgetWithChildrenShortcut);

	// 全选， 默认为Ctrl + A
	/*QShortcut* pSelectAllShorcut = new QShortcut(QKeySequence(QKeySequence::SelectAll),
		this, SLOT(handleSelectAll()), nullptr, Qt::WidgetWithChildrenShortcut);*/

	// 删除， 默认为Delete
	new QShortcut(QKeySequence(QKeySequence::Delete), this, 
		SLOT(onDelete()), nullptr, Qt::WidgetWithChildrenShortcut);
	new QShortcut(QKeySequence("Ctrl+D"), this,
		SLOT(onDelete()), nullptr, Qt::WidgetWithChildrenShortcut);

	// 刷新，默认为F5
	//new QShortcut(QKeySequence(QKeySequence::Refresh),
	//	this, SLOT(handleRefreshFile()), nullptr, Qt::WidgetWithChildrenShortcut);

	// 重命名，F2
	new QShortcut(QKeySequence(Qt::Key_F2),
		this, SLOT(onRename()), nullptr, Qt::WidgetWithChildrenShortcut);

	// 打开选中文件， Enter
	new QShortcut(QKeySequence(Qt::Key_Return), this, 
		SLOT(onOpen()), nullptr, Qt::WidgetWithChildrenShortcut);
	new QShortcut(QKeySequence(Qt::Key_Enter), this, 
		SLOT(onOpen()), nullptr, Qt::WidgetWithChildrenShortcut);

	// item右键菜单
	connect(m_itemMenu, &HzItemMenu::onOpen, this, &HzDesktopIconView::onOpen);
	connect(m_itemMenu, &HzItemMenu::onCopy, this, &HzDesktopIconView::onCopy);
	connect(m_itemMenu, &HzItemMenu::onCut, this, &HzDesktopIconView::onCut);
	connect(m_itemMenu, &HzItemMenu::onDelete, this, &HzDesktopIconView::onDelete);
	connect(m_itemMenu, &HzItemMenu::onRename, this, &HzDesktopIconView::onRename);

	// 空白处右键菜单
	connect(m_desktopBlankMenu, &HzDesktopBlankMenu::onHide, [this]() {setVisible(false); });
	connect(m_desktopBlankMenu, &HzDesktopBlankMenu::onSetIconSizeMode,
		this, &HzDesktopIconView::handleSetIconSizeMode);
	connect(m_desktopBlankMenu, &HzDesktopBlankMenu::onSetItemSortMode,
		this, &HzDesktopIconView::handleSetItemSortMode);
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
	return !m_itemModel->item(index.row())->isEnabled();
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
	if (selectedIndexes().empty()) {
		// 隐藏桌面
		setVisible(false);
	}
	else {
		onOpen();
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

void HzDesktopIconView::dropEvent(QDropEvent* e)
{
	if (e->source() == this) {
		handleInternalDrop(e);
	}
	else {
		handleExternalDrop(e);
	}

	setState(NoState);
}

void HzDesktopIconView::contextMenuEvent(QContextMenuEvent* event)
{
	QStringList selectedPathList = getSelectedPaths();

	if (selectedPathList.empty()) {
		m_desktopBlankMenu->exec(QCursor::pos());
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

void HzDesktopIconView::handleLayoutChanged()
{
	m_maxViewRow = height() / m_param.gridSize.height();
	m_maxViewColumn = width() / m_param.gridSize.width();
}

void HzDesktopIconView::handleInternalDrop(QDropEvent* e)
{
	QModelIndexList indexList = selectedIndexes();
	QList<QStandardItem*> dropItems;

	// 按照row从大到小进行排序
	qSort(indexList.begin(), indexList.end(), [](const QModelIndex& index1, const QModelIndex& index2) {
		return index1.row() > index2.row(); });

	for (const QModelIndex& index : indexList) {
		dropItems.push_back(m_itemModel->takeItem(index.row()));
		m_itemModel->removeRow(index.row());
	}

	// 获取鼠标位置对应的row
	QPoint dropPos = e->pos();
	int insertRow = dropPos.x() / m_param.gridSize.width() * m_maxViewRow
		+ dropPos.y() / m_param.gridSize.height();

	m_itemModel->insertItems(insertRow, dropItems);

	//updateGeometries();
	viewport()->update(viewport()->rect());
}

void HzDesktopIconView::handleExternalDrop(QDropEvent* e)
{
}

void HzDesktopIconView::handleSetIconSizeMode(IconSizeMode mode)
{
	m_param.setIconSizeMode(mode);

	handleLayoutChanged();

	update();
}

void HzDesktopIconView::handleSetItemSortMode(ItemSortMode mode)
{
}

void HzDesktopIconView::onOpen()
{
	QModelIndexList indexList = selectedIndexes();
	for (const QModelIndex& aindex : indexList) {
		QDesktopServices::openUrl(QUrl::fromLocalFile(m_itemModel->filePath(aindex)));
	}
}

void HzDesktopIconView::onCopy()
{
	QMimeData* mimeData = new QMimeData;
	QList<QUrl> urls;
	QModelIndexList indexList = selectedIndexes();

	for (const QModelIndex& index : indexList) {
		urls.append(QUrl::fromLocalFile(m_itemModel->filePath(index)));
	}

	mimeData->setUrls(urls);
	int dropEffect = 5; // 2 for cut and 5 for copy
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream.setByteOrder(QDataStream::LittleEndian);
	stream << dropEffect;
	mimeData->setData("Preferred DropEffect", data);
	QApplication::clipboard()->setMimeData(mimeData);
}

void HzDesktopIconView::onCut()
{
	QMimeData* mimeData = new QMimeData;
	QList<QUrl> urls;
	QModelIndexList indexList = selectedIndexes();

	for (const QModelIndex& index : indexList) {
		urls.append(QUrl::fromLocalFile(m_itemModel->filePath(index)));
	}

	mimeData->setUrls(urls);
	int dropEffect = 2; // 2 for cut and 5 for copy
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream.setByteOrder(QDataStream::LittleEndian);
	stream << dropEffect;
	mimeData->setData("Preferred DropEffect", data);
	QApplication::clipboard()->setMimeData(mimeData);
}

void HzDesktopIconView::onPaste()
{
	QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

	const QMimeData* mimeData = QApplication::clipboard()->mimeData();
	if (!mimeData->hasUrls() ||
		!mimeData->hasFormat("Preferred DropEffect")) {
		return;
	}

	int dropEffect = 0;
	QByteArray data = mimeData->data("Preferred DropEffect");
	QDataStream stream(&data, QIODevice::ReadOnly);
	stream.setByteOrder(QDataStream::LittleEndian);
	stream >> dropEffect;
	QList<QUrl> urls = mimeData->urls();
	if (dropEffect != 2 && dropEffect != 5) return;
	QStringList srcs;
	for (QUrl url : urls) {
		if (!url.isLocalFile()) continue;
		srcs << url.toLocalFile();
	}
	if (dropEffect == 2) QApplication::clipboard()->clear();

	for (QString src : srcs) {
		QFileInfo srcFileInfo(src);
		QFileInfo dstFileInfo(QDir(desktopPath), srcFileInfo.fileName());
		if (srcFileInfo.isFile()) {
			// TODO 暂时先不处理这个，先处理拖拽的逻辑
			// 考虑到格子的实现，这些可能要放到公用里
			//CopySingleFile(src, dstFileInfo.absoluteFilePath(), move);
		}
		else {
			//CopyDir(src, dstFileInfo.absoluteFilePath(), move);
		}
	}
	//if (inNetwork()) onRefresh();    //由于性能的原因，网络文件的增删不会被Model感知，需要手动刷新
}

void HzDesktopIconView::onDelete()
{
	QModelIndexList indexList = selectedIndexes();

	for (const QModelIndex& index : indexList) {
		FILEOP_FLAGS dwOpFlags = FOF_ALLOWUNDO | FOF_NO_UI;

		SHFILEOPSTRUCTA fileOp = { 0 };
		fileOp.hwnd = NULL;
		fileOp.wFunc = FO_DELETE; ///> 文件删除操作
		fileOp.pFrom = StrDupA(m_itemModel->filePath(index).toStdString().c_str());
		fileOp.pTo = NULL;
		fileOp.fFlags = dwOpFlags;
		fileOp.hNameMappings = NULL;
		fileOp.lpszProgressTitle = "hz delete file";

		SHFileOperationA(&fileOp);
	}
}

void HzDesktopIconView::onRename()
{
}

void HzDesktopIconView::initItemsPos()
{
	int rowCount = model()->rowCount();
	for (int i = 0; i < rowCount; ++i) {

	}
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
	QVector<QModelIndex>::const_iterator it = intersectVector.begin();
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