#include <QPainter>

#include "HzItemDelegate.h"

#define POLYGON 4   //等腰三角形直角边长
#define WIDTH 1     //分隔符粗细的一半

HzItemDelegate::HzItemDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{}

HzItemDelegate::~HzItemDelegate()
{}

void HzItemDelegate::setItemSize(const QSize& itemSize)
{
	m_itemSize = itemSize;
}

QSize HzItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return m_itemSize;
}

//void HzItemDelegate::paint(
//	QPainter * painter, 
//	const QStyleOptionViewItem & option, 
//	const QModelIndex & index
//) const
//{
//	TestListView* dragView = qobject_cast<TestListView*>(option.styleObject);
//	bool isDraging = dragView->isDraging();
//
//	QRect rect = option.rect;
//
//	painter->setRenderHint(QPainter::Antialiasing, true);
//	painter->setPen(Qt::NoPen);
//
//	if (option.state & (QStyle::State_MouseOver | QStyle::State_Selected)) {
//
//		QStandardItemModel* listModel = qobject_cast<QStandardItemModel*>(dragView->model());
//		TestListViewItem* item = static_cast<TestListViewItem*>(listModel->item(index.row()));
//		item->setIcon(item->Img_hover);
//
//		if (option.state & QStyle::State_MouseOver) {
//		}
//		if (option.state & QStyle::State_Selected) {
//			painter->setBrush(QColor(180, 0, 0));
//			painter->drawRect(rect.topLeft().x(), rect.topLeft().y(), 4, rect.height());
//
//			painter->setBrush(QColor(230, 231, 234));
//			painter->drawRect(rect.topLeft().x() + 4, rect.topLeft().y(), rect.width() - 4, rect.height());
//
//		}
//	}
//	else {
//		QStandardItemModel* listModel = qobject_cast<QStandardItemModel*>(dragView->model());
//		TestListViewItem* item = static_cast<TestListViewItem*>(listModel->item(index.row()));
//		item->setIcon(item->Img);
//	}
//
//	//begin drag
//	if (isDraging) {
//		int theDragRow = dragView->dragRow();
//		int UpRow = dragView->highlightedRow();
//		int DownRow = UpRow + 1;
//		int rowCount = dragView->model()->rowCount() - 1;
//
//		//绘制DropIndicator
//		if (index.row() == UpRow && index.row() != theDragRow - 1 && index.row() != theDragRow) {
//			painter->setBrush(QColor(66, 133, 244));
//
//			if (UpRow == rowCount) {
//				//到达尾部,三角形向上移动一个WIDTH的距离,以使分隔符宽度*2
//				QPolygon trianglePolygon_bottomLeft;
//				trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - (POLYGON + WIDTH) + 1 - WIDTH);
//				trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - WIDTH + 1 - WIDTH);
//				trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x() + POLYGON, rect.bottomLeft().y() - WIDTH + 1 - WIDTH);
//
//				QPolygon trianglePolygon_bottomRight;
//				trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() + 1, rect.bottomRight().y() - (POLYGON + WIDTH) + 1 - WIDTH);
//				trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() + 1, rect.bottomRight().y() - WIDTH + 1 - WIDTH);
//				trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() - POLYGON + 1, rect.bottomRight().y() - WIDTH + 1 - WIDTH);
//
//				painter->drawRect(rect.bottomLeft().x(), rect.bottomLeft().y() - 2 * WIDTH + 1, rect.width(), 2 * WIDTH);  //rect
//				painter->drawPolygon(trianglePolygon_bottomLeft);
//				painter->drawPolygon(trianglePolygon_bottomRight);
//			}
//			else {
//				//正常情况,组成上半部分(+1是根据实际情况修正)
//				QPolygon trianglePolygon_bottomLeft;
//				trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - (POLYGON + WIDTH) + 1);
//				trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - WIDTH + 1);
//				trianglePolygon_bottomLeft << QPoint(rect.bottomLeft().x() + POLYGON, rect.bottomLeft().y() - WIDTH + 1);
//
//				QPolygon trianglePolygon_bottomRight;
//				trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() + 1, rect.bottomRight().y() - (POLYGON + WIDTH) + 1);
//				trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() + 1, rect.bottomRight().y() - WIDTH + 1);
//				trianglePolygon_bottomRight << QPoint(rect.bottomRight().x() - POLYGON + 1, rect.bottomRight().y() - WIDTH + 1);
//
//				painter->drawRect(rect.bottomLeft().x(), rect.bottomLeft().y() - WIDTH + 1, rect.width(), WIDTH);  //rect
//				painter->drawPolygon(trianglePolygon_bottomLeft);
//				painter->drawPolygon(trianglePolygon_bottomRight);
//			}
//		}
//		else if (index.row() == DownRow && index.row() != theDragRow + 1 && index.row() != theDragRow) {
//			painter->setBrush(QColor(66, 133, 244));
//
//			if (DownRow == 0) {
//				//reach the head
//				QPolygon trianglePolygon_topLeft;
//				trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + (POLYGON + WIDTH) + WIDTH);
//				trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + WIDTH + WIDTH);
//				trianglePolygon_topLeft << QPoint(rect.topLeft().x() + POLYGON, rect.topLeft().y() + WIDTH + WIDTH);
//
//				QPolygon trianglePolygon_topRight;
//				trianglePolygon_topRight << QPoint(rect.topRight().x() + 1, rect.topRight().y() + (POLYGON + WIDTH) + WIDTH);
//				trianglePolygon_topRight << QPoint(rect.topRight().x() + 1, rect.topRight().y() + WIDTH + WIDTH);
//				trianglePolygon_topRight << QPoint(rect.topRight().x() - POLYGON + 1, rect.topRight().y() + WIDTH + WIDTH);
//
//				painter->drawRect(rect.topLeft().x(), rect.topLeft().y(), rect.width(), 2 * WIDTH);  //rect
//				painter->drawPolygon(trianglePolygon_topLeft);
//				painter->drawPolygon(trianglePolygon_topRight);
//			}
//			else {
//				//normal
//				QPolygon trianglePolygon_topLeft;
//				trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + (POLYGON + WIDTH));
//				trianglePolygon_topLeft << QPoint(rect.topLeft().x(), rect.topLeft().y() + WIDTH);
//				trianglePolygon_topLeft << QPoint(rect.topLeft().x() + POLYGON, rect.topLeft().y() + WIDTH);
//
//				QPolygon trianglePolygon_topRight;
//				trianglePolygon_topRight << QPoint(rect.topRight().x() + 1, rect.topRight().y() + (POLYGON + WIDTH));
//				trianglePolygon_topRight << QPoint(rect.topRight().x() + 1, rect.topRight().y() + WIDTH);
//				trianglePolygon_topRight << QPoint(rect.topRight().x() - POLYGON + 1, rect.topRight().y() + WIDTH);
//
//				painter->drawRect(rect.topLeft().x(), rect.topLeft().y(), rect.width(), WIDTH);  //rect
//				painter->drawPolygon(trianglePolygon_topLeft);
//				painter->drawPolygon(trianglePolygon_topRight);
//			}
//		}
//		QStyledItemDelegate::paint(painter, option, index);
//		return;
//	}
//	//end drag
//
//	QStyledItemDelegate::paint(painter, option, index);
//}

QWidget* HzItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return nullptr;
}