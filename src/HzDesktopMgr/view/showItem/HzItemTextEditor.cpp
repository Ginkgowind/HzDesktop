#include <qDebug>

#include "HzItemTextEditor.h"

QPainter* HzItemTextEditor::s_pMeasurePainter = nullptr;

HzItemTextEditor::HzItemTextEditor(QWidget* parent, const QStyleOptionViewItem& option, const QTextOption& textOption)
	: QPlainTextEdit(parent)
{
	// TODO �˽�WordWrapMode��LineWrapMode������
	document()->setDefaultTextOption(textOption);
	document()->setDocumentMargin(0);
	document()->setDefaultFont(option.font);

	// TODO �༭ʱ�����ֺ�������view����ʾ��С��ͬ��
	// �����м��и��ո������ַ���������������ʱ��
	// �˳��༭�ͻỻ���Ҽ����������󣬵���������һ����ʾ������
	setFont(option.font);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setContextMenuPolicy(Qt::PreventContextMenu);

	if (s_pMeasurePainter == nullptr) {
		s_pMeasurePainter = new QPainter();
		s_pMeasurePainter->setFont(option.font);
	}

	connect(this, &QPlainTextEdit::textChanged, this, &HzItemTextEditor::handleTextChanged);
}

HzItemTextEditor::~HzItemTextEditor()
{}

void HzItemTextEditor::updateGeometryParam(const QPoint& middleTop, int maxWidth)
{
	m_middleTop = middleTop;

	m_maxWidth = maxWidth;
}

void HzItemTextEditor::handleTextChanged()
{
	// textLimitRect��measurePixmap�ĸ߶ȶ�����ν
	QRectF textLimitRect(0, 0, m_maxWidth, 200);
	QPixmap measurePixmap(m_maxWidth, 10);

	s_pMeasurePainter->begin(&measurePixmap);
	// TODO���캯�����Ѿ����ù���Ϊʲô���ﻹҪ���ã�
	s_pMeasurePainter->setFont(font());

	QSize currentTextSize = s_pMeasurePainter->boundingRect(
		textLimitRect, toPlainText(), document()->defaultTextOption()).toRect().size();
	// ������͸߶�Ϊһ��
	currentTextSize.rheight() = qMax(currentTextSize.rheight(), QFontMetrics(font()).lineSpacing());
	// TODO ����������һ�ж���Ŀհ�����ʲô
	currentTextSize.rwidth() += EDITOR_X_ADJUSTMENT;
	currentTextSize.rheight() += EDITOR_Y_ADJUSTMENT;

	setGeometry(
		m_middleTop.x() - currentTextSize.rwidth() / 2,
		m_middleTop.y(),
		currentTextSize.rwidth(),
		currentTextSize.rheight()
	);

	qDebug()
		<< currentTextSize
		<< document()->size()
		<< document()->pageSize();

	s_pMeasurePainter->end();
}