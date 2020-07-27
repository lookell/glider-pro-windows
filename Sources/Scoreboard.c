#include "Scoreboard.h"

//============================================================================
//----------------------------------------------------------------------------
//                                Scoreboard.c
//----------------------------------------------------------------------------
//============================================================================


#include "ColorUtils.h"
#include "Interactions.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "Play.h"
#include "Player.h"
#include "RectUtils.h"
#include "Room.h"
#include "RoomGraphics.h"
#include "Sound.h"
#include "StringUtils.h"


#define kGrayBackgroundColor    251
#define kScoreRollAmount        13


void RefreshRoomTitle (SInt16 mode);
void RefreshNumGliders (void);
void RefreshPoints (void);


Rect		boardSrcRect, badgeSrcRect, boardDestRect;
HDC			boardSrcMap, badgeSrcMap;
Rect		boardTSrcRect, boardTDestRect;
HDC			boardTSrcMap;
Rect		boardGSrcRect, boardGDestRect;
HDC			boardGSrcMap;
Rect		boardPSrcRect, boardPDestRect;
Rect		boardPQDestRect, boardGQDestRect;
Rect		badgesBlankRects[kNumBadges], badgesBadgesRects[kNumBadges];
Rect		badgesDestRects[kNumBadges];
HDC			boardPSrcMap;
SInt32		displayedScore;
SInt16		wasScoreboardMode;
Boolean		doRollScore;


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
			&boardSrcRect, &boardDestRect, srcCopy, 0L);
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
	COLORREF	wasColor;
	Str255		titleString;

	ColorRect(boardTSrcMap, &boardTSrcRect, kGrayBackgroundColor);

	switch (mode)
	{
		case kEscapedTitleMode:
		PasStringCopyC("Hit Delete key if unable to Follow", titleString);
		break;

		case kSavingTitleMode:
		PasStringCopyC("Saving Game\xC9", titleString); // "Saving Game…"
		break;

		default:
		PasStringCopy(thisRoom->name, titleString);
		break;
	}

	MoveToEx(boardTSrcMap, 1, 10, NULL);
	wasColor = SetTextColor(boardTSrcMap, blackColor);
	Mac_DrawString(boardTSrcMap, titleString);

	MoveToEx(boardTSrcMap, 0, 9, NULL);
	SetTextColor(boardTSrcMap, whiteColor);
	Mac_DrawString(boardTSrcMap, titleString);

	SetTextColor(boardTSrcMap, wasColor);

	Mac_CopyBits(boardTSrcMap, boardSrcMap,
			&boardTSrcRect, &boardTDestRect, srcCopy, nil);
}

//--------------------------------------------------------------  RefreshNumGliders

void RefreshNumGliders (void)
{
	COLORREF	wasColor;
	Str255		nGlidersStr;
	SInt32		displayMortals;

	ColorRect(boardGSrcMap, &boardGSrcRect, kGrayBackgroundColor);

	displayMortals = mortals;
	if (displayMortals < 0)
		displayMortals = 0;
	NumToString(displayMortals, nGlidersStr);

	MoveToEx(boardGSrcMap, 1, 10, NULL);
	wasColor = SetTextColor(boardGSrcMap, blackColor);
	Mac_DrawString(boardGSrcMap, nGlidersStr);

	MoveToEx(boardGSrcMap, 0, 9, NULL);
	SetTextColor(boardGSrcMap, whiteColor);
	Mac_DrawString(boardGSrcMap, nGlidersStr);

	SetTextColor(boardGSrcMap, wasColor);

	Mac_CopyBits(boardGSrcMap, boardSrcMap,
			&boardGSrcRect, &boardGDestRect, srcCopy, nil);
}

//--------------------------------------------------------------  RefreshPoints

void RefreshPoints (void)
{
	COLORREF	wasColor;
	Str255		scoreStr;

	ColorRect(boardPSrcMap, &boardPSrcRect, kGrayBackgroundColor);

	NumToString(theScore, scoreStr);

	MoveToEx(boardPSrcMap, 1, 10, NULL);
	wasColor = SetTextColor(boardPSrcMap, blackColor);
	Mac_DrawString(boardPSrcMap, scoreStr);

	MoveToEx(boardPSrcMap, 0, 9, NULL);
	SetTextColor(boardPSrcMap, whiteColor);
	Mac_DrawString(boardPSrcMap, scoreStr);

	SetTextColor(boardPSrcMap, wasColor);

	Mac_CopyBits(boardPSrcMap, boardSrcMap,
			&boardPSrcRect, &boardPDestRect, srcCopy, nil);

	displayedScore = theScore;
}

//--------------------------------------------------------------  QuickGlidersRefresh

void QuickGlidersRefresh (void)
{
	COLORREF	wasColor;
	Str255		nGlidersStr;
	HDC			mainWindowDC;

	ColorRect(boardGSrcMap, &boardGSrcRect, kGrayBackgroundColor);

	NumToString((SInt32)mortals, nGlidersStr);

	MoveToEx(boardGSrcMap, 1, 10, NULL);
	wasColor = SetTextColor(boardGSrcMap, blackColor);
	Mac_DrawString(boardGSrcMap, nGlidersStr);

	MoveToEx(boardGSrcMap, 0, 9, NULL);
	SetTextColor(boardGSrcMap, whiteColor);
	Mac_DrawString(boardGSrcMap, nGlidersStr);

	SetTextColor(boardGSrcMap, wasColor);

	mainWindowDC = GetMainWindowDC();
	Mac_CopyBits(boardGSrcMap, mainWindowDC,
			&boardGSrcRect, &boardGQDestRect, srcCopy, nil);
	ReleaseMainWindowDC(mainWindowDC);
}

//--------------------------------------------------------------  QuickScoreRefresh

void QuickScoreRefresh (void)
{
	COLORREF	wasColor;
	Str255		scoreStr;
	HDC			mainWindowDC;

	ColorRect(boardPSrcMap, &boardPSrcRect, kGrayBackgroundColor);

	NumToString(displayedScore, scoreStr);

	MoveToEx(boardPSrcMap, 1, 10, NULL);
	wasColor = SetTextColor(boardPSrcMap, blackColor);
	Mac_DrawString(boardPSrcMap, scoreStr);

	MoveToEx(boardPSrcMap, 0, 9, NULL);
	SetTextColor(boardPSrcMap, whiteColor);
	Mac_DrawString(boardPSrcMap, scoreStr);

	SetTextColor(boardPSrcMap, wasColor);

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

