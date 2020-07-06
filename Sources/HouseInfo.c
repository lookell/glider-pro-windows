#include "HouseInfo.h"

//============================================================================
//----------------------------------------------------------------------------
//                                HouseInfo.c
//----------------------------------------------------------------------------
//============================================================================


#include "DialogUtils.h"
#include "HighScores.h"
#include "House.h"
#include "HouseIO.h"
#include "MacTypes.h"
#include "Menu.h"
#include "Play.h"
#include "ResourceIDs.h"
#include "Room.h"


#define kBannerTextItem			1004
#define kLockHouseButton		1006
#define kClearScoresButton		1009
#define kTrailerTextItem		1011
#define kNoPhoneCheck			1014
#define kBannerNCharsItem		1015
#define kTrailerNCharsItem		1016
#define kHouseSizeItem			1018


SInt32 CountTotalHousePoints (void);
INT_PTR CALLBACK HouseFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
Boolean WarnLockingHouse (HWND ownerWindow);
void HowToZeroScores (HWND ownerWindow);


#ifndef COMPILEDEMO

//==============================================================  Functions
//--------------------------------------------------------------  CountTotalHousePoints

// The following functions all handle the "House Info" dialog in the editor.

SInt32 CountTotalHousePoints (void)
{
	SInt32		pointTotal;
	SInt16		numRooms, h, i;

	pointTotal = (SInt32)RealRoomNumberCount() * (SInt32)kRoomVisitScore;

	numRooms = thisHouse.nRooms;
	for (i = 0; i < numRooms; i++)
	{
		if (thisHouse.rooms[i].suite != kRoomIsEmpty)
		{
			for (h = 0; h < kMaxRoomObs; h++)
			{
				switch (thisHouse.rooms[i].objects[h].what)
				{
					case kRedClock:
					pointTotal += kRedClockPoints;
					break;

					case kBlueClock:
					pointTotal += kBlueClockPoints;
					break;

					case kYellowClock:
					pointTotal += kYellowClockPoints;
					break;

					case kCuckoo:
					pointTotal += kCuckooClockPoints;
					break;

					case kStar:
					pointTotal += kStarPoints;
					break;

					case kInvisBonus:
					pointTotal += thisHouse.rooms[i].objects[h].data.c.points;
					break;

					default:
					break;
				}
			}
		}
	}

	return (pointTotal);
}

//--------------------------------------------------------------  HouseFilter

static void SendEditChangeNotification(HWND hwndParent, int itemID)
{
	SendMessage(hwndParent, WM_COMMAND, MAKEWPARAM(itemID, EN_CHANGE),
			(LPARAM)GetDlgItem(hwndParent, itemID));
}

INT_PTR CALLBACK HouseFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		CenterOverOwner(hDlg);
		ParamDialogText(hDlg, (const DialogParams *)lParam);

		// NOTE: Multiline edit controls don't send EN_CHANGE notifications
		// to their parent when the text is set via WM_SETTEXT, so we'll send
		// these notifications manually.
		SetDialogString(hDlg, kBannerTextItem, thisHouse.banner);
		SendEditChangeNotification(hDlg, kBannerTextItem);
		SetDialogString(hDlg, kTrailerTextItem, thisHouse.trailer);
		SendEditChangeNotification(hDlg, kTrailerTextItem);

		SetDlgItemInt(hDlg, kHouseSizeItem, CountTotalHousePoints(), TRUE);
		if (phoneBitSet)
			CheckDlgButton(hDlg, kNoPhoneCheck, BST_CHECKED);
		else
			CheckDlgButton(hDlg, kNoPhoneCheck, BST_UNCHECKED);
		return TRUE;

	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{
		case IDOK:
			GetDialogString(hDlg, kBannerTextItem,
					thisHouse.banner, ARRAYSIZE(thisHouse.banner));
			GetDialogString(hDlg, kTrailerTextItem,
					thisHouse.trailer, ARRAYSIZE(thisHouse.trailer));

			phoneBitSet = (IsDlgButtonChecked(hDlg, kNoPhoneCheck) != BST_UNCHECKED);
			if (phoneBitSet)
				thisHouse.flags = thisHouse.flags | 0x00000002;
			else
				thisHouse.flags = thisHouse.flags & 0xFFFFFFFD;

			fileDirty = true;
			UpdateMenus(false);
			EndDialog(hDlg, IDOK);
			break;

		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;

		case kLockHouseButton:
			if (WarnLockingHouse(hDlg))
			{
				changeLockStateOfHouse = true;
				saveHouseLocked = true;
				fileDirty = true;
				UpdateMenus(false);
			}
			break;

		case kClearScoresButton:
			HowToZeroScores(hDlg);
			break;

		case kBannerTextItem:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				int nChars = GetDialogStringLen(hDlg, kBannerTextItem);
				SetDlgItemInt(hDlg, kBannerNCharsItem, nChars, FALSE);
			}
			break;

		case kTrailerTextItem:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				int nChars = GetDialogStringLen(hDlg, kTrailerTextItem);
				SetDlgItemInt(hDlg, kTrailerNCharsItem, nChars, FALSE);
			}
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  DoHouseInfo

void DoHouseInfo (HWND ownerWindow)
{
	DialogParams params = { 0 };
	SInt32 h, v;
	SInt16 numRooms, version;
	wchar_t versStr[32];
	wchar_t loVers[32];
	wchar_t nRoomsStr[32];

	numRooms = RealRoomNumberCount();
	version = thisHouse.version;
	if (!noRoomAtAll)
	{
		h = (SInt32)thisHouse.rooms[thisHouse.firstRoom].suite;
		v = (SInt32)thisHouse.rooms[thisHouse.firstRoom].floor;
	}

	// Convert version to two strings, the 1's and 1/10th's part.
	StringCchPrintf(versStr, ARRAYSIZE(versStr), L"%ld", (long)(version >> 8));
	StringCchPrintf(loVers, ARRAYSIZE(loVers), L"%ld", (long)(version % 0x0100));
	// Number of rooms -> string.
	StringCchPrintf(nRoomsStr, ARRAYSIZE(nRoomsStr), L"%ld", (long)numRooms);

	params.arg[0] = versStr;
	params.arg[1] = loVers;
	params.arg[2] = nRoomsStr;
	DialogBoxParam(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(kHouseInfoDialogID),
			ownerWindow, HouseFilter, (LPARAM)&params);
}

//--------------------------------------------------------------  WarnLockingHouse

Boolean WarnLockingHouse (HWND ownerWindow)
{
	SInt16 hitWhat;

	hitWhat = Alert(kLockHouseAlert, ownerWindow, NULL);
	return (hitWhat == IDOK);
}

//--------------------------------------------------------------  HowToZeroScores

void HowToZeroScores (HWND ownerWindow)
{
	SInt16 hitWhat;

	hitWhat = Alert(kZeroScoresAlert, ownerWindow, NULL);
	switch (hitWhat)
	{
		case 1002:	// zero all
		ZeroHighScores(&thisHouse);
		fileDirty = true;
		UpdateMenus(false);
		break;

		case 1003:	// zero all but highest
		ZeroAllButHighestScore(&thisHouse);
		fileDirty = true;
		UpdateMenus(false);
		break;
	}
}

#endif

