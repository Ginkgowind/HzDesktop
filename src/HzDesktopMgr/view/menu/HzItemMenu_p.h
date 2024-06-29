#pragma once

#include <wil/resource.h>
#include <shobjidl_core.h>

#include <QObject>
//#include <string>

#include "HzItemMenu.h"
#include "common/QtpqReimp.h"


class MenuHelper
{
public:

	static inline void appendMenuItem(HMENU menu, UINT id);

	static inline void insertMenuItem(HMENU menu, UINT id, UINT item);

	static inline void appendSeparator(HMENU menu);

	static inline void insertSeparator(HMENU menu, UINT item);

	static inline void addSubMenuItem(HMENU menu, UINT id, wil::unique_hmenu subMenu);
	
	static inline void insertSubMenuItem(HMENU menu, UINT id, wil::unique_hmenu subMenu, UINT item);

	static inline void CheckItem(HMENU hMenu, UINT itemID, bool bCheck);
};

class HzItemMenuPrivate : public HzDesktopPrivate
{
	Q_OBJECT

		HZQ_DECLARE_PUBLIC(HzItemMenu)

public:
	// 这里因为桌面上的文件同时含有公共桌面和用户桌面两个文件夹的内容，
	// 经测试只有通过顶层桌面接口枚举出来的id才有效，别的各种方法都会有奇奇怪怪的问题
	// tips : 外部要处理返回值的资源释放
	// TODO后续做格子的时候再优化代码结构，只有桌面的才枚举，别的不用这种方式
	std::vector<PITEMID_CHILD> getPidcFromPaths(const QStringList paths);

	void showItemsMenuWin10(
		WId ownerWId,
		const QStringList& pathList,
		int showX,
		int showY
	);

	void executeActionFromContextMenu(const QStringList& pathList, const std::string& action);
};

class HzDesktopBkgMenuPrivate : public HzDesktopPrivate
{
	Q_OBJECT

	HZQ_DECLARE_PUBLIC(HzDesktopBkgMenu)

public:
	HzDesktopBkgMenuPrivate();
	~HzDesktopBkgMenuPrivate();

	void updateMenu(HMENU menu);


	LRESULT ParentWindowSubclass(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	wil::unique_hmenu buildViewMenu();

	wil::unique_hmenu buildSortMenu();

	QVector<UINT> getNewFileCmdsVec(HMENU hMenu);

	void executeActionFromContextMenu(const QString& path, const std::string& action);
};
