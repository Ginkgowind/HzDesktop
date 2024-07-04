#pragma once

#include <QPlainTextEdit>

class HzItemTextEditor  : public QPlainTextEdit
{
	Q_OBJECT

public:
	HzItemTextEditor(QWidget *parent);
	~HzItemTextEditor();

	static void setMeasureEditorWidth(int width);

private:
	void initEditor(QPlainTextEdit* editor);

	void handleTextChanged();

	int m_linHeight;

	static QPlainTextEdit* s_pMeasureEditor;
};
