//============================================================================
//----------------------------------------------------------------------------
//                                  Banner.c
//----------------------------------------------------------------------------
//============================================================================

#include "Banner.h"

#include "ColorUtils.h"
#include "House.h"
#include "HouseIO.h"
#include "Macintosh.h"
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

SInt16 g_numStarsRemaining;
Boolean g_bannerStarCountOn;

//==============================================================  Functions
//--------------------------------------------------------------  DrawBanner
// Displays opening banner (when a new game is begun).  The banner looks
// like a sheet of notebook paper.  The text printed on it is specified
// by the author of the house.

void DrawBanner (Point *topLeft)
{
	Rect wholePage, partPage, mapBounds;
	HDC tempMap;
	HDC tempMask;

	QSetRect(&wholePage, 0, 0, 330, 220);
	mapBounds = g_workSrcRect;
	ZeroRectCorner(&mapBounds);
	CenterRectInRect(&wholePage, &mapBounds);
	topLeft->h = wholePage.left;
	topLeft->v = wholePage.top;
	partPage = wholePage;
	partPage.bottom = partPage.top + 190;
	LoadScaledGraphic(g_workSrcMap, g_theHouseFile, kBannerPageTopPICT, &partPage);

	partPage = wholePage;
	partPage.top = partPage.bottom - 30;
	mapBounds = partPage;
	ZeroRectCorner(&mapBounds);
	tempMap = CreateOffScreenGWorld(&mapBounds, kPreferredDepth);
	LoadGraphic(tempMap, g_theHouseFile, kBannerPageBottomPICT);

	tempMask = CreateOffScreenGWorld(&mapBounds, 1);
	LoadGraphic(tempMask, g_theHouseFile, kBannerPageBottomMask);

	Mac_CopyMask(tempMap, tempMask, g_workSrcMap,
			&mapBounds, &mapBounds, &partPage);

	DisposeGWorld(tempMap);
	DisposeGWorld(tempMask);
}

//--------------------------------------------------------------  CountStarsInHouse
// Goes through the current house and counts the total number of stars within.

SInt16 CountStarsInHouse (const houseType *house)
{
	SInt16 i, h, numRooms, numStars;

	numStars = 0;

	numRooms = house->nRooms;
	for (i = 0; i < numRooms; i++)
	{
		if (house->rooms[i].suite != kRoomIsEmpty)
		{
			for (h = 0; h < kMaxRoomObs; h++)
			{
				if (house->rooms[i].objects[h].what == kStar)
				{
					numStars++;
				}
			}
		}
	}

	return (numStars);
}

//--------------------------------------------------------------  DrawBannerMessage
// This function prints the author's message acorss the notebook paper banner.

void DrawBannerMessage (Point topLeft)
{
	WCHAR bannerStr[256];
	WCHAR subStr[256];
	size_t lineOffset;
	size_t lineLength;
	SInt16 count;
	HFONT bannerFont;

	WinFromMacString(bannerStr, ARRAYSIZE(bannerStr), g_thisHouse.banner);

	bannerFont = CreateTahomaFont(-12, FW_BOLD);
	SaveDC(g_workSrcMap);
	SelectFont(g_workSrcMap, bannerFont);
	SetBkMode(g_workSrcMap, TRANSPARENT);
	SetTextAlign(g_workSrcMap, TA_LEFT | TA_BASELINE);

	SetTextColor(g_workSrcMap, blackColor);
	count = 0;
	while (GetLineOfText(bannerStr, count, &lineOffset, &lineLength))
	{
		TextOut(g_workSrcMap, topLeft.h + 16, topLeft.v + 32 + (count * 20),
				&bannerStr[lineOffset], (int)lineLength);
		count++;
	}

	if (g_bannerStarCountOn)
	{
		if (g_numStarsRemaining != 1)
			GetLocalizedString(1, bannerStr, ARRAYSIZE(bannerStr));
		else
			GetLocalizedString(2, bannerStr, ARRAYSIZE(bannerStr));

		NumToString(g_numStarsRemaining, subStr, ARRAYSIZE(subStr));
		StringCchCat(bannerStr, ARRAYSIZE(bannerStr), subStr);

		if (g_numStarsRemaining != 1)
			GetLocalizedString(3, subStr, ARRAYSIZE(subStr));
		else
			GetLocalizedString(4, subStr, ARRAYSIZE(subStr));
		StringCchCat(bannerStr, ARRAYSIZE(bannerStr), subStr);

		SetTextColor(g_workSrcMap, redColor);
		TextOut(g_workSrcMap, topLeft.h + 16, topLeft.v + 164,
				bannerStr, (int)wcslen(bannerStr));
		GetLocalizedString(5, subStr, ARRAYSIZE(subStr));
		TextOut(g_workSrcMap, topLeft.h + 16, topLeft.v + 180,
				subStr, (int)wcslen(subStr));
	}
	RestoreDC(g_workSrcMap, -1);
	DeleteFont(bannerFont);
}

//--------------------------------------------------------------  BringUpBanner
// Handles bringing up displaying and disposing of the banner.

void BringUpBanner (void)
{
	Rect wholePage;
	Point topLeft;

	DrawBanner(&topLeft);
	DrawBannerMessage(topLeft);
	DissolveScreenOn(&g_justRoomsRect);  // was g_workSrcRect
	QSetRect(&wholePage, 0, 0, 330, 220);
	QOffsetRect(&wholePage, topLeft.h, topLeft.v);

	Mac_CopyBits(g_backSrcMap, g_workSrcMap,
			&wholePage, &wholePage, srcCopy, nil);

	if (g_demoGoing)
		WaitForInputEvent(4);
	else
		WaitForInputEvent(15);

	DissolveScreenOn(&g_justRoomsRect);
}

//--------------------------------------------------------------  DisplayStarsRemaining
// This brings up a small message indicating the number of stars remaining
// in a house.  It comes up when the player gets a star (the game is paused
// and the user informed as to how many remain).

void DisplayStarsRemaining (void)
{
	Rect src, bounds;
	WCHAR theStr[256];
	HDC mainWindowDC;
	HFONT textFont;

	QSetRect(&bounds, 0, 0, 256, 64);
	CenterRectInRect(&bounds, &g_workSrcRect);
	QOffsetRect(&bounds, -g_workSrcRect.left, -g_workSrcRect.top);
	src = bounds;
	QInsetRect(&src, 64, 32);

	NumToString(g_numStarsRemaining, theStr, ARRAYSIZE(theStr));

	mainWindowDC = GetMainWindowDC(g_mainWindow);
	if (g_numStarsRemaining < 2)
	{
		LoadScaledGraphic(mainWindowDC, g_theHouseFile, kStarRemainingPICT, &bounds);
	}
	else
	{
		SaveDC(mainWindowDC);
		LoadScaledGraphic(mainWindowDC, g_theHouseFile, kStarsRemainingPICT, &bounds);
		textFont = CreateTahomaFont(-12, FW_BOLD);
		SelectFont(mainWindowDC, textFont);
		SetTextAlign(mainWindowDC, TA_CENTER | TA_BASELINE);
		SetTextColor(mainWindowDC, Index2ColorRef(4L));
		SetBkMode(mainWindowDC, TRANSPARENT);
		TextOut(mainWindowDC, bounds.left + 102, bounds.top + 23, theStr, (int)wcslen(theStr));
		RestoreDC(mainWindowDC, -1);
		DeleteFont(textFont);
	}
	ReleaseMainWindowDC(g_mainWindow, mainWindowDC);

	Sleep(1000);
	WaitForInputEvent(30);
	CopyRectWorkToMain(&bounds);
}
