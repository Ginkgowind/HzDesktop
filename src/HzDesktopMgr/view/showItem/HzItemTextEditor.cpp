#include <qDebug>

#include "HzItemTextEditor.h"

QPainter* HzItemTextEditor::s_pMeasurePainter = nullptr;

HzItemTextEditor::HzItemTextEditor(QWidget* parent, const QStyleOptionViewItem& option, const QTextOption& textOption)
	: QPlainTextEdit(parent)
{
	// TODO 了解WordWrapMode与LineWrapMode的区别
	document()->setDefaultTextOption(textOption);
	document()->setDocumentMargin(0);
	document()->setDefaultFont(option.font);

	// TODO 编辑时的文字好像和最后view上显示大小不同，
	// 假如中间有个空格，整个字符串长度又正好满时，
	// 退出编辑就会换行且计算行数有误，导致最下面一行显示不出来
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
	// textLimitRect的高度要足够大，measurePixmap的高度无所谓
	QRectF textLimitRect(0, 0, m_maxWidth, 200);
	QPixmap measurePixmap(m_maxWidth, 10);

	s_pMeasurePainter->begin(&measurePixmap);
	// TODO构造函数里已经设置过了为什么这里还要设置？
	s_pMeasurePainter->setFont(font());

	QSize currentTextSize = s_pMeasurePainter->boundingRect(
		textLimitRect, toPlainText(), document()->defaultTextOption()).toRect().size();
	// TODO 不加下面这一行多出的空白行是什么
	currentTextSize.rheight() += 5;
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
		<< document()->size()
		<< document()->pageSize();

	s_pMeasurePainter->end();
}