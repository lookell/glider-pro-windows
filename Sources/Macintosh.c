#define GP_USE_WINAPI_H

#include "Macintosh.h"

#include "StringUtils.h"


//--------------------------------------------------------------  CopyBits
// Copy some portion of a bitmap from one graphics port to another.

void Mac_CopyBits(
	HDC srcBits,
	HDC dstBits,
	const Rect *srcRect,
	const Rect *dstRect,
	SInt16 mode,
	HRGN maskRgn)
{
	INT xSrc, ySrc, wSrc, hSrc;
	INT xDst, yDst, wDst, hDst;
	COLORREF wasBkColor, wasTextColor;

	if (srcRect->left >= srcRect->right || srcRect->top >= srcRect->bottom)
		return;
	if (dstRect->left >= dstRect->right || dstRect->top >= dstRect->bottom)
		return;
	if (mode != srcCopy && mode != srcXor && mode != transparent)
		return;

	xSrc = srcRect->left;
	ySrc = srcRect->top;
	wSrc = srcRect->right - srcRect->left;
	hSrc = srcRect->bottom - srcRect->top;
	xDst = dstRect->left;
	yDst = dstRect->top;
	wDst = dstRect->right - dstRect->left;
	hDst = dstRect->bottom - dstRect->top;

	if (maskRgn != NULL)
	{
		SaveDC(dstBits);
		ExtSelectClipRgn(dstBits, maskRgn, RGN_AND);
	}
	switch (mode)
	{
		case srcCopy:
		StretchBlt(dstBits, xDst, yDst, wDst, hDst,
				srcBits, xSrc, ySrc, wSrc, hSrc, SRCCOPY);
		break;

		case srcXor:
		// invert the destination where the source is black
		// and keep the destination where the source is white
		wasBkColor = SetBkColor(dstBits, RGB(0xFF, 0xFF, 0xFF));
		wasTextColor = SetTextColor(dstBits, RGB(0x00, 0x00, 0x00));
		StretchBlt(dstBits, xDst, yDst, wDst, hDst,
				srcBits, xSrc, ySrc, wSrc, hSrc, 0x00990066); // DSxn
		SetTextColor(dstBits, wasTextColor);
		SetBkColor(dstBits, wasBkColor);
		break;

		case transparent:
		TransparentBlt(dstBits, xDst, yDst, wDst, hDst,
				srcBits, xSrc, ySrc, wSrc, hSrc, GetBkColor(dstBits));
		break;
	}
	if (maskRgn != NULL)
	{
		RestoreDC(dstBits, -1);
	}
}

//--------------------------------------------------------------  CopyMask
// Copy some portion of a bitmap from one graphics port to another,
// with a mask to specify how much of each pixel is copied over.
//
// Black pixels in the mask indicate where the source is copied to the
// destination, and white pixels indicate where the destination is left
// alone.
//
// If the mask is not monochrome, then each output pixel's color component
// is a combination of the matching components from the source, mask, and
// destination. If each RGB component ranges from 0.0 to 1.0, then the
// expression to calculate the output component value is
//
//     (1.0 - mask) * source + (mask) * destination
//
// This is similar to alpha blending onto an opaque background, where the
// colors are represented with straight alpha (not premultiplied). That
// blending is calculated with this expression:
//
//     (alpha) * source + (1.0 - alpha) * destination
//
// The difference is that the alpha is just equal to (1.0 - mask).
//
// This implementation does NOT support utilizing any existing alpha channel
// in the source or destination; the mask represents the source's alpha
// channel, and the destination is treated as an opaque image.

static BYTE BlendMask(BYTE src, BYTE mask, BYTE dst)
{
	WORD alpha = 255 - ((WORD)mask);
	WORD weightedSrc = (alpha) * ((WORD)src);
	WORD weightedDst = (255 - alpha) * ((WORD)dst);
	return (BYTE)((weightedSrc + weightedDst) / 255);
}

void Mac_CopyMask(
	HDC srcBits,
	HDC maskBits,
	HDC dstBits,
	const Rect *srcRect,
	const Rect *maskRect,
	const Rect *dstRect)
{
	BITMAPINFO bmInfo;
	HBITMAP srcDIB;
	HBITMAP maskDIB;
	HBITMAP dstDIB;
	HBITMAP hbmPrev;
	RGBQUAD *srcPtr;
	RGBQUAD *maskPtr;
	RGBQUAD *dstPtr;
	HDC tmpDC;
	DWORD pixelIndex;
	DWORD pixelCount;

	int xSrc = srcRect->left;
	int ySrc = srcRect->top;
	int wSrc = srcRect->right - srcRect->left;
	int hSrc = srcRect->bottom - srcRect->top;
	int xMask = maskRect->left;
	int yMask = maskRect->top;
	int wMask = maskRect->right - maskRect->left;
	int hMask = maskRect->bottom - maskRect->top;
	int xDst = dstRect->left;
	int yDst = dstRect->top;
	int wDst = dstRect->right - dstRect->left;
	int hDst = dstRect->bottom - dstRect->top;
	int wOut = wDst;
	int hOut = hDst;

	if ((wSrc <= 0 || hSrc <= 0) ||
		(wMask <= 0 || hMask <= 0) ||
		(wDst <= 0 || hDst <= 0))
	{
		return;
	}

	// Create 32-bit DIB sections to hold RGBQUAD versions of the
	// source, mask, and destination images

	ZeroMemory(&bmInfo, sizeof(bmInfo));
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = wOut;
	bmInfo.bmiHeader.biHeight = hOut;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = 32;
	bmInfo.bmiHeader.biCompression = BI_RGB;
	bmInfo.bmiHeader.biSizeImage = 0;
	bmInfo.bmiHeader.biXPelsPerMeter = 0;
	bmInfo.bmiHeader.biYPelsPerMeter = 0;
	bmInfo.bmiHeader.biClrUsed = 0;
	bmInfo.bmiHeader.biClrImportant = 0;

	srcDIB = CreateDIBSection(NULL, &bmInfo, DIB_RGB_COLORS, (void **)&srcPtr, NULL, 0);
	if (srcDIB == NULL)
	{
		return;
	}
	maskDIB = CreateDIBSection(NULL, &bmInfo, DIB_RGB_COLORS, (void **)&maskPtr, NULL, 0);
	if (maskDIB == NULL)
	{
		DeleteObject(srcDIB);
		return;
	}
	dstDIB = CreateDIBSection(NULL, &bmInfo, DIB_RGB_COLORS, (void **)&dstPtr, NULL, 0);
	if (dstDIB == NULL)
	{
		DeleteObject(maskDIB);
		DeleteObject(srcDIB);
		return;
	}
	tmpDC = CreateCompatibleDC(NULL);
	if (tmpDC == NULL)
	{
		DeleteObject(dstDIB);
		DeleteObject(maskDIB);
		DeleteObject(srcDIB);
		return;
	}

	hbmPrev = (HBITMAP)SelectObject(tmpDC, srcDIB);
	StretchBlt(tmpDC, 0, 0, wOut, hOut, srcBits, xSrc, ySrc, wSrc, hSrc, SRCCOPY);
	SelectObject(tmpDC, hbmPrev);

	hbmPrev = (HBITMAP)SelectObject(tmpDC, maskDIB);
	StretchBlt(tmpDC, 0, 0, wOut, hOut, maskBits, xMask, yMask, wMask, hMask, SRCCOPY);
	SelectObject(tmpDC, hbmPrev);

	hbmPrev = (HBITMAP)SelectObject(tmpDC, dstDIB);
	StretchBlt(tmpDC, 0, 0, wOut, hOut, dstBits, xDst, yDst, wDst, hDst, SRCCOPY);
	SelectObject(tmpDC, hbmPrev);

	// Perform the actual mask blending operation

	GdiFlush();
	pixelCount = (DWORD)wOut * (DWORD)hOut;
	for (pixelIndex = 0; pixelIndex < pixelCount; pixelIndex++)
	{
		RGBQUAD srcRGB = srcPtr[pixelIndex];
		RGBQUAD maskRGB = maskPtr[pixelIndex];
		RGBQUAD dstRGB = dstPtr[pixelIndex];
		RGBQUAD outRGB = dstRGB;
		outRGB.rgbBlue = BlendMask(srcRGB.rgbBlue, maskRGB.rgbBlue, dstRGB.rgbBlue);
		outRGB.rgbGreen = BlendMask(srcRGB.rgbGreen, maskRGB.rgbGreen, dstRGB.rgbGreen);
		outRGB.rgbRed = BlendMask(srcRGB.rgbRed, maskRGB.rgbRed, dstRGB.rgbRed);
		dstPtr[pixelIndex] = outRGB;
	}
	GdiFlush();

	// Blit the output image to its final destination, and clean up

	hbmPrev = (HBITMAP)SelectObject(tmpDC, dstDIB);
	StretchBlt(dstBits, xDst, yDst, wDst, hDst, tmpDC, 0, 0, wOut, hOut, SRCCOPY);
	SelectObject(tmpDC, hbmPrev);

	DeleteDC(tmpDC);
	DeleteObject(dstDIB);
	DeleteObject(maskDIB);
	DeleteObject(srcDIB);
}

//--------------------------------------------------------------  DrawPicture
// Draw the given bitmap into the destination output device. The bitmap
// is stretched or shrinked as necessary to fit exactly with the given
// destination rectangle.

void Mac_DrawPicture(HDC hdcDst, HBITMAP myPicture, const Rect *dstRect)
{
	HDC hdcSrc;
	HGDIOBJ prevBitmap;
	BITMAP bmInfo;
	INT xSrc, ySrc, wSrc, hSrc;
	INT xDst, yDst, wDst, hDst;

	if (dstRect->left >= dstRect->right || dstRect->top >= dstRect->bottom)
		return;
	if (GetObject(myPicture, sizeof(bmInfo), &bmInfo) != sizeof(bmInfo))
		return;

	xSrc = 0;
	ySrc = 0;
	wSrc = bmInfo.bmWidth;
	hSrc = bmInfo.bmHeight;
	xDst = dstRect->left;
	yDst = dstRect->top;
	wDst = dstRect->right - dstRect->left;
	hDst = dstRect->bottom - dstRect->top;

	hdcSrc = CreateCompatibleDC(NULL);
	prevBitmap = SelectObject(hdcSrc, myPicture);
	StretchBlt(hdcDst, xDst, yDst, wDst, hDst,
		hdcSrc, xSrc, ySrc, wSrc, hSrc, SRCCOPY);
	SelectObject(hdcSrc, prevBitmap);
	DeleteDC(hdcSrc);
}

//--------------------------------------------------------------  DrawString
// Draw the given character string, starting from the current position
// and using the current settings. The reference point of the text is
// the left edge, at the baseline (TA_LEFT | TA_BASELINE in GDI).
// The current position is updated by this function to the right edge
// of the text, at the baseline (TA_UPDATECP in GDI).

void Mac_DrawString(HDC hdc, ConstStringPtr s)
{
	WCHAR buffer[256];
	INT prevBkMode;
	UINT prevTextAlign;

	WinFromMacString(buffer, ARRAYSIZE(buffer), s);
	prevBkMode = SetBkMode(hdc, TRANSPARENT);
	prevTextAlign = SetTextAlign(hdc, TA_LEFT | TA_BASELINE | TA_UPDATECP);
	// The MacRoman string's length byte is used here, because each
	// MacRoman byte corresponds to only one UTF-16 code unit.
	TextOut(hdc, 0, 0, buffer, s[0]);
	SetTextAlign(hdc, prevTextAlign);
	SetBkMode(hdc, prevBkMode);
}

//--------------------------------------------------------------  FrameRect
// Draw a frame within the specified rectangle using the current brush.
// The vertical strokes use the specified width, and the horizontal
// strokes use the specified height.

void Mac_FrameRect(HDC hdc, const Rect *r, HBRUSH hbr, SInt16 w, SInt16 h)
{
	HRGN theRgn;

	if (r->left >= r->right || r->top >= r->bottom)
		return;

	theRgn = CreateRectRgn(r->left, r->top, r->right, r->bottom);
	FrameRgn(hdc, theRgn, hbr, w, h);
	DeleteObject(theRgn);
}

//--------------------------------------------------------------  GetDateTime
// Retrieve the number of seconds since midnight, January 1, 1904.
// The time difference is in terms of the local time zone.

void Mac_GetDateTime(UInt32 *secs)
{
	SYSTEMTIME localMacEpoch, utcMacEpoch;
	FILETIME epochTime, currentTime;
	ULARGE_INTEGER epochSecs, currentSecs;

	if (secs == NULL)
		return;
	*secs = 0;

	localMacEpoch.wYear = 1904;
	localMacEpoch.wMonth = 1;
	localMacEpoch.wDayOfWeek = 5; // Friday
	localMacEpoch.wDay = 1;
	localMacEpoch.wHour = 0;
	localMacEpoch.wMinute = 0;
	localMacEpoch.wSecond = 0;
	localMacEpoch.wMilliseconds = 0;
	if (!TzSpecificLocalTimeToSystemTime(NULL, &localMacEpoch, &utcMacEpoch))
		return;

	if (!SystemTimeToFileTime(&utcMacEpoch, &epochTime))
		return;
	epochSecs.LowPart = epochTime.dwLowDateTime;
	epochSecs.HighPart = epochTime.dwHighDateTime;
	epochSecs.QuadPart /= (10 * 1000 * 1000);

	GetSystemTimeAsFileTime(&currentTime);
	currentSecs.LowPart = currentTime.dwLowDateTime;
	currentSecs.HighPart = currentTime.dwHighDateTime;
	currentSecs.QuadPart /= (10 * 1000 * 1000);

	*secs = (UInt32)(currentSecs.QuadPart - epochSecs.QuadPart);
}

void Mac_InvalWindowRect(HWND window, const Rect *bounds)
{
	RECT rcDirty;

	rcDirty.left = bounds->left;
	rcDirty.top = bounds->top;
	rcDirty.right = bounds->right;
	rcDirty.bottom = bounds->bottom;
	InvalidateRect(window, &rcDirty, TRUE);
}

//--------------------------------------------------------------  Line
// Draw a line from the current position to the specified offsets.
// 'dh' specifies the horizontal movement and 'dv' specifies the
// vertical movement of the current pen. Negative values move the
// pen left or up, and positive values move the pen right or down.
// NOTE: Macintosh lines include the end pixel, unlike GDI lines.

void Mac_Line(HDC hdc, SInt16 dh, SInt16 dv)
{
	POINT curPos;

	GetCurrentPositionEx(hdc, &curPos);
	LineTo(hdc, curPos.x + dh, curPos.y + dv);
	LineTo(hdc, curPos.x, curPos.y);
	LineTo(hdc, curPos.x + dh, curPos.y + dv);
}

//--------------------------------------------------------------  LineTo
// Draw a line from the current position to the specified position.
// 'h' and 'v' specify the coordinates of the line's end point.
// NOTE: Macintosh lines include the end pixel, unlike GDI lines.

void Mac_LineTo(HDC hdc, SInt16 h, SInt16 v)
{
	POINT curPos;

	GetCurrentPositionEx(hdc, &curPos);
	LineTo(hdc, h, v);
	LineTo(hdc, curPos.x, curPos.y);
	LineTo(hdc, h, v);
}

//--------------------------------------------------------------  PaintRect
// Fill the specified rectangle with the current brush.

void Mac_PaintRect(HDC hdc, const Rect *r, HBRUSH hbr)
{
	HRGN theRgn;

	if (r->left >= r->right || r->top >= r->bottom)
		return;

	theRgn = CreateRectRgn(r->left, r->top, r->right, r->bottom);
	FillRgn(hdc, theRgn, hbr);
	DeleteObject(theRgn);
}

//--------------------------------------------------------------  StringWidth
// Calculate the width, in logical units, of the given Pascal string,
// using the currently selected font.

SInt16 Mac_StringWidth(HDC hdc, ConstStringPtr s)
{
	WCHAR buffer[256];
	SIZE extents;

	WinFromMacString(buffer, ARRAYSIZE(buffer), s);
	if (!GetTextExtentPoint32(hdc, buffer, (int)wcslen(buffer), &extents))
		return 0;
	return (SInt16)extents.cx;
}

//--------------------------------------------------------------  Global Data

#define QD_RGB(r, g, b) RGB(((r) >> 8) & 0xFF, ((g) >> 8) & 0xFF, ((b) >> 8) & 0xFF)

// The eight basic QuickDraw colors
const COLORREF blackColor = QD_RGB(0x0000, 0x0000, 0x0000);
const COLORREF yellowColor = QD_RGB(0xFC00, 0xF37D, 0x052F);
const COLORREF magentaColor = QD_RGB(0xF2D7, 0x0856, 0x84EC);
const COLORREF redColor = QD_RGB(0xDD6B, 0x08C2, 0x06A2);
const COLORREF cyanColor = QD_RGB(0x0241, 0xAB54, 0xEAFF);
const COLORREF greenColor = QD_RGB(0x0000, 0x8000, 0x11B0);
const COLORREF blueColor = QD_RGB(0x0000, 0x0000, 0xD400);
const COLORREF whiteColor = QD_RGB(0xFFFF, 0xFFFF, 0xFFFF);

// The default 4-bit Macintosh color palette
const COLORREF MacColor4[] = {
	QD_RGB(0xFFFF, 0xFFFF, 0xFFFF), // white
	QD_RGB(0xFC00, 0xF37D, 0x052F), // yellow
	QD_RGB(0xFFFF, 0x648A, 0x028C), // orange
	QD_RGB(0xDD6B, 0x08C2, 0x06A2), // red
	QD_RGB(0xF2D7, 0x0856, 0x84EC), // magenta
	QD_RGB(0x46E3, 0x0000, 0xA53E), // purple
	QD_RGB(0x0000, 0x0000, 0xD400), // blue
	QD_RGB(0x0241, 0xAB54, 0xEAFF), // cyan
	QD_RGB(0x1F21, 0xB793, 0x1431), // green
	QD_RGB(0x0000, 0x64AF, 0x11B0), // dark green
	QD_RGB(0x5600, 0x2C9D, 0x0524), // brown
	QD_RGB(0x90D7, 0x7160, 0x3A34), // tan
	QD_RGB(0xC000, 0xC000, 0xC000), // light gray
	QD_RGB(0x8000, 0x8000, 0x8000), // medium gray
	QD_RGB(0x4000, 0x4000, 0x4000), // dark gray
	QD_RGB(0x0000, 0x0000, 0x0000), // black
};

// The default 8-bit Macintosh color palette
const COLORREF MacColor8[] = {
	QD_RGB(0xFFFF, 0xFFFF, 0xFFFF), QD_RGB(0xFFFF, 0xFFFF, 0xCCCC), QD_RGB(0xFFFF, 0xFFFF, 0x9999),
	QD_RGB(0xFFFF, 0xFFFF, 0x6666), QD_RGB(0xFFFF, 0xFFFF, 0x3333), QD_RGB(0xFFFF, 0xFFFF, 0x0000),
	QD_RGB(0xFFFF, 0xCCCC, 0xFFFF), QD_RGB(0xFFFF, 0xCCCC, 0xCCCC), QD_RGB(0xFFFF, 0xCCCC, 0x9999),
	QD_RGB(0xFFFF, 0xCCCC, 0x6666), QD_RGB(0xFFFF, 0xCCCC, 0x3333), QD_RGB(0xFFFF, 0xCCCC, 0x0000),
	QD_RGB(0xFFFF, 0x9999, 0xFFFF), QD_RGB(0xFFFF, 0x9999, 0xCCCC), QD_RGB(0xFFFF, 0x9999, 0x9999),
	QD_RGB(0xFFFF, 0x9999, 0x6666), QD_RGB(0xFFFF, 0x9999, 0x3333), QD_RGB(0xFFFF, 0x9999, 0x0000),
	QD_RGB(0xFFFF, 0x6666, 0xFFFF), QD_RGB(0xFFFF, 0x6666, 0xCCCC), QD_RGB(0xFFFF, 0x6666, 0x9999),
	QD_RGB(0xFFFF, 0x6666, 0x6666), QD_RGB(0xFFFF, 0x6666, 0x3333), QD_RGB(0xFFFF, 0x6666, 0x0000),
	QD_RGB(0xFFFF, 0x3333, 0xFFFF), QD_RGB(0xFFFF, 0x3333, 0xCCCC), QD_RGB(0xFFFF, 0x3333, 0x9999),
	QD_RGB(0xFFFF, 0x3333, 0x6666), QD_RGB(0xFFFF, 0x3333, 0x3333), QD_RGB(0xFFFF, 0x3333, 0x0000),
	QD_RGB(0xFFFF, 0x0000, 0xFFFF), QD_RGB(0xFFFF, 0x0000, 0xCCCC), QD_RGB(0xFFFF, 0x0000, 0x9999),
	QD_RGB(0xFFFF, 0x0000, 0x6666), QD_RGB(0xFFFF, 0x0000, 0x3333), QD_RGB(0xFFFF, 0x0000, 0x0000),

	QD_RGB(0xCCCC, 0xFFFF, 0xFFFF), QD_RGB(0xCCCC, 0xFFFF, 0xCCCC), QD_RGB(0xCCCC, 0xFFFF, 0x9999),
	QD_RGB(0xCCCC, 0xFFFF, 0x6666), QD_RGB(0xCCCC, 0xFFFF, 0x3333), QD_RGB(0xCCCC, 0xFFFF, 0x0000),
	QD_RGB(0xCCCC, 0xCCCC, 0xFFFF), QD_RGB(0xCCCC, 0xCCCC, 0xCCCC), QD_RGB(0xCCCC, 0xCCCC, 0x9999),
	QD_RGB(0xCCCC, 0xCCCC, 0x6666), QD_RGB(0xCCCC, 0xCCCC, 0x3333), QD_RGB(0xCCCC, 0xCCCC, 0x0000),
	QD_RGB(0xCCCC, 0x9999, 0xFFFF), QD_RGB(0xCCCC, 0x9999, 0xCCCC), QD_RGB(0xCCCC, 0x9999, 0x9999),
	QD_RGB(0xCCCC, 0x9999, 0x6666), QD_RGB(0xCCCC, 0x9999, 0x3333), QD_RGB(0xCCCC, 0x9999, 0x0000),
	QD_RGB(0xCCCC, 0x6666, 0xFFFF), QD_RGB(0xCCCC, 0x6666, 0xCCCC), QD_RGB(0xCCCC, 0x6666, 0x9999),
	QD_RGB(0xCCCC, 0x6666, 0x6666), QD_RGB(0xCCCC, 0x6666, 0x3333), QD_RGB(0xCCCC, 0x6666, 0x0000),
	QD_RGB(0xCCCC, 0x3333, 0xFFFF), QD_RGB(0xCCCC, 0x3333, 0xCCCC), QD_RGB(0xCCCC, 0x3333, 0x9999),
	QD_RGB(0xCCCC, 0x3333, 0x6666), QD_RGB(0xCCCC, 0x3333, 0x3333), QD_RGB(0xCCCC, 0x3333, 0x0000),
	QD_RGB(0xCCCC, 0x0000, 0xFFFF), QD_RGB(0xCCCC, 0x0000, 0xCCCC), QD_RGB(0xCCCC, 0x0000, 0x9999),
	QD_RGB(0xCCCC, 0x0000, 0x6666), QD_RGB(0xCCCC, 0x0000, 0x3333), QD_RGB(0xCCCC, 0x0000, 0x0000),

	QD_RGB(0x9999, 0xFFFF, 0xFFFF), QD_RGB(0x9999, 0xFFFF, 0xCCCC), QD_RGB(0x9999, 0xFFFF, 0x9999),
	QD_RGB(0x9999, 0xFFFF, 0x6666), QD_RGB(0x9999, 0xFFFF, 0x3333), QD_RGB(0x9999, 0xFFFF, 0x0000),
	QD_RGB(0x9999, 0xCCCC, 0xFFFF), QD_RGB(0x9999, 0xCCCC, 0xCCCC), QD_RGB(0x9999, 0xCCCC, 0x9999),
	QD_RGB(0x9999, 0xCCCC, 0x6666), QD_RGB(0x9999, 0xCCCC, 0x3333), QD_RGB(0x9999, 0xCCCC, 0x0000),
	QD_RGB(0x9999, 0x9999, 0xFFFF), QD_RGB(0x9999, 0x9999, 0xCCCC), QD_RGB(0x9999, 0x9999, 0x9999),
	QD_RGB(0x9999, 0x9999, 0x6666), QD_RGB(0x9999, 0x9999, 0x3333), QD_RGB(0x9999, 0x9999, 0x0000),
	QD_RGB(0x9999, 0x6666, 0xFFFF), QD_RGB(0x9999, 0x6666, 0xCCCC), QD_RGB(0x9999, 0x6666, 0x9999),
	QD_RGB(0x9999, 0x6666, 0x6666), QD_RGB(0x9999, 0x6666, 0x3333), QD_RGB(0x9999, 0x6666, 0x0000),
	QD_RGB(0x9999, 0x3333, 0xFFFF), QD_RGB(0x9999, 0x3333, 0xCCCC), QD_RGB(0x9999, 0x3333, 0x9999),
	QD_RGB(0x9999, 0x3333, 0x6666), QD_RGB(0x9999, 0x3333, 0x3333), QD_RGB(0x9999, 0x3333, 0x0000),
	QD_RGB(0x9999, 0x0000, 0xFFFF), QD_RGB(0x9999, 0x0000, 0xCCCC), QD_RGB(0x9999, 0x0000, 0x9999),
	QD_RGB(0x9999, 0x0000, 0x6666), QD_RGB(0x9999, 0x0000, 0x3333), QD_RGB(0x9999, 0x0000, 0x0000),

	QD_RGB(0x6666, 0xFFFF, 0xFFFF), QD_RGB(0x6666, 0xFFFF, 0xCCCC), QD_RGB(0x6666, 0xFFFF, 0x9999),
	QD_RGB(0x6666, 0xFFFF, 0x6666), QD_RGB(0x6666, 0xFFFF, 0x3333), QD_RGB(0x6666, 0xFFFF, 0x0000),
	QD_RGB(0x6666, 0xCCCC, 0xFFFF), QD_RGB(0x6666, 0xCCCC, 0xCCCC), QD_RGB(0x6666, 0xCCCC, 0x9999),
	QD_RGB(0x6666, 0xCCCC, 0x6666), QD_RGB(0x6666, 0xCCCC, 0x3333), QD_RGB(0x6666, 0xCCCC, 0x0000),
	QD_RGB(0x6666, 0x9999, 0xFFFF), QD_RGB(0x6666, 0x9999, 0xCCCC), QD_RGB(0x6666, 0x9999, 0x9999),
	QD_RGB(0x6666, 0x9999, 0x6666), QD_RGB(0x6666, 0x9999, 0x3333), QD_RGB(0x6666, 0x9999, 0x0000),
	QD_RGB(0x6666, 0x6666, 0xFFFF), QD_RGB(0x6666, 0x6666, 0xCCCC), QD_RGB(0x6666, 0x6666, 0x9999),
	QD_RGB(0x6666, 0x6666, 0x6666), QD_RGB(0x6666, 0x6666, 0x3333), QD_RGB(0x6666, 0x6666, 0x0000),
	QD_RGB(0x6666, 0x3333, 0xFFFF), QD_RGB(0x6666, 0x3333, 0xCCCC), QD_RGB(0x6666, 0x3333, 0x9999),
	QD_RGB(0x6666, 0x3333, 0x6666), QD_RGB(0x6666, 0x3333, 0x3333), QD_RGB(0x6666, 0x3333, 0x0000),
	QD_RGB(0x6666, 0x0000, 0xFFFF), QD_RGB(0x6666, 0x0000, 0xCCCC), QD_RGB(0x6666, 0x0000, 0x9999),
	QD_RGB(0x6666, 0x0000, 0x6666), QD_RGB(0x6666, 0x0000, 0x3333), QD_RGB(0x6666, 0x0000, 0x0000),

	QD_RGB(0x3333, 0xFFFF, 0xFFFF), QD_RGB(0x3333, 0xFFFF, 0xCCCC), QD_RGB(0x3333, 0xFFFF, 0x9999),
	QD_RGB(0x3333, 0xFFFF, 0x6666), QD_RGB(0x3333, 0xFFFF, 0x3333), QD_RGB(0x3333, 0xFFFF, 0x0000),
	QD_RGB(0x3333, 0xCCCC, 0xFFFF), QD_RGB(0x3333, 0xCCCC, 0xCCCC), QD_RGB(0x3333, 0xCCCC, 0x9999),
	QD_RGB(0x3333, 0xCCCC, 0x6666), QD_RGB(0x3333, 0xCCCC, 0x3333), QD_RGB(0x3333, 0xCCCC, 0x0000),
	QD_RGB(0x3333, 0x9999, 0xFFFF), QD_RGB(0x3333, 0x9999, 0xCCCC), QD_RGB(0x3333, 0x9999, 0x9999),
	QD_RGB(0x3333, 0x9999, 0x6666), QD_RGB(0x3333, 0x9999, 0x3333), QD_RGB(0x3333, 0x9999, 0x0000),
	QD_RGB(0x3333, 0x6666, 0xFFFF), QD_RGB(0x3333, 0x6666, 0xCCCC), QD_RGB(0x3333, 0x6666, 0x9999),
	QD_RGB(0x3333, 0x6666, 0x6666), QD_RGB(0x3333, 0x6666, 0x3333), QD_RGB(0x3333, 0x6666, 0x0000),
	QD_RGB(0x3333, 0x3333, 0xFFFF), QD_RGB(0x3333, 0x3333, 0xCCCC), QD_RGB(0x3333, 0x3333, 0x9999),
	QD_RGB(0x3333, 0x3333, 0x6666), QD_RGB(0x3333, 0x3333, 0x3333), QD_RGB(0x3333, 0x3333, 0x0000),
	QD_RGB(0x3333, 0x0000, 0xFFFF), QD_RGB(0x3333, 0x0000, 0xCCCC), QD_RGB(0x3333, 0x0000, 0x9999),
	QD_RGB(0x3333, 0x0000, 0x6666), QD_RGB(0x3333, 0x0000, 0x3333), QD_RGB(0x3333, 0x0000, 0x0000),

	QD_RGB(0x0000, 0xFFFF, 0xFFFF), QD_RGB(0x0000, 0xFFFF, 0xCCCC), QD_RGB(0x0000, 0xFFFF, 0x9999),
	QD_RGB(0x0000, 0xFFFF, 0x6666), QD_RGB(0x0000, 0xFFFF, 0x3333), QD_RGB(0x0000, 0xFFFF, 0x0000),
	QD_RGB(0x0000, 0xCCCC, 0xFFFF), QD_RGB(0x0000, 0xCCCC, 0xCCCC), QD_RGB(0x0000, 0xCCCC, 0x9999),
	QD_RGB(0x0000, 0xCCCC, 0x6666), QD_RGB(0x0000, 0xCCCC, 0x3333), QD_RGB(0x0000, 0xCCCC, 0x0000),
	QD_RGB(0x0000, 0x9999, 0xFFFF), QD_RGB(0x0000, 0x9999, 0xCCCC), QD_RGB(0x0000, 0x9999, 0x9999),
	QD_RGB(0x0000, 0x9999, 0x6666), QD_RGB(0x0000, 0x9999, 0x3333), QD_RGB(0x0000, 0x9999, 0x0000),
	QD_RGB(0x0000, 0x6666, 0xFFFF), QD_RGB(0x0000, 0x6666, 0xCCCC), QD_RGB(0x0000, 0x6666, 0x9999),
	QD_RGB(0x0000, 0x6666, 0x6666), QD_RGB(0x0000, 0x6666, 0x3333), QD_RGB(0x0000, 0x6666, 0x0000),
	QD_RGB(0x0000, 0x3333, 0xFFFF), QD_RGB(0x0000, 0x3333, 0xCCCC), QD_RGB(0x0000, 0x3333, 0x9999),
	QD_RGB(0x0000, 0x3333, 0x6666), QD_RGB(0x0000, 0x3333, 0x3333), QD_RGB(0x0000, 0x3333, 0x0000),
	QD_RGB(0x0000, 0x0000, 0xFFFF), QD_RGB(0x0000, 0x0000, 0xCCCC), QD_RGB(0x0000, 0x0000, 0x9999),
	QD_RGB(0x0000, 0x0000, 0x6666), QD_RGB(0x0000, 0x0000, 0x3333),

	QD_RGB(0xEEEE, 0x0000, 0x0000), QD_RGB(0xDDDD, 0x0000, 0x0000),
	QD_RGB(0xBBBB, 0x0000, 0x0000), QD_RGB(0xAAAA, 0x0000, 0x0000),
	QD_RGB(0x8888, 0x0000, 0x0000), QD_RGB(0x7777, 0x0000, 0x0000),
	QD_RGB(0x5555, 0x0000, 0x0000), QD_RGB(0x4444, 0x0000, 0x0000),
	QD_RGB(0x2222, 0x0000, 0x0000), QD_RGB(0x1111, 0x0000, 0x0000),

	QD_RGB(0x0000, 0xEEEE, 0x0000), QD_RGB(0x0000, 0xDDDD, 0x0000),
	QD_RGB(0x0000, 0xBBBB, 0x0000), QD_RGB(0x0000, 0xAAAA, 0x0000),
	QD_RGB(0x0000, 0x8888, 0x0000), QD_RGB(0x0000, 0x7777, 0x0000),
	QD_RGB(0x0000, 0x5555, 0x0000), QD_RGB(0x0000, 0x4444, 0x0000),
	QD_RGB(0x0000, 0x2222, 0x0000), QD_RGB(0x0000, 0x1111, 0x0000),

	QD_RGB(0x0000, 0x0000, 0xEEEE), QD_RGB(0x0000, 0x0000, 0xDDDD),
	QD_RGB(0x0000, 0x0000, 0xBBBB), QD_RGB(0x0000, 0x0000, 0xAAAA),
	QD_RGB(0x0000, 0x0000, 0x8888), QD_RGB(0x0000, 0x0000, 0x7777),
	QD_RGB(0x0000, 0x0000, 0x5555), QD_RGB(0x0000, 0x0000, 0x4444),
	QD_RGB(0x0000, 0x0000, 0x2222), QD_RGB(0x0000, 0x0000, 0x1111),

	QD_RGB(0xEEEE, 0xEEEE, 0xEEEE), QD_RGB(0xDDDD, 0xDDDD, 0xDDDD),
	QD_RGB(0xBBBB, 0xBBBB, 0xBBBB), QD_RGB(0xAAAA, 0xAAAA, 0xAAAA),
	QD_RGB(0x8888, 0x8888, 0x8888), QD_RGB(0x7777, 0x7777, 0x7777),
	QD_RGB(0x5555, 0x5555, 0x5555), QD_RGB(0x4444, 0x4444, 0x4444),
	QD_RGB(0x2222, 0x2222, 0x2222), QD_RGB(0x1111, 0x1111, 0x1111),
	QD_RGB(0x0000, 0x0000, 0x0000),
};
