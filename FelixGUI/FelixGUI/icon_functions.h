#pragma once
#include <QtWidgets/QMainWindow>
#include <Windows.h>

#ifndef ICON_FUNCTIONS_H
#define ICON_FUNCTIONS_H

template <typename Int>
static inline Int pad4(Int);
QPixmap fromHICON(HICON);
void initBitMapInfoHeader(int width, int height, bool topToBottom, 
	DWORD compression, DWORD bitCount, 
	BITMAPINFOHEADER* bih);

template <class BITMAPINFO_T> // BITMAPINFO, BITMAPINFO_COLORTABLE256
static inline void initBitMapInfo(int width, int height, bool topToBottom,
	DWORD compression, DWORD bitCount,
	BITMAPINFO_T* bmi);

static inline uchar* getDiBits(HDC, HBITMAP, int, int, bool);
static inline QImage qt_imageFromWinIconHBITMAP(HDC, HBITMAP, int, int);
static inline bool hasAlpha(const QImage&);

#endif