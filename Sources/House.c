#include "House.h"

//============================================================================
//----------------------------------------------------------------------------
//                                  House.c
//----------------------------------------------------------------------------
//============================================================================


#include "AnimCursor.h"
#include "Banner.h"
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
#include "Room.h"
#include "RoomGraphics.h"
#include "SavedGames.h"
#include "StringUtils.h"
#include "Utilities.h"
#include "WindowUtils.h"


#define kGoToFirstRadio		1002
#define kGoToPrevRadio		1003
#define kGoToFSRadio		1004
#define kFloorEditText		1005
#define kSuiteEditText		1006


INT_PTR CALLBACK GoToFilter (HWND, UINT, WPARAM, LPARAM);


housePtr	thisHouse;
linksPtr	linksList;
Str32		thisHouseName;
SInt16		srcLocations[kMaxRoomObs];
SInt16		destLocations[kMaxRoomObs];
SInt16		wasFloor, wasSuite;
retroLink	retroLinkList[kMaxRoomObs];
Boolean		houseUnlocked;


//==============================================================  Functions
//--------------------------------------------------------------  CreateNewHouse
// Called to create a new house file.

#ifndef COMPILEDEMO
Boolean CreateNewHouse (HWND hwndOwner)
{
	MessageBox(hwndOwner, L"CreateNewHouse unimplemented", 0, MB_ICONERROR);
	return false;
#if 0
	AEKeyword			theKeyword;
	DescType			actualType;
	Size				actualSize;
	NavReplyRecord		theReply;
	NavDialogOptions	dialogOptions;
	FSSpec				tempSpec;
	FSSpec				theSpec;
	OSErr				theErr;

	theErr = NavGetDefaultDialogOptions(&dialogOptions);
	theErr = NavPutFile(nil, &theReply, &dialogOptions, nil, 'gliH', 'ozm5', nil);
	if (theErr == userCanceledErr)
		return false;
	if (!theReply.validRecord)
		return (false);

	theErr = AEGetNthPtr(&(theReply.selection), 1, typeFSS, &theKeyword,
			&actualType, &theSpec, sizeof(FSSpec), &actualSize);

	if (theReply.replacing)
	{
		theErr = FSMakeFSSpec(theSpec.vRefNum, theSpec.parID,
				theSpec.name, &tempSpec);
		if (!CheckFileError(theErr, theSpec.name))
			return (false);

		theErr = FSpDelete(&tempSpec);
		if (!CheckFileError(theErr, theSpec.name))
			return (false);
	}

	if (houseOpen)
	{
		if (!CloseHouse())
			return (false);
	}

	theErr = FSpCreate(&theSpec, 'ozm5', 'gliH', theReply.keyScript);
	if (!CheckFileError(theErr, "\pNew House"))
		return (false);
	HCreateResFile(theSpec.vRefNum, theSpec.parID, theSpec.name);
	if (ResError() != noErr)
		YellowAlert(kYellowFailedResCreate, ResError());

	PasStringCopy(theSpec.name, thisHouseName);
	AddExtraHouse(&theSpec);
	BuildHouseList();
	InitCursor();
	if (!OpenHouse())
		return (false);

	return (true);
#endif
}
#endif

//--------------------------------------------------------------  InitializeEmptyHouse

// Initializes all the structures for an empty (new) house.

#ifndef COMPILEDEMO
Boolean InitializeEmptyHouse (HWND ownerWindow)
{
	Str255			tempStr;

	if (thisHouse != NULL)
		free(thisHouse->rooms);
	free(thisHouse);

	thisHouse = malloc(sizeof(*thisHouse));

	if (thisHouse == NULL)
	{
		YellowAlert(ownerWindow, kYellowUnaccounted, 1);
		return (false);
	}

	thisHouse->version = kHouseVersion;
	thisHouse->firstRoom = -1;
	thisHouse->timeStamp = 0L;
	thisHouse->flags = 0L;
	thisHouse->initial.h = 32;
	thisHouse->initial.v = 32;
	ZeroHighScores(thisHouse);

	GetLocalizedString(11, tempStr);
	PasStringCopy(tempStr, thisHouse->banner);
	GetLocalizedString(12, tempStr);
	PasStringCopy(tempStr, thisHouse->trailer);
	thisHouse->hasGame = false;
	thisHouse->nRooms = 0;

	wardBitSet = false;
	phoneBitSet = false;

	numberRooms = 0;
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

	return (true);
}
#endif

//--------------------------------------------------------------  RealRoomNumberCount

// Returns the real number of rooms in a house (some rooms may still…
// be place-holders - they were deleted earlier and are flagged as…
// deleted but still occupy space in the file).

SInt16 RealRoomNumberCount (void)
{
	SInt16		realRoomCount, i;

	realRoomCount = thisHouse->nRooms;
	if (realRoomCount != 0)
	{
		for (i = 0; i < thisHouse->nRooms; i++)
		{
			if (thisHouse->rooms[i].suite == kRoomIsEmpty)
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

	if (thisHouse->nRooms <= 0)
	{
		firstRoom = -1;
		noRoomAtAll = true;
	}
	else
	{
		firstRoom = thisHouse->firstRoom;
		if ((firstRoom >= thisHouse->nRooms) || (firstRoom < 0))
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
		initialPt = thisHouse->initial;

	QSetRect(theRect, 0, 0, kGliderWide, kGliderHigh);
	QOffsetRect(theRect, initialPt.h, initialPt.v);
}

//--------------------------------------------------------------  HouseHasOriginalPicts

// Returns true is the current house has custom artwork imbedded.

static BOOL CALLBACK EnumResTypeProc(HMODULE hModule, LPWSTR lpszType, LONG_PTR lParam)
{
	if (lpszType == RT_BITMAP)
		*((Boolean *)lParam) = true;
	return TRUE;
}

Boolean HouseHasOriginalPicts (void)
{
	Boolean		hasPicts;

	hasPicts = false;
	if (houseResFork == NULL)
		return hasPicts;
	EnumResourceTypes(houseResFork, EnumResTypeProc, (LONG_PTR)&hasPicts);
	return hasPicts;
}

//--------------------------------------------------------------  CountHouseLinks

// Counts up the number of linked objects in a house.

SInt16 CountHouseLinks (void)
{
	SInt16		numRooms, numLinks;
	SInt16		r, i, what;

	numLinks = 0;
	numRooms = thisHouse->nRooms;

	for (r = 0; r < numRooms; r++)
	{
		for (i = 0; i < kMaxRoomObs; i++)
		{
			what = thisHouse->rooms[r].objects[i].what;
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
				if (thisHouse->rooms[r].objects[i].data.e.where != -1)
					numLinks++;
				break;

				case kMailboxLf:
				case kMailboxRt:
				case kFloorTrans:
				case kCeilingTrans:
				case kInvisTrans:
				case kDeluxeTrans:
				if (thisHouse->rooms[r].objects[i].data.d.where != -1)
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

	numRooms = thisHouse->nRooms;
	numLinks = 0;

	for (r = 0; r < numRooms; r++)
	{
		for (i = 0; i < kMaxRoomObs; i++)
		{
			what = thisHouse->rooms[r].objects[i].what;
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
				thisObject = thisHouse->rooms[r].objects[i];
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
				thisObject = thisHouse->rooms[r].objects[i];
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

	busy = true;
	probe = 0;

	do
	{
		if (thisHouse->rooms[which].objects[probe].what == kObjectIsEmpty)
		{
			looking = true;
			probe2 = probe + 1;			// begin by looking at the next object
			do
			{
				if (thisHouse->rooms[which].objects[probe2].what != kObjectIsEmpty)
				{
					thisHouse->rooms[which].objects[probe] =
							thisHouse->rooms[which].objects[probe2];
					thisHouse->rooms[which].objects[probe2].what = kObjectIsEmpty;
					if (srcLocations[probe2] != -1)
						linksList[srcLocations[probe2]].srcObj = probe;
					if (destLocations[probe2] != -1)
					{
						linksList[destLocations[probe2]].destObj = probe;
						room = linksList[destLocations[probe2]].srcRoom;
						obj = linksList[destLocations[probe2]].srcObj;
						thisHouse->rooms[room].objects[obj].data.e.who = (Byte)probe;
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
	linksList = malloc(sizeof(*linksList) * numLinks);
	if (linksList == NULL)
		RedAlert(kErrNoMemory);

	GenerateLinksList();

	numRooms = thisHouse->nRooms;

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

	numRooms = thisHouse->nRooms;
	count = 0;

	for (r = 0; r < numRooms; r++)
	{
		if (thisHouse->rooms[r].visited)
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

	numRooms = thisHouse->nRooms;

	for (r = 0; r < numRooms; r++)
	{
		for (i = 0; i < kMaxRoomObs; i++)
		{
			what = thisHouse->rooms[r].objects[i].what;
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
				thisObject = thisHouse->rooms[r].objects[i];
				if (thisObject.data.e.where != -1)
				{
					ExtractFloorSuite(thisObject.data.e.where, &floor, &suite);
					roomLinked = GetRoomNumber(floor, suite);
					if (roomLinked == thisRoomNumber)
					{
						objectLinked = (SInt16)thisObject.data.e.who;
						if (retroLinkList[objectLinked].room == -1)
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
				thisObject = thisHouse->rooms[r].objects[i];
				if (thisObject.data.d.where != -1)
				{
					ExtractFloorSuite(thisObject.data.d.where, &floor, &suite);
					roomLinked = GetRoomNumber(floor, suite);
					if (roomLinked == thisRoomNumber)
					{
						objectLinked = (SInt16)thisObject.data.d.who;
						if (retroLinkList[objectLinked].room == -1)
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
			*roomToGoTo = -1;
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
	Str255		roomStr, message;
	SInt16		wasRoom, floor, suite;
	SInt16		i, h, numRooms;

	CopyThisRoomToRoom();
	wasRoom = thisRoomNumber;
	GetLocalizedString(13, message);
	OpenMessageWindow(message);

	SpinCursor(3);

	numRooms = thisHouse->nRooms;
	for (i = 0; i < numRooms; i++)
	{
		if (thisHouse->rooms[i].suite != kRoomIsEmpty)
		{
			NumToString(i, roomStr);
			GetLocalizedString(14, message);
			PasStringConcat(message, roomStr);
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
						thisRoom->objects[h].data.d.where = MergeFloorSuite(floor, suite);
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
						thisRoom->objects[h].data.e.where = MergeFloorSuite(floor, suite);
					}
					break;
				}
			}
			CopyThisRoomToRoom();
		}
	}

	thisHouse->version = kHouseVersion;

	InitCursor();
	CloseMessageWindow();
	ForceThisRoom(wasRoom);
}

//--------------------------------------------------------------  ShiftWholeHouse

void ShiftWholeHouse (SInt16 howFar)
{
	return;
#if 0
#pragma unused (howFar)
	short		wasRoom;
	short		i, h, numRooms;
	char		wasState;

	OpenMessageWindow("\pShifting Whole House…");
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

