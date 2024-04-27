#include <assert.h>

#include "PathFilesMgr.h"
#include "common/CommonTools.h"
#include "windows/FileInfoGather.h"

// 监控文件夹变化，收到通知数据的缓存大小
#define OBSERVE_DIR_BUFFER_SIZE (sizeof(FILE_NOTIFY_INFORMATION) + ((sizeof(WCHAR) * MAX_PATH) * 2 * 4))

PathFilesMgr::PathFilesMgr(std::vector<std::wstring>& observeDirPaths)
	: m_observeDirPaths(std::move(observeDirPaths))
{

}

PathFilesMgr::~PathFilesMgr()
{
}

void PathFilesMgr::startWork()
{
	RefreshFilesInfo();

	// 启动线程
}

void PathFilesMgr::stopWork()
{
}

void PathFilesMgr::workThreadFunc()
{
	// 每个目录关联一个事件以及句柄
	int nDirCount = m_observeDirPaths.size();
	FILE_NOTIFY_INFORMATION* pNotification = NULL;
	BOOL watchState = FALSE;
	DWORD dwNotifyFilter =
		FILE_NOTIFY_CHANGE_FILE_NAME |
		FILE_NOTIFY_CHANGE_DIR_NAME |
		FILE_NOTIFY_CHANGE_ATTRIBUTES;
	HANDLE* hEventArray = NULL;

	// 初始化
	if (!initObserver(dwNotifyFilter))
	{
		//LOG_ERROR_W("_InitObserver failed");
		goto _end; // 初始化失败，去做资源清理
	}

	// 初始化事件数组
	hEventArray = (HANDLE*)malloc((nDirCount + 1) * sizeof(HANDLE));
	if (hEventArray == NULL)
	{
		//LOG_ERROR_W("malloc event array failed");
		goto _end;
	}
	::ZeroMemory(hEventArray, (nDirCount + 1) * sizeof(HANDLE));

	hEventArray[0] = m_exitEvent;

	int monitorSucCount = 0;
	for (int i = 0; i < nDirCount; i++)
	{
		if (m_observerInfos[i].hEvent != NULL)
		{
			monitorSucCount++;
			hEventArray[monitorSucCount] = m_observerInfos[i].hEvent;
		}
	}

	while (TRUE)
	{
		DWORD dwWait = ::WaitForMultipleObjects(monitorSucCount + 1, hEventArray, FALSE, INFINITE);
		if (dwWait == WAIT_OBJECT_0)
		{
			//LOG_INFO("Receive exit event");
			break;
		}
		else if (WAIT_OBJECT_0 < dwWait && dwWait <= WAIT_OBJECT_0 + monitorSucCount)
		{
			int eventIndex = dwWait - WAIT_OBJECT_0;
			int watchInfoIndex = -1;
			for (size_t i = 0; i < m_observerInfos.size(); ++i)
			{
				if (m_observerInfos[i].hEvent == hEventArray[eventIndex])
				{
					watchInfoIndex = i;
					break;
				}
			}

			if (watchInfoIndex == -1)
			{
				//LOG_ERROR("something wrong, maybe m_observerInfos size() changed, please check code, eventIndex:%d, errno:(0x%x)", eventIndex, ::GetLastError());
				break;
			}

			DWORD bytesReturned = 0;
			BOOL bRet = ::GetOverlappedResult(
				m_observerInfos[watchInfoIndex].hDirectory,
				&m_observerInfos[watchInfoIndex].overlapped,
				&bytesReturned,
				TRUE);
			if (!bRet)
			{
				//LOG_ERROR_W("GetOverlappedResult failed, error=%d", ::GetLastError());
				continue;   // 继续监听
			}

			pNotification = (FILE_NOTIFY_INFORMATION*)m_observerInfos[watchInfoIndex].notifyDataBuf;
			handleObserveResult(m_observeDirPaths[watchInfoIndex], pNotification);
			::ZeroMemory(m_observerInfos[watchInfoIndex].notifyDataBuf, OBSERVE_DIR_BUFFER_SIZE);

			// 使用异步的ReadDirectoryChangesW
			watchState = ::ReadDirectoryChangesW(
				m_observerInfos[watchInfoIndex].hDirectory,
				m_observerInfos[watchInfoIndex].notifyDataBuf,
				OBSERVE_DIR_BUFFER_SIZE,
				TRUE,
				dwNotifyFilter,
				NULL,
				&m_observerInfos[watchInfoIndex].overlapped,
				NULL);

			if (watchState == FALSE)
			{
				//LOG_ERROR_W("ReadDirectoryChangesW failed.Dir path:%ws err:%d",
				//	m_observeDirPaths[watchInfoIndex].c_str(), ::GetLastError());
			}
		}
		else
		{
			//LOG_ERROR_A("WaitForMultipleObjects, dwWait=%d, error=%d", dwWait, ::GetLastError());
			break;
		}
	}

_end:
	uninitObserver();
	if (hEventArray != NULL)
	{
		free(hEventArray);
		hEventArray = NULL;
	}
}

bool PathFilesMgr::initObserver(DWORD dwNotifyFilter)
{
	return false;
}

void PathFilesMgr::uninitObserver()
{
	for (int i = 0; i < m_observerInfos.size(); i++)
	{
		if (m_observerInfos[i].notifyDataBuf)
		{
			free(m_observerInfos[i].notifyDataBuf);
		}

		if (m_observerInfos[i].hEvent)
		{
			::CloseHandle(m_observerInfos[i].hEvent);
		}
	}

	m_observerInfos.clear();
}

void PathFilesMgr::RefreshFilesInfo()
{
	std::vector<HZ::FileInfo> fileInfoList;

	for (const std::wstring& strScanDirectory : m_observeDirPaths)
	{
		if (!HZ::FileInfoGather::GatherFilesInfo(strScanDirectory, fileInfoList, L"*", true))
		{
			//LOG_WARN_W("scan lnk file failed, scanDir:%ws", strScanDirectory.c_str());
			continue;
		}
	}

	return;
	// 通知显示列表
}

void PathFilesMgr::handleObserveResult(
	const std::wstring& strWatchDirectory,
	const FILE_NOTIFY_INFORMATION* pNotification
)
{
	assert(pNotification);

	std::wstring strFileName(
		pNotification->FileName,
		pNotification->FileNameLength / sizeof(wchar_t));

	std::wstring strFileAbsPath = HZ::AppendPathConstW(strWatchDirectory, strFileName);
	DWORD cbOffset = 0;
	std::wstring strRenameOldFilePath;

	do
	{
		switch (pNotification->Action)
		{
		case FILE_ACTION_ADDED:
		{
			//LOG_DEBUG_W("FILE_ACTION_ADDED: %ws", strFileAbsPath.c_str());
			handleFileCreated(strFileAbsPath);
			break;
		}
		case FILE_ACTION_REMOVED:
		{
			//LOG_DEBUG_W("FILE_ACTION_REMOVED: %ws", strFileAbsPath.c_str());
			handleFileDeleted(strFileAbsPath);
			break;
		}
		case FILE_ACTION_MODIFIED:
		{
			//LOG_DEBUG_W("FILE_ACTION_MODIFIED: %ws", strFileAbsPath.c_str());
			handleFileModified(strFileAbsPath);
			break;
		}
		case FILE_ACTION_RENAMED_OLD_NAME:
		{
			//LOG_DEBUG_W("FILE_ACTION_RENAMED_OLD_NAME: %ws", strFileAbsPath.c_str());
			strRenameOldFilePath = strFileAbsPath;
			break;
		}
		case FILE_ACTION_RENAMED_NEW_NAME:
		{
			//LOG_DEBUG_W("FILE_ACTION_RENAMED_NEW_NAME: %ws", strFileAbsPath.c_str());
			handleFileRenamed(strRenameOldFilePath, strFileAbsPath);
			break;
		}
		default:
			break;
		}

		cbOffset = pNotification->NextEntryOffset;
		pNotification = (PFILE_NOTIFY_INFORMATION)((LPBYTE)pNotification + cbOffset);

		if (cbOffset)
		{
			// 获取新的路径信息
			strFileName = std::wstring(
				pNotification->FileName,
				pNotification->FileNameLength / sizeof(wchar_t));
			strFileAbsPath = HZ::AppendPathConstW(strWatchDirectory, strFileName);
		}

	} while (cbOffset);
}

void PathFilesMgr::handleFileCreated(const std::wstring& filePath)
{
	//auto it = std::find_if(mVecAppLinkInfo.begin(), mVecAppLinkInfo.end(), _IsSpecifiedPath(filePath));
	//if (it != mVecAppLinkInfo.end())    // 找到了说明出错
	//{
	//	//LOG_ERROR_W("_OnLnkFileCreated failed, lnk path already exist : %ws", filePath.c_str());
	//	return;
	//}
}

void PathFilesMgr::handleFileDeleted(const std::wstring& filePath)
{
}

void PathFilesMgr::handleFileModified(const std::wstring& filePath)
{
}

void PathFilesMgr::handleFileRenamed(const std::wstring& oldPath, const std::wstring& newPath)
{
}
