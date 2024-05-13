#pragma once

#include <QStyledItemDelegate>

class HzItemDelegate  : public QStyledItemDelegate
{
	Q_OBJECT

public:
	HzItemDelegate(QObject *parent);
	~HzItemDelegate();

	void setItemSize(const QSize& itemSize);

protected:
	QSize sizeHint(
		const QStyleOptionViewItem& option,
		const QModelIndex& index
	) const override;

	//void paint(
	//	QPainter* painter, 
	//	const QStyleOptionViewItem& option, 
	//	const QModelIndex& index
	//) const override;

	QWidget* createEditor(QWidget* parent,
		const QStyleOptionViewItem& option,
		const QModelIndex& index) const override;

private:
	QSize m_itemSize;
};
