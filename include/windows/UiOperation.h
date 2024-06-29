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

	// �����ļ���ͼ�����ֻ������48x48�ģ�����ϵͳ��ȡʱ���Զ��ϳ�һ��256x256��ͼƬ�����ҽ�48x48������256��СͼƬ�����Ͻ�
	// ��ʱ���ź�����ʾ�����Ͻ�һ����С��ͼƬ���������ж��Ƿ��д�����
	bool isInvalidPixmap(const QPixmap& pixmap);

	QString getDirectString(const QString& resStr);

	// ��ʱ����
	QIcon getIconFromLocation(const QString& location);

	QString getTextFromGUID(const QString& guidStr);

	// ��ʱ����
	QIcon getIconFromGUID(const QString& guid);

	QPixmap getThumbnailFromPath(const QString& filePath);

	QPixmap getPixmapFromPath(const QString& filePath);
}