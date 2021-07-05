//============================================================================
//----------------------------------------------------------------------------
//                                ObjectInfo.c
//----------------------------------------------------------------------------
//============================================================================

#include "ObjectInfo.h"

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
#include "StringUtils.h"
#include "Utilities.h"

#include <strsafe.h>

#include <stdlib.h>

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

HRESULT GetObjectName (SInt16 objectType, PWSTR *ppszObjectName)
{
	return AllocLoadString(
		HINST_THISCOMPONENT,
		kObjectNameStringsBase + objectType,
		ppszObjectName
	);
}

//--------------------------------------------------------------  UpdateBlowerInfo

void UpdateBlowerInfo (HWND hDlg, HDC hdc)
{
	const LONG kArrowheadLength = 4;
	RECT bounds;
	LONG centerHori, centerVert;
	HPEN hPen, oldPen;
	HWND focusedWindow;

	if ((g_thisRoom->objects[g_objActive].what != kLeftFan) &&
		(g_thisRoom->objects[g_objActive].what != kRightFan))
	{
		GetWindowRect(GetDlgItem(hDlg, kBlowerArrow), &bounds);
		MapWindowRect(HWND_DESKTOP, hDlg, &bounds);

		FillRect(hdc, &bounds, GetSysColorBrush(COLOR_BTNFACE));

		// The bounds are deflated here to allow a focus rectangle to be drawn
		// later, when it is appropriate to do so, without the focus rectangle
		// overlapping the arrow.
		InflateRect(&bounds, -1, -1);
		centerHori = bounds.left + (bounds.right - bounds.left) / 2;
		centerVert = bounds.top + (bounds.bottom - bounds.top) / 2;

		hPen = CreatePen(PS_SOLID, 2, GetSysColor(COLOR_WINDOWTEXT));
		oldPen = SelectPen(hdc, hPen);

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

		SelectPen(hdc, oldPen);
		DeletePen(hPen);

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
		what = g_thisRoom->objects[g_objActive].what;

		if (g_thisRoom->objects[g_objActive].data.a.initial)
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

		if ((g_thisRoom->objects[g_objActive].data.a.vector & 0x01) == 0x01)
		{
			CheckRadioButton(hDlg, kBlowerUpButton, kBlowerLeftButton,
				kBlowerUpButton);
		}
		else if ((g_thisRoom->objects[g_objActive].data.a.vector & 0x02) == 0x02)
		{
			CheckRadioButton(hDlg, kBlowerUpButton, kBlowerLeftButton,
				kBlowerRightButton);
		}
		else if ((g_thisRoom->objects[g_objActive].data.a.vector & 0x04) == 0x04)
		{
			CheckRadioButton(hDlg, kBlowerUpButton, kBlowerLeftButton,
				kBlowerDownButton);
		}
		else if ((g_thisRoom->objects[g_objActive].data.a.vector & 0x08) == 0x08)
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

		if (g_retroLinkList[g_objActive].room == -1)
			ShowWindow(GetDlgItem(hDlg, kBlowerLinkedFrom), SW_HIDE);

		CenterDialogOverOwner(hDlg);
		ParamDialogText(hDlg, (const DialogParams *)lParam);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case kBlowerLinkedFrom:
			what = g_thisRoom->objects[g_objActive].what;

			if (IsDlgButtonChecked(hDlg, kBlowerInitialState))
				g_thisRoom->objects[g_objActive].data.a.initial = true;
			else
				g_thisRoom->objects[g_objActive].data.a.initial = false;

			if ((what == kInvisBlower) || (what == kLiftArea))
			{
				if (IsDlgButtonChecked(hDlg, kBlowerUpButton))
					g_thisRoom->objects[g_objActive].data.a.vector = 0x01;
				else if (IsDlgButtonChecked(hDlg, kBlowerRightButton))
					g_thisRoom->objects[g_objActive].data.a.vector = 0x02;
				else if (IsDlgButtonChecked(hDlg, kBlowerDownButton))
					g_thisRoom->objects[g_objActive].data.a.vector = 0x04;
				else if (IsDlgButtonChecked(hDlg, kBlowerLeftButton))
					g_thisRoom->objects[g_objActive].data.a.vector = 0x08;
			}

			if ((what == kLeftFan) || (what == kRightFan))
			{
				if (IsDlgButtonChecked(hDlg, kLeftFacingRadio))
					g_thisRoom->objects[g_objActive].what = kLeftFan;
				else
					g_thisRoom->objects[g_objActive].what = kRightFan;
				if (KeepObjectLegal())
				{
				}
				InvalidateRect(g_mainWindow, NULL, TRUE);
				GetThisRoomsObjRects();
				ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
				DrawThisRoomsObjects();
			}
			g_fileDirty = true;
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
		if ((g_objActive < 0) || (g_retroLinkList[g_objActive].room == -1))
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
		if (g_thisRoom->objects[g_objActive].what == kCustomPict)
			wasPict = g_thisRoom->objects[g_objActive].data.g.height;
		else
			wasPict = g_thisRoom->objects[g_objActive].data.e.where;
		SetDlgItemInt(hDlg, kCustPictIDItem, (UINT)wasPict, TRUE);

		CenterDialogOverOwner(hDlg);
		ParamDialogText(hDlg, (const DialogParams *)lParam);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			wasPict = (INT)GetDlgItemInt(hDlg, kCustPictIDItem, NULL, TRUE);
			if (g_thisRoom->objects[g_objActive].what == kCustomPict)
			{
				if ((wasPict < 10000) || (wasPict > 32767))
				{
					MessageBeep(MB_ICONWARNING);
					wasPict = g_thisRoom->objects[g_objActive].data.g.height;
					SetDlgItemInt(hDlg, kCustPictIDItem, (UINT)wasPict, TRUE);
					hwndPictID = GetDlgItem(hDlg, kCustPictIDItem);
					SendMessage(hDlg, WM_NEXTDLGCTL, (WPARAM)hwndPictID, TRUE);
				}
				else
				{
					g_thisRoom->objects[g_objActive].data.g.height = (SInt16)wasPict;
					if (KeepObjectLegal())
					{
					}
					g_fileDirty = true;
					UpdateMenus(false);
					InvalidateRect(g_mainWindow, NULL, TRUE);
					GetThisRoomsObjRects();
					ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
					DrawThisRoomsObjects();
					EndDialog(hDlg, LOWORD(wParam));
				}
			}
			else
			{
				if ((wasPict < 3000) || (wasPict > 32767))
				{
					MessageBeep(MB_ICONWARNING);
					wasPict = g_thisRoom->objects[g_objActive].data.e.where;
					SetDlgItemInt(hDlg, kCustPictIDItem, (UINT)wasPict, TRUE);
					hwndPictID = GetDlgItem(hDlg, kCustPictIDItem);
					SendMessage(hDlg, WM_NEXTDLGCTL, (WPARAM)hwndPictID, TRUE);
				}
				else
				{
					g_thisRoom->objects[g_objActive].data.e.where = (SInt16)wasPict;
					g_fileDirty = true;
					UpdateMenus(false);
					InvalidateRect(g_mainWindow, NULL, TRUE);
					GetThisRoomsObjRects();
					ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
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

		if (g_thisRoom->objects[g_objActive].data.e.type == kToggle)
		{
			hwndInitialFocus = GetDlgItem(hDlg, kToggleRadio);
			CheckRadioButton(hDlg, kToggleRadio, kForceOffRadio, kToggleRadio);
		}
		else if (g_thisRoom->objects[g_objActive].data.e.type == kForceOn)
		{
			hwndInitialFocus = GetDlgItem(hDlg, kForceOnRadio);
			CheckRadioButton(hDlg, kToggleRadio, kForceOffRadio, kForceOnRadio);
		}
		else if (g_thisRoom->objects[g_objActive].data.e.type == kForceOff)
		{
			hwndInitialFocus = GetDlgItem(hDlg, kForceOffRadio);
			CheckRadioButton(hDlg, kToggleRadio, kForceOffRadio, kForceOffRadio);
		}
		if (hwndInitialFocus == NULL)
			hwndInitialFocus = GetDlgItem(hDlg, kToggleRadio);

		if (g_thisRoom->objects[g_objActive].data.e.who == 255)
			EnableWindow(GetDlgItem(hDlg, kSwitchGotoButton), FALSE);

		if (g_retroLinkList[g_objActive].room == -1)
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
				g_thisRoom->objects[g_objActive].data.e.type = kToggle;
			else if (IsDlgButtonChecked(hDlg, kForceOnRadio))
				g_thisRoom->objects[g_objActive].data.e.type = kForceOn;
			else if (IsDlgButtonChecked(hDlg, kForceOffRadio))
				g_thisRoom->objects[g_objActive].data.e.type = kForceOff;
			g_fileDirty = true;
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
		if (g_retroLinkList[g_objActive].room == -1)
			ShowWindow(GetDlgItem(hDlg, kTriggerLinkedFrom), SW_HIDE);

		if (g_thisRoom->objects[g_objActive].data.e.who == 255)
			EnableWindow(GetDlgItem(hDlg, kTriggerGotoButton), FALSE);

		delayIs = g_thisRoom->objects[g_objActive].data.e.delay;
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
				delayIs = g_thisRoom->objects[g_objActive].data.e.delay;
				SetDlgItemInt(hDlg, kTriggerDelayItem, (UINT)delayIs, TRUE);
				hwndFocus = GetDlgItem(hDlg, kTriggerDelayItem);
				SendMessage(hDlg, WM_NEXTDLGCTL, (WPARAM)hwndFocus, TRUE);
			}
			else
			{
				g_thisRoom->objects[g_objActive].data.e.delay = (SInt16)delayIs;
				g_fileDirty = true;
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
		if (g_thisRoom->objects[g_objActive].data.f.initial)
			CheckDlgButton(hDlg, kLightInitialState, BST_CHECKED);
		else
			CheckDlgButton(hDlg, kLightInitialState, BST_UNCHECKED);

		if (g_retroLinkList[g_objActive].room == -1)
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
				g_thisRoom->objects[g_objActive].data.f.initial = true;
			else
				g_thisRoom->objects[g_objActive].data.f.initial = false;

			ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
			DrawThisRoomsObjects();
			InvalidateRect(g_mainWindow, NULL, TRUE);
			g_fileDirty = true;
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
		if (g_retroLinkList[g_objActive].room == -1)
			ShowWindow(GetDlgItem(hDlg, kApplianceLinkedFrom), SW_HIDE);

		if (g_thisRoom->objects[g_objActive].data.g.initial)
			CheckDlgButton(hDlg, kApplianceInitialState, BST_CHECKED);
		else
			CheckDlgButton(hDlg, kApplianceInitialState, BST_UNCHECKED);

		what = g_thisRoom->objects[g_objActive].what;
		if ((what == kShredder) || (what == kMacPlus) || (what == kTV) ||
			(what == kCoffee) || (what == kVCR) || (what == kMicrowave))
		{
			ShowWindow(GetDlgItem(hDlg, kApplianceDelay), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, kApplianceDelayLabel), SW_HIDE);
		}

		delay = g_thisRoom->objects[g_objActive].data.g.delay;
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
				delay = g_thisRoom->objects[g_objActive].data.g.delay;
				SetDlgItemInt(hDlg, kApplianceDelay, (UINT)delay, TRUE);
				SendMessage(hDlg, WM_NEXTDLGCTL,
					(WPARAM)GetDlgItem(hDlg, kApplianceDelay), TRUE);
			}
			else
			{
				g_thisRoom->objects[g_objActive].data.g.delay = (Byte)delay;
				if (IsDlgButtonChecked(hDlg, kApplianceInitialState))
					g_thisRoom->objects[g_objActive].data.g.initial = true;
				else
					g_thisRoom->objects[g_objActive].data.g.initial = false;
				g_fileDirty = true;
				UpdateMenus(false);
				InvalidateRect(g_mainWindow, NULL, TRUE);
				GetThisRoomsObjRects();
				ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
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
		if (g_retroLinkList[g_objActive].room == -1)
			ShowWindow(GetDlgItem(hDlg, kMicrowaveLinkedFrom), SW_HIDE);

		if (g_thisRoom->objects[g_objActive].data.g.initial)
			CheckDlgButton(hDlg, kMicrowaveInitialState, BST_CHECKED);
		else
			CheckDlgButton(hDlg, kMicrowaveInitialState, BST_UNCHECKED);

		kills = g_thisRoom->objects[g_objActive].data.g.byte0;
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
				g_thisRoom->objects[g_objActive].data.g.initial = true;
			else
				g_thisRoom->objects[g_objActive].data.g.initial = false;
			kills = 0;
			if (IsDlgButtonChecked(hDlg, kKillBandsCheckbox))
				kills += 1;
			if (IsDlgButtonChecked(hDlg, kKillBatteryCheckbox))
				kills += 2;
			if (IsDlgButtonChecked(hDlg, kKillFoilCheckbox))
				kills += 4;
			g_thisRoom->objects[g_objActive].data.g.byte0 = kills;

			g_fileDirty = true;
			UpdateMenus(false);
			InvalidateRect(g_mainWindow, NULL, TRUE);
			GetThisRoomsObjRects();
			ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
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
		if (g_retroLinkList[g_objActive].room == -1)
			ShowWindow(GetDlgItem(hDlg, kGreaseLinkedFrom), SW_HIDE);

		if (g_thisRoom->objects[g_objActive].data.c.initial)
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
				g_thisRoom->objects[g_objActive].data.c.initial = false;
			else
				g_thisRoom->objects[g_objActive].data.c.initial = true;
			g_fileDirty = true;
			UpdateMenus(false);
			InvalidateRect(g_mainWindow, NULL, TRUE);
			GetThisRoomsObjRects();
			ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
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
		switch (g_thisRoom->objects[g_objActive].data.c.points)
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

		if (g_retroLinkList[g_objActive].room == -1)
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
				g_thisRoom->objects[g_objActive].data.c.points = 100;
			}
			else if (IsDlgButtonChecked(hDlg, k300PtRadio))
			{
				g_thisRoom->objects[g_objActive].data.c.points = 300;
			}
			else if (IsDlgButtonChecked(hDlg, k500PtRadio))
			{
				g_thisRoom->objects[g_objActive].data.c.points = 500;
			}
			g_fileDirty = true;
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
		if (g_retroLinkList[g_objActive].room == -1)
			ShowWindow(GetDlgItem(hDlg, kTransLinkedFrom), SW_HIDE);

		if (g_thisRoom->objects[g_objActive].what != kDeluxeTrans)
		{
			ShowWindow(GetDlgItem(hDlg, kTransInitialState), SW_HIDE);
			hwndInitialFocus = GetDlgItem(hDlg, IDOK);
		}
		else
		{
			initialState = (g_thisRoom->objects[g_objActive].data.d.wide & 0xF0) >> 4;
			if (initialState)
				CheckDlgButton(hDlg, kTransInitialState, BST_CHECKED);
			else
				CheckDlgButton(hDlg, kTransInitialState, BST_UNCHECKED);
			hwndInitialFocus = GetDlgItem(hDlg, kTransInitialState);
		}

		if (g_thisRoom->objects[g_objActive].data.d.who == 255)
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
			if (g_thisRoom->objects[g_objActive].what == kDeluxeTrans)
			{
				if (IsDlgButtonChecked(hDlg, kTransInitialState))
					initialState = 0x1;
				else
					initialState = 0x0;
				g_thisRoom->objects[g_objActive].data.d.wide = initialState << 4;
			}
			g_fileDirty = true;
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
		if (g_retroLinkList[g_objActive].room == -1)
			ShowWindow(GetDlgItem(hDlg, kEnemyLinkedFrom), SW_HIDE);

		delay = g_thisRoom->objects[g_objActive].data.h.delay;
		SetDlgItemInt(hDlg, kEnemyDelayItem, (UINT)delay, TRUE);

		if (g_thisRoom->objects[g_objActive].data.h.initial)
			CheckDlgButton(hDlg, kEnemyInitialState, BST_CHECKED);
		else
			CheckDlgButton(hDlg, kEnemyInitialState, BST_UNCHECKED);

		if (g_thisRoom->objects[g_objActive].what == kBall)
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
				(g_thisRoom->objects[g_objActive].what != kBall))
			{
				MessageBeep(MB_ICONWARNING);
				delay = g_thisRoom->objects[g_objActive].data.h.delay;
				SetDlgItemInt(hDlg, kEnemyDelayItem, (UINT)delay, TRUE);
				SendMessage(hDlg, WM_NEXTDLGCTL,
					(WPARAM)GetDlgItem(hDlg, kEnemyDelayItem), TRUE);
			}
			else
			{
				if (IsDlgButtonChecked(hDlg, kEnemyInitialState))
					g_thisRoom->objects[g_objActive].data.h.initial = true;
				else
					g_thisRoom->objects[g_objActive].data.h.initial = false;
				if (g_thisRoom->objects[g_objActive].what != kBall)
					g_thisRoom->objects[g_objActive].data.h.delay = (Byte)delay;
				g_fileDirty = true;
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
		if (g_retroLinkList[g_objActive].room == -1)
			ShowWindow(GetDlgItem(hDlg, kFlowerLinkedFrom), SW_HIDE);

		flower = g_thisRoom->objects[g_objActive].data.i.pict;
		if ((flower < 0) || (flower >= kNumFlowers))
			flower = 0;
		CheckRadioButton(hDlg, kRadioFlower1, kRadioFlower6, kRadioFlowerBase + flower);

		CenterDialogOverOwner(hDlg);
		ParamDialogText(hDlg, (const DialogParams *)lParam);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case kFlowerLinkedFrom:
			for (flower = 0; flower < kNumFlowers; flower++)
				if (IsDlgButtonChecked(hDlg, kRadioFlowerBase + flower))
					break;
			if (flower >= kNumFlowers)
				flower = 0;
			if (flower != g_thisRoom->objects[g_objActive].data.i.pict)
			{
				Mac_InvalWindowRect(g_mainWindow, &g_thisRoom->objects[g_objActive].data.i.bounds);
				g_thisRoom->objects[g_objActive].data.i.bounds.right =
					g_thisRoom->objects[g_objActive].data.i.bounds.left +
					RectWide(&g_flowerSrc[flower]);
				g_thisRoom->objects[g_objActive].data.i.bounds.top =
					g_thisRoom->objects[g_objActive].data.i.bounds.bottom -
					RectTall(&g_flowerSrc[flower]);
				g_thisRoom->objects[g_objActive].data.i.pict = flower;
				Mac_InvalWindowRect(g_mainWindow, &g_thisRoom->objects[g_objActive].data.i.bounds);
				GetThisRoomsObjRects();
				ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
				DrawThisRoomsObjects();
				g_fileDirty = true;
				UpdateMenus(false);
				g_wasFlower = flower;
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
	PWSTR kindStrBuffer = NULL;
	PCWSTR kindStr;
	wchar_t distStr[16];
	INT_PTR result;

	NumToString(g_objActive + 1, numberStr, ARRAYSIZE(numberStr));
	GetObjectName(g_thisRoom->objects[g_objActive].what, &kindStrBuffer);
	kindStr = (kindStrBuffer != NULL) ? kindStrBuffer : L"";
	NumToString(g_thisRoom->objects[g_objActive].data.a.distance,
			distStr, ARRAYSIZE(distStr));

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	params.arg[2] = distStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kBlowerInfoDialogID),
		hwndOwner, BlowerFilter, (LPARAM)&params);

	free(kindStrBuffer);

	if (result == kBlowerLinkedFrom)
	{
		GoToObjectInRoomNum(g_retroLinkList[g_objActive].object,
			g_retroLinkList[g_objActive].room);
	}
}

//--------------------------------------------------------------  DoFurnitureObjectInfo

void DoFurnitureObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	PWSTR kindStrBuffer = NULL;
	PCWSTR kindStr;
	INT_PTR result;

	if (g_objActive == kInitialGliderSelected)
	{
		StringCchCopy(numberStr, ARRAYSIZE(numberStr), L"-");
		kindStr = L"Glider Begins";
	}
	else if (g_objActive == kLeftGliderSelected)
	{
		StringCchCopy(numberStr, ARRAYSIZE(numberStr), L"-");
		kindStr = L"New Glider (left)";
	}
	else if (g_objActive == kRightGliderSelected)
	{
		StringCchCopy(numberStr, ARRAYSIZE(numberStr), L"-");
		kindStr = L"New Glider (right)";
	}
	else
	{
		NumToString(g_objActive + 1, numberStr, ARRAYSIZE(numberStr));
		GetObjectName(g_thisRoom->objects[g_objActive].what, &kindStrBuffer);
		kindStr = (kindStrBuffer != NULL) ? kindStrBuffer : L"";
	}

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kFurnitureInfoDialogID),
		hwndOwner, FurnitureFilter, (LPARAM)&params);

	free(kindStrBuffer);

	if (result == kFurnitureLinkedFrom)
	{
		if ((g_objActive >= 0) && (g_objActive < kMaxRoomObs))
		{
			GoToObjectInRoomNum(g_retroLinkList[g_objActive].object,
				g_retroLinkList[g_objActive].room);
		}
	}
}

//--------------------------------------------------------------  DoCustPictObjectInfo

void DoCustPictObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	PWSTR kindStrBuffer = NULL;
	PCWSTR kindStr;

	NumToString(g_objActive + 1, numberStr, ARRAYSIZE(numberStr));
	GetObjectName(g_thisRoom->objects[g_objActive].what, &kindStrBuffer);
	kindStr = (kindStrBuffer != NULL) ? kindStrBuffer : L"";

	if (g_thisRoom->objects[g_objActive].what == kCustomPict)
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

	free(kindStrBuffer);
}

//--------------------------------------------------------------  DoSwitchObjectInfo

void DoSwitchObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	PWSTR kindStrBuffer = NULL;
	PCWSTR kindStr;
	wchar_t roomStr[32];
	wchar_t objStr[16];
	SInt16 floor, suite;
	INT_PTR result;

	floor = 0;
	suite = kRoomIsEmpty;

	NumToString(g_objActive + 1, numberStr, ARRAYSIZE(numberStr));
	GetObjectName(g_thisRoom->objects[g_objActive].what, &kindStrBuffer);
	kindStr = (kindStrBuffer != NULL) ? kindStrBuffer : L"";
	if (g_thisRoom->objects[g_objActive].data.e.where == -1)
	{
		StringCchCopy(roomStr, ARRAYSIZE(roomStr), L"none");
	}
	else
	{
		ExtractFloorSuite(&g_thisHouse,
			g_thisRoom->objects[g_objActive].data.e.where, &floor, &suite);
		StringCchPrintf(roomStr, ARRAYSIZE(roomStr), L"%d / %d", (int)floor, (int)suite);
	}

	if (g_thisRoom->objects[g_objActive].data.e.who == 255)
	{
		StringCchCopy(objStr, ARRAYSIZE(objStr), L"none");
	}
	else
	{
		NumToString((SInt32)g_thisRoom->objects[g_objActive].data.e.who + 1,
				objStr, ARRAYSIZE(objStr));
	}

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	params.arg[2] = roomStr;
	params.arg[3] = objStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kSwitchInfoDialogID),
		hwndOwner, SwitchFilter, (LPARAM)&params);

	free(kindStrBuffer);

	if (result == kLinkSwitchButton)
	{
		g_linkType = kSwitchLinkOnly;
		g_linkerIsSwitch = true;
		OpenLinkWindow();
		g_linkRoom = g_thisRoomNumber;
		g_linkObject = (Byte)g_objActive;
		DeselectObject();
	}
	else if (result == kSwitchGotoButton)
	{
		GoToObjectInRoom(g_thisRoom->objects[g_objActive].data.e.who, floor, suite);
	}
	else if (result == kSwitchLinkedFrom)
	{
		GoToObjectInRoomNum(g_retroLinkList[g_objActive].object,
			g_retroLinkList[g_objActive].room);
	}
}

//--------------------------------------------------------------  DoTriggerObjectInfo

void DoTriggerObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	PWSTR kindStrBuffer = NULL;
	PCWSTR kindStr;
	wchar_t roomStr[32];
	wchar_t objStr[16];
	SInt16 floor, suite;
	INT_PTR result;

	floor = 0;
	suite = kRoomIsEmpty;

	NumToString(g_objActive + 1, numberStr, ARRAYSIZE(numberStr));
	GetObjectName(g_thisRoom->objects[g_objActive].what, &kindStrBuffer);
	kindStr = (kindStrBuffer != NULL) ? kindStrBuffer : L"";
	if (g_thisRoom->objects[g_objActive].data.e.where == -1)
	{
		StringCchCopy(roomStr, ARRAYSIZE(roomStr), L"none");
	}
	else
	{
		ExtractFloorSuite(&g_thisHouse,
			g_thisRoom->objects[g_objActive].data.e.where, &floor, &suite);
		StringCchPrintf(roomStr, ARRAYSIZE(roomStr), L"%d / %d", (int)floor, (int)suite);
	}

	if (g_thisRoom->objects[g_objActive].data.e.who == 255)
	{
		StringCchCopy(objStr, ARRAYSIZE(objStr), L"none");
	}
	else
	{
		NumToString((SInt32)g_thisRoom->objects[g_objActive].data.e.who + 1,
				objStr, ARRAYSIZE(objStr));
	}

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	params.arg[2] = roomStr;
	params.arg[3] = objStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kTriggerInfoDialogID),
		hwndOwner, TriggerFilter, (LPARAM)&params);

	free(kindStrBuffer);

	if (result == kLinkTriggerButton)
	{
		g_linkType = kTriggerLinkOnly;
		g_linkerIsSwitch = true;
		OpenLinkWindow();
		g_linkRoom = g_thisRoomNumber;
		g_linkObject = (Byte)g_objActive;
		DeselectObject();
	}
	else if (result == kTriggerGotoButton)
	{
		GoToObjectInRoom(g_thisRoom->objects[g_objActive].data.e.who, floor, suite);
	}
	else if (result == kTriggerLinkedFrom)
	{
		GoToObjectInRoomNum(g_retroLinkList[g_objActive].object,
			g_retroLinkList[g_objActive].room);
	}
}

//--------------------------------------------------------------  DoLightObjectInfo

void DoLightObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	PWSTR kindStrBuffer = NULL;
	PCWSTR kindStr;
	INT_PTR result;

	NumToString(g_objActive + 1, numberStr, ARRAYSIZE(numberStr));
	GetObjectName(g_thisRoom->objects[g_objActive].what, &kindStrBuffer);
	kindStr = (kindStrBuffer != NULL) ? kindStrBuffer : L"";

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kLightInfoDialogID),
		hwndOwner, LightFilter, (LPARAM)&params);

	free(kindStrBuffer);

	if (result == kLightLinkedFrom)
	{
		GoToObjectInRoomNum(g_retroLinkList[g_objActive].object,
			g_retroLinkList[g_objActive].room);
	}
}

//--------------------------------------------------------------  DoApplianceObjectInfo

void DoApplianceObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	PWSTR kindStrBuffer = NULL;
	PCWSTR kindStr;
	INT_PTR result;

	NumToString(g_objActive + 1, numberStr, ARRAYSIZE(numberStr));
	GetObjectName(g_thisRoom->objects[g_objActive].what, &kindStrBuffer);
	kindStr = (kindStrBuffer != NULL) ? kindStrBuffer : L"";

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kApplianceInfoDialogID),
		hwndOwner, ApplianceFilter, (LPARAM)&params);

	free(kindStrBuffer);

	if (result == kApplianceLinkedFrom)
	{
		GoToObjectInRoomNum(g_retroLinkList[g_objActive].object,
			g_retroLinkList[g_objActive].room);
	}
}

//--------------------------------------------------------------  DoMicrowaveObjectInfo

void DoMicrowaveObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	PWSTR kindStrBuffer = NULL;
	PCWSTR kindStr;
	INT_PTR result;

	NumToString(g_objActive + 1, numberStr, ARRAYSIZE(numberStr));
	GetObjectName(g_thisRoom->objects[g_objActive].what, &kindStrBuffer);
	kindStr = (kindStrBuffer != NULL) ? kindStrBuffer : L"";

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kMicrowaveInfoDialogID),
		hwndOwner, MicrowaveFilter, (LPARAM)&params);

	free(kindStrBuffer);

	if (result == kMicrowaveLinkedFrom)
	{
		GoToObjectInRoomNum(g_retroLinkList[g_objActive].object,
			g_retroLinkList[g_objActive].room);
	}
}

//--------------------------------------------------------------  DoGreaseObjectInfo

void DoGreaseObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	PWSTR kindStrBuffer = NULL;
	PCWSTR kindStr;
	INT_PTR result;

	NumToString(g_objActive + 1, numberStr, ARRAYSIZE(numberStr));
	GetObjectName(g_thisRoom->objects[g_objActive].what, &kindStrBuffer);
	kindStr = (kindStrBuffer != NULL) ? kindStrBuffer : L"";

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kGreaseInfoDialogID),
		hwndOwner, GreaseFilter, (LPARAM)&params);

	free(kindStrBuffer);

	if (result == kGreaseLinkedFrom)
	{
		GoToObjectInRoomNum(g_retroLinkList[g_objActive].object,
			g_retroLinkList[g_objActive].room);
	}
}

//--------------------------------------------------------------  DoInvisBonusObjectInfo

void DoInvisBonusObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	PWSTR kindStrBuffer = NULL;
	PCWSTR kindStr;
	INT_PTR result;

	NumToString(g_objActive + 1, numberStr, ARRAYSIZE(numberStr));
	GetObjectName(g_thisRoom->objects[g_objActive].what, &kindStrBuffer);
	kindStr = (kindStrBuffer != NULL) ? kindStrBuffer : L"";

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kInvisBonusInfoDialogID),
		hwndOwner, InvisBonusFilter, (LPARAM)&params);

	free(kindStrBuffer);

	if (result == kInvisBonusLinkedFrom)
	{
		GoToObjectInRoomNum(g_retroLinkList[g_objActive].object,
			g_retroLinkList[g_objActive].room);
	}
}

//--------------------------------------------------------------  DoTransObjectInfo

void DoTransObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	PWSTR kindStrBuffer = NULL;
	PCWSTR kindStr;
	wchar_t roomStr[32];
	wchar_t objStr[16];
	SInt16 floor, suite;
	INT_PTR result;

	floor = 0;
	suite = kRoomIsEmpty;

	NumToString(g_objActive + 1, numberStr, ARRAYSIZE(numberStr));
	GetObjectName(g_thisRoom->objects[g_objActive].what, &kindStrBuffer);
	kindStr = (kindStrBuffer != NULL) ? kindStrBuffer : L"";
	if (g_thisRoom->objects[g_objActive].data.d.where == -1)
	{
		StringCchCopy(roomStr, ARRAYSIZE(roomStr), L"none");
	}
	else
	{
		ExtractFloorSuite(&g_thisHouse,
			g_thisRoom->objects[g_objActive].data.d.where, &floor, &suite);
		StringCchPrintf(roomStr, ARRAYSIZE(roomStr), L"%d / %d", (int)floor, (int)suite);
	}

	if (g_thisRoom->objects[g_objActive].data.d.who == 255)
	{
		StringCchCopy(objStr, ARRAYSIZE(objStr), L"none");
	}
	else
	{
		NumToString((SInt32)g_thisRoom->objects[g_objActive].data.d.who + 1,
				objStr, ARRAYSIZE(objStr));
	}

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	params.arg[2] = roomStr;
	params.arg[3] = objStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kTransInfoDialogID),
		hwndOwner, TransFilter, (LPARAM)&params);

	free(kindStrBuffer);

	if (result == kLinkTransButton)
	{
		g_linkType = kTransportLinkOnly;
		g_linkerIsSwitch = false;
		OpenLinkWindow();
		g_linkRoom = g_thisRoomNumber;
		g_linkObject = (Byte)g_objActive;
		DeselectObject();
	}
	else if (result == kTransGotoButton)
	{
		GoToObjectInRoom(g_thisRoom->objects[g_objActive].data.d.who, floor, suite);
	}
	else if (result == kTransLinkedFrom)
	{
		GoToObjectInRoomNum(g_retroLinkList[g_objActive].object,
			g_retroLinkList[g_objActive].room);
	}
}

//--------------------------------------------------------------  DoEnemyObjectInfo

void DoEnemyObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	PWSTR kindStrBuffer = NULL;
	PCWSTR kindStr;
	INT_PTR result;

	NumToString(g_objActive + 1, numberStr, ARRAYSIZE(numberStr));
	GetObjectName(g_thisRoom->objects[g_objActive].what, &kindStrBuffer);
	kindStr = (kindStrBuffer != NULL) ? kindStrBuffer : L"";

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kEnemyInfoDialogID),
		hwndOwner, EnemyFilter, (LPARAM)&params);

	free(kindStrBuffer);

	if (result == kEnemyLinkedFrom)
	{
		GoToObjectInRoomNum(g_retroLinkList[g_objActive].object,
			g_retroLinkList[g_objActive].room);
	}
}

//--------------------------------------------------------------  DoFlowerObjectInfo

void DoFlowerObjectInfo (HWND hwndOwner)
{
	DialogParams params = { 0 };
	wchar_t numberStr[16];
	PWSTR kindStrBuffer = NULL;
	PCWSTR kindStr;
	INT_PTR result;

	NumToString(g_objActive + 1, numberStr, ARRAYSIZE(numberStr));
	GetObjectName(g_thisRoom->objects[g_objActive].what, &kindStrBuffer);
	kindStr = (kindStrBuffer != NULL) ? kindStrBuffer : L"";

	params.arg[0] = numberStr;
	params.arg[1] = kindStr;
	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kFlowerInfoDialogID),
		hwndOwner, FlowerFilter, (LPARAM)&params);

	free(kindStrBuffer);

	if (result == kFlowerLinkedFrom)
	{
		GoToObjectInRoomNum(g_retroLinkList[g_objActive].object,
			g_retroLinkList[g_objActive].room);
	}
}

//--------------------------------------------------------------  DoObjectInfo

void DoObjectInfo (HWND hwndOwner)
{
	if ((g_objActive == kInitialGliderSelected) ||
		(g_objActive == kLeftGliderSelected) ||
		(g_objActive == kRightGliderSelected))
	{
		DoFurnitureObjectInfo(hwndOwner);
		return;
	}

	switch (g_thisRoom->objects[g_objActive].what)
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
