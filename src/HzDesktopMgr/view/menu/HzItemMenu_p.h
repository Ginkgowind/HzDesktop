#pragma once

#include <wil/resource.h>
#include <shobjidl_core.h>

#include <QObject>
//#include <string>

#include "HzItemMenu.h"
#include "windows/IconBitmapUtils.h"
#include "common/QtpqReimp.h"
//#include "common/MenuHelper.h"

class MenuHelper
{
public:
	MenuHelper(HINSTANCE resInstance);

	void appendMenuItem(HMENU menu, UINT id, UINT uIcon = 0);

	void insertMenuItem(HMENU menu, UINT item, UINT id, UINT uIcon = 0);

	void appendSeparator(HMENU menu);

	void insertSeparator(HMENU menu, UINT item);

	void addSubMenuItem(HMENU menu, HMENU subMenu, UINT id, UINT uIcon = 0);

	void insertSubMenuItem(HMENU menu, UINT id, UINT item, HMENU subMenu, UINT uIcon = 0);

	void CheckItem(HMENU hMenu, UINT itemID, bool bCheck);

	void EnableItem(HMENU hMenu, UINT itemID, bool bEnable);

private:

	HINSTANCE m_resInstance;

	IconBitmapUtils m_bitmapUtils;
};

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

	void updateMenu(HMENU hMenu);

	HMENU buildViewMenu();

	HMENU buildSortMenu();

	HMENU buildFunctionMenu();

	QVector<UINT> getNewFileCmdsVec(HMENU hMenu);

	void executeActionFromContextMenu(const QString& path, const std::string& action);

	MenuHelper m_menuHelper;
};
