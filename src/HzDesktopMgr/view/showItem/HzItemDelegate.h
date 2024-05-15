#pragma once

#include <QStyledItemDelegate>

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
		const int iconMargin,
		const int oneLineHeight
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

	QWidget* createEditor(QWidget* parent,
		const QStyleOptionViewItem& option,
		const QModelIndex& index
	) const override;

private:
	QStandardItem* getItemFromParam(
		const QStyleOptionViewItem& option,
		const QModelIndex& index
	) const;

	void paintBackground(
		QPainter* painter,
		const QStyleOptionViewItem& option
	) const;

	void paintIcon(
		QPainter* painter,
		const QStyleOptionViewItem& option,
		QStandardItem* item
	) const;

	void paintText(
		QPainter* painter,
		const QStyleOptionViewItem& option,
		QStandardItem* item
	) const;

	void setBackgroundPainter(
		QPainter* painter,
		const QStyleOptionViewItem& option
	) const;

private:
	QSize m_iconSize;
	int m_iconMargin;
	int m_oneLineHeight;
};
