#include <QSettings>
#include <QDir>
#include <shlwapi.h>
#include <Shlobj.h>
#include <QtWinExtras/QtWin>

#include "HzDesktopItemModel_p.h"
#include "windows/tools.h"
#include "common/CommonTools.h"

// 控制系统图标的显示与隐藏，仿照腾讯桌面整理，此处从简，只监听显示与隐藏
#define SYSTEM_ICON_REG_SUBPATH \
	"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\HideDesktopIcons\\NewStartPanel"

#define OBSERVE_DIR_BUFFER_SIZE (sizeof(FILE_NOTIFY_INFORMATION) + ((sizeof(WCHAR) * MAX_PATH) * 2 * 4))

DesktopSystemItemWatcher::DesktopSystemItemWatcher()
{
}

DesktopSystemItemWatcher::~DesktopSystemItemWatcher()
{
}

void DesktopSystemItemWatcher::init()
{
	initWatcher();

	refreshSystemAppsInfo();
}

bool DesktopSystemItemWatcher::initWatcher()
{
	LSTATUS bRet = false;

	do
	{
		LSTATUS lRet = RegOpenKeyExA(
			HKEY_CURRENT_USER,
			SYSTEM_ICON_REG_SUBPATH,
			0,
			KEY_READ,
			&m_monitorKey
		);
		if (lRet != ERROR_SUCCESS) {
			break;
		}

		m_monitorEvent = ::CreateEventA(NULL, FALSE, FALSE, NULL);
		if (m_monitorEvent == NULL) {
			break;
		}

		bRet = true;
	} while (false);

	return bRet;
}

void DesktopSystemItemWatcher::refreshSystemAppsInfo()
{
	static const QString showStatusRegPath =
		"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\HideDesktopIcons\\NewStartPanel";

	m_systemAppList.clear();
	QSettings settings(showStatusRegPath, QSettings::Registry64Format);
	QStringList clsidValueList = settings.childKeys() + QStringList("{645FF040-5081-101B-9F08-00AA002F954E}");
	for (const QString& clsidValue : clsidValueList) {
		bool bHidden = settings.value(clsidValue).toBool();
		if (!bHidden) {
			m_systemAppList.push_back({
				clsidValue,
				getSystemAppDisplayName("::" + clsidValue),
				getSystemAppIcon(clsidValue)
				});
		}
	}

	emit systemAppRefreshed(m_systemAppList);
}

QString DesktopSystemItemWatcher::getSystemAppDisplayName(const QString& clsidPath)
{
	TCHAR szDisplayName[MAX_PATH] = { 0 };

	IShellFolder* desktopFolder = nullptr;
	LPITEMIDLIST systemAppPidl = nullptr;

	do
	{
		HRESULT hRet = SHGetDesktopFolder(&desktopFolder);
		if (FAILED(hRet)) {
			break;
		}

		hRet = desktopFolder->ParseDisplayName(
			NULL, NULL,
			(LPWSTR)clsidPath.toStdWString().c_str(),
			NULL, &systemAppPidl, NULL
		);
		if (FAILED(hRet)) {
			break;
		}

		STRRET strDisplayName;
		hRet = desktopFolder->GetDisplayNameOf(systemAppPidl, SHGDN_NORMAL, &strDisplayName);
		if (FAILED(hRet)) {
			break;
		}

		hRet = StrRetToBuf(&strDisplayName, systemAppPidl, szDisplayName, MAX_PATH);
		if (FAILED(hRet)) {
			break;
		}
	} while (false);

	if (systemAppPidl) {
		CoTaskMemFree(systemAppPidl);
	}

	if (desktopFolder) {
		desktopFolder->Release();
	}

	return szDisplayName;
}

bool DesktopSystemItemWatcher::setSystemAppDisplayName(const QString& clsidPath, const QString& name)
{
	bool bRet = false;

	IShellFolder* desktopFolder = nullptr;
	LPITEMIDLIST systemAppPidl = nullptr;

	do
	{
		HRESULT hRet = SHGetDesktopFolder(&desktopFolder);
		if (FAILED(hRet)) {
			break;
		}

		hRet = desktopFolder->ParseDisplayName(
			NULL, NULL,
			(LPWSTR)clsidPath.toStdWString().c_str(),
			NULL, &systemAppPidl, NULL
		);
		if (FAILED(hRet)) {
			break;
		}

		hRet = desktopFolder->SetNameOf(NULL, systemAppPidl,
			name.toStdWString().c_str(), 0, NULL);
		if (FAILED(hRet)) {
			break;
		}

		bRet = true;
	} while (false);

	if (systemAppPidl) {
		CoTaskMemFree(systemAppPidl);
	}

	if (desktopFolder) {
		desktopFolder->Release();
	}

	return bRet;
}

QIcon DesktopSystemItemWatcher::getSystemAppIcon(const QString& clsidValue)
{
	QIcon retIcon;

	HICON hIcon = NULL;
	HKEY hKey;

	do
	{
		// 打开注册表键
		std::string strSubKey = "CLSID\\" + clsidValue.toStdString() + "\\DefaultIcon";
		LSTATUS lRet = RegOpenKeyExA(HKEY_CLASSES_ROOT,
			strSubKey.c_str(),
			0, KEY_READ, &hKey);
		if (lRet != ERROR_SUCCESS) {
			break;
		}

		// 查询图标路径
		char value[MAX_PATH];
		DWORD valueLength = sizeof(value);
		lRet = RegQueryValueExA(
			hKey, NULL,	// 默认值
			NULL, NULL,
			(LPBYTE)value,
			&valueLength);
		if (lRet != ERROR_SUCCESS) {
			break;
		}

		// 分离图标路径和图标索引
		std::string regValue = value;
		std::string iconPath;
		int iconIndex = 0;
		size_t commaPos = regValue.find_last_of(',');
		if (commaPos != std::string::npos) {
			iconPath = regValue.substr(0, commaPos);
			iconIndex = std::stoi(regValue.substr(commaPos + 1));
		}
		else {
			iconPath = regValue; // 如果没有逗号，整个字符串就是路径
			iconIndex = 0;    // 默认图标索引为0
		}

		// 提取图标
		hIcon = ExtractIconA(GetModuleHandleW(NULL), iconPath.c_str(), iconIndex);
		if (hIcon == NULL) {
			break;
		}
		
		retIcon = QtWin::fromHICON(hIcon);

	} while (false);

	if (hKey) {
		RegCloseKey(hKey);
	}

	if (hIcon) {
		DestroyIcon(hIcon);
	}

	return retIcon;
}

void DesktopSystemItemWatcher::run()
{
	if (!initWatcher()) {
		return;
	}

	while (TRUE) {
		LSTATUS regNotifyStatus = RegNotifyChangeKeyValue(
			m_monitorKey,
			FALSE,
			REG_NOTIFY_CHANGE_LAST_SET,
			m_monitorEvent,
			TRUE
		);
		if (regNotifyStatus != ERROR_SUCCESS) {
			continue;
		}

		DWORD dwWait = WaitForSingleObject(
			m_monitorEvent,
			INFINITE
		);

		if (dwWait == WAIT_OBJECT_0)
		{
			// 等待1秒再更新，防止注册表才写了一半的情况
			QThread::msleep(1000);
		}
		else
		{

		}
	}
}

DesktopFileItemWatcher::DesktopFileItemWatcher()
{
}

DesktopFileItemWatcher::~DesktopFileItemWatcher()
{
}

void DesktopFileItemWatcher::init()
{
	initWatcherDir();

	refreshFileItem();
}

void DesktopFileItemWatcher::run()
{
	// 每个目录关联一个事件以及句柄
	int nDirCount = m_observerInfos.size();
	QVector<HANDLE> hEventArray;
	FILE_NOTIFY_INFORMATION* pNotification = NULL;
	BOOL watchState = FALSE;
	DWORD dwNotifyFilter =
		FILE_NOTIFY_CHANGE_FILE_NAME |
		FILE_NOTIFY_CHANGE_DIR_NAME |
		FILE_NOTIFY_CHANGE_ATTRIBUTES;

	// 初始化，
	// 现在有两个init，要整合一下
	if (!initWatcher(dwNotifyFilter))
	{
		//LOG_ERROR_W("_InitObserver failed");
		goto _end; // 初始化失败，去做资源清理
	}

	int monitorSucCount = 0;
	for (int i = 0; i < nDirCount; ++i) {
		if (m_observerInfos[i].hEvent != NULL) {
			hEventArray.append(m_observerInfos[i].hEvent);
			++monitorSucCount;
		}
	}

	while (TRUE) {
		DWORD dwWait = ::WaitForMultipleObjects(monitorSucCount, hEventArray.data(), FALSE, INFINITE);
		if (WAIT_OBJECT_0 <= dwWait && dwWait < WAIT_OBJECT_0 + monitorSucCount) {
			int eventIndex = dwWait - WAIT_OBJECT_0;
			int watchInfoIndex = -1;
			// 匹配是哪个路径
			for (size_t i = 0; i < m_observerInfos.size(); ++i) {
				if (m_observerInfos[i].hEvent == hEventArray[eventIndex]) {
					watchInfoIndex = i;
					break;
				}
			}

			if (watchInfoIndex == -1) {
				//LOG_ERROR("something wrong, maybe m_observerInfos size() changed, please check code, eventIndex:%d, errno:(0x%x)", eventIndex, ::GetLastError());
				break;
			}

			DWORD bytesReturned = 0;
			if (!::GetOverlappedResult(
				m_observerInfos[watchInfoIndex].hDirectory,
				&m_observerInfos[watchInfoIndex].overlapped,
				&bytesReturned,
				TRUE)) {
				//LOG_ERROR_W("GetOverlappedResult failed, error=%d", ::GetLastError());
				continue;   // 继续监听
			}

			pNotification = (FILE_NOTIFY_INFORMATION*)m_observerInfos[watchInfoIndex].notifyDataBuf;
			handleObserveResult(m_observerInfos[watchInfoIndex].watchPath, pNotification);
			ZeroMemory(m_observerInfos[watchInfoIndex].notifyDataBuf, OBSERVE_DIR_BUFFER_SIZE);

			// 使用异步的ReadDirectoryChangesW
			if (::ReadDirectoryChangesW(
				m_observerInfos[watchInfoIndex].hDirectory,
				m_observerInfos[watchInfoIndex].notifyDataBuf,
				OBSERVE_DIR_BUFFER_SIZE,
				TRUE, dwNotifyFilter, NULL,
				&m_observerInfos[watchInfoIndex].overlapped, NULL) == FALSE
				) {
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
	uninitWatcher();
}

bool DesktopFileItemWatcher::initWatcherDir()
{
	for (auto csidl : { CSIDL_DESKTOPDIRECTORY, CSIDL_COMMON_DESKTOPDIRECTORY }) {
		WCHAR path[MAX_PATH] = { 0 };
		HRESULT hr = SHGetFolderPathW(NULL, csidl, NULL, 0, path);
		if (FAILED(hr)) {
			//LOG_ERROR_W("SHGetFolderPathW index(%d) failed hr(0x%x), err(%d).", i, hr, GetLastError());
			continue;
		}
		m_observerInfos.push_back({ path });
	}

	return !m_observerInfos.empty();
}

bool DesktopFileItemWatcher::initWatcher(DWORD dwNotifyFilter)
{
	int nInitSuccCnt = 0;

	for (DirObserver& observer : m_observerInfos) {
		observer.notifyDataBuf = new char[OBSERVE_DIR_BUFFER_SIZE] {0};

		observer.hEvent = ::CreateEventW(NULL, FALSE, FALSE, NULL);
		if (observer.hEvent == NULL) {
			//LOG_ERROR_W("Fail to create event, error=%d", ::GetLastError());
			continue;
		}
		observer.overlapped.hEvent = observer.hEvent;

		observer.hDirectory = ::CreateFileW(
			observer.watchPath.c_str(),
			GENERIC_READ | FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			NULL);

		if (observer.hDirectory == INVALID_HANDLE_VALUE)
		{
			//LOG_ERROR_W("CreateFile  failed:%ws err:%d",
			//	mVecAppLinkRootDir[i].c_str(), ::GetLastError());
			continue;
		}

		// 使用异步的ReadDirectoryChangesW
		if (::ReadDirectoryChangesW(
			observer.hDirectory,
			observer.notifyDataBuf,
			OBSERVE_DIR_BUFFER_SIZE,
			TRUE,
			dwNotifyFilter,
			NULL,
			&observer.overlapped,
			NULL) == FALSE)
		{
			//LOG_ERROR_W("ReadDirectoryChangesW failed.Dir path:%ws err:%d",
			//	mVecAppLinkRootDir[i].c_str(), ::GetLastError());
			continue;
		}

		++nInitSuccCnt;
	}

	return (nInitSuccCnt > 0);
}

void DesktopFileItemWatcher::uninitWatcher()
{
	for (int i = 0; i < m_observerInfos.size(); i++) {
		if (m_observerInfos[i].notifyDataBuf) {
			free(m_observerInfos[i].notifyDataBuf);
		}
	}

	m_observerInfos.clear();
}

void DesktopFileItemWatcher::refreshFileItem()
{
	m_fileItemList.clear();

	for (const auto& info : m_observerInfos) {
		QDir watchDir(QString::fromStdWString(info.watchPath));
		m_fileItemList.append(watchDir.entryInfoList(
			QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot
		));
	}

	emit fileItemRefreshed(m_fileItemList);
}

void DesktopFileItemWatcher::handleObserveResult(const std::wstring& strWatchDirectory, const FILE_NOTIFY_INFORMATION* pNotification)
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

void DesktopFileItemWatcher::handleFileCreated(const std::wstring& filePath)
{
	//auto it = std::find_if(mVecAppLinkInfo.begin(), mVecAppLinkInfo.end(), _IsSpecifiedPath(filePath));
	//if (it != mVecAppLinkInfo.end())    // 找到了说明出错
	//{
	//	//LOG_ERROR_W("_OnLnkFileCreated failed, lnk path already exist : %ws", filePath.c_str());
	//	return;
	//}
}

void DesktopFileItemWatcher::handleFileDeleted(const std::wstring& filePath)
{
}

void DesktopFileItemWatcher::handleFileModified(const std::wstring& filePath)
{
}

void DesktopFileItemWatcher::handleFileRenamed(const std::wstring& oldPath, const std::wstring& newPath)
{
}

HzDesktopItemModelPrivate::HzDesktopItemModelPrivate()
{
}

HzDesktopItemModelPrivate::~HzDesktopItemModelPrivate()
{}

void HzDesktopItemModelPrivate::init()
{
	connect(&m_systemItemWatcher,
		&DesktopSystemItemWatcher::systemAppRefreshed,
		this, [this](SystemAppInfoList infoList) {
			m_systemAppList = std::move(infoList);
		});

	connect(&m_fileItemWatcher,
		&DesktopFileItemWatcher::fileItemRefreshed,
		this, [this](QFileInfoList infoList) {
			m_fileItemList = std::move(infoList);
		});

	m_systemItemWatcher.init();
	m_fileItemWatcher.init();

	// TODO 决定优先级
	m_systemItemWatcher.start();
	m_fileItemWatcher.start();

	const QModelIndex topLeft = hzq_ptr->createIndex(0, 0);
	const QModelIndex bottomRight = hzq_ptr->createIndex(hzq_ptr->rowCount(), NumColumns - 1);
	emit hzq_ptr->dataChanged(topLeft, bottomRight);
}

const SystemAppInfoList& HzDesktopItemModelPrivate::getSystemAppInfoList() const
{
	return m_systemAppList;
}

const QFileInfoList& HzDesktopItemModelPrivate::getFileItemInfoList() const
{
	return m_fileItemList;
}

const QModelIndex HzDesktopItemModelPrivate::translateRealIndex(const QModelIndex& index) const
{
	// 根据row，决定是来自哪个部分
	const int& row = index.row();
	const int& column = index.column();

	if (column < HzDesktopItemModelPrivate::NumColumns) {
		if (row < m_systemAppList.size()) {
			return hzq_ptr->createIndex(row, column, SystemAppItem);
		}
		else if (row < m_systemAppList.size() + m_fileItemList.size()) {
			int offsetRow = row - m_systemAppList.size();
			return hzq_ptr->createIndex(offsetRow, column, NormalFileItem);;
		}
	}

	return QModelIndex();
}
