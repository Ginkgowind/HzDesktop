#include <windows.h>
#include <shlobj.h>
#include <objbase.h>

#include <QMimeData>
#include <QUrl>

#include "spdlog/spdlog.h"
#include "UiOperation.h"

//RAII����

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
				HRESULT res = SHParseDisplayName(windowsPath.c_str(), nullptr, &id, 0, nullptr);   //·��תPIDL
				if (!SUCCEEDED(res) || !id) {
					continue;
				}
				idvec.push_back(id);
				idChildvec.push_back(nullptr);
				res = SHBindToParent(id, IID_IShellFolder, (void**)&ifolder, &idChildvec.back());   //��ȡishellfolder
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
				(const ITEMIDLIST**)idChildvec.data(),     //��ȡ�Ҽ�UI��ť
				IID_IContextMenu, nullptr, (void**)&pContextMenu);//�ŵ�pContextMenu��
			if (FAILED(res)) {
				break;
			}

			FComInterfaceReleaser menuReleaser(pContextMenu);
			hMenu = CreatePopupMenu();
			if (!hMenu) {
				break;
			}

			if (SUCCEEDED(pContextMenu->QueryContextMenu(hMenu, 0, 1, 0x7FFF, CMF_NORMAL))) {    //Ĭ��ѡ��
				int iCmd = TrackPopupMenuEx(hMenu, TPM_RETURNCMD, showX,
					showY, hOwnerWnd, nullptr);
				if (iCmd > 0) {   //ִ�в˵�����
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
			UINT size = ILGetSize(pidl); // ��ȡITEMIDLIST�Ĵ�С
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
			HRESULT res = SHParseDisplayName(windowsPath.c_str(), nullptr, &id, 0, nullptr);   //·��תPIDL
			if (!SUCCEEDED(res) || !id) continue;
			idvec.push_back(id);
			idChildvec.push_back(nullptr);
			res = SHBindToParent(id, IID_IShellFolder, (void**)&ifolder, &idChildvec.back());   //��ȡishellfolder
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
			// ��ǰͼƬ��С����256�ģ�����Ҫ���м��
			return;
		}

		const QList<QSize>& avaliableSizeList = icon.availableSizes();
		int avaliableSizeCount = avaliableSizeList.size();

		if (avaliableSizeCount < MIN_PIXMAP_COUNT) {
			// ���ô�СС��2��������ͼ�ֱ꣬��ʹ��ԭʼ��С
			return;
		}

		QSize secondBigPixmapSize = avaliableSizeList[avaliableSizeCount - MIN_PIXMAP_COUNT];
		if (currentPixmapSize.width() <= secondBigPixmapSize.width()) {
			// ��ǰ��С�ȵڶ����ͼƬ��ҪС��˵�������˱�256����ߴ��ͼƬ������Qt�ϳɵģ�����Ҫ����
			return;
		}

		// ��ȡ��ǰͼƬ�ü��ڶ���ߴ������½�����ͼƬ
		QPixmap tempPixmap = pixmap.copy(
			secondBigPixmapSize.width(), secondBigPixmapSize.height(),
			currentPixmapSize.width() - secondBigPixmapSize.width(),
			currentPixmapSize.height() - secondBigPixmapSize.width());

		if (tempPixmap.toImage().allGray()) {
			// �ü����ͼƬ��������Ч���أ��ж�Ϊ��ЧͼƬ��ʹ�õڶ���ߴ��ͼƬ
			icon = icon.pixmap(secondBigPixmapSize);
		}

		//return pixmap.scaled(
		//	{ ICON_SIZE, ICON_SIZE },
		//	Qt::KeepAspectRatio,
		//	Qt::SmoothTransformation
		//);;
	}
}