//============================================================================
//----------------------------------------------------------------------------
//                                ColorUtils.c
//----------------------------------------------------------------------------
//============================================================================

#include "ColorUtils.h"

#include "DrawUtils.h"
#include "Macintosh.h"

//==============================================================  Functions
//--------------------------------------------------------------  Index2ColorRef
// Given an index into the current palette, this function returns the
// corresponding COLORREF value. If the index is out of bounds, then
// RGB(0,0,0) is returned.

COLORREF Index2ColorRef (SInt32 color)
{
	if (color < 0 || color >= ARRAYSIZE(MacColor8))
	{
		return RGB(0x00, 0x00, 0x00);
	}
	return MacColor8[color];
}

//--------------------------------------------------------------  ColorRect
// Given a rectangle and color index, this function draws a solid
// rectangle in that color.  Current port, pen mode, etc. assumed.

void ColorRect (HDC hdc, const Rect *theRect, SInt32 color)
{
	COLORREF theRGBColor, wasColor;

	theRGBColor = Index2ColorRef(color);
	wasColor = SetDCBrushColor(hdc, theRGBColor);
	Mac_PaintRect(hdc, theRect, GetStockBrush(DC_BRUSH));
	SetDCBrushColor(hdc, wasColor);
}

//--------------------------------------------------------------  ColorOval
// Given a rectangle and color index, this function draws a solid
// oval in that color.  Current port, pen mode, etc. assumed.

void ColorOval (HDC hdc, const Rect *theRect, SInt32 color)
{
	COLORREF theRGBColor, wasColor;
	HRGN theRegion;

	if (theRect->left >= theRect->right || theRect->top >= theRect->bottom)
		return;

	theRGBColor = Index2ColorRef(color);
	wasColor = SetDCBrushColor(hdc, theRGBColor);
	theRegion = CreateEllipticRgn(theRect->left, theRect->top,
			theRect->right + 1, theRect->bottom + 1);
	FillRgn(hdc, theRegion, GetStockBrush(DC_BRUSH));
	DeleteRgn(theRegion);
	SetDCBrushColor(hdc, wasColor);
}

//--------------------------------------------------------------  ColorRegion
// Given a region and color index, this function draws a solid
// region in that color.  Current port, pen mode, etc. assumed.

void ColorRegion (HDC hdc, HRGN theRgn, SInt32 color)
{
	COLORREF theRGBColor, wasColor;

	theRGBColor = Index2ColorRef(color);
	wasColor = SetDCBrushColor(hdc, theRGBColor);
	FillRgn(hdc, theRgn, GetStockBrush(DC_BRUSH));
	SetDCBrushColor(hdc, wasColor);
}

//--------------------------------------------------------------  ColorLine
// Given a the end points for a line and color index, this function
// draws a line in that color.  Current port, pen mode, etc. assumed.

void ColorLine (HDC hdc, SInt16 h0, SInt16 v0, SInt16 h1, SInt16 v1, SInt32 color)
{
	COLORREF theRGBColor, wasColor;
	HPEN wasPen;

	theRGBColor = Index2ColorRef(color);
	wasColor = SetDCPenColor(hdc, theRGBColor);
	wasPen = SelectPen(hdc, GetStockPen(DC_PEN));
	DrawInclusiveLine(hdc, h0, v0, h1, v1);
	SelectPen(hdc, wasPen);
	SetDCPenColor(hdc, wasColor);
}

//--------------------------------------------------------------  HiliteRect
// Given a rect and two hilite colors, this function frames the top and
// left edges of the rect with color 1 and frames the bottom and right
// sides with color 2.  A rect can be made to appear "hi-lit" or "3D"
// in this way.

void HiliteRect (HDC hdc, const Rect *theRect, SInt32 color1, SInt32 color2)
{
	ColorLine(hdc, theRect->left, theRect->top, theRect->right - 2,
			theRect->top, color1);
	ColorLine(hdc, theRect->left, theRect->top, theRect->left,
			theRect->bottom - 2, color1);
	ColorLine(hdc, theRect->right - 1, theRect->top, theRect->right - 1,
			theRect->bottom - 2, color2);
	ColorLine(hdc, theRect->left + 1, theRect->bottom - 1, theRect->right - 1,
			theRect->bottom - 1, color2);
}

//--------------------------------------------------------------  ColorFrameRect
// Given a rectangle and color index, this function frames a
// rectangle in that color.  Current port, pen mode, etc. assumed.

void ColorFrameRect (HDC hdc, const Rect *theRect, SInt32 color)
{
	COLORREF theRGBColor, wasColor;

	theRGBColor = Index2ColorRef(color);
	wasColor = SetDCBrushColor(hdc, theRGBColor);
	Mac_FrameRect(hdc, theRect, GetStockBrush(DC_BRUSH), 1, 1);
	SetDCBrushColor(hdc, wasColor);
}

//--------------------------------------------------------------  ColorFrameOval
// Given a rectangle and color index, this function frames an
// oval in that color.  Current port, pen mode, etc. assumed.

void ColorFrameOval (HDC hdc, const Rect *theRect, SInt32 color)
{
	COLORREF theRGBColor, wasColor;
	HRGN theRegion;

	if (theRect->left >= theRect->right || theRect->top >= theRect->bottom)
		return;

	theRGBColor = Index2ColorRef(color);
	wasColor = SetDCBrushColor(hdc, theRGBColor);
	theRegion = CreateEllipticRgn(theRect->left, theRect->top,
			theRect->right + 1, theRect->bottom + 1);
	FrameRgn(hdc, theRegion, GetStockBrush(DC_BRUSH), 1, 1);
	DeleteRgn(theRegion);
	SetDCBrushColor(hdc, wasColor);
}

//--------------------------------------------------------------  LtGrayForeColor
// This function finds the closest match to a "light gray" in the
// current palette and sets the pen color to that.

COLORREF LtGrayForeColor (void)
{
	return QD_RGB(0xBFFF, 0xBFFF, 0xBFFF);
}

//--------------------------------------------------------------  GrayForeColor
// This function finds the closest match to a "medium gray" in the
// current palette and sets the pen color to that.

COLORREF GrayForeColor (void)
{
	return QD_RGB(0x7FFF, 0x7FFF, 0x7FFF);
}

//--------------------------------------------------------------  DkGrayForeColor
// This function finds the closest match to a "dark gray" in the
// current palette and sets the pen color to that.

COLORREF DkGrayForeColor (void)
{
	return QD_RGB(0x3FFF, 0x3FFF, 0x3FFF);
}

//--------------------------------------------------------------  DitherShadowRect
// This functions fills in every other pixel in the given rectangle with black
// and leaves the rest unmodified, creating a dithered shadow.

void DitherShadowRect (HDC hdc, const Rect *theRect)
{
	HRGN theRgn;

	if (theRect->left >= theRect->right || theRect->top >= theRect->bottom)
		return;

	theRgn = CreateRectRgn(theRect->left, theRect->top,
			theRect->right, theRect->bottom);
	DitherShadowRegion(hdc, theRgn);
	DeleteRgn(theRgn);
}

//--------------------------------------------------------------  DitherShadowOval
// Similar to DitherShadowRect, but the shadow is drawn within the given oval.

void DitherShadowOval (HDC hdc, const Rect *theRect)
{
	HRGN theRgn;

	if (theRect->left >= theRect->right || theRect->top >= theRect->bottom)
		return;

	theRgn = CreateEllipticRgn(theRect->left, theRect->top,
			theRect->right + 1, theRect->bottom + 1);
	DitherShadowRegion(hdc, theRgn);
	DeleteRgn(theRgn);
}

//--------------------------------------------------------------  DitherShadowRegion
// Similar to DitherShadowRect, but the shadow is drawn within the given region.
// The region's coordinates are presumed to be in logical units.

void DitherShadowRegion (HDC hdc, HRGN theRgn)
{
	HBITMAP shadowBitmap;
	HBRUSH shadowBrush;
	COLORREF wasTextColor;
	COLORREF wasBkColor;
	int wasROP2;

	shadowBitmap = CreateShadowBitmap();
	shadowBrush = CreatePatternBrush(shadowBitmap);
	wasTextColor = SetTextColor(hdc, RGB(0x00, 0x00, 0x00));
	wasBkColor = SetBkColor(hdc, RGB(0xFF, 0xFF, 0xFF));
	wasROP2 = SetROP2(hdc, R2_MASKPEN);
	FillRgn(hdc, theRgn, shadowBrush);
	SetROP2(hdc, wasROP2);
	SetBkColor(hdc, wasBkColor);
	SetTextColor(hdc, wasTextColor);
	DeleteBrush(shadowBrush);
	DeleteBitmap(shadowBitmap);
}

//--------------------------------------------------------------  CreateShadowBitmap
// Create a dithered gray bitmap for filling in shadows. It can be made into
// a brush by calling CreatePatternBrush(), and then colorized by calling
// SetTextColor() and SetBkColor() on the DC that the brush is selected into.

HBITMAP CreateShadowBitmap (void)
{
	const WORD grayBits[8] = { 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA };
	return CreateBitmap(8, 8, 1, 1, grayBits);
}
