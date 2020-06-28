#ifndef COLOR_UTILS_H_
#define COLOR_UTILS_H_

//============================================================================
//----------------------------------------------------------------------------
//                                ColorUtils.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

COLORREF Index2ColorRef (SInt32 color);
void ColorText (HDC hdc, StringPtr theStr, SInt32 color);
void ColorRect (HDC hdc, Rect *theRect, SInt32 color);
void ColorOval (HDC hdc, Rect *theRect, SInt32 color);
void ColorRegion (HDC hdc, HRGN theRgn, SInt32 color);
void ColorLine (HDC hdc, SInt16 h0, SInt16 v0, SInt16 h1, SInt16 v1, SInt32 color);
void HiliteRect (HDC hdc, Rect *theRect, SInt32 color1, SInt32 color2);
void ColorFrameRect (HDC hdc, Rect *theRect, SInt32 color);
void ColorFrameWHRect (HDC hdc, SInt16 left, SInt16 top, SInt16 wide, SInt16 high,
	SInt32 color);
void ColorFrameOval (HDC hdc, Rect *theRect, SInt32 color);
COLORREF LtGrayForeColor (void);
COLORREF GrayForeColor (void);
COLORREF DkGrayForeColor (void);
void ColorShadowRect (HDC hdc, Rect *theRect, SInt32 color);
void ColorShadowOval (HDC hdc, Rect *theRect, SInt32 color);
void ColorShadowRegion (HDC hdc, HRGN theRgn, SInt32 color);

#endif
