//============================================================================
//----------------------------------------------------------------------------
//                               DialogUtils.c
//----------------------------------------------------------------------------
//============================================================================

#include "DialogUtils.h"

#include "GliderDefines.h"
#include "StringUtils.h"
#include "Utilities.h"

#include <strsafe.h>

#include <stdlib.h>

static BOOL CALLBACK FormatWindowText (HWND hwnd, LPARAM lParam);
static INT_PTR CALLBACK AlertProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

//==============================================================  Functions
//--------------------------------------------------------------  FormatWindowText

static BOOL CALLBACK FormatWindowText (HWND hwnd, LPARAM lParam)
{
	const DialogParams *params = (const DialogParams *)lParam;
	PWSTR oldText, newText;
	int textLength;
	int i, oldTextSize;
	size_t newTextSize;
	PCWSTR theParam;
	PWSTR remainingText;
	size_t remainingSize;
	Boolean copyChar;

	oldText = NULL;
	newText = NULL;

	// Get the original window text
	textLength = GetWindowTextLength(hwnd);
	if (textLength <= 1)
		goto ending;
	oldTextSize = textLength + 1;
	oldText = (PWSTR)calloc(oldTextSize, sizeof(*oldText));
	if (oldText == NULL)
		goto ending;
	if (!GetWindowText(hwnd, oldText, oldTextSize))
		goto ending;
	oldText[textLength] = L'\0';

	// Calculate the size of the new window text
	newTextSize = oldTextSize;
	for (i = 0; i < textLength; i++)
	{
		if (oldText[i] == L'^')
		{
			switch (oldText[i + 1])
			{
			case L'0':
			case L'1':
			case L'2':
			case L'3':
				theParam = params->arg[oldText[i + 1] - L'0'];
				if (theParam == NULL)
				{
					theParam = L"";
				}
				newTextSize += wcslen(theParam);
				break;
			}
		}
	}

	// Format the window text
	newText = (PWSTR)calloc(newTextSize, sizeof(*newText));
	if (newText == NULL)
		goto ending;
	remainingText = newText;
	remainingSize = newTextSize;
	for (i = 0; i < textLength; i++)
	{
		copyChar = true;
		if (oldText[i] == L'^')
		{
			switch (oldText[i + 1])
			{
			case L'0':
			case L'1':
			case L'2':
			case L'3':
				theParam = params->arg[oldText[i + 1] - L'0'];
				if (theParam == NULL)
				{
					theParam = L"";
				}
				i++;
				StringCchCatEx(remainingText, remainingSize, theParam,
						&remainingText, &remainingSize, 0);
				copyChar = false;
				break;
			}
		}
		if (copyChar && remainingSize >= 2)
		{
			*(remainingText++) = oldText[i];
			remainingSize--;
		}
	}
	SetWindowText(hwnd, newText);
	
ending:
	free(newText);
	free(oldText);
	return TRUE;
}

//--------------------------------------------------------------  ParamDialogText

void ParamDialogText(HWND hDlg, const DialogParams *params)
{
	if (params == NULL)
		return;
	EnumChildWindows(hDlg, FormatWindowText, (LPARAM)params);
}

//--------------------------------------------------------------  AlertProc

static INT_PTR CALLBACK AlertProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		CenterDialogOverOwner(hDlg);
		ParamDialogText(hDlg, (const DialogParams *)lParam);
		if (FocusDefaultButton(hDlg))
			return FALSE;
		else
			return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) != 0 && LOWORD(wParam) != 0xFFFF)
		{
			if (LOWORD(wParam) != IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
			}
			else
			{
				if (GetDlgItem(hDlg, IDCANCEL) != NULL)
				{
					EndDialog(hDlg, IDCANCEL);
				}
				else if ((GetWindowLongPtr(hDlg, GWL_STYLE) & WS_SYSMENU) == WS_SYSMENU)
				{
					EndDialog(hDlg, IDCANCEL);
				}
			}
		}
		SetWindowLongPtr(hDlg, DWLP_MSGRESULT, 0);
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  Alert
// Given a dialog resource ID and four string parameters, this function
// displays the dialog with a standard modal alert box procedure.

SInt16 Alert (SInt16 dialogID, HWND ownerWindow, const DialogParams *params)
{
	return (SInt16)DialogBoxParam(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(dialogID), ownerWindow,
			AlertProc, (LPARAM)params);
}

//--------------------------------------------------------------  FocusDefaultButton
// Gives focus to the default button on the dialog. This button's ID is
// whatever is returned from sending the dialog a DM_GETDEFID message.
// This function returns TRUE if the focus was changed, or FALSE otherwise.

BOOL FocusDefaultButton (HWND hDlg)
{
	LRESULT defaultButtonId;
	HWND defaultButton;

	defaultButtonId = SendMessage(hDlg, DM_GETDEFID, 0, 0);
	if (HIWORD(defaultButtonId) == DC_HASDEFID)
	{
		defaultButton = GetDlgItem(hDlg, LOWORD(defaultButtonId));
		SendMessage(hDlg, WM_NEXTDLGCTL, (WPARAM)defaultButton, TRUE);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//--------------------------------------------------------------  CenterWindowOverRect

void CenterWindowOverRect (HWND hwnd, const RECT *targetRect)
{
	RECT windowRect;
	int windowWidth;
	int windowHeight;
	int targetWidth;
	int targetHeight;
	int newX;
	int newY;

	if (!GetWindowRect(hwnd, &windowRect))
	{
		return;
	}
	windowWidth = windowRect.right - windowRect.left;
	windowHeight = windowRect.bottom - windowRect.top;
	targetWidth = targetRect->right - targetRect->left;
	targetHeight = targetRect->bottom - targetRect->top;
	// Horizontal spacing is 50% left and 50% right.
	// Vertical spacing is 45% top and 55% bottom.
	newX = targetRect->left + ((targetWidth - windowWidth) / 2);
	newY = targetRect->top + ((targetHeight - windowHeight) * 9 / 20);
	SetWindowPos(hwnd, NULL, newX, newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

//--------------------------------------------------------------  CenterWindowOverOwner
// Centers the given window over its owner. If the window has no
// owner, then it is centered over the working area of the monitor
// where it is located.

void CenterWindowOverOwner (HWND hwnd)
{
	HWND hwndOwner;
	HMONITOR hMonitor;
	MONITORINFO monitorInfo;
	RECT targetRect;

	hwndOwner = GetWindow(hwnd, GW_OWNER);
	if (hwndOwner != NULL)
	{
		if (!GetWindowRect(hwndOwner, &targetRect))
		{
			return;
		}
	}
	else
	{
		hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		if (hMonitor == NULL)
		{
			return;
		}
		monitorInfo.cbSize = sizeof(monitorInfo);
		if (!GetMonitorInfo(hMonitor, &monitorInfo))
		{
			return;
		}
		targetRect = monitorInfo.rcWork;
	}
	CenterWindowOverRect(hwnd, &targetRect);
}

//--------------------------------------------------------------  CenterDialogOverOwner
// Similar to CenterWindowOverOwner, but also sends the DM_REPOSITION message
// to ensure that the dialog box is fully visible on the screen.

void CenterDialogOverOwner (HWND hDlg)
{
	CenterWindowOverOwner(hDlg);
	SendMessage(hDlg, DM_REPOSITION, 0, 0);
}

//--------------------------------------------------------------  GetDialogString
// Returns a string from a specific dialog item.

void GetDialogString (HWND theDialog, int item, StringPtr theString, int stringCapacity)
{
	wchar_t *itemText;
	wchar_t *tmpPtr;
	HWND dialogItem;
	int itemTextLength;

	if (stringCapacity <= 0)
		return;

	theString[0] = 0; // set to empty string
	dialogItem = GetDlgItem(theDialog, item);
	itemTextLength = GetWindowTextLength(dialogItem);
	if (itemTextLength <= 0)
		return;
	tmpPtr = (wchar_t *)calloc((size_t)itemTextLength + 1, sizeof(*tmpPtr));
	if (tmpPtr == NULL)
		return;
	GetWindowText(dialogItem, tmpPtr, itemTextLength + 1);
	itemText = WinToMacLineEndings(tmpPtr);
	free(tmpPtr);
	if (itemText == NULL)
		return;
	MacFromWinString(theString, stringCapacity, itemText);
	free(itemText);
}

//--------------------------------------------------------------  SetDialogString
// Sets a specific string to a specific dialog item.

void SetDialogString (HWND theDialog, int item, ConstStringPtr theString)
{
	wchar_t buffer[256];
	wchar_t *itemText;

	WinFromMacString(buffer, ARRAYSIZE(buffer), theString);
	itemText = MacToWinLineEndings(buffer);
	if (itemText != NULL)
	{
		SetDlgItemText(theDialog, item, itemText);
		free(itemText);
	}
}

//--------------------------------------------------------------  GetDialogStringLen
// Returns the length of a dialog item string (text).

int GetDialogStringLen (HWND theDialog, int item)
{
	Str255 theString;

	GetDialogString(theDialog, item, theString, ARRAYSIZE(theString));
	return theString[0];
}

//--------------------------------------------------------------  GetDialogItemRect
// Returns the bounding rectangle of the specified dialog item.

void GetDialogItemRect (HWND theDialog, int item, Rect *theRect)
{
	HWND hwndChild;
	RECT windowRect;

	if (theRect == NULL)
	{
		return;
	}
	theRect->left = 0;
	theRect->top = 0;
	theRect->right = 0;
	theRect->bottom = 0;

	if (theDialog == NULL)
	{
		return;
	}
	hwndChild = GetDlgItem(theDialog, item);
	if (hwndChild == NULL)
	{
		return;
	}
	if (!GetWindowRect(hwndChild, &windowRect))
	{
		return;
	}
	SetLastError(ERROR_SUCCESS);
	if (!MapWindowPoints(HWND_DESKTOP, theDialog, (POINT *)&windowRect, 2))
	{
		if (GetLastError() != ERROR_SUCCESS)
		{
			return;
		}
	}

	theRect->left = (SInt16)windowRect.left;
	theRect->top = (SInt16)windowRect.top;
	theRect->right = (SInt16)windowRect.right;
	theRect->bottom = (SInt16)windowRect.bottom;
}

//--------------------------------------------------------------  AddMenuToComboBox
// Replaces the items in a combo box with the items from an HMENU.
// The strings from the menu are the strings in the combo box, and the
// item IDs from the menu become the item data values in the combo box.

void AddMenuToComboBox (HWND theDialog, int whichItem, HMENU theMenu)
{
	HWND hwndCombo;
	int menuIndex, numItems;
	int comboIndex;
	wchar_t *itemString;
	MENUITEMINFO menuItemInfo;
	BOOL succeeded;

	if (theDialog == NULL || theMenu == NULL)
		return;

	hwndCombo = GetDlgItem(theDialog, whichItem);
	if (hwndCombo == NULL)
		return;

	numItems = GetMenuItemCount(theMenu);
	if (numItems < 0)
		return;

	SendMessage(hwndCombo, CB_RESETCONTENT, 0, 0);

	itemString = NULL;
	menuItemInfo.cbSize = sizeof(menuItemInfo);
	menuItemInfo.fMask = MIIM_ID | MIIM_STRING;
	for (menuIndex = 0; menuIndex < numItems; menuIndex++)
	{
		menuItemInfo.dwTypeData = NULL;
		succeeded = GetMenuItemInfo(theMenu, menuIndex, TRUE, &menuItemInfo);
		if (!succeeded)
			continue;
		itemString = (wchar_t *)calloc((size_t)menuItemInfo.cch + 1, sizeof(*itemString));
		if (itemString == NULL)
			RedAlert(kErrNoMemory);
		menuItemInfo.dwTypeData = itemString;
		menuItemInfo.cch += 1;
		succeeded = GetMenuItemInfo(theMenu, menuIndex, TRUE, &menuItemInfo);
		if (!succeeded)
		{
			free(itemString);
			itemString = NULL;
			continue;
		}
		comboIndex = (int)SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)itemString);
		SendMessage(hwndCombo, CB_SETITEMDATA, comboIndex, menuItemInfo.wID);
		free(itemString);
	}
}

//--------------------------------------------------------------  GetComboBoxMenuValue
// Returns which item is currently selected in a combo box menu.
// This returns the selected item's data, not the item's index.

void GetComboBoxMenuValue (HWND theDialog, int whichItem, SInt16 *value)
{
	HWND hwndCombo;
	int selectedIndex;

	if (theDialog == NULL || value == NULL)
		return;

	*value = -1;
	hwndCombo = GetDlgItem(theDialog, whichItem);
	if (hwndCombo == NULL)
		return;
	selectedIndex = (int)SendMessage(hwndCombo, CB_GETCURSEL, 0, 0);
	if (selectedIndex < 0)
		return;
	*value = (SInt16)SendMessage(hwndCombo, CB_GETITEMDATA, selectedIndex, 0);
}

//--------------------------------------------------------------  SetComboBoxMenuValue
// Forces a specific item to be selected in a combo box menu.
// The value is the item's data, not its index.

void SetComboBoxMenuValue (HWND theDialog, int whichItem, SInt16 value)
{
	HWND hwndCombo;
	int index, numItems;
	LRESULT itemData;

	if (theDialog == NULL)
		return;

	hwndCombo = GetDlgItem(theDialog, whichItem);
	if (hwndCombo == NULL)
		return;
	numItems = (int)SendMessage(hwndCombo, CB_GETCOUNT, 0, 0);
	for (index = 0; index < numItems; index++)
	{
		itemData = SendMessage(hwndCombo, CB_GETITEMDATA, index, 0);
		if (itemData == value)
		{
			SendMessage(hwndCombo, CB_SETCURSEL, index, 0);
			break;
		}
	}
}
