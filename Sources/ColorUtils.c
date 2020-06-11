
//============================================================================
//----------------------------------------------------------------------------
//								  ColorUtils.c
//----------------------------------------------------------------------------
//============================================================================


#include "Environ.h"
#include "Externs.h"
#include "Macintosh.h"


//==============================================================  Functions
//--------------------------------------------------------------  Index2ColorRef

// Given an index into the current palette, this function returns the
// corresponding COLORREF value. If the index is out of bounds, then
// RGB(0,0,0) is returned.

COLORREF Index2ColorRef (SInt32 color)
{
	const COLORREF defaultClr = RGB(0x00, 0x00, 0x00);
	RGBColor rgb;
	size_t index;

	if (color < 0)
		return defaultClr;
	index = (size_t)color;

	if (thisMac.isDepth == 4)
	{
		if (index >= ARRAYSIZE(MacColor4))
			return defaultClr;
		rgb = MacColor4[index];
	}
	else
	{
		if (index >= ARRAYSIZE(MacColor8))
			return defaultClr;
		rgb = MacColor8[index];
	}

	return RGB(rgb.red >> 8, rgb.green >> 8, rgb.blue >> 8);
}

//--------------------------------------------------------------  ColorText

// Given a string and a color index (index into the current palette),…
// this function draws text in that color.  It assumes the current port,…
// the current font, the current pen location, etc.

void ColorText (HDC hdc, StringPtr theStr, SInt32 color)
{
	COLORREF	theRGBColor, wasColor;

	theRGBColor = Index2ColorRef(color);
	wasColor = SetTextColor(hdc, theRGBColor);
	Mac_DrawString(hdc, theStr);
	SetTextColor(hdc, wasColor);
}

//--------------------------------------------------------------  ColorRect

// Given a rectangle and color index, this function draws a solid…
// rectangle in that color.  Current port, pen mode, etc. assumed.

void ColorRect (HDC hdc, Rect *theRect, SInt32 color)
{
	COLORREF	theRGBColor, wasColor;

	theRGBColor = Index2ColorRef(color);
	wasColor = SetDCBrushColor(hdc, theRGBColor);
	Mac_PaintRect(hdc, theRect, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hdc, wasColor);
}

//--------------------------------------------------------------  ColorOval

// Given a rectangle and color index, this function draws a solid…
// oval in that color.  Current port, pen mode, etc. assumed.

void ColorOval (HDC hdc, Rect *theRect, SInt32 color)
{
	COLORREF	theRGBColor, wasColor;
	HRGN		theRegion;

	theRGBColor = Index2ColorRef(color);
	wasColor = SetDCBrushColor(hdc, theRGBColor);
	theRegion = CreateEllipticRgn(theRect->left, theRect->top,
			theRect->right + 1, theRect->bottom + 1);
	FillRgn(hdc, theRegion, GetStockObject(DC_BRUSH));
	DeleteObject(theRegion);
	SetDCBrushColor(hdc, wasColor);
}

//--------------------------------------------------------------  ColorRegion

// Given a region and color index, this function draws a solid…
// region in that color.  Current port, pen mode, etc. assumed.

void ColorRegion (HDC hdc, HRGN theRgn, SInt32 color)
{
	COLORREF	theRGBColor, wasColor;

	theRGBColor = Index2ColorRef(color);
	wasColor = SetDCBrushColor(hdc, theRGBColor);
	FillRgn(hdc, theRgn, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hdc, wasColor);
}

//--------------------------------------------------------------  ColorLine

// Given a the end points for a line and color index, this function…
// draws a line in that color.  Current port, pen mode, etc. assumed.

void ColorLine (HDC hdc, SInt16 h0, SInt16 v0, SInt16 h1, SInt16 v1, SInt32 color)
{
	COLORREF	theRGBColor, wasColor;
	HGDIOBJ		wasPen;

	theRGBColor = Index2ColorRef(color);
	wasColor = SetDCPenColor(hdc, theRGBColor);
	wasPen = SelectObject(hdc, GetStockObject(DC_PEN));
	MoveToEx(hdc, h0, v0, NULL);
	Mac_LineTo(hdc, h1, v1);
	SelectObject(hdc, wasPen);
	SetDCPenColor(hdc, wasColor);
}

//--------------------------------------------------------------  HiliteRect

// Given a rect and two hilite colors, this function frames the top and…
// left edges of the rect with color 1 and frames the bottom and right…
// sides with color 2.  A rect can be made to appear "hi-lit" or "3D"…
// in this way.

void HiliteRect (HDC hdc, Rect *theRect, SInt32 color1, SInt32 color2)
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

// Given a rectangle and color index, this function frames a…
// rectangle in that color.  Current port, pen mode, etc. assumed.

void ColorFrameRect (HDC hdc, Rect *theRect, SInt32 color)
{
	COLORREF	theRGBColor, wasColor;

	theRGBColor = Index2ColorRef(color);
	wasColor = SetDCBrushColor(hdc, theRGBColor);
	Mac_FrameRect(hdc, theRect, GetStockObject(DC_BRUSH), 1, 1);
	SetDCBrushColor(hdc, wasColor);
}

//--------------------------------------------------------------  ColorFrameWHRect

// Given a the top-left corner of a rectangle, its width and height,…
// and a color index, this function frames a rectangle in that color.
// Current port, pen mode, etc. assumed.

void ColorFrameWHRect (HDC hdc, SInt16 left, SInt16 top, SInt16 wide, SInt16 high,
		SInt32 color)
{
	Rect		theRect;

	theRect.left = left;
	theRect.top = top;
	theRect.right = left + wide;
	theRect.bottom = top + high;
	ColorFrameRect(hdc, &theRect, color);
}

//--------------------------------------------------------------  ColorFrameOval

// Given a rectangle and color index, this function frames an…
// oval in that color.  Current port, pen mode, etc. assumed.

void ColorFrameOval (HDC hdc, Rect *theRect, SInt32 color)
{
	COLORREF	theRGBColor, wasColor;
	HRGN		theRegion;

	theRGBColor = Index2ColorRef(color);
	wasColor = SetDCBrushColor(hdc, theRGBColor);
	theRegion = CreateEllipticRgn(theRect->left, theRect->top,
			theRect->right + 1, theRect->bottom + 1);
	FrameRgn(hdc, theRegion, GetStockObject(DC_BRUSH), 1, 1);
	DeleteObject(theRegion);
	SetDCBrushColor(hdc, wasColor);
}

//--------------------------------------------------------------  LtGrayForeColor

// This function finds the closest match to a "light gray" in the…
// current palette and sets the pen color to that.

COLORREF LtGrayForeColor (void)
{
	RGBColor	color;

	color.red = 0xBFFF;
	color.green = 0xBFFF;
	color.blue = 0xBFFF;

	return RGB(color.red >> 8, color.green >> 8, color.blue >> 8);
}

//--------------------------------------------------------------  GrayForeColor

// This function finds the closest match to a "medium gray" in the…
// current palette and sets the pen color to that.

COLORREF GrayForeColor (void)
{
	RGBColor	color;

	color.red = 0x7FFF;
	color.green = 0x7FFF;
	color.blue = 0x7FFF;

	return RGB(color.red >> 8, color.green >> 8, color.blue >> 8);
}

//--------------------------------------------------------------  DkGrayForeColor

// This function finds the closest match to a "dark gray" in the…
// current palette and sets the pen color to that.

COLORREF DkGrayForeColor (void)
{
	RGBColor	color;

	color.red = 0x3FFF;
	color.green = 0x3FFF;
	color.blue = 0x3FFF;

	return RGB(color.red >> 8, color.green >> 8, color.blue >> 8);
}

//--------------------------------------------------------------  RestoreColorsSlam

// This function forces the Macintosh to rebuild the palette.  It is…
// called to restore a sense or normality after some serious munging…
// with the palette.

void RestoreColorsSlam (void)
{
	return;
#if 0
	RestoreDeviceClut(nil);
	PaintBehind(nil, GetGrayRgn());
	DrawMenuBar();
#endif
}

//--------------------------------------------------------------  ColorRectShadow

// Similar to ColorRect, but every other pixel is set to the specified color
// to create a dithered shadow.

void ColorShadowRect (HDC hdc, Rect *theRect, SInt32 color)
{
	HRGN theRgn = CreateRectRgn(theRect->left, theRect->top,
			theRect->right, theRect->bottom);
	ColorShadowRegion(hdc, theRgn, color);
	DeleteObject(theRgn);
}

//--------------------------------------------------------------  ColorOvalShadow

// Similar to ColorOval, but every other pixel is set to the specified color
// to create a dithered shadow.

void ColorShadowOval (HDC hdc, Rect *theRect, SInt32 color)
{
	HRGN theRgn = CreateEllipticRgn(theRect->left, theRect->top,
			theRect->right + 1, theRect->bottom + 1);
	ColorShadowRegion(hdc, theRgn, color);
	DeleteObject(theRgn);
}

//--------------------------------------------------------------  ColorRegionShadow

// Similar to ColorRegion, but every other pixel is set to the specified color
// to create a dithered shadow.

void ColorShadowRegion (HDC hdc, HRGN theRgn, SInt32 color)
{
	const WORD	grayBits[8] = {
		0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA,
	};
	HBITMAP		hbmGray;
	HBRUSH		hbrShadow;

	// create the shadow bitmap and brush, and save the DC's settings
	hbmGray = CreateBitmap(8, 8, 1, 1, grayBits);
	hbrShadow = CreatePatternBrush(hbmGray);
	SaveDC(hdc);
	// set the black bits in the brush to white on the destination
	SetTextColor(hdc, RGB(0x00, 0x00, 0x00));
	SetBkColor(hdc, RGB(0xFF, 0xFF, 0xFF));
	SetROP2(hdc, R2_MERGENOTPEN); // DPno
	FillRgn(hdc, theRgn, hbrShadow);
	// set the black bits in the brush to the desired color on the destination
	SetTextColor(hdc, Index2ColorRef(color));
	SetROP2(hdc, R2_MASKPEN); // DPa
	FillRgn(hdc, theRgn, hbrShadow);
	// restore the DC's settings, and delete the shadow bitmap and brush
	RestoreDC(hdc, -1);
	DeleteObject(hbrShadow);
	DeleteObject(hbmGray);
}

