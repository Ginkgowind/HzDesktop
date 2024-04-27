#pragma once

#include <windows.h>
#include <vector>
#include <string>

#include "windows/EnsureCleanup.h"

struct DirObserver
{
	CEnsureCloseFile hDirectory;
	HANDLE hEvent;
	OVERLAPPED overlapped;
	char* notifyDataBuf;
};

class PathFilesMgr
{
public:
	PathFilesMgr(std::vector<std::wstring>& vecObserveDirs);

	~PathFilesMgr();

	void startWork();

	void stopWork();

private:
	void workThreadFunc();

	bool initObserver(DWORD dwNotifyFilter);

	void uninitObserver();

private:
	void RefreshFilesInfo();

	void handleObserveResult(
		const std::wstring& strWatchDirectory,
		const FILE_NOTIFY_INFORMATION* pNotification
	);

	void handleFileCreated(const std::wstring& filePath);

	void handleFileDeleted(const std::wstring& filePath);

	void handleFileModified(const std::wstring& filePath);

	void handleFileRenamed(const std::wstring& oldPath, const std::wstring& newPath);

private:
	// �߳��˳��¼�
	HANDLE m_exitEvent;

	// ��ʼ������¼���WebSandboxAppPolicyGenerator����Ҫ�ȴ���ʼ��Ϣ��ȡ���
	HANDLE m_initOverEvent;

	// ����Ŀ¼�����ά�������ݽṹ�����߳��е��ã����ô���ͬ������
	std::vector<DirObserver> m_observerInfos;

	// ��ص�·��
	std::vector<std::wstring> m_observeDirPaths;
};

