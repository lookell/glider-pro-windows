#define GP_USE_WINAPI_H

#include "House.h"

//============================================================================
//----------------------------------------------------------------------------
//                                  House.c
//----------------------------------------------------------------------------
//============================================================================


#include "AnimCursor.h"
#include "Banner.h"
#include "ByteIO.h"
#include "DialogUtils.h"
#include "FileError.h"
#include "HighScores.h"
#include "HouseIO.h"
#include "Link.h"
#include "MacTypes.h"
#include "Map.h"
#include "Menu.h"
#include "ObjectEdit.h"
#include "Play.h"
#include "RectUtils.h"
#include "ResourceIDs.h"
#include "ResourceLoader.h"
#include "Room.h"
#include "RoomGraphics.h"
#include "SavedGames.h"
#include "SelectHouse.h"
#include "StructIO.h"
#include "StringUtils.h"
#include "Utilities.h"
#include "WindowUtils.h"

#include <commdlg.h>
#include <strsafe.h>

#include <stdlib.h>


#define kGoToFirstRadio		1002
#define kGoToPrevRadio		1003
#define kGoToFSRadio		1004
#define kFloorEditText		1005
#define kSuiteEditText		1006


void SortRoomsObjects (SInt16 which);
INT_PTR CALLBACK GoToFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


houseType thisHouse;
linksPtr linksList;
SInt16 wasFloor, wasSuite;
retroLink retroLinkList[kMaxRoomObs];
Boolean houseUnlocked;
Str32 thisHouseName;

static SInt16 srcLocations[kMaxRoomObs];
static SInt16 destLocations[kMaxRoomObs];


//==============================================================  Functions
//--------------------------------------------------------------  CreateNewHouse
// Called to create a new house file.

#ifndef COMPILEDEMO
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
	theSpec.hIcon = NULL;

	// Create the empty house file

	if (FAILED(Gp_CreateHouseFile(theSpec.path)))
	{
		YellowAlert(hwndOwner, kYellowFailedWrite, 0);
		return false;
	}

	// Add the new house to the house list and open it for business

	if (houseOpen)
	{
		if (!CloseHouse(hwndOwner))
		{
			return false;
		}
	}

	PasStringCopy(theSpec.name, thisHouseName);
	AddExtraHouse(&theSpec);
	BuildHouseList(hwndOwner);
	InitCursor();
	if (!OpenHouse(hwndOwner))
	{
		return false;
	}

	return true;
}
#endif

//--------------------------------------------------------------  InitializeEmptyHouse
// Initializes all the structures for an empty (new) house.

#ifndef COMPILEDEMO
void InitializeEmptyHouse (void)
{
	wchar_t tempStr[256];

	free(thisHouse.rooms);

	ZeroMemory(&thisHouse, sizeof(thisHouse));
	thisHouse.version = kHouseVersion;
	thisHouse.timeStamp = 0L;
	thisHouse.flags = 0L;
	thisHouse.initial.h = 32;
	thisHouse.initial.v = 32;
	GetLocalizedString(11, tempStr, ARRAYSIZE(tempStr));
	MacFromWinString(thisHouse.banner, ARRAYSIZE(thisHouse.banner), tempStr);
	GetLocalizedString(12, tempStr, ARRAYSIZE(tempStr));
	MacFromWinString(thisHouse.trailer, ARRAYSIZE(thisHouse.trailer), tempStr);
	ZeroHighScores(&thisHouse);
	thisHouse.hasGame = false;
	thisHouse.firstRoom = -1;
	thisHouse.nRooms = 0;
	thisHouse.rooms = NULL;

	wardBitSet = false;
	phoneBitSet = false;

	mapLeftRoom = 60;
	mapTopRoom = 50;
	thisRoomNumber = kRoomIsEmpty;
	previousRoom = -1;
	houseUnlocked = true;
	OpenMapWindow();
	UpdateMapWindow();
	noRoomAtAll = true;
	fileDirty = true;
	UpdateMenus(false);
	ReflectCurrentRoom(true);
}
#endif

//--------------------------------------------------------------  RealRoomNumberCount
// Returns the real number of rooms in a house (some rooms may still…
// be place-holders - they were deleted earlier and are flagged as…
// deleted but still occupy space in the file).

SInt16 RealRoomNumberCount (void)
{
	SInt16		realRoomCount, i;

	realRoomCount = thisHouse.nRooms;
	if (realRoomCount != 0)
	{
		for (i = 0; i < thisHouse.nRooms; i++)
		{
			if (thisHouse.rooms[i].suite == kRoomIsEmpty)
				realRoomCount--;
		}
	}

	return (realRoomCount);
}

//--------------------------------------------------------------  GetFirstRoomNumber
// Returns the room number (indicee into house file) of the room where…
// the player is to begin.

SInt16 GetFirstRoomNumber (void)
{
	SInt16		firstRoom;

	if (thisHouse.nRooms <= 0)
	{
		firstRoom = -1;
		noRoomAtAll = true;
	}
	else
	{
		firstRoom = thisHouse.firstRoom;
		if ((firstRoom >= thisHouse.nRooms) || (firstRoom < 0))
			firstRoom = 0;
	}

	return (firstRoom);
}

//--------------------------------------------------------------  WhereDoesGliderBegin
// Returns a rectangle indicating where in the first room the player's…
// glider is to appear.

void WhereDoesGliderBegin (Rect *theRect, SInt16 mode)
{
	Point initialPt;

	initialPt.h = 0;
	initialPt.v = 0;
	if (mode == kResumeGameMode)
		initialPt = smallGame.where;
	else if (mode == kNewGameMode)
		initialPt = thisHouse.initial;

	QSetRect(theRect, 0, 0, kGliderWide, kGliderHigh);
	QOffsetRect(theRect, initialPt.h, initialPt.v);
}

//--------------------------------------------------------------  HouseHasOriginalPicts
// Returns true is the current house has custom artwork imbedded.

static BOOLEAN EnumHousePicts (SInt16 resID, void *userData)
{
	(void)resID;

	// If this function is called, then there is at least one custom image.
	*((Boolean *)userData) = true;
	return FALSE;
}

Boolean HouseHasOriginalPicts (void)
{
	Boolean hasPicts;

	hasPicts = false;
	Gp_EnumHouseImages(EnumHousePicts, &hasPicts);
	return hasPicts;
}

//--------------------------------------------------------------  CountHouseLinks
// Counts up the number of linked objects in a house.

SInt16 CountHouseLinks (void)
{
	SInt16		numRooms, numLinks;
	SInt16		r, i, what;

	numLinks = 0;
	numRooms = thisHouse.nRooms;

	for (r = 0; r < numRooms; r++)
	{
		for (i = 0; i < kMaxRoomObs; i++)
		{
			what = thisHouse.rooms[r].objects[i].what;
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
				if (thisHouse.rooms[r].objects[i].data.e.where != -1)
					numLinks++;
				break;

				case kMailboxLf:
				case kMailboxRt:
				case kFloorTrans:
				case kCeilingTrans:
				case kInvisTrans:
				case kDeluxeTrans:
				if (thisHouse.rooms[r].objects[i].data.d.where != -1)
					numLinks++;
				break;
			}
		}
	}

	return (numLinks);
}

//--------------------------------------------------------------  GenerateLinksList
// Generates a list of all objects that have links and what rooms…
// and objects they are linked to.  It is called in order to preserve…
// the links if the objects or rooms in a house are to be shuffled…
// around.

#ifndef COMPILEDEMO
void GenerateLinksList (void)
{
	objectType	thisObject;
	SInt16		numLinks, numRooms, r, i, what;
	SInt16		floor, suite, roomLinked, objectLinked;

	numRooms = thisHouse.nRooms;
	numLinks = 0;

	for (r = 0; r < numRooms; r++)
	{
		for (i = 0; i < kMaxRoomObs; i++)
		{
			what = thisHouse.rooms[r].objects[i].what;
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
				thisObject = thisHouse.rooms[r].objects[i];
				if (thisObject.data.e.where != -1)
				{
					ExtractFloorSuite(thisObject.data.e.where, &floor, &suite);
					roomLinked = GetRoomNumber(floor, suite);
					objectLinked = (SInt16)thisObject.data.e.who;
					linksList[numLinks].srcRoom = r;
					linksList[numLinks].srcObj = i;
					linksList[numLinks].destRoom = roomLinked;
					linksList[numLinks].destObj = objectLinked;
					numLinks++;
				}
				break;

				case kMailboxLf:
				case kMailboxRt:
				case kFloorTrans:
				case kCeilingTrans:
				case kInvisTrans:
				case kDeluxeTrans:
				thisObject = thisHouse.rooms[r].objects[i];
				if (thisObject.data.d.where != -1)
				{
					ExtractFloorSuite(thisObject.data.d.where, &floor, &suite);
					roomLinked = GetRoomNumber(floor, suite);
					objectLinked = (SInt16)thisObject.data.d.who;
					linksList[numLinks].srcRoom = r;
					linksList[numLinks].srcObj = i;
					linksList[numLinks].destRoom = roomLinked;
					linksList[numLinks].destObj = objectLinked;
					numLinks++;
				}
				break;
			}
		}
	}
}
#endif

//--------------------------------------------------------------  SortRoomObjects
// I'm a little fuzzy on what this does.

#ifndef COMPILEDEMO
void SortRoomsObjects (SInt16 which)
{
	SInt16		probe, probe2, room, obj;
	Boolean		busy, looking;

	if (which < 0 || which >= thisHouse.nRooms)
		return;

	busy = true;
	probe = 0;

	do
	{
		if (thisHouse.rooms[which].objects[probe].what == kObjectIsEmpty)
		{
			looking = true;
			probe2 = probe + 1;			// begin by looking at the next object
			do
			{
				if (thisHouse.rooms[which].objects[probe2].what != kObjectIsEmpty)
				{
					thisHouse.rooms[which].objects[probe] =
							thisHouse.rooms[which].objects[probe2];
					thisHouse.rooms[which].objects[probe2].what = kObjectIsEmpty;
					if (srcLocations[probe2] != -1)
						linksList[srcLocations[probe2]].srcObj = probe;
					if (destLocations[probe2] != -1)
					{
						linksList[destLocations[probe2]].destObj = probe;
						room = linksList[destLocations[probe2]].srcRoom;
						obj = linksList[destLocations[probe2]].srcObj;
						thisHouse.rooms[room].objects[obj].data.e.who = (Byte)probe;
					}
					fileDirty = true;
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
#endif

//--------------------------------------------------------------  SortHouseObjects
// I'm a little fuzzy on what this does exactly either.

#ifndef COMPILEDEMO
void SortHouseObjects (void)
{
	SInt16		numLinks, numRooms, r, i, l;

	SpinCursor(3);

	CopyThisRoomToRoom();

	numLinks = CountHouseLinks();
	if (numLinks == 0)
		return;

	linksList = NULL;
	linksList = (linksPtr)malloc(sizeof(*linksList) * numLinks);
	if (linksList == NULL)
		RedAlert(kErrNoMemory);

	GenerateLinksList();

	numRooms = thisHouse.nRooms;

	for (r = 0; r < numRooms; r++)
	{
		for (i = 0; i < kMaxRoomObs; i++)	// initialize arrays
		{
			srcLocations[i] = -1;
			destLocations[i] = -1;
		}

		for (i = 0; i < kMaxRoomObs; i++)	// walk object list
		{
			for (l = 0; l < numLinks; l++)	// walk link list
			{
				if ((linksList[l].srcRoom == r) && (linksList[l].srcObj == i))
					srcLocations[i] = l;
				if ((linksList[l].destRoom == r) && (linksList[l].destObj == i))
					destLocations[i] = l;
			}
		}
		SortRoomsObjects(r);

		if ((r & 0x0007) == 0x0007)
			IncrementCursor();
	}

	SpinCursor(3);
	free(linksList);
	ForceThisRoom(thisRoomNumber);
}
#endif

//--------------------------------------------------------------  CountRoomsVisited
// Goes through and counts the number of rooms a player has been to in…
// the current game.

SInt16 CountRoomsVisited (void)
{
	SInt16		numRooms, r, count;

	numRooms = thisHouse.nRooms;
	count = 0;

	for (r = 0; r < numRooms; r++)
	{
		if (thisHouse.rooms[r].visited)
			count++;
	}

	return (count);
}

//--------------------------------------------------------------  GenerateRetroLinks
// Walk entire house looking for objects which are linked to objects…
// in the current room.

void GenerateRetroLinks (void)
{
	objectType	thisObject;
	SInt16		i, r, numRooms, floor, suite;
	SInt16		what, roomLinked, objectLinked;

	for (i = 0; i < kMaxRoomObs; i++)		// Initialize array.
		retroLinkList[i].room = -1;

	numRooms = thisHouse.nRooms;

	for (r = 0; r < numRooms; r++)
	{
		for (i = 0; i < kMaxRoomObs; i++)
		{
			what = thisHouse.rooms[r].objects[i].what;
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
				thisObject = thisHouse.rooms[r].objects[i];
				if (thisObject.data.e.where != -1)
				{
					ExtractFloorSuite(thisObject.data.e.where, &floor, &suite);
					roomLinked = GetRoomNumber(floor, suite);
					if (roomLinked == thisRoomNumber)
					{
						objectLinked = (SInt16)thisObject.data.e.who;
						if ((objectLinked >= 0 && objectLinked < kMaxRoomObs) &&
							(retroLinkList[objectLinked].room == -1))
						{
							retroLinkList[objectLinked].room = r;
							retroLinkList[objectLinked].object = i;
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
				thisObject = thisHouse.rooms[r].objects[i];
				if (thisObject.data.d.where != -1)
				{
					ExtractFloorSuite(thisObject.data.d.where, &floor, &suite);
					roomLinked = GetRoomNumber(floor, suite);
					if (roomLinked == thisRoomNumber)
					{
						objectLinked = (SInt16)thisObject.data.d.who;
						if ((objectLinked >= 0 && objectLinked < kMaxRoomObs) &&
							(retroLinkList[objectLinked].room == -1))
						{
							retroLinkList[objectLinked].room = r;
							retroLinkList[objectLinked].object = i;
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
		CenterOverOwner(hDlg);
		SetWindowLongPtr(hDlg, DWLP_USER, lParam);

		if (GetFirstRoomNumber() == thisRoomNumber)
			EnableWindow(GetDlgItem(hDlg, kGoToFirstRadio), FALSE);
		if ((!RoomNumExists(previousRoom)) || (previousRoom == thisRoomNumber))
			EnableWindow(GetDlgItem(hDlg, kGoToPrevRadio), FALSE);

		CheckRadioButton(hDlg, kGoToFirstRadio, kGoToFSRadio, kGoToFSRadio);
		SetDlgItemInt(hDlg, kFloorEditText, wasFloor, TRUE);
		SetDlgItemInt(hDlg, kSuiteEditText, wasSuite, TRUE);
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
				*roomToGoTo = previousRoom;
			}
			else if (IsDlgButtonChecked(hDlg, kGoToFSRadio))
			{
				wasFloor = (SInt16)GetDlgItemInt(hDlg, kFloorEditText, &validNumber, TRUE);
				if (!validNumber)
				{
					editCtrl = GetDlgItem(hDlg, kFloorEditText);
					SendMessage(hDlg, WM_NEXTDLGCTL, (WPARAM)editCtrl, TRUE);
					MessageBeep(MB_ICONWARNING);
					return TRUE;
				}
				wasSuite = (SInt16)GetDlgItemInt(hDlg, kSuiteEditText, &validNumber, TRUE);
				if (!validNumber)
				{
					editCtrl = GetDlgItem(hDlg, kSuiteEditText);
					SendMessage(hDlg, WM_NEXTDLGCTL, (WPARAM)editCtrl, TRUE);
					MessageBeep(MB_ICONWARNING);
					return TRUE;
				}
				*roomToGoTo = GetRoomNumber(wasFloor, wasSuite);
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
		if (RoomNumExists(roomToGoTo))
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
// This function goes through an old version 1 house and converts it…
// to version 2.

void ConvertHouseVer1To2 (void)
{
	wchar_t roomStr[32];
	wchar_t message[256];
	SInt16 wasRoom, floor, suite;
	SInt16 i, h, numRooms;

	CopyThisRoomToRoom();
	wasRoom = thisRoomNumber;
	GetLocalizedString(13, message, ARRAYSIZE(message));
	OpenMessageWindow(message);

	SpinCursor(3);

	numRooms = thisHouse.nRooms;
	for (i = 0; i < numRooms; i++)
	{
		if (thisHouse.rooms[i].suite != kRoomIsEmpty)
		{
			StringCchPrintf(roomStr, ARRAYSIZE(roomStr), L"%d", (int)i);
			GetLocalizedString(14, message, ARRAYSIZE(message));
			StringCchCat(message, ARRAYSIZE(message), roomStr);
			SetMessageWindowMessage(message);
			SpinCursor(1);

			ForceThisRoom(i);
			for (h = 0; h < kMaxRoomObs; h++)
			{
				switch (thisRoom->objects[h].what)
				{
					case kMailboxLf:
					case kMailboxRt:
					case kFloorTrans:
					case kCeilingTrans:
					case kInvisTrans:
					case kDeluxeTrans:
					if (thisRoom->objects[h].data.d.where != -1)
					{
						ExtractFloorSuite(thisRoom->objects[h].data.d.where, &floor, &suite);
						floor += kNumUndergroundFloors;
						thisRoom->objects[h].data.d.where = MergeFloorSuiteVer2(floor, suite);
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
					if (thisRoom->objects[h].data.e.where != -1)
					{
						ExtractFloorSuite(thisRoom->objects[h].data.e.where, &floor, &suite);
						floor += kNumUndergroundFloors;
						thisRoom->objects[h].data.e.where = MergeFloorSuiteVer2(floor, suite);
					}
					break;
				}
			}
			CopyThisRoomToRoom();
		}
	}

	thisHouse.version = kHouseVersion;

	InitCursor();
	CloseMessageWindow();
	ForceThisRoom(wasRoom);
}

//--------------------------------------------------------------  ShiftWholeHouse

void ShiftWholeHouse (SInt16 howFar)
{
	(void)howFar;

	return;
#if 0
#pragma unused (howFar)
	SInt16		wasRoom;
	SInt16		i, h, numRooms;
	char		wasState;

	OpenMessageWindow_Pascal("\pShifting Whole House…");
	SpinCursor(3);

	CopyThisRoomToRoom();
	wasRoom = thisRoomNumber;
	wasState = HGetState((Handle)thisHouse);
	HLock((Handle)thisHouse);
	numRooms = (*thisHouse)->nRooms;

	for (i = 0; i < numRooms; i++)
	{
		if ((*thisHouse)->rooms[i].suite != kRoomIsEmpty)
		{
			SpinCursor(1);

			ForceThisRoom(i);
			for (h = 0; h < kMaxRoomObs; h++)
			{
			}
			CopyThisRoomToRoom();
		}
	}

	HSetState((Handle)thisHouse, wasState);
	ForceThisRoom(wasRoom);

	InitCursor();
	CloseMessageWindow();
#endif
}

