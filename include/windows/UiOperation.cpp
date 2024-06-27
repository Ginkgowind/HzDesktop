#include <windows.h>
#include <shlobj.h>
#include <objbase.h>
#include <shlwapi.h>
#include <commoncontrols.h>
#include <wil/com.h>

#include <QDebug>
#include <QUrl>
#include <QMimeData>
#include <QSettings>
#include <QFileIconProvider>
#include <QtWinExtras/QtWin>

#include "UiOperation.h"
#include "spdlog/spdlog.h"
#include "config/HzDesktopParam.h"

#define BUFFER_SIZE 1024


//RAII机制

class FItemIdListReleaser {
public:
	explicit FItemIdListReleaser(ITEMIDLIST* idList) : m_idList(idList) {}
	~FItemIdListReleaser() { if (m_idList) CoTaskMemFree(m_idList); }
private:
	ITEMIDLIST* m_idList;
};

class FComInterfaceReleaser {
public:
	explicit FComInterfaceReleaser(IUnknown* i) : m_ptr(i) {}
	~FComInterfaceReleaser() { if (m_ptr) m_ptr->Release(); }
private:
	IUnknown* m_ptr;
};

class FItemIdListVectorReleaser {
public:
	explicit FItemIdListVectorReleaser(const std::vector<ITEMIDLIST*>& idArray) : m_array(idArray) {}
	~FItemIdListVectorReleaser() {
		for (ITEMIDLIST* item : m_array)
			CoTaskMemFree(item);
	}
	FItemIdListVectorReleaser& operator=(const FItemIdListVectorReleaser&) = delete;
private:
	const std::vector<ITEMIDLIST*>& m_array;
};

namespace HZ
{
	bool showContentMenuWin10(
		WId ownerWId,
		const QStringList& pathList,
		int showX,
		int showY)
	{
		bool bRet = false;
		HRESULT hRet = S_OK;

		HWND hOwnerWnd = reinterpret_cast<HWND>(ownerWId);
		IContextMenu* pContextMenu = nullptr;
		HMENU hMenu = nullptr;

		do
		{
			if (pathList.empty()) {
				break;
			}

			std::vector<ITEMIDLIST*> idvec;
			std::vector<LPCITEMIDLIST> idChildvec;
			IShellFolder* ifolder = nullptr;
			
			for (QString path : pathList) {
				std::wstring windowsPath = path.toStdWString();
				std::replace(windowsPath.begin(), windowsPath.end(), '/', '\\');
				ITEMIDLIST* id = nullptr;
				HRESULT res = SHParseDisplayName(windowsPath.c_str(), nullptr, &id, 0, nullptr);   //路径转PIDL
				if (!SUCCEEDED(res) || !id) {
					continue;
				}
				idvec.push_back(id);
				idChildvec.push_back(nullptr);
				res = SHBindToParent(id, IID_IShellFolder, (void**)&ifolder, &idChildvec.back());   //获取ishellfolder
				if (!SUCCEEDED(res) || !idChildvec.back())
					idChildvec.pop_back();
				else if (path.compare(pathList.back()) != 0 && ifolder) {
					ifolder->Release();
					ifolder = nullptr;
				}
			}
			FItemIdListVectorReleaser vecReleaser(idvec);
			FComInterfaceReleaser ifolderReleaser(ifolder);
			if (ifolder == nullptr || idChildvec.empty()) {
				break;
			}

			IContextMenu* pContextMenu = nullptr;
			HRESULT res = ifolder->GetUIObjectOf(hOwnerWnd, (UINT)idChildvec.size(),
				(const ITEMIDLIST**)idChildvec.data(),     //获取右键UI按钮
				IID_IContextMenu, nullptr, (void**)&pContextMenu);//放到pContextMenu中
			if (FAILED(res)) {
				break;
			}

			FComInterfaceReleaser menuReleaser(pContextMenu);
			hMenu = CreatePopupMenu();
			if (!hMenu) {
				break;
			}

			if (SUCCEEDED(pContextMenu->QueryContextMenu(hMenu, 0, 1, 0x7FFF, CMF_NORMAL))) {    //默认选项
				int iCmd = TrackPopupMenuEx(hMenu, TPM_RETURNCMD, showX,
					showY, hOwnerWnd, nullptr);
				if (iCmd > 0) {   //执行菜单命令
					CMINVOKECOMMANDINFOEX info = { 0 };
					info.cbSize = sizeof(info);
					info.fMask = CMIC_MASK_UNICODE;
					info.hwnd = hOwnerWnd;
					info.lpVerb = MAKEINTRESOURCEA(iCmd - 1);
					info.lpVerbW = MAKEINTRESOURCEW(iCmd - 1);
					info.nShow = SW_SHOWNORMAL;
					pContextMenu->InvokeCommand((LPCMINVOKECOMMANDINFO)&info);
				}
			}

			bRet = true;
		} while (false);

		if (pContextMenu) {
			pContextMenu->Release();
		}

		if (hMenu) {
			DestroyMenu(hMenu);
		}

		return bRet;
	}

	QByteArray ConvertToQByteArray(const std::vector<LPCITEMIDLIST>& idChildvec) {
		QByteArray result;
		for (const auto& pidl : idChildvec) {
			UINT size = ILGetSize(pidl); // 获取ITEMIDLIST的大小
			result.append(reinterpret_cast<const char*>(pidl), size);
		}
		return result;
	}

	bool isInvalidPixmap(const QPixmap& pixmap)
	{
		bool bRet = false;

		do
		{
			int a = pixmap.width();
			if (pixmap.width() < MAX_ICON_SIZE) {
				// 当前图片大小不是256的，不需要进行检测
				break;
			}

			// 获取当前图片裁剪第二大尺寸后的右下角区域图片
			QPixmap tempPixmap = pixmap.copy(
				SECOND_MAX_ICON_SIZE, SECOND_MAX_ICON_SIZE,
				pixmap.width() - SECOND_MAX_ICON_SIZE,
				pixmap.height() - SECOND_MAX_ICON_SIZE);

			if (!tempPixmap.toImage().allGray()) {
				break;
			}

			bRet = true;
		} while (false);

		return bRet;
	}

	QString getDirectString(const QString& resStr)
	{
		WCHAR outBuff[BUFFER_SIZE] = { 0 };
		SHLoadIndirectString(resStr.toStdWString().c_str(), outBuff,
			BUFFER_SIZE, nullptr);

		return QString::fromStdWString(outBuff);
	}

	QIcon getIconFromLocation(const QString& location)
	{
		HICON hIcon = nullptr;

		// 分离图标路径和图标索引
		QString locationCopy = location;
		QString iconPath;
		int iconIndex = 0;
		int commaPos = locationCopy.lastIndexOf(',');
		if (commaPos != std::string::npos) {
			iconPath = locationCopy.mid(0, commaPos);
			iconIndex = locationCopy.mid(commaPos + 1).toInt();
		}
		else {
			iconPath = locationCopy; // 如果没有逗号，整个字符串就是路径
			iconIndex = 0;    // 默认图标索引为0
		}

		// iconIndex为负数就是指定资源标识符, 为正数就是该图标在资源文件中的顺序序号,
		// 为-1时表示默认资源，不能使用ExtractIconEx提取图标
		if (iconIndex == -1) {
			WCHAR resPath[MAX_PATH] = { 0 };
			ExpandEnvironmentStrings(iconPath.toStdWString().c_str(), resPath, MAX_PATH);
			HMODULE hModule = LoadLibrary(resPath);
			hIcon = (HICON)LoadImage(hModule, L"#1", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
		}
		else {
			ExtractIconExA(iconPath.toStdString().c_str(), iconIndex, &hIcon, nullptr, 1);
		}

		return QtWin::fromHICON(hIcon);
	}

	QString getTextFromGUID(const QString& guidStr)
	{
		static const QVector<QString> s_clsidPaths = {
			"HKEY_CLASSES_ROOT\\CLSID\\",
			"HKEY_CLASSES_ROOT\\WOW6432Node\\CLSID\\",
			"HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Classes\\CLSID\\",
		};

		QString itemText;

		GUID guid = { 0 };

		do
		{
			//CLSIDFromString((LPCOLESTR)(LPCWSTR)guidStr.toStdWString().c_str(), &guid);
			//if (IsEqualGUID(guid, GUID_NULL)) {
			//	break;
			//}

			for (const QString& clsidPath : s_clsidPaths)
			{
				QSettings guidReg(clsidPath + guidStr, QSettings::NativeFormat);

				for (const QString& valueName : { "LocalizedString", "InfoTip", "." }) {
					itemText = guidReg.value(valueName).toString();
					itemText = getDirectString(itemText);
					if (!itemText.isEmpty()) {
						break;
					}
				}

				if (!itemText.isEmpty()) {
					break;
				}
			}
		} while (false);

		return itemText;
	}

	QIcon getIconFromGUID(const QString& guid)
	{
		return QIcon();
	}

	QPixmap getThumbnailFromPath(const QString& filePath)
	{
		QPixmap pixmap;
		LPITEMIDLIST idl = nullptr;

		do
		{
			QString correctFilePath = filePath;
			//QString correctFilePath = "C:\\Users\\SXF-Admin\\Desktop";
			correctFilePath.replace('/', '\\');

			HRESULT hr = SHParseDisplayName(correctFilePath.toStdWString().c_str(), nullptr, &idl, 0, nullptr);
			if (FAILED(hr)) {
				break;
			}

			wil::com_ptr<IShellFolder> pShellFolder;
			// TODO 了解所有获取com接口的方式及其原理
			hr = SHBindToParent(idl, IID_PPV_ARGS(&pShellFolder), nullptr);
			if (FAILED(hr)) {
				break;
			}

			LPCITEMIDLIST pChildPIDL = ILFindLastID(idl);

			wil::com_ptr<IExtractImage> pExtractImage;
			hr = pShellFolder->GetUIObjectOf(nullptr, 1, const_cast<LPCITEMIDLIST*>(&pChildPIDL),
				__uuidof(*pExtractImage), nullptr, IID_PPV_ARGS_Helper(&pExtractImage));
			if (FAILED(hr)) {
				break;
			}

			SIZE size = { MAX_ICON_SIZE, MAX_ICON_SIZE };
			DWORD dwFlags = IEIFLAG_OFFLINE | IEIFLAG_QUALITY;

			// As per the MSDN documentation, GetLocation must be called before Extract.
			TCHAR szImage[MAX_PATH];
			DWORD dwPriority;
			hr = pExtractImage->GetLocation(szImage, MAX_PATH, &dwPriority, &size, 32, &dwFlags);
			if (FAILED(hr) && hr != E_PENDING) {
				break;
			}

			// TODO 看看有没有没释放的HBITMAP
			wil::unique_hbitmap hThumbnailBitmap;
			hr = pExtractImage->Extract(&hThumbnailBitmap);
			if (FAILED(hr)) {
				break;
			}

			// 直接用HBITMAP转QPixmap会有问题，故先转成HICON再转QPixmap
			QPixmap tmpPixmap = QtWin::fromHBITMAP(hThumbnailBitmap.get(), QtWin::HBitmapAlpha);
			HICON hIcon = QtWin::toHICON(tmpPixmap);
			pixmap = QtWin::fromHICON(hIcon);
			DestroyIcon(hIcon);
		} while (false);

		if (idl) {
			CoTaskMemFree(idl);
		}

		return pixmap;
	}

	QPixmap getPixmapFromPath(const QString& filePath)
	{
		QPixmap pixmap;
		LPITEMIDLIST idl;
		do
		{
			QString correctFilePath = filePath;
			correctFilePath.replace('/', '\\');

			HRESULT hr = SHParseDisplayName(correctFilePath.toStdWString().c_str(), nullptr, &idl, 0, nullptr);
			if (FAILED(hr)) {
				break;
			}

			SHFILEINFO sfi = { 0 };
			// 获取文件的图标索引
			DWORD_PTR dwResult = SHGetFileInfo((LPCWSTR)idl, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_PIDL);
			if (dwResult == 0) {
				break;
			}

			auto extractPixmapFunc = [&](int ilMode) {
				// 获取系统图像列表的句柄
				wil::com_ptr<IImageList> pImageList;
				hr = SHGetImageList(ilMode, IID_PPV_ARGS(&pImageList));
				if (FAILED(hr)) {
					return;
				}

				HICON hIcon;
				hr = pImageList->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &hIcon);
				if (FAILED(hr)) {
					return;
				}

				pixmap = QtWin::fromHICON(hIcon);
				DestroyIcon(hIcon);
				// TODO 看看还有哪里没有destory
				};

			extractPixmapFunc(SHIL_LAST);
			if (isInvalidPixmap(pixmap)) {
				extractPixmapFunc(SHIL_EXTRALARGE);
			}
		} while (false);

		if (idl) {
			CoTaskMemFree(idl);
		}

		return pixmap;
	}
}