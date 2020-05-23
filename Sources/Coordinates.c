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
#include "ResourceIDs.h"


#define kHoriCoordItem		1001
#define kVertCoordItem		1002
#define kDistCoordItem		1003


INT_PTR CALLBACK CoordWindowProc(HWND, UINT, WPARAM, LPARAM);


HWND			coordWindow;
SInt16			isCoordH, isCoordV;
SInt16			coordH, coordV, coordD;
Boolean			isCoordOpen;


//==============================================================  Functions
//--------------------------------------------------------------  SetCoordinateHVD

// Given a horizontal, vertical and distance value, this function…
// displays these values in the Coordinates window.

void SetCoordinateHVD (SInt16 h, SInt16 v, SInt16 d)
{
#ifndef COMPILEDEMO
	if (h != -2)
		coordH = h;
	if (v != -2)
		coordV = v;
	if (d != -2)
		coordD = d;
	UpdateCoordWindow();
#endif
}

//--------------------------------------------------------------  DeltaCoordinateD

// When the user is dragging a handle (such as the height of a blower)…
// this function can be called and passed the amount by which the user…
// has changed the height (delta).  This function then displays it in…
// the Coordinate window.

void DeltaCoordinateD (SInt16 d)
{
#ifndef COMPILEDEMO
	coordD = d;
	UpdateCoordWindow();
#endif
}

//--------------------------------------------------------------  CoordWindowProc

// Handles messages for the Coordinate window with a dialog procedure.

INT_PTR CALLBACK CoordWindowProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_CLOSE:
		ToggleCoordinateWindow();
		return TRUE;

	case WM_MOVE:
		isCoordH = GET_X_LPARAM(lParam);
		isCoordV = GET_Y_LPARAM(lParam);
		return TRUE;

	case WM_CTLCOLORSTATIC:
		SetBkColor((HDC)wParam, GetSysColor(COLOR_BTNFACE));
		if (GetDlgCtrlID((HWND)lParam) == kDistCoordItem)
			SetTextColor((HDC)wParam, blueColor);
		else
			SetTextColor((HDC)wParam, GetSysColor(COLOR_BTNTEXT));
		return (INT_PTR)GetSysColorBrush(COLOR_BTNFACE);
	}
	return FALSE;
}

//--------------------------------------------------------------  UpdateCoordWindow

// Completely redraws and updates the Coordinate window.

void UpdateCoordWindow (void)
{
#ifndef COMPILEDEMO
	WCHAR text[256];

	if (coordWindow == NULL)
		return;

	if (coordH != -1)
		StringCchPrintf(text, ARRAYSIZE(text), L"h: %ld", (long)coordH);
	else
		StringCchCopy(text, ARRAYSIZE(text), L"h: -");
	SetDlgItemText(coordWindow, kHoriCoordItem, text);

	if (coordV != -1)
		StringCchPrintf(text, ARRAYSIZE(text), L"v: %ld", (long)coordV);
	else
		StringCchCopy(text, ARRAYSIZE(text), L"v: -");
	SetDlgItemText(coordWindow, kVertCoordItem, text);

	if (coordD != -1)
		StringCchPrintf(text, ARRAYSIZE(text), L"d: %ld", (long)coordD);
	else
		StringCchCopy(text, ARRAYSIZE(text), L"d: -");
	SetDlgItemText(coordWindow, kDistCoordItem, text);
#endif
}

//--------------------------------------------------------------  OpenCoordWindow
// Brings up the Coordinate window.

void OpenCoordWindow (void)
{
#ifndef COMPILEDEMO
	Rect		src, dest;
	Point		globalMouse;
	short		direction, dist;

	if (coordWindow == NULL)
	{
		coordWindow = CreateDialog(HINST_THISCOMPONENT,
				MAKEINTRESOURCE(kCoordinateWindowID),
				mainWindow, CoordWindowProc);

		if (coordWindow == NULL)
			RedAlert(kErrNoMemory);

//		if (OptionKeyDown())
//		{
//			isCoordH = qd.screenBits.bounds.right - 55;
//			isCoordV = 204;
//		}
		SetWindowPos(coordWindow, NULL, isCoordH, isCoordV, 0, 0,
				SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
		ShowWindow(coordWindow, SW_SHOW);

		coordH = -1;
		coordV = -1;
		coordD = -1;
		UpdateCoordWindow();

		if (objActive != kNoObjectSelected)
		{
			if (ObjectHasHandle(&direction, &dist))
				coordD = dist;
			SetCoordinateHVD(theMarquee.bounds.left, theMarquee.bounds.top, coordD);
		}
	}

	UpdateCoordinateCheckmark(true);
#endif
}

//--------------------------------------------------------------  CloseCoordWindow
// Closes and disposes of the Coordinate window.

void CloseCoordWindow (void)
{
	CloseThisWindow(&coordWindow);
	UpdateCoordinateCheckmark(false);
}

//--------------------------------------------------------------  ToggleCoordinateWindow
// Toggles the Coordinate windows state between open and closed.

void ToggleCoordinateWindow (void)
{
#ifndef COMPILEDEMO
	if (coordWindow == NULL)
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
}

