#pragma once

#include <QWinMime>
#include <QMimeData>
#include <ShlObj_core.h>
//#include <QStaticByteArrayMatcher>

class HzWindowsMimeIdl : public QWinMime
{
public:
	HzWindowsMimeIdl();

	// for converting from Qt
	bool canConvertFromMime(const FORMATETC& formatetc, const QMimeData* mimeData) const override;
	bool convertFromMime(const FORMATETC& formatetc, const QMimeData* mimeData, STGMEDIUM* pmedium) const override;
	QVector<FORMATETC> formatsForMime(const QString& mimeType, const QMimeData* mimeData) const override;

	// for converting to Qt
	bool canConvertToMime(const QString& mimeType, IDataObject* pDataObj) const override;
	QVariant convertToMime(const QString& mime, IDataObject* pDataObj, QVariant::Type preferredType) const override;
	QString mimeForFormat(const FORMATETC& formatetc) const override;

private:
	int CF_SHELLIDL;
};
