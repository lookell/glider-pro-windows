#include "HouseIO.h"

//============================================================================
//----------------------------------------------------------------------------
//									HouseIO.c
//----------------------------------------------------------------------------
//============================================================================


#include "ByteIO.h"
#include "DialogUtils.h"
#include "Environ.h"
#include "Externs.h"
#include "House.h"
#include "Macintosh.h"
#include "ObjectEdit.h"
#include "ResourceIDs.h"
#include "StructIO.h"


#define kSaveChanges			IDYES
#define kDiscardChanges			IDNO


void LoopMovie (void);
void OpenHouseMovie (void);
void CloseHouseMovie (void);
Boolean IsFileReadOnly (houseSpec *);


Movie		theMovie;
Rect		movieRect;
HANDLE		houseRefNum;
HMODULE		houseResFork;
SInt16		wasHouseVersion;
Boolean		houseOpen, fileDirty, gameDirty;
Boolean		changeLockStateOfHouse, saveHouseLocked, houseIsReadOnly;
Boolean		hasMovie, tvInRoom;

extern	houseSpecPtr	theHousesSpecs;
extern	SInt16			thisHouseIndex, tvWithMovieNumber;
extern	SInt16			numberRooms, housesFound;
extern	Boolean			noRoomAtAll, quitting, wardBitSet;
extern	Boolean			phoneBitSet, bannerStarCountOn;


//==============================================================  Functions
//--------------------------------------------------------------  LoopMovie

void LoopMovie (void)
{
	return;
#if 0
	Handle		theLoop;
	UserData	theUserData;
	short		theCount;

	theLoop = NewHandle(sizeof(long));
	(** (long **) theLoop) = 0;
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
	FSSpec		theSpec;
	FInfo		finderInfo;
	Handle		spaceSaver;
	OSErr		theErr;
	short		movieRefNum;
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
	Str32		demoHouseName;

	PasStringCopyC("Demo House", demoHouseName);
	if (houseOpen)
	{
		if (!CloseHouse(ownerWindow))
			return(false);
	}
	if ((housesFound < 1) || (thisHouseIndex == -1))
		return(false);

	#ifdef COMPILEDEMO
	if (!Mac_EqualString(theHousesSpecs[thisHouseIndex].name, demoHouseName, false))
		return (false);
	#endif

	houseIsReadOnly = false;
	houseRefNum = CreateFile(theHousesSpecs[thisHouseIndex].path,
			GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (houseRefNum == INVALID_HANDLE_VALUE)
	{
		houseIsReadOnly = true;
		houseRefNum = CreateFile(theHousesSpecs[thisHouseIndex].path,
				GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	if (houseRefNum == INVALID_HANDLE_VALUE)
	{
		houseIsReadOnly = false;
		CheckFileError(ownerWindow, GetLastError(), thisHouseName);
		return (false);
	}

	houseOpen = true;
	OpenHouseResFork(ownerWindow);

	hasMovie = false;
	tvInRoom = false;
	tvWithMovieNumber = -1;
	OpenHouseMovie();

	return (true);
}

//--------------------------------------------------------------  OpenSpecificHouse
// Opens the specific house passed in.

#ifndef COMPILEDEMO
Boolean OpenSpecificHouse (FSSpec *specs)
{
	return false;
#if 0
	short		i;
	Boolean		itOpened;

	if ((housesFound < 1) || (thisHouseIndex == -1))
		return (false);

	itOpened = true;

	for (i = 0; i < housesFound; i++)
	{
		if ((theHousesSpecs[i].vRefNum == specs->vRefNum) &&
				(theHousesSpecs[i].parID == specs->parID) &&
				(EqualString(theHousesSpecs[i].name, specs->name, false, true)))
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

	GetLocalizedString(15, tempStr);
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
// With a house open, this function reads in the actual bits of data…
// into memory.

Boolean ReadHouse (HWND ownerWindow)
{
	LARGE_INTEGER	byteCount, distance;
	SInt16			whichRoom;
	byteio			byteReader;

	if (!houseOpen)
	{
		YellowAlert(ownerWindow, kYellowUnaccounted, 2);
		return (false);
	}

	if (gameDirty || fileDirty)
	{
		if (houseIsReadOnly)
		{
			if (!WriteScoresToDisk(ownerWindow))
			{
				YellowAlert(ownerWindow, kYellowFailedWrite, 0);
				return(false);
			}
		}
		else if (!WriteHouse(ownerWindow, false))
			return(false);
	}

	if (!GetFileSizeEx(houseRefNum, &byteCount))
	{
		CheckFileError(ownerWindow, GetLastError(), thisHouseName);
		return(false);
	}

	#ifdef COMPILEDEMO
	if (byteCount.QuadPart != 16526LL)
		return (false);
	#endif

	if (thisHouse != NULL)
	{
		free(thisHouse->rooms);
		free(thisHouse);
	}

	thisHouse = malloc(sizeof(*thisHouse));
	if (thisHouse == NULL)
	{
		YellowAlert(ownerWindow, kYellowNoMemory, 10);
		return(false);
	}

	distance.QuadPart = 0;
	if (!SetFilePointerEx(houseRefNum, distance, NULL, FILE_BEGIN))
	{
		CheckFileError(ownerWindow, GetLastError(), thisHouseName);
		return(false);
	}

	if (!byteio_init_handle_reader(&byteReader, houseRefNum))
		RedAlert(kErrNoMemory);
	if (!ReadHouseType(&byteReader, thisHouse))
	{
		numberRooms = 0;
		noRoomAtAll = true;
		YellowAlert(ownerWindow, kYellowNoRooms, 0);
		CheckFileError(ownerWindow, GetLastError(), thisHouseName);
		byteio_close(&byteReader);
		return(false);
	}
	byteio_close(&byteReader);

	numberRooms = thisHouse->nRooms;
	#ifdef COMPILEDEMO
	if (numberRooms != 45)
		return (false);
	#endif
	if ((numberRooms < 1) || (byteCount.QuadPart == 0LL))
	{
		numberRooms = 0;
		noRoomAtAll = true;
		YellowAlert(ownerWindow, kYellowNoRooms, 0);
		return(false);
	}

	wasHouseVersion = thisHouse->version;
	if (wasHouseVersion >= kNewHouseVersion)
	{
		YellowAlert(ownerWindow, kYellowNewerVersion, 0);
		return(false);
	}

	houseUnlocked = ((thisHouse->timeStamp & 0x00000001) == 0);
	#ifdef COMPILEDEMO
	if (houseUnlocked)
		return (false);
	#endif
	changeLockStateOfHouse = false;
	saveHouseLocked = false;

	whichRoom = thisHouse->firstRoom;
	#ifdef COMPILEDEMO
	if (whichRoom != 0)
		return (false);
	#endif

	wardBitSet = ((thisHouse->flags & 0x00000001) == 0x00000001);
	phoneBitSet = ((thisHouse->flags & 0x00000002) == 0x00000002);
	bannerStarCountOn = ((thisHouse->flags & 0x00000004) == 0x00000000);

	noRoomAtAll = (RealRoomNumberCount() == 0);
	thisRoomNumber = -1;
	previousRoom = -1;
	if (!noRoomAtAll)
		CopyRoomToThisRoom(whichRoom);

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

	return (true);
}

//--------------------------------------------------------------  WriteHouse
// This function writes out the house data to disk.

Boolean WriteHouse (HWND ownerWindow, Boolean checkIt)
{
	UInt32			timeStamp;
	byteio			byteWriter;
	LARGE_INTEGER	distance;

	if (!houseOpen)
	{
		YellowAlert(ownerWindow, kYellowUnaccounted, 4);
		return (false);
	}

	distance.QuadPart = 0;
	if (!SetFilePointerEx(houseRefNum, distance, NULL, FILE_BEGIN))
	{
		CheckFileError(ownerWindow, GetLastError(), thisHouseName);
		return(false);
	}

	CopyThisRoomToRoom();

	if (checkIt)
		CheckHouseForProblems();

	if (fileDirty)
	{
		Mac_GetDateTime(&timeStamp);
		timeStamp &= 0x7FFFFFFF;

		if (changeLockStateOfHouse)
			houseUnlocked = !saveHouseLocked;

		if (houseUnlocked)								// house unlocked
			timeStamp &= 0x7FFFFFFE;
		else
			timeStamp |= 0x00000001;
		thisHouse->timeStamp = (SInt32)timeStamp;
		thisHouse->version = wasHouseVersion;
	}

	if (!byteio_init_handle_writer(&byteWriter, houseRefNum))
		RedAlert(kErrNoMemory);
	if (!WriteHouseType(&byteWriter, thisHouse))
	{
		CheckFileError(ownerWindow, GetLastError(), thisHouseName);
		byteio_close(&byteWriter);
		return(false);
	}
	byteio_close(&byteWriter);

	if (!SetEndOfFile(houseRefNum))
	{
		CheckFileError(ownerWindow, GetLastError(), thisHouseName);
		return(false);
	}

	if (changeLockStateOfHouse)
	{
		changeLockStateOfHouse = false;
		ReflectCurrentRoom(true);
	}

	gameDirty = false;
	fileDirty = false;
	UpdateMenus(false);
	return (true);
}

//--------------------------------------------------------------  CloseHouse
// This function closes the current house that is open.

Boolean CloseHouse (HWND ownerWindow)
{
	if (!houseOpen)
		return (true);

	if (gameDirty)
	{
		if (houseIsReadOnly)
		{
			if (!WriteScoresToDisk(ownerWindow))
				YellowAlert(ownerWindow, kYellowFailedWrite, 0);
		}
		else if (!WriteHouse(ownerWindow, theMode == kEditMode))
			YellowAlert(ownerWindow, kYellowFailedWrite, 0);
	}
	else if (fileDirty)
	{
#ifndef COMPILEDEMO
		if (!QuerySaveChanges(ownerWindow)) // false signifies user canceled
			return(false);
#endif
	}

	CloseHouseResFork();
	CloseHouseMovie();
	CloseHandle(houseRefNum);

	houseOpen = false;

	gameDirty = false;
	fileDirty = false;
	return (true);
}

//--------------------------------------------------------------  OpenHouseResFork
// Opens the resource fork of the current house that is open.

void OpenHouseResFork (HWND ownerWindow)
{
	WCHAR		fileName[MAX_PATH];
	PWCH		extPtr;
	HRESULT		hr;

	if (houseResFork == NULL)
	{
		hr = StringCchCopy(fileName, ARRAYSIZE(fileName),
				theHousesSpecs[thisHouseIndex].path);
		if (FAILED(hr))
		{
			YellowAlert(ownerWindow, kYellowFailedResOpen, -1);
			return;
		}
		extPtr = wcsrchr(fileName, L'.');
		if (extPtr == NULL)
		{
			YellowAlert(ownerWindow, kYellowFailedResOpen, -1);
			return;
		}
		*extPtr = L'\0';
		hr = StringCchCat(fileName, ARRAYSIZE(fileName), L".glr");
		if (FAILED(hr))
		{
			YellowAlert(ownerWindow, kYellowFailedResOpen, -1);
			return;
		}
		houseResFork = LoadLibraryEx(fileName, NULL, LOAD_LIBRARY_AS_DATAFILE);
		if (houseResFork == NULL)
			YellowAlert(ownerWindow, kYellowFailedResOpen, (SInt16)GetLastError());
	}
}

//--------------------------------------------------------------  CloseHouseResFork
// Closes the resource fork of the current house that is open.

void CloseHouseResFork (void)
{
	if (houseResFork != NULL)
	{
		FreeLibrary(houseResFork);
		houseResFork = NULL;
	}
}

//--------------------------------------------------------------  QuerySaveChanges
// If changes were made, this function will present the user with a…
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
// This is a dialog used to present an error code and explanation…
// to the user when a non-lethal error has occurred.  Ideally, of…
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

//--------------------------------------------------------------  IsFileReadOnly

Boolean IsFileReadOnly (houseSpec *theSpec)
{
	return false;
#if 0
#pragma unused (theSpec)

	return false;
	/*
	Str255			tempStr;
	ParamBlockRec	theBlock;
	HParamBlockRec	hBlock;
	VolumeParam		*volPtr;
	OSErr			theErr;

	volPtr = (VolumeParam *)&theBlock;
	volPtr->ioCompletion = nil;
	volPtr->ioVolIndex = 0;
	volPtr->ioNamePtr = tempStr;
	volPtr->ioVRefNum = theSpec->vRefNum;

	theErr = PBGetVInfo(&theBlock, false);
	if (CheckFileError(theErr, "\pRead/Write"))
	{
		if (((volPtr->ioVAtrb & 0x0080) == 0x0080) ||
				((volPtr->ioVAtrb & 0x8000) == 0x8000))
			return (true);		// soft/hard locked bits
		else
		{
			hBlock.fileParam.ioCompletion = nil;
			hBlock.fileParam.ioVRefNum = theSpec->vRefNum;
			hBlock.fileParam.ioFVersNum = 0;
			hBlock.fileParam.ioFDirIndex = 0;
			hBlock.fileParam.ioNamePtr = theSpec->name;
			hBlock.fileParam.ioDirID = theSpec->parID;

			theErr = PBHGetFInfo(&hBlock, false);
			if (CheckFileError(theErr, "\pRead/Write"))
			{
				if ((hBlock.fileParam.ioFlAttrib & 0x0001) == 0x0001)
					return (true);
				else
					return (false);
			}
			else
				return (false);
		}
	}
	else
		return (false);
	*/
#endif
}

