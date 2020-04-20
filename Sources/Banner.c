//============================================================================
//----------------------------------------------------------------------------
//									Banner.c
//----------------------------------------------------------------------------
//============================================================================


#define _CRT_SECURE_NO_WARNINGS
//#include <NumberFormatting.h>
#include "Macintosh.h"
#include "Externs.h"
#include "Environ.h"
#include "MainWindow.h"
#include "RectUtils.h"
#include "Room.h"
#include "Utilities.h"


#define kBannerPageTopPICT		1993
#define kBannerPageBottomPICT	1992
#define kBannerPageBottomMask	1991
#define kStarsRemainingPICT		1017
#define kStarRemainingPICT		1018


void DrawBanner (Point *);
void DrawBannerMessage (Point);


SInt16		numStarsRemaining;
Boolean		bannerStarCountOn;

extern	Rect		justRoomsRect;
extern	Boolean		quickerTransitions, demoGoing, isUseSecondScreen;


//==============================================================  Functions
//--------------------------------------------------------------  DrawBanner
// Displays opening banner (when a new game is begun).  The banner looks…
// like a sheet of notebook paper.  The text printed on it is specified…
// by the author of the house.

void DrawBanner (Point *topLeft)
{
	Rect		wholePage, partPage, mapBounds;
	HDC			tempMap;
	HDC			tempMask;
	OSErr		theErr;

	QSetRect(&wholePage, 0, 0, 330, 220);
	mapBounds = thisMac.screen;
	ZeroRectCorner(&mapBounds);
	CenterRectInRect(&wholePage, &mapBounds);
	topLeft->h = wholePage.left;
	topLeft->v = wholePage.top;
	partPage = wholePage;
	partPage.bottom = partPage.top + 190;
	LoadScaledGraphic(workSrcMap, kBannerPageTopPICT, &partPage);

	partPage = wholePage;
	partPage.top = partPage.bottom - 30;
	mapBounds = partPage;
	ZeroRectCorner(&mapBounds);
	theErr = CreateOffScreenGWorld(&tempMap, &mapBounds, kPreferredDepth);
	LoadGraphic(tempMap, kBannerPageBottomPICT);

	theErr = CreateOffScreenGWorld(&tempMask, &mapBounds, 1);
	LoadGraphic(tempMask, kBannerPageBottomMask);

	Mac_CopyMask(tempMap, tempMask, workSrcMap,
			&mapBounds, &mapBounds, &partPage);

	DisposeGWorld(tempMap);
	DisposeGWorld(tempMask);
}

//--------------------------------------------------------------  CountStarsInHouse
// Goes through the current house and counts the total number of stars within.

SInt16 CountStarsInHouse (void)
{
	SInt16		i, h, numRooms, numStars;

	numStars = 0;

	numRooms = thisHouse->nRooms;
	for (i = 0; i < numRooms; i++)
	{
		if (thisHouse->rooms[i].suite != kRoomIsEmpty)
			for (h = 0; h < kMaxRoomObs; h++)
			{
				if (thisHouse->rooms[i].objects[h].what == kStar)
					numStars++;
			}
	}

	return (numStars);
}

//--------------------------------------------------------------  DrawBannerMessage

// This function prints the author's message acorss the notebook paper banner.

void DrawBannerMessage (Point topLeft)
{
	Str255		bannerStr, subStr;
	SInt16		count;
	LOGFONT		lfBanner;
	HFONT		bannerFont;

	lfBanner.lfHeight = -12;
	lfBanner.lfWidth = 0;
	lfBanner.lfEscapement = 0;
	lfBanner.lfOrientation = 0;
	lfBanner.lfWeight = FW_BOLD;
	lfBanner.lfItalic = FALSE;
	lfBanner.lfUnderline = FALSE;
	lfBanner.lfStrikeOut = FALSE;
	lfBanner.lfCharSet = DEFAULT_CHARSET;
	lfBanner.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lfBanner.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lfBanner.lfQuality = DEFAULT_QUALITY;
	lfBanner.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	wcscpy(lfBanner.lfFaceName, L"Tahoma");

	PasStringCopy(thisHouse->banner, bannerStr);

	bannerFont = CreateFontIndirect(&lfBanner);
	SaveDC(workSrcMap);
	SelectObject(workSrcMap, bannerFont);
	SetTextColor(workSrcMap, blackColor);
	count = 0;
	do
	{
		GetLineOfText(bannerStr, count, subStr);
		MoveToEx(workSrcMap, topLeft.h + 16, topLeft.v + 32 + (count * 20), NULL);
		Mac_DrawString(workSrcMap, subStr);
		count++;
	}
	while (subStr[0] > 0);

	if (bannerStarCountOn)
	{
		if (numStarsRemaining != 1)
			GetLocalizedString(1, bannerStr);
		else
			GetLocalizedString(2, bannerStr);

		Mac_NumToString((SInt32)numStarsRemaining, subStr);
		PasStringConcat(bannerStr, subStr);

		if (numStarsRemaining != 1)
			GetLocalizedString(3, subStr);
		else
			GetLocalizedString(4, subStr);
		PasStringConcat(bannerStr, subStr);

		SetTextColor(workSrcMap, redColor);
		MoveToEx(workSrcMap, topLeft.h + 16, topLeft.v + 164, NULL);
		Mac_DrawString(workSrcMap, bannerStr);
		MoveToEx(workSrcMap, topLeft.h + 16, topLeft.v + 180, NULL);
		GetLocalizedString(5, subStr);
		Mac_DrawString(workSrcMap, subStr);
	}
	RestoreDC(workSrcMap, -1);
	DeleteObject(bannerFont);
}

//--------------------------------------------------------------  BringUpBanner
// Handles bringing up displaying and disposing of the banner.

void BringUpBanner (void)
{
	Rect		wholePage;
	Point		topLeft;

	DrawBanner(&topLeft);
	DrawBannerMessage(topLeft);
	if (quickerTransitions)
		DissBitsChunky(&justRoomsRect);		// was workSrcRect
	else
		DissBits(&justRoomsRect);
	QSetRect(&wholePage, 0, 0, 330, 220);
	QOffsetRect(&wholePage, topLeft.h, topLeft.v);

	Mac_CopyBits(backSrcMap, workSrcMap,
			&wholePage, &wholePage, srcCopy, nil);

	if (demoGoing)
		WaitForInputEvent(4);
	else
		WaitForInputEvent(15);

	if (quickerTransitions)
		DissBitsChunky(&justRoomsRect);
	else
		DissBits(&justRoomsRect);
}

//--------------------------------------------------------------  DisplayStarsRemaining
// This brings up a small message indicating the number of stars remaining…
// in a house.  It comes up when the player gets a star (the game is paused…
// and the user informed as to how many remain).

void DisplayStarsRemaining (void)
{
	return;
#if 0
	Rect		src, bounds;
	Str255		theStr;

	SetPortWindowPort(mainWindow);
	QSetRect(&bounds, 0, 0, 256, 64);
	CenterRectInRect(&bounds, &thisMac.screen);
	QOffsetRect(&bounds, -thisMac.screen.left, -thisMac.screen.top);
	src = bounds;
	InsetRect(&src, 64, 32);

	TextFont(applFont);
	TextFace(bold);
	TextSize(12);
	NumToString((long)numStarsRemaining, theStr);

	QOffsetRect(&bounds, 0, -20);
	if (numStarsRemaining < 2)
		LoadScaledGraphic(kStarRemainingPICT, &bounds);
	else
	{
		LoadScaledGraphic(kStarsRemainingPICT, &bounds);
		MoveTo(bounds.left + 102 - (StringWidth(theStr) / 2), bounds.top + 23);
		ColorText(theStr, 4L);
	}

	DelayTicks(60);
	if (WaitForInputEvent(30))
		RestoreEntireGameScreen();
	CopyRectWorkToMain(&bounds);
#endif
}

