#define GP_USE_WINAPI_H

#include "Scoreboard.h"

//============================================================================
//----------------------------------------------------------------------------
//                                Scoreboard.c
//----------------------------------------------------------------------------
//============================================================================


#include "ColorUtils.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "Play.h"
#include "Player.h"
#include "RectUtils.h"
#include "Room.h"
#include "RoomGraphics.h"
#include "Sound.h"
#include "StringUtils.h"

#include <strsafe.h>


#define kGrayBackgroundColor    251
#define kScoreRollAmount        13


void RefreshRoomTitle (SInt16 mode);
void RefreshNumGliders (void);
void RefreshPoints (void);


Rect boardSrcRect;
Rect badgeSrcRect;
Rect boardDestRect;
HDC boardSrcMap;
HDC badgeSrcMap;
Rect boardTSrcRect;
Rect boardTDestRect;
HDC boardTSrcMap;
Rect boardGSrcRect;
Rect boardGDestRect;
HDC boardGSrcMap;
Rect boardPSrcRect;
Rect boardPDestRect;
HDC boardPSrcMap;
Rect boardPQDestRect;
Rect boardGQDestRect;
Rect badgesBlankRects[kNumBadges];
Rect badgesBadgesRects[kNumBadges];
Rect badgesDestRects[kNumBadges];
SInt16 wasScoreboardMode;

static SInt32 displayedScore;
static Boolean doRollScore;


//==============================================================  Functions
//--------------------------------------------------------------  RefreshScoreboard

void RefreshScoreboard (SInt16 mode)
{
	HDC			mainWindowDC;

	doRollScore = true;

	RefreshRoomTitle(mode);
	RefreshNumGliders();
	RefreshPoints();

	mainWindowDC = GetMainWindowDC();
	Mac_CopyBits(boardSrcMap, mainWindowDC,
			&boardSrcRect, &boardDestRect, srcCopy, nil);
	ReleaseMainWindowDC(mainWindowDC);

	QuickBatteryRefresh(false);
	QuickBandsRefresh(false);
	QuickFoilRefresh(false);
}

//--------------------------------------------------------------  HandleDynamicScoreboard

void HandleDynamicScoreboard (void)
{
	#define		kFoilLow		2		// 25%
	#define		kBatteryLow		17		// 25%
	#define		kHeliumLow		-38		// 25%
	#define		kBandsLow		2		// 25%
	SInt32		whosTurn;

	if (theScore > displayedScore)
	{
		if (doRollScore)
		{
			displayedScore += kScoreRollAmount;
			if (displayedScore > theScore)
				displayedScore = theScore;
		}
		else
			displayedScore = theScore;

		PlayPrioritySound(kScoreTikSound, kScoreTikPriority);
		QuickScoreRefresh();
	}

	whosTurn = gameFrame & 0x00000007;
	switch (whosTurn)
	{
		case 0:		// show foil
		if ((foilTotal > 0) && (foilTotal < kFoilLow))
			QuickFoilRefresh(false);
		break;

		case 1:		// hide battery
		if ((batteryTotal > 0) && (batteryTotal < kBatteryLow))
			QuickBatteryRefresh(true);
		else if ((batteryTotal < 0) && (batteryTotal > kHeliumLow))
			QuickBatteryRefresh(true);
		break;

		case 2:		// show rubber bands
		if ((bandsTotal > 0) && (bandsTotal < kBandsLow))
			QuickBandsRefresh(false);
		break;

		case 4:		// show battery
		if ((batteryTotal > 0) && (batteryTotal < kBatteryLow))
			QuickBatteryRefresh(false);
		else if ((batteryTotal < 0) && (batteryTotal > kHeliumLow))
			QuickBatteryRefresh(false);
		break;

		case 5:		// hide foil
		if ((foilTotal > 0) && (foilTotal < kFoilLow))
			QuickFoilRefresh(true);
		break;

		case 7:		// hide rubber bands
		if ((bandsTotal > 0) && (bandsTotal < kBandsLow))
			QuickBandsRefresh(true);
		break;
	}
}

//--------------------------------------------------------------  RefreshRoomTitle

void RefreshRoomTitle (SInt16 mode)
{
	wchar_t titleString[256];
	int numChars;

	ColorRect(boardTSrcMap, &boardTSrcRect, kGrayBackgroundColor);

	switch (mode)
	{
	case kEscapedTitleMode:
		StringCchCopy(titleString, ARRAYSIZE(titleString),
			L"Hit Delete key if unable to Follow");
		break;
	case kSavingTitleMode:
		StringCchCopy(titleString, ARRAYSIZE(titleString), L"Saving Game…");
		break;
	default:
		WinFromMacString(titleString, ARRAYSIZE(titleString), thisRoom->name);
		break;
	}
	numChars = (int)wcslen(titleString);

	SaveDC(boardTSrcMap);
	SetBkMode(boardTSrcMap, TRANSPARENT);
	SetTextAlign(boardTSrcMap, TA_TOP | TA_LEFT);

	SetTextColor(boardTSrcMap, blackColor);
	TextOut(boardTSrcMap, 1, 1, titleString, numChars);

	SetTextColor(boardTSrcMap, whiteColor);
	TextOut(boardTSrcMap, 0, 0, titleString, numChars);

	RestoreDC(boardTSrcMap, -1);

	Mac_CopyBits(boardTSrcMap, boardSrcMap,
			&boardTSrcRect, &boardTDestRect, srcCopy, nil);
}

//--------------------------------------------------------------  RefreshNumGliders

void RefreshNumGliders (void)
{
	wchar_t nGlidersStr[256];
	SInt32 displayMortals;
	int numChars;

	ColorRect(boardGSrcMap, &boardGSrcRect, kGrayBackgroundColor);

	displayMortals = mortals;
	if (displayMortals < 0)
	{
		displayMortals = 0;
	}
	StringCchPrintf(nGlidersStr, ARRAYSIZE(nGlidersStr), L"%ld", (long)displayMortals);
	numChars = (int)wcslen(nGlidersStr);

	SaveDC(boardGSrcMap);
	SetBkMode(boardGSrcMap, TRANSPARENT);
	SetTextAlign(boardGSrcMap, TA_TOP | TA_LEFT);

	SetTextColor(boardGSrcMap, blackColor);
	TextOut(boardGSrcMap, 1, 1, nGlidersStr, numChars);

	SetTextColor(boardGSrcMap, whiteColor);
	TextOut(boardGSrcMap, 0, 0, nGlidersStr, numChars);

	RestoreDC(boardGSrcMap, -1);

	Mac_CopyBits(boardGSrcMap, boardSrcMap,
			&boardGSrcRect, &boardGDestRect, srcCopy, nil);
}

//--------------------------------------------------------------  RefreshPoints

void RefreshPoints (void)
{
	wchar_t scoreStr[256];
	int numChars;

	ColorRect(boardPSrcMap, &boardPSrcRect, kGrayBackgroundColor);

	StringCchPrintf(scoreStr, ARRAYSIZE(scoreStr), L"%ld", (long)theScore);
	numChars = (int)wcslen(scoreStr);

	SaveDC(boardPSrcMap);
	SetBkMode(boardPSrcMap, TRANSPARENT);
	SetTextAlign(boardPSrcMap, TA_TOP | TA_LEFT);

	SetTextColor(boardPSrcMap, blackColor);
	TextOut(boardPSrcMap, 1, 1, scoreStr, numChars);

	SetTextColor(boardPSrcMap, whiteColor);
	TextOut(boardPSrcMap, 0, 0, scoreStr, numChars);

	RestoreDC(boardPSrcMap, -1);

	Mac_CopyBits(boardPSrcMap, boardSrcMap,
			&boardPSrcRect, &boardPDestRect, srcCopy, nil);

	displayedScore = theScore;
}

//--------------------------------------------------------------  QuickGlidersRefresh

void QuickGlidersRefresh (void)
{
	wchar_t nGlidersStr[256];
	int numChars;
	HDC mainWindowDC;

	ColorRect(boardGSrcMap, &boardGSrcRect, kGrayBackgroundColor);

	StringCchPrintf(nGlidersStr, ARRAYSIZE(nGlidersStr), L"%ld", (long)mortals);
	numChars = (int)wcslen(nGlidersStr);

	SaveDC(boardGSrcMap);
	SetBkMode(boardGSrcMap, TRANSPARENT);
	SetTextAlign(boardGSrcMap, TA_TOP | TA_LEFT);

	SetTextColor(boardGSrcMap, blackColor);
	TextOut(boardGSrcMap, 1, 1, nGlidersStr, numChars);

	SetTextColor(boardGSrcMap, whiteColor);
	TextOut(boardGSrcMap, 0, 0, nGlidersStr, numChars);

	RestoreDC(boardGSrcMap, -1);

	mainWindowDC = GetMainWindowDC();
	Mac_CopyBits(boardGSrcMap, mainWindowDC,
			&boardGSrcRect, &boardGQDestRect, srcCopy, nil);
	ReleaseMainWindowDC(mainWindowDC);
}

//--------------------------------------------------------------  QuickScoreRefresh

void QuickScoreRefresh (void)
{
	wchar_t scoreStr[256];
	int numChars;
	HDC mainWindowDC;

	ColorRect(boardPSrcMap, &boardPSrcRect, kGrayBackgroundColor);

	StringCchPrintf(scoreStr, ARRAYSIZE(scoreStr), L"%ld", (long)displayedScore);
	numChars = (int)wcslen(scoreStr);

	SaveDC(boardPSrcMap);
	SetBkMode(boardPSrcMap, TRANSPARENT);
	SetTextAlign(boardPSrcMap, TA_TOP | TA_LEFT);

	SetTextColor(boardPSrcMap, blackColor);
	TextOut(boardPSrcMap, 1, 1, scoreStr, numChars);

	SetTextColor(boardPSrcMap, whiteColor);
	TextOut(boardPSrcMap, 0, 0, scoreStr, numChars);

	RestoreDC(boardPSrcMap, -1);

	mainWindowDC = GetMainWindowDC();
	Mac_CopyBits(boardPSrcMap, mainWindowDC,
			&boardPSrcRect, &boardPQDestRect, srcCopy, nil);
	ReleaseMainWindowDC(mainWindowDC);
}

//--------------------------------------------------------------  QuickBatteryRefresh

void QuickBatteryRefresh (Boolean flash)
{
	HDC			mainWindowDC;

	mainWindowDC = GetMainWindowDC();
	if ((batteryTotal > 0) && (!flash))
	{
		Mac_CopyBits(badgeSrcMap, mainWindowDC,
				&badgesBadgesRects[kBatteryBadge],
				&badgesDestRects[kBatteryBadge],
				srcCopy, nil);
	}
	else if ((batteryTotal < 0) && (!flash))
	{
		Mac_CopyBits(badgeSrcMap, mainWindowDC,
				&badgesBadgesRects[kHeliumBadge],
				&badgesDestRects[kHeliumBadge],
				srcCopy, nil);
	}
	else
	{
		Mac_CopyBits(badgeSrcMap, mainWindowDC,
				&badgesBlankRects[kBatteryBadge],
				&badgesDestRects[kBatteryBadge],
				srcCopy, nil);
	}
	ReleaseMainWindowDC(mainWindowDC);
}

//--------------------------------------------------------------  QuickBandsRefresh

void QuickBandsRefresh (Boolean flash)
{
	HDC			mainWindowDC;

	mainWindowDC = GetMainWindowDC();
	if ((bandsTotal > 0) && (!flash))
	{
		Mac_CopyBits(badgeSrcMap, mainWindowDC,
				&badgesBadgesRects[kBandsBadge],
				&badgesDestRects[kBandsBadge],
				srcCopy, nil);
	}
	else
	{
		Mac_CopyBits(badgeSrcMap, mainWindowDC,
				&badgesBlankRects[kBandsBadge],
				&badgesDestRects[kBandsBadge],
				srcCopy, nil);
	}
	ReleaseMainWindowDC(mainWindowDC);
}

//--------------------------------------------------------------  QuickFoilRefresh

void QuickFoilRefresh (Boolean flash)
{
	HDC			mainWindowDC;

	mainWindowDC = GetMainWindowDC();
	if ((foilTotal > 0) && (!flash))
	{
		Mac_CopyBits(badgeSrcMap, mainWindowDC,
				&badgesBadgesRects[kFoilBadge],
				&badgesDestRects[kFoilBadge],
				srcCopy, nil);
	}
	else
	{
		Mac_CopyBits(badgeSrcMap, mainWindowDC,
				&badgesBlankRects[kFoilBadge],
				&badgesDestRects[kFoilBadge],
				srcCopy, nil);
	}
	ReleaseMainWindowDC(mainWindowDC);
}

//--------------------------------------------------------------  AdjustScoreboardHeight

void AdjustScoreboardHeight (void)
{
	SInt16		offset, newMode;

	if (numNeighbors == 9)
		newMode = kScoreboardHigh;
	else
		newMode = kScoreboardLow;

	if (wasScoreboardMode != newMode)
	{
		offset = 0;
		switch (newMode)
		{
			case kScoreboardHigh:		// 9 neighbors
			offset = localRoomsDest[kCentralRoom].top;
			offset = -offset;
			justRoomsRect = workSrcRect;
			break;

			case kScoreboardLow:		// 1 or 3 neighbors
			offset = localRoomsDest[kCentralRoom].top;
			justRoomsRect = workSrcRect;
			justRoomsRect.top = localRoomsDest[kCentralRoom].top;
			justRoomsRect.bottom = localRoomsDest[kCentralRoom].bottom;
			break;
		}

		QOffsetRect(&boardDestRect, 0, offset);
		QOffsetRect(&boardGQDestRect, 0, offset);
		QOffsetRect(&boardPQDestRect, 0, offset);
		QOffsetRect(&badgesDestRects[kBatteryBadge], 0, offset);
		QOffsetRect(&badgesDestRects[kBandsBadge], 0, offset);
		QOffsetRect(&badgesDestRects[kFoilBadge], 0, offset);
		QOffsetRect(&badgesDestRects[kHeliumBadge], 0, offset);

		wasScoreboardMode = newMode;
	}
}

