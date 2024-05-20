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

	QStandardItem* genQStandardItem(const QString& clsidValue);

	QString getSystemAppDisplayName(const QString& csidlPath);

	bool setSystemAppDisplayName(const QString& csidlPath, const QString& name);

	QIcon getSystemAppIcon(const QString& csidlPath);

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

protected:
	void run() override;

private:
	bool initWatcherDir();

	bool initWatcher(DWORD dwNotifyFilter);

	void uninitWatcher();

	QStandardItem* genQStandardItem(const QFileInfo& fileInfo);

	QIcon getUltimateIcon(const QFileInfo& fileInfo);

	void handleObserveResult(
		const std::wstring& strWatchDirectory,
		const FILE_NOTIFY_INFORMATION* pNotification
	);

	void handleFileCreated(const std::wstring& filePath);

	void handleFileDeleted(const std::wstring& filePath);

	void handleFileModified(const std::wstring& filePath);

	void handleFileRenamed(const std::wstring& oldPath, const std::wstring& newPath);

	static std::function<bool(const QFileInfo& itInfo)> compMemberPath(const std::wstring& targetPath);

private:
	// ����Ŀ¼�����ά�������ݽṹ�����߳��е��ã����ô���ͬ������
	std::vector<DirObserver> m_observerInfos;

	CEnsureRegCloseKey m_monitorKey;
	CEnsureCloseHandle m_monitorEvent;
};

//QObjectPrivateδ�������ʴ˴�����
class HzDesktopItemModelPrivate 
	: public HzDesktopPrivate
{
	Q_OBJECT

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
	DesktopSystemItemWatcher m_systemItemWatcher;
	
	DesktopFileItemWatcher m_fileItemWatcher;

	friend class HzDesktopItemModel;
};
