//============================================================================
//----------------------------------------------------------------------------
//                                ColorUtils.h
//----------------------------------------------------------------------------
//============================================================================

#ifndef COLOR_UTILS_H_
#define COLOR_UTILS_H_

#include "MacTypes.h"
#include "WinAPI.h"

COLORREF Index2ColorRef (SInt32 color);
void ColorRect (HDC hdc, const Rect *theRect, SInt32 color);
void ColorOval (HDC hdc, const Rect *theRect, SInt32 color);
void ColorRegion (HDC hdc, HRGN theRgn, SInt32 color);
void ColorLine (HDC hdc, SInt16 h0, SInt16 v0, SInt16 h1, SInt16 v1, SInt32 color);
void HiliteRect (HDC hdc, const Rect *theRect, SInt32 color1, SInt32 color2);
void ColorFrameRect (HDC hdc, const Rect *theRect, SInt32 color);
void ColorFrameOval (HDC hdc, const Rect *theRect, SInt32 color);
COLORREF LtGrayForeColor (void);
COLORREF GrayForeColor (void);
COLORREF DkGrayForeColor (void);
void DitherShadowRect (HDC hdc, const Rect *theRect);
void DitherShadowOval (HDC hdc, const Rect *theRect);
void DitherShadowRegion (HDC hdc, HRGN theRgn);
void DitherShadowPath (HDC hdc);
HBITMAP CreateShadowBitmap (void);

#endif
