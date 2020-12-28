#include "GameOver.h"

//============================================================================
//----------------------------------------------------------------------------
//                                 GameOver.c
//----------------------------------------------------------------------------
//============================================================================

#include "ColorUtils.h"
#include "Environ.h"
#include "FrameTimer.h"
#include "House.h"
#include "HouseIO.h"
#include "Macintosh.h"
#include "Main.h"
#include "MainWindow.h"
#include "Music.h"
#include "Objects.h"
#include "Play.h"
#include "RectUtils.h"
#include "Render.h"
#include "ResourceIDs.h"
#include "Sound.h"
#include "StringUtils.h"
#include "Utilities.h"

#define kNumCountDownFrames		16
#define kPageFrames				14

typedef struct pageType
{
	Rect dest;
	Rect was;
	SInt16 frame;
	SInt16 counter;
	Boolean stuck;
} pageType, *pagePtr;

void DoGameOverStarAnimation (void);
void SetUpFinalScreen (void);
void InitDiedGameOver (void);
void HandlePages (void);
void DrawPages (void);

Rect g_angelSrcRect;
HDC g_angelSrcMap;
HDC g_angelMaskMap;
SInt16 g_countDown;
Boolean g_gameOver;

static pageType g_pages[8];
static Rect g_pageSrcRect;
static Rect g_pageSrc[kPageFrames];
static Rect g_lettersSrc[8];
static HRGN g_roomRgn;
static HDC g_pageSrcMap;
static HDC g_gameOverSrcMap;
static HDC g_pageMaskMap;
static SInt16 g_stopPages;
static SInt16 g_pagesStuck;

//==============================================================  Functions
//--------------------------------------------------------------  DoGameOver
// Handles a game over.  This is a game over where the player has
// completed the house.

void DoGameOver (void)
{
	HDC			mainWindowDC;

	g_playing = false;
	SetUpFinalScreen();
	mainWindowDC = GetMainWindowDC();
	ColorRect(mainWindowDC, &g_workSrcRect, 244);
	ReleaseMainWindowDC(mainWindowDC);
	DoGameOverStarAnimation();
}

//--------------------------------------------------------------  SetUpFinalScreen
// This sets up the game over screen (again, this function is for when
// the player completes the house).

void SetUpFinalScreen (void)
{
	Rect		tempRect;
	Str255		tempStr, subStr;
	WCHAR		outStr[256];
	SInt16		count, hOffset, vOffset, i, textDown;
	HFONT		gameOverFont;

	ColorRect(g_workSrcMap, &g_workSrcRect, 244);
	QSetRect(&tempRect, 0, 0, 640, 460);
	CenterRectInRect(&tempRect, &g_workSrcRect);
	LoadScaledGraphic(g_workSrcMap, g_theHouseFile, kMilkywayPictID, &tempRect);
	textDown = tempRect.top;
	if (textDown < 0)
		textDown = 0;

	PasStringCopy(g_thisHouse.trailer, tempStr);

	SaveDC(g_workSrcMap);
	gameOverFont = CreateTahomaFont(-12, FW_BOLD);
	SelectObject(g_workSrcMap, gameOverFont);
	SetBkMode(g_workSrcMap, TRANSPARENT);
	SetTextAlign(g_workSrcMap, TA_BASELINE | TA_CENTER);
	count = 0;
	do
	{
		GetLineOfText(tempStr, count, subStr);
		WinFromMacString(outStr, ARRAYSIZE(outStr), subStr);
		hOffset = HalfRectWide(&g_workSrcRect);
		vOffset = textDown + 32 + (count * 20);
		SetTextColor(g_workSrcMap, blackColor);
		TextOut(g_workSrcMap, hOffset + 1, vOffset + 1, outStr, subStr[0]);
		SetTextColor(g_workSrcMap, whiteColor);
		TextOut(g_workSrcMap, hOffset, vOffset, outStr, subStr[0]);
		count++;
	}
	while (subStr[0] > 0);
	RestoreDC(g_workSrcMap, -1);
	DeleteObject(gameOverFont);

	CopyRectWorkToBack(&g_workSrcRect);

	for (i = 0; i < 8; i++)		// initialize the falling stars
	{
		g_pages[i].dest = g_starSrc[0];
		QOffsetRect(&g_pages[i].dest,
				g_workSrcRect.right + RandomInt(g_workSrcRect.right / 5) +
				(g_workSrcRect.right/ 4) * i,
				RandomInt(g_workSrcRect.bottom) - g_workSrcRect.bottom / 2);
		g_pages[i].was = g_pages[i].dest;
		g_pages[i].frame = RandomInt(6);
	}
}

//--------------------------------------------------------------  DoGameOverStarAnimation
// This handles the falling stars and the flying angel when a player
// completes a house.

void DoGameOverStarAnimation (void)
{
	#define		kStarFalls	8
	MSG			msg;
	Rect		angelDest;
	SInt16		which, i, count, pass;
	Boolean		noInteruption;

	angelDest = g_angelSrcRect;
	QOffsetRect(&angelDest, -96, 0);
	noInteruption = true;
	count = 0;
	pass = 0;

	while (noInteruption)
	{
		if ((angelDest.left % 32) == 0 && angelDest.left >= 0)		// add a star
		{
			PlayPrioritySound(kMysticSound, kMysticPriority);
			which = angelDest.left / 32;
			which = which % 8;
			ZeroRectCorner(&g_pages[which].dest);
			QOffsetRect(&g_pages[which].dest, angelDest.left, angelDest.bottom);
			if (count < (which + 1))
				count = which + 1;
		}

		for (i = 0; i < count; i++)
		{
			g_pages[i].frame++;
			if (g_pages[i].frame >= 6)
				g_pages[i].frame = 0;

			Mac_CopyMask(g_bonusSrcMap, g_bonusMaskMap, g_workSrcMap,
					&g_starSrc[g_pages[i].frame],
					&g_starSrc[g_pages[i].frame],
					&g_pages[i].dest);

			g_pages[i].was = g_pages[i].dest;
			g_pages[i].was.top -= kStarFalls;

			AddRectToWorkRectsWhole(&g_pages[i].was);
			AddRectToBackRects(&g_pages[i].dest);

			if (g_pages[i].dest.top < g_workSrcRect.bottom)
				QOffsetRect(&g_pages[i].dest, 0, kStarFalls);
		}

		if (angelDest.left <= (g_workSrcRect.right + 2))
		{
			Mac_CopyMask(g_angelSrcMap, g_angelMaskMap, g_workSrcMap,
					&g_angelSrcRect, &g_angelSrcRect, &angelDest);
			angelDest.left -= 2;
			AddRectToWorkRectsWhole(&angelDest);
			angelDest.left += 2;
			AddRectToBackRects(&angelDest);
			QOffsetRect(&angelDest, 2, 0);
			pass = 0;
		}

		CopyRectsQD();

		g_numWork2Main = 0;
		g_numBack2Work = 0;

		while (PeekMessageOrWaitForFrame(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				PostQuitMessage((int)msg.wParam);
				g_quitting = true;
				noInteruption = false;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			switch (msg.message)
			{
			case WM_KEYDOWN:
			case WM_LBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_XBUTTONDOWN:
				noInteruption = false;
				break;
			}
		}

		if (pass < 110)
			pass++;
		else
		{
			WaitForInputEvent(4);
			noInteruption = false;
		}
	}
}

//--------------------------------------------------------------  FlagGameOver
// Called to indicate (flag) that a game is over.  Actual game over
// sequence comes up after a short delay.

void FlagGameOver (void)
{
	g_gameOver = true;
	g_countDown = kNumCountDownFrames;
	SetMusicalMode(kPlayWholeScoreMode);
}

//--------------------------------------------------------------  InitDiedGameOver
// This is called when a game is over due to the fact that the player
// lost their last glider (died), not due to getting through the entire
// house.  This function initializes the strucures/variables.

void InitDiedGameOver (void)
{
	#define		kPageSpacing		40
	#define		kPageRightOffset	128
	#define		kPageBackUp			128
	SInt16		i;

	QSetRect(&g_pageSrcRect, 0, 0, 25, 32 * 8);
	g_gameOverSrcMap = CreateOffScreenGWorld(&g_pageSrcRect, kPreferredDepth);
	LoadGraphic(g_gameOverSrcMap, g_theHouseFile, kLettersPictID);

	QSetRect(&g_pageSrcRect, 0, 0, 32, 32 * kPageFrames);
	g_pageSrcMap = CreateOffScreenGWorld(&g_pageSrcRect, kPreferredDepth);
	LoadGraphic(g_pageSrcMap, g_theHouseFile, kPagesPictID);

	g_pageMaskMap = CreateOffScreenGWorld(&g_pageSrcRect, 1);
	LoadGraphic(g_pageMaskMap, g_theHouseFile, kPagesMaskID);

	for (i = 0; i < kPageFrames; i++)	// initialize src page rects
	{
		QSetRect(&g_pageSrc[i], 0, 0, 32, 32);
		QOffsetRect(&g_pageSrc[i], 0, 32 * i);
	}

	for (i = 0; i < 8; i++)				// initialize dest page rects
	{
		QSetRect(&g_pages[i].dest, 0, 0, 32, 32);
		CenterRectInRect(&g_pages[i].dest, &g_workSrcRect);
		QOffsetRect(&g_pages[i].dest, -g_workSrcRect.left, -g_workSrcRect.top);
		if (i < 4)
			QOffsetRect(&g_pages[i].dest, -kPageSpacing * (4 - i), 0);
		else
			QOffsetRect(&g_pages[i].dest, kPageSpacing * (i - 3), 0);
		QOffsetRect(&g_pages[i].dest, RectWide(&g_workSrcRect) / -2,
				RectWide(&g_workSrcRect) / -2);
		if (g_pages[i].dest.left % 2 == 1)
			QOffsetRect(&g_pages[i].dest, 1, 0);
		g_pages[i].was = g_pages[i].dest;
		g_pages[i].frame = 0;
		g_pages[i].counter = RandomInt(32);
		g_pages[i].stuck = false;
	}

	for (i = 0; i < 8; i++)
	{
		QSetRect(&g_lettersSrc[i], 0, 0, 25, 32);
		QOffsetRect(&g_lettersSrc[i], 0, 32 * i);
	}

	g_roomRgn = CreateRectRgn(g_justRoomsRect.left, g_justRoomsRect.top,
			g_justRoomsRect.right, g_justRoomsRect.bottom);
	g_pagesStuck = 0;
	g_stopPages = (RectTall(&g_workSrcRect) / 2) - 16;
}

//--------------------------------------------------------------  HandlePages
// This handles the pieces of paper that blow across the screen.

void HandlePages (void)
{
	SInt16		i;

	for (i = 0; i < 8; i++)
	{
		if ((g_pages[i].dest.bottom + RandomInt(8)) > g_stopPages)
		{
			g_pages[i].frame = 0;
			if (!g_pages[i].stuck)
			{
				g_pages[i].dest.right = g_pages[i].dest.left + 25;
				g_pages[i].stuck = true;
				g_pagesStuck++;
			}
		}
		else
		{
			if (g_pages[i].frame == 0)
			{
				g_pages[i].counter--;
				if (g_pages[i].counter <= 0)
					g_pages[i].frame = 1;
			}
			else if (g_pages[i].frame == 7)
			{
				g_pages[i].counter--;
				if (g_pages[i].counter <= 0)
				{
					g_pages[i].frame = 8;
					if (RandomInt(2) == 0)
						PlayPrioritySound(kPaper3Sound, kPapersPriority);
					else
						PlayPrioritySound(kPaper4Sound, kPapersPriority);
				}
				else
					QOffsetRect(&g_pages[i].dest, 10, 10);
			}
			else
			{
				g_pages[i].frame++;
				switch (g_pages[i].frame)
				{
					case 5:
					QOffsetRect(&g_pages[i].dest, 6, 6);
					break;

					case 6:
					QOffsetRect(&g_pages[i].dest, 8, 8);
					break;

					case 7:
					QOffsetRect(&g_pages[i].dest, 8, 8);
					g_pages[i].counter = RandomInt(4) + 4;
					break;

					case 8:
					case 9:
					QOffsetRect(&g_pages[i].dest, 8, 8);
					break;

					case 10:
					QOffsetRect(&g_pages[i].dest, 6, 6);
					break;

					case kPageFrames:
					QOffsetRect(&g_pages[i].dest, 8, 0);
					g_pages[i].frame = 0;
					g_pages[i].counter = RandomInt(8) + 8;
					if (RandomInt(2) == 0)
						PlayPrioritySound(kPaper1Sound, kPapersPriority);
					else
						PlayPrioritySound(kPaper2Sound, kPapersPriority);
					break;
				}
			}
		}
	}
}

//--------------------------------------------------------------  DrawPages
// This function does the drawing for the pieces of paper that blow
// across the screen.

void DrawPages (void)
{
	SInt16		i;

	for (i = 0; i < 8; i++)
	{
		if (g_pages[i].stuck)
		{
			Mac_CopyBits(g_gameOverSrcMap, g_workSrcMap,
					&g_lettersSrc[i], &g_pages[i].dest,
					srcCopy, g_roomRgn);
		}
		else
		{
			Mac_CopyMask(g_pageSrcMap, g_pageMaskMap, g_workSrcMap,
					&g_pageSrc[g_pages[i].frame],
					&g_pageSrc[g_pages[i].frame],
					&g_pages[i].dest);
		}

		QUnionSimilarRect(&g_pages[i].dest, &g_pages[i].was, &g_pages[i].was);
		AddRectToWorkRects(&g_pages[i].was);
		AddRectToBackRects(&g_pages[i].dest);

		CopyRectsQD();

		g_numWork2Main = 0;
		g_numBack2Work = 0;

		g_pages[i].was = g_pages[i].dest;
	}
}

//--------------------------------------------------------------  DoDiedGameOver
// This is called when a game is over due to the fact that the player
// lost their last glider (died), not due to getting through the entire
// house.

void DoDiedGameOver (void)
{
	MSG msg;
	Boolean userAborted;

	userAborted = false;
	InitDiedGameOver();
	CopyRectMainToWork(&g_workSrcRect);
	CopyRectMainToBack(&g_workSrcRect);

	while (g_pagesStuck < 8)
	{
		HandlePages();
		DrawPages();
		while (PeekMessageOrWaitForFrame(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				PostQuitMessage((int)msg.wParam);
				g_quitting = true;
				g_pagesStuck = 8;
				userAborted = true;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			switch (msg.message)
			{
			case WM_KEYDOWN:
			case WM_LBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_XBUTTONDOWN:
				g_pagesStuck = 8;
				userAborted = true;
				break;
			}
		}
	}

	if (g_roomRgn != nil)
		DeleteObject(g_roomRgn);

	DisposeGWorld(g_pageSrcMap);
	g_pageSrcMap = nil;

	DisposeGWorld(g_pageMaskMap);
	g_pageMaskMap = nil;

	DisposeGWorld(g_gameOverSrcMap);
	g_gameOverSrcMap = nil;
	g_playing = false;

	if (g_demoGoing)
	{
		if (!userAborted)
			WaitForInputEvent(1);
	}
	else
	{
		if (!userAborted)
			WaitForInputEvent(10);
	}
}
