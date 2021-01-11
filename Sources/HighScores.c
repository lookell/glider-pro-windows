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

#include <strsafe.h>

#define kHighNameItem			1002
#define kNameNCharsItem			1005
#define kHighBannerItem			1002
#define kBannerScoreNCharsItem	1005

void DrawHighScores (void);
void SortHighScores (void);
INT_PTR CALLBACK NameFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void GetHighScoreName (HWND ownerWindow, SInt16 place);
INT_PTR CALLBACK BannerFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void GetHighScoreBanner (HWND ownerWindow);
Boolean FindHighScoresFolder (LPWSTR scoresDirPath, DWORD cchDirPath);
Boolean GetHighScoresFilePath (LPWSTR lpPath, DWORD cchPath, ConstStringPtr baseName);

Str31 g_highBanner;
Str15 g_highName;
SInt16 g_lastHighScore;

//==============================================================  Functions
//--------------------------------------------------------------  DoHighScores
// Handles fading in and cleaning up the high scores screen.

void DoHighScores (void)
{
	Rect		tempRect;

	Mac_PaintRect(g_workSrcMap, &g_workSrcRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
	QSetRect(&tempRect, 0, 0, 640, 460);
	QOffsetRect(&tempRect, g_splashOriginH, g_splashOriginV);
	LoadScaledGraphic(g_workSrcMap, g_theHouseFile, kStarPictID, &tempRect);
	DissolveScreenOn(&g_workSrcRect);
	DrawHighScores();
	DissolveScreenOn(&g_workSrcRect);
	Sleep(1000);
	WaitForInputEvent(30);
}

//--------------------------------------------------------------  DrawHighScores
// Draws the actual scores on the screen.

#define kScoreSpacing			18
#define kScoreWide				352
#define kKimsLifted				4

void DrawHighScores (void)
{
	HDC			tempMap, tempMask;
	Rect		tempRect, tempRect2;
	Str255		tempStr;
	SInt16		scoreLeft, bannerWidth, i, dropIt;
	INT			hOffset, vOffset;
	HFONT		theFont;
	COLORREF	wasColor;

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
	SelectObject(g_workSrcMap, theFont);
	PasStringCopyC("\xA5 ", tempStr); // "• "
	PasStringConcat(tempStr, g_thisHouseName);
	PasStringConcatC(tempStr, " \xA5"); // " •"
	hOffset = scoreLeft + ((kScoreWide - Mac_StringWidth(g_workSrcMap, tempStr)) / 2);
	vOffset = dropIt - 65;
	MoveToEx(g_workSrcMap, hOffset - 1, vOffset - 1, NULL);
	SetTextColor(g_workSrcMap, blackColor);
	Mac_DrawString(g_workSrcMap, tempStr);
	MoveToEx(g_workSrcMap, hOffset, vOffset, NULL);
	SetTextColor(g_workSrcMap, cyanColor);
	Mac_DrawString(g_workSrcMap, tempStr);
	RestoreDC(g_workSrcMap, -1);
	DeleteObject(theFont);

	theFont = CreateTahomaFont(-12, FW_BOLD);
	SaveDC(g_workSrcMap);
	SelectObject(g_workSrcMap, theFont);
													// message for score #1
	PasStringCopy(g_thisHouse.highScores.banner, tempStr);
	bannerWidth = Mac_StringWidth(g_workSrcMap, tempStr);
	hOffset = scoreLeft + (kScoreWide - bannerWidth) / 2;
	vOffset = dropIt - kKimsLifted;
	SetTextColor(g_workSrcMap, blackColor);
	MoveToEx(g_workSrcMap, hOffset, vOffset, NULL);
	Mac_DrawString(g_workSrcMap, tempStr);
	SetTextColor(g_workSrcMap, yellowColor);
	MoveToEx(g_workSrcMap, hOffset, vOffset - 1, NULL);
	Mac_DrawString(g_workSrcMap, tempStr);

	QSetRect(&tempRect, 0, 0, bannerWidth + 8, kScoreSpacing);
	QOffsetRect(&tempRect, scoreLeft - 3 + (kScoreWide - bannerWidth) / 2,
			dropIt + 5 - kScoreSpacing - kKimsLifted);
	wasColor = SetDCBrushColor(g_workSrcMap, blackColor);
	Mac_FrameRect(g_workSrcMap, &tempRect, (HBRUSH)GetStockObject(DC_BRUSH), 1, 1);
	QOffsetRect(&tempRect, -1, -1);
	SetDCBrushColor(g_workSrcMap, yellowColor);
	Mac_FrameRect(g_workSrcMap, &tempRect, (HBRUSH)GetStockObject(DC_BRUSH), 1, 1);
	SetDCBrushColor(g_workSrcMap, wasColor);

	for (i = 0; i < kMaxScores; i++)
	{
		if (g_thisHouse.highScores.scores[i] > 0L)
		{
			NumToString(i + 1L, tempStr);		// draw placing number
			SetTextColor(g_workSrcMap, blackColor);
			if (i == 0)
				MoveToEx(g_workSrcMap, scoreLeft + 1, dropIt - kScoreSpacing - kKimsLifted, NULL);
			else
				MoveToEx(g_workSrcMap, scoreLeft + 1, dropIt + (i * kScoreSpacing), NULL);
			Mac_DrawString(g_workSrcMap, tempStr);
			if (i == g_lastHighScore)
				SetTextColor(g_workSrcMap, whiteColor);
			else
				SetTextColor(g_workSrcMap, cyanColor);
			if (i == 0)
				MoveToEx(g_workSrcMap, scoreLeft + 0, dropIt - 1 - kScoreSpacing - kKimsLifted, NULL);
			else
				MoveToEx(g_workSrcMap, scoreLeft + 0, dropIt - 1 + (i * kScoreSpacing), NULL);
			Mac_DrawString(g_workSrcMap, tempStr);
													// draw high score name
			PasStringCopy(g_thisHouse.highScores.names[i], tempStr);
			SetTextColor(g_workSrcMap, blackColor);
			if (i == 0)
				MoveToEx(g_workSrcMap, scoreLeft + 31, dropIt - kScoreSpacing - kKimsLifted, NULL);
			else
				MoveToEx(g_workSrcMap, scoreLeft + 31, dropIt + (i * kScoreSpacing), NULL);
			Mac_DrawString(g_workSrcMap, tempStr);
			if (i == g_lastHighScore)
				SetTextColor(g_workSrcMap, whiteColor);
			else
				SetTextColor(g_workSrcMap, yellowColor);
			if (i == 0)
				MoveToEx(g_workSrcMap, scoreLeft + 30, dropIt - 1 - kScoreSpacing - kKimsLifted, NULL);
			else
				MoveToEx(g_workSrcMap, scoreLeft + 30, dropIt - 1 + (i * kScoreSpacing), NULL);
			Mac_DrawString(g_workSrcMap, tempStr);
													// draw level number
			NumToString(g_thisHouse.highScores.levels[i], tempStr);
			SetTextColor(g_workSrcMap, blackColor);
			if (i == 0)
				MoveToEx(g_workSrcMap, scoreLeft + 161, dropIt - kScoreSpacing - kKimsLifted, NULL);
			else
				MoveToEx(g_workSrcMap, scoreLeft + 161, dropIt + (i * kScoreSpacing), NULL);
			Mac_DrawString(g_workSrcMap, tempStr);
			if (i == g_lastHighScore)
				SetTextColor(g_workSrcMap, whiteColor);
			else
				SetTextColor(g_workSrcMap, yellowColor);
			if (i == 0)
				MoveToEx(g_workSrcMap, scoreLeft + 160, dropIt - 1 - kScoreSpacing - kKimsLifted, NULL);
			else
				MoveToEx(g_workSrcMap, scoreLeft + 160, dropIt - 1 + (i * kScoreSpacing), NULL);
			Mac_DrawString(g_workSrcMap, tempStr);
													// draw word "rooms"
			if (g_thisHouse.highScores.levels[i] == 1)
				GetLocalizedString_Pascal(6, tempStr);
			else
				GetLocalizedString_Pascal(7, tempStr);
			SetTextColor(g_workSrcMap, blackColor);
			if (i == 0)
				MoveToEx(g_workSrcMap, scoreLeft + 193, dropIt - kScoreSpacing - kKimsLifted, NULL);
			else
				MoveToEx(g_workSrcMap, scoreLeft + 193, dropIt + (i * kScoreSpacing), NULL);
			Mac_DrawString(g_workSrcMap, tempStr);
			SetTextColor(g_workSrcMap, cyanColor);
			if (i == 0)
				MoveToEx(g_workSrcMap, scoreLeft + 192, dropIt - 1 - kScoreSpacing - kKimsLifted, NULL);
			else
				MoveToEx(g_workSrcMap, scoreLeft + 192, dropIt - 1 + (i * kScoreSpacing), NULL);
			Mac_DrawString(g_workSrcMap, tempStr);
													// draw high score points
			NumToString(g_thisHouse.highScores.scores[i], tempStr);
			SetTextColor(g_workSrcMap, blackColor);
			if (i == 0)
				MoveToEx(g_workSrcMap, scoreLeft + 291, dropIt - kScoreSpacing - kKimsLifted, NULL);
			else
				MoveToEx(g_workSrcMap, scoreLeft + 291, dropIt + (i * kScoreSpacing), NULL);
			Mac_DrawString(g_workSrcMap, tempStr);
			if (i == g_lastHighScore)
				SetTextColor(g_workSrcMap, whiteColor);
			else
				SetTextColor(g_workSrcMap, yellowColor);
			if (i == 0)
				MoveToEx(g_workSrcMap, scoreLeft + 290, dropIt - 1 - kScoreSpacing - kKimsLifted, NULL);
			else
				MoveToEx(g_workSrcMap, scoreLeft + 290, dropIt - 1 + (i * kScoreSpacing), NULL);
			Mac_DrawString(g_workSrcMap, tempStr);
		}
	}

	RestoreDC(g_workSrcMap, -1);
	DeleteObject(theFont);

	theFont = CreateTahomaFont(-9, FW_BOLD);
	SaveDC(g_workSrcMap);
	SelectObject(g_workSrcMap, theFont);
	SetTextColor(g_workSrcMap, blueColor);
	MoveToEx(g_workSrcMap, scoreLeft + 80, dropIt - 1 + (10 * kScoreSpacing), NULL);
	GetLocalizedString_Pascal(8, tempStr);
	Mac_DrawString(g_workSrcMap, tempStr);
	RestoreDC(g_workSrcMap, -1);
	DeleteObject(theFont);
}

//--------------------------------------------------------------  SortHighScores
// This does a simple sort of the high scores.

void SortHighScores (void)
{
	scoresType	tempScores;
	SInt32		greatest;
	SInt16		i, h, which;

	for (h = 0; h < kMaxScores; h++)
	{
		greatest = -1L;
		which = -1;
		for (i = 0; i < kMaxScores; i++)
		{
			if (g_thisHouse.highScores.scores[i] > greatest)
			{
				greatest = g_thisHouse.highScores.scores[i];
				which = i;
			}
		}
		if (which != -1)
		{
			PasStringCopy(g_thisHouse.highScores.names[which], tempScores.names[h]);
			tempScores.scores[h] = g_thisHouse.highScores.scores[which];
			tempScores.timeStamps[h] = g_thisHouse.highScores.timeStamps[which];
			tempScores.levels[h] = g_thisHouse.highScores.levels[which];
			g_thisHouse.highScores.scores[which] = -1L;
		}
	}
	PasStringCopy(g_thisHouse.highScores.banner, tempScores.banner);
	g_thisHouse.highScores = tempScores;
}

//--------------------------------------------------------------  ZeroHighScores
// This funciton goes through and resets or "zeros" all high scores.

void ZeroHighScores (housePtr house)
{
	SInt16 i;

	PasStringCopy(g_thisHouseName, house->highScores.banner);
	for (i = 0; i < kMaxScores; i++)
	{
		PasStringCopyC("--------------", house->highScores.names[i]);
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
		PasStringCopyC("--------------", house->highScores.names[i]);
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
	SInt16		placing, i;

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
		PasStringCopy(g_highName, g_thisHouse.highScores.names[kMaxScores - 1]);
		if (placing == 0)
		{
			GetHighScoreBanner(ownerWindow);
			PasStringCopy(g_highBanner, g_thisHouse.highScores.banner);
		}
		g_thisHouse.highScores.scores[kMaxScores - 1] = g_theScore;
		g_thisHouse.highScores.timeStamps[kMaxScores - 1] = Mac_GetDateTime();
		g_thisHouse.highScores.levels[kMaxScores - 1] = CountRoomsVisited();
		SortHighScores();
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
	wchar_t houseStr[64];

	StringCchPrintf(scoreStr, ARRAYSIZE(scoreStr), L"%ld", (long)g_theScore);
	StringCchPrintf(placeStr, ARRAYSIZE(placeStr), L"%ld", (long)place);
	WinFromMacString(houseStr, ARRAYSIZE(houseStr), g_thisHouseName);

	params.arg[0] = scoreStr;
	params.arg[1] = placeStr;
	params.arg[2] = houseStr;
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
	// "%APPDATA%\glider-pro-windows" or next to the executable (depending
	// on whether "portable.dat" exists next to the executable or not.
	// (See the GetDataFolderPath function.)

	WCHAR pathBuffer[MAX_PATH];
	HRESULT hr;

	if (!GetDataFolderPath(pathBuffer, ARRAYSIZE(pathBuffer)))
		return false;
	hr = StringCchCat(pathBuffer, ARRAYSIZE(pathBuffer), L"\\Scores");
	if (FAILED(hr))
		return false;
	if (!CreateDirectory(pathBuffer, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
		return false;

	hr = StringCchCopy(scoresDirPath, cchDirPath, pathBuffer);
	return SUCCEEDED(hr);
}

//--------------------------------------------------------------  GetHighScoresFilePath

Boolean GetHighScoresFilePath (LPWSTR lpPath, DWORD cchPath, ConstStringPtr baseName)
{
	WCHAR pathBuffer[MAX_PATH];
	WCHAR wideBaseName[256];

	if (!WinFromMacString(wideBaseName, ARRAYSIZE(wideBaseName), baseName))
		return false;
	if (!FindHighScoresFolder(pathBuffer, ARRAYSIZE(pathBuffer)))
		return false;
	if (FAILED(StringCchCat(pathBuffer, ARRAYSIZE(pathBuffer), L"\\")))
		return false;
	if (FAILED(StringCchCat(pathBuffer, ARRAYSIZE(pathBuffer), wideBaseName)))
		return false;
	if (FAILED(StringCchCat(pathBuffer, ARRAYSIZE(pathBuffer), L".gls")))
		return false;

	return SUCCEEDED(StringCchCopy(lpPath, cchPath, pathBuffer));
}

//--------------------------------------------------------------  WriteScoresToDisk

Boolean WriteScoresToDisk (HWND ownerWindow)
{
	WCHAR		pathBuffer[MAX_PATH];
	HANDLE		scoresFileHandle;
	byteio		*byteWriter;
	HRESULT		writeResult;
	HRESULT		closeResult;

	if (!GetHighScoresFilePath(pathBuffer, ARRAYSIZE(pathBuffer), g_thisHouseName))
		return false;

	scoresFileHandle = CreateFile(pathBuffer, GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (scoresFileHandle == INVALID_HANDLE_VALUE)
	{
		CheckFileError(ownerWindow, HRESULT_FROM_WIN32(GetLastError()), L"High Scores File");
		return false;
	}

	byteWriter = byteio_init_handle_writer(scoresFileHandle);
	if (byteWriter == NULL)
	{
		CloseHandle(scoresFileHandle);
		return false;
	}
	writeResult = WriteScoresType(byteWriter, &g_thisHouse.highScores);
	closeResult = byteio_close(byteWriter);
	CloseHandle(scoresFileHandle);
	if (FAILED(writeResult))
	{
		CheckFileError(ownerWindow, writeResult, L"High Scores File");
		return false;
	}
	if (FAILED(closeResult))
	{
		CheckFileError(ownerWindow, closeResult, L"High Scores File");
		return false;
	}

	g_gameDirty = false;
	return true;
}

//--------------------------------------------------------------  ReadScoresFromDisk

Boolean ReadScoresFromDisk (HWND ownerWindow)
{
	scoresType	tempScores;
	WCHAR		pathBuffer[MAX_PATH];
	HANDLE		scoresFileHandle;
	byteio		*byteReader;
	HRESULT		readResult;
	DWORD		lastError;

	if (!GetHighScoresFilePath(pathBuffer, ARRAYSIZE(pathBuffer), g_thisHouseName))
		return false;

	scoresFileHandle = CreateFile(pathBuffer, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	lastError = GetLastError();
	if (scoresFileHandle == INVALID_HANDLE_VALUE)
	{
		if (lastError != ERROR_FILE_NOT_FOUND)
			CheckFileError(ownerWindow, HRESULT_FROM_WIN32(lastError), L"High Scores File");
		return false;
	}

	byteReader = byteio_init_handle_reader(scoresFileHandle);
	if (byteReader == NULL)
	{
		CloseHandle(scoresFileHandle);
		return false;
	}
	readResult = ReadScoresType(byteReader, &tempScores);
	byteio_close(byteReader);
	CloseHandle(scoresFileHandle);
	if (FAILED(readResult))
	{
		CheckFileError(ownerWindow, readResult, L"High Scores File");
		return false;
	}

	g_thisHouse.highScores = tempScores;
	return true;
}
