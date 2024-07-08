#pragma once

#include <QStyledItemDelegate>
#include <QPixmapCache>
#include <QTextOption>

#include "config/HzDesktopParam.h"

QT_BEGIN_NAMESPACE
class QStandardItem;
QT_END_NAMESPACE

class HzItemDelegate  : public QStyledItemDelegate
{
	Q_OBJECT

public:
	HzItemDelegate(QObject *parent, HzDesktopParam* param);
	~HzItemDelegate();

	QSize sizeHint(
		const QStyleOptionViewItem& option,
		const QModelIndex& index
	) const override;

protected:
	void paint(
		QPainter* painter, 
		const QStyleOptionViewItem& option, 
		const QModelIndex& index
	) const override;

	QWidget* createEditor(QWidget* parent,
		const QStyleOptionViewItem& option,
		const QModelIndex& index
	) const override;

	void updateEditorGeometry(
		QWidget* editor,
		const QStyleOptionViewItem& option,
		const QModelIndex& index
	) const override;

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

	// 不用paint传来的painter，会有很多问题
	QPixmap paintIconText(
		const QStyleOptionViewItem& option,
		const HzDesktopParam& param,
		QStandardItem* item
	) const;

private:

	QPainter* m_painter;

	QTextOption m_textOption;
};
