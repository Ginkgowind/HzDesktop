#pragma once

#include <QStyledItemDelegate>

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
		const QModelIndex& index) const override;

private:
	void paintBackground(
		QPainter* painter,
		const QStyleOptionViewItem& option
	) const;

	void paintIcon();

	void painText();

	void setBackgroundPainter(
		QPainter* painter,
		const QStyleOptionViewItem& option
	) const;

private:
	QSize m_iconSize;
	int m_iconMargin;
	int m_oneLineHeight;
};
