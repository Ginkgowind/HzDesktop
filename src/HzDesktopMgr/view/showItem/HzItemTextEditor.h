#pragma once

#include <QPlainTextEdit>
#include <QHBoxLayout>

class HzItemTextEditor  : public QPlainTextEdit
{
	Q_OBJECT

public:
	HzItemTextEditor(QWidget *parent);
	~HzItemTextEditor();

	void setMiddleTop(const QPoint& pos);

	static void setMeasureEditorWidth(int width);

private:
	void initEditor(QPlainTextEdit* editor);

	void handleTextChanged();

	QPoint m_middleTop;

	int m_lineHeight;

	static QPlainTextEdit* s_pMeasureEditor;
};
