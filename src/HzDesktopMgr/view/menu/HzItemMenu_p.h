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

// 仅适用于桌面
class HzItemMenuPrivate : public HzDesktopPrivate
{
	Q_OBJECT

	HZQ_DECLARE_PUBLIC(HzItemMenu)

public:
	// 这里因为桌面上的文件同时含有公共桌面和用户桌面两个文件夹的内容，
	// 经测试只有通过顶层桌面接口枚举出来的id才有效，别的各种方法都会有奇奇怪怪的问题
	// tips : 外部要处理返回值的资源释放
	// TODO后续做格子的时候再优化代码结构，只有桌面的才枚举，别的不用这种方式
	std::vector<PITEMID_CHILD> getDesktopPidcFromPaths(const QStringList& paths);

	void executeActionFromContextMenu(const QStringList& pathList, const std::string& action);
};

// 这个类是通用的，桌面和普通文件夹都可用
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
