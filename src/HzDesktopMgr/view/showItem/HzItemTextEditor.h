#pragma once

#include <QPlainTextEdit>

class HzItemTextEditor  : public QPlainTextEdit
{
	Q_OBJECT

public:
	HzItemTextEditor(QWidget *parent);
	~HzItemTextEditor();
};
