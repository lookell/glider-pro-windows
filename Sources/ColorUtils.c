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
	Mac_PaintRect(hdc, theRect, (HBRUSH)GetStockObject(DC_BRUSH));
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
	FillRgn(hdc, theRegion, (HBRUSH)GetStockObject(DC_BRUSH));
	DeleteObject(theRegion);
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
	FillRgn(hdc, theRgn, (HBRUSH)GetStockObject(DC_BRUSH));
	SetDCBrushColor(hdc, wasColor);
}

//--------------------------------------------------------------  ColorLine
// Given a the end points for a line and color index, this function
// draws a line in that color.  Current port, pen mode, etc. assumed.

void ColorLine (HDC hdc, SInt16 h0, SInt16 v0, SInt16 h1, SInt16 v1, SInt32 color)
{
	COLORREF theRGBColor, wasColor;
	HGDIOBJ wasPen;

	theRGBColor = Index2ColorRef(color);
	wasColor = SetDCPenColor(hdc, theRGBColor);
	wasPen = SelectObject(hdc, GetStockObject(DC_PEN));
	DrawInclusiveLine(hdc, h0, v0, h1, v1);
	SelectObject(hdc, wasPen);
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
	Mac_FrameRect(hdc, theRect, (HBRUSH)GetStockObject(DC_BRUSH), 1, 1);
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
	FrameRgn(hdc, theRegion, (HBRUSH)GetStockObject(DC_BRUSH), 1, 1);
	DeleteObject(theRegion);
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

void ColorShadowRect (HDC hdc, const Rect *theRect, SInt32 color)
{
	HRGN theRgn;

	if (theRect->left >= theRect->right || theRect->top >= theRect->bottom)
		return;

	theRgn = CreateRectRgn(theRect->left, theRect->top,
			theRect->right, theRect->bottom);
	ColorShadowRegion(hdc, theRgn, color);
	DeleteObject(theRgn);
}

//--------------------------------------------------------------  DitherShadowOval
// Similar to DitherShadowRect, but the shadow is drawn within the given oval.

void ColorShadowOval (HDC hdc, const Rect *theRect, SInt32 color)
{
	HRGN theRgn;

	if (theRect->left >= theRect->right || theRect->top >= theRect->bottom)
		return;

	theRgn = CreateEllipticRgn(theRect->left, theRect->top,
			theRect->right + 1, theRect->bottom + 1);
	ColorShadowRegion(hdc, theRgn, color);
	DeleteObject(theRgn);
}

//--------------------------------------------------------------  DitherShadowOval
// Similar to DitherShadowRect, but the shadow is drawn within the given polygon.

void ColorShadowPolygon (HDC hdc, const POINT *pointList, int pointCount, int fillMode, SInt32 color)
{
	HRGN theRgn;

	theRgn = CreatePolygonRgn(pointList, pointCount, fillMode);
	ColorShadowRegion(hdc, theRgn, color);
	DeleteObject(theRgn);
}

//--------------------------------------------------------------  DitherShadowRegion
// Similar to DitherShadowRect, but the shadow is drawn within the given region.
// The region's coordinates are presumed to be in logical units.

void ColorShadowRegion (HDC hdc, HRGN theRgn, SInt32 color)
{
	// NOTE: These shadows in the original game are only rendered in two colors:
	// RGB(0x00, 0x00, 0x00) and RGB(0x11, 0x11, 0x11). These colors were represented
	// as the Macintosh 8-bit palette indexes 255 and 254, respectively. To apply
	// these shadows, a bitwise OR was used to combine the shadow color index (255
	// or 254) with the existing color. The resulting color could only be 255 or 254.
	// The below code is not strictly equivalent, but is close enough without
	// restricting the game to use 8-bit DIB sections everywhere.

	HBRUSH shadowBrush;
	int wasROP2;

	shadowBrush = CreateDither50Brush(Index2ColorRef(color), RGB(0xFF, 0xFF, 0xFF));
	wasROP2 = SetROP2(hdc, R2_MASKPEN);
	FillRgn(hdc, theRgn, shadowBrush);
	SetROP2(hdc, wasROP2);
	DeleteObject(shadowBrush);
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

//--------------------------------------------------------------  FillInShadowBitmapData

typedef struct ShadowBitmapData
{
	BITMAPINFOHEADER infoHeader;
	RGBQUAD pixelData[8][8];
} ShadowBitmapData;

void FillInShadowBitmapData (ShadowBitmapData *bitmapData, COLORREF color1, COLORREF color2)
{
	RGBQUAD rgbQuad1;
	RGBQUAD rgbQuad2;
	size_t row;
	size_t col;

	rgbQuad1.rgbBlue = GetBValue(color1);
	rgbQuad1.rgbGreen = GetGValue(color1);
	rgbQuad1.rgbRed = GetRValue(color1);
	rgbQuad1.rgbReserved = 0x00;

	rgbQuad2.rgbBlue = GetBValue(color2);
	rgbQuad2.rgbGreen = GetGValue(color2);
	rgbQuad2.rgbRed = GetRValue(color2);
	rgbQuad2.rgbReserved = 0x00;

	bitmapData->infoHeader.biSize = sizeof(bitmapData->infoHeader);
	bitmapData->infoHeader.biWidth = 8;
	bitmapData->infoHeader.biHeight = -8; // NOTE: top-down bitmap
	bitmapData->infoHeader.biPlanes = 1;
	bitmapData->infoHeader.biBitCount = 32;
	bitmapData->infoHeader.biCompression = BI_RGB;
	bitmapData->infoHeader.biSizeImage = sizeof(bitmapData->pixelData);
	bitmapData->infoHeader.biXPelsPerMeter = 0;
	bitmapData->infoHeader.biYPelsPerMeter = 0;
	bitmapData->infoHeader.biClrUsed = 0;
	bitmapData->infoHeader.biClrImportant = 0;
	for (row = 0; row < 8; row++)
	{
		for (col = 0; col < 8; col++)
		{
			// NOTE: Adding row and col here gives the desired dither pattern.
			if (((row + col) % 2) == 0)
			{
				bitmapData->pixelData[row][col] = rgbQuad1;
			}
			else
			{
				bitmapData->pixelData[row][col] = rgbQuad2;
			}
		}
	}
}

//--------------------------------------------------------------  CreateDither50Brush
// Creates an 8-by-8 pattern brush with a dithered, equal mixing of the two given
// colors. The first color is placed at the top left, and alternates with the second
// color outwards across the entire square pattern.

HBRUSH CreateDither50Brush (COLORREF color1, COLORREF color2)
{
	ShadowBitmapData bitmapData;

	FillInShadowBitmapData(&bitmapData, color1, color2);
	return CreateDIBPatternBrushPt(&bitmapData, DIB_RGB_COLORS);
}
