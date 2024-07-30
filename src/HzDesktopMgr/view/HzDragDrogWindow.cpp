#include <QGuiApplication>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>

#include <Shlwapi.h>

#include "HzDragDrogWindow.h"

HzDragDrogWindow::HzDragDrogWindow(const std::wstring& dirPath, QWidget* parent)
	: HzDragDropInterface(dirPath)
	, QAbstractItemView(parent)
	, m_emptyData(new QMimeData)
{
	//InitializeDragDropHelper(reinterpret_cast<HWND>(winId()));
}

HzDragDrogWindow::~HzDragDrogWindow()
{
	delete m_emptyData;
}

IFACEMETHODIMP HzDragDrogWindow::DragEnter(IDataObject* pdtobj, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect)
{
	HzDragDropInterface::DragEnter(pdtobj, grfKeyState, ptl, pdwEffect);

	QDragEnterEvent event(
		mapFromGlobal(QCursor::pos()),
		Qt::DropActions(*pdwEffect),
		m_emptyData,
		QGuiApplication::mouseButtons(),
		QGuiApplication::keyboardModifiers()
	);
	dragEnterEvent(&event);

	return S_OK;
}

IFACEMETHODIMP HzDragDrogWindow::DragOver(DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect)
{
	HzDragDropInterface::DragOver(grfKeyState, ptl, pdwEffect);

	QDragMoveEvent event(
		mapFromGlobal(QCursor::pos()),
		Qt::DropActions(*pdwEffect),
		m_emptyData,
		QGuiApplication::mouseButtons(),
		QGuiApplication::keyboardModifiers()
	);
	dragMoveEvent(&event);

	return S_OK;
}

IFACEMETHODIMP HzDragDrogWindow::DragLeave()
{
	HzDragDropInterface::DragLeave();

	QDragLeaveEvent event;
	dragLeaveEvent(&event);

	return S_OK;
}

IFACEMETHODIMP HzDragDrogWindow::Drop(IDataObject* pdtobj, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect)
{
	HzDragDropInterface::Drop(pdtobj, grfKeyState, ptl, pdwEffect);

	QDropEvent event(
		mapFromGlobal(QCursor::pos()),
		Qt::DropActions(*pdwEffect),
		m_emptyData,
		QGuiApplication::mouseButtons(),
		QGuiApplication::keyboardModifiers()
	);
	dropEvent(&event);

	return S_OK;
}

IFACEMETHODIMP HzDragDrogWindow::QueryInterface(REFIID riid, void** ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(HzDragDrogWindow, IDropTarget),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) HzDragDrogWindow::AddRef()
{
	return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) HzDragDrogWindow::Release()
{
	long cRef = InterlockedDecrement(&_cRef);
	if (!cRef)
		delete this;
	return cRef;
}