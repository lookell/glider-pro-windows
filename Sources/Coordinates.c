//============================================================================
//----------------------------------------------------------------------------
//								  Coordinates.c
//----------------------------------------------------------------------------
//============================================================================


//#include <NumberFormatting.h>
#include "Macintosh.h"
#include "Externs.h"
#include "Environ.h"
#include "Marquee.h"
#include "ObjectEdit.h"
#include "RectUtils.h"


Rect			coordWindowRect;
WindowPtr		coordWindow;
short			isCoordH, isCoordV;
short			coordH, coordV, coordD;
Boolean			isCoordOpen;


//==============================================================  Functions
//--------------------------------------------------------------  SetCoordinateHVD

// Given a horizontal, vertical and distance value, this function…
// displays these values in the Coordinates window.

void SetCoordinateHVD (SInt16 h, SInt16 v, SInt16 d)
{
	return;
#if 0
#ifndef COMPILEDEMO
	if (h != -2)
		coordH = h;
	if (v != -2)
		coordV = v;
	if (d != -2)
		coordD = d;
	UpdateCoordWindow();
#endif
#endif
}

//--------------------------------------------------------------  DeltaCoordinateD

// When the user is dragging a handle (such as the height of a blower)…
// this function can be called and passed the amount by which the user…
// has changed the height (delta).  This function then displays it in…
// the Coordinate window.

void DeltaCoordinateD (SInt16 d)
{
	return;
#if 0
#ifndef COMPILEDEMO
	coordD = d;
	UpdateCoordWindow();
#endif
#endif
}

//--------------------------------------------------------------  UpdateCoordWindow

// Completely redraws and updates the Coordinate window.

void UpdateCoordWindow (void)
{
	return;
#if 0
#ifndef COMPILEDEMO
	Str255		tempStr, numStr;
	GrafPtr		wasPort;

	if (coordWindow == nil)
		return;

	GetPort(&wasPort);
	SetPort((GrafPtr)coordWindow);
	EraseRect(&coordWindowRect);

	PasStringCopy("\ph: ", tempStr);
	if (coordH != -1)
	{
		NumToString((long)coordH, numStr);
		PasStringConcat(tempStr, numStr);
	}
	else
		PasStringConcat(tempStr, "\p-");
	MoveTo(5, 12);
	DrawString(tempStr);

	PasStringCopy("\pv: ", tempStr);
	if (coordV != -1)
	{
		NumToString((long)coordV, numStr);
		PasStringConcat(tempStr, numStr);
	}
	else
		PasStringConcat(tempStr, "\p-");
	MoveTo(4, 22);
	DrawString(tempStr);

	ForeColor(blueColor);
	PasStringCopy("\pd: ", tempStr);
	if (coordD != -1)
	{
		NumToString((long)coordD, numStr);
		PasStringConcat(tempStr, numStr);
	}
	else
		PasStringConcat(tempStr, "\p-");
	MoveTo(5, 32);
	DrawString(tempStr);
	ForeColor(blackColor);

	SetPort((GrafPtr)wasPort);
#endif
#endif
}

//--------------------------------------------------------------  OpenCoordWindow
// Brings up the Coordinate window.

void OpenCoordWindow (void)
{
	return;
#if 0
#ifndef COMPILEDEMO
	Rect		src, dest;
	Point		globalMouse;
	short		direction, dist;

	if (coordWindow == nil)
	{
		QSetRect(&coordWindowRect, 0, 0, 50, 38);
		if (thisMac.hasColor)
			coordWindow = NewCWindow(nil, &coordWindowRect,
					"\pTools", false, kWindoidWDEF, kPutInFront, true, 0L);
		else
			coordWindow = NewWindow(nil, &coordWindowRect,
					"\pTools", false, kWindoidWDEF, kPutInFront, true, 0L);

		if (coordWindow == nil)
			RedAlert(kErrNoMemory);

//		if (OptionKeyDown())
//		{
//			isCoordH = qd.screenBits.bounds.right - 55;
//			isCoordV = 204;
//		}
		MoveWindow(coordWindow, isCoordH, isCoordV, true);
		globalMouse = MyGetGlobalMouse();
		QSetRect(&src, 0, 0, 1, 1);
		QOffsetRect(&src, globalMouse.h, globalMouse.v);
		GetWindowRect(coordWindow, &dest);
		BringToFront(coordWindow);
		ShowHide(coordWindow, true);
//		FlagWindowFloating(coordWindow);	TEMP - use flaoting windows
		HiliteAllWindows();

		coordH = -1;
		coordV = -1;
		coordD = -1;
		TextFace(applFont);
		TextSize(9);

		if (objActive != kNoObjectSelected)
		{
			if (ObjectHasHandle(&direction, &dist))
				coordD = dist;
			SetCoordinateHVD(theMarquee.bounds.left, theMarquee.bounds.top, coordD);
		}
	}

	UpdateCoordinateCheckmark(true);
#endif
#endif
}

//--------------------------------------------------------------  CloseCoordWindow
// Closes and disposes of the Coordinate window.

void CloseCoordWindow (void)
{
	return;
#if 0
	CloseThisWindow(&coordWindow);
	UpdateCoordinateCheckmark(false);
#endif
}

//--------------------------------------------------------------  ToggleCoordinateWindow
// Toggles the Coordinate windows state between open and closed.

void ToggleCoordinateWindow (void)
{
	return;
#if 0
#ifndef COMPILEDEMO
	if (coordWindow == nil)
	{
		OpenCoordWindow();
		isCoordOpen = true;
	}
	else
	{
		CloseCoordWindow();
		isCoordOpen = false;
	}
#endif
#endif
}

