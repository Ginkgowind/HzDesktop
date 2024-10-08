#include <QApplication>
#include <QClipboard>
#include <QStandardPaths>
#include <QFileInfo>
#include <QPainter>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QTimer>
#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>

#include "HzDesktopIconView.h"
#include "HzDesktopIconView_p.h"
#include "windows/tools.h"

HzDesktopIconViewPrivate::HzDesktopIconViewPrivate()
	: m_bNewFileByMenu(false)
{
	initDragCursors();
}

HzDesktopIconViewPrivate::~HzDesktopIconViewPrivate()
{
}

void HzDesktopIconViewPrivate::initDragCursors()
{
	QFont font("Microsoft YaHei", 8);

	QPen textPen(Qt::blue);
	textPen.setWidth(1);

	QPen borderPen(Qt::black);
	borderPen.setWidth(1);

	QPainter painter;

	auto drawFunc = [&](QPixmap pixmap, const QString& text) -> QPixmap
		{
			painter.begin(&pixmap);
			painter.fillRect(pixmap.rect(), Qt::white);
			painter.setFont(font);
			painter.setPen(textPen);
			painter.drawText(pixmap.rect(), Qt::AlignCenter, text);
			painter.setPen(borderPen);
			painter.drawRect(0, 0, pixmap.width() - 1, pixmap.height() - 1);
			painter.end();

			return pixmap;
		};

	m_dragCursorMap[Qt::CopyAction] = drawFunc(QPixmap(50, 22), QStringLiteral(" + 复制 "));
	m_dragCursorMap[Qt::MoveAction] = drawFunc(QPixmap(50, 22), QStringLiteral(" > 移动 "));
	if (HZ::getSystemVersion() == 11) {
		m_dragCursorMap[Qt::LinkAction] = drawFunc(QPixmap(100, 22), QStringLiteral(" * 创建快捷方式 "));
	}
}

QPixmap HzDesktopIconViewPrivate::renderToPixmap(const QModelIndexList& indexes, QRect* r) const
{
	HZQ_Q(const HzDesktopIconView);

	Q_ASSERT(r);
	QItemViewPaintPairs paintPairs = draggablePaintPairs(indexes, r);
	if (paintPairs.isEmpty()) {
		return QPixmap();
	}

	QPixmap pixmap(r->size());
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	QStyleOptionViewItem option = q->viewOptions();
	for (int j = 0; j < paintPairs.count(); ++j) {
		option.rect = paintPairs.at(j).rect.translated(-r->topLeft());
		const QModelIndex& current = paintPairs.at(j).index;
		//adjustViewOptionsForIndex(&option, current);
		q->itemDelegate(current)->paint(&painter, option, current);
	}

	return pixmap;
}

QItemViewPaintPairs HzDesktopIconViewPrivate::draggablePaintPairs(const QModelIndexList& indexes, QRect* r) const
{
	Q_ASSERT(r);
	Q_Q(const HzDesktopIconView);
	QRect& rect = *r;
	const QRect viewportRect = q->viewport()->rect();
	QItemViewPaintPairs ret;
	for (const auto& index : indexes) {
		const QRect current = q->visualRect(index);
		if (current.intersects(viewportRect)) {
			ret.append({ current, index });
			rect |= current;
		}
	}
	QRect clipped = rect & viewportRect;
	rect.setLeft(clipped.left());
	rect.setRight(clipped.right());
	return ret;
}

void HzDesktopIconViewPrivate::handleOpen()
{
	HZQ_Q(HzDesktopIconView);

	QModelIndexList indexList = q->selectedIndexes();
	for (const QModelIndex& aindex : indexList) {
		QDesktopServices::openUrl(QUrl::fromLocalFile(q->m_itemModel->filePath(aindex)));
	}
}

void HzDesktopIconViewPrivate::handleRename()
{
	HZQ_Q(HzDesktopIconView);

	QModelIndexList indexes = q->selectedIndexes();
	if (indexes.empty()) {
		return;
	}

	q->QAbstractItemView::edit(indexes[0]);
}

void HzDesktopIconViewPrivate::handleSelectAll()
{
	HZQ_Q(HzDesktopIconView);

	QItemSelection selection;
	for (int i = 0; i < q->model()->rowCount(); i++) {
		const QModelIndex& index = q->model()->index(i, 0);
		if (q->isIndexHidden(index)) {
			continue;
		}
		selection.append(QItemSelectionRange(index));
	}

	q->selectionModel()->select(selection, QItemSelectionModel::Select);
}

void HzDesktopIconViewPrivate::handleMenuNewFile()
{
	m_bNewFileByMenu = true;

	// 设置有效期
	QTimer::singleShot(100, this, [this]() {
		m_bNewFileByMenu = false;
	});
}

void HzDesktopIconViewPrivate::handleFileCreated(const QModelIndex& index)
{
	HZQ_Q(HzDesktopIconView);

	bool bNewFileByMenu = m_bNewFileByMenu;
	// 文件创建时可能还未绘制处理，故此处延时执行
	QTimer::singleShot(60, this, [=]() {
		q->setCurrentIndex(index);
		if (bNewFileByMenu) {
			q->QAbstractItemView::edit(index);
		}
	});
}