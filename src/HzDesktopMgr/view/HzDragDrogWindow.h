#pragma once

#include <QMimeData>
#include <QAbstractItemView>

#include "helper/DragDropHelpers.h"

/*
 * 此类作为HzDragDropInterface和QAbstractItemView之间Drag相关事件的中转，来使子类能够使用Qt风格编码
 */

// TODO 下面这两个继承反过来就会编译报错，了解为什么
class HzDragDrogWindow
	: public QAbstractItemView
    , public HzDragDropInterface
{
public: 
    HzDragDrogWindow(const std::wstring& dirPath, QWidget* parent);

    ~HzDragDrogWindow();

protected:
    // IDropTarget
    IFACEMETHODIMP DragEnter(IDataObject* pdtobj, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect) override;

    IFACEMETHODIMP DragOver(DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect) override;

    IFACEMETHODIMP DragLeave() override;

    IFACEMETHODIMP Drop(IDataObject* pdtobj, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect) override;

    // IUnknown
	IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv) override;

	IFACEMETHODIMP_(ULONG) AddRef() override;

	IFACEMETHODIMP_(ULONG) Release() override;

private:
    QMimeData* m_emptyData;
    long _cRef;
};

