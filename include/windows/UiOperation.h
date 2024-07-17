#pragma once

#include <string>
#include <QWidget>
#include <QMimeData>
#include <QIcon>

namespace HZ
{
	QIcon getUltimateIcon(const QString& filePath);

	// �����ļ���ͼ����󲻰���256*256�ģ�����ϵͳ��ȡʱ���Զ��ϳ�һ��256x256��ͼƬ�����ҽ���С�Ļ�����256��СͼƬ�����Ͻ�
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