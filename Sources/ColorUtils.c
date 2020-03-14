
//============================================================================
//----------------------------------------------------------------------------
//								  ColorUtils.c
//----------------------------------------------------------------------------
//============================================================================


#include "Externs.h"
//#include <Palettes.h>
#include "Macintosh.h"


//==============================================================  Functions
//--------------------------------------------------------------  ColorText

// Given a string and a color index (index into the current palette),…
// this function draws text in that color.  It assumes the current port,…
// the current font, the current pen location, etc.

void ColorText (StringPtr theStr, SInt32 color)
{
	return;
#if 0
	RGBColor	theRGBColor, wasColor;

	GetForeColor(&wasColor);
	Index2Color(color, &theRGBColor);
	RGBForeColor(&theRGBColor);
	DrawString(theStr);
	RGBForeColor(&wasColor);
#endif
}

//--------------------------------------------------------------  ColorRect

// Given a rectangle and color index, this function draws a solid…
// rectangle in that color.  Current port, pen mode, etc. assumed.

void ColorRect (Rect *theRect, SInt32 color)
{
	return;
#if 0
	RGBColor	theRGBColor, wasColor;

	GetForeColor(&wasColor);
	Index2Color(color, &theRGBColor);
	RGBForeColor(&theRGBColor);
	PaintRect(theRect);
	RGBForeColor(&wasColor);
#endif
}

//--------------------------------------------------------------  ColorOval

// Given a rectangle and color index, this function draws a solid…
// oval in that color.  Current port, pen mode, etc. assumed.

void ColorOval (Rect *theRect, SInt32 color)
{
	return;
#if 0
	RGBColor	theRGBColor, wasColor;

	GetForeColor(&wasColor);
	Index2Color(color, &theRGBColor);
	RGBForeColor(&theRGBColor);
	PaintOval(theRect);
	RGBForeColor(&wasColor);
#endif
}

//--------------------------------------------------------------  ColorRegion

// Given a region and color index, this function draws a solid…
// region in that color.  Current port, pen mode, etc. assumed.

void ColorRegion (RgnHandle theRgn, SInt32 color)
{
	return;
#if 0
	RGBColor	theRGBColor, wasColor;

	GetForeColor(&wasColor);
	Index2Color(color, &theRGBColor);
	RGBForeColor(&theRGBColor);
	PaintRgn(theRgn);
	RGBForeColor(&wasColor);
#endif
}

//--------------------------------------------------------------  ColorLine

// Given a the end points for a line and color index, this function…
// draws a line in that color.  Current port, pen mode, etc. assumed.

void ColorLine (SInt16 h0, SInt16 v0, SInt16 h1, SInt16 v1, SInt32 color)
{
	return;
#if 0
	RGBColor	theRGBColor, wasColor;

	GetForeColor(&wasColor);
	Index2Color(color, &theRGBColor);
	RGBForeColor(&theRGBColor);
	MoveTo(h0, v0);
	LineTo(h1, v1);
	RGBForeColor(&wasColor);
#endif
}

//--------------------------------------------------------------  HiliteRect

// Given a rect and two hilite colors, this function frames the top and…
// left edges of the rect with color 1 and frames the bottom and right…
// sides with color 2.  A rect can be made to appear "hi-lit" or "3D"…
// in this way.

void HiliteRect (Rect *theRect, SInt16 color1, SInt16 color2)
{
	return;
#if 0
	ColorLine(theRect->left, theRect->top, theRect->right - 2,
			theRect->top, color1);
	ColorLine(theRect->left, theRect->top, theRect->left,
			theRect->bottom - 2, color1);
	ColorLine(theRect->right - 1, theRect->top, theRect->right - 1,
			theRect->bottom - 2, color2);
	ColorLine(theRect->left + 1, theRect->bottom - 1, theRect->right - 1,
			theRect->bottom - 1, color2);
#endif
}

//--------------------------------------------------------------  ColorFrameRect

// Given a rectangle and color index, this function frames a…
// rectangle in that color.  Current port, pen mode, etc. assumed.

void ColorFrameRect (Rect *theRect, SInt32 color)
{
	return;
#if 0
	RGBColor	theRGBColor, wasColor;

	GetForeColor(&wasColor);
	Index2Color(color, &theRGBColor);
	RGBForeColor(&theRGBColor);
	FrameRect(theRect);
	RGBForeColor(&wasColor);
#endif
}

//--------------------------------------------------------------  ColorFrameWHRect

// Given a the top-left corner of a rectangle, its width and height,…
// and a color index, this function frames a rectangle in that color.
// Current port, pen mode, etc. assumed.

void ColorFrameWHRect (SInt16 left, SInt16 top, SInt16 wide, SInt16 high, SInt32 color)
{
	return;
#if 0
	Rect		theRect;

	theRect.left = left;
	theRect.top = top;
	theRect.right = left + wide;
	theRect.bottom = top + high;
	ColorFrameRect(&theRect, color);
#endif
}

//--------------------------------------------------------------  ColorFrameOval

// Given a rectangle and color index, this function frames an…
// oval in that color.  Current port, pen mode, etc. assumed.

void ColorFrameOval (Rect *theRect, SInt32 color)
{
	return;
#if 0
	RGBColor	theRGBColor, wasColor;

	GetForeColor(&wasColor);
	Index2Color(color, &theRGBColor);
	RGBForeColor(&theRGBColor);
	FrameOval(theRect);
	RGBForeColor(&wasColor);
#endif
}

//--------------------------------------------------------------  LtGrayForeColor

// This function finds the closest match to a "light gray" in the…
// current palette and sets the pen color to that.

void LtGrayForeColor (void)
{
	return;
#if 0
	RGBColor	color;

	color.red = (unsigned short) 0xBFFF;
	color.green = (unsigned short) 0xBFFF;
	color.blue = (unsigned short) 0xBFFF;

	RGBForeColor(&color);
#endif
}

//--------------------------------------------------------------  GrayForeColor

// This function finds the closest match to a "medium gray" in the…
// current palette and sets the pen color to that.

void GrayForeColor (void)
{
	return;
#if 0
	RGBColor	color;

	color.red = (unsigned short) 0x7FFF;
	color.green = (unsigned short) 0x7FFF;
	color.blue = (unsigned short) 0x7FFF;

	RGBForeColor(&color);
#endif
}

//--------------------------------------------------------------  DkGrayForeColor

// This function finds the closest match to a "dark gray" in the…
// current palette and sets the pen color to that.

void DkGrayForeColor (void)
{
	return;
#if 0
	RGBColor	color;

	color.red = (unsigned short) 0x3FFF;
	color.green = (unsigned short) 0x3FFF;
	color.blue = (unsigned short) 0x3FFF;

	RGBForeColor(&color);
#endif
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

