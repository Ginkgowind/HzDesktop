#pragma once

#include <QFontMetrics>
#include <QSize>
#include <QDateTime>

#define SECOND_MAX_ICON_SIZE	128
#define MAX_ICON_SIZE			256

#define LARGE_ICON_SIZE			128
#define MEDIUM_ICON_SIZE		100
#define SMALL_ICON_SIZE			64

// �ı���ʾ�������
// TODO ����ȷ��֮�󣬾��Ż�Ϊ����font����height�ĺ�
#define MAX_TEXT_SHOW_LINE 2

enum WorkMode
{
	DesktopMode,
	GridMode,
	FileMappingMode
};

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

//private:
	void updateGridSize();

public:
	WorkMode workMode;

	QString dirPath;

	bool bEnableDoubleClick;

	// ����ʾ��������У��������Զ�����
	bool bAutoArrange;

	bool bShowLnkArrow;

	IconSizeMode iconMode;

	int sortRole;

	Qt::SortOrder sortOrder;

	MenuShowStyle menuShowStyle;

	QSize iconSize;

	QSize iconMargin;

	QSize itemSpaceSize;

	QSize gridSize;

	QFont font;
};

void removePixmapCache(const QString& filePath);