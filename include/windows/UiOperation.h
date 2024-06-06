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

	void testComMenu(HWND hwnd);

	QMimeData* multiDrag(const QStringList& pathList);

	// 部分文件的图标最大只包含了48x48的，但是Qt读取时会自动合成一个256x256的图片，并且将48x48绘制在256大小图片的左上角
	// 此时缩放后导致显示在左上角一个很小的图片，这里规避处理进行矫正
	void correctPixmapIfIsInvalid(QIcon& icon);

	QString getDirectString(const QString& resStr);

	QIcon getIconFromLocation(const QString& location);

	QString getTextFromGUID(const QString& guidStr);

	QIcon getIconFromGUID(const QString& guid);
}