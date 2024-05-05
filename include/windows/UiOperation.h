#pragma once

#include <string>
#include <QWidget>
#include <QMimeData>

namespace HZ
{
	bool showContentMenuWin10(
		WId ownerWId,
		const QStringList& pathList,
		int x,
		int y
	);

	QMimeData* multiDrag(const QStringList& pathList);
}