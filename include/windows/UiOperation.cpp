#include <windows.h>
#include <shlobj.h>
#include <objbase.h>

#include <QMimeData>
#include <QUrl>

#include "spdlog/spdlog.h"
#include "UiOperation.h"

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

	QMimeData* multiDrag(const QStringList& pathList)
	{
		if (pathList.empty()) return false;
		std::vector<ITEMIDLIST*> idvec;
		std::vector<LPCITEMIDLIST> idChildvec;
		QList<QUrl> urls;
		IShellFolder* ifolder = nullptr;
		for (QString path : pathList) {
			urls.append(QUrl::fromLocalFile(path));
			std::wstring windowsPath = path.toStdWString();
			std::replace(windowsPath.begin(), windowsPath.end(), '/', '\\');
			ITEMIDLIST* id = nullptr;
			HRESULT res = SHParseDisplayName(windowsPath.c_str(), nullptr, &id, 0, nullptr);   //路径转PIDL
			if (!SUCCEEDED(res) || !id) continue;
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
		if (ifolder == nullptr || idChildvec.empty()) return false;
		
		QMimeData* mimeData = new QMimeData;
		mimeData->setUrls(urls);
		mimeData->setData("application/x-qt-windows-mime;value=\"Shell IDList Array\"", 
			ConvertToQByteArray(idChildvec));

		return mimeData;
	}

	void correctPixmapIfIsInvalid(QIcon& icon)
	{
#define MAX_PIXMAP_SIZE (256)
#define MIN_PIXMAP_COUNT (2)
#define ICON_SIZE (90)


		QPixmap pixmap = icon.pixmap(ICON_SIZE);
		QSize currentPixmapSize = pixmap.size();

		if (currentPixmapSize.width() != MAX_PIXMAP_SIZE) {
			// 当前图片大小不是256的，不需要进行检测
			return;
		}

		const QList<QSize>& avaliableSizeList = icon.availableSizes();
		int avaliableSizeCount = avaliableSizeList.size();

		if (avaliableSizeCount < MIN_PIXMAP_COUNT) {
			// 可用大小小于2，非正常图标，直接使用原始大小
			return;
		}

		QSize secondBigPixmapSize = avaliableSizeList[avaliableSizeCount - MIN_PIXMAP_COUNT];
		if (currentPixmapSize.width() <= secondBigPixmapSize.width()) {
			// 当前大小比第二大的图片还要小，说明包含了比256更大尺寸的图片，不是Qt合成的，不需要处理
			return;
		}

		// 获取当前图片裁剪第二大尺寸后的右下角区域图片
		QPixmap tempPixmap = pixmap.copy(
			secondBigPixmapSize.width(), secondBigPixmapSize.height(),
			currentPixmapSize.width() - secondBigPixmapSize.width(),
			currentPixmapSize.height() - secondBigPixmapSize.width());

		if (tempPixmap.toImage().allGray()) {
			// 裁剪后的图片不包含有效像素，判定为无效图片，使用第二大尺寸的图片
			icon = icon.pixmap(secondBigPixmapSize);
		}

		//return pixmap.scaled(
		//	{ ICON_SIZE, ICON_SIZE },
		//	Qt::KeepAspectRatio,
		//	Qt::SmoothTransformation
		//);;
	}
}