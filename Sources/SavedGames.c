#include "SavedGames.h"

//============================================================================
//----------------------------------------------------------------------------
//                                SavedGames.c
//----------------------------------------------------------------------------
//============================================================================

#include "Banner.h"
#include "ByteIO.h"
#include "DialogUtils.h"
#include "FileError.h"
#include "House.h"
#include "HouseIO.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "Play.h"
#include "Player.h"
#include "ResourceIDs.h"
#include "Room.h"
#include "SelectHouse.h"
#include "StructIO.h"
#include "StringUtils.h"
#include "Utilities.h"

#include <commdlg.h>
#include <strsafe.h>

#include <stdlib.h>

#define kSavedGameVersion		0x0200

BOOL GetSaveFolderPath (LPWSTR lpSavePath, DWORD cchSavePath);
void SavedGameMismatchError (HWND ownerWindow, ConstStringPtr gameName);

gameType g_smallGame;

//==============================================================  Functions
//--------------------------------------------------------------  GetSaveFolderPath

BOOL GetSaveFolderPath (LPWSTR lpSavePath, DWORD cchSavePath)
{
	WCHAR pathBuffer[MAX_PATH];
	HRESULT hr;

	if (!GetDataFolderPath(pathBuffer, ARRAYSIZE(pathBuffer)))
		return FALSE;
	hr = StringCchCat(pathBuffer, ARRAYSIZE(pathBuffer), L"\\Saves");
	if (FAILED(hr))
		return FALSE;
	if (!CreateDirectory(pathBuffer, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
		return FALSE;

	hr = StringCchCopy(lpSavePath, cchSavePath, pathBuffer);
	return SUCCEEDED(hr);
}

//--------------------------------------------------------------  SaveGame2

void SaveGame2 (HWND ownerWindow)
{
	OPENFILENAME		ofn = { 0 };
	Str255				gameNameStr;
	WCHAR				startPath[MAX_PATH];
	WCHAR				gamePath[MAX_PATH];
	roomType			*srcRoom;
	savedRoom			*destRoom;
	game2Type			savedGame;
	SInt16				r, i, numRooms;
	HANDLE				gameFileHandle;
	byteio				*byteWriter;
	HRESULT				writeResult;
	HRESULT				closeResult;

	if (!GetSaveFolderPath(startPath, ARRAYSIZE(startPath)))
		startPath[0] = L'\0';

	numRooms = g_thisHouse.nRooms;

	savedGame.savedData = (savedRoom *)calloc(numRooms, sizeof(savedRoom));
	if (savedGame.savedData == NULL)
	{
		YellowAlert(ownerWindow, kYellowFailedSaveGame, -1);
		return;
	}

	GetFirstWordOfString(g_thisHouseName, gameNameStr);
	PasStringConcatC(gameNameStr, " Game");

	WinFromMacString(gamePath, ARRAYSIZE(gamePath), gameNameStr);
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = ownerWindow;
	ofn.lpstrFilter = L"Glider PRO Saved Game (*.glg)\0*.glg\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = gamePath;
	ofn.nMaxFile = ARRAYSIZE(gamePath);
	if (startPath[0] != L'\0')
		ofn.lpstrInitialDir = startPath;
	ofn.lpstrTitle = L"Save Game As:";
	ofn.Flags = OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt = L"glg";
	if (!GetSaveFileName(&ofn))
	{
		free(savedGame.savedData);
		return;
	}

	savedGame.house.vRefNum = 0;
	savedGame.house.parID = 0;
	PasStringCopy(g_theHousesSpecs[g_thisHouseIndex].name, savedGame.house.name);
	savedGame.version = kSavedGameVersion;
	savedGame.wasStarsLeft = g_numStarsRemaining;
	savedGame.timeStamp = g_thisHouse.timeStamp;
	savedGame.where.h = g_theGlider.dest.left;
	savedGame.where.v = g_theGlider.dest.top;
	savedGame.score = g_theScore;
	savedGame.unusedLong = 0L;
	savedGame.unusedLong2 = 0L;
	savedGame.energy = g_batteryTotal;
	savedGame.bands = g_bandsTotal;
	savedGame.roomNumber = g_thisRoomNumber;
	savedGame.gliderState = g_theGlider.mode;
	savedGame.numGliders = g_mortals;
	savedGame.foil = g_foilTotal;
	savedGame.nRooms = numRooms;
	savedGame.facing = g_theGlider.facing;
	savedGame.showFoil = g_showFoil;

	for (r = 0; r < numRooms; r++)
	{
		destRoom = &(savedGame.savedData[r]);
		srcRoom = &(g_thisHouse.rooms[r]);

		destRoom->unusedShort = 0;
		destRoom->unusedByte = 0;
		destRoom->visited = srcRoom->visited;
		for (i = 0; i < kMaxRoomObs; i++)
			destRoom->objects[i] = srcRoom->objects[i];
	}

	gameFileHandle = CreateFile(ofn.lpstrFile, GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (gameFileHandle != INVALID_HANDLE_VALUE)
	{
		byteWriter = byteio_init_handle_writer(gameFileHandle);
		if (byteWriter != NULL)
		{
			writeResult = WriteGame2Type(byteWriter, &savedGame);
			closeResult = byteio_close(byteWriter);
			if (FAILED(writeResult))
			{
				CheckFileError(ownerWindow, writeResult, L"Saved Game");
			}
			if (FAILED(closeResult))
			{
				CheckFileError(ownerWindow, closeResult, L"Saved Game");
			}
		}
		else
		{
			CheckFileError(ownerWindow, E_OUTOFMEMORY, L"Saved Game");
		}
		CloseHandle(gameFileHandle);
	}
	else
	{
		CheckFileError(ownerWindow, HRESULT_FROM_WIN32(GetLastError()), L"Saved Game");
	}
	free(savedGame.savedData);
}

//--------------------------------------------------------------  SavedGameMismatchError

void SavedGameMismatchError (HWND ownerWindow, ConstStringPtr gameName)
{
	DialogParams params = { 0 };
	wchar_t gameStr[256];
	wchar_t houseStr[64];

	WinFromMacString(gameStr, ARRAYSIZE(gameStr), gameName);
	WinFromMacString(houseStr, ARRAYSIZE(houseStr), g_thisHouseName);

	params.arg[0] = gameStr;
	params.arg[1] = houseStr;
	Alert(kSavedGameErrorAlert, ownerWindow, &params);
}

//--------------------------------------------------------------  OpenSavedGame

Boolean OpenSavedGame (HWND ownerWindow)
{
	OPENFILENAME		ofn = { 0 };
	WCHAR				startPath[MAX_PATH];
	WCHAR				gamePath[MAX_PATH];
	roomType			*destRoom;
	savedRoom			*srcRoom;
	game2Type			savedGame;
	SInt16				r, i;
	HANDLE				gameFileHandle;
	byteio				*byteReader;
	HRESULT				readResult;

	if (!GetSaveFolderPath(startPath, ARRAYSIZE(startPath)))
		startPath[0] = L'\0';
	gamePath[0] = L'\0';

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = ownerWindow;
	ofn.lpstrFilter = L"Glider PRO Saved Game (*.glg)\0*.glg\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = gamePath;
	ofn.nMaxFile = ARRAYSIZE(gamePath);
	if (startPath[0] != L'\0')
		ofn.lpstrInitialDir = startPath;
	ofn.lpstrTitle = L"Open Saved Game:";
	ofn.Flags = OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt = L"glg";
	if (!GetOpenFileName(&ofn))
		return false;

	gameFileHandle = CreateFile(ofn.lpstrFile, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (gameFileHandle == INVALID_HANDLE_VALUE)
	{
		CheckFileError(ownerWindow, HRESULT_FROM_WIN32(GetLastError()), L"Saved Game");
		return false;
	}
	byteReader = byteio_init_handle_reader(gameFileHandle);
	if (byteReader == NULL)
		RedAlert(kErrNoMemory);
	savedGame.savedData = NULL;
	readResult = ReadGame2Type(byteReader, &savedGame);
	byteio_close(byteReader);
	CloseHandle(gameFileHandle);
	if (FAILED(readResult))
	{
		CheckFileError(ownerWindow, readResult, L"Saved Game");
		return false;
	}

	if (!PasStringEqual(savedGame.house.name, g_thisHouseName, true))
	{
		SavedGameMismatchError(ownerWindow, savedGame.house.name);
		free(savedGame.savedData);
		return false;
	}
	else if (g_thisHouse.timeStamp != savedGame.timeStamp)
	{
		YellowAlert(ownerWindow, kYellowSavedTimeWrong, 0);
		free(savedGame.savedData);
		return false;
	}
	else if (savedGame.version != kSavedGameVersion)
	{
		YellowAlert(ownerWindow, kYellowSavedVersWrong, kSavedGameVersion);
		free(savedGame.savedData);
		return false;
	}
	else if (savedGame.nRooms != g_thisHouse.nRooms)
	{
		YellowAlert(ownerWindow, kYellowSavedRoomsWrong,
				savedGame.nRooms - g_thisHouse.nRooms);
		free(savedGame.savedData);
		return false;
	}
	else
	{
		g_smallGame.wasStarsLeft = savedGame.wasStarsLeft;
		g_smallGame.where.h = savedGame.where.h;
		g_smallGame.where.v = savedGame.where.v;
		g_smallGame.score = savedGame.score;
		g_smallGame.unusedLong = savedGame.unusedLong;
		g_smallGame.unusedLong2 = savedGame.unusedLong2;
		g_smallGame.energy = savedGame.energy;
		g_smallGame.bands = savedGame.bands;
		g_smallGame.roomNumber = savedGame.roomNumber;
		g_smallGame.gliderState = savedGame.gliderState;
		g_smallGame.numGliders = savedGame.numGliders;
		g_smallGame.foil = savedGame.foil;
		g_smallGame.unusedShort = 0;
		g_smallGame.facing = savedGame.facing;
		g_smallGame.showFoil = savedGame.showFoil;

		if (savedGame.savedData == NULL)
		{
			savedGame.nRooms = 0;
		}
		for (r = 0; r < savedGame.nRooms; r++)
		{
			srcRoom = &(savedGame.savedData[r]);
			destRoom = &(g_thisHouse.rooms[r]);
			destRoom->visited = srcRoom->visited;
			for (i = 0; i < kMaxRoomObs; i++)
				destRoom->objects[i] = srcRoom->objects[i];
		}
	}

	free(savedGame.savedData);

	return true;
}

//--------------------------------------------------------------  SaveGame
// This is probably about 3 days away from becoming the "old" function
// for saving games.

void SaveGame (HWND ownerWindow, Boolean doSave)
{
	if (g_twoPlayerGame)
		return;

	if (doSave)
	{
		g_thisHouse.savedGame.version = kSavedGameVersion;
		g_thisHouse.savedGame.wasStarsLeft = g_numStarsRemaining;
		g_thisHouse.savedGame.timeStamp = (SInt32)Mac_GetDateTime();
		g_thisHouse.savedGame.where.h = g_theGlider.dest.left;
		g_thisHouse.savedGame.where.v = g_theGlider.dest.top;
		g_thisHouse.savedGame.score = g_theScore;
		g_thisHouse.savedGame.unusedLong = 0L;
		g_thisHouse.savedGame.unusedLong2 = 0L;
		g_thisHouse.savedGame.energy = g_batteryTotal;
		g_thisHouse.savedGame.bands = g_bandsTotal;
		g_thisHouse.savedGame.roomNumber = g_thisRoomNumber;
		g_thisHouse.savedGame.gliderState = g_theGlider.mode;
		g_thisHouse.savedGame.numGliders = g_mortals;
		g_thisHouse.savedGame.foil = g_foilTotal;
		g_thisHouse.savedGame.unusedShort = 0;
		g_thisHouse.savedGame.facing = g_theGlider.facing;
		g_thisHouse.savedGame.showFoil = g_showFoil;

		g_thisHouse.hasGame = true;
	}
	else
	{
		g_thisHouse.hasGame = false;
	}

	if (doSave)
	{
		if (!WriteHouse(ownerWindow, g_theMode == kEditMode))
			YellowAlert(ownerWindow, kYellowFailedWrite, 0);
	}
}
