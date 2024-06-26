#include <QPixmapCache>

#include "HzDesktopParam.h"

#define TEXT_MAX_HEIGHT			40

HzDesktopParam::HzDesktopParam()
	: workMode(GridMode)
	, iconMargin(5, 5)
	, itemSpaceSize(10, 20)
	, bEnableDoubleClick(true)
	, bAutoArrange(false)
	//, bAutoArrange(true)
	, iconMode(MediumIcon)
	, sortRole(FileNameRole)
	, sortOrder(Qt::AscendingOrder)
{
	setIconSizeMode(iconMode);
}

void HzDesktopParam::setIconSizeMode(IconSizeMode mode)
{
	switch (mode)
	{
	case SmallIcon:
		iconSize = { SMALL_ICON_SIZE, SMALL_ICON_SIZE };
		break;
	case MediumIcon:
		iconSize = { MEDIUM_ICON_SIZE, MEDIUM_ICON_SIZE };
		break;
	case LargeIcon:
	default:
		iconSize = { LARGE_ICON_SIZE, LARGE_ICON_SIZE };
		break;
	}

	iconMode = mode;

	updateGridSize();
}

void HzDesktopParam::updateGridSize()
{
	gridSize = iconSize
		+ 2 * iconMargin
		+ QSize(0, TEXT_MAX_HEIGHT)
		+ itemSpaceSize;
}

void removePixmapCache(const QString& filePath)
{
	QPixmapCache::remove(QString::number(LargeIcon) + filePath);
	QPixmapCache::remove(QString::number(MediumIcon) + filePath);
	QPixmapCache::remove(QString::number(SmallIcon) + filePath);
}