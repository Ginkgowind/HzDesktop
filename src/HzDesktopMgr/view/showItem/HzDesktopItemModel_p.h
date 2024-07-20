#pragma once

#include <QObject>
#include <QThread>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QStandardItem>

#include <windows.h>
#include <vector>
#include <string>
#include <functional>

#include "HzDesktopItemModel.h"
#include "windows/EnsureCleanup.h"
#include "common/QtpqReimp.h"

class ItemHelper {
public:
	static void setDisplayName(const QString& filePath, const QString& name);
};

/**************************监控系统图标************************/

class DesktopSystemItemWatcher : public QThread
{
	Q_OBJECT

public:
	DesktopSystemItemWatcher();
	~DesktopSystemItemWatcher();

	void init();

	void refreshSystemAppsInfo();

	bool initWatcher();

	QStandardItem* genQStandardItem(const QString& clsidPath);

signals:
	// 因为是传递到主线程去处理，所以此处不传引用
	void systemAppRefreshed(QList<QStandardItem*> systemAppList);

protected:
	void run() override;

private:
	CEnsureRegCloseKey m_monitorKey;
	CEnsureCloseHandle m_monitorEvent;
};

/************************监控桌面普通图标**********************/

struct DirObserver
{
	std::wstring watchPath;
	CEnsureCloseFile hDirectory;
	CEnsureCloseHandle hEvent;
	OVERLAPPED overlapped;
	char* notifyDataBuf;
};

class DesktopFileItemWatcher : public QThread
{
	Q_OBJECT

public:
	DesktopFileItemWatcher();
	~DesktopFileItemWatcher();

	void init();

	void refreshFileItem();

signals:
	// 因为是传递到主线程去处理，所以此处不传引用
	void fileItemRefreshed(QList<QStandardItem*> fileItemList);

	void onFileCreated(QStandardItem* item);

	void onFileDeleted(const QString& filePath);

	void onFileModified(const QString& filePath, QStandardItem* item);

	void onFileRenamed(
		const QString& oldPath, 
		const QString& newPath,
		const QString& newFileName
	);

protected:
	void run() override;

private:
	bool initWatcherDir();

	bool initWatcher(DWORD dwNotifyFilter);

	void uninitWatcher();

	QStandardItem* genQStandardItem(const QFileInfo& fileInfo);

	void handleObserveResult(
		const QString& strWatchDirectory,
		const FILE_NOTIFY_INFORMATION* pNotification
	);

	QString formatFileSize(qint64 size);

private:
	// 各个目录监控所维护的数据结构，单线程中调用，不用处理同步问题
	std::vector<DirObserver> m_observerInfos;

	CEnsureRegCloseKey m_monitorKey;
	CEnsureCloseHandle m_monitorEvent;
};

class HzDesktopItemModelPrivate 
	: public HzDesktopPrivate
{
	Q_OBJECT

	HZQ_DECLARE_PUBLIC(HzDesktopItemModel)

public:
	enum { NumColumns = 4 };
	enum ItemType {
		SystemAppItem,
		NormalFileItem
	};

	HzDesktopItemModelPrivate();
	~HzDesktopItemModelPrivate();

	void init();

private:
	void handleFileCreated(QStandardItem* item);

	void handleFileDeleted(const QString& filePath);

	void handleFileModified(const QString& filePath, QStandardItem* item);

	void handleFileRenamed(
		const QString& oldPath, 
		const QString& newPath,
		const QString& newFileName
	);

private:
	int m_initFinishedCnt;

	DesktopSystemItemWatcher m_systemItemWatcher;
	
	DesktopFileItemWatcher m_fileItemWatcher;
};
