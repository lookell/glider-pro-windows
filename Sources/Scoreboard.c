//============================================================================
//----------------------------------------------------------------------------
//                                Scoreboard.c
//----------------------------------------------------------------------------
//============================================================================

#include "Scoreboard.h"

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

Rect g_boardSrcRect;
Rect g_badgeSrcRect;
Rect g_boardDestRect;
HDC g_boardSrcMap;
HDC g_badgeSrcMap;
Rect g_boardTSrcRect;
Rect g_boardTDestRect;
HDC g_boardTSrcMap;
Rect g_boardGSrcRect;
Rect g_boardGDestRect;
HDC g_boardGSrcMap;
Rect g_boardPSrcRect;
Rect g_boardPDestRect;
HDC g_boardPSrcMap;
Rect g_boardPQDestRect;
Rect g_boardGQDestRect;
Rect g_badgesBlankRects[kNumBadges];
Rect g_badgesBadgesRects[kNumBadges];
Rect g_badgesDestRects[kNumBadges];
HFONT g_scoreboardFont;
SInt16 g_wasScoreboardMode;

static SInt32 g_displayedScore;
static Boolean g_doRollScore;

//==============================================================  Functions
//--------------------------------------------------------------  RefreshScoreboard

void RefreshScoreboard (SInt16 mode)
{
	HDC mainWindowDC;

	g_doRollScore = true;

	RefreshRoomTitle(mode);
	RefreshNumGliders();
	RefreshPoints();

	mainWindowDC = GetMainWindowDC(g_mainWindow);
	Mac_CopyBits(g_boardSrcMap, mainWindowDC,
			&g_boardSrcRect, &g_boardDestRect, srcCopy, nil);
	ReleaseMainWindowDC(g_mainWindow, mainWindowDC);

	QuickBatteryRefresh(false);
	QuickBandsRefresh(false);
	QuickFoilRefresh(false);
}

//--------------------------------------------------------------  HandleDynamicScoreboard

void HandleDynamicScoreboard (void)
{
	#define kFoilLow  2  // 25%
	#define kBatteryLow  17  // 25%
	#define kHeliumLow  -38  // 25%
	#define kBandsLow  2  // 25%
	SInt32 whosTurn;

	if (g_theScore > g_displayedScore)
	{
		if (g_doRollScore)
		{
			g_displayedScore += kScoreRollAmount;
			if (g_displayedScore > g_theScore)
				g_displayedScore = g_theScore;
		}
		else
			g_displayedScore = g_theScore;

		PlayPrioritySound(kScoreTikSound, kScoreTikPriority);
		QuickScoreRefresh();
	}

	whosTurn = g_gameFrame & 0x00000007;
	switch (whosTurn)
	{
		case 0:  // show foil
		if ((g_foilTotal > 0) && (g_foilTotal < kFoilLow))
			QuickFoilRefresh(false);
		break;

		case 1:  // hide battery
		if ((g_batteryTotal > 0) && (g_batteryTotal < kBatteryLow))
			QuickBatteryRefresh(true);
		else if ((g_batteryTotal < 0) && (g_batteryTotal > kHeliumLow))
			QuickBatteryRefresh(true);
		break;

		case 2:  // show rubber bands
		if ((g_bandsTotal > 0) && (g_bandsTotal < kBandsLow))
			QuickBandsRefresh(false);
		break;

		case 4:  // show battery
		if ((g_batteryTotal > 0) && (g_batteryTotal < kBatteryLow))
			QuickBatteryRefresh(false);
		else if ((g_batteryTotal < 0) && (g_batteryTotal > kHeliumLow))
			QuickBatteryRefresh(false);
		break;

		case 5:  // hide foil
		if ((g_foilTotal > 0) && (g_foilTotal < kFoilLow))
			QuickFoilRefresh(true);
		break;

		case 7:  // hide rubber bands
		if ((g_bandsTotal > 0) && (g_bandsTotal < kBandsLow))
			QuickBandsRefresh(true);
		break;
	}
}

//--------------------------------------------------------------  RefreshRoomTitle

void RefreshRoomTitle (SInt16 mode)
{
	wchar_t titleString[256];
	int numChars;

	ColorRect(g_boardTSrcMap, &g_boardTSrcRect, kGrayBackgroundColor);

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
		WinFromMacString(titleString, ARRAYSIZE(titleString), g_thisRoom->name);
		break;
	}
	numChars = (int)wcslen(titleString);

	SaveDC(g_boardTSrcMap);
	SetBkMode(g_boardTSrcMap, TRANSPARENT);
	SetTextAlign(g_boardTSrcMap, TA_TOP | TA_LEFT);
	SelectFont(g_boardTSrcMap, g_scoreboardFont);

	SetTextColor(g_boardTSrcMap, blackColor);
	TextOut(g_boardTSrcMap, 1, 1, titleString, numChars);

	SetTextColor(g_boardTSrcMap, whiteColor);
	TextOut(g_boardTSrcMap, 0, 0, titleString, numChars);

	RestoreDC(g_boardTSrcMap, -1);

	Mac_CopyBits(g_boardTSrcMap, g_boardSrcMap,
			&g_boardTSrcRect, &g_boardTDestRect, srcCopy, nil);
}

//--------------------------------------------------------------  RefreshNumGliders

void RefreshNumGliders (void)
{
	wchar_t nGlidersStr[256];
	SInt32 displayMortals;
	int numChars;

	ColorRect(g_boardGSrcMap, &g_boardGSrcRect, kGrayBackgroundColor);

	displayMortals = g_mortals;
	if (displayMortals < 0)
	{
		displayMortals = 0;
	}
	StringCchPrintf(nGlidersStr, ARRAYSIZE(nGlidersStr), L"%ld", (long)displayMortals);
	numChars = (int)wcslen(nGlidersStr);

	SaveDC(g_boardGSrcMap);
	SetBkMode(g_boardGSrcMap, TRANSPARENT);
	SetTextAlign(g_boardGSrcMap, TA_TOP | TA_LEFT);
	SelectFont(g_boardGSrcMap, g_scoreboardFont);

	SetTextColor(g_boardGSrcMap, blackColor);
	TextOut(g_boardGSrcMap, 1, 1, nGlidersStr, numChars);

	SetTextColor(g_boardGSrcMap, whiteColor);
	TextOut(g_boardGSrcMap, 0, 0, nGlidersStr, numChars);

	RestoreDC(g_boardGSrcMap, -1);

	Mac_CopyBits(g_boardGSrcMap, g_boardSrcMap,
			&g_boardGSrcRect, &g_boardGDestRect, srcCopy, nil);
}

//--------------------------------------------------------------  RefreshPoints

void RefreshPoints (void)
{
	wchar_t scoreStr[256];
	int numChars;

	ColorRect(g_boardPSrcMap, &g_boardPSrcRect, kGrayBackgroundColor);

	StringCchPrintf(scoreStr, ARRAYSIZE(scoreStr), L"%ld", (long)g_theScore);
	numChars = (int)wcslen(scoreStr);

	SaveDC(g_boardPSrcMap);
	SetBkMode(g_boardPSrcMap, TRANSPARENT);
	SetTextAlign(g_boardPSrcMap, TA_TOP | TA_LEFT);
	SelectFont(g_boardPSrcMap, g_scoreboardFont);

	SetTextColor(g_boardPSrcMap, blackColor);
	TextOut(g_boardPSrcMap, 1, 1, scoreStr, numChars);

	SetTextColor(g_boardPSrcMap, whiteColor);
	TextOut(g_boardPSrcMap, 0, 0, scoreStr, numChars);

	RestoreDC(g_boardPSrcMap, -1);

	Mac_CopyBits(g_boardPSrcMap, g_boardSrcMap,
			&g_boardPSrcRect, &g_boardPDestRect, srcCopy, nil);

	g_displayedScore = g_theScore;
}

//--------------------------------------------------------------  QuickGlidersRefresh

void QuickGlidersRefresh (void)
{
	wchar_t nGlidersStr[256];
	int numChars;
	HDC mainWindowDC;

	ColorRect(g_boardGSrcMap, &g_boardGSrcRect, kGrayBackgroundColor);

	StringCchPrintf(nGlidersStr, ARRAYSIZE(nGlidersStr), L"%ld", (long)g_mortals);
	numChars = (int)wcslen(nGlidersStr);

	SaveDC(g_boardGSrcMap);
	SetBkMode(g_boardGSrcMap, TRANSPARENT);
	SetTextAlign(g_boardGSrcMap, TA_TOP | TA_LEFT);
	SelectFont(g_boardGSrcMap, g_scoreboardFont);

	SetTextColor(g_boardGSrcMap, blackColor);
	TextOut(g_boardGSrcMap, 1, 1, nGlidersStr, numChars);

	SetTextColor(g_boardGSrcMap, whiteColor);
	TextOut(g_boardGSrcMap, 0, 0, nGlidersStr, numChars);

	RestoreDC(g_boardGSrcMap, -1);

	mainWindowDC = GetMainWindowDC(g_mainWindow);
	Mac_CopyBits(g_boardGSrcMap, mainWindowDC,
			&g_boardGSrcRect, &g_boardGQDestRect, srcCopy, nil);
	ReleaseMainWindowDC(g_mainWindow, mainWindowDC);
}

//--------------------------------------------------------------  QuickScoreRefresh

void QuickScoreRefresh (void)
{
	wchar_t scoreStr[256];
	int numChars;
	HDC mainWindowDC;

	ColorRect(g_boardPSrcMap, &g_boardPSrcRect, kGrayBackgroundColor);

	StringCchPrintf(scoreStr, ARRAYSIZE(scoreStr), L"%ld", (long)g_displayedScore);
	numChars = (int)wcslen(scoreStr);

	SaveDC(g_boardPSrcMap);
	SetBkMode(g_boardPSrcMap, TRANSPARENT);
	SetTextAlign(g_boardPSrcMap, TA_TOP | TA_LEFT);
	SelectFont(g_boardPSrcMap, g_scoreboardFont);

	SetTextColor(g_boardPSrcMap, blackColor);
	TextOut(g_boardPSrcMap, 1, 1, scoreStr, numChars);

	SetTextColor(g_boardPSrcMap, whiteColor);
	TextOut(g_boardPSrcMap, 0, 0, scoreStr, numChars);

	RestoreDC(g_boardPSrcMap, -1);

	mainWindowDC = GetMainWindowDC(g_mainWindow);
	Mac_CopyBits(g_boardPSrcMap, mainWindowDC,
			&g_boardPSrcRect, &g_boardPQDestRect, srcCopy, nil);
	ReleaseMainWindowDC(g_mainWindow, mainWindowDC);
}

//--------------------------------------------------------------  QuickBatteryRefresh

void QuickBatteryRefresh (Boolean flash)
{
	HDC mainWindowDC;

	mainWindowDC = GetMainWindowDC(g_mainWindow);
	if ((g_batteryTotal > 0) && (!flash))
	{
		Mac_CopyBits(g_badgeSrcMap, mainWindowDC,
				&g_badgesBadgesRects[kBatteryBadge],
				&g_badgesDestRects[kBatteryBadge],
				srcCopy, nil);
	}
	else if ((g_batteryTotal < 0) && (!flash))
	{
		Mac_CopyBits(g_badgeSrcMap, mainWindowDC,
				&g_badgesBadgesRects[kHeliumBadge],
				&g_badgesDestRects[kHeliumBadge],
				srcCopy, nil);
	}
	else
	{
		Mac_CopyBits(g_badgeSrcMap, mainWindowDC,
				&g_badgesBlankRects[kBatteryBadge],
				&g_badgesDestRects[kBatteryBadge],
				srcCopy, nil);
	}
	ReleaseMainWindowDC(g_mainWindow, mainWindowDC);
}

//--------------------------------------------------------------  QuickBandsRefresh

void QuickBandsRefresh (Boolean flash)
{
	HDC mainWindowDC;

	mainWindowDC = GetMainWindowDC(g_mainWindow);
	if ((g_bandsTotal > 0) && (!flash))
	{
		Mac_CopyBits(g_badgeSrcMap, mainWindowDC,
				&g_badgesBadgesRects[kBandsBadge],
				&g_badgesDestRects[kBandsBadge],
				srcCopy, nil);
	}
	else
	{
		Mac_CopyBits(g_badgeSrcMap, mainWindowDC,
				&g_badgesBlankRects[kBandsBadge],
				&g_badgesDestRects[kBandsBadge],
				srcCopy, nil);
	}
	ReleaseMainWindowDC(g_mainWindow, mainWindowDC);
}

//--------------------------------------------------------------  QuickFoilRefresh

void QuickFoilRefresh (Boolean flash)
{
	HDC mainWindowDC;

	mainWindowDC = GetMainWindowDC(g_mainWindow);
	if ((g_foilTotal > 0) && (!flash))
	{
		Mac_CopyBits(g_badgeSrcMap, mainWindowDC,
				&g_badgesBadgesRects[kFoilBadge],
				&g_badgesDestRects[kFoilBadge],
				srcCopy, nil);
	}
	else
	{
		Mac_CopyBits(g_badgeSrcMap, mainWindowDC,
				&g_badgesBlankRects[kFoilBadge],
				&g_badgesDestRects[kFoilBadge],
				srcCopy, nil);
	}
	ReleaseMainWindowDC(g_mainWindow, mainWindowDC);
}

//--------------------------------------------------------------  AdjustScoreboardHeight

void AdjustScoreboardHeight (void)
{
	if (g_numNeighbors == 9)
	{
		SetScoreboardMode(kScoreboardHigh);
	}
	else
	{
		SetScoreboardMode(kScoreboardLow);
	}
}

//--------------------------------------------------------------  GetScoreboardMode

SInt16 GetScoreboardMode (void)
{
	return g_wasScoreboardMode;
}

//--------------------------------------------------------------  SetScoreboardMode

SInt16 SetScoreboardMode (SInt16 newMode)
{
	SInt16 wasMode;
	SInt16 offset;

	wasMode = g_wasScoreboardMode;

	if (g_wasScoreboardMode != newMode)
	{
		offset = 0;
		switch (newMode)
		{
		case kScoreboardHigh:  // 9 neighbors
			offset = g_localRoomsDest[kCentralRoom].top;
			offset = -offset;
			g_justRoomsRect = g_workSrcRect;
			break;

		case kScoreboardLow:  // 1 or 3 neighbors
			offset = g_localRoomsDest[kCentralRoom].top;
			g_justRoomsRect = g_workSrcRect;
			g_justRoomsRect.top = g_localRoomsDest[kCentralRoom].top;
			g_justRoomsRect.bottom = g_localRoomsDest[kCentralRoom].bottom;
			break;
		}

		QOffsetRect(&g_boardDestRect, 0, offset);
		QOffsetRect(&g_boardGQDestRect, 0, offset);
		QOffsetRect(&g_boardPQDestRect, 0, offset);
		QOffsetRect(&g_badgesDestRects[kBatteryBadge], 0, offset);
		QOffsetRect(&g_badgesDestRects[kBandsBadge], 0, offset);
		QOffsetRect(&g_badgesDestRects[kFoilBadge], 0, offset);
		QOffsetRect(&g_badgesDestRects[kHeliumBadge], 0, offset);

		g_wasScoreboardMode = newMode;
	}

	return wasMode;
}
