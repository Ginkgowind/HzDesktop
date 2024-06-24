#include <QApplication>
#include <QClipboard>
#include <QStandardPaths>
#include <QFileInfo>
#include <QPainter>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>

#include "HzDesktopIconView.h"
#include "HzDesktopIconView_p.h"
#include "windows/tools.h"

HzDesktopIconViewPrivate::HzDesktopIconViewPrivate()
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
	if (paintPairs.isEmpty())
		return QPixmap();

	// mac的retina显示才需要考虑此问题
	//QWindow* window = q->windowHandle(WindowHandleMode::Closest);
	//const qreal scale = window ? window->devicePixelRatio() : qreal(1);
	//QPixmap pixmap(r->size() * scale);
	//pixmap.setDevicePixelRatio(scale);

	QPixmap pixmap(r->size());

	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	QStyleOptionViewItem option = q->viewOptions();
	option.state |= QStyle::State_Selected;
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

void HzDesktopIconViewPrivate::handleCopy()
{
	HZQ_Q(HzDesktopIconView);

	QMimeData* mimeData = new QMimeData;
	QList<QUrl> urls;
	QModelIndexList indexList = q->selectedIndexes();

	for (const QModelIndex& index : indexList) {
		urls.append(QUrl::fromLocalFile(q->m_itemModel->filePath(index)));
	}

	mimeData->setUrls(urls);
	int dropEffect = 5; // 2 for cut and 5 for copy
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream.setByteOrder(QDataStream::LittleEndian);
	stream << dropEffect;
	mimeData->setData("Preferred DropEffect", data);
	QApplication::clipboard()->setMimeData(mimeData);
}

void HzDesktopIconViewPrivate::handleCut()
{
	HZQ_Q(HzDesktopIconView);

	QMimeData* mimeData = new QMimeData;
	QList<QUrl> urls;
	QModelIndexList indexList = q->selectedIndexes();

	for (const QModelIndex& index : indexList) {
		urls.append(QUrl::fromLocalFile(q->m_itemModel->filePath(index)));
	}

	mimeData->setUrls(urls);
	int dropEffect = 2; // 2 for cut and 5 for copy
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream.setByteOrder(QDataStream::LittleEndian);
	stream << dropEffect;
	mimeData->setData("Preferred DropEffect", data);
	QApplication::clipboard()->setMimeData(mimeData);
}

void HzDesktopIconViewPrivate::handlePaste()
{
	HZQ_Q(HzDesktopIconView);

	QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

	const QMimeData* mimeData = QApplication::clipboard()->mimeData();
	if (!mimeData->hasUrls() ||
		!mimeData->hasFormat("Preferred DropEffect")) {
		return;
	}

	int dropEffect = 0;
	QByteArray data = mimeData->data("Preferred DropEffect");
	QDataStream stream(&data, QIODevice::ReadOnly);
	stream.setByteOrder(QDataStream::LittleEndian);
	stream >> dropEffect;
	QList<QUrl> urls = mimeData->urls();
	if (dropEffect != 2 && dropEffect != 5) return;
	QStringList srcs;
	for (QUrl url : urls) {
		if (!url.isLocalFile()) continue;
		srcs << url.toLocalFile();
	}
	if (dropEffect == 2) QApplication::clipboard()->clear();

	for (QString src : srcs) {
		QFileInfo srcFileInfo(src);
		QFileInfo dstFileInfo(QDir(desktopPath), srcFileInfo.fileName());
		if (srcFileInfo.isFile()) {
			// TODO 暂时先不处理这个，先处理拖拽的逻辑
			// 考虑到格子的实现，这些可能要放到公用里
			//CopySingleFile(src, dstFileInfo.absoluteFilePath(), move);
		}
		else {
			//CopyDir(src, dstFileInfo.absoluteFilePath(), move);
		}
	}
	//if (inNetwork()) onRefresh();    //由于性能的原因，网络文件的增删不会被Model感知，需要手动刷新
}

void HzDesktopIconViewPrivate::handleDelete()
{
	HZQ_Q(HzDesktopIconView);

	QModelIndexList indexList = q->selectedIndexes();

	// TODO 目前无法删除文件夹
	for (const QModelIndex& index : indexList) {
		FILEOP_FLAGS dwOpFlags = FOF_ALLOWUNDO | FOF_NO_UI;

		SHFILEOPSTRUCTA fileOp = { 0 };
		fileOp.hwnd = NULL;
		fileOp.wFunc = FO_DELETE; ///> 文件删除操作
		fileOp.pFrom = StrDupA(q->m_itemModel->filePath(index).toStdString().c_str());
		fileOp.pTo = NULL;
		fileOp.fFlags = dwOpFlags;
		fileOp.hNameMappings = NULL;
		fileOp.lpszProgressTitle = "hz delete file";

		SHFileOperationA(&fileOp);
	}
}

void HzDesktopIconViewPrivate::handleRename()
{
	HZQ_Q(HzDesktopIconView);

	QModelIndexList indexes = q->selectedIndexes();
	if (indexes.empty()) {
		return;
	}

	q->edit(indexes[0]);
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