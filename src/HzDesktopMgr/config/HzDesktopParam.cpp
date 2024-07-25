#include <QPixmapCache>

#include "HzDesktopParam.h"

HzDesktopParam::HzDesktopParam()
	: workMode(GridMode)
	, iconMargin(5, 5)
	, itemSpaceSize(10, 20)
	, bEnableDoubleClick(true)
	, bAutoArrange(false)
	//, bAutoArrange(true)
	, iconMode(MediumIcon)
	, sortRole(Qt::DisplayRole)
	, sortOrder(Qt::AscendingOrder)
	, font("Microsoft YaHei")
{
	// TODO ±à¼­¿òÎÄ±¾ÏÔÊ¾Æ«×ó
	font.setPixelSize(16);

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
	int textAreaHeight = MAX_TEXT_SHOW_LINE * QFontMetrics(font).lineSpacing();

	gridSize = iconSize
		+ 2 * iconMargin
		+ QSize(0, textAreaHeight)
		+ itemSpaceSize;
}

void removePixmapCache(const QString& filePath)
{
	QPixmapCache::remove(QString::number(LargeIcon) + filePath);
	QPixmapCache::remove(QString::number(MediumIcon) + filePath);
	QPixmapCache::remove(QString::number(SmallIcon) + filePath);
}