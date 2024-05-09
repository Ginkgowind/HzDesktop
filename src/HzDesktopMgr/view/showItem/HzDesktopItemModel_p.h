#pragma once

#include <QObject>
#include <QThread>
#include <QFileIconProvider>
#include <QFileInfo>

#include <windows.h>
#include <vector>
#include <string>

#include "HzDesktopItemModel.h"
#include "windows/EnsureCleanup.h"

/**************************���ϵͳͼ��************************/
struct SystemAppInfo
{
	QString clsidValue;
	QString showName;
	QIcon showIcon;
};
typedef QList<SystemAppInfo> SystemAppInfoList;

class DesktopSystemItemWatcher : public QThread
{
	Q_OBJECT

public:
	DesktopSystemItemWatcher();
	~DesktopSystemItemWatcher();

	void init();

signals:
	// ��Ϊ�Ǵ��ݵ����߳�ȥ�������Դ˴���������
	void systemAppRefreshed(SystemAppInfoList systemAppList);

private:
	bool initWatcher();

	void refreshSystemAppsInfo();

	QString getSystemAppDisplayName(const QString& csidlPath);

	bool setSystemAppDisplayName(const QString& csidlPath, const QString& name);

	QIcon getSystemAppIcon(const QString& csidlPath);

protected:
	void run() override;

private:
	CEnsureRegCloseKey m_monitorKey;
	CEnsureCloseHandle m_monitorEvent;
	SystemAppInfoList m_systemAppList;
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

signals:
	// ��Ϊ�Ǵ��ݵ����߳�ȥ�������Դ˴���������
	void fileItemRefreshed(QFileInfoList fileItemList);

protected:
	void run() override;

private:
	bool initWatcherDir();

	bool initWatcher(DWORD dwNotifyFilter);

	void uninitWatcher();

	void refreshFileItem();

	void handleObserveResult(
		const std::wstring& strWatchDirectory,
		const FILE_NOTIFY_INFORMATION* pNotification
	);

	void handleFileCreated(const std::wstring& filePath);

	void handleFileDeleted(const std::wstring& filePath);

	void handleFileModified(const std::wstring& filePath);

	void handleFileRenamed(const std::wstring& oldPath, const std::wstring& newPath);


private:
	// ����Ŀ¼�����ά�������ݽṹ�����߳��е��ã����ô���ͬ������
	std::vector<DirObserver> m_observerInfos;

	CEnsureRegCloseKey m_monitorKey;
	CEnsureCloseHandle m_monitorEvent;
	QFileInfoList m_fileItemList;
};

//QObjectPrivateδ�������ʴ˴�����
class HzDesktopItemModelPrivate 
	: public HzDesktopItemDataMgr
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

	const SystemAppInfoList& getSystemAppInfoList() const;

	const QFileInfoList& getFileItemInfoList() const;

	const QModelIndex translateRealIndex(const QModelIndex& index) const;

private:
	DesktopSystemItemWatcher m_systemItemWatcher;
	
	DesktopFileItemWatcher m_fileItemWatcher;

	SystemAppInfoList m_systemAppList;
	
	QFileInfoList m_fileItemList;

	friend class HzDesktopItemModel;
};
