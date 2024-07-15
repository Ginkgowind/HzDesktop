#pragma once

#include <QPlainTextEdit>
#include <QStyleOptionViewItem>
#include <QPainter>

#define EDITOR_X_ADJUSTMENT 10
#define EDITOR_Y_ADJUSTMENT 5

class HzItemTextEditor  : public QPlainTextEdit
{
	Q_OBJECT

public:
	HzItemTextEditor(QWidget* parent, const QStyleOptionViewItem& option, const QTextOption& textOption);
	~HzItemTextEditor();

	void updateGeometryParam(const QPoint& middleTop, int maxWidth);

private:

	void handleTextChanged();

	QPoint m_middleTop;

	int m_maxWidth;

	static QPainter* s_pMeasurePainter;
};
