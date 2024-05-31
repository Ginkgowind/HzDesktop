#include <QSettings>
#include <QDir>
#include <QDateTime>
#include <shlwapi.h>
#include <Shlobj.h>
#include <QtWinExtras/QtWin>

#include "HzDesktopItemModel_p.h"
#include "config/HzDesktopParam.h"
#include "windows/tools.h"
#include "common/CommonTools.h"
#include "windows/UiOperation.h"

// ����ϵͳͼ�����ʾ�����أ�������Ѷ���������˴��Ӽ�ֻ������ʾ������
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

void DesktopSystemItemWatcher::refreshSystemAppsInfo()
{
	static const QString showStatusRegPath =
		"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\HideDesktopIcons\\NewStartPanel";

	QList<QStandardItem*> systemItems;
	QSettings settings(showStatusRegPath, QSettings::Registry64Format);
	QStringList clsidValueList = settings.childKeys() + QStringList("{645FF040-5081-101B-9F08-00AA002F954E}");
	for (const QString& clsidValue : clsidValueList) {
		bool bHidden = settings.value(clsidValue).toBool();
		if (!bHidden) {
			QStandardItem* newItem = genQStandardItem(clsidValue);
			systemItems.push_back(newItem);
		}
	}

	emit systemAppRefreshed(systemItems);
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

QStandardItem* DesktopSystemItemWatcher::genQStandardItem(const QString& clsidValue)
{
	QStandardItem* newItem = new QStandardItem();
	QIcon itemIcon = getSystemAppIcon(clsidValue);
	HZ::correctPixmapIfIsInvalid(itemIcon);
	newItem->setIcon(itemIcon);
	newItem->setText(getSystemAppDisplayName("::" + clsidValue));
	newItem->setData("::" + clsidValue, CustomRoles::FilePathRole);
	
	return newItem;
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

// TODO �����õ�icon�ߴ�ƫС��������ʾ������ͼƬ�ȽϺ�
QIcon DesktopSystemItemWatcher::getSystemAppIcon(const QString& clsidValue)
{
	QIcon retIcon;

	HICON hIcon = NULL;
	HKEY hKey;

	do
	{
		// ��ע����
		std::string strSubKey = "CLSID\\" + clsidValue.toStdString() + "\\DefaultIcon";
		LSTATUS lRet = RegOpenKeyExA(HKEY_CLASSES_ROOT,
			strSubKey.c_str(),
			0, KEY_READ, &hKey);
		if (lRet != ERROR_SUCCESS) {
			break;
		}

		// ��ѯͼ��·��
		char value[MAX_PATH];
		DWORD valueLength = sizeof(value);
		lRet = RegQueryValueExA(
			hKey, NULL,	// Ĭ��ֵ
			NULL, NULL,
			(LPBYTE)value,
			&valueLength);
		if (lRet != ERROR_SUCCESS) {
			break;
		}

		// ����ͼ��·����ͼ������
		std::string regValue = value;
		std::string iconPath;
		int iconIndex = 0;
		size_t commaPos = regValue.find_last_of(',');
		if (commaPos != std::string::npos) {
			iconPath = regValue.substr(0, commaPos);
			iconIndex = std::stoi(regValue.substr(commaPos + 1));
		}
		else {
			iconPath = regValue; // ���û�ж��ţ������ַ�������·��
			iconIndex = 0;    // Ĭ��ͼ������Ϊ0
		}

		// ��ȡͼ��
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
			// �ȴ�1���ٸ��£���ֹע����д��һ������
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

void DesktopFileItemWatcher::refreshFileItem()
{
	QList<QStandardItem*> itemList;

	for (const auto& info : m_observerInfos) {
		QDir watchDir(QString::fromStdWString(info.watchPath));
		QFileInfoList fileInfoList = watchDir.entryInfoList(
			QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot
		);

		for (const QFileInfo& fileInfo : fileInfoList) {
			QStandardItem* newItem = genQStandardItem(fileInfo);
			itemList.push_back(newItem);
		}
	}

	emit fileItemRefreshed(itemList);
}

void DesktopFileItemWatcher::run()
{
	// ÿ��Ŀ¼����һ���¼��Լ����
	int nDirCount = m_observerInfos.size();
	QVector<HANDLE> hEventArray;
	FILE_NOTIFY_INFORMATION* pNotification = NULL;
	BOOL watchState = FALSE;
	DWORD dwNotifyFilter =
		FILE_NOTIFY_CHANGE_FILE_NAME |
		FILE_NOTIFY_CHANGE_DIR_NAME |
		FILE_NOTIFY_CHANGE_ATTRIBUTES |
		FILE_NOTIFY_CHANGE_LAST_WRITE;

	// ��ʼ����
	// ����������init��Ҫ����һ��
	if (!initWatcher(dwNotifyFilter))
	{
		//LOG_ERROR_W("_InitObserver failed");
		goto _end; // ��ʼ��ʧ�ܣ�ȥ����Դ����
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
			// ƥ�����ĸ�·��
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
				continue;   // ��������
			}

			pNotification = (FILE_NOTIFY_INFORMATION*)m_observerInfos[watchInfoIndex].notifyDataBuf;
			handleObserveResult(
				QString::fromStdWString(m_observerInfos[watchInfoIndex].watchPath),
				pNotification);
			ZeroMemory(m_observerInfos[watchInfoIndex].notifyDataBuf, OBSERVE_DIR_BUFFER_SIZE);

			// ʹ���첽��ReadDirectoryChangesW
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

		// ʹ���첽��ReadDirectoryChangesW
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

QStandardItem* DesktopFileItemWatcher::genQStandardItem(const QFileInfo& fileInfo)
{
	QStandardItem* newItem = new QStandardItem();
	
	QIcon itemIcon = getUltimateIcon(fileInfo);
	HZ::correctPixmapIfIsInvalid(itemIcon);
	newItem->setIcon(itemIcon);
	newItem->setText(getFileShowText(fileInfo));
	newItem->setData(fileInfo.absoluteFilePath(), CustomRoles::FilePathRole);
	newItem->setData(fileInfo.fileName(), CustomRoles::FileNameRole);
	newItem->setData(fileInfo.size(), CustomRoles::FileSizeRole);
	newItem->setData(fileInfo.suffix(), CustomRoles::FileTypeRole);
	newItem->setData(fileInfo.lastModified(), CustomRoles::FileLastModifiedRole);

	return newItem;
}

QIcon DesktopFileItemWatcher::getUltimateIcon(const QFileInfo& fileInfo)
{
	if (fileInfo.isShortcut()) {
		QFileInfo targetFileInfo(fileInfo.symLinkTarget());
		return QFileIconProvider().icon(targetFileInfo);
	}
	else {
		return QFileIconProvider().icon(fileInfo);
	}

	return QIcon();
}

QString DesktopFileItemWatcher::getFileShowText(const QFileInfo& fileInfo)
{
	// Qt��QFileInfo::isShortcut δ�ж�.url��׺
	bool bIsShortCut
		= fileInfo.suffix().compare("lnk", Qt::CaseInsensitive) == 0
		|| fileInfo.suffix().compare("url", Qt::CaseInsensitive) == 0
		|| fileInfo.suffix().compare("pif", Qt::CaseInsensitive) == 0
		|| fileInfo.suffix().compare("scf", Qt::CaseInsensitive) == 0;

	// TODO ������Ϊ������
	bool bShowSuffix = true;

	if (bIsShortCut || !bShowSuffix) {
		return fileInfo.baseName();
	}

	return fileInfo.fileName();
}

void DesktopFileItemWatcher::handleObserveResult(const QString& strWatchDirectory, const FILE_NOTIFY_INFORMATION* pNotification)
{
	assert(pNotification);

	QString strFileName = QString::fromStdWString(
		std::wstring(pNotification->FileName,
			pNotification->FileNameLength / sizeof(wchar_t)));
	QString strFileAbsPath = QDir(strWatchDirectory).absoluteFilePath(strFileName);
	
	DWORD cbOffset = 0;
	QString strRenameOldFilePath;

	do
	{
		switch (pNotification->Action)
		{
		case FILE_ACTION_ADDED:
		{
			//LOG_DEBUG_W("FILE_ACTION_ADDED: %ws", strFileAbsPath.c_str());
			QStandardItem* newItem = genQStandardItem(QFileInfo(strFileAbsPath));
			emit onFileCreated(newItem);
			break;
		}
		case FILE_ACTION_REMOVED:
		{
			//LOG_DEBUG_W("FILE_ACTION_REMOVED: %ws", strFileAbsPath.c_str());
			emit onFileDeleted(strFileAbsPath);
			break;
		}
		case FILE_ACTION_MODIFIED:
		{
			//LOG_DEBUG_W("FILE_ACTION_MODIFIED: %ws", strFileAbsPath.c_str());
			QStandardItem* newItem = genQStandardItem(QFileInfo(strFileAbsPath));
			emit onFileModified(strFileAbsPath, newItem);
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
			emit onFileRenamed(strRenameOldFilePath, strFileAbsPath, strFileName);
			break;
		}
		default:
			break;
		}

		cbOffset = pNotification->NextEntryOffset;
		pNotification = (PFILE_NOTIFY_INFORMATION)((LPBYTE)pNotification + cbOffset);

		if (cbOffset)
		{
			// ��ȡ�µ�·����Ϣ
			strFileName = QString::fromStdWString(
				std::wstring(pNotification->FileName,
					pNotification->FileNameLength / sizeof(wchar_t)));
			strFileAbsPath = QDir(strWatchDirectory).absoluteFilePath(strFileName);
		}

	} while (cbOffset);
}

HzDesktopItemModelPrivate::HzDesktopItemModelPrivate()
{
}

HzDesktopItemModelPrivate::~HzDesktopItemModelPrivate()
{}

void HzDesktopItemModelPrivate::init()
{
	HZQ_Q(HzDesktopItemModel);

	connect(&m_systemItemWatcher,
		&DesktopSystemItemWatcher::systemAppRefreshed,
		this, [&](QList<QStandardItem*> itemList) {
			for (QStandardItem* item : itemList) {
				// TODO �����Ȱ�˳�����������Ϊ������
				q->appendRow(item);
			}
			// TODO Ϊʲô�����������У�
			//hzq_ptr->appendRow(itemList);
		});

	connect(&m_fileItemWatcher,
		&DesktopFileItemWatcher::fileItemRefreshed,
		this, [&](QList<QStandardItem*> itemList) {
			for (QStandardItem* item : itemList) {
				q->appendRow(item);
			}
		});

	// TODO Ŀǰ�����ɾ������겻�ƶ��������Ͼ�ˢ�²�����������Ϊʲô
	connect(&m_fileItemWatcher, &DesktopFileItemWatcher::onFileCreated,
		this, &HzDesktopItemModelPrivate::handleFileCreated);

	connect(&m_fileItemWatcher, &DesktopFileItemWatcher::onFileDeleted,
		this, &HzDesktopItemModelPrivate::handleFileDeleted);

	connect(&m_fileItemWatcher, &DesktopFileItemWatcher::onFileModified,
		this, &HzDesktopItemModelPrivate::handleFileModified);

	connect(&m_fileItemWatcher, &DesktopFileItemWatcher::onFileRenamed,
		this, &HzDesktopItemModelPrivate::handleFileRenamed);

	m_systemItemWatcher.init();
	m_fileItemWatcher.init();

	// TODO �������ȼ�
	m_systemItemWatcher.start();
	m_fileItemWatcher.start();
}

void HzDesktopItemModelPrivate::handleFileCreated(QStandardItem* item)
{
	HZQ_Q(HzDesktopItemModel);

	// ���ҵ�һ���ո����õ���λ�ã��������ӵ�ĩβ
	for (int i = 0; i < q->rowCount(); i++) {
		QStandardItem* tmpItem = q->itemFromIndex(q->index(i, 0));
		if (!tmpItem->isEnabled()) {
			q->setItem(i, item);
			item = nullptr;
		}
	}

	if (item) {
		q->appendRow(item);
	}
}

void HzDesktopItemModelPrivate::handleFileDeleted(const QString& filePath)
{
	HZQ_Q(HzDesktopItemModel);

	for (int i = 0; i < q->rowCount(); i++) {
		if (q->filePath(q->index(i, 0)) == filePath) {
			q->removeRow(i);
			break;
		}
	}
}

void HzDesktopItemModelPrivate::handleFileModified(const QString& filePath, QStandardItem* item)
{
	HZQ_Q(HzDesktopItemModel);

	for (int i = 0; i < q->rowCount(); i++) {
		if (q->filePath(q->index(i, 0)) == filePath) {
			q->setItem(i, item);
			break;
		}
	}
}

void HzDesktopItemModelPrivate::handleFileRenamed(
	const QString& oldPath, 
	const QString& newPath, 
	const QString& newFileName
)
{
	HZQ_Q(HzDesktopItemModel);

	for (int i = 0; i < q->rowCount(); i++) {
		if (q->filePath(q->index(i, 0)) == oldPath) {
			QStandardItem* item = q->itemFromIndex(q->index(i, 0));
			item->setText(newFileName);
			item->setData(newPath, CustomRoles::FilePathRole);
			break;
		}
	}
}

