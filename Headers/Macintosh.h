#ifndef MACINTOSH_H_
#define MACINTOSH_H_

#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

// Convert Macintosh ticks (1/60 second units) to milliseconds
#define TicksToMillis(ticks) (UInt32)((unsigned long long)(ticks) * 50 / 3)

#ifdef GP_USE_WINAPI_H
extern const COLORREF blackColor;
extern const COLORREF yellowColor;
extern const COLORREF magentaColor;
extern const COLORREF redColor;
extern const COLORREF cyanColor;
extern const COLORREF greenColor;
extern const COLORREF blueColor;
extern const COLORREF whiteColor;
extern const COLORREF MacColor4[16];
extern const COLORREF MacColor8[256];
#endif

#define srcCopy 0
#define srcXor 2
#define transparent 36

#ifdef GP_USE_WINAPI_H
void Mac_CopyBits(
	HDC srcBits,
	HDC dstBits,
	const Rect *srcRect,
	const Rect *dstRect,
	SInt16 mode,
	HRGN maskRgn);
void Mac_CopyMask(
	HDC srcBits,
	HDC maskBits,
	HDC dstBits,
	const Rect *srcRect,
	const Rect *maskRect,
	const Rect *dstRect);
void Mac_DrawPicture(HDC hdcDst, HBITMAP myPicture, const Rect *dstRect);
void Mac_DrawString(HDC hdc, ConstStringPtr s);
void Mac_FrameRect(HDC hdc, const Rect *r, HBRUSH hbr, SInt16 w, SInt16 h);
#endif
void Mac_GetDateTime(UInt32 *secs);
#ifdef GP_USE_WINAPI_H
void Mac_InvalWindowRect(HWND window, const Rect *bounds);
void Mac_Line(HDC hdc, SInt16 dh, SInt16 dv);
void Mac_LineTo(HDC hdc, SInt16 h, SInt16 v);
void Mac_PaintRect(HDC hdc, const Rect *r, HBRUSH hbr);
SInt16 Mac_StringWidth(HDC hdc, ConstStringPtr s);
#endif

#endif
