#pragma once

#include <QPlainTextEdit>
#include <QStyleOptionViewItem>
#include <QPainter>

class HzItemTextEditor  : public QPlainTextEdit
{
	Q_OBJECT

public:
	HzItemTextEditor(QWidget* parent, const QStyleOptionViewItem& option, const QTextOption& textOption);
	~HzItemTextEditor();

	void updateGeometryParam(const QPoint& middleTop, int maxWidth);
private:
	void initEditor(QPlainTextEdit* editor);

	void handleTextChanged();

	QPoint m_middleTop;

	int m_maxWidth;

	static QPainter* s_pMeasurePainter;
};
