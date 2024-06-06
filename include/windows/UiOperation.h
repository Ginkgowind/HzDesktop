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

	// �����ļ���ͼ�����ֻ������48x48�ģ�����Qt��ȡʱ���Զ��ϳ�һ��256x256��ͼƬ�����ҽ�48x48������256��СͼƬ�����Ͻ�
	// ��ʱ���ź�����ʾ�����Ͻ�һ����С��ͼƬ�������ܴ�����н���
	void correctPixmapIfIsInvalid(QIcon& icon);

	QString getDirectString(const QString& resStr);

	QIcon getIconFromLocation(const QString& location);

	QString getTextFromGUID(const QString& guidStr);

	QIcon getIconFromGUID(const QString& guid);
}