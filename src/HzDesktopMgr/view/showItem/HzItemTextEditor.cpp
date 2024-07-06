#include <qDebug>

#include "HzItemTextEditor.h"

QPlainTextEdit* HzItemTextEditor::s_pMeasureEditor = nullptr;

HzItemTextEditor::HzItemTextEditor(QWidget* parent)
	: QPlainTextEdit(parent)
{
	initEditor(this);
	if (s_pMeasureEditor == nullptr) {
		s_pMeasureEditor = new QPlainTextEdit(parent);
		initEditor(s_pMeasureEditor);
		s_pMeasureEditor->show();
	}

	connect(this, &QPlainTextEdit::textChanged, this, &HzItemTextEditor::handleTextChanged);
}

HzItemTextEditor::~HzItemTextEditor()
{}

void HzItemTextEditor::setMiddleTop(const QPoint & pos)
{
	m_middleTop = pos;
}

void HzItemTextEditor::setMeasureEditorWidth(int width)
{
	s_pMeasureEditor->setFixedWidth(width);
}

void HzItemTextEditor::initEditor(QPlainTextEdit* editor)
{
	editor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	editor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	editor->setContextMenuPolicy(Qt::PreventContextMenu);

	// TODO 了解setLineWrapMode的区别
	editor->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

	QTextOption option = editor->document()->defaultTextOption();
	option.setAlignment(Qt::AlignHCenter);
	editor->document()->setDefaultTextOption(option);

	//editor->document()->setDocumentMargin(0);
}

void HzItemTextEditor::handleTextChanged()
{
	s_pMeasureEditor->setPlainText(toPlainText());

	// 目前测试的 currentTextSize.y() 是文本行数
	QSize currentTextSize = s_pMeasureEditor->document()->size().toSize();
	int lineNum = currentTextSize.rheight() + blockCount();
	currentTextSize.rheight() = lineNum * fontMetrics().lineSpacing();
	currentTextSize.rwidth() += 2;

	// TODO 文字周边还是有空白边距，最下面还会多一行空行，想办法都去掉

	setGeometry(
		m_middleTop.x() - currentTextSize.rwidth() / 2,
		m_middleTop.y(),
		currentTextSize.rwidth(),
		currentTextSize.rheight()
	);

	qDebug()
		<< currentTextSize
		<< blockCount()
		<< toPlainText();
}