#include <QUrl>
#include <QFileInfo>

#include <wil/com.h>

#include "HzWindowsMimeIdl.h"

#define CHECK_FAILED_RETURN_FALSE(hr)	\
	if (FAILED(hr)) {					\
		return false;					\
	}

static int getCf(const FORMATETC& formatetc)
{
	return formatetc.cfFormat;
}

static FORMATETC setCf(int cf)
{
	FORMATETC formatetc;
	formatetc.cfFormat = CLIPFORMAT(cf);
	formatetc.dwAspect = DVASPECT_CONTENT;
	formatetc.lindex = -1;
	formatetc.ptd = nullptr;
	formatetc.tymed = TYMED_HGLOBAL;
	return formatetc;
}

static bool setData(const QByteArray& data, STGMEDIUM* pmedium)
{
	HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE, SIZE_T(data.size()));
	if (!hData)
		return false;

	void* out = GlobalLock(hData);
	memcpy(out, data.data(), size_t(data.size()));
	GlobalUnlock(hData);

	//HGLOBAL hData = OleDuplicateData((HANDLE)data.data(), 49363, 0);

	pmedium->tymed = TYMED_HGLOBAL;
	pmedium->hGlobal = hData;
	pmedium->pUnkForRelease = nullptr;
	return true;
}

static QByteArray getData(int cf, IDataObject* pDataObj, int lindex = -1)
{
	QByteArray data;
	FORMATETC formatetc = setCf(cf);
	formatetc.lindex = lindex;
	STGMEDIUM s;
	if (pDataObj->GetData(&formatetc, &s) == S_OK) {
		const void* val = GlobalLock(s.hGlobal);
		data = QByteArray::fromRawData(reinterpret_cast<const char*>(val), int(GlobalSize(s.hGlobal)));
		data.detach();
		GlobalUnlock(s.hGlobal);
		ReleaseStgMedium(&s);
	}
	else {
		//Try reading IStream data
		formatetc.tymed = TYMED_ISTREAM;
		if (pDataObj->GetData(&formatetc, &s) == S_OK) {
			char szBuffer[4096];
			ULONG actualRead = 0;
			LARGE_INTEGER pos = { {0, 0} };
			//Move to front (can fail depending on the data model implemented)
			HRESULT hr = s.pstm->Seek(pos, STREAM_SEEK_SET, nullptr);
			while (SUCCEEDED(hr)) {
				hr = s.pstm->Read(szBuffer, sizeof(szBuffer), &actualRead);
				if (SUCCEEDED(hr) && actualRead > 0) {
					data += QByteArray::fromRawData(szBuffer, int(actualRead));
				}
				if (actualRead != sizeof(szBuffer))
					break;
			}
			data.detach();
			ReleaseStgMedium(&s);
		}
	}
	return data;
}

static bool canGetData(int cf, IDataObject* pDataObj)
{
	FORMATETC formatetc = setCf(cf);
	if (pDataObj->QueryGetData(&formatetc) != S_OK) {
		formatetc.tymed = TYMED_ISTREAM;
		return pDataObj->QueryGetData(&formatetc) == S_OK;
	}
	return true;
}

static inline QString shellIdlLiteral() { return QStringLiteral("application/x-qt-windows-mime;value=\"Shell IDList Array\""); }

//static inline QString shellIdlLiteral() { return QStringLiteral("Shell IDList Array"); }


HzWindowsMimeIdl::HzWindowsMimeIdl()
{
	CF_SHELLIDL = QWinMime::registerMimeType(QString::fromStdWString(CFSTR_SHELLIDLIST));
	//CF_SHELLIDL = QWinMime::registerMimeType(shellIdlLiteral());
}

QVector<FORMATETC> HzWindowsMimeIdl::formatsForMime(const QString& mimeType, const QMimeData* mimeData) const
{
	QVector<FORMATETC> formatetcs;
	if (mimeType == shellIdlLiteral() && (!mimeData->data(shellIdlLiteral()).isEmpty()))
		formatetcs += setCf(CF_SHELLIDL);
	return formatetcs;
}

QString HzWindowsMimeIdl::mimeForFormat(const FORMATETC& formatetc) const
{
	if (getCf(formatetc) == CF_SHELLIDL)
		return shellIdlLiteral();
	return QString();
}

bool HzWindowsMimeIdl::canConvertToMime(const QString& mimeType, IDataObject* pDataObj) const
{
	return mimeType == shellIdlLiteral() && canGetData(CF_SHELLIDL, pDataObj);
}

bool HzWindowsMimeIdl::canConvertFromMime(const FORMATETC& formatetc, const QMimeData* mimeData) const
{
	if (getCf(formatetc) == CF_SHELLIDL) {
		auto test = mimeData->data(shellIdlLiteral());
	}

	return getCf(formatetc) == CF_SHELLIDL && (!mimeData->data(shellIdlLiteral()).isEmpty());
}

QVariant HzWindowsMimeIdl::convertToMime(const QString& mime, IDataObject* pDataObj, QVariant::Type preferredType) const
{
	Q_UNUSED(preferredType);
	QVariant result;
	if (canConvertToMime(mime, pDataObj)) {
		QByteArray data = getData(CF_SHELLIDL, pDataObj);
		result = data;
	}
	return result;
}

bool HzWindowsMimeIdl::convertFromMime(const FORMATETC& formatetc, const QMimeData* mimeData, STGMEDIUM* pmedium) const
{
	if (!canConvertFromMime(formatetc, mimeData) || !mimeData->hasUrls()) {
		return false;
	}

	QVector<QString> paths;
	for (QUrl url : mimeData->urls()) {
		paths.push_back(url.toLocalFile());
	}

	// 找出父路径，有不同就直接返回
	QString parentPath(QFileInfo(paths[0]).absolutePath());
	for (int i = 1; i < paths.size(); ++i) {
		if (QFileInfo(paths[i]).absolutePath() != parentPath) {
			return false;
		}
	}

	PIDLIST_ABSOLUTE pidlDirectory = NULL;
	HRESULT hr = SHParseDisplayName(parentPath.replace('/', '\\').toStdWString().c_str(), nullptr, &pidlDirectory, 0, nullptr);
	CHECK_FAILED_RETURN_FALSE(hr);

	IShellFolder* pDesktop = NULL;
	hr = SHGetDesktopFolder(&pDesktop);
	CHECK_FAILED_RETURN_FALSE(hr);

	wil::com_ptr<IShellFolder> pShellFolder;
	hr = pDesktop->BindToObject(pidlDirectory, NULL, IID_PPV_ARGS(&pShellFolder));
	CHECK_FAILED_RETURN_FALSE(hr);

	std::vector<PITEMID_CHILD> pidlList;
	for (auto path : paths) {
		QString fileName = QFileInfo(path).fileName();
		LPITEMIDLIST id = nullptr;
		hr = pShellFolder->ParseDisplayName(nullptr, nullptr, (LPWSTR)fileName.toStdWString().c_str(), nullptr,
			&id, nullptr);
		CHECK_FAILED_RETURN_FALSE(hr);
		pidlList.push_back(id);
	}

	/* First, we need to decide how much memory to
	allocate to the structure. This is based on
	the number of items that will be stored in
	this structure. */
	UINT uSize = 0;

	UINT nItems = static_cast<UINT>(pidlList.size());

	/* Size of the base structure + offset array. */
	UINT uBaseSize = sizeof(CIDA) + (sizeof(UINT) * nItems);

	uSize += uBaseSize;

	/* Size of the parent pidl. */
	uSize += ILGetSize(pidlDirectory);

	/* Add the total size of the child pidl's. */
	for (auto pidl : pidlList)
	{
		uSize += ILGetSize(pidl);
	}

	HGLOBAL hglbIDList = GlobalAlloc(GMEM_MOVEABLE, uSize);

	if (hglbIDList == NULL)
	{
		return false;
	}

	auto pcidaData = static_cast<LPVOID>(GlobalLock(hglbIDList));

	CIDA* pcida = static_cast<CIDA*>(pcidaData);

	pcida->cidl = nItems;

	UINT* pOffsets = pcida->aoffset;

	pOffsets[0] = uBaseSize;

	LPBYTE pData;

	pData = (LPBYTE)(((LPBYTE)pcida) + pcida->aoffset[0]);

	memcpy(pData, (LPVOID)pidlDirectory, ILGetSize(pidlDirectory));


	UINT uPreviousSize;
	int i = 0;

	uPreviousSize = ILGetSize(pidlDirectory);

	/* Store each of the pidl's. */
	for (auto pidl : pidlList) {
		pOffsets[i + 1] = pOffsets[i] + uPreviousSize;

		pData = (LPBYTE)(((LPBYTE)pcida) + pcida->aoffset[i + 1]);

		memcpy(pData, (LPVOID)pidl, ILGetSize(pidl));

		uPreviousSize = ILGetSize(pidl);

		i++;
	}

	GlobalUnlock(hglbIDList);

	pmedium->pUnkForRelease = 0;
	pmedium->hGlobal = hglbIDList;
	pmedium->tymed = TYMED_HGLOBAL;

	return true;
}