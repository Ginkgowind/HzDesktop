#include <QApplication>
#include <QClipboard>
#include <QStandardPaths>
#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>

#include "HzDesktopIconView.h"
#include "HzDesktopIconView_p.h"

HzDesktopIconViewPrivate::HzDesktopIconViewPrivate()
{
}

HzDesktopIconViewPrivate::~HzDesktopIconViewPrivate()
{
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
}
