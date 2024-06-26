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

	// �����ļ���ͼ�����ֻ������48x48�ģ�����Qt��ȡʱ���Զ��ϳ�һ��256x256��ͼƬ�����ҽ�48x48������256��СͼƬ�����Ͻ�
	// ��ʱ���ź�����ʾ�����Ͻ�һ����С��ͼƬ�������ܴ�����н���
	void correctPixmapIfIsInvalid(QIcon& icon);

	QString getDirectString(const QString& resStr);

	// ��ʱ����
	QIcon getIconFromLocation(const QString& location);

	QString getTextFromGUID(const QString& guidStr);

	// ��ʱ����
	QIcon getIconFromGUID(const QString& guid);

	QIcon getIconFromPath(const QString& filePath, bool horizontally = false, bool vertically = false);

	QPixmap getPixmapFromPath(const QString& filePath, bool horizontally = false, bool vertically = false);
}