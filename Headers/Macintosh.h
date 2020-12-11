#ifndef MACINTOSH_H_
#define MACINTOSH_H_

#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

#ifdef GP_USE_WINAPI_H
// Make a COLORREF value from three UInt16 components
#define QD_RGB(r, g, b) RGB(((r) >> 8) & 0xFF, ((g) >> 8) & 0xFF, ((b) >> 8) & 0xFF)

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
UInt32 Mac_GetDateTime(void);
#ifdef GP_USE_WINAPI_H
void Mac_InvalWindowRect(HWND window, const Rect *bounds);
void Mac_Line(HDC hdc, SInt16 dh, SInt16 dv);
void Mac_LineTo(HDC hdc, SInt16 h, SInt16 v);
void Mac_PaintRect(HDC hdc, const Rect *r, HBRUSH hbr);
SInt16 Mac_StringWidth(HDC hdc, ConstStringPtr s);
#endif

#endif
