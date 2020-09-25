#define GP_USE_WINAPI_H

#include "WindowUtils.h"

//============================================================================
//----------------------------------------------------------------------------
//                               WindowUtils.c
//----------------------------------------------------------------------------
//============================================================================


#include "DialogUtils.h"
#include "MacTypes.h"
#include "MainWindow.h"
#include "RectUtils.h"
#include "ResourceIDs.h"
#include "StringUtils.h"


#define kMessageItem		1001


static INT_PTR CALLBACK MessageWindowProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


static HWND mssgWindow;
static COLORREF mssgTextColor;


//==============================================================  Functions
//--------------------------------------------------------------  OpenMessageWindow
// Brings up a simple message window.  Nice sort of utility function.
// Anytime you want a small, quick message to come up, call this.

void OpenMessageWindow (PCWSTR title)
{
	MSG msg;

	mssgTextColor = GetSysColor(COLOR_WINDOWTEXT);
	mssgWindow = CreateDialog(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kMessageWindowID),
		mainWindow, MessageWindowProc);
	SetWindowText(mssgWindow, title);
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
	(void)hDlg;

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

void SetMessageWindowMessage (PCWSTR message)
{
	MSG msg;

	SetDlgItemText(mssgWindow, kMessageItem, message);

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

