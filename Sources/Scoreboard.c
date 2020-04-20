
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
HDC			boardSrcMap, badgeSrcMap;
Rect		boardTSrcRect, boardTDestRect;
HDC			boardTSrcMap;
Rect		boardGSrcRect, boardGDestRect;
HDC			boardGSrcMap;
Rect		boardPSrcRect, boardPDestRect;
Rect		boardPQDestRect, boardGQDestRect;
Rect		badgesBlankRects[4], badgesBadgesRects[4];
Rect		badgesDestRects[4];
HDC			boardPSrcMap;
SInt32		displayedScore;
SInt16		wasScoreboardMode;
Boolean		doRollScore;

extern	Rect		localRoomsDest[], justRoomsRect;
extern	SInt32		gameFrame;
extern	SInt16		numNeighbors, otherPlayerEscaped;
extern	Boolean		evenFrame, onePlayerLeft;


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
	COLORREF	theRGBColor, wasColor;
	Str255		titleString;

	//SetPort((GrafPtr)boardTSrcMap);

	if (thisMac.isDepth == 4)
		theRGBColor = Index2ColorRef(kGrayBackgroundColor4);
	else
		theRGBColor = Index2ColorRef(kGrayBackgroundColor);
	wasColor = SetDCBrushColor(boardTSrcMap, theRGBColor);
	Mac_PaintRect(boardTSrcMap, &boardTSrcRect, GetStockObject(DC_BRUSH));
	SetDCBrushColor(boardTSrcMap, wasColor);

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
	COLORREF	theRGBColor, wasColor;
	Str255		nGlidersStr;
	SInt32		displayMortals;

	//SetPort((GrafPtr)boardGSrcMap);

	if (thisMac.isDepth == 4)
		theRGBColor = Index2ColorRef(kGrayBackgroundColor4);
	else
		theRGBColor = Index2ColorRef(kGrayBackgroundColor);
	wasColor = SetDCBrushColor(boardGSrcMap, theRGBColor);
	Mac_PaintRect(boardGSrcMap, &boardGSrcRect, GetStockObject(DC_BRUSH));
	SetDCBrushColor(boardGSrcMap, wasColor);

	displayMortals = mortals;
	if (displayMortals < 0)
		displayMortals = 0;
	Mac_NumToString(displayMortals, nGlidersStr);

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
	COLORREF	theRGBColor, wasColor;
	Str255		scoreStr;

	//SetPort((GrafPtr)boardPSrcMap);

	if (thisMac.isDepth == 4)
		theRGBColor = Index2ColorRef(kGrayBackgroundColor4);
	else
		theRGBColor = Index2ColorRef(kGrayBackgroundColor);
	wasColor = SetDCBrushColor(boardPSrcMap, theRGBColor);
	Mac_PaintRect(boardPSrcMap, &boardPSrcRect, GetStockObject(DC_BRUSH));
	SetDCBrushColor(boardPSrcMap, wasColor);

	Mac_NumToString(theScore, scoreStr);

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
	COLORREF	theRGBColor, wasColor;
	Str255		nGlidersStr;
	HDC			mainWindowDC;

	//SetPort((GrafPtr)boardGSrcMap);

	if (thisMac.isDepth == 4)
		theRGBColor = Index2ColorRef(kGrayBackgroundColor4);
	else
		theRGBColor = Index2ColorRef(kGrayBackgroundColor);
	wasColor = SetDCBrushColor(boardGSrcMap, theRGBColor);
	Mac_PaintRect(boardGSrcMap, &boardGSrcRect, GetStockObject(DC_BRUSH));
	SetDCBrushColor(boardGSrcMap, wasColor);

	Mac_NumToString((SInt32)mortals, nGlidersStr);

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

//--------------------------------------------------------------  BlackenScoreboard

void BlackenScoreboard (void)
{
	UpdateMenuBarWindow();
}

