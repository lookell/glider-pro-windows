//============================================================================
//----------------------------------------------------------------------------
//                                  House.c
//----------------------------------------------------------------------------
//============================================================================

#include "House.h"

#include "DialogUtils.h"
#include "FileError.h"
#include "HighScores.h"
#include "HouseIO.h"
#include "Link.h"
#include "MainWindow.h"
#include "Map.h"
#include "Menu.h"
#include "ObjectEdit.h"
#include "Objects.h"
#include "Play.h"
#include "RectUtils.h"
#include "ResourceIDs.h"
#include "ResourceLoader.h"
#include "Room.h"
#include "RoomGraphics.h"
#include "SavedGames.h"
#include "SelectHouse.h"
#include "StringUtils.h"
#include "Utilities.h"
#include "WindowUtils.h"

#include <commdlg.h>
#include <strsafe.h>

#include <stdlib.h>

#define kGoToFirstRadio     1002
#define kGoToPrevRadio      1003
#define kGoToFSRadio        1004
#define kFloorEditText      1005
#define kSuiteEditText      1006

void SortRoomsObjects (SInt16 which);
INT_PTR CALLBACK GoToFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

houseType g_thisHouse;
linksPtr g_linksList;
SInt16 g_wasFloor;
SInt16 g_wasSuite;
retroLink g_retroLinkList[kMaxRoomObs];
Boolean g_houseUnlocked;
Str32 g_thisHouseName;

static SInt16 g_srcLocations[kMaxRoomObs];
static SInt16 g_destLocations[kMaxRoomObs];

//==============================================================  Functions
//--------------------------------------------------------------  CreateNewHouse
// Called to create a new house file.

Boolean CreateNewHouse (HWND hwndOwner)
{
	OPENFILENAME ofn;
	wchar_t houseDirPath[MAX_PATH];
	wchar_t houseFilePath[MAX_PATH] = { 0 };
	houseSpec theSpec;

	// Get the new house's file name from the user

	if (!GetHouseFolderPath(houseDirPath, ARRAYSIZE(houseDirPath)))
	{
		houseDirPath[0] = L'\0';
	}

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwndOwner;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = L"Glider PRO House (*.glh)\0*.glh\0\0";
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = houseFilePath;
	ofn.nMaxFile = ARRAYSIZE(houseFilePath);
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = (houseDirPath[0] != L'\0') ? houseDirPath : NULL;
	ofn.lpstrTitle = NULL;
	ofn.Flags = OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = L"glh";
	ofn.lCustData = 0;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;
	if (!GetSaveFileName(&ofn))
	{
		return false;
	}

	// Fill out the house spec with its path and name

	StringCchCopy(theSpec.path, ARRAYSIZE(theSpec.path), ofn.lpstrFile);
	// chop off extension
	ofn.lpstrFile[ofn.nFileExtension - 1] = L'\0';
	StringCchCopy(theSpec.houseName, ARRAYSIZE(theSpec.houseName),
		&ofn.lpstrFile[ofn.nFileOffset]);
	MacFromWinString(theSpec.name, ARRAYSIZE(theSpec.name), theSpec.houseName);
	theSpec.iconIndex = 0;

	// Create the empty house file

	if (FAILED(Gp_CreateHouseFile(theSpec.path)))
	{
		YellowAlert(hwndOwner, kYellowFailedWrite, 0);
		return false;
	}

	// Add the new house to the house list and open it for business

	if (g_houseOpen)
	{
		if (!CloseHouse(hwndOwner))
		{
			return false;
		}
	}

	PasStringCopy(theSpec.name, g_thisHouseName);
	AddExtraHouse(&theSpec);
	BuildHouseList(hwndOwner);
	if (!OpenHouse(hwndOwner))
	{
		return false;
	}

	return true;
}

//--------------------------------------------------------------  InitializeEmptyHouse
// Initializes all the structures for an empty (new) house.

void InitializeEmptyHouse (void)
{
	wchar_t tempStr[256];

	free(g_thisHouse.rooms);

	ZeroMemory(&g_thisHouse, sizeof(g_thisHouse));
	g_thisHouse.version = kHouseVersion;
	g_thisHouse.timeStamp = 0L;
	g_thisHouse.flags = 0L;
	g_thisHouse.initial.h = 32;
	g_thisHouse.initial.v = 32;
	GetLocalizedString(11, tempStr, ARRAYSIZE(tempStr));
	MacFromWinString(g_thisHouse.banner, ARRAYSIZE(g_thisHouse.banner), tempStr);
	GetLocalizedString(12, tempStr, ARRAYSIZE(tempStr));
	MacFromWinString(g_thisHouse.trailer, ARRAYSIZE(g_thisHouse.trailer), tempStr);
	ZeroHighScores(&g_thisHouse);
	g_thisHouse.hasGame = false;
	g_thisHouse.firstRoom = -1;
	g_thisHouse.nRooms = 0;
	g_thisHouse.rooms = NULL;

	g_wardBitSet = false;
	g_phoneBitSet = false;

	g_mapLeftRoom = 60;
	g_mapTopRoom = 50;
	g_thisRoomNumber = kRoomIsEmpty;
	g_previousRoom = -1;
	g_houseUnlocked = true;
	OpenMapWindow();
	UpdateMapWindow();
	g_noRoomAtAll = true;
	g_fileDirty = true;
	UpdateMenus(false);
	ReflectCurrentRoom(true);
}

//--------------------------------------------------------------  RealRoomNumberCount
// Returns the real number of rooms in a house (some rooms may still
// be place-holders - they were deleted earlier and are flagged as
// deleted but still occupy space in the file).

SInt16 RealRoomNumberCount (const houseType *house)
{
	SInt16 realRoomCount, i;

	realRoomCount = house->nRooms;
	if (realRoomCount != 0)
	{
		for (i = 0; i < house->nRooms; i++)
		{
			if (house->rooms[i].suite == kRoomIsEmpty)
			{
				realRoomCount--;
			}
		}
	}

	return (realRoomCount);
}

//--------------------------------------------------------------  GetFirstRoomNumber
// Returns the room number (indicee into house file) of the room where
// the player is to begin.

SInt16 GetFirstRoomNumber (void)
{
	SInt16 firstRoom;

	if (g_thisHouse.nRooms <= 0)
	{
		firstRoom = -1;
		g_noRoomAtAll = true;
	}
	else
	{
		firstRoom = g_thisHouse.firstRoom;
		if ((firstRoom >= g_thisHouse.nRooms) || (firstRoom < 0))
			firstRoom = 0;
	}

	return (firstRoom);
}

//--------------------------------------------------------------  WhereDoesGliderBegin
// Returns a rectangle indicating where in the first room the player's
// glider is to appear.

void WhereDoesGliderBegin (Rect *theRect, SInt16 mode)
{
	Point initialPt;

	initialPt.h = 0;
	initialPt.v = 0;
	if (mode == kResumeGameMode)
		initialPt = g_smallGame.where;
	else if (mode == kNewGameMode)
		initialPt = g_thisHouse.initial;

	QSetRect(theRect, 0, 0, kGliderWide, kGliderHigh);
	QOffsetRect(theRect, initialPt.h, initialPt.v);
}

//--------------------------------------------------------------  CountHouseLinks
// Counts up the number of linked objects in a house.

SInt16 CountHouseLinks (void)
{
	SInt16 numRooms, numLinks;
	SInt16 r, i, what;

	numLinks = 0;
	numRooms = g_thisHouse.nRooms;

	for (r = 0; r < numRooms; r++)
	{
		for (i = 0; i < kMaxRoomObs; i++)
		{
			what = g_thisHouse.rooms[r].objects[i].what;
			switch (what)
			{
				case kLightSwitch:
				case kMachineSwitch:
				case kThermostat:
				case kPowerSwitch:
				case kKnifeSwitch:
				case kInvisSwitch:
				case kTrigger:
				case kLgTrigger:
				if (g_thisHouse.rooms[r].objects[i].data.e.where != -1)
					numLinks++;
				break;

				case kMailboxLf:
				case kMailboxRt:
				case kFloorTrans:
				case kCeilingTrans:
				case kInvisTrans:
				case kDeluxeTrans:
				if (g_thisHouse.rooms[r].objects[i].data.d.where != -1)
					numLinks++;
				break;
			}
		}
	}

	return (numLinks);
}

//--------------------------------------------------------------  GenerateLinksList
// Generates a list of all objects that have links and what rooms
// and objects they are linked to.  It is called in order to preserve
// the links if the objects or rooms in a house are to be shuffled
// around.

void GenerateLinksList (void)
{
	objectType thisObject;
	SInt16 numLinks, numRooms, r, i, what;
	SInt16 floor, suite, roomLinked, objectLinked;

	numRooms = g_thisHouse.nRooms;
	numLinks = 0;

	for (r = 0; r < numRooms; r++)
	{
		for (i = 0; i < kMaxRoomObs; i++)
		{
			what = g_thisHouse.rooms[r].objects[i].what;
			switch (what)
			{
				case kLightSwitch:
				case kMachineSwitch:
				case kThermostat:
				case kPowerSwitch:
				case kKnifeSwitch:
				case kInvisSwitch:
				case kTrigger:
				case kLgTrigger:
				thisObject = g_thisHouse.rooms[r].objects[i];
				if (thisObject.data.e.where != -1)
				{
					ExtractFloorSuite(&g_thisHouse, thisObject.data.e.where, &floor, &suite);
					roomLinked = GetRoomNumber(&g_thisHouse, floor, suite);
					objectLinked = (SInt16)thisObject.data.e.who;
					g_linksList[numLinks].srcRoom = r;
					g_linksList[numLinks].srcObj = i;
					g_linksList[numLinks].destRoom = roomLinked;
					g_linksList[numLinks].destObj = objectLinked;
					numLinks++;
				}
				break;

				case kMailboxLf:
				case kMailboxRt:
				case kFloorTrans:
				case kCeilingTrans:
				case kInvisTrans:
				case kDeluxeTrans:
				thisObject = g_thisHouse.rooms[r].objects[i];
				if (thisObject.data.d.where != -1)
				{
					ExtractFloorSuite(&g_thisHouse, thisObject.data.d.where, &floor, &suite);
					roomLinked = GetRoomNumber(&g_thisHouse, floor, suite);
					objectLinked = (SInt16)thisObject.data.d.who;
					g_linksList[numLinks].srcRoom = r;
					g_linksList[numLinks].srcObj = i;
					g_linksList[numLinks].destRoom = roomLinked;
					g_linksList[numLinks].destObj = objectLinked;
					numLinks++;
				}
				break;
			}
		}
	}
}

//--------------------------------------------------------------  SortRoomObjects
// I'm a little fuzzy on what this does.

void SortRoomsObjects (SInt16 which)
{
	SInt16 probe, probe2, room, obj;
	Boolean busy, looking;

	if (which < 0 || which >= g_thisHouse.nRooms)
		return;

	busy = true;
	probe = 0;

	do
	{
		if (g_thisHouse.rooms[which].objects[probe].what == kObjectIsEmpty)
		{
			looking = true;
			probe2 = probe + 1;  // begin by looking at the next object
			do
			{
				if (g_thisHouse.rooms[which].objects[probe2].what != kObjectIsEmpty)
				{
					g_thisHouse.rooms[which].objects[probe] =
							g_thisHouse.rooms[which].objects[probe2];
					g_thisHouse.rooms[which].objects[probe2].what = kObjectIsEmpty;
					if (g_srcLocations[probe2] != -1)
						g_linksList[g_srcLocations[probe2]].srcObj = probe;
					if (g_destLocations[probe2] != -1)
					{
						g_linksList[g_destLocations[probe2]].destObj = probe;
						room = g_linksList[g_destLocations[probe2]].srcRoom;
						obj = g_linksList[g_destLocations[probe2]].srcObj;
						if (ObjectIsLinkTransport(&g_thisHouse.rooms[room].objects[obj]))
						{
							g_thisHouse.rooms[room].objects[obj].data.d.who = (Byte)probe;
						}
						else if (ObjectIsLinkSwitch(&g_thisHouse.rooms[room].objects[obj]))
						{
							g_thisHouse.rooms[room].objects[obj].data.e.who = (Byte)probe;
						}
					}
					g_fileDirty = true;
					looking = false;
				}
				probe2++;
				if ((probe2 >= kMaxRoomObs) && (looking))
				{
					looking = false;
					busy = false;
				}
			}
			while (looking);
		}
		probe++;
		if (probe >= (kMaxRoomObs - 1))
			busy = false;
	}
	while (busy);
}

//--------------------------------------------------------------  SortHouseObjects
// I'm a little fuzzy on what this does exactly either.

void SortHouseObjects (void)
{
	SInt16 numLinks, numRooms, r, i, l;

	CopyThisRoomToRoom();

	numLinks = CountHouseLinks();
	if (numLinks == 0)
		return;

	g_linksList = NULL;
	g_linksList = (linksPtr)calloc(numLinks, sizeof(*g_linksList));
	if (g_linksList == NULL)
		RedAlert(kErrNoMemory);

	GenerateLinksList();

	numRooms = g_thisHouse.nRooms;

	for (r = 0; r < numRooms; r++)
	{
		for (i = 0; i < kMaxRoomObs; i++)  // initialize arrays
		{
			g_srcLocations[i] = -1;
			g_destLocations[i] = -1;
		}

		for (i = 0; i < kMaxRoomObs; i++)  // walk object list
		{
			for (l = 0; l < numLinks; l++)  // walk link list
			{
				if ((g_linksList[l].srcRoom == r) && (g_linksList[l].srcObj == i))
					g_srcLocations[i] = l;
				if ((g_linksList[l].destRoom == r) && (g_linksList[l].destObj == i))
					g_destLocations[i] = l;
			}
		}
		SortRoomsObjects(r);
	}

	free(g_linksList);
	ForceThisRoom(g_thisRoomNumber);
}

//--------------------------------------------------------------  CountRoomsVisited
// Goes through and counts the number of rooms a player has been to in
// the current game.

SInt16 CountRoomsVisited (void)
{
	SInt16 numRooms, r, count;

	numRooms = g_thisHouse.nRooms;
	count = 0;

	for (r = 0; r < numRooms; r++)
	{
		if (g_thisHouse.rooms[r].visited)
			count++;
	}

	return (count);
}

//--------------------------------------------------------------  GenerateRetroLinks
// Walk entire house looking for objects which are linked to objects
// in the current room.

void GenerateRetroLinks (void)
{
	objectType thisObject;
	SInt16 i, r, numRooms, floor, suite;
	SInt16 what, roomLinked, objectLinked;

	for (i = 0; i < kMaxRoomObs; i++)  // Initialize array.
		g_retroLinkList[i].room = -1;

	numRooms = g_thisHouse.nRooms;

	for (r = 0; r < numRooms; r++)
	{
		for (i = 0; i < kMaxRoomObs; i++)
		{
			what = g_thisHouse.rooms[r].objects[i].what;
			switch (what)
			{
				case kLightSwitch:
				case kMachineSwitch:
				case kThermostat:
				case kPowerSwitch:
				case kKnifeSwitch:
				case kInvisSwitch:
				case kTrigger:
				case kLgTrigger:
				thisObject = g_thisHouse.rooms[r].objects[i];
				if (thisObject.data.e.where != -1)
				{
					ExtractFloorSuite(&g_thisHouse, thisObject.data.e.where, &floor, &suite);
					roomLinked = GetRoomNumber(&g_thisHouse, floor, suite);
					if (roomLinked == g_thisRoomNumber)
					{
						objectLinked = (SInt16)thisObject.data.e.who;
						if ((objectLinked >= 0 && objectLinked < kMaxRoomObs) &&
							(g_retroLinkList[objectLinked].room == -1))
						{
							g_retroLinkList[objectLinked].room = r;
							g_retroLinkList[objectLinked].object = i;
						}
					}
				}
				break;

				case kMailboxLf:
				case kMailboxRt:
				case kFloorTrans:
				case kCeilingTrans:
				case kInvisTrans:
				case kDeluxeTrans:
				thisObject = g_thisHouse.rooms[r].objects[i];
				if (thisObject.data.d.where != -1)
				{
					ExtractFloorSuite(&g_thisHouse, thisObject.data.d.where, &floor, &suite);
					roomLinked = GetRoomNumber(&g_thisHouse, floor, suite);
					if (roomLinked == g_thisRoomNumber)
					{
						objectLinked = (SInt16)thisObject.data.d.who;
						if ((objectLinked >= 0 && objectLinked < kMaxRoomObs) &&
							(g_retroLinkList[objectLinked].room == -1))
						{
							g_retroLinkList[objectLinked].room = r;
							g_retroLinkList[objectLinked].object = i;
						}
					}
				}
				break;
			}
		}
	}
}

//--------------------------------------------------------------  GoToFilter
// Dialog filter for the "Go To Room..." dialog.

INT_PTR CALLBACK GoToFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	BOOL validNumber;
	SInt16 *roomToGoTo = (SInt16 *)GetWindowLongPtr(hDlg, DWLP_USER);
	HWND editCtrl;

	switch (message)
	{
	case WM_INITDIALOG:
		CenterDialogOverOwner(hDlg);
		SetWindowLongPtr(hDlg, DWLP_USER, lParam);

		if (GetFirstRoomNumber() == g_thisRoomNumber)
			EnableWindow(GetDlgItem(hDlg, kGoToFirstRadio), FALSE);
		if ((!RoomNumExists(&g_thisHouse, g_previousRoom)) ||
			(g_previousRoom == g_thisRoomNumber))
		{
			EnableWindow(GetDlgItem(hDlg, kGoToPrevRadio), FALSE);
		}

		CheckRadioButton(hDlg, kGoToFirstRadio, kGoToFSRadio, kGoToFSRadio);
		SetDlgItemInt(hDlg, kFloorEditText, g_wasFloor, TRUE);
		SetDlgItemInt(hDlg, kSuiteEditText, g_wasSuite, TRUE);
		editCtrl = GetDlgItem(hDlg, kFloorEditText);
		SendMessage(hDlg, WM_NEXTDLGCTL, (WPARAM)editCtrl, TRUE);
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			*roomToGoTo = kRoomIsEmpty;
			EndDialog(hDlg, IDCANCEL);
			break;

		case IDOK:
			*roomToGoTo = kRoomIsEmpty;
			if (IsDlgButtonChecked(hDlg, kGoToFirstRadio))
			{
				*roomToGoTo = GetFirstRoomNumber();
			}
			else if (IsDlgButtonChecked(hDlg, kGoToPrevRadio))
			{
				*roomToGoTo = g_previousRoom;
			}
			else if (IsDlgButtonChecked(hDlg, kGoToFSRadio))
			{
				g_wasFloor = (SInt16)GetDlgItemInt(hDlg, kFloorEditText, &validNumber, TRUE);
				if (!validNumber)
				{
					editCtrl = GetDlgItem(hDlg, kFloorEditText);
					SendMessage(hDlg, WM_NEXTDLGCTL, (WPARAM)editCtrl, TRUE);
					MessageBeep(MB_ICONWARNING);
					return TRUE;
				}
				g_wasSuite = (SInt16)GetDlgItemInt(hDlg, kSuiteEditText, &validNumber, TRUE);
				if (!validNumber)
				{
					editCtrl = GetDlgItem(hDlg, kSuiteEditText);
					SendMessage(hDlg, WM_NEXTDLGCTL, (WPARAM)editCtrl, TRUE);
					MessageBeep(MB_ICONWARNING);
					return TRUE;
				}
				*roomToGoTo = GetRoomNumber(&g_thisHouse, g_wasFloor, g_wasSuite);
			}
			EndDialog(hDlg, IDOK);
			break;

		case kGoToFirstRadio:
		case kGoToPrevRadio:
			if (HIWORD(wParam) == BN_DOUBLECLICKED)
			{
				SendMessage(hDlg, WM_COMMAND, MAKEWPARAM(IDOK, 0), 0);
			}
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  DoGoToDialog
// "Go To Room..." dialog.

void DoGoToDialog (HWND ownerWindow)
{
	SInt16 roomToGoTo;
	INT_PTR dlgResult;

	roomToGoTo = kRoomIsEmpty;
	dlgResult = DialogBoxParam(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(kGoToDialogID), ownerWindow,
			GoToFilter, (LPARAM)&roomToGoTo);

	if (dlgResult == IDOK)
	{
		if (RoomNumExists(&g_thisHouse, roomToGoTo))
		{
			DeselectObject();
			CopyRoomToThisRoom(roomToGoTo);
			ReflectCurrentRoom(false);
		}
		else
		{
			MessageBeep(MB_ICONWARNING);
		}
	}
}

//--------------------------------------------------------------  ConvertHouseVer1To2
// This function goes through an old version 1 house and converts it
// to version 2.

void ConvertHouseVer1To2 (void)
{
	wchar_t roomStr[32];
	wchar_t message[256];
	SInt16 wasRoom, floor, suite;
	SInt16 i, h, numRooms;
	HWND mssgWindow;

	CopyThisRoomToRoom();
	wasRoom = g_thisRoomNumber;
	GetLocalizedString(13, message, ARRAYSIZE(message));
	mssgWindow = OpenMessageWindow(message, g_mainWindow);

	numRooms = g_thisHouse.nRooms;
	for (i = 0; i < numRooms; i++)
	{
		if (g_thisHouse.rooms[i].suite != kRoomIsEmpty)
		{
			StringCchPrintf(roomStr, ARRAYSIZE(roomStr), L"%d", (int)i);
			GetLocalizedString(14, message, ARRAYSIZE(message));
			StringCchCat(message, ARRAYSIZE(message), roomStr);
			SetMessageWindowMessage(mssgWindow, message);

			ForceThisRoom(i);
			for (h = 0; h < kMaxRoomObs; h++)
			{
				switch (g_thisRoom->objects[h].what)
				{
					case kMailboxLf:
					case kMailboxRt:
					case kFloorTrans:
					case kCeilingTrans:
					case kInvisTrans:
					case kDeluxeTrans:
					if (g_thisRoom->objects[h].data.d.where != -1)
					{
						ExtractFloorSuiteVer1(g_thisRoom->objects[h].data.d.where, &floor, &suite);
						g_thisRoom->objects[h].data.d.where = MergeFloorSuiteVer2(floor, suite);
					}
					break;

					case kLightSwitch:
					case kMachineSwitch:
					case kThermostat:
					case kPowerSwitch:
					case kKnifeSwitch:
					case kInvisSwitch:
					case kTrigger:
					case kLgTrigger:
					if (g_thisRoom->objects[h].data.e.where != -1)
					{
						ExtractFloorSuiteVer1(g_thisRoom->objects[h].data.e.where, &floor, &suite);
						g_thisRoom->objects[h].data.e.where = MergeFloorSuiteVer2(floor, suite);
					}
					break;
				}
			}
			CopyThisRoomToRoom();
		}
	}

	g_thisHouse.version = kHouseVersion;

	CloseMessageWindow(mssgWindow);
	ForceThisRoom(wasRoom);
}

//--------------------------------------------------------------  ShiftWholeHouse

void ShiftWholeHouse (SInt16 howFar)
{
	(void)howFar;

	return;
#if 0
#pragma unused (howFar)
	SInt16 wasRoom;
	SInt16 i, h, numRooms;
	char wasState;

	OpenMessageWindow_Pascal("\pShifting Whole House…");

	CopyThisRoomToRoom();
	wasRoom = g_thisRoomNumber;
	wasState = HGetState((Handle)g_thisHouse);
	HLock((Handle)g_thisHouse);
	numRooms = (*g_thisHouse)->nRooms;

	for (i = 0; i < numRooms; i++)
	{
		if ((*g_thisHouse)->rooms[i].suite != kRoomIsEmpty)
		{
			ForceThisRoom(i);
			for (h = 0; h < kMaxRoomObs; h++)
			{
			}
			CopyThisRoomToRoom();
		}
	}

	HSetState((Handle)g_thisHouse, wasState);
	ForceThisRoom(wasRoom);

	CloseMessageWindow();
#endif
}
