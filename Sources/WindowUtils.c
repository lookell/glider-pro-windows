#include "WindowUtils.h"

//============================================================================
//----------------------------------------------------------------------------
//                               WindowUtils.c
//----------------------------------------------------------------------------
//============================================================================


#include "DialogUtils.h"
#include "Environ.h"
#include "Externs.h"
#include "Macintosh.h"
#include "RectUtils.h"
#include "ResourceIDs.h"


#define kFloatingKind		2048

#define kMessageItem		1001


static INT_PTR CALLBACK MessageWindowProc (HWND, UINT, WPARAM, LPARAM);


static HWND mssgWindow;
static COLORREF mssgTextColor;


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
	MSG msg;
	WCHAR windowTitle[256];

	WinFromMacString(windowTitle, ARRAYSIZE(windowTitle), title);
	mssgTextColor = GetSysColor(COLOR_WINDOWTEXT);
	mssgWindow = CreateDialog(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(kMessageWindowID),
			mainWindow, MessageWindowProc);
	SetWindowText(mssgWindow, windowTitle);
	CenterOverOwner(mssgWindow);
	EnableWindow(mainWindow, FALSE);
	ShowWindow(mssgWindow, SW_SHOW);

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			PostQuitMessage((int)msg.wParam);
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

//--------------------------------------------------------------  MessageWindowProc

static INT_PTR CALLBACK MessageWindowProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_CTLCOLORSTATIC:
	{
		HDC hdc = (HDC)wParam;
		HWND hwndStatic = (HWND)lParam;
		if (GetDlgCtrlID(hwndStatic) == kMessageItem)
		{
			SetTextColor(hdc, mssgTextColor);
			SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
			return (INT_PTR)GetSysColorBrush(COLOR_BTNFACE);
		}
		return FALSE;
	}
	}
	return FALSE;
}

//--------------------------------------------------------------  SetMessageWindowMessage

// For the above message window, this function displays a string of text…
// in the center of the window.

void SetMessageWindowMessage (StringPtr message)
{
	WCHAR messageText[256];
	MSG msg;

	WinFromMacString(messageText, ARRAYSIZE(messageText), message);
	SetDlgItemText(mssgWindow, kMessageItem, messageText);

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			PostQuitMessage((int)msg.wParam);
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

//--------------------------------------------------------------  SetMessageWindowColor

// For the above message window, this function sets the color of the message
// text displayed within the window. (This was originally done by calling the
// QuickDraw function ForeColor before calling SetMessageWindowMessage.)

void SetMessageTextColor (COLORREF textColor)
{
	MSG msg;

	mssgTextColor = textColor;
	InvalidateRect(GetDlgItem(mssgWindow, kMessageItem), NULL, TRUE);

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			PostQuitMessage((int)msg.wParam);
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

//--------------------------------------------------------------  CloseMessageWindow

// Closes the previously referred to "message window".

void CloseMessageWindow (void)
{
	if (mssgWindow != NULL)
	{
		EnableWindow(mainWindow, TRUE);
		DestroyWindow(mssgWindow);
		mssgWindow = NULL;
	}
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

