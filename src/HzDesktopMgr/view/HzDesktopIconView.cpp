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

	//setDropIndicatorShown(true); // ��ʾ�Ϸ�λ��ָʾ��

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

	// ���ƣ�Ĭ��ΪCtrl + C
	connect(new QShortcut(QKeySequence::Copy, this), &QShortcut::activated, 
		d, &HzDesktopIconViewPrivate::handleCopy);

	// ���У� Ĭ��ΪCtrl + X
	connect(new QShortcut(QKeySequence::Cut, this), &QShortcut::activated,
		d, &HzDesktopIconViewPrivate::handleCut);

	// ճ���� Ĭ��ΪCtrl + V
	connect(new QShortcut(QKeySequence::Paste, this), &QShortcut::activated,
		d, &HzDesktopIconViewPrivate::handlePaste);

	// ȫѡ�� Ĭ��ΪCtrl + A
	/*QShortcut* pSelectAllShorcut = new QShortcut(QKeySequence(QKeySequence::SelectAll),
		this, SLOT(handleSelectAll()), nullptr, Qt::WidgetWithChildrenShortcut);*/

	// ɾ���� Ĭ��ΪDelete
	connect(new QShortcut(QKeySequence::Delete, this), &QShortcut::activated,
		d, &HzDesktopIconViewPrivate::handleDelete);
	connect(new QShortcut(QKeySequence("Ctrl+D"), this), &QShortcut::activated,
		d, &HzDesktopIconViewPrivate::handleDelete);

	// ˢ�£�Ĭ��ΪF5
	//new QShortcut(QKeySequence(QKeySequence::Refresh),
	//	this, SLOT(handleRefreshFile()), nullptr, Qt::WidgetWithChildrenShortcut);

	// ��������F2
	connect(new QShortcut(QKeySequence(Qt::Key_F2), this), &QShortcut::activated,
		d, &HzDesktopIconViewPrivate::handleRename);

	// ��ѡ���ļ��� Enter
	connect(new QShortcut(QKeySequence(Qt::Key_Return), this), &QShortcut::activated,
		d, &HzDesktopIconViewPrivate::handleOpen);
	connect(new QShortcut(QKeySequence(Qt::Key_Enter), this), &QShortcut::activated,
		d, &HzDesktopIconViewPrivate::handleOpen);

	// item�Ҽ��˵�
	connect(m_itemMenu, &HzItemMenu::onOpen, d, &HzDesktopIconViewPrivate::handleOpen);
	connect(m_itemMenu, &HzItemMenu::onCopy, d, &HzDesktopIconViewPrivate::handleCopy);
	connect(m_itemMenu, &HzItemMenu::onCut, d, &HzDesktopIconViewPrivate::handleCut);
	connect(m_itemMenu, &HzItemMenu::onDelete, d, &HzDesktopIconViewPrivate::handleDelete);
	connect(m_itemMenu, &HzItemMenu::onRename, d, &HzDesktopIconViewPrivate::handleRename);

	// �հ״��Ҽ��˵�
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

	// TODO itemһ�б仯ʱ����menu������״̬
	connect(m_itemModel, &HzDesktopItemModel::itemChanged,
		[this](QStandardItem* item) {m_desktopBlankMenu->hideSortStatus(); });
}

void HzDesktopIconView::initParam()
{
	m_param.dirPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
}

QRect HzDesktopIconView::visualRect(const QModelIndex& index) const
{
	// TODO �������ʱ���߼�
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
		// TODO ���� logical selection mode (key and mouse click selection)
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

	// dropEvent֮�����ǻ��ִ���һ��mouseMoveEvent����������Ҫ���ù�������
	bool bFilterDragSelecting = (state() == DraggingState) ||
		(preState == DraggingState && (m_pressedPos - e->pos()).manhattanLength() > QApplication::startDragDistance());

	if ((e->buttons() & Qt::LeftButton) && !bFilterDragSelecting) {
		// �ο� QAbstractItemView ʵ��״̬�������ѡ�߼�
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

		// �ο�QListView ʵ�ֿ�ѡ�����
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
		// ��������
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
//	 //��ʼ�ϷŲ���
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
			// szTitle���ڰ�����Ŀ�괰�ڵı���
			//_tprintf(TEXT("�Ϸŵ� '%s'\n"), szTitle);
			int a = 1;
		}
	}

	e->accept();

	m_hoverIndex = indexAt(e->pos());

	qDebug() << e->proposedAction() << e->pos() << m_hoverIndex.row();

	const QPoint pos = e->pos() - QPoint(0, 0);
	if (indexAt(pos).isValid()) {
		// TODO ���Զ�����ʱҪ����
	}
	else {
		// �������ǰ�������������
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

	// TODO Ϊʲô����ĺ������������Ҳ������壬�ѵ�����Ϊ���߶���inline��
	// TODO �����Ƿ��б仯���ж��Ƿ�ȡ����ʾ״̬
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

	// TODO ����һ��͸�����ݣ�Ϊʲô�����ƾͻᴩ͸
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

	//e->rect() TODO �������rect������Ҫ������Щmodelindex �Դ����Ż�����
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
		// ����������ߵ�λ��
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

	// TODO �˽�˴�ֱ�ӵ��ú����븴�Ʊ�������ʲô����
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

	// �Զ�����ʱ�������λ�ð��������ȥ������ÿһ�������ݸ���λ�ý��в���
	if (m_param.bAutoArrange) {
		// ����row�Ӵ�С�������򣬴Ӷ���ɾ�����
		qSort(indexList.begin(), indexList.end(), [](const QModelIndex& index1, const QModelIndex& index2) {
			return index1.row() > index2.row(); });

		QList<QStandardItem*> dropItems;
		for (const QModelIndex& index : indexList) {
			dropItems.push_back(m_itemModel->takeItem(index.row()));
			m_itemModel->removeRow(index.row());
		}

		// TODO ��ק��������һ���������
		for (auto it = dropItems.rbegin(); it < dropItems.rend(); it++) {
			m_itemModel->insertRow(insertRow++, *it);
		}
	}
	else {
		// ����row��С����������򣬴Ӷ��Ȳ���С��
		qSort(indexList.begin(), indexList.end(), [](const QModelIndex& index1, const QModelIndex& index2) {
			return index1.row() < index2.row(); });

		QRect dragStartIndexRect = visualRect(indexAt(m_pressedPos));
		//QMap<QPersistentModelIndex, QRect> indexRectMap;
		QMap<QModelIndex, QRect> indexRectMap;
		for (auto& index : indexList) {
			indexRectMap[index] = visualRect(index);
		}

		// TODO ΪʲôQMap��֧�����µ�����ʽ
		for (auto& [index, rect] : indexRectMap.toStdMap()) {
			QStandardItem* item = m_itemModel->takeItem(index.row());
			// TODO ��ϸ���˽�itemFromIndex��item�������Լ�root��parent
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
	// ����û��ʹ��QSortFilterProxyModel����Ϊ����������֮���ܽ����϶�ͼ�꣬
	// ����QSortFilterProxyModel�����Ƕ�ԭʼmodel��ӳ�䣬����������������

	// ��������ʱ����ɾ�������е�ռλitem
	m_itemModel->removeAllDisableItem();

	m_itemModel->setSortRole(role);
	m_itemModel->sort(0, m_param.sortOrder);
}

void HzDesktopIconView::handleSetItemSortOrder(Qt::SortOrder order)
{
	// ��������ʱ����ɾ�������е�ռλitem
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
	// TODO ����ΪʲôҪnormalized
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