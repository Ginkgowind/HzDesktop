// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2023 - TortoiseGit
// Copyright (C) 2009, 2011, 2014 - TortoiseSVN

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
#pragma once
#include <windows.h>
#include <CommCtrl.h>
#include <Uxtheme.h>
#pragma warning(push)
#pragma warning(disable: 4458)
#include <GdiPlus.h>
#pragma warning(pop)

#include <map>

using FN_LoadIconWithScaleDown = HRESULT(WINAPI*)(HINSTANCE hinst, PCWSTR pszName, int cx, int cy, HICON* phico);

/**
 * \ingroup utils
 * provides helper functions for converting icons to bitmaps
 */
class IconBitmapUtils
{
public:
	IconBitmapUtils();
	~IconBitmapUtils();

	HBITMAP LoadAndResizeFirstIcon(HINSTANCE hInst, UINT uIcon, int width, int height);

	HBITMAP IconToBitmapPARGB32(HINSTANCE hInst, UINT uIcon, int width, int height);
	
	HBITMAP IconToBitmapPARGB32(HICON hIcon, int width, int height);
	
	HRESULT Create32BitHBITMAP(HDC hdc, const SIZE *psize, __deref_opt_out void **ppvBits, __out HBITMAP* phBmp) const;
	
	HRESULT ConvertBufferToPARGB32(HPAINTBUFFER hPaintBuffer, HDC hdc, HICON hicon, SIZE& sizIcon);
	
	bool HasAlpha(__in Gdiplus::ARGB* pargb, const SIZE& sizImage, int cxRow) const;
	
	HRESULT ConvertToPARGB32(HDC hdc, __inout Gdiplus::ARGB* pargb, HBITMAP hbmp, const SIZE& sizImage, int cxRow) const;

private:
	FN_LoadIconWithScaleDown _GetLoadIconWithScaleDownPtr();

private:
	std::map<UINT, HBITMAP>     bitmaps;
};
