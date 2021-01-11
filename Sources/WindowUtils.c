//============================================================================
//----------------------------------------------------------------------------
//                               WindowUtils.c
//----------------------------------------------------------------------------
//============================================================================

#include "WindowUtils.h"

#include "DialogUtils.h"
#include "ResourceIDs.h"

#define kMessageItem		1001

static INT_PTR CALLBACK MessageWindowProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

//==============================================================  Functions
//--------------------------------------------------------------  OpenMessageWindow
// Brings up a simple message window.  Nice sort of utility function.
// Anytime you want a small, quick message to come up, call this.

HWND OpenMessageWindow (PCWSTR title, HWND hwndOwner)
{
	HWND mssgWindow;
	MSG msg;

	mssgWindow = CreateDialog(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kMessageWindowID),
		hwndOwner, MessageWindowProc);
	SetWindowText(mssgWindow, title);
	CenterDialogOverOwner(mssgWindow);
	if (hwndOwner != NULL)
	{
		EnableWindow(hwndOwner, FALSE);
	}
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

	return mssgWindow;
}

//--------------------------------------------------------------  MessageWindowProc

static INT_PTR CALLBACK MessageWindowProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	(void)hDlg;

	switch (message)
	{
	case WM_INITDIALOG:
		SetWindowLongPtr(hDlg, DWLP_USER, (LONG_PTR)(ULONG_PTR)GetSysColor(COLOR_BTNTEXT));
		return TRUE;

	case WM_CTLCOLORSTATIC:
	{
		HDC hdc = (HDC)wParam;
		HWND hwndStatic = (HWND)lParam;
		if (GetDlgCtrlID(hwndStatic) == kMessageItem)
		{
			COLORREF textColor = (COLORREF)(ULONG_PTR)GetWindowLongPtr(hDlg, DWLP_USER);
			SetTextColor(hdc, textColor);
			SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
			return (INT_PTR)GetSysColorBrush(COLOR_BTNFACE);
		}
		return FALSE;
	}
	}
	return FALSE;
}

//--------------------------------------------------------------  SetMessageWindowMessage
// For the above message window, this function displays a string of text
// in the center of the window.

void SetMessageWindowMessage (HWND mssgWindow, PCWSTR message)
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

void SetMessageTextColor (HWND mssgWindow, COLORREF textColor)
{
	MSG msg;

	SetWindowLongPtr(mssgWindow, DWLP_USER, (LONG_PTR)(ULONG_PTR)textColor);
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

void CloseMessageWindow (HWND mssgWindow)
{
	HWND hwndOwner;

	if (mssgWindow != NULL)
	{
		hwndOwner = GetWindow(mssgWindow, GW_OWNER);
		if (hwndOwner != NULL)
		{
			EnableWindow(hwndOwner, TRUE);
		}
		DestroyWindow(mssgWindow);
	}
}
