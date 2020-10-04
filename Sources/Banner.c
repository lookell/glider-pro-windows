#define GP_USE_WINAPI_H

#include "Banner.h"

//============================================================================
//----------------------------------------------------------------------------
//                                  Banner.c
//----------------------------------------------------------------------------
//============================================================================


#include "ColorUtils.h"
#include "Environ.h"
#include "House.h"
#include "Macintosh.h"
#include "Main.h"
#include "MainWindow.h"
#include "Play.h"
#include "RectUtils.h"
#include "Render.h"
#include "ResourceIDs.h"
#include "Room.h"
#include "StringUtils.h"
#include "Transitions.h"
#include "Utilities.h"
#include "WinAPI.h"

#include <strsafe.h>


void DrawBanner (Point *topLeft);
void DrawBannerMessage (Point topLeft);


SInt16 numStarsRemaining;
Boolean bannerStarCountOn;


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

	numRooms = thisHouse.nRooms;
	for (i = 0; i < numRooms; i++)
	{
		if (thisHouse.rooms[i].suite != kRoomIsEmpty)
			for (h = 0; h < kMaxRoomObs; h++)
			{
				if (thisHouse.rooms[i].objects[h].what == kStar)
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
	HFONT		bannerFont;

	PasStringCopy(thisHouse.banner, bannerStr);

	bannerFont = CreateTahomaFont(-12, FW_BOLD);
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
			GetLocalizedString_Pascal(1, bannerStr);
		else
			GetLocalizedString_Pascal(2, bannerStr);

		NumToString((SInt32)numStarsRemaining, subStr);
		PasStringConcat(bannerStr, subStr);

		if (numStarsRemaining != 1)
			GetLocalizedString_Pascal(3, subStr);
		else
			GetLocalizedString_Pascal(4, subStr);
		PasStringConcat(bannerStr, subStr);

		SetTextColor(workSrcMap, redColor);
		MoveToEx(workSrcMap, topLeft.h + 16, topLeft.v + 164, NULL);
		Mac_DrawString(workSrcMap, bannerStr);
		MoveToEx(workSrcMap, topLeft.h + 16, topLeft.v + 180, NULL);
		GetLocalizedString_Pascal(5, subStr);
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
	Rect		src, bounds;
	WCHAR		theStr[256];
	HDC			mainWindowDC;
	HFONT		textFont;

	QSetRect(&bounds, 0, 0, 256, 64);
	CenterRectInRect(&bounds, &thisMac.screen);
	QOffsetRect(&bounds, -thisMac.screen.left, -thisMac.screen.top);
	src = bounds;
	QInsetRect(&src, 64, 32);

	StringCchPrintf(theStr, ARRAYSIZE(theStr), L"%d", (int)numStarsRemaining);

	mainWindowDC = GetMainWindowDC();
	QOffsetRect(&bounds, 0, -20);
	if (numStarsRemaining < 2)
		LoadScaledGraphic(mainWindowDC, kStarRemainingPICT, &bounds);
	else
	{
		SaveDC(mainWindowDC);
		LoadScaledGraphic(mainWindowDC, kStarsRemainingPICT, &bounds);
		textFont = CreateTahomaFont(-12, FW_BOLD);
		SelectObject(mainWindowDC, textFont);
		SetTextAlign(mainWindowDC, TA_BASELINE | TA_CENTER);
		SetTextColor(mainWindowDC, Index2ColorRef(4L));
		SetBkMode(mainWindowDC, TRANSPARENT);
		TextOut(mainWindowDC, bounds.left + 102, bounds.top + 23, theStr, (int)wcslen(theStr));
		RestoreDC(mainWindowDC, -1);
		DeleteObject(textFont);
	}
	ReleaseMainWindowDC(mainWindowDC);

	DelayTicks(60);
	if (WaitForInputEvent(30))
		RestoreEntireGameScreen();
	CopyRectWorkToMain(&bounds);
}

