#pragma once

#include <string>
#include <QWidget>
#include <QMimeData>
#include <QIcon>

namespace HZ
{
	//struct IconLocation {
	//	QString iconPath;
	//	int iconIndex;
	//};

	bool showContentMenuWin10(
		WId ownerWId,
		const QStringList& pathList,
		int x,
		int y
	);

	void removePixmapCache(const QString& filePath);

	// 部分文件的图标最大只包含了48x48的，但是Qt读取时会自动合成一个256x256的图片，并且将48x48绘制在256大小图片的左上角
	// 此时缩放后导致显示在左上角一个很小的图片，这里规避处理进行矫正
	void correctPixmapIfIsInvalid(QIcon& icon);

	QString getDirectString(const QString& resStr);

	// 暂时废弃
	QIcon getIconFromLocation(const QString& location);

	QString getTextFromGUID(const QString& guidStr);

	// 暂时废弃
	QIcon getIconFromGUID(const QString& guid);

	QIcon getIconFromPath(const QString& filePath, bool horizontally = false, bool vertically = false);

	QPixmap getPixmapFromPath(const QString& filePath, bool horizontally = false, bool vertically = false);
}