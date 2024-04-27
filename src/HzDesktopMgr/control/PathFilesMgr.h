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
	// 线程退出事件
	HANDLE m_exitEvent;

	// 初始化完成事件，WebSandboxAppPolicyGenerator中需要等待初始信息获取完成
	HANDLE m_initOverEvent;

	// 各个目录监控所维护的数据结构，单线程中调用，不用处理同步问题
	std::vector<DirObserver> m_observerInfos;

	// 监控的路径
	std::vector<std::wstring> m_observeDirPaths;
};

