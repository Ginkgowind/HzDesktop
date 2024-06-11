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

/**************************���ϵͳͼ��************************/

class DesktopSystemItemWatcher : public QThread
{
	Q_OBJECT

public:
	DesktopSystemItemWatcher();
	~DesktopSystemItemWatcher();

	void init();

	void refreshSystemAppsInfo();

signals:
	// ��Ϊ�Ǵ��ݵ����߳�ȥ�������Դ˴���������
	void systemAppRefreshed(QList<QStandardItem*> systemAppList);

private:
	bool initWatcher();

	QStandardItem* genQStandardItem(const QString& clsidPath);

	QString getSystemAppDisplayName(const QString& clsidPath);

	bool setSystemAppDisplayName(const QString& clsidPath, const QString& name);

	QIcon getSystemAppIcon(const QString& clsidPath);

protected:
	void run() override;

private:
	CEnsureRegCloseKey m_monitorKey;
	CEnsureCloseHandle m_monitorEvent;
};

/************************���������ͨͼ��**********************/

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
	// ��Ϊ�Ǵ��ݵ����߳�ȥ�������Դ˴���������
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

	QIcon getUltimateIcon(const QFileInfo& fileInfo);

	QString getFileShowText(const QFileInfo& fileInfo);

	void handleObserveResult(
		const QString& strWatchDirectory,
		const FILE_NOTIFY_INFORMATION* pNotification
	);

private:
	// ����Ŀ¼�����ά�������ݽṹ�����߳��е��ã����ô���ͬ������
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
	DesktopSystemItemWatcher m_systemItemWatcher;
	
	DesktopFileItemWatcher m_fileItemWatcher;
};
