#ifndef MACINTOSH_H_
#define MACINTOSH_H_

#include "MacTypes.h"
#include "WinAPI.h"

// Convert Macintosh ticks (1/60 second units) to milliseconds
#define TicksToMillis(ticks) ((ticks) * 50 / 3)

// Convert milliseconds to Macintosh ticks (1/60 second units)
#define MillisToTicks(millis) ((millis) * 3 / 50)

#define QDBlack		QDColors[0]
#define QDYellow	QDColors[1]
#define QDMagenta	QDColors[2]
#define QDRed		QDColors[3]
#define QDCyan		QDColors[4]
#define QDGreen		QDColors[5]
#define QDBlue		QDColors[6]
#define QDWhite		QDColors[7]
extern const RGBColor QDColors[8];
extern const RGBColor MacColor4[16];
extern const RGBColor MacColor8[256];

#define MAKE_MAC_COLOR_(rgb) RGB((rgb).red >> 8, (rgb).green >> 8, (rgb).blue >> 8)
#define blackColor MAKE_MAC_COLOR_(QDBlack)
#define yellowColor MAKE_MAC_COLOR_(QDYellow)
#define magentaColor MAKE_MAC_COLOR_(QDMagenta)
#define redColor MAKE_MAC_COLOR_(QDRed)
#define cyanColor MAKE_MAC_COLOR_(QDCyan)
#define greenColor MAKE_MAC_COLOR_(QDGreen)
#define blueColor MAKE_MAC_COLOR_(QDBlue)
#define whiteColor MAKE_MAC_COLOR_(QDWhite)

#define srcCopy 0
#define srcXor 2
#define transparent 36

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
SInt32 Mac_DeltaPoint(Point ptA, Point ptB);
void Mac_DrawPicture(HDC hdcDst, HBITMAP myPicture, const Rect *dstRect);
void Mac_DrawString(HDC hdc, StringPtr s);
Boolean Mac_EqualString(StringPtr aStr, StringPtr bStr, Boolean caseSens);
void Mac_FrameRect(HDC hdc, const Rect *r, HBRUSH hbr, SInt16 w, SInt16 h);
void Mac_GetDateTime(UInt32 *secs);
void Mac_InsetRect(Rect *r, SInt16 dh, SInt16 dv);
void Mac_InvalWindowRect(HWND window, const Rect *bounds);
void Mac_Line(HDC hdc, SInt16 dh, SInt16 dv);
void Mac_LineTo(HDC hdc, SInt16 h, SInt16 v);
void Mac_NumToString(SInt32 theNum, StringPtr theString);
void Mac_PaintRect(HDC hdc, const Rect *r, HBRUSH hbr);
Boolean Mac_PtInRect(Point pt, const Rect *r);
Boolean Mac_SectRect(const Rect *src1, const Rect *src2, Rect *dstRect);
SInt16 Mac_StringWidth(HDC hdc, StringPtr s);

int WinFromMacString(wchar_t *winbuf, int winlen, ConstStringPtr macbuf);
int MacFromWinString(StringPtr macbuf, int maclen, const wchar_t *winbuf);

#endif
