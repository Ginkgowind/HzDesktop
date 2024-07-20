#pragma once

#include <wil/resource.h>
#include <shobjidl_core.h>

#include <QObject>
//#include <string>

#include "HzItemMenu.h"
#include "common/QtpqReimp.h"

// ������������
class HzItemMenuPrivate : public HzDesktopPrivate
{
	Q_OBJECT

	HZQ_DECLARE_PUBLIC(HzItemMenu)

public:
	// ������Ϊ�����ϵ��ļ�ͬʱ���й���������û����������ļ��е����ݣ�
	// ������ֻ��ͨ����������ӿ�ö�ٳ�����id����Ч����ĸ��ַ�������������ֵֹ�����
	// tips : �ⲿҪ������ֵ����Դ�ͷ�
	// TODO���������ӵ�ʱ�����Ż�����ṹ��ֻ������Ĳ�ö�٣���Ĳ������ַ�ʽ
	std::vector<PITEMID_CHILD> getDesktopPidcFromPaths(const QStringList& paths);

	void executeActionFromContextMenu(const QStringList& pathList, const std::string& action);
};

// �������ͨ�õģ��������ͨ�ļ��ж�����
class HzDesktopBkgMenuPrivate : public HzDesktopPrivate
{
	Q_OBJECT

	HZQ_DECLARE_PUBLIC(HzDesktopBkgMenu)

public:
	HzDesktopBkgMenuPrivate();
	~HzDesktopBkgMenuPrivate();

	void updateMenu(HMENU menu);

	wil::unique_hmenu buildViewMenu();

	wil::unique_hmenu buildSortMenu();

	QVector<UINT> getNewFileCmdsVec(HMENU hMenu);

	void executeActionFromContextMenu(const QString& path, const std::string& action);
};
