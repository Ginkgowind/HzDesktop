#include "HzDesktopParam.h"

#define TEXT_MAX_HEIGHT			40

HzDesktopParam::HzDesktopParam()
	: iconMargin(5, 5)
	, itemSpaceSize(10, 20)
{
	setIconSizeMode(Medium);
}

void HzDesktopParam::setIconSizeMode(IconSizeMode mode)
{
	switch (mode)
	{
	case Small:
		iconSize = { 64, 64 };
		break;
	case Medium:
		iconSize = { 90, 90 };
		break;
	case Large:
	default:
		iconSize = { 108, 108 };
		break;
	}

	updateGridSize();
}

void HzDesktopParam::updateGridSize()
{
	gridSize = iconSize
		+ iconMargin
		+ QSize(0, TEXT_MAX_HEIGHT)
		+ itemSpaceSize;
}
