#include "Coordinates.h"

//============================================================================
//----------------------------------------------------------------------------
//                               Coordinates.c
//----------------------------------------------------------------------------
//============================================================================

#include "MainWindow.h"
#include "Marquee.h"
#include "Menu.h"
#include "ObjectEdit.h"
#include "ResourceIDs.h"
#include "Utilities.h"

#include <strsafe.h>

#define kHoriCoordItem		1001
#define kVertCoordItem		1002
#define kDistCoordItem		1003

INT_PTR CALLBACK CoordWindowProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void UpdateCoordWindow (void);

HWND g_coordWindow;
SInt16 g_isCoordH;
SInt16 g_isCoordV;
Boolean g_isCoordOpen;

static SInt16 g_coordH;
static SInt16 g_coordV;
static SInt16 g_coordD;

//==============================================================  Functions
//--------------------------------------------------------------  SetCoordinateHVD
// Given a horizontal, vertical and distance value, this function
// displays these values in the Coordinates window.

void SetCoordinateHVD (SInt16 h, SInt16 v, SInt16 d)
{
	if (COMPILEDEMO)
		return;

	if (h != -2)
		g_coordH = h;
	if (v != -2)
		g_coordV = v;
	if (d != -2)
		g_coordD = d;
	UpdateCoordWindow();
}

//--------------------------------------------------------------  DeltaCoordinateD
// When the user is dragging a handle (such as the height of a blower)
// this function can be called and passed the amount by which the user
// has changed the height (delta).  This function then displays it in
// the Coordinate window.

void DeltaCoordinateD (SInt16 d)
{
	if (COMPILEDEMO)
		return;

	g_coordD = d;
	UpdateCoordWindow();
}

//--------------------------------------------------------------  CoordWindowProc
// Handles messages for the Coordinate window with a dialog procedure.

INT_PTR CALLBACK CoordWindowProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	WINDOWPLACEMENT placement;

	switch (message)
	{
	case WM_INITDIALOG:
		return FALSE; // don't change the focused window

	case WM_CLOSE:
		ToggleCoordinateWindow();
		return TRUE;

	case WM_MOVE:
		placement.length = sizeof(placement);
		GetWindowPlacement(hDlg, &placement);
		g_isCoordH = (SInt16)placement.rcNormalPosition.left;
		g_isCoordV = (SInt16)placement.rcNormalPosition.top;
		return TRUE;

	case WM_CTLCOLORSTATIC:
		SetBkColor((HDC)wParam, GetSysColor(COLOR_BTNFACE));
		if (GetDlgCtrlID((HWND)lParam) == kDistCoordItem)
			SetTextColor((HDC)wParam, GetSysColor(COLOR_HOTLIGHT));
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
	WCHAR text[256];

	if (COMPILEDEMO)
		return;

	if (g_coordWindow == NULL)
		return;

	if (g_coordH != -1)
		StringCchPrintf(text, ARRAYSIZE(text), L"h: %ld", (long)g_coordH);
	else
		StringCchCopy(text, ARRAYSIZE(text), L"h: -");
	SetDlgItemText(g_coordWindow, kHoriCoordItem, text);

	if (g_coordV != -1)
		StringCchPrintf(text, ARRAYSIZE(text), L"v: %ld", (long)g_coordV);
	else
		StringCchCopy(text, ARRAYSIZE(text), L"v: -");
	SetDlgItemText(g_coordWindow, kVertCoordItem, text);

	if (g_coordD != -1)
		StringCchPrintf(text, ARRAYSIZE(text), L"d: %ld", (long)g_coordD);
	else
		StringCchCopy(text, ARRAYSIZE(text), L"d: -");
	SetDlgItemText(g_coordWindow, kDistCoordItem, text);
}

//--------------------------------------------------------------  OpenCoordWindow
// Brings up the Coordinate window.

void OpenCoordWindow (void)
{
	WINDOWPLACEMENT placement;
	SInt16 direction, dist;

	if (COMPILEDEMO)
		return;

	if (g_coordWindow == NULL)
	{
		g_coordWindow = CreateDialog(HINST_THISCOMPONENT,
				MAKEINTRESOURCE(kCoordinateWindowID),
				g_mainWindow, CoordWindowProc);

		if (g_coordWindow == NULL)
			RedAlert(kErrNoMemory);

//		if (OptionKeyDown())
//		{
//			g_isCoordH = qd.screenBits.bounds.right - 55;
//			g_isCoordV = 204;
//		}
		placement.length = sizeof(placement);
		GetWindowPlacement(g_coordWindow, &placement);
		OffsetRect(&placement.rcNormalPosition,
				-placement.rcNormalPosition.left,
				-placement.rcNormalPosition.top);
		OffsetRect(&placement.rcNormalPosition, g_isCoordH, g_isCoordV);
		placement.showCmd = SW_SHOWNOACTIVATE;
		SetWindowPlacement(g_coordWindow, &placement);

		g_coordH = -1;
		g_coordV = -1;
		g_coordD = -1;
		UpdateCoordWindow();

		if (g_objActive != kNoObjectSelected)
		{
			if (ObjectHasHandle(&direction, &dist))
				g_coordD = dist;
			SetCoordinateHVD(g_theMarquee.bounds.left, g_theMarquee.bounds.top, g_coordD);
		}
	}

	UpdateCoordinateCheckmark(true);
}

//--------------------------------------------------------------  CloseCoordWindow
// Closes and disposes of the Coordinate window.

void CloseCoordWindow (void)
{
	if (g_coordWindow != NULL)
	{
		DestroyWindow(g_coordWindow);
		g_coordWindow = NULL;
	}
	UpdateCoordinateCheckmark(false);
}

//--------------------------------------------------------------  ToggleCoordinateWindow
// Toggles the Coordinate windows state between open and closed.

void ToggleCoordinateWindow (void)
{
	if (COMPILEDEMO)
		return;

	if (g_coordWindow == NULL)
	{
		OpenCoordWindow();
		g_isCoordOpen = true;
	}
	else
	{
		CloseCoordWindow();
		g_isCoordOpen = false;
	}
}
