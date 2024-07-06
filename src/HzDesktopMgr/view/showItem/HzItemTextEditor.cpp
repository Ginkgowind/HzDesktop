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

	// TODO �˽�setLineWrapMode������
	editor->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

	QTextOption option = editor->document()->defaultTextOption();
	option.setAlignment(Qt::AlignHCenter);
	editor->document()->setDefaultTextOption(option);

	//editor->document()->setDocumentMargin(0);
}

void HzItemTextEditor::handleTextChanged()
{
	s_pMeasureEditor->setPlainText(toPlainText());

	// Ŀǰ���Ե� currentTextSize.y() ���ı�����
	QSize currentTextSize = s_pMeasureEditor->document()->size().toSize();
	int lineNum = currentTextSize.rheight() + blockCount();
	currentTextSize.rheight() = lineNum * fontMetrics().lineSpacing();
	currentTextSize.rwidth() += 2;

	// TODO �����ܱ߻����пհױ߾࣬�����滹���һ�п��У���취��ȥ��

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