//============================================================================
//----------------------------------------------------------------------------
//                                HighScores.c
//----------------------------------------------------------------------------
//============================================================================

#include "HighScores.h"

#include "ByteIO.h"
#include "DialogUtils.h"
#include "Environ.h"
#include "FileError.h"
#include "House.h"
#include "HouseIO.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "Menu.h"
#include "Player.h"
#include "RectUtils.h"
#include "ResourceIDs.h"
#include "Sound.h"
#include "StringUtils.h"
#include "StructIO.h"
#include "Transitions.h"
#include "Utilities.h"

#include <shlwapi.h>
#include <stdlib.h>
#include <strsafe.h>

SInt16 GetWidthOfString (HDC hdc, PCWSTR theString);
void DrawHighScores (void);
void SortHighScores (scoresType *theScores);
INT_PTR CALLBACK NameFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void GetHighScoreName (HWND ownerWindow, SInt16 place);
INT_PTR CALLBACK BannerFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void GetHighScoreBanner (HWND ownerWindow);
Boolean FindHighScoresFolder (LPWSTR scoresDirPath, DWORD cchDirPath);
Boolean GetHighScoresFilePath (PWSTR pszPath, DWORD cchPath, PCWSTR pszHousePath);

Str31 g_highBanner;
Str15 g_highName;
SInt16 g_lastHighScore;

//==============================================================  Functions
//--------------------------------------------------------------  DoHighScores
// Handles fading in and cleaning up the high scores screen.

void DoHighScores (void)
{
	Rect tempRect;

	Mac_PaintRect(g_workSrcMap, &g_workSrcRect, GetStockBrush(BLACK_BRUSH));
	QSetRect(&tempRect, 0, 0, 640, 460);
	QOffsetRect(&tempRect, g_splashOriginH, g_splashOriginV);
	LoadScaledGraphic(g_workSrcMap, g_theHouseFile, kStarPictID, &tempRect);
	DissolveScreenOn(&g_workSrcRect);
	DrawHighScores();
	DissolveScreenOn(&g_workSrcRect);
	Sleep(1000);
	WaitForInputEvent(30);
}

//--------------------------------------------------------------  GetWidthOfString

SInt16 GetWidthOfString (HDC hdc, PCWSTR theString)
{
	SIZE extents = { 0 };

	if (!GetTextExtentPoint32W(hdc, theString, (int)wcslen(theString), &extents))
	{
		return 0;
	}
	return (SInt16)extents.cx;
}

//--------------------------------------------------------------  DrawHighScores
// Draws the actual scores on the screen.

void DrawHighScores (void)
{
	#define kScoreSpacing  18
	#define kScoreWide  352
	#define kKimsLifted  4
	HDC tempMap, tempMask;
	Rect tempRect, tempRect2;
	WCHAR tempStr[MAX_PATH + 4];
	SInt16 scoreLeft, bannerWidth, i, dropIt;
	INT hOffset, vOffset;
	HFONT theFont;
	COLORREF numberColor;
	COLORREF entryColor;
	COLORREF wasColor;

	scoreLeft = (RectWide(&g_workSrcRect) - kScoreWide) / 2;
	dropIt = 129 + g_splashOriginV;

	QSetRect(&tempRect, 0, 0, 332, 30);
	tempMap = CreateOffScreenGWorld(&tempRect, kPreferredDepth);
	LoadGraphic(tempMap, g_theHouseFile, kHighScoresPictID);

	tempMask = CreateOffScreenGWorld(&tempRect, 1);
	LoadGraphic(tempMask, g_theHouseFile, kHighScoresMaskID);

	tempRect2 = tempRect;
	QOffsetRect(&tempRect2, scoreLeft + (kScoreWide - 332) / 2, dropIt - 60);

	Mac_CopyMask(tempMap, tempMask, g_workSrcMap,
			&tempRect, &tempRect, &tempRect2);

	DisposeGWorld(tempMap);
	DisposeGWorld(tempMask);

	theFont = CreateTahomaFont(-14, FW_BOLD);

	SaveDC(g_workSrcMap);
	SelectFont(g_workSrcMap, theFont);
	SetBkMode(g_workSrcMap, TRANSPARENT);
	SetTextAlign(g_workSrcMap, TA_LEFT | TA_BASELINE);
	StringCchCopy(tempStr, ARRAYSIZE(tempStr), L"\u2022 "); // "• "
	StringCchCat(tempStr, ARRAYSIZE(tempStr), g_thisHouseName);
	StringCchCat(tempStr, ARRAYSIZE(tempStr), L" \u2022"); // " •"
	hOffset = scoreLeft + ((kScoreWide - GetWidthOfString(g_workSrcMap, tempStr)) / 2);
	vOffset = dropIt - 65;
	SetTextColor(g_workSrcMap, blackColor);
	TextOutW(g_workSrcMap, hOffset - 1, vOffset - 1, tempStr, (int)wcslen(tempStr));
	SetTextColor(g_workSrcMap, cyanColor);
	TextOutW(g_workSrcMap, hOffset, vOffset, tempStr, (int)wcslen(tempStr));
	RestoreDC(g_workSrcMap, -1);
	DeleteFont(theFont);

	theFont = CreateTahomaFont(-12, FW_BOLD);
	SaveDC(g_workSrcMap);
	SelectFont(g_workSrcMap, theFont);
	SetBkMode(g_workSrcMap, TRANSPARENT);
	SetTextAlign(g_workSrcMap, TA_LEFT | TA_BASELINE);

	// message for score #1
	WinFromMacString(tempStr, ARRAYSIZE(tempStr), g_thisHouse.highScores.banner);
	bannerWidth = GetWidthOfString(g_workSrcMap, tempStr);
	hOffset = scoreLeft + (kScoreWide - bannerWidth) / 2;
	vOffset = dropIt - kKimsLifted;
	SetTextColor(g_workSrcMap, blackColor);
	TextOutW(g_workSrcMap, hOffset, vOffset, tempStr, (int)wcslen(tempStr));
	SetTextColor(g_workSrcMap, yellowColor);
	TextOutW(g_workSrcMap, hOffset, vOffset - 1, tempStr, (int)wcslen(tempStr));

	QSetRect(&tempRect, 0, 0, bannerWidth + 8, kScoreSpacing);
	QOffsetRect(&tempRect, scoreLeft - 3 + (kScoreWide - bannerWidth) / 2,
			dropIt + 5 - kScoreSpacing - kKimsLifted);
	wasColor = SetDCBrushColor(g_workSrcMap, blackColor);
	Mac_FrameRect(g_workSrcMap, &tempRect, GetStockBrush(DC_BRUSH), 1, 1);
	QOffsetRect(&tempRect, -1, -1);
	SetDCBrushColor(g_workSrcMap, yellowColor);
	Mac_FrameRect(g_workSrcMap, &tempRect, GetStockBrush(DC_BRUSH), 1, 1);
	SetDCBrushColor(g_workSrcMap, wasColor);

	for (i = 0; i < kMaxScores; i++)
	{
		if (g_thisHouse.highScores.scores[i] > 0L)
		{
			if (i == 0)
			{
				vOffset = dropIt - 1 - kScoreSpacing - kKimsLifted;
			}
			else
			{
				vOffset = dropIt - 1 + (i * kScoreSpacing);
			}

			if (i == g_lastHighScore)
			{
				numberColor = whiteColor;
				entryColor = whiteColor;
			}
			else
			{
				numberColor = cyanColor;
				entryColor = yellowColor;
			}

			// draw placing number
			NumToString(i + 1L, tempStr, ARRAYSIZE(tempStr));
			SetTextColor(g_workSrcMap, blackColor);
			TextOutW(g_workSrcMap, scoreLeft + 1, vOffset + 1, tempStr, (int)wcslen(tempStr));
			SetTextColor(g_workSrcMap, numberColor);
			TextOutW(g_workSrcMap, scoreLeft + 0, vOffset, tempStr, (int)wcslen(tempStr));

			// draw high score name
			WinFromMacString(tempStr, ARRAYSIZE(tempStr), g_thisHouse.highScores.names[i]);
			SetTextColor(g_workSrcMap, blackColor);
			TextOutW(g_workSrcMap, scoreLeft + 31, vOffset + 1, tempStr, (int)wcslen(tempStr));
			SetTextColor(g_workSrcMap, entryColor);
			TextOutW(g_workSrcMap, scoreLeft + 30, vOffset, tempStr, (int)wcslen(tempStr));

			// draw level number
			NumToString(g_thisHouse.highScores.levels[i], tempStr, ARRAYSIZE(tempStr));
			SetTextColor(g_workSrcMap, blackColor);
			TextOutW(g_workSrcMap, scoreLeft + 161, vOffset + 1, tempStr, (int)wcslen(tempStr));
			SetTextColor(g_workSrcMap, entryColor);
			TextOutW(g_workSrcMap, scoreLeft + 160, vOffset, tempStr, (int)wcslen(tempStr));

			// draw word "rooms"
			if (g_thisHouse.highScores.levels[i] == 1)
				GetLocalizedString(6, tempStr, ARRAYSIZE(tempStr));
			else
				GetLocalizedString(7, tempStr, ARRAYSIZE(tempStr));
			SetTextColor(g_workSrcMap, blackColor);
			TextOutW(g_workSrcMap, scoreLeft + 193, vOffset + 1, tempStr, (int)wcslen(tempStr));
			SetTextColor(g_workSrcMap, cyanColor);
			TextOutW(g_workSrcMap, scoreLeft + 192, vOffset, tempStr, (int)wcslen(tempStr));

			// draw high score points
			NumToString(g_thisHouse.highScores.scores[i], tempStr, ARRAYSIZE(tempStr));
			SetTextColor(g_workSrcMap, blackColor);
			TextOutW(g_workSrcMap, scoreLeft + 291, vOffset + 1, tempStr, (int)wcslen(tempStr));
			SetTextColor(g_workSrcMap, entryColor);
			TextOutW(g_workSrcMap, scoreLeft + 290, vOffset, tempStr, (int)wcslen(tempStr));
		}
	}

	RestoreDC(g_workSrcMap, -1);
	DeleteFont(theFont);

	theFont = CreateTahomaFont(-9, FW_BOLD);
	SaveDC(g_workSrcMap);
	SelectFont(g_workSrcMap, theFont);
	SetBkMode(g_workSrcMap, TRANSPARENT);
	SetTextAlign(g_workSrcMap, TA_LEFT | TA_BASELINE);
	SetTextColor(g_workSrcMap, blueColor);
	GetLocalizedString(8, tempStr, ARRAYSIZE(tempStr));
	vOffset = dropIt - 1 + (10 * kScoreSpacing);
	TextOutW(g_workSrcMap, scoreLeft + 80, vOffset, tempStr, (int)wcslen(tempStr));
	RestoreDC(g_workSrcMap, -1);
	DeleteFont(theFont);
}

//--------------------------------------------------------------  SortHighScores
// This does a simple sort of the high scores.

void SortHighScores (scoresType *theScores)
{
	scoresType tempScores = { 0 };
	SInt32 greatest;
	SInt16 i, h, which;

	for (h = 0; h < kMaxScores; h++)
	{
		greatest = -1L;
		which = -1;
		for (i = 0; i < kMaxScores; i++)
		{
			if (theScores->scores[i] > greatest)
			{
				greatest = theScores->scores[i];
				which = i;
			}
		}
		if (which != -1)
		{
			PasStringCopy(theScores->names[which], tempScores.names[h],
					ARRAYSIZE(tempScores.names[h]));
			tempScores.scores[h] = theScores->scores[which];
			tempScores.timeStamps[h] = theScores->timeStamps[which];
			tempScores.levels[h] = theScores->levels[which];
			theScores->scores[which] = -1L;
		}
	}
	PasStringCopy(theScores->banner, tempScores.banner, ARRAYSIZE(tempScores.banner));
	*theScores = tempScores;
}

//--------------------------------------------------------------  ZeroHighScores
// This funciton goes through and resets or "zeros" all high scores.

void ZeroHighScores (housePtr house)
{
	SInt16 i;

	MacFromWinString(
		house->highScores.banner,
		ARRAYSIZE(house->highScores.banner),
		g_thisHouseName
	);
	for (i = 0; i < kMaxScores; i++)
	{
		PasStringCopyC("--------------", house->highScores.names[i],
				ARRAYSIZE(house->highScores.names[i]));
		house->highScores.scores[i] = 0L;
		house->highScores.timeStamps[i] = 0L;
		house->highScores.levels[i] = 0;
	}
}

//--------------------------------------------------------------  ZeroAllButHighestScore
// Like the above, but this function preserves the highest score.

void ZeroAllButHighestScore (housePtr house)
{
	SInt16 i;

	for (i = 1; i < kMaxScores; i++)
	{
		PasStringCopyC("--------------", house->highScores.names[i],
				ARRAYSIZE(house->highScores.names[i]));
		house->highScores.scores[i] = 0L;
		house->highScores.timeStamps[i] = 0L;
		house->highScores.levels[i] = 0;
	}
}

//--------------------------------------------------------------  TestHighScore
// This function is called after a game ends in order to test the
// current high score against the high score list.  It returns true
// if the player is on the high score list now.

Boolean TestHighScore (HWND ownerWindow)
{
	SInt16 placing, i;

	if (g_resumedSavedGame)
		return (false);

	g_lastHighScore = -1;
	placing = -1;

	for (i = 0; i < kMaxScores; i++)
	{
		if (g_theScore > g_thisHouse.highScores.scores[i])
		{
			placing = i;
			g_lastHighScore = i;
			break;
		}
	}

	if (placing != -1)
	{
		GetHighScoreName(ownerWindow, placing + 1);
		PasStringCopy(g_highName, g_thisHouse.highScores.names[kMaxScores - 1],
				ARRAYSIZE(g_thisHouse.highScores.names[kMaxScores - 1]));
		if (placing == 0)
		{
			GetHighScoreBanner(ownerWindow);
			PasStringCopy(g_highBanner, g_thisHouse.highScores.banner,
					ARRAYSIZE(g_thisHouse.highScores.banner));
		}
		g_thisHouse.highScores.scores[kMaxScores - 1] = g_theScore;
		g_thisHouse.highScores.timeStamps[kMaxScores - 1] = Mac_GetDateTime();
		g_thisHouse.highScores.levels[kMaxScores - 1] = CountRoomsVisited(&g_thisHouse);
		SortHighScores(&g_thisHouse.highScores);
		g_gameDirty = true;
	}

	return (placing != -1);
}

//--------------------------------------------------------------  NameFilter
// Dialog filter for the "Enter High Score Name" dialog.

INT_PTR CALLBACK NameFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		CenterDialogOverOwner(hDlg);
		ParamDialogText(hDlg, (const DialogParams *)lParam);
		SetDialogString(hDlg, kHighNameItem, g_highName);
		SendDlgItemMessage(hDlg, kHighNameItem,
				EM_LIMITTEXT, ARRAYSIZE(g_highName) - 1, 0);
		PlayPrioritySound(kEnergizeSound, kEnergizePriority);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			GetDialogString(hDlg, kHighNameItem, g_highName, ARRAYSIZE(g_highName));
			PlayPrioritySound(kCarriageSound, kCarriagePriority);
			EndDialog(hDlg, IDOK);
			break;

		case kHighNameItem:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				int nChars = GetDialogStringLen(hDlg, kHighNameItem);
				SetDlgItemInt(hDlg, kNameNCharsItem, nChars, FALSE); 
				if (GetWindowTextLength(GetDlgItem(hDlg, kNameNCharsItem)) != 0)
					PlayPrioritySound(kTypingSound, kTypingPriority);
			}
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  GetHighScoreName
// Brings up a dialog to get player's name (due to a high score).

void GetHighScoreName (HWND ownerWindow, SInt16 place)
{
	DialogParams params = { 0 };
	wchar_t scoreStr[32];
	wchar_t placeStr[32];

	NumToString(g_theScore, scoreStr, ARRAYSIZE(scoreStr));
	NumToString(place, placeStr, ARRAYSIZE(placeStr));

	params.arg[0] = scoreStr;
	params.arg[1] = placeStr;
	params.arg[2] = g_thisHouseName;
	DialogBoxParam(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(kHighNameDialogID),
			ownerWindow, NameFilter, (LPARAM)&params);
}

//--------------------------------------------------------------  BannerFilter
// Dialog filter for the "Enter Message" dialog.

INT_PTR CALLBACK BannerFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	(void)lParam;

	switch (message)
	{
	case WM_INITDIALOG:
		CenterDialogOverOwner(hDlg);
		SetDialogString(hDlg, kHighBannerItem, g_highBanner);
		SendDlgItemMessage(hDlg, kHighBannerItem,
				EM_LIMITTEXT, ARRAYSIZE(g_highBanner) - 1, 0);
		PlayPrioritySound(kEnergizeSound, kEnergizePriority);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			GetDialogString(hDlg, kHighBannerItem, g_highBanner, ARRAYSIZE(g_highBanner));
			PlayPrioritySound(kCarriageSound, kCarriagePriority);
			EndDialog(hDlg, IDOK);
			break;

		case kHighBannerItem:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				int nChars = GetDialogStringLen(hDlg, kHighBannerItem);
				SetDlgItemInt(hDlg, kBannerScoreNCharsItem, nChars, FALSE);
				if (GetWindowTextLength(GetDlgItem(hDlg, kBannerScoreNCharsItem)) != 0)
					PlayPrioritySound(kTypingSound, kTypingPriority);
			}
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  GetHighScoreBanner
// A player who gets the #1 slot gets to enter a short message (that
// appears across the top of the high scores list).  This dialog
// gets that message.

void GetHighScoreBanner (HWND ownerWindow)
{
	DialogBox(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(kHighBannerDialogID),
			ownerWindow, BannerFilter);
}

//--------------------------------------------------------------  FindHighScoresFolder

Boolean FindHighScoresFolder (LPWSTR scoresDirPath, DWORD cchDirPath)
{
	// This used to search the system preferences folder for a folder
	// named "G-PRO Scores ƒ". Now it just uses the "Scores" folder under
	// "%APPDATA%\glider-pro-mermaid" or next to the executable (depending
	// on whether "portable.dat" exists next to the executable or not.
	// (See the GetDataFolderPath function.)

	WCHAR pathBuffer[MAX_PATH];
	HRESULT hr;

	if (!GetDataFolderPath(pathBuffer, ARRAYSIZE(pathBuffer)))
		return false;
	if (!PathAppend(pathBuffer, L"Scores"))
		return false;
	if (!CreateDirectory(pathBuffer, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
		return false;

	hr = StringCchCopy(scoresDirPath, cchDirPath, pathBuffer);
	return SUCCEEDED(hr);
}

//--------------------------------------------------------------  GetHighScoresFilePath

Boolean GetHighScoresFilePath (PWSTR pszPath, DWORD cchPath, PCWSTR pszHousePath)
{
	WCHAR pathBuffer[MAX_PATH];

	if (!FindHighScoresFolder(pathBuffer, ARRAYSIZE(pathBuffer)))
		return false;
	if (!PathAppend(pathBuffer, PathFindFileName(pszHousePath)))
		return false;
	if (!PathRenameExtension(pathBuffer, L".gls"))
		return false;
	if (FAILED(StringCchCopy(pszPath, cchPath, pathBuffer)))
		return false;

	return true;
}

//--------------------------------------------------------------  WriteScoresToDisk

Boolean WriteScoresToDisk (HWND ownerWindow)
{
	PWSTR houseFilePath;
	WCHAR pathBuffer[MAX_PATH];
	HANDLE scoresFileHandle;
	byteio *byteWriter;
	HRESULT writeResult;
	HRESULT closeResult;

	if (FAILED(Gp_GetHouseFilePath(g_theHouseFile, &houseFilePath)))
		RedAlert(kErrNoMemory);

	if (!GetHighScoresFilePath(pathBuffer, ARRAYSIZE(pathBuffer), houseFilePath))
	{
		free(houseFilePath);
		return false;
	}

	scoresFileHandle = CreateFile(pathBuffer, GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (scoresFileHandle == INVALID_HANDLE_VALUE)
	{
		CheckFileError(ownerWindow, HRESULT_FROM_WIN32(GetLastError()), L"High Scores File");
		free(houseFilePath);
		return false;
	}

	byteWriter = byteio_init_handle_writer(scoresFileHandle);
	if (byteWriter == NULL)
	{
		CloseHandle(scoresFileHandle);
		free(houseFilePath);
		return false;
	}
	writeResult = WriteScoresType(byteWriter, &g_thisHouse.highScores);
	closeResult = byteio_close(byteWriter);
	CloseHandle(scoresFileHandle);
	if (FAILED(writeResult))
	{
		CheckFileError(ownerWindow, writeResult, L"High Scores File");
		free(houseFilePath);
		return false;
	}
	if (FAILED(closeResult))
	{
		CheckFileError(ownerWindow, closeResult, L"High Scores File");
		free(houseFilePath);
		return false;
	}

	g_gameDirty = false;
	free(houseFilePath);
	return true;
}

//--------------------------------------------------------------  ReadScoresFromDisk

Boolean ReadScoresFromDisk (HWND ownerWindow)
{
	scoresType tempScores;
	PWSTR houseFilePath;
	WCHAR pathBuffer[MAX_PATH];
	HANDLE scoresFileHandle;
	byteio *byteReader;
	HRESULT readResult;
	DWORD lastError;

	if (FAILED(Gp_GetHouseFilePath(g_theHouseFile, &houseFilePath)))
		RedAlert(kErrNoMemory);

	if (!GetHighScoresFilePath(pathBuffer, ARRAYSIZE(pathBuffer), houseFilePath))
	{
		free(houseFilePath);
		return false;
	}

	scoresFileHandle = CreateFile(pathBuffer, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	lastError = GetLastError();
	if (scoresFileHandle == INVALID_HANDLE_VALUE)
	{
		if (lastError != ERROR_FILE_NOT_FOUND)
			CheckFileError(ownerWindow, HRESULT_FROM_WIN32(lastError), L"High Scores File");
		free(houseFilePath);
		return false;
	}

	byteReader = byteio_init_handle_reader(scoresFileHandle);
	if (byteReader == NULL)
	{
		CloseHandle(scoresFileHandle);
		free(houseFilePath);
		return false;
	}
	readResult = ReadScoresType(byteReader, &tempScores);
	byteio_close(byteReader);
	CloseHandle(scoresFileHandle);
	if (FAILED(readResult))
	{
		CheckFileError(ownerWindow, readResult, L"High Scores File");
		free(houseFilePath);
		return false;
	}

	g_thisHouse.highScores = tempScores;
	free(houseFilePath);
	return true;
}
