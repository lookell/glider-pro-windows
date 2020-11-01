#define GP_USE_WINAPI_H

#include "HouseIO.h"

//============================================================================
//----------------------------------------------------------------------------
//                                 HouseIO.c
//----------------------------------------------------------------------------
//============================================================================


#include "Banner.h"
#include "DialogUtils.h"
#include "FileError.h"
#include "HighScores.h"
#include "House.h"
#include "HouseLegal.h"
#include "Macintosh.h"
#include "Main.h"
#include "MainWindow.h"
#include "Menu.h"
#include "ObjectEdit.h"
#include "Objects.h"
#include "Play.h"
#include "ResourceIDs.h"
#include "ResourceLoader.h"
#include "Room.h"
#include "RoomGraphics.h"
#include "SelectHouse.h"
#include "StringUtils.h"

#include <strsafe.h>

#include <stdlib.h>


#define kSaveChanges			IDYES
#define kDiscardChanges			IDNO


void LoopMovie (void);
void OpenHouseMovie (void);
void CloseHouseMovie (void);


Movie theMovie;
Rect movieRect;
SInt16 wasHouseVersion;
Boolean houseOpen;
Boolean fileDirty;
Boolean gameDirty;
Boolean changeLockStateOfHouse;
Boolean saveHouseLocked;
Boolean houseIsReadOnly;
Boolean hasMovie;
Boolean tvInRoom;


//==============================================================  Functions
//--------------------------------------------------------------  LoopMovie

void LoopMovie (void)
{
	return;
#if 0
	Handle		theLoop;
	UserData	theUserData;
	SInt16		theCount;

	theLoop = NewHandle(sizeof(SInt32));
	(** (SInt32 **) theLoop) = 0;
	theUserData = GetMovieUserData(theMovie);
	theCount = CountUserDataType(theUserData, 'LOOP');
	while (theCount--)
	{
		RemoveUserData(theUserData, 'LOOP', 1);
	}
	AddUserData(theUserData, theLoop, 'LOOP');
#endif
}

//--------------------------------------------------------------  OpenHouseMovie

void OpenHouseMovie (void)
{
#ifdef COMPILEQT
	TimeBase	theTime;
	houseSpec	theSpec;
	FInfo		finderInfo;
	Handle		spaceSaver;
	OSErr		theErr;
	SInt16		movieRefNum;
	Boolean		dataRefWasChanged;

	if (thisMac.hasQT)
	{
		theSpec = theHousesSpecs[thisHouseIndex];
		PasStringConcat(theSpec.name, "\p.mov");

		theErr = FSpGetFInfo(&theSpec, &finderInfo);
		if (theErr != noErr)
			return;

		theErr = OpenMovieFile(&theSpec, &movieRefNum, fsCurPerm);
		if (theErr != noErr)
		{
			YellowAlert(kYellowQTMovieNotLoaded, theErr);
			return;
		}

		theErr = NewMovieFromFile(&theMovie, movieRefNum, nil, theSpec.name,
				newMovieActive, &dataRefWasChanged);
		if (theErr != noErr)
		{
			YellowAlert(kYellowQTMovieNotLoaded, theErr);
			theErr = CloseMovieFile(movieRefNum);
			return;
		}
		theErr = CloseMovieFile(movieRefNum);

		spaceSaver = NewHandle(307200L);
		if (spaceSaver == nil)
		{
			YellowAlert(kYellowQTMovieNotLoaded, 749);
			CloseHouseMovie();
			return;
		}

		GoToBeginningOfMovie(theMovie);
		theErr = LoadMovieIntoRam(theMovie,
				GetMovieTime(theMovie, 0L), GetMovieDuration(theMovie), 0);
		if (theErr != noErr)
		{
			YellowAlert(kYellowQTMovieNotLoaded, theErr);
			DisposeHandle(spaceSaver);
			CloseHouseMovie();
			return;
		}
		DisposeHandle(spaceSaver);

		theErr = PrerollMovie(theMovie, 0, 0x000F0000);
		if (theErr != noErr)
		{
			YellowAlert(kYellowQTMovieNotLoaded, theErr);
			CloseHouseMovie();
			return;
		}

		theTime = GetMovieTimeBase(theMovie);
		SetTimeBaseFlags(theTime, loopTimeBase);
		SetMovieMasterTimeBase(theMovie, theTime, nil);
		LoopMovie();

		GetMovieBox(theMovie, &movieRect);

		theHousesSpecs[thisHouseIndex].hasMovie = true;
		hasMovie = true;
	}
#endif
}

//--------------------------------------------------------------  CloseHouseMovie

void CloseHouseMovie (void)
{
#ifdef COMPILEQT
	OSErr		theErr;

	if ((thisMac.hasQT) && (hasMovie))
	{
		theErr = LoadMovieIntoRam(theMovie,
				GetMovieTime(theMovie, 0L), GetMovieDuration(theMovie), flushFromRam);
		DisposeMovie(theMovie);
	}
#endif
	hasMovie = false;
}

//--------------------------------------------------------------  OpenHouse
// Opens a house (whatever current selection is).  Returns true if all went well.

Boolean OpenHouse (HWND ownerWindow)
{
	HRESULT hr;

	if (houseOpen)
	{
		if (!CloseHouse(ownerWindow))
		{
			return false;
		}
	}
	if ((housesFound < 1) || (thisHouseIndex == -1))
	{
		return false;
	}

	#ifdef COMPILEDEMO
	if (lstrcmpi(theHousesSpecs[thisHouseIndex].houseName, L"Demo House") != 0)
	{
		return false;
	}
	#endif

	hr = Gp_LoadHouseFile(theHousesSpecs[thisHouseIndex].path);
	if (FAILED(hr))
	{
		houseIsReadOnly = false;
		CheckFileError(ownerWindow, ERROR_OPEN_FAILED,
			theHousesSpecs[thisHouseIndex].houseName);
		return false;
	}
	houseIsReadOnly = Gp_HouseFileReadOnly();
	theHousesSpecs[thisHouseIndex].readOnly = houseIsReadOnly;

	houseOpen = true;

	hasMovie = false;
	theHousesSpecs[thisHouseIndex].hasMovie = false;
	tvInRoom = false;
	tvWithMovieNumber = -1;
	OpenHouseMovie();

	return true;
}

//--------------------------------------------------------------  OpenSpecificHouse
// Opens the specific house passed in.

#ifndef COMPILEDEMO
Boolean OpenSpecificHouse (FSSpec *specs)
{
	(void)specs;

	return false;
#if 0
	SInt16		i;
	Boolean		itOpened;

	if ((housesFound < 1) || (thisHouseIndex == -1))
		return (false);

	itOpened = true;

	for (i = 0; i < housesFound; i++)
	{
		if ((theHousesSpecs[i].vRefNum == specs->vRefNum) &&
				(theHousesSpecs[i].parID == specs->parID) &&
				(PasStringEqual(theHousesSpecs[i].name, specs->name, false)))
		{
			thisHouseIndex = i;
			PasStringCopy(theHousesSpecs[thisHouseIndex].name, thisHouseName);
			if (OpenHouse())
				itOpened = ReadHouse();
			else
				itOpened = false;
			break;
		}
	}

	return (itOpened);
#endif
}
#endif

//--------------------------------------------------------------  SaveHouseAs

#ifndef COMPILEDEMO
Boolean SaveHouseAs (void)
{
	// TEMP - fix this later -- use NavServices (see House.c)
/*
	StandardFileReply	theReply;
	FSSpec				oldHouse;
	OSErr				theErr;
	Boolean				noProblems;
	Str255				tempStr;

	noProblems = true;

	GetLocalizedString_Pascal(15, tempStr);
	StandardPutFile(tempStr, thisHouseName, &theReply);
	if (theReply.sfGood)
	{
		oldHouse = theHousesSpecs[thisHouseIndex];

		CloseHouseResFork();						// close this house file
		theErr = FSClose(houseRefNum);
		if (theErr != noErr)
		{
			CheckFileError(theErr, "\pPreferences");
			return(false);
		}
													// create new house file
		theErr = FSpCreate(&theReply.sfFile, 'ozm5', 'gliH', theReply.sfScript);
		if (!CheckFileError(theErr, theReply.sfFile.name))
			return (false);
		HCreateResFile(theReply.sfFile.vRefNum, theReply.sfFile.parID,
				theReply.sfFile.name);
		if (ResError() != noErr)
			YellowAlert(kYellowFailedResCreate, ResError());
		PasStringCopy(theReply.sfFile.name, thisHouseName);
													// open new house data fork
		theErr = FSpOpenDF(&theReply.sfFile, fsRdWrPerm, &houseRefNum);
		if (!CheckFileError(theErr, thisHouseName))
			return (false);

		houseOpen = true;

		noProblems = WriteHouse(false);				// write out house data
		if (!noProblems)
			return(false);

		BuildHouseList();
		if (OpenSpecificHouse(&theReply.sfFile))	// open new house again
		{
		}
		else
		{
			if (OpenSpecificHouse(&oldHouse))
			{
				YellowAlert(kYellowOpenedOldHouse, 0);
			}
			else
			{
				YellowAlert(kYellowLostAllHouses, 0);
				noProblems = false;
			}
		}
	}


	return (noProblems);
	*/
	return false;
}
#endif

//--------------------------------------------------------------  ReadHouse
// With a house open, this function reads in the actual bits of data
// into memory.

Boolean ReadHouse (HWND ownerWindow)
{
	uint64_t byteCount;
	SInt16 whichRoom;
	HRESULT hr;

	if (!houseOpen)
	{
		YellowAlert(ownerWindow, kYellowUnaccounted, 2);
		return false;
	}

	if (gameDirty || fileDirty)
	{
		if (houseIsReadOnly)
		{
			if (!WriteScoresToDisk(ownerWindow))
			{
				YellowAlert(ownerWindow, kYellowFailedWrite, 0);
				return false;
			}
		}
		else if (!WriteHouse(ownerWindow, false))
		{
			return false;
		}
	}

	byteCount = Gp_HouseFileDataSize();

	#ifdef COMPILEDEMO
	if (byteCount != 16526)
	{
		return false;
	}
	#endif

	free(thisHouse.rooms);
	ZeroMemory(&thisHouse, sizeof(thisHouse));

	hr = Gp_ReadHouseData(&thisHouse);
	if (FAILED(hr))
	{
		ZeroMemory(&thisHouse, sizeof(thisHouse));
		thisHouse.nRooms = 0;
		noRoomAtAll = true;
		YellowAlert(ownerWindow, kYellowNoRooms, 0);
		return false;
	}

	#ifdef COMPILEDEMO
	if (thisHouse.nRooms != 45)
	{
		return false;
	}
	#endif
	if ((thisHouse.nRooms < 1) || (byteCount == 0))
	{
		thisHouse.nRooms = 0;
		noRoomAtAll = true;
		YellowAlert(ownerWindow, kYellowNoRooms, 0);
		return false;
	}

	wasHouseVersion = thisHouse.version;
	if (wasHouseVersion >= kNewHouseVersion)
	{
		YellowAlert(ownerWindow, kYellowNewerVersion, 0);
		return false;
	}

	houseUnlocked = ((thisHouse.timeStamp & 0x00000001) == 0);
	#ifdef COMPILEDEMO
	if (houseUnlocked)
	{
		return false;
	}
	#endif
	changeLockStateOfHouse = false;
	saveHouseLocked = false;

	whichRoom = thisHouse.firstRoom;
	#ifdef COMPILEDEMO
	if (whichRoom != 0)
	{
		return false;
	}
	#endif

	wardBitSet = ((thisHouse.flags & 0x00000001) == 0x00000001);
	phoneBitSet = ((thisHouse.flags & 0x00000002) == 0x00000002);
	bannerStarCountOn = ((thisHouse.flags & 0x00000004) == 0x00000000);

	noRoomAtAll = (RealRoomNumberCount() == 0);
	thisRoomNumber = -1;
	previousRoom = -1;
	if (!noRoomAtAll)
	{
		CopyRoomToThisRoom(whichRoom);
	}

	if (houseIsReadOnly)
	{
		houseUnlocked = false;
		if (ReadScoresFromDisk(ownerWindow))
		{
		}
	}

	objActive = kNoObjectSelected;
	ReflectCurrentRoom(true);
	gameDirty = false;
	fileDirty = false;
	UpdateMenus(false);

	return true;
}

//--------------------------------------------------------------  WriteHouse
// This function writes out the house data to disk.

Boolean WriteHouse (HWND ownerWindow, Boolean checkIt)
{
	UInt32 timeStamp;
	HRESULT hr;

	if (!houseOpen)
	{
		YellowAlert(ownerWindow, kYellowUnaccounted, 4);
		return false;
	}

	CopyThisRoomToRoom();

	if (checkIt)
	{
		CheckHouseForProblems();
	}

	if (fileDirty)
	{
		timeStamp = Mac_GetDateTime();
		timeStamp &= 0x7FFFFFFF;

		if (changeLockStateOfHouse)
		{
			houseUnlocked = !saveHouseLocked;
		}

		if (houseUnlocked)
		{
			timeStamp &= 0x7FFFFFFE;
		}
		else
		{
			timeStamp |= 0x00000001;
		}
		thisHouse.timeStamp = (SInt32)timeStamp;
		thisHouse.version = wasHouseVersion;
	}

	hr = Gp_WriteHouseData(&thisHouse);
	if (FAILED(hr))
	{
		if (HRESULT_FACILITY(hr) == FACILITY_WIN32)
		{
			CheckFileError(ownerWindow, (DWORD)HRESULT_CODE(hr),
				theHousesSpecs[thisHouseIndex].houseName);
		}
		return false;
	}

	if (changeLockStateOfHouse)
	{
		changeLockStateOfHouse = false;
		ReflectCurrentRoom(true);
	}

	gameDirty = false;
	fileDirty = false;
	UpdateMenus(false);
	return true;
}

//--------------------------------------------------------------  CloseHouse
// This function closes the current house that is open.

Boolean CloseHouse (HWND ownerWindow)
{
	if (!houseOpen)
	{
		return (true);
	}

	if (gameDirty)
	{
		if (houseIsReadOnly)
		{
			if (!WriteScoresToDisk(ownerWindow))
			{
				YellowAlert(ownerWindow, kYellowFailedWrite, 0);
			}
		}
		else if (!WriteHouse(ownerWindow, theMode == kEditMode))
		{
			YellowAlert(ownerWindow, kYellowFailedWrite, 0);
		}
	}
	else if (fileDirty)
	{
#ifndef COMPILEDEMO
		if (!QuerySaveChanges(ownerWindow)) // false signifies user canceled
		{
			return(false);
		}
#endif
	}

	CloseHouseMovie();
	Gp_UnloadHouseFile();

	houseOpen = false;

	gameDirty = false;
	fileDirty = false;
	return (true);
}

//--------------------------------------------------------------  QuerySaveChanges
// If changes were made, this function will present the user with a
// dialog asking them if they would like to save the changes.

#ifndef COMPILEDEMO
Boolean QuerySaveChanges (HWND ownerWindow)
{
	DialogParams	params = { 0 };
	wchar_t			houseStr[64];
	SInt16			hitWhat;
	Boolean			whoCares;

	if (!fileDirty)
		return(true);

	WinFromMacString(houseStr, ARRAYSIZE(houseStr), thisHouseName);
	params.arg[0] = houseStr;
	hitWhat = Alert(kSaveChangesAlert, ownerWindow, &params);
	if (hitWhat == kSaveChanges)
	{
		if (wasHouseVersion < kHouseVersion)
			ConvertHouseVer1To2();
		wasHouseVersion = kHouseVersion;
		if (WriteHouse(ownerWindow, true))
			return (true);
		else
			return (false);
	}
	else if (hitWhat == kDiscardChanges)
	{
		fileDirty = false;
		if (!quitting)
		{
			whoCares = CloseHouse(ownerWindow);
			if (OpenHouse(ownerWindow))
				whoCares = ReadHouse(ownerWindow);
		}
		UpdateMenus(false);
		return (true);
	}
	else
		return (false);
}
#endif

//--------------------------------------------------------------  YellowAlert
// This is a dialog used to present an error code and explanation
// to the user when a non-lethal error has occurred.  Ideally, of
// course, this never is called.

void YellowAlert (HWND ownerWindow, SInt16 whichAlert, SInt16 identifier)
{
	DialogParams	params = { 0 };
	wchar_t			errStr[256];
	wchar_t			errNumStr[16];
	INT				result;
	SInt16			whoCares;

	result = LoadString(HINST_THISCOMPONENT, kYellowAlertStringBase + whichAlert,
			errStr, ARRAYSIZE(errStr));
	if (result <= 0)
		errStr[0] = L'\0';
	StringCchPrintf(errNumStr, ARRAYSIZE(errNumStr), L"%d", (int)identifier);

	params.arg[0] = errStr;
	params.arg[1] = errNumStr;
	whoCares = Alert(kYellowAlert, ownerWindow, &params);
}

