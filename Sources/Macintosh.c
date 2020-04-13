#include "Macintosh.h"
#include "WinAPI.h"
#include <inttypes.h>
#include <limits.h>
#include <string.h>
#include <strsafe.h>


#define CP_MACROMAN 10000


//--------------------------------------------------------------  CopyBits
// Copy some portion of a bitmap from one graphics port to another.

void Mac_CopyBits(
	HDC srcBits,
	HDC dstBits,
	const Rect* srcRect,
	const Rect* dstRect,
	SInt16 mode,
	HRGN maskRgn)
{
	INT xSrc, ySrc, wSrc, hSrc;
	INT xDst, yDst, wDst, hDst;

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

	SaveDC(dstBits);
	if (maskRgn != NULL)
		SelectClipRgn(dstBits, maskRgn);

	switch (mode)
	{
		case srcCopy:
		StretchBlt(dstBits, xDst, yDst, wDst, hDst,
				srcBits, xSrc, ySrc, wSrc, hSrc, SRCCOPY);
		break;

		case srcXor:
		// invert the destination where the source is black
		// and keep the destination where the source is white
		BitBlt(dstBits, xDst, yDst, wDst, hDst, NULL, 0, 0, DSTINVERT);
		SetBkColor(dstBits, RGB(0xFF, 0xFF, 0xFF));
		SetTextColor(dstBits, RGB(0x00, 0x00, 0x00));
		StretchBlt(dstBits, xDst, yDst, wDst, hDst,
				srcBits, xSrc, ySrc, wSrc, hSrc, SRCINVERT);
		break;

		case transparent:
		TransparentBlt(dstBits, xDst, yDst, wDst, hDst,
				srcBits, xSrc, ySrc, wSrc, hSrc, GetBkColor(dstBits));
		break;
	}

	RestoreDC(dstBits, -1);
}

//--------------------------------------------------------------  CopyBits
// Copy some portion of a bitmap from one graphics port to another,
// with a mask to specify how much of each pixel is copied over.

void Mac_CopyMask(
	HDC srcBits,
	HDC maskBits,
	HDC dstBits,
	const Rect* srcRect,
	const Rect* maskRect,
	const Rect* dstRect)
{
	// TODO: add support for masks where pixels are between black and white
	HDC newSrcBits;
	HBITMAP newSrcBitmap, hbmPrev;
	INT xSrc, ySrc, wSrc, hSrc;
	INT xMask, yMask, wMask, hMask;
	INT xDst, yDst, wDst, hDst;

	if (srcRect->left >= srcRect->right || srcRect->top >= srcRect->bottom)
		return;
	if (maskRect->left >= maskRect->right || maskRect->top >= maskRect->bottom)
		return;
	if (dstRect->left >= dstRect->right || dstRect->top >= dstRect->bottom)
		return;

	xSrc = srcRect->left;
	ySrc = srcRect->top;
	wSrc = srcRect->right - srcRect->left;
	hSrc = srcRect->bottom - srcRect->top;
	xMask = maskRect->left;
	yMask = maskRect->top;
	wMask = maskRect->right - maskRect->left;
	hMask = maskRect->bottom - maskRect->top;
	xDst = dstRect->left;
	yDst = dstRect->top;
	wDst = dstRect->right - dstRect->left;
	hDst = dstRect->bottom - dstRect->top;

	SaveDC(dstBits);
	// make sure that monochrome masks are colorized correctly
	SetTextColor(dstBits, RGB(0x00, 0x00, 0x00));
	SetBkColor(dstBits, RGB(0xFF, 0xFF, 0xFF));
	// apply the mask to the destination
	StretchBlt(dstBits, xDst, yDst, wDst, hDst,
		maskBits, xMask, yMask, wMask, hMask, SRCAND); // DSa
	// create temporary drawing surface to adjust the source bits
	newSrcBits = CreateCompatibleDC(srcBits);
	SetTextColor(newSrcBits, RGB(0x00, 0x00, 0x00));
	SetBkColor(newSrcBits, RGB(0xFF, 0xFF, 0xFF));
	newSrcBitmap = CreateCompatibleBitmap(srcBits, wSrc, hSrc);
	hbmPrev = SelectObject(newSrcBits, newSrcBitmap);
	BitBlt(newSrcBits, 0, 0, wSrc, hSrc, srcBits, xSrc, ySrc, SRCCOPY);
	// black out unwanted source bits with the inverted mask
	StretchBlt(newSrcBits, 0, 0, wSrc, hSrc,
		maskBits, xMask, yMask, wMask, hMask, 0x00220326); // DSna
	// apply adjusted source bits to the destination
	StretchBlt(dstBits, xDst, yDst, wDst, hDst,
		newSrcBits, 0, 0, wSrc, hSrc, SRCPAINT); // DSo
	// clean up
	RestoreDC(dstBits, -1);
	SelectObject(newSrcBits, hbmPrev);
	DeleteObject(newSrcBitmap);
	DeleteDC(newSrcBits);
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

void Mac_DrawString(HDC hdc, StringPtr s)
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

//--------------------------------------------------------------  InsetRect
// Shrink or expand the given rectangle. The rectangle's sides
// are moved inwards by the given deltas. To expand the rectangle,
// call this function with negative delta values. If either the width
// or the height of the resulting rectangle is less than 1, then the
// rectangle is set to the empty rectangle (0,0,0,0).

void Mac_InsetRect(Rect *r, SInt16 dh, SInt16 dv)
{
	if (r == NULL)
		return;
	r->left += dh;
	r->top += dv;
	r->right -= dh;
	r->bottom -= dv;
	if (r->left > r->right || r->top > r->bottom)
	{
		r->left = 0;
		r->top = 0;
		r->right = 0;
		r->bottom = 0;
	}
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
	MoveToEx(hdc, curPos.x + dh, curPos.y + dv, NULL);
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
	MoveToEx(hdc, h, v, NULL);
	LineTo(hdc, curPos.x, curPos.y);
	LineTo(hdc, h, v);
}

//--------------------------------------------------------------  NumToString
// Convert the given number to a decimal string representation.
// The string is written to the given output string.

void Mac_NumToString(SInt32 theNum, StringPtr theString)
{
	CHAR buffer[sizeof("-2147483648")];
	size_t length;
	HRESULT hr;

	if (theString == NULL)
		return;
	theString[0] = 0; // return an empty string if an error occurs.
	hr = StringCchPrintfA(buffer, ARRAYSIZE(buffer), "%" PRId32, theNum);
	if (FAILED(hr))
		return;
	hr = StringCchLengthA(buffer, ARRAYSIZE(buffer), &length);
	if (FAILED(hr))
		return;
	theString[0] = length;
	memcpy(&theString[1], buffer, length);
}

//--------------------------------------------------------------  PtInRect
// Check if the given point is enclosed within the given rectangle.
// Return nonzero (true) if the point is within the rectangle; otherwise
// return zero (false). The point is considered within the rectangle
// if it is within all four sides, or if it lies on the left or top side.
// The point is not considered within the rectangle if it lies on the
// right or bottom side (because its corresponding pixel would be
// outside the rectangle).

Boolean Mac_PtInRect(Point pt, const Rect *r)
{
	if (r == NULL)
		return false;
	return (pt.h >= r->left) && (pt.h < r->right) &&
			(pt.v >= r->top) && (pt.v < r->bottom);
}

//--------------------------------------------------------------  SectRect
// Calculate the intersection of the two source rectangles, and write
// that intersection into the destination rectangle. If the source
// rectangles do not intersect, then the destination rectangle is set
// to all zeroes. The return value is TRUE if the source rectangles
// intersect, and FALSE if they do not intersect.

Boolean Mac_SectRect(const Rect *src1, const Rect *src2, Rect *dstRect)
{
	Rect sectRect;

	sectRect.left = 0;
	sectRect.top = 0;
	sectRect.right = 0;
	sectRect.bottom = 0;
	// If either source rectangle is empty, then the intersection is empty.
	if ((src1->left >= src1->right) || (src1->top >= src1->bottom) ||
		(src2->left >= src2->right) || (src2->top >= src2->bottom))
	{
		*dstRect = sectRect;
		return false;
	}
	// If the first rectangle is above, below, or to the side of the second
	// rectangle (touching or not), then the intersection is empty.
	if ((src1->bottom <= src2->top) || (src2->bottom <= src1->top) ||
		(src1->right <= src2->left) || (src2->right <= src1->left))
	{
		*dstRect = sectRect;
		return false;
	}
	// The two rectangles intersect, so calculate the intersection's boundary.
	sectRect.left = (src1->left > src2->left) ? src1->left : src2->left;
	sectRect.top = (src1->top > src2->top) ? src1->top : src2->top;
	sectRect.right = (src1->right <= src2->right) ? src1->right : src2->right;
	sectRect.bottom = (src1->bottom <= src2->bottom) ? src1->bottom : src2->bottom;
	*dstRect = sectRect;
	return true;
}

//--------------------------------------------------------------  WinFromMacString
// Convert a MacRoman Pascal-style string to a UTF-16 C-style string.
// This is a wrapper around the Windows API function 'MultiByteToWideChar'.
// The return value of this function is the return value of 'MultiByteToWideChar'.

int WinFromMacString(wchar_t *winbuf, int winlen, StringPtr macbuf)
{
	int result;
	size_t maclen;

	if (macbuf == NULL)
		return 0;
	maclen = macbuf[0];
	memmove(&macbuf[0], &macbuf[1], maclen);
	macbuf[maclen] = '\0';
	result = MultiByteToWideChar(
		CP_MACROMAN,
		0,
		(LPCCH)macbuf,
		-1,
		winbuf,
		winlen
	);
	memmove(&macbuf[1], &macbuf[0], maclen);
	macbuf[0] = maclen;
	return result;
}

//--------------------------------------------------------------  MacFromWinString
// Convert a UTF-16 C-style string to a MacRoman Pascal-style string.
// (This may result in data-loss!)
// This is a wrapper around the Windows API function 'WideCharToMultiByte'.
// The return value of this function is the return value of 'WideCharToMultiByte'.

int MacFromWinString(StringPtr macbuf, int maclen, const wchar_t *winbuf)
{
	int result;
	size_t winlen;

	if (macbuf == NULL)
		return 0;
	if (FAILED(StringCchLengthW(winbuf, INT_MAX, &winlen)))
		return 0;
	result = WideCharToMultiByte(
		CP_MACROMAN,
		0,
		winbuf,
		winlen,
		(LPSTR)&macbuf[1],
		maclen - 1,
		NULL,
		NULL
	);
	if (maclen != 0)
		macbuf[0] = result & 0xFF;
	return result;
}

//--------------------------------------------------------------  Global Data

// The eight basic QuickDraw colors
const RGBColor QDColors[] = {
	{ 0x0000, 0x0000, 0x0000 }, // black
	{ 0xFC00, 0xF37D, 0x052F }, // yellow
	{ 0xF2D7, 0x0856, 0x84EC }, // magenta
	{ 0xDD6B, 0x08C2, 0x06A2 }, // red
	{ 0x0241, 0xAB54, 0xEAFF }, // cyan
	{ 0x0000, 0x8000, 0x11B0 }, // green
	{ 0x0000, 0x0000, 0xD400 }, // blue
	{ 0xFFFF, 0xFFFF, 0xFFFF }, // white
};

// The default 4-bit Macintosh color palette
const RGBColor MacColor4[] = {
	{ 0xFFFF, 0xFFFF, 0xFFFF }, // white
	{ 0xFFFF, 0xFFFF, 0x0000 }, // yellow
	{ 0xFFFF, 0x6666, 0x0000 }, // orange
	{ 0xDDDD, 0x0000, 0x0000 }, // red
	{ 0xFFFF, 0x0000, 0x9999 }, // magenta
	{ 0x3333, 0x0000, 0x9999 }, // purple
	{ 0x0000, 0x0000, 0xCCCC }, // blue
	{ 0x0000, 0x9999, 0xFFFF }, // cyan
	{ 0x0000, 0xAAAA, 0x0000 }, // green
	{ 0x0000, 0x6666, 0x0000 }, // dark green
	{ 0x6666, 0x3333, 0x0000 }, // brown
	{ 0x9999, 0x6666, 0x3333 }, // tan
	{ 0xBBBB, 0xBBBB, 0xBBBB }, // light gray
	{ 0x8888, 0x8888, 0x8888 }, // medium gray
	{ 0x4444, 0x4444, 0x4444 }, // dark gray
	{ 0x0000, 0x0000, 0x0000 }, // black
};

// The default 8-bit Macintosh color palette
const RGBColor MacColor8[] = {
	{ 0xFFFF, 0xFFFF, 0xFFFF }, { 0xFFFF, 0xFFFF, 0xCCCC }, { 0xFFFF, 0xFFFF, 0x9999 },
	{ 0xFFFF, 0xFFFF, 0x6666 }, { 0xFFFF, 0xFFFF, 0x3333 }, { 0xFFFF, 0xFFFF, 0x0000 },
	{ 0xFFFF, 0xCCCC, 0xFFFF }, { 0xFFFF, 0xCCCC, 0xCCCC }, { 0xFFFF, 0xCCCC, 0x9999 },
	{ 0xFFFF, 0xCCCC, 0x6666 }, { 0xFFFF, 0xCCCC, 0x3333 }, { 0xFFFF, 0xCCCC, 0x0000 },
	{ 0xFFFF, 0x9999, 0xFFFF }, { 0xFFFF, 0x9999, 0xCCCC }, { 0xFFFF, 0x9999, 0x9999 },
	{ 0xFFFF, 0x9999, 0x6666 }, { 0xFFFF, 0x9999, 0x3333 }, { 0xFFFF, 0x9999, 0x0000 },
	{ 0xFFFF, 0x6666, 0xFFFF }, { 0xFFFF, 0x6666, 0xCCCC }, { 0xFFFF, 0x6666, 0x9999 },
	{ 0xFFFF, 0x6666, 0x6666 }, { 0xFFFF, 0x6666, 0x3333 }, { 0xFFFF, 0x6666, 0x0000 },
	{ 0xFFFF, 0x3333, 0xFFFF }, { 0xFFFF, 0x3333, 0xCCCC }, { 0xFFFF, 0x3333, 0x9999 },
	{ 0xFFFF, 0x3333, 0x6666 }, { 0xFFFF, 0x3333, 0x3333 }, { 0xFFFF, 0x3333, 0x0000 },
	{ 0xFFFF, 0x0000, 0xFFFF }, { 0xFFFF, 0x0000, 0xCCCC }, { 0xFFFF, 0x0000, 0x9999 },
	{ 0xFFFF, 0x0000, 0x6666 }, { 0xFFFF, 0x0000, 0x3333 }, { 0xFFFF, 0x0000, 0x0000 },

	{ 0xCCCC, 0xFFFF, 0xFFFF }, { 0xCCCC, 0xFFFF, 0xCCCC }, { 0xCCCC, 0xFFFF, 0x9999 },
	{ 0xCCCC, 0xFFFF, 0x6666 }, { 0xCCCC, 0xFFFF, 0x3333 }, { 0xCCCC, 0xFFFF, 0x0000 },
	{ 0xCCCC, 0xCCCC, 0xFFFF }, { 0xCCCC, 0xCCCC, 0xCCCC }, { 0xCCCC, 0xCCCC, 0x9999 },
	{ 0xCCCC, 0xCCCC, 0x6666 }, { 0xCCCC, 0xCCCC, 0x3333 }, { 0xCCCC, 0xCCCC, 0x0000 },
	{ 0xCCCC, 0x9999, 0xFFFF }, { 0xCCCC, 0x9999, 0xCCCC }, { 0xCCCC, 0x9999, 0x9999 },
	{ 0xCCCC, 0x9999, 0x6666 }, { 0xCCCC, 0x9999, 0x3333 }, { 0xCCCC, 0x9999, 0x0000 },
	{ 0xCCCC, 0x6666, 0xFFFF }, { 0xCCCC, 0x6666, 0xCCCC }, { 0xCCCC, 0x6666, 0x9999 },
	{ 0xCCCC, 0x6666, 0x6666 }, { 0xCCCC, 0x6666, 0x3333 }, { 0xCCCC, 0x6666, 0x0000 },
	{ 0xCCCC, 0x3333, 0xFFFF }, { 0xCCCC, 0x3333, 0xCCCC }, { 0xCCCC, 0x3333, 0x9999 },
	{ 0xCCCC, 0x3333, 0x6666 }, { 0xCCCC, 0x3333, 0x3333 }, { 0xCCCC, 0x3333, 0x0000 },
	{ 0xCCCC, 0x0000, 0xFFFF }, { 0xCCCC, 0x0000, 0xCCCC }, { 0xCCCC, 0x0000, 0x9999 },
	{ 0xCCCC, 0x0000, 0x6666 }, { 0xCCCC, 0x0000, 0x3333 }, { 0xCCCC, 0x0000, 0x0000 },

	{ 0x9999, 0xFFFF, 0xFFFF }, { 0x9999, 0xFFFF, 0xCCCC }, { 0x9999, 0xFFFF, 0x9999 },
	{ 0x9999, 0xFFFF, 0x6666 }, { 0x9999, 0xFFFF, 0x3333 }, { 0x9999, 0xFFFF, 0x0000 },
	{ 0x9999, 0xCCCC, 0xFFFF }, { 0x9999, 0xCCCC, 0xCCCC }, { 0x9999, 0xCCCC, 0x9999 },
	{ 0x9999, 0xCCCC, 0x6666 }, { 0x9999, 0xCCCC, 0x3333 }, { 0x9999, 0xCCCC, 0x0000 },
	{ 0x9999, 0x9999, 0xFFFF }, { 0x9999, 0x9999, 0xCCCC }, { 0x9999, 0x9999, 0x9999 },
	{ 0x9999, 0x9999, 0x6666 }, { 0x9999, 0x9999, 0x3333 }, { 0x9999, 0x9999, 0x0000 },
	{ 0x9999, 0x6666, 0xFFFF }, { 0x9999, 0x6666, 0xCCCC }, { 0x9999, 0x6666, 0x9999 },
	{ 0x9999, 0x6666, 0x6666 }, { 0x9999, 0x6666, 0x3333 }, { 0x9999, 0x6666, 0x0000 },
	{ 0x9999, 0x3333, 0xFFFF }, { 0x9999, 0x3333, 0xCCCC }, { 0x9999, 0x3333, 0x9999 },
	{ 0x9999, 0x3333, 0x6666 }, { 0x9999, 0x3333, 0x3333 }, { 0x9999, 0x3333, 0x0000 },
	{ 0x9999, 0x0000, 0xFFFF }, { 0x9999, 0x0000, 0xCCCC }, { 0x9999, 0x0000, 0x9999 },
	{ 0x9999, 0x0000, 0x6666 }, { 0x9999, 0x0000, 0x3333 }, { 0x9999, 0x0000, 0x0000 },

	{ 0x6666, 0xFFFF, 0xFFFF }, { 0x6666, 0xFFFF, 0xCCCC }, { 0x6666, 0xFFFF, 0x9999 },
	{ 0x6666, 0xFFFF, 0x6666 }, { 0x6666, 0xFFFF, 0x3333 }, { 0x6666, 0xFFFF, 0x0000 },
	{ 0x6666, 0xCCCC, 0xFFFF }, { 0x6666, 0xCCCC, 0xCCCC }, { 0x6666, 0xCCCC, 0x9999 },
	{ 0x6666, 0xCCCC, 0x6666 }, { 0x6666, 0xCCCC, 0x3333 }, { 0x6666, 0xCCCC, 0x0000 },
	{ 0x6666, 0x9999, 0xFFFF }, { 0x6666, 0x9999, 0xCCCC }, { 0x6666, 0x9999, 0x9999 },
	{ 0x6666, 0x9999, 0x6666 }, { 0x6666, 0x9999, 0x3333 }, { 0x6666, 0x9999, 0x0000 },
	{ 0x6666, 0x6666, 0xFFFF }, { 0x6666, 0x6666, 0xCCCC }, { 0x6666, 0x6666, 0x9999 },
	{ 0x6666, 0x6666, 0x6666 }, { 0x6666, 0x6666, 0x3333 }, { 0x6666, 0x6666, 0x0000 },
	{ 0x6666, 0x3333, 0xFFFF }, { 0x6666, 0x3333, 0xCCCC }, { 0x6666, 0x3333, 0x9999 },
	{ 0x6666, 0x3333, 0x6666 }, { 0x6666, 0x3333, 0x3333 }, { 0x6666, 0x3333, 0x0000 },
	{ 0x6666, 0x0000, 0xFFFF }, { 0x6666, 0x0000, 0xCCCC }, { 0x6666, 0x0000, 0x9999 },
	{ 0x6666, 0x0000, 0x6666 }, { 0x6666, 0x0000, 0x3333 }, { 0x6666, 0x0000, 0x0000 },

	{ 0x3333, 0xFFFF, 0xFFFF }, { 0x3333, 0xFFFF, 0xCCCC }, { 0x3333, 0xFFFF, 0x9999 },
	{ 0x3333, 0xFFFF, 0x6666 }, { 0x3333, 0xFFFF, 0x3333 }, { 0x3333, 0xFFFF, 0x0000 },
	{ 0x3333, 0xCCCC, 0xFFFF }, { 0x3333, 0xCCCC, 0xCCCC }, { 0x3333, 0xCCCC, 0x9999 },
	{ 0x3333, 0xCCCC, 0x6666 }, { 0x3333, 0xCCCC, 0x3333 }, { 0x3333, 0xCCCC, 0x0000 },
	{ 0x3333, 0x9999, 0xFFFF }, { 0x3333, 0x9999, 0xCCCC }, { 0x3333, 0x9999, 0x9999 },
	{ 0x3333, 0x9999, 0x6666 }, { 0x3333, 0x9999, 0x3333 }, { 0x3333, 0x9999, 0x0000 },
	{ 0x3333, 0x6666, 0xFFFF }, { 0x3333, 0x6666, 0xCCCC }, { 0x3333, 0x6666, 0x9999 },
	{ 0x3333, 0x6666, 0x6666 }, { 0x3333, 0x6666, 0x3333 }, { 0x3333, 0x6666, 0x0000 },
	{ 0x3333, 0x3333, 0xFFFF }, { 0x3333, 0x3333, 0xCCCC }, { 0x3333, 0x3333, 0x9999 },
	{ 0x3333, 0x3333, 0x6666 }, { 0x3333, 0x3333, 0x3333 }, { 0x3333, 0x3333, 0x0000 },
	{ 0x3333, 0x0000, 0xFFFF }, { 0x3333, 0x0000, 0xCCCC }, { 0x3333, 0x0000, 0x9999 },
	{ 0x3333, 0x0000, 0x6666 }, { 0x3333, 0x0000, 0x3333 }, { 0x3333, 0x0000, 0x0000 },

	{ 0x0000, 0xFFFF, 0xFFFF }, { 0x0000, 0xFFFF, 0xCCCC }, { 0x0000, 0xFFFF, 0x9999 },
	{ 0x0000, 0xFFFF, 0x6666 }, { 0x0000, 0xFFFF, 0x3333 }, { 0x0000, 0xFFFF, 0x0000 },
	{ 0x0000, 0xCCCC, 0xFFFF }, { 0x0000, 0xCCCC, 0xCCCC }, { 0x0000, 0xCCCC, 0x9999 },
	{ 0x0000, 0xCCCC, 0x6666 }, { 0x0000, 0xCCCC, 0x3333 }, { 0x0000, 0xCCCC, 0x0000 },
	{ 0x0000, 0x9999, 0xFFFF }, { 0x0000, 0x9999, 0xCCCC }, { 0x0000, 0x9999, 0x9999 },
	{ 0x0000, 0x9999, 0x6666 }, { 0x0000, 0x9999, 0x3333 }, { 0x0000, 0x9999, 0x0000 },
	{ 0x0000, 0x6666, 0xFFFF }, { 0x0000, 0x6666, 0xCCCC }, { 0x0000, 0x6666, 0x9999 },
	{ 0x0000, 0x6666, 0x6666 }, { 0x0000, 0x6666, 0x3333 }, { 0x0000, 0x6666, 0x0000 },
	{ 0x0000, 0x3333, 0xFFFF }, { 0x0000, 0x3333, 0xCCCC }, { 0x0000, 0x3333, 0x9999 },
	{ 0x0000, 0x3333, 0x6666 }, { 0x0000, 0x3333, 0x3333 }, { 0x0000, 0x3333, 0x0000 },
	{ 0x0000, 0x0000, 0xFFFF }, { 0x0000, 0x0000, 0xCCCC }, { 0x0000, 0x0000, 0x9999 },
	{ 0x0000, 0x0000, 0x6666 }, { 0x0000, 0x0000, 0x3333 },

	{ 0xEEEE, 0x0000, 0x0000 }, { 0xDDDD, 0x0000, 0x0000 },
	{ 0xBBBB, 0x0000, 0x0000 }, { 0xAAAA, 0x0000, 0x0000 },
	{ 0x8888, 0x0000, 0x0000 }, { 0x7777, 0x0000, 0x0000 },
	{ 0x5555, 0x0000, 0x0000 }, { 0x4444, 0x0000, 0x0000 },
	{ 0x2222, 0x0000, 0x0000 }, { 0x1111, 0x0000, 0x0000 },

	{ 0x0000, 0xEEEE, 0x0000 }, { 0x0000, 0xDDDD, 0x0000 },
	{ 0x0000, 0xBBBB, 0x0000 }, { 0x0000, 0xAAAA, 0x0000 },
	{ 0x0000, 0x8888, 0x0000 }, { 0x0000, 0x7777, 0x0000 },
	{ 0x0000, 0x5555, 0x0000 }, { 0x0000, 0x4444, 0x0000 },
	{ 0x0000, 0x2222, 0x0000 }, { 0x0000, 0x1111, 0x0000 },

	{ 0x0000, 0x0000, 0xEEEE }, { 0x0000, 0x0000, 0xDDDD },
	{ 0x0000, 0x0000, 0xBBBB }, { 0x0000, 0x0000, 0xAAAA },
	{ 0x0000, 0x0000, 0x8888 }, { 0x0000, 0x0000, 0x7777 },
	{ 0x0000, 0x0000, 0x5555 }, { 0x0000, 0x0000, 0x4444 },
	{ 0x0000, 0x0000, 0x2222 }, { 0x0000, 0x0000, 0x1111 },

	{ 0xEEEE, 0xEEEE, 0xEEEE }, { 0xDDDD, 0xDDDD, 0xDDDD },
	{ 0xBBBB, 0xBBBB, 0xBBBB }, { 0xAAAA, 0xAAAA, 0xAAAA },
	{ 0x8888, 0x8888, 0x8888 }, { 0x7777, 0x7777, 0x7777 },
	{ 0x5555, 0x5555, 0x5555 }, { 0x4444, 0x4444, 0x4444 },
	{ 0x2222, 0x2222, 0x2222 }, { 0x1111, 0x1111, 0x1111 },
	{ 0x0000, 0x0000, 0x0000 },
};
