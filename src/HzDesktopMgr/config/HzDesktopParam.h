#pragma once

#include <QSize>

enum MenuShowStyle
{
	HzStyle,
	Win10Style,
	Win11Style
};

enum IconSizeMode
{
	Large,
	Medium,
	Small
};

enum ItemSortMode
{
	None,
	Name,
	Size,
	ItemType,
	DateModified
};

struct HzDesktopParam
{
public:
	//MenuShowStyle setMenuShowStyle();
	HzDesktopParam();

	void setIconSizeMode(IconSizeMode mode);

private:
	void updateGridSize();

public:
	MenuShowStyle menuShowStyle;

	QSize iconSize;

	QSize iconMargin;

	QSize itemSpaceSize;

	QSize gridSize;
};

