
//============================================================================
//----------------------------------------------------------------------------
//								WindowUtils.c
//----------------------------------------------------------------------------
//============================================================================


#include "Externs.h"
#include "Environ.h"
#include "RectUtils.h"


#define kFloatingKind		2048
#define kMessageWindowTall	48


HWND	mssgWindow;


//==============================================================  Functions
//--------------------------------------------------------------  GetWindowTopLeft

// Returns the top left coordinate of the specified window.  Corrdinates…
// are (of course) global (local coordinates would always be (0, 0)).

void GetWindowLeftTop (HWND theWindow, SInt16 *left, SInt16 *top)
{
	RECT		bounds;

	*left = 0;
	*top = 0;

	if (theWindow != NULL)
	{
		GetClientRect(theWindow, &bounds);
		MapWindowPoints(theWindow, HWND_DESKTOP, (LPPOINT)&bounds, 2);
		*left = (SInt16)bounds.left;
		*top = (SInt16)bounds.top;
	}
}

//--------------------------------------------------------------  GetWindowRect

// Returns bounding rectangle of the specified window in global coords.

void Gp_GetWindowRect (HWND theWindow, Rect *bounds)
{
	RECT		theRect;

	if (theWindow != NULL)
	{
		GetClientRect(theWindow, &theRect);
		MapWindowPoints(theWindow, HWND_DESKTOP, (LPPOINT)&theRect, 2);
		bounds->left = (SInt16)theRect.left;
		bounds->top = (SInt16)theRect.top;
		bounds->right = (SInt16)theRect.right;
		bounds->bottom = (SInt16)theRect.bottom;
	}
}

//--------------------------------------------------------------  GetLocalWindowRect
// Returns bounding rectangle of the specified window in local coords.
// (When you just need its width and height.)

void GetLocalWindowRect (HWND theWindow, Rect *bounds)
{
	RECT		theRect;

	if (theWindow != NULL)
	{
		GetClientRect(theWindow, &theRect);
		bounds->left = (SInt16)theRect.left;
		bounds->top = (SInt16)theRect.top;
		bounds->right = (SInt16)theRect.right;
		bounds->bottom = (SInt16)theRect.bottom;
	}
}

//--------------------------------------------------------------  FlagWindowFloating
// Sets the specified window's windowKind field to my own kFloatingKind…
// variable.  This way I can examine a window later and determine if…
// it's supposed to "float" above all other windows.
/*
void FlagWindowFloating (WindowPtr theWindow)
{
	if (theWindow != nil)
	{
		((WindowPeek)theWindow)->windowKind = kFloatingKind;
		BringToFront(theWindow);
	}
}

//--------------------------------------------------------------  IsWindowFloating

// Tests a specific window to see if it is supposed to "float" above all…
// other windows.

Boolean	IsWindowFloating (WindowPtr theWindow)
{
	if (theWindow != nil)
	{
		return (((WindowPeek)theWindow)->windowKind == kFloatingKind);
	}
	else
		return (false);
}
*/
//--------------------------------------------------------------  OpenMessageWindow
// Brings up a simple message window.  Nice sort of utility function.
// Anytime you want a small, quick message to come up, call this.

void OpenMessageWindow (StringPtr title)
{
	return;
	#if 0
	Rect		mssgWindowRect;

	SetRect(&mssgWindowRect, 0, 0, 256, kMessageWindowTall);
	if (thisMac.hasColor)
		mssgWindow = NewCWindow(nil, &mssgWindowRect,
				title, false, noGrowDocProc, kPutInFront, false, 0L);
	else
		mssgWindow = NewWindow(nil, &mssgWindowRect,
				title, false, noGrowDocProc, kPutInFront, false, 0L);

	if (mssgWindow != nil)
	{
		ShowWindow(mssgWindow);
		SetPort((GrafPtr)mssgWindow);
		ClipRect(&mssgWindowRect);
		ForeColor(blackColor);
		BackColor(whiteColor);
		TextFont(systemFont);
	}
	#endif
}

//--------------------------------------------------------------  SetMessageWindowMessage

// For the above message window, this function displays a string of text…
// in the center of the window.

void SetMessageWindowMessage (StringPtr message)
{
	return;
	#if 0
	Rect		mssgWindowRect;

	if (mssgWindow != nil)
	{
		SetPort((GrafPtr)mssgWindow);
		SetRect(&mssgWindowRect, 0, 0, 256, kMessageWindowTall);
		InsetRect(&mssgWindowRect, 16, 16);
		EraseRect(&mssgWindowRect);
		MoveTo(mssgWindowRect.left, mssgWindowRect.bottom - 6);
		DrawString(message);
	}
	#endif
}

//--------------------------------------------------------------  SetMessageWindowColor

// For the above message window, this function sets the color of the message
// text displayed within the window. (This was originally done by calling the
// QuickDraw function ForeColor before calling SetMessageWindowMessage.)

void SetMessageTextColor (COLORREF textColor)
{
	// TODO: implement this
}

//--------------------------------------------------------------  CloseMessageWindow

// Closes the previously referred to "message window".

void CloseMessageWindow (void)
{
	return;
	#if 0
	if (mssgWindow != nil)
		DisposeWindow(mssgWindow);
	mssgWindow = nil;
	#endif
}

//--------------------------------------------------------------  CloseThisWindow

// Given a specific window, this function will close it and set the window…
// pointer to null.

void CloseThisWindow (HWND *theWindow)
{
	if (*theWindow != NULL)
		DestroyWindow(*theWindow);
	*theWindow = NULL;
}

