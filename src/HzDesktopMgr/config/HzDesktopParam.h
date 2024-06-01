#pragma once

#include <QSize>
#include <QDateTime>

enum MenuShowStyle
{
	HzStyle,
	Win10Style,
	Win11Style
};

enum IconSizeMode
{
	LargeIcon,
	MediumIcon,
	SmallIcon
};

enum CustomRoles {
	FilePathRole = Qt::UserRole + 1,
	FileNameRole,
	FileSizeRole,
	FileTypeRole,
	FileLastModifiedRole
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
	bool bEnableDoubleClick;

	bool bAutoArrange;

	IconSizeMode iconMode;

	CustomRoles sortRole;

	Qt::SortOrder sortOrder;

	MenuShowStyle menuShowStyle;

	QSize iconSize;

	QSize iconMargin;

	QSize itemSpaceSize;

	QSize gridSize;
};

