#include <windows.h>
#include <shlobj.h>
#include <objbase.h>

#include "spdlog/spdlog.h"
#include "UiOperation.h"

namespace HZ
{
	bool showContentMenuWin10(
		WId ownerWId,
		const QString& filePath,
		int x,
		int y)
	{
		bool bRet = false;
		HRESULT hRet = S_OK;

		HWND hOwnerWnd = reinterpret_cast<HWND>(ownerWId);
		IShellItem* pShellItem = nullptr;
		IContextMenu* pContextMenu = nullptr;
		HMENU hMenu = nullptr;

		do
		{
			hRet = SHCreateItemFromParsingName(
				filePath.toStdWString().c_str(),
				nullptr,
				IID_PPV_ARGS(&pShellItem)
			);
			if (FAILED(hRet)) {
				break;
			}

			hRet = pShellItem->BindToHandler(nullptr, BHID_SFUIObject, IID_PPV_ARGS(&pContextMenu));
			if (FAILED(hRet)) {
				break;
			}

			hMenu = CreatePopupMenu();
			if (!hMenu) {
				break;
			}

			hRet = pContextMenu->QueryContextMenu(hMenu, 0, 1, 0x7FFF, CMF_NORMAL);
			if (FAILED(hRet)) {
				break;
			}

			int res = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_NONOTIFY, x, y, 0, hOwnerWnd, nullptr);
			if (res > 0) {
				CMINVOKECOMMANDINFO info = { 0 };
				info.cbSize = sizeof(CMINVOKECOMMANDINFO);
				info.hwnd = hOwnerWnd;
				info.lpVerb = MAKEINTRESOURCEA(res - 1);	// 查看这个值的含义
				info.nShow = SW_SHOWNORMAL;
				pContextMenu->InvokeCommand(&info);
			}

			bRet = true;
		} while (false);

		if (pShellItem) {
			pShellItem->Release();
		}

		if (pContextMenu) {
			pContextMenu->Release();
		}

		if (hMenu) {
			DestroyMenu(hMenu);
		}

		return bRet;
	}
}