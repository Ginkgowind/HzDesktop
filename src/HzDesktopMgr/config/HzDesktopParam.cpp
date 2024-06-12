#include <QPixmapCache>

#include "HzDesktopParam.h"

#define TEXT_MAX_HEIGHT			40

HzDesktopParam::HzDesktopParam()
	: workMode(GridMode)
	, iconMargin(5, 5)
	, itemSpaceSize(10, 20)
	, bEnableDoubleClick(true)
	//, bAutoArrange(false)
	, bAutoArrange(true)
	, iconMode(MediumIcon)
	, sortRole(FileNameRole)
	, sortOrder(Qt::DescendingOrder)
{
	setIconSizeMode(iconMode);
}

void HzDesktopParam::setIconSizeMode(IconSizeMode mode)
{
	switch (mode)
	{
	case SmallIcon:
		iconSize = { 64, 64 };
		break;
	case MediumIcon:
		iconSize = { 90, 90 };
		break;
	case LargeIcon:
	default:
		iconSize = { 108, 108 };
		break;
	}

	iconMode = mode;

	updateGridSize();
}

void HzDesktopParam::updateGridSize()
{
	gridSize = iconSize
		+ iconMargin
		+ QSize(0, TEXT_MAX_HEIGHT)
		+ itemSpaceSize;
}

void removePixmapCache(const QString& filePath)
{
	QPixmapCache::remove(QString::number(LargeIcon) + filePath);
	QPixmapCache::remove(QString::number(MediumIcon) + filePath);
	QPixmapCache::remove(QString::number(SmallIcon) + filePath);
}