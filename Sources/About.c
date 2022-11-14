//============================================================================
//----------------------------------------------------------------------------
//                                  About.c
//----------------------------------------------------------------------------
//============================================================================

#include "About.h"

#include "DialogUtils.h"
#include "Environ.h"
#include "ResourceIDs.h"

#include <strsafe.h>

static void UpdateMainPict (HWND hDlg);
static INT_PTR CALLBACK AboutFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

//==============================================================  Functions
//--------------------------------------------------------------  DoAbout
// Brings up the About dialog box.

void DoAbout (HWND ownerWindow)
{
	DialogBox(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(kAboutDialogID),
			ownerWindow, AboutFilter);
}

//==============================================================  Static Functions
//--------------------------------------------------------------  UpdateMainPict
// Redraws the main graphic in the dialog (in response to an update event).

static void UpdateMainPict (HWND hDlg)
{
	WCHAR theStr[100];
	MEMORYSTATUSEX memoryStatus;
	unsigned long freeKilobytes;
	long screenWidth, screenHeight, screenDepth;

	memoryStatus.dwLength = sizeof(memoryStatus);
	if (GlobalMemoryStatusEx(&memoryStatus))
	{
		freeKilobytes = (unsigned long)(memoryStatus.ullAvailPhys / 1024);
		StringCchPrintf(theStr, ARRAYSIZE(theStr), L"Memory: %luK\n", freeKilobytes);
	}
	else
	{
		StringCchCopy(theStr, ARRAYSIZE(theStr), L"Memory: (unknown)");
	}
	SetDlgItemText(hDlg, kFreeMemoryText, theStr);

	screenWidth = g_thisMac.screen.right - g_thisMac.screen.left;
	screenHeight = g_thisMac.screen.bottom - g_thisMac.screen.top;
	screenDepth = g_thisMac.isDepth;
	StringCchPrintf(theStr, ARRAYSIZE(theStr), L"Screen: %ldx%ldx%ld",
			screenWidth, screenHeight, screenDepth);
	SetDlgItemText(hDlg, kScreenInfoText, theStr);
}

//--------------------------------------------------------------  AboutFilter
// Dialog filter for the About dialog.

static INT_PTR CALLBACK AboutFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	const UINT_PTR ABOUT_TIMER_ID = 1;

	(void)lParam;

	switch (message)
	{
	case WM_INITDIALOG:
		CenterDialogOverOwner(hDlg);
		UpdateMainPict(hDlg);
		SetTimer(hDlg, ABOUT_TIMER_ID, 2000, NULL);
		return TRUE;

	case WM_DESTROY:
		KillTimer(hDlg, ABOUT_TIMER_ID);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			break;
		}
		return TRUE;

	case WM_TIMER:
		if (wParam == ABOUT_TIMER_ID)
			UpdateMainPict(hDlg);
		return TRUE;
	}
	return FALSE;
}
