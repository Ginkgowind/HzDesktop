#pragma once

#include <QStyledItemDelegate>
#include <QPixmapCache>

QT_BEGIN_NAMESPACE
class QStandardItem;
QT_END_NAMESPACE

class HzItemDelegate  : public QStyledItemDelegate
{
	Q_OBJECT

public:
	HzItemDelegate(QObject *parent);
	~HzItemDelegate();

	void setUiParam(
		const QSize& iconSize,
		const int iconMargin
	);

protected:
	QSize sizeHint(
		const QStyleOptionViewItem& option,
		const QModelIndex& index
	) const override;

	void paint(
		QPainter* painter, 
		const QStyleOptionViewItem& option, 
		const QModelIndex& index
	) const override;

	//QWidget* createEditor(QWidget* parent,
	//	const QStyleOptionViewItem& option,
	//	const QModelIndex& index
	//) const override;

private:
	QStandardItem* getItemFromParam(
		const QStyleOptionViewItem& option,
		const QModelIndex& index
	) const;

	void paintBackground(
		QPainter* painter,
		const QStyleOptionViewItem& option
	) const;

	void paintText(
		QPainter* painter,
		const QStyleOptionViewItem& option,
		QStandardItem* item
	) const;

	// 不用paint传来的painter，会有很多问题
	QPixmap paintIconText(
		const QStyleOptionViewItem& option,
		QStandardItem* item
	) const;

	inline int calculateTextLine(
		const QRect& rect,
		const QString& text
	) const;

private:
	QPixmapCache m_showPixmapCache;

	QPainter* m_painter;

	QFontMetrics* m_metrics;

	QSize m_iconSize;
	int m_iconMargin;
};
