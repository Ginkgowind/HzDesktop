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

	void removePixmapCache(const QString& filePath);

	// 部分文件的图标最大只包含了48x48的，但是系统读取时会自动合成一个256x256的图片，并且将48x48绘制在256大小图片的左上角
	// 此时缩放后导致显示在左上角一个很小的图片，这里规避判断是否有此问题
	bool isInvalidPixmap(const QPixmap& pixmap);

	QString getDirectString(const QString& resStr);

	// 暂时废弃
	QIcon getIconFromLocation(const QString& location);

	QString getTextFromGUID(const QString& guidStr);

	// 暂时废弃
	QIcon getIconFromGUID(const QString& guid);

	QPixmap getThumbnailFromPath(const QString& filePath);

	QPixmap getPixmapFromPath(const QString& filePath);
}