#define GP_USE_WINAPI_H

#include "ObjectInfo.h"

//============================================================================
//----------------------------------------------------------------------------
//                                ObjectInfo.c
//----------------------------------------------------------------------------
//============================================================================


#include "DialogUtils.h"
#include "GliderStructs.h"
#include "House.h"
#include "HouseIO.h"
#include "HouseLegal.h"
#include "Link.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "Menu.h"
#include "ObjectAdd.h"
#include "ObjectEdit.h"
#include "Objects.h"
#include "RectUtils.h"
#include "ResourceIDs.h"
#include "Room.h"

#include <strsafe.h>


#define kBlowerInitialState     1006
#define kForceCheckbox          1007
#define kBlowerArrow            1008
#define kDirectionText          1009
#define kBlowerUpButton         1011
#define kBlowerRightButton      1012
#define kBlowerDownButton       1013
#define kBlowerLeftButton       1014
#define kBlowerLinkedFrom       1015
#define kLeftFacingRadio        1016
#define kRightFacingRadio       1017

#define kFurnitureLinkedFrom    1006

#define kCustPictIDItem         1007

#define kToggleRadio            1006
#define kForceOnRadio           1007
#define kForceOffRadio          1008
#define kLinkSwitchButton       1009
#define kSwitchGotoButton       1014
#define kSwitchLinkedFrom       1015

#define kTriggerDelayItem       1006
#define kLinkTriggerButton      1009
#define kTriggerGotoButton      1014
#define kTriggerLinkedFrom      1015

#define kLightInitialState      1006
#define kLightLinkedFrom        1008

#define kApplianceInitialState  1006
#define kApplianceDelay         1008
#define kApplianceDelayLabel    1009
#define kApplianceLinkedFrom    1010

#define kMicrowaveInitialState  1006
#define kKillBandsCheckbox      1008
#define kKillBatteryCheckbox    1009
#define kKillFoilCheckbox       1010
#define kMicrowaveLinkedFrom    1011

#define kGreaseSpilled          1006
#define kGreaseLinkedFrom       1008

#define k100PtRadio             1006
#define k300PtRadio             1007
#define k500PtRadio             1008
#define kInvisBonusLinkedFrom   1009

#define kLinkTransButton        1006
#define kTransGotoButton        1011
#define kTransLinkedFrom        1012
#define kTransInitialState      1013

#define kEnemyDelayItem         1007
#define kEnemyDelayLabelItem    1008
#define kEnemyInitialState      1010
#define kEnemyLinkedFrom        1011

#define kRadioFlower1           1006
#define kRadioFlower2           1007
#define kRadioFlower3           1008
#define kRadioFlower4           1009
#define kRadioFlower5           1010
#define kRadioFlower6           1011
#define kFlowerLinkedFrom       1013


void UpdateBlowerInfo (HWND hDlg, HDC hdc);
INT_PTR CALLBACK BlowerFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK FurnitureFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK CustPictFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK SwitchFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK TriggerFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK LightFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ApplianceFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK MicrowaveFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK GreaseFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK InvisBonusFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK TransFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK EnemyFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK FlowerFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void DoBlowerObjectInfo (HWND hwndOwner);
void DoFurnitureObjectInfo (HWND hwndOwner);
void DoCustPictObjectInfo (HWND hwndOwner);
void DoSwitchObjectInfo (HWND hwndOwner);
void DoTriggerObjectInfo (HWND hwndOwner);
void DoLightObjectInfo (HWND hwndOwner);
void DoApplianceObjectInfo (HWND hwndOwner);
void DoMicrowaveObjectInfo (HWND hwndOwner);
void DoGreaseObjectInfo (HWND hwndOwner);
void DoInvisBonusObjectInfo (HWND hwndOwner);
void DoTransObjectInfo (HWND hwndOwner);
void DoEnemyObjectInfo (HWND hwndOwner);
void DoFlowerObjectInfo (HWND hwndOwner);


//==============================================================  Functions
//--------------------------------------------------------------  GetObjectName

void GetObjectName (wchar_t *pszDest, size_t cchDest, SInt16 objectType)
{
	UINT strID;
	LPCWSTR strPtr;
	int strLen;

	if (pszDest == NULL || cchDest < 1)
		return;
	pszDest[0] = L'\0';

	strID = (UINT)(kObjectNameStringsBase + objectType);
	strLen = LoadString(HINST_THISCOMPONENT, strID, (LPWSTR)&strPtr, 0);
	if (strLen <= 0 || strPtr == NULL)
		return;
	StringCchCopyN(pszDest, cchDest, strPtr, (size_t)strLen);
}

//--------------------------------------------------------------  UpdateBlowerInfo

void UpdateBlowerInfo (HWND hDlg, HDC hdc)
{
	const LONG kArrowheadLength = 4;
	RECT bounds;
	LONG centerHori, centerVert;
	HPEN hPen, oldPen;
	HWND focusedWindow;

	if ((thisRoom->objects[objActive].what != kLeftFan) &&
		(thisRoom->objects[objActive].what != kRightFan))
	{
		GetWindowRect(GetDlgItem(hDlg, kBlowerArrow), &bounds);
		MapWindowPoints(HWND_DESKTOP, hDlg, (POINT *)&bounds, 2);

		FillRect(hdc, &bounds, GetSysColorBrush(COLOR_BTNFACE));

		// The bounds are deflated here to allow a focus rectangle to be drawn
		// later, when it is appropriate to do so, without the focus rectangle
		// overlapping the arrow.
		InflateRect(&bounds, -1, -1);
		centerHori = bounds.left + (bounds.right - bounds.left) / 2;
		centerVert = bounds.top + (bounds.bottom - bounds.top) / 2;

		hPen = CreatePen(PS_SOLID, 2, GetSysColor(COLOR_WINDOWTEXT));
		oldPen = (HPEN)SelectObject(hdc, hPen);

		// Draw an arrow pointing in the currently selected direction
		if (IsDlgButtonChecked(hDlg, kBlowerUpButton))
		{
			MoveToEx(hdc, centerHori, bounds.top, NULL);
			LineTo(hdc, centerHori, bounds.bottom - 1);
			MoveToEx(hdc, centerHori, bounds.top + 1, NULL);
			LineTo(hdc, centerHori + kArrowheadLength, bounds.top + 1 + kArrowheadLength);
			MoveToEx(hdc, centerHori - 1, bounds.top + 1, NULL);
			LineTo(hdc, centerHori - 1 - kArrowheadLength, bounds.top + 1 + kArrowheadLength);
		}
		else if (IsDlgButtonChecked(hDlg, kBlowerRightButton))
		{
			MoveToEx(hdc, bounds.right - 1, centerVert, NULL);
			LineTo(hdc, bounds.left, centerVert);
			MoveToEx(hdc, bounds.right - 2, centerVert, NULL);
			LineTo(hdc, bounds.right - 2 - kArrowheadLength, centerVert + kArrowheadLength);
			MoveToEx(hdc, bounds.right - 1, centerVert, NULL);
			LineTo(hdc, bounds.right - 2 - kArrowheadLength, centerVert - 1 - kArrowheadLength);
		}
		else if (IsDlgButtonChecked(hDlg, kBlowerDownButton))
		{
			MoveToEx(hdc, centerHori, bounds.top, NULL);
			LineTo(hdc, centerHori, bounds.bottom - 1);
			MoveToEx(hdc, centerHori, bounds.bottom - 2, NULL);
			LineTo(hdc, centerHori + kArrowheadLength, bounds.bottom - 2 - kArrowheadLength);
			MoveToEx(hdc, centerHori - 1, bounds.bottom - 2, NULL);
			LineTo(hdc, centerHori - 1 - kArrowheadLength, bounds.bottom - 2 - kArrowheadLength);
		}
		else if (IsDlgButtonChecked(hDlg, kBlowerLeftButton))
		{
			MoveToEx(hdc, bounds.left, centerVert, NULL);
			LineTo(hdc, bounds.right - 1, centerVert);
			MoveToEx(hdc, bounds.left, centerVert, NULL);
			LineTo(hdc, bounds.left + 1 + kArrowheadLength, centerVert - 1 - kArrowheadLength);
			MoveToEx(hdc, bounds.left + 1, centerVert, NULL);
			LineTo(hdc, bounds.left + 1 + kArrowheadLength, centerVert + kArrowheadLength);
		}

		SelectObject(hdc, oldPen);
		DeleteObject(hPen);

		// Draw the focus rectangle around the custom-drawn arrow, so that
		// the direction option buttons look like they are part of a single
		// control.
		InflateRect(&bounds, 1, 1);
		focusedWindow = GetFocus();
		if ((GetDlgItem(hDlg, kBlowerUpButton) == focusedWindow) ||
			(GetDlgItem(hDlg, kBlowerRightButton) == focusedWindow) ||
			(GetDlgItem(hDlg, kBlowerDownButton) == focusedWindow) ||
			(GetDlgItem(hDlg, kBlowerLeftButton) == focusedWindow))
		{
			if ((SendMessage(hDlg, WM_QUERYUISTATE, 0, 0) & UISF_HIDEFOCUS) == 0)
			{
				DrawFocusRect(hdc, &bounds);
			}
		}
	}
}

//--------------------------------------------------------------  BlowerFilter

INT_PTR CALLBACK BlowerFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	SInt16 what;

	switch (message)
	{
	case WM_INITDIALOG:
		what = thisRoom->objects[objActive].what;

		if (thisRoom->objects[objActive].data.a.initial)
			CheckDlgButton(hDlg, kBlowerInitialState, BST_CHECKED);
		else
			CheckDlgButton(hDlg, kBlowerInitialState, BST_UNCHECKED);

		if ((what == kTaper) || (what == kCandle) || (what == kStubby) ||
			(what == kTiki) || (what == kBBQ))
		{
			ShowWindow(GetDlgItem(hDlg, kBlowerInitialState), SW_HIDE);
		}

		if ((what == kLeftFan) || (what == kRightFan))
		{
			if (what == kLeftFan)
			{
				CheckRadioButton(hDlg, kLeftFacingRadio, kRightFacingRadio,
					kLeftFacingRadio);
			}
			else
			{
				CheckRadioButton(hDlg, kLeftFacingRadio, kRightFacingRadio,
					kRightFacingRadio);
			}
			ShowWindow(GetDlgItem(hDlg, kDirectionText), SW_HIDE);
		}
		else
		{
			ShowWindow(GetDlgItem(hDlg, kLeftFacingRadio), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, kRightFacingRadio), SW_HIDE);
		}

		if ((thisRoom->objects[objActive].data.a.vector & 0x01) == 0x01)
		{
			CheckRadioButton(hDlg, kBlowerUpButton, kBlowerLeftButton,
				kBlowerUpButton);
		}
		else if ((thisRoom->objects[objActive].data.a.vector & 0x02) == 0x02)
		{
			CheckRadioButton(hDlg, kBlowerUpButton, kBlowerLeftButton,
				kBlowerRightButton);
		}
		else if ((thisRoom->objects[objActive].data.a.vector & 0x04) == 0x04)
		{
			CheckRadioButton(hDlg, kBlowerUpButton, kBlowerLeftButton,
				kBlowerDownButton);
		}
		else if ((thisRoom->objects[objActive].data.a.vector & 0x08) == 0x08)
		{
			CheckRadioButton(hDlg, kBlowerUpButton, kBlowerLeftButton,
				kBlowerLeftButton);
		}
		if ((what != kInvisBlower) && (what != kLiftArea))
		{
			ShowWindow(GetDlgItem(hDlg, kBlowerUpButton), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, kBlowerRightButton), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, kBlowerDownButton), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, kBlowerLeftButton), SW_HIDE);
		}

		if (retroLinkList[objActive].room == -1)
			ShowWindow(GetDlgItem(hDlg, kBlowerLinkedFrom), SW_HIDE);

		CenterDialogOverOwner(hDlg);
		ParamDialogText(hDlg, (const DialogParams *)lParam);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case kBlowerLinkedFrom:
			what = thisRoom->objects[objActive].what;

			if (IsDlgButtonChecked(hDlg, kBlowerInitialState))
				thisRoom->objects[objActive].data.a.initial = true;
			else
				thisRoom->objects[objActive].data.a.initial = false;

			if ((what == kInvisBlower) || (what == kLiftArea))
			{
				if (IsDlgButtonChecked(hDlg, kBlowerUpButton))
					thisRoom->objects[objActive].data.a.vector = 0x01;
				else if (IsDlgButtonChecked(hDlg, kBlowerRightButton))
					thisRoom->objects[objActive].data.a.vector = 0x02;
				else if (IsDlgButtonChecked(hDlg, kBlowerDownButton))
					thisRoom->objects[objActive].data.a.vector = 0x04;
				else if (IsDlgButtonChecked(hDlg, kBlowerLeftButton))
					thisRoom->objects[objActive].data.a.vector = 0x08;
			}

			if ((what == kLeftFan) || (what == kRightFan))
			{
				if (IsDlgButtonChecked(hDlg, kLeftFacingRadio))
					thisRoom->objects[objActive].what = kLeftFan;
				else
					thisRoom->objects[objActive].what = kRightFan;
				if (KeepObjectLegal())
				{
				}
				Mac_InvalWindowRect(mainWindow, &mainWindowRect);
				GetThisRoomsObjRects();
				ReadyBackground(thisRoom->background, thisRoom->tiles);
				DrawThisRoomsObjects();
			}
			fileDirty = true;
			UpdateMenus(false);
			EndDialog(hDlg, LOWORD(wParam));
			break;

		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;

		case kBlowerUpButton:
		case kBlowerRightButton:
		case kBlowerDownButton:
		case kBlowerLeftButton:
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
			case BN_SETFOCUS:
			case BN_KILLFOCUS:
			{
				HDC hdc = GetDC(hDlg);
				UpdateBlowerInfo(hDlg, hdc);
				ReleaseDC(hDlg, hdc);
				break;
			}
			}
			break;
		}
		return TRUE;

	case WM_UPDATEUISTATE:
		InvalidateRect(hDlg, NULL, TRUE);
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(hDlg, &ps);
		UpdateBlowerInfo(hDlg, ps.hdc);
		EndPaint(hDlg, &ps);
		return TRUE;
	}
	}
	return FALSE;
}

//--------------------------------------------------------------  FurnitureFilter

INT_PTR CALLBACK FurnitureFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		if ((objActive < 0) || (retroLinkList[objActive].room == -1))
			ShowWindow(GetDlgItem(hDlg, kFurnitureLinkedFrom), SW_HIDE);

		CenterDialogOverOwner(hDlg);
		ParamDialogText(hDlg, (const DialogParams *)lParam);
		FocusDefaultButton(hDlg);
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
		case kFurnitureLinkedFrom:
			EndDialog(hDlg, LOWORD(wParam));
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  CustPictFilter

INT_PTR CALLBACK CustPictFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	INT wasPict;
	HWND hwndPictID;

	switch (message)
	{
	case WM_INITDIALOG:
		if (thisRoom->objects[objActive].what == kCustomPict)
			wasPict = thisRoom->objects[objActive].data.g.height;
		else
			wasPict = thisRoom->objects[objActive].data.e.where;
		SetDlgItemInt(hDlg, kCustPictIDItem, (UINT)wasPict, TRUE);

		CenterDialogOverOwner(hDlg);
		ParamDialogText(hDlg, (const DialogParams *)lParam);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			wasPict = (INT)GetDlgItemInt(hDlg, kCustPictIDItem, NULL, TRUE);
			if (thisRoom->objects[objActive].what == kCustomPict)
			{
				if ((wasPict < 10000) || (wasPict > 32767))
				{
					MessageBeep(MB_ICONWARNING);
					wasPict = thisRoom->objects[objActive].data.g.height;
					SetDlgItemInt(hDlg, kCustPictIDItem, (UINT)wasPict, TRUE);
					hwndPictID = GetDlgItem(hDlg, kCustPictIDItem);
					SendMessage(hDlg, WM_NEXTDLGCTL, (WPARAM)hwndPictID, TRUE);
				}
				else
				{
					thisRoom->objects[objActive].data.g.height = (SInt16)wasPict;
					if (KeepObjectLegal())
					{
					}
					fileDirty = true;
					UpdateMenus(false);
					Mac_InvalWindowRect(mainWindow, &mainWindowRect);
					GetThisRoomsObjRects();
					ReadyBackground(thisRoom->background, thisRoom->tiles);
					DrawThisRoomsObjects();
					EndDialog(hDlg, LOWORD(wParam));
				}
			}
			else
			{
				if ((wasPict < 3000) || (wasPict > 32767))
				{
					MessageBeep(MB_ICONWARNING);
					wasPict = thisRoom->objects[objActive].data.e.where;
					SetDlgItemInt(hDlg, kCustPictIDItem, (UINT)wasPict, TRUE);
					hwndPictID = GetDlgItem(hDlg, kCustPictIDItem);
					SendMessage(hDlg, WM_NEXTDLGCTL, (WPARAM)hwndPictID, TRUE);
				}
				else
				{
					thisRoom->objects[objActive].data.e.where = (SInt16)wasPict;
					fileDirty = true;
					UpdateMenus(false);
					Mac_InvalWindowRect(mainWindow, &mainWindowRect);
					GetThisRoomsObjRects();
					ReadyBackground(thisRoom->background, thisRoom->tiles);
					DrawThisRoomsObjects();
					EndDialog(hDlg, LOWORD(wParam));
				}
			}
			break;

		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  SwitchFilter

INT_PTR CALLBACK SwitchFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{
		HWND hwndInitialFocus = NULL;

		if (thisRoom->objects[objActive].data.e.type == kToggle)
		{
			hwndInitialFocus = GetDlgItem(hDlg, kToggleRadio);
			CheckRadioButton(hDlg, kToggleRadio, kForceOffRadio, kToggleRadio);
		}
		else if (thisRoom->objects[objActive].data.e.type == kForceOn)
		{
			hwndInitialFocus = GetDlgItem(hDlg, kForceOnRadio);
			CheckRadioButton(hDlg, kToggleRadio, kForceOffRadio, kForceOnRadio);
		}
		else if (thisRoom->objects[objActive].data.e.type == kForceOff)
		{
			hwndInitialFocus = GetDlgItem(hDlg, kForceOffRadio);
			CheckRadioButton(hDlg, kToggleRadio, kForceOffRadio, kForceOffRadio);
		}
		if (hwndInitialFocus == NULL)
			hwndInitialFocus = GetDlgItem(hDlg, kToggleRadio);

		if (thisRoom->objects[objActive].data.e.who == 255)
			EnableWindow(GetDlgItem(hDlg, kSwitchGotoButton), FALSE);

		if (retroLinkList[objActive].room == -1)
			ShowWindow(GetDlgItem(hDlg, kSwitchLinkedFrom), SW_HIDE);

		CenterDialogOverOwner(hDlg);
		ParamDialogText(hDlg, (const DialogParams *)lParam);
		SendMessage(hDlg, WM_NEXTDLGCTL, (WPARAM)hwndInitialFocus, TRUE);
		return FALSE;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case kLinkSwitchButton:
		case kSwitchGotoButton:
		case kSwitchLinkedFrom:
			if (IsDlgButtonChecked(hDlg, kToggleRadio))
				thisRoom->objects[objActive].data.e.type = kToggle;
			else if (IsDlgButtonChecked(hDlg, kForceOnRadio))
				thisRoom->objects[objActive].data.e.type = kForceOn;
			else if (IsDlgButtonChecked(hDlg, kForceOffRadio))
				thisRoom->objects[objActive].data.e.type = kForceOff;
			fileDirty = true;
			UpdateMenus(false);
			EndDialog(hDlg, LOWORD(wParam));
			break;

		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  TriggerFilter

INT_PTR CALLBACK TriggerFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	INT delayIs;
	HWND hwndFocus;

	switch (message)
	{
	case WM_INITDIALOG:
		if (retroLinkList[objActive].room == -1)
			ShowWindow(GetDlgItem(hDlg, kTriggerLinkedFrom), SW_HIDE);

		if (thisRoom->objects[objActive].data.e.who == 255)
			EnableWindow(GetDlgItem(hDlg, kTriggerGotoButton), FALSE);

		delayIs = thisRoom->objects[objActive].data.e.delay;
		SetDlgItemInt(hDlg, kTriggerDelayItem, (UINT)delayIs, TRUE);

		CenterDialogOverOwner(hDlg);
		ParamDialogText(hDlg, (const DialogParams *)lParam);
		hwndFocus = GetDlgItem(hDlg, kTriggerDelayItem);
		SendMessage(hDlg, WM_NEXTDLGCTL, (WPARAM)hwndFocus, TRUE);
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case kLinkTriggerButton:
		case kTriggerGotoButton:
		case kTriggerLinkedFrom:
			delayIs = GetDlgItemInt(hDlg, kTriggerDelayItem, NULL, TRUE);
			if ((delayIs < 0) || (delayIs > 32767))
			{
				MessageBeep(MB_ICONWARNING);
				delayIs = thisRoom->objects[objActive].data.e.delay;
				SetDlgItemInt(hDlg, kTriggerDelayItem, (UINT)delayIs, TRUE);
				hwndFocus = GetDlgItem(hDlg, kTriggerDelayItem);
				SendMessage(hDlg, WM_NEXTDLGCTL, (WPARAM)hwndFocus, TRUE);
			}
			else
			{
				thisRoom->objects[objActive].data.e.delay = (SInt16)delayIs;
				fileDirty = true;
				UpdateMenus(false);
				EndDialog(hDlg, LOWORD(wParam));
			}
			break;

		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  LightFilter

INT_PTR CALLBACK LightFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		if (thisRoom->objects[objActive].data.f.initial)
			CheckDlgButton(hDlg, kLightInitialState, BST_CHECKED);
		else
			CheckDlgButton(hDlg, kLightInitialState, BST_UNCHECKED);

		if (retroLinkList[objActive].room == -1)
			ShowWindow(GetDlgItem(hDlg, kLightLinkedFrom), SW_HIDE);

		CenterDialogOverOwner(hDlg);
		ParamDialogText(hDlg, (const DialogParams *)lParam);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case kLightLinkedFrom:
			if (IsDlgButtonChecked(hDlg, kLightInitialState))
				thisRoom->objects[objActive].data.f.initial = true;
			else
				thisRoom->objects[objActive].data.f.initial = false;

			ReadyBackground(thisRoom->background, thisRoom->tiles);
			DrawThisRoomsObjects();
			Mac_InvalWindowRect(mainWindow, &mainWindowRect);
			fileDirty = true;
			UpdateMenus(false);
			EndDialog(hDlg, LOWORD(wParam));
			break;

		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  ApplianceFilter

INT_PTR CALLBACK ApplianceFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	SInt16 what;
	INT delay;

	switch (message)
	{
	case WM_INITDIALOG:
		if (retroLinkList[objActive].room == -1)
			ShowWindow(GetDlgItem(hDlg, kApplianceLinkedFrom), SW_HIDE);

		if (thisRoom->objects[objActive].data.g.initial)
			CheckDlgButton(hDlg, kApplianceInitialState, BST_CHECKED);
		else
			CheckDlgButton(hDlg, kApplianceInitialState, BST_UNCHECKED);

		what = thisRoom->objects[objActive].what;
		if ((what == kShredder) || (what == kMacPlus) || (what == kTV) ||
			(what == kCoffee) || (what == kVCR) || (what == kMicrowave))
		{
			ShowWindow(GetDlgItem(hDlg, kApplianceDelay), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, kApplianceDelayLabel), SW_HIDE);
		}

		delay = thisRoom->objects[objActive].data.g.delay;
		SetDlgItemInt(hDlg, kApplianceDelay, (UINT)delay, TRUE);

		CenterDialogOverOwner(hDlg);
		ParamDialogText(hDlg, (const DialogParams *)lParam);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case kApplianceLinkedFrom:
			delay = (INT)GetDlgItemInt(hDlg, kApplianceDelay, NULL, TRUE);
			if ((delay < 0) || (delay > 255))
			{
				MessageBeep(MB_ICONWARNING);
				delay = thisRoom->objects[objActive].data.g.delay;
				SetDlgItemInt(hDlg, kApplianceDelay, (UINT)delay, TRUE);
				SendMessage(hDlg, WM_NEXTDLGCTL,
					(WPARAM)GetDlgItem(hDlg, kApplianceDelay), TRUE);
			}
			else
			{
				thisRoom->objects[objActive].data.g.delay = (Byte)delay;
				if (IsDlgButtonChecked(hDlg, kApplianceInitialState))
					thisRoom->objects[objActive].data.g.initial = true;
				else
					thisRoom->objects[objActive].data.g.initial = false;
				fileDirty = true;
				UpdateMenus(false);
				Mac_InvalWindowRect(mainWindow, &mainWindowRect);
				GetThisRoomsObjRects();
				ReadyBackground(thisRoom->background, thisRoom->tiles);
				DrawThisRoomsObjects();
				EndDialog(hDlg, LOWORD(wParam));
			}
			break;

		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  MicrowaveFilter

INT_PTR CALLBACK MicrowaveFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	Byte kills;

	switch (message)
	{
	case WM_INITDIALOG:
		if (retroLinkList[objActive].room == -1)
			ShowWindow(GetDlgItem(hDlg, kMicrowaveLinkedFrom), SW_HIDE);

		if (thisRoom->objects[objActive].data.g.initial)
			CheckDlgButton(hDlg, kMicrowaveInitialState, BST_CHECKED);
		else
			CheckDlgButton(hDlg, kMicrowaveInitialState, BST_UNCHECKED);

		kills = thisRoom->objects[objActive].data.g.byte0;
		if ((kills & 0x01) == 0x01)
			CheckDlgButton(hDlg, kKillBandsCheckbox, BST_CHECKED);
		else
			CheckDlgButton(hDlg, kKillBandsCheckbox, BST_UNCHECKED);
		if ((kills & 0x02) == 0x02)
			CheckDlgButton(hDlg, kKillBatteryCheckbox, BST_CHECKED);
		else
			CheckDlgButton(hDlg, kKillBatteryCheckbox, BST_UNCHECKED);
		if ((kills & 0x04) == 0x04)
			CheckDlgButton(hDlg, kKillFoilCheckbox, BST_CHECKED);
		else
			CheckDlgButton(hDlg, kKillFoilCheckbox, BST_UNCHECKED);

		CenterDialogOverOwner(hDlg);
		ParamDialogText(hDlg, (const DialogParams *)lParam);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case kMicrowaveLinkedFrom:
			if (IsDlgButtonChecked(hDlg, kMicrowaveInitialState))
				thisRoom->objects[objActive].data.g.initial = true;
			else
				thisRoom->objects[objActive].data.g.initial = false;
			kills = 0;
			if (IsDlgButtonChecked(hDlg, kKillBandsCheckbox))
				kills += 1;
			if (IsDlgButtonChecked(hDlg, kKillBatteryCheckbox))
				kills += 2;
			if (IsDlgButtonChecked(hDlg, kKillFoilCheckbox))
				kills += 4;
			thisRoom->objects[objActive].data.g.byte0 = kills;

			fileDirty = true;
			UpdateMenus(false);
			Mac_InvalWindowRect(mainWindow, &mainWindowRect);
			GetThisRoomsObjRects();
			ReadyBackground(thisRoom->background, thisRoom->tiles);
			DrawThisRoomsObjects();
			EndDialog(hDlg, LOWORD(wParam));
			break;

		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  GreaseFilter

INT_PTR CALLBACK GreaseFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		if (retroLinkList[objActive].room == -1)
			ShowWindow(GetDlgItem(hDlg, kGreaseLinkedFrom), SW_HIDE);

		if (thisRoom->objects[objActive].data.c.initial)
			CheckDlgButton(hDlg, kGreaseSpilled, BST_UNCHECKED);
		else
			CheckDlgButton(hDlg, kGreaseSpilled, BST_CHECKED);

		CenterDialogOverOwner(hDlg);
		ParamDialogText(hDlg, (const DialogParams *)lParam);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case kGreaseLinkedFrom:
			if (IsDlgButtonChecked(hDlg, kGreaseSpilled))
				thisRoom->objects[objActive].data.c.initial = false;
			else
				thisRoom->objects[objActive].data.c.initial = true;
			fileDirty = true;
			UpdateMenus(false);
			Mac_InvalWindowRect(mainWindow, &mainWindowRect);
			GetThisRoomsObjRects();
			ReadyBackground(thisRoom->background, thisRoom->tiles);
			DrawThisRoomsObjects();
			EndDialog(hDlg, LOWORD(wParam));
			break;

		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  InvisBonusFilter

INT_PTR CALLBACK InvisBonusFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		switch (thisRoom->objects[objActive].data.c.points)
		{
		case 300:
			CheckRadioButton(hDlg, k100PtRadio, k500PtRadio, k300PtRadio);
			break;

		case 500:
			CheckRadioButton(hDlg, k100PtRadio, k500PtRadio, k500PtRadio);
			break;

		default:
			CheckRadioButton(hDlg, k100PtRadio, k500PtRadio, k100PtRadio);
			break;
		}

		if (retroLinkList[objActive].room == -1)
			ShowWindow(GetDlgItem(hDlg, kInvisBonusLinkedFrom), SW_HIDE);

		CenterDialogOverOwner(hDlg);
		ParamDialogText(hDlg, (const DialogParams *)lParam);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case kInvisBonusLinkedFrom:
			if (IsDlgButtonChecked(hDlg, k100PtRadio))
			{
				thisRoom->objects[objActive].data.c.points = 100;
			}
			else if (IsDlgButtonChecked(hDlg, k300PtRadio))
			{
				thisRoom->objects[objActive].data.c.points = 300;
			}
			else if (IsDlgButtonChecked(hDlg, k500PtRadio))
			{
				thisRoom->objects[objActive].data.c.points = 500;
			}
			fileDirty = true;
			UpdateMenus(false);
			EndDialog(hDlg, LOWORD(wParam));
			break;

		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  TransFilter

INT_PTR CALLBACK TransFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hwndInitialFocus;
	Byte initialState;

	switch (message)
	{
	case WM_INITDIALOG:
		if (retroLinkList[objActive].room == -1)
			ShowWindow(GetDlgItem(hDlg, kTransLinkedFrom), SW_HIDE);

		if (thisRoom->objects[objActive].what != kDeluxeTrans)
		{
			ShowWindow(GetDlgItem(hDlg, kTransInitialState), SW_HIDE);
			hwndInitialFocus = GetDlgItem(hDlg, IDOK);
		}
		else
		{
			initialState = (thisRoom->objects[objActive].data.d.wide & 0xF0) >> 4;
			if (initialState)
				CheckDlgButton(hDlg, kTransInitialState, BST_CHECKED);
			else
				CheckDlgButton(hDlg, kTransInitialState, BST_UNCHECKED);
			hwndInitialFocus = GetDlgItem(hDlg, kTransInitialState);
		}

		if (thisRoom->objects[objActive].data.d.who == 255)
			EnableWindow(GetDlgItem(hDlg, kTransGotoButton), FALSE);

		CenterDialogOverOwner(hDlg);
		ParamDialogText(hDlg, (const DialogParams *)lParam);
		SendMessage(hDlg, WM_NEXTDLGCTL, (WPARAM)hwndInitialFocus, TRUE);
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case kLinkTransButton:
		case kTransGotoButton:
		case kTransLinkedFrom:
			if (thisRoom->objects[objActive].what == kDeluxeTrans)
			{
				if (IsDlgButtonChecked(hDlg, kTransInitialState))
					initialState = 0x1;
				else
					initialState = 0x0;
				thisRoom->objects[objActive].data.d.wide = initialState << 4;
			}
			fileDirty = true;
			UpdateMenus(false);
			EndDialog(hDlg, LOWORD(wParam));
			break;

		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  EnemyFilter

INT_PTR CALLBACK EnemyFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	INT delay;

	switch (message)
	{
	case WM_INITDIALOG:
		if (retroLinkList[objActive].room == -1)
			ShowWindow(GetDlgItem(hDlg, kEnemyLinkedFrom), SW_HIDE);

		delay = thisRoom->objects[objActive].data.h.delay;
		SetDlgItemInt(hDlg, kEnemyDelayItem, (UINT)delay, TRUE);

		if (thisRoom->objects[objActive].data.h.initial)
			CheckDlgButton(hDlg, kEnemyInitialState, BST_CHECKED);
		else
			CheckDlgButton(hDlg, kEnemyInitialState, BST_UNCHECKED);

		if (thisRoom->objects[objActive].what == kBall)
		{
			ShowWindow(GetDlgItem(hDlg, kEnemyDelayItem), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, kEnemyDelayLabelItem), SW_HIDE);
		}

		CenterDialogOverOwner(hDlg);
		ParamDialogText(hDlg, (const DialogParams *)lParam);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case kEnemyLinkedFrom:
			delay = GetDlgItemInt(hDlg, kEnemyDelayItem, NULL, TRUE);
			if (((delay < 0) || (delay > 255)) &&
				(thisRoom->objects[objActive].what != kBall))
			{
				MessageBeep(MB_ICONWARNING);
				delay = thisRoom->objects[objActive].data.h.delay;
				SetDlgItemInt(hDlg, kEnemyDelayItem, (UINT)delay, TRUE);
				SendMessage(hDlg, WM_NEXTDLGCTL,
					(WPARAM)GetDlgItem(hDlg, kEnemyDelayItem), TRUE);
			}
			else
			{
				if (IsDlgButtonChecked(hDlg, kEnemyInitialState))
					thisRoom->objects[objActive].data.h.initial = true;
				else
					thisRoom->objects[objActive].data.h.initial = false;
				if (thisRoom->objects[objActive].what != kBall)
					thisRoom->objects[objActive].data.h.delay = (Byte)delay;
				fileDirty = true;
				UpdateMenus(false);
				EndDialog(hDlg, LOWORD(wParam));
			}
			break;

		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  EnemyFilter

INT_PTR CALLBACK FlowerFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	SInt16 flower;

	switch (message)
	{
	case WM_INITDIALOG:
		if (retroLinkList[objActive].room == -1)
			ShowWindow(GetDlgItem(hDlg, kFlowerLinkedFrom), SW_HIDE);

		flower = thisRoom->objects[objActive].data.i.pict;
		if ((flower < 0) || (flower >= kNumFlowers))
			flower = 0;
		CheckRadioButton(hDlg, kRadioFlower1, kRadioFlower6, kRadioFlower1 + flower);

		CenterDialogOverOwner(hDlg);
		ParamDialogText(hDlg, (const DialogParams *)lParam);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case kFlowerLinkedFrom:
			for (flower = 0; flower < kNumFlowers; flower++)
				if (IsDlgButtonChecked(hDlg, kRadioFlower1 + flower))
					break;
			if (flower >= kNumFlowers)
				flower = 0;
			if (flower != thisRoom->objects[objActive].data.i.pict)
			{
				Mac_InvalWindowRect(mainWindow, &thisRoom->objects[objActive].data.i.bounds);
				thisRoom->objects[objActive].data.i.bounds.right =
					thisRoom->objects[objActive].data.i.bounds.left +
					RectWide(&flowerSrc[flower]);
				thisRoom->objects[objActive].data.i.bounds.top =
					thisRoom->objects[objActive].data.i.bounds.bottom -
					RectTall(&flowerSrc[flower]);
				thisRoom->objects[objActive].data.i.pict = flower;
				Mac_InvalWindowRect(mainWindow, &thisRoom->objects[objActive].data.i.bounds);
				GetThisRoomsObjRects();
				ReadyBackground(thisRoom->background, thisRoom->tiles);
				DrawThisRoomsObjects();
				fileDirty = true;
				UpdateMenus(false);
				wasFlower = flower;
			}
			EndDialog(hDlg, LOWORD(wParam));
			break;

		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  DoBlowerObjectInfo

void DoBlowerObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	wchar_t kindStr[256];
	wchar_t distStr[16];
	INT_PTR result;

	StringCchPrintf(numberStr, ARRAYSIZE(numberStr), L"%d", (int)(objActive + 1));
	GetObjectName(kindStr, ARRAYSIZE(kindStr), thisRoom->objects[objActive].what);
	StringCchPrintf(distStr, ARRAYSIZE(distStr), L"%d",
		(int)thisRoom->objects[objActive].data.a.distance);

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	params.arg[2] = distStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kBlowerInfoDialogID),
		hwndOwner, BlowerFilter, (LPARAM)&params);

	if (result == kBlowerLinkedFrom)
	{
		GoToObjectInRoomNum(retroLinkList[objActive].object,
			retroLinkList[objActive].room);
	}
}

//--------------------------------------------------------------  DoFurnitureObjectInfo

void DoFurnitureObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	wchar_t kindStr[256];
	INT_PTR result;

	if (objActive == kInitialGliderSelected)
	{
		StringCchCopy(numberStr, ARRAYSIZE(numberStr), L"-");
		StringCchCopy(kindStr, ARRAYSIZE(kindStr), L"Glider Begins");
	}
	else if (objActive == kLeftGliderSelected)
	{
		StringCchCopy(numberStr, ARRAYSIZE(numberStr), L"-");
		StringCchCopy(kindStr, ARRAYSIZE(kindStr), L"New Glider (left)");
	}
	else if (objActive == kRightGliderSelected)
	{
		StringCchCopy(numberStr, ARRAYSIZE(numberStr), L"-");
		StringCchCopy(kindStr, ARRAYSIZE(kindStr), L"New Glider (right)");
	}
	else
	{
		StringCchPrintf(numberStr, ARRAYSIZE(numberStr), L"%d", (int)(objActive + 1));
		GetObjectName(kindStr, ARRAYSIZE(kindStr), thisRoom->objects[objActive].what);
	}

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kFurnitureInfoDialogID),
		hwndOwner, FurnitureFilter, (LPARAM)&params);

	if (result == kFurnitureLinkedFrom)
	{
		GoToObjectInRoomNum(retroLinkList[objActive].object,
			retroLinkList[objActive].room);
	}
}

//--------------------------------------------------------------  DoCustPictObjectInfo

void DoCustPictObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	wchar_t kindStr[256];

	StringCchPrintf(numberStr, ARRAYSIZE(numberStr), L"%d", (int)(objActive + 1));
	GetObjectName(kindStr, ARRAYSIZE(kindStr), thisRoom->objects[objActive].what);

	if (thisRoom->objects[objActive].what == kCustomPict)
	{
		params.arg[0] = numberStr;
		params.arg[1] = kindStr;
		params.arg[2] = L"PICT";
		params.arg[3] = L"10000";
	}
	else
	{
		params.arg[0] = numberStr;
		params.arg[1] = kindStr;
		params.arg[2] = L"Sound";
		params.arg[3] = L"3000";
	}
	DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kCustPictInfoDialogID),
		hwndOwner, CustPictFilter, (LPARAM)&params);
}

//--------------------------------------------------------------  DoSwitchObjectInfo

void DoSwitchObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	wchar_t kindStr[256];
	wchar_t roomStr[32];
	wchar_t objStr[16];
	SInt16 floor, suite;
	INT_PTR result;

	floor = 0;
	suite = kRoomIsEmpty;

	StringCchPrintf(numberStr, ARRAYSIZE(numberStr), L"%d", (int)(objActive + 1));
	GetObjectName(kindStr, ARRAYSIZE(kindStr), thisRoom->objects[objActive].what);
	if (thisRoom->objects[objActive].data.e.where == -1)
	{
		StringCchCopy(roomStr, ARRAYSIZE(roomStr), L"none");
	}
	else
	{
		ExtractFloorSuite(thisRoom->objects[objActive].data.e.where, &floor, &suite);
		StringCchPrintf(roomStr, ARRAYSIZE(roomStr), L"%d / %d", (int)floor, (int)suite);
	}

	if (thisRoom->objects[objActive].data.e.who == 255)
	{
		StringCchCopy(objStr, ARRAYSIZE(objStr), L"none");
	}
	else
	{
		StringCchPrintf(objStr, ARRAYSIZE(objStr), L"%d",
			(int)(thisRoom->objects[objActive].data.e.who + 1));
	}

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	params.arg[2] = roomStr;
	params.arg[3] = objStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kSwitchInfoDialogID),
		hwndOwner, SwitchFilter, (LPARAM)&params);

	if (result == kLinkSwitchButton)
	{
		linkType = kSwitchLinkOnly;
		linkerIsSwitch = true;
		OpenLinkWindow();
		linkRoom = thisRoomNumber;
		linkObject = (Byte)objActive;
		DeselectObject();
	}
	else if (result == kSwitchGotoButton)
	{
		GoToObjectInRoom(thisRoom->objects[objActive].data.e.who, floor, suite);
	}
	else if (result == kSwitchLinkedFrom)
	{
		GoToObjectInRoomNum(retroLinkList[objActive].object,
			retroLinkList[objActive].room);
	}
}

//--------------------------------------------------------------  DoTriggerObjectInfo

void DoTriggerObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	wchar_t kindStr[256];
	wchar_t roomStr[32];
	wchar_t objStr[16];
	SInt16 floor, suite;
	INT_PTR result;

	floor = 0;
	suite = kRoomIsEmpty;

	StringCchPrintf(numberStr, ARRAYSIZE(numberStr), L"%d", (int)(objActive + 1));
	GetObjectName(kindStr, ARRAYSIZE(kindStr), thisRoom->objects[objActive].what);
	if (thisRoom->objects[objActive].data.e.where == -1)
	{
		StringCchCopy(roomStr, ARRAYSIZE(roomStr), L"none");
	}
	else
	{
		ExtractFloorSuite(thisRoom->objects[objActive].data.e.where, &floor, &suite);
		StringCchPrintf(roomStr, ARRAYSIZE(roomStr), L"%d / %d", (int)floor, (int)suite);
	}

	if (thisRoom->objects[objActive].data.e.who == 255)
	{
		StringCchCopy(objStr, ARRAYSIZE(objStr), L"none");
	}
	else
	{
		StringCchPrintf(objStr, ARRAYSIZE(objStr), L"%d",
			(int)(thisRoom->objects[objActive].data.e.who + 1));
	}

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	params.arg[2] = roomStr;
	params.arg[3] = objStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kTriggerInfoDialogID),
		hwndOwner, TriggerFilter, (LPARAM)&params);

	if (result == kLinkTriggerButton)
	{
		linkType = kTriggerLinkOnly;
		linkerIsSwitch = true;
		OpenLinkWindow();
		linkRoom = thisRoomNumber;
		linkObject = (Byte)objActive;
		DeselectObject();
	}
	else if (result == kTriggerGotoButton)
	{
		GoToObjectInRoom(thisRoom->objects[objActive].data.e.who, floor, suite);
	}
	else if (result == kTriggerLinkedFrom)
	{
		GoToObjectInRoomNum(retroLinkList[objActive].object,
			retroLinkList[objActive].room);
	}
}

//--------------------------------------------------------------  DoLightObjectInfo

void DoLightObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	wchar_t kindStr[256];
	INT_PTR result;

	StringCchPrintf(numberStr, ARRAYSIZE(numberStr), L"%d", (int)(objActive + 1));
	GetObjectName(kindStr, ARRAYSIZE(kindStr), thisRoom->objects[objActive].what);

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kLightInfoDialogID),
		hwndOwner, LightFilter, (LPARAM)&params);

	if (result == kLightLinkedFrom)
	{
		GoToObjectInRoomNum(retroLinkList[objActive].object,
			retroLinkList[objActive].room);
	}
}

//--------------------------------------------------------------  DoApplianceObjectInfo

void DoApplianceObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	wchar_t kindStr[256];
	INT_PTR result;

	StringCchPrintf(numberStr, ARRAYSIZE(numberStr), L"%d", (int)(objActive + 1));
	GetObjectName(kindStr, ARRAYSIZE(kindStr), thisRoom->objects[objActive].what);

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kApplianceInfoDialogID),
		hwndOwner, ApplianceFilter, (LPARAM)&params);

	if (result == kApplianceLinkedFrom)
	{
		GoToObjectInRoomNum(retroLinkList[objActive].object,
			retroLinkList[objActive].room);
	}
}

//--------------------------------------------------------------  DoMicrowaveObjectInfo

void DoMicrowaveObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	wchar_t kindStr[256];
	INT_PTR result;

	StringCchPrintf(numberStr, ARRAYSIZE(numberStr), L"%d", (int)(objActive + 1));
	GetObjectName(kindStr, ARRAYSIZE(kindStr), thisRoom->objects[objActive].what);

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kMicrowaveInfoDialogID),
		hwndOwner, MicrowaveFilter, (LPARAM)&params);

	if (result == kMicrowaveLinkedFrom)
	{
		GoToObjectInRoomNum(retroLinkList[objActive].object,
			retroLinkList[objActive].room);
	}
}

//--------------------------------------------------------------  DoGreaseObjectInfo

void DoGreaseObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	wchar_t kindStr[256];
	INT_PTR result;

	StringCchPrintf(numberStr, ARRAYSIZE(numberStr), L"%d", (int)(objActive + 1));
	GetObjectName(kindStr, ARRAYSIZE(kindStr), thisRoom->objects[objActive].what);

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kGreaseInfoDialogID),
		hwndOwner, GreaseFilter, (LPARAM)&params);

	if (result == kGreaseLinkedFrom)
	{
		GoToObjectInRoomNum(retroLinkList[objActive].object,
			retroLinkList[objActive].room);
	}
}

//--------------------------------------------------------------  DoInvisBonusObjectInfo

void DoInvisBonusObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	wchar_t kindStr[256];
	INT_PTR result;

	StringCchPrintf(numberStr, ARRAYSIZE(numberStr), L"%d", (int)(objActive + 1));
	GetObjectName(kindStr, ARRAYSIZE(kindStr), thisRoom->objects[objActive].what);

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kInvisBonusInfoDialogID),
		hwndOwner, InvisBonusFilter, (LPARAM)&params);

	if (result == kInvisBonusLinkedFrom)
	{
		GoToObjectInRoomNum(retroLinkList[objActive].object,
			retroLinkList[objActive].room);
	}
}

//--------------------------------------------------------------  DoTransObjectInfo

void DoTransObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	wchar_t kindStr[256];
	wchar_t roomStr[32];
	wchar_t objStr[16];
	SInt16 floor, suite;
	INT_PTR result;

	floor = 0;
	suite = kRoomIsEmpty;

	StringCchPrintf(numberStr, ARRAYSIZE(numberStr), L"%d", (int)(objActive + 1));
	GetObjectName(kindStr, ARRAYSIZE(kindStr), thisRoom->objects[objActive].what);
	if (thisRoom->objects[objActive].data.d.where == -1)
	{
		StringCchCopy(roomStr, ARRAYSIZE(roomStr), L"none");
	}
	else
	{
		ExtractFloorSuite(thisRoom->objects[objActive].data.d.where, &floor, &suite);
		StringCchPrintf(roomStr, ARRAYSIZE(roomStr), L"%d / %d", (int)floor, (int)suite);
	}

	if (thisRoom->objects[objActive].data.d.who == 255)
	{
		StringCchCopy(objStr, ARRAYSIZE(objStr), L"none");
	}
	else
	{
		StringCchPrintf(objStr, ARRAYSIZE(objStr), L"%d",
			(int)thisRoom->objects[objActive].data.d.who + 1);
	}

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	params.arg[2] = roomStr;
	params.arg[3] = objStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kTransInfoDialogID),
		hwndOwner, TransFilter, (LPARAM)&params);

	if (result == kLinkTransButton)
	{
		linkType = kTransportLinkOnly;
		linkerIsSwitch = false;
		OpenLinkWindow();
		linkRoom = thisRoomNumber;
		linkObject = (Byte)objActive;
		DeselectObject();
	}
	else if (result == kTransGotoButton)
	{
		GoToObjectInRoom(thisRoom->objects[objActive].data.d.who, floor, suite);
	}
	else if (result == kTransLinkedFrom)
	{
		GoToObjectInRoomNum(retroLinkList[objActive].object,
			retroLinkList[objActive].room);
	}
}

//--------------------------------------------------------------  DoEnemyObjectInfo

void DoEnemyObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	wchar_t kindStr[256];
	INT_PTR result;

	StringCchPrintf(numberStr, ARRAYSIZE(numberStr), L"%d", (int)(objActive + 1));
	GetObjectName(kindStr, ARRAYSIZE(kindStr), thisRoom->objects[objActive].what);

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kEnemyInfoDialogID),
		hwndOwner, EnemyFilter, (LPARAM)&params);

	if (result == kEnemyLinkedFrom)
	{
		GoToObjectInRoomNum(retroLinkList[objActive].object,
			retroLinkList[objActive].room);
	}
}

//--------------------------------------------------------------  DoFlowerObjectInfo

void DoFlowerObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	wchar_t kindStr[256];
	INT_PTR result;

	StringCchPrintf(numberStr, ARRAYSIZE(numberStr), L"%d", (int)(objActive + 1));
	GetObjectName(kindStr, ARRAYSIZE(kindStr), thisRoom->objects[objActive].what);

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kFlowerInfoDialogID),
		hwndOwner, FlowerFilter, (LPARAM)&params);

	if (result == kFlowerLinkedFrom)
	{
		GoToObjectInRoomNum(retroLinkList[objActive].object,
			retroLinkList[objActive].room);
	}
}

//--------------------------------------------------------------  DoObjectInfo

void DoObjectInfo (HWND hwndOwner)
{
	if ((objActive == kInitialGliderSelected) ||
		(objActive == kLeftGliderSelected) ||
		(objActive == kRightGliderSelected))
	{
		DoFurnitureObjectInfo(hwndOwner);
		return;
	}

	switch (thisRoom->objects[objActive].what)
	{
	case kFloorVent:
	case kCeilingVent:
	case kFloorBlower:
	case kCeilingBlower:
	case kSewerGrate:
	case kLeftFan:
	case kRightFan:
	case kTaper:
	case kCandle:
	case kStubby:
	case kTiki:
	case kBBQ:
	case kInvisBlower:
	case kGrecoVent:
	case kSewerBlower:
	case kLiftArea:
		DoBlowerObjectInfo(hwndOwner);
		break;

	case kTable:
	case kShelf:
	case kCabinet:
	case kFilingCabinet:
	case kWasteBasket:
	case kMilkCrate:
	case kCounter:
	case kDresser:
	case kDeckTable:
	case kStool:
	case kTrunk:
	case kInvisObstacle:
	case kManhole:
	case kBooks:
	case kInvisBounce:
	case kRedClock:
	case kBlueClock:
	case kYellowClock:
	case kCuckoo:
	case kPaper:
	case kBattery:
	case kBands:
	case kFoil:
	case kStar:
	case kSparkle:
	case kHelium:
	case kSlider:
	case kUpStairs:
	case kDownStairs:
	case kDoorInLf:
	case kDoorInRt:
	case kDoorExRt:
	case kDoorExLf:
	case kWindowInLf:
	case kWindowInRt:
	case kWindowExRt:
	case kWindowExLf:
	case kCinderBlock:
	case kFlowerBox:
	case kCDs:
	case kGuitar:
	case kStereo:
	case kCobweb:
	case kOzma:
	case kMirror:
	case kMousehole:
	case kFireplace:
	case kWallWindow:
	case kBear:
	case kCalendar:
	case kVase1:
	case kVase2:
	case kBulletin:
	case kCloud:
	case kFaucet:
	case kRug:
	case kChimes:
		DoFurnitureObjectInfo(hwndOwner);
		break;

	case kGreaseRt:
	case kGreaseLf:
		DoGreaseObjectInfo(hwndOwner);
		break;

	case kInvisBonus:
		DoInvisBonusObjectInfo(hwndOwner);
		break;

	case kMailboxLf:
	case kMailboxRt:
	case kFloorTrans:
	case kCeilingTrans:
	case kInvisTrans:
	case kDeluxeTrans:
		DoTransObjectInfo(hwndOwner);
		break;

	case kLightSwitch:
	case kMachineSwitch:
	case kThermostat:
	case kPowerSwitch:
	case kKnifeSwitch:
	case kInvisSwitch:
		DoSwitchObjectInfo(hwndOwner);
		break;

	case kTrigger:
	case kLgTrigger:
		DoTriggerObjectInfo(hwndOwner);
		break;

	case kCeilingLight:
	case kLightBulb:
	case kTableLamp:
	case kHipLamp:
	case kDecoLamp:
	case kFlourescent:
	case kTrackLight:
	case kInvisLight:
		DoLightObjectInfo(hwndOwner);
		break;

	case kShredder:
	case kToaster:
	case kMacPlus:
	case kTV:
	case kCoffee:
	case kOutlet:
	case kVCR:
		DoApplianceObjectInfo(hwndOwner);
		break;

	case kMicrowave:
		DoMicrowaveObjectInfo(hwndOwner);
		break;

	case kBalloon:
	case kCopterLf:
	case kCopterRt:
	case kDartLf:
	case kDartRt:
	case kBall:
	case kDrip:
	case kFish:
		DoEnemyObjectInfo(hwndOwner);
		break;

	case kFlower:
		DoFlowerObjectInfo(hwndOwner);
		break;

	case kSoundTrigger:
	case kCustomPict:
		DoCustPictObjectInfo(hwndOwner);
		break;

	default:
		MessageBeep(MB_ICONWARNING);
		break;
	}
}

