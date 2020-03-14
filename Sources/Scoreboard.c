
//============================================================================
//----------------------------------------------------------------------------
//								   Scoreboard.c
//----------------------------------------------------------------------------
//============================================================================


//#include <NumberFormatting.h>
#include "Macintosh.h"
#include "Externs.h"
#include "Environ.h"
#include "RectUtils.h"


#define kGrayBackgroundColor	251
#define kGrayBackgroundColor4	10
#define kFoilBadge				0
#define kBandsBadge				1
#define kBatteryBadge			2
#define kHeliumBadge			3
#define kScoreRollAmount		13


void RefreshRoomTitle (SInt16);
void RefreshNumGliders (void);
void RefreshPoints (void);


Rect		boardSrcRect, badgeSrcRect, boardDestRect;
GWorldPtr	boardSrcMap, badgeSrcMap;
Rect		boardTSrcRect, boardTDestRect;
GWorldPtr	boardTSrcMap;
Rect		boardGSrcRect, boardGDestRect;
GWorldPtr	boardGSrcMap;
Rect		boardPSrcRect, boardPDestRect;
Rect		boardPQDestRect, boardGQDestRect;
Rect		badgesBlankRects[4], badgesBadgesRects[4];
Rect		badgesDestRects[4];
GWorldPtr	boardPSrcMap;
long		displayedScore;
short		wasScoreboardMode;
Boolean		doRollScore;

extern	Rect		localRoomsDest[], justRoomsRect;
extern	long		gameFrame;
extern	short		numNeighbors, otherPlayerEscaped;
extern	Boolean		evenFrame, onePlayerLeft;


//==============================================================  Functions
//--------------------------------------------------------------  RefreshScoreboard

void RefreshScoreboard (SInt16 mode)
{
	return;
#if 0
	doRollScore = true;

	RefreshRoomTitle(mode);
	RefreshNumGliders();
	RefreshPoints();

	CopyBits((BitMap *)*GetGWorldPixMap(boardSrcMap),
			GetPortBitMapForCopyBits(GetWindowPort(mainWindow)),
			&boardSrcRect, &boardDestRect, srcCopy, 0L);

	QuickBatteryRefresh(false);
	QuickBandsRefresh(false);
	QuickFoilRefresh(false);
#endif
}

//--------------------------------------------------------------  HandleDynamicScoreboard

 void HandleDynamicScoreboard (void)
 {
	return;
#if 0
 	#define		kFoilLow		2		// 25%
 	#define		kBatteryLow		17		// 25%
 	#define		kHeliumLow		-38		// 25%
 	#define		kBandsLow		2		// 25%
 	long		whosTurn;

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
#endif
 }

//--------------------------------------------------------------  RefreshRoomTitle

void RefreshRoomTitle (SInt16 mode)
{
	return;
#if 0
	RGBColor	theRGBColor, wasColor;

	SetPort((GrafPtr)boardTSrcMap);

	GetForeColor(&wasColor);
	if (thisMac.isDepth == 4)
		Index2Color(kGrayBackgroundColor4, &theRGBColor);
	else
		Index2Color(kGrayBackgroundColor, &theRGBColor);
	RGBForeColor(&theRGBColor);
	PaintRect(&boardTSrcRect);
	RGBForeColor(&wasColor);

	MoveTo(1, 10);
	ForeColor(blackColor);
	switch (mode)
	{
		case kEscapedTitleMode:
		DrawString("\pHit Delete key if unable to Follow");
		break;

		case kSavingTitleMode:
		DrawString("\pSaving Game…");
		break;

		default:
		DrawString(thisRoom->name);
		break;
	}
	MoveTo(0, 9);
	ForeColor(whiteColor);
	switch (mode)
	{
		case kEscapedTitleMode:
		DrawString("\pHit Delete key if unable to Follow");
		break;

		case kSavingTitleMode:
		DrawString("\pSaving Game…");
		break;

		default:
		DrawString(thisRoom->name);
		break;
	}
	ForeColor(blackColor);

	CopyBits((BitMap *)*GetGWorldPixMap(boardTSrcMap),
			(BitMap *)*GetGWorldPixMap(boardSrcMap),
			&boardTSrcRect, &boardTDestRect, srcCopy, nil);
#endif
}

//--------------------------------------------------------------  RefreshNumGliders

void RefreshNumGliders (void)
{
	return;
#if 0
	RGBColor	theRGBColor, wasColor;
	Str255		nGlidersStr;
	long		displayMortals;

	SetPort((GrafPtr)boardGSrcMap);

	GetForeColor(&wasColor);
	if (thisMac.isDepth == 4)
		Index2Color(kGrayBackgroundColor4, &theRGBColor);
	else
		Index2Color(kGrayBackgroundColor, &theRGBColor);
	RGBForeColor(&theRGBColor);
	PaintRect(&boardGSrcRect);
	RGBForeColor(&wasColor);

	displayMortals = mortals;
	if (displayMortals < 0)
		displayMortals = 0;
	NumToString(displayMortals, nGlidersStr);

	MoveTo(1, 10);
	ForeColor(blackColor);
	DrawString(nGlidersStr);

	MoveTo(0, 9);
	ForeColor(whiteColor);
	DrawString(nGlidersStr);

	ForeColor(blackColor);

	CopyBits((BitMap *)*GetGWorldPixMap(boardGSrcMap),
			(BitMap *)*GetGWorldPixMap(boardSrcMap),
			&boardGSrcRect, &boardGDestRect, srcCopy, nil);
#endif
}

//--------------------------------------------------------------  RefreshPoints

void RefreshPoints (void)
{
	return;
#if 0
	RGBColor	theRGBColor, wasColor;
	Str255		scoreStr;

	SetPort((GrafPtr)boardPSrcMap);

	GetForeColor(&wasColor);
	if (thisMac.isDepth == 4)
		Index2Color(kGrayBackgroundColor4, &theRGBColor);
	else
		Index2Color(kGrayBackgroundColor, &theRGBColor);
	RGBForeColor(&theRGBColor);
	PaintRect(&boardPSrcRect);
	RGBForeColor(&wasColor);

	NumToString(theScore, scoreStr);

	MoveTo(1, 10);
	ForeColor(blackColor);
	DrawString(scoreStr);

	MoveTo(0, 9);
	ForeColor(whiteColor);
	DrawString(scoreStr);

	ForeColor(blackColor);

	CopyBits((BitMap *)*GetGWorldPixMap(boardPSrcMap),
			(BitMap *)*GetGWorldPixMap(boardSrcMap),
			&boardPSrcRect, &boardPDestRect, srcCopy, nil);

	displayedScore = theScore;
#endif
}

//--------------------------------------------------------------  QuickGlidersRefresh

void QuickGlidersRefresh (void)
{
	return;
#if 0
	RGBColor	theRGBColor, wasColor;
	Str255		nGlidersStr;

	SetPort((GrafPtr)boardGSrcMap);

	GetForeColor(&wasColor);
	if (thisMac.isDepth == 4)
		Index2Color(kGrayBackgroundColor4, &theRGBColor);
	else
		Index2Color(kGrayBackgroundColor, &theRGBColor);
	RGBForeColor(&theRGBColor);
	PaintRect(&boardGSrcRect);
	RGBForeColor(&wasColor);

	NumToString((long)mortals, nGlidersStr);

	MoveTo(1, 10);
	ForeColor(blackColor);
	DrawString(nGlidersStr);

	MoveTo(0, 9);
	ForeColor(whiteColor);
	DrawString(nGlidersStr);

	ForeColor(blackColor);

	CopyBits((BitMap *)*GetGWorldPixMap(boardGSrcMap),
			GetPortBitMapForCopyBits(GetWindowPort(mainWindow)),
			&boardGSrcRect, &boardGQDestRect, srcCopy, nil);
#endif
}

//--------------------------------------------------------------  QuickScoreRefresh

void QuickScoreRefresh (void)
{
	return;
#if 0
	RGBColor	theRGBColor, wasColor;
	Str255		scoreStr;

	SetPort((GrafPtr)boardPSrcMap);

	GetForeColor(&wasColor);
	if (thisMac.isDepth == 4)
		Index2Color(kGrayBackgroundColor4, &theRGBColor);
	else
		Index2Color(kGrayBackgroundColor, &theRGBColor);
	RGBForeColor(&theRGBColor);
	PaintRect(&boardPSrcRect);
	RGBForeColor(&wasColor);

	NumToString(displayedScore, scoreStr);

	MoveTo(1, 10);
	ForeColor(blackColor);
	DrawString(scoreStr);

	MoveTo(0, 9);
	ForeColor(whiteColor);
	DrawString(scoreStr);

	ForeColor(blackColor);

	CopyBits((BitMap *)*GetGWorldPixMap(boardPSrcMap),
			GetPortBitMapForCopyBits(GetWindowPort(mainWindow)),
			&boardPSrcRect, &boardPQDestRect, srcCopy, nil);
#endif
}

//--------------------------------------------------------------  QuickBatteryRefresh

void QuickBatteryRefresh (Boolean flash)
{
	return;
#if 0
	if ((batteryTotal > 0) && (!flash))
	{
		CopyBits((BitMap *)*GetGWorldPixMap(badgeSrcMap),
				GetPortBitMapForCopyBits(GetWindowPort(mainWindow)),
				&badgesBadgesRects[kBatteryBadge],
				&badgesDestRects[kBatteryBadge],
				srcCopy, nil);
	}
	else if ((batteryTotal < 0) && (!flash))
	{
		CopyBits((BitMap *)*GetGWorldPixMap(badgeSrcMap),
				GetPortBitMapForCopyBits(GetWindowPort(mainWindow)),
				&badgesBadgesRects[kHeliumBadge],
				&badgesDestRects[kHeliumBadge],
				srcCopy, nil);
	}
	else
	{
		CopyBits((BitMap *)*GetGWorldPixMap(badgeSrcMap),
				GetPortBitMapForCopyBits(GetWindowPort(mainWindow)),
				&badgesBlankRects[kBatteryBadge],
				&badgesDestRects[kBatteryBadge],
				srcCopy, nil);
	}
#endif
}

//--------------------------------------------------------------  QuickBandsRefresh

void QuickBandsRefresh (Boolean flash)
{
	return;
#if 0
	if ((bandsTotal > 0) && (!flash))
	{
		CopyBits((BitMap *)*GetGWorldPixMap(badgeSrcMap),
				GetPortBitMapForCopyBits(GetWindowPort(mainWindow)),
				&badgesBadgesRects[kBandsBadge],
				&badgesDestRects[kBandsBadge],
				srcCopy, nil);
	}
	else
	{
		CopyBits((BitMap *)*GetGWorldPixMap(badgeSrcMap),
				GetPortBitMapForCopyBits(GetWindowPort(mainWindow)),
				&badgesBlankRects[kBandsBadge],
				&badgesDestRects[kBandsBadge],
				srcCopy, nil);
	}
#endif
}

//--------------------------------------------------------------  QuickFoilRefresh

void QuickFoilRefresh (Boolean flash)
{
	return;
#if 0
	if ((foilTotal > 0) && (!flash))
	{
		CopyBits((BitMap *)*GetGWorldPixMap(badgeSrcMap),
				GetPortBitMapForCopyBits(GetWindowPort(mainWindow)),
				&badgesBadgesRects[kFoilBadge],
				&badgesDestRects[kFoilBadge],
				srcCopy, nil);
	}
	else
	{
		CopyBits((BitMap *)*GetGWorldPixMap(badgeSrcMap),
				GetPortBitMapForCopyBits(GetWindowPort(mainWindow)),
				&badgesBlankRects[kFoilBadge],
				&badgesDestRects[kFoilBadge],
				srcCopy, nil);
	}
#endif
}

//--------------------------------------------------------------  AdjustScoreboardHeight

void AdjustScoreboardHeight (void)
{
	return;
#if 0
	short		offset, newMode;

	if (numNeighbors == 9)
		newMode = kScoreboardHigh;
	else
		newMode = kScoreboardLow;

	if (wasScoreboardMode != newMode)
	{
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
#endif
}

//--------------------------------------------------------------  BlackenScoreboard

void BlackenScoreboard (void)
{
	return;
#if 0
	UpdateMenuBarWindow();
#endif
}

