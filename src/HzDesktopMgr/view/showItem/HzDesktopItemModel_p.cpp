#include <QSettings>
#include <QDir>
#include <shlwapi.h>
#include <Shlobj.h>
#include <QtWinExtras/QtWin>

#include "HzDesktopItemModel_p.h"
#include "windows/tools.h"

// ����ϵͳͼ�����ʾ�����أ�������Ѷ���������˴��Ӽ�ֻ������ʾ������
#define SYSTEM_ICON_REG_SUBPATH \
	"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\HideDesktopIcons\\NewStartPanel"


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
	initWatcher();

	refreshFileItem();
}

bool DesktopFileItemWatcher::initWatcher()
{
	for (auto csidl : { CSIDL_DESKTOPDIRECTORY, CSIDL_COMMON_DESKTOPDIRECTORY })
	{
		WCHAR path[MAX_PATH] = { 0 };
		HRESULT hr = SHGetFolderPathW(NULL, csidl, NULL, 0, path);
		if (FAILED(hr))
		{
			//LOG_ERROR_W("SHGetFolderPathW index(%d) failed hr(0x%x), err(%d).", i, hr, GetLastError());
			continue;
		}
		m_observerInfos.push_back({ path });
	}

	refreshFileItem();

	return !m_observerInfos.empty();
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

HzDesktopItemModelPrivate::HzDesktopItemModelPrivate()
{
}

HzDesktopItemModelPrivate::~HzDesktopItemModelPrivate()
{}

void DesktopFileItemWatcher::run()
{
}

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

	// TODO �������ȼ�
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
	// ����row�������������ĸ�����
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
