#include "icon_functions.h"

/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

/*
TAKEN FROM QT5.14 SOURCE CODE NOT MY CODE
*/

template <typename Int>
static inline Int pad4(Int v)
{
	return (v + Int(3)) & ~Int(3);
}

void initBitMapInfoHeader(int width, int height, bool topToBottom,
	DWORD compression, DWORD bitCount,
	BITMAPINFOHEADER* bih)
{
	memset(bih, 0, sizeof(BITMAPINFOHEADER));
	bih->biSize = sizeof(BITMAPINFOHEADER);
	bih->biWidth = width;
	bih->biHeight = topToBottom ? -height : height;
	bih->biPlanes = 1;
	bih->biBitCount = WORD(bitCount);
	bih->biCompression = compression;
	// scan lines are word-aligned (unless RLE)
	const DWORD bytesPerLine = pad4(DWORD(width) * bitCount / 8);
	bih->biSizeImage = bytesPerLine * DWORD(height);
}

template <class BITMAPINFO_T> // BITMAPINFO, BITMAPINFO_COLORTABLE256
static inline void initBitMapInfo(int width, int height, bool topToBottom,
	DWORD compression, DWORD bitCount,
	BITMAPINFO_T* bmi)
{
	initBitMapInfoHeader(width, height, topToBottom, compression, bitCount, &bmi->bmiHeader);
	memset(bmi->bmiColors, 0, sizeof(bmi->bmiColors));
}

static inline uchar* getDiBits(HDC hdc, HBITMAP bitmap, int width, int height, bool topToBottom = true)
{
	BITMAPINFO bmi;
	initBitMapInfo(width, height, topToBottom, BI_RGB, 32u, &bmi);
	uchar* result = new uchar[bmi.bmiHeader.biSizeImage];
	if (!GetDIBits(hdc, bitmap, 0, UINT(height), result, &bmi, DIB_RGB_COLORS)) {
		delete[] result;
		qErrnoWarning("%s: GetDIBits() failed to get bitmap bits.", __FUNCTION__);
		return nullptr;
	}
	return result;
}

static inline QImage qt_imageFromWinIconHBITMAP(HDC hdc, HBITMAP bitmap, int w, int h)
{
	QImage image(w, h, QImage::Format_ARGB32_Premultiplied);
	if (image.isNull())
		return image;
	QScopedArrayPointer<uchar> data(getDiBits(hdc, bitmap, w, h, true));
	if (data.isNull())
		return QImage();
	memcpy(image.bits(), data.data(), size_t(image.byteCount()));
	return image;
}

static inline bool hasAlpha(const QImage& image)
{
	const int w = image.width();
	const int h = image.height();
	for (int y = 0; y < h; ++y) {
		const QRgb* scanLine = reinterpret_cast<const QRgb*>(image.scanLine(y));
		for (int x = 0; x < w; ++x) {
			if (qAlpha(scanLine[x]) != 0)
				return true;
		}
	}
	return false;
}

QPixmap fromHICON(HICON icon) {
	HDC screenDevice = GetDC(nullptr);
	HDC hdc = CreateCompatibleDC(screenDevice);
	ReleaseDC(nullptr, screenDevice);
	ICONINFO iconinfo;
	const bool result = GetIconInfo(icon, &iconinfo); //x and y Hotspot describes the icon center
	if (!result) {
		qErrnoWarning("QPixmap::fromWinHICON(), failed to GetIconInfo()");
		DeleteDC(hdc);
		return QPixmap();
	}
	const int w = int(iconinfo.xHotspot) * 2;
	const int h = int(iconinfo.yHotspot) * 2;
	BITMAPINFOHEADER bitmapInfo;
	initBitMapInfoHeader(w, h, false, BI_RGB, 32u, &bitmapInfo);
	DWORD* bits;
	HBITMAP winBitmap = CreateDIBSection(hdc, reinterpret_cast<BITMAPINFO*>(&bitmapInfo),
		DIB_RGB_COLORS, reinterpret_cast<VOID**>(&bits),
		nullptr, 0);
	HGDIOBJ oldhdc = static_cast<HBITMAP>(SelectObject(hdc, winBitmap));
	DrawIconEx(hdc, 0, 0, icon, w, h, 0, nullptr, DI_NORMAL);
	QImage image = qt_imageFromWinIconHBITMAP(hdc, winBitmap, w, h);
	if (!image.isNull() && !hasAlpha(image)) { //If no alpha was found, we use the mask to set alpha values
		DrawIconEx(hdc, 0, 0, icon, w, h, 0, nullptr, DI_MASK);
		const QImage mask = qt_imageFromWinIconHBITMAP(hdc, winBitmap, w, h);
		for (int y = 0; y < h; y++) {
			QRgb* scanlineImage = reinterpret_cast<QRgb*>(image.scanLine(y));
			const QRgb* scanlineMask = mask.isNull() ? nullptr : reinterpret_cast<const QRgb*>(mask.scanLine(y));
			for (int x = 0; x < w; x++) {
				if (scanlineMask && qRed(scanlineMask[x]) != 0)
					scanlineImage[x] = 0; //mask out this pixel
				else
					scanlineImage[x] |= 0xff000000; // set the alpha channel to 255
			}
		}
	}
	//dispose resources created by iconinfo call
	DeleteObject(iconinfo.hbmMask);
	DeleteObject(iconinfo.hbmColor);
	SelectObject(hdc, oldhdc); //restore state
	DeleteObject(winBitmap);
	DeleteDC(hdc);
	return QPixmap::fromImage(std::move(image));
}