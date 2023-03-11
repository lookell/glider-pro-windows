#ifndef MACINTOSH_H_
#define MACINTOSH_H_

#include "MacTypes.h"
#include "WinAPI.h"

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

#define srcCopy 0
#define srcXor 2

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
void Mac_FrameRect(HDC hdc, const Rect *r, HBRUSH hbr, SInt16 w, SInt16 h);
UInt32 Mac_GetDateTime(void);
void Mac_InvalWindowRect(HWND window, const Rect *bounds);
void Mac_PaintRect(HDC hdc, const Rect *r, HBRUSH hbr);

#endif
