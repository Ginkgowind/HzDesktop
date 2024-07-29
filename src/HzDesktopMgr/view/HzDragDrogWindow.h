#pragma once

#include <QMimeData>
#include <QAbstractItemView>

#include "helper/DragDropHelpers.h"

/*
 * ������ΪHzDragDropInterface��QAbstractItemView֮��Drag����¼�����ת����ʹ�����ܹ�ʹ��Qt������
 */

// TODO �����������̳з������ͻ���뱨���˽�Ϊʲô
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

private:
    QMimeData* m_emptyData;
};

