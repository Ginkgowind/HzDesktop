#pragma once

#include <QStyledItemDelegate>
#include <QPixmapCache>

#include "config/HzDesktopParam.h"

QT_BEGIN_NAMESPACE
class QStandardItem;
QT_END_NAMESPACE

class HzItemDelegate  : public QStyledItemDelegate
{
	Q_OBJECT

public:
	HzItemDelegate(QObject *parent);
	~HzItemDelegate();

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
	QStandardItem* getItemFromOption(
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

	// ����paint������painter�����кܶ�����
	QPixmap paintIconText(
		const QStyleOptionViewItem& option,
		const HzDesktopParam& param,
		QStandardItem* item
	) const;

private:
	QPixmapCache m_showPixmapCache;

	QPainter* m_painter;

	QFontMetrics* m_metrics;
};
