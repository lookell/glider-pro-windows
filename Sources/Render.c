//============================================================================
//----------------------------------------------------------------------------
//                                  Render.c
//----------------------------------------------------------------------------
//============================================================================

#include "Render.h"

#include "DynamicMaps.h"
#include "Dynamics.h"
#include "FrameTimer.h"
#include "GliderStructs.h"
#include "Grease.h"
#include "HouseIO.h"
#include "Macintosh.h"
#include "Main.h"
#include "MainWindow.h"
#include "Objects.h"
#include "Play.h"
#include "Player.h"
#include "RectUtils.h"
#include "Room.h"
#include "RubberBands.h"
#include "Sound.h"

#define kMaxGarbageRects		48

void DrawReflection (const gliderType *thisGlider, Boolean oneOrTwo);
void RenderFlames (void);
void RenderPendulums (void);
void RenderFlyingPoints (void);
void RenderSparkles (void);
void RenderStars (void);
void RenderGlider (const gliderType *thisGlider, Boolean oneOrTwo);
void RenderBands (void);
void RenderShreds (void);
HRGN CreateClipRgnFromRects (HDC hdcDest, const Rect *theRects, size_t numRects);
void CopyRectsQD (void);

SInt16 g_numWork2Main;
SInt16 g_numBack2Work;
Boolean g_hasMirror;

static Rect g_work2MainRects[kMaxGarbageRects];
static Rect g_back2WorkRects[kMaxGarbageRects];
static HRGN g_mirrorRgn;

//==============================================================  Functions
//--------------------------------------------------------------  AddRectToWorkRects

void AddRectToWorkRects (const Rect *theRect)
{
	if (g_numWork2Main < (kMaxGarbageRects - 1))
	{
		g_work2MainRects[g_numWork2Main] = *theRect;
		if (g_work2MainRects[g_numWork2Main].left < g_justRoomsRect.left)
			g_work2MainRects[g_numWork2Main].left = g_justRoomsRect.left;
		else if (g_work2MainRects[g_numWork2Main].right > g_justRoomsRect.right)
			g_work2MainRects[g_numWork2Main].right = g_justRoomsRect.right;
		if (g_work2MainRects[g_numWork2Main].top < g_justRoomsRect.top)
			g_work2MainRects[g_numWork2Main].top = g_justRoomsRect.top;
		else if (g_work2MainRects[g_numWork2Main].bottom > g_justRoomsRect.bottom)
			g_work2MainRects[g_numWork2Main].bottom = g_justRoomsRect.bottom;
		g_numWork2Main++;
	}
}

//--------------------------------------------------------------  AddRectToBackRects

void AddRectToBackRects (const Rect *theRect)
{
	if (g_numBack2Work < (kMaxGarbageRects - 1))
	{
		g_back2WorkRects[g_numBack2Work] = *theRect;
		if (g_back2WorkRects[g_numBack2Work].left < 0)
			g_back2WorkRects[g_numBack2Work].left = 0;
		else if (g_back2WorkRects[g_numBack2Work].right > g_workSrcRect.right)
			g_back2WorkRects[g_numBack2Work].right = g_workSrcRect.right;
		if (g_back2WorkRects[g_numBack2Work].top < 0)
			g_back2WorkRects[g_numBack2Work].top = 0;
		else if (g_back2WorkRects[g_numBack2Work].bottom > g_workSrcRect.bottom)
			g_back2WorkRects[g_numBack2Work].bottom = g_workSrcRect.bottom;
		g_numBack2Work++;
	}
}

//--------------------------------------------------------------  AddRectToWorkRectsWhole

void AddRectToWorkRectsWhole (const Rect *theRect)
{
	if (g_numWork2Main < (kMaxGarbageRects - 1))
	{
		if ((theRect->right <= g_workSrcRect.left) ||
				(theRect->bottom <= g_workSrcRect.top) ||
				(theRect->left >= g_workSrcRect.right) ||
				(theRect->top >= g_workSrcRect.bottom))
			return;

		g_work2MainRects[g_numWork2Main] = *theRect;

		if (g_work2MainRects[g_numWork2Main].left < g_workSrcRect.left)
			g_work2MainRects[g_numWork2Main].left = g_workSrcRect.left;
		else if (g_work2MainRects[g_numWork2Main].right > g_workSrcRect.right)
			g_work2MainRects[g_numWork2Main].right = g_workSrcRect.right;
		if (g_work2MainRects[g_numWork2Main].top < g_workSrcRect.top)
			g_work2MainRects[g_numWork2Main].top = g_workSrcRect.top;
		else if (g_work2MainRects[g_numWork2Main].bottom > g_workSrcRect.bottom)
			g_work2MainRects[g_numWork2Main].bottom = g_workSrcRect.bottom;

		if ((g_work2MainRects[g_numWork2Main].right ==
				g_work2MainRects[g_numWork2Main].left) ||
				(g_work2MainRects[g_numWork2Main].top ==
				g_work2MainRects[g_numWork2Main].bottom))
			return;

		g_numWork2Main++;
	}
}

//--------------------------------------------------------------  DrawReflection

void DrawReflection (const gliderType *thisGlider, Boolean oneOrTwo)
{
	Rect		src, dest;
	SInt16		which;

	if (thisGlider->dontDraw)
		return;

	if (thisGlider->facing == kFaceRight)
		which = 0;
	else
		which = 1;

	dest = thisGlider->dest;
	QOffsetRect(&dest, g_playOriginH, g_playOriginV);
	QOffsetRect(&dest, kReflectionOffsetH, KReflectionOffsetV);

	SaveDC(g_workSrcMap);
	ExtSelectClipRgn(g_workSrcMap, g_mirrorRgn, RGN_AND);

	if (oneOrTwo)
	{
		if ((!g_twoPlayerGame) && (g_showFoil))
			Mac_CopyMask(g_glid2SrcMap, g_glidMaskMap, g_workSrcMap,
					&thisGlider->src, &thisGlider->mask, &dest);
		else
			Mac_CopyMask(g_glidSrcMap, g_glidMaskMap, g_workSrcMap,
					&thisGlider->src, &thisGlider->mask, &dest);
	}
	else
	{
		Mac_CopyMask(g_glid2SrcMap, g_glidMaskMap, g_workSrcMap,
				&thisGlider->src, &thisGlider->mask, &dest);
	}

	RestoreDC(g_workSrcMap, -1);

	src = thisGlider->whole;
	QOffsetRect(&src, g_playOriginH, g_playOriginV);
	QOffsetRect(&src, kReflectionOffsetH, KReflectionOffsetV);
	AddRectToWorkRects(&src);
	AddRectToBackRects(&dest);
}

//--------------------------------------------------------------  RenderFlames

void RenderFlames (void)
{
	SInt16		i;

	if ((g_numFlames == 0) && (g_numTikiFlames == 0) && (g_numCoals == 0))
		return;

	for (i = 0; i < g_numFlames; i++)
	{
		g_flames[i].mode++;
		g_flames[i].src.top += 15;
		g_flames[i].src.bottom += 15;
		if (g_flames[i].mode >= kNumCandleFlames)
		{
			g_flames[i].mode = 0;
			g_flames[i].src.top = 0;
			g_flames[i].src.bottom = 15;
		}

		Mac_CopyBits(g_savedMaps[g_flames[i].who].map, g_workSrcMap,
				&g_flames[i].src, &g_flames[i].dest, srcCopy, nil);

		AddRectToWorkRects(&g_flames[i].dest);
	}

	for (i = 0; i < g_numTikiFlames; i++)
	{
		g_tikiFlames[i].mode++;
		g_tikiFlames[i].src.top += 10;
		g_tikiFlames[i].src.bottom += 10;
		if (g_tikiFlames[i].mode >= kNumTikiFlames)
		{
			g_tikiFlames[i].mode = 0;
			g_tikiFlames[i].src.top = 0;
			g_tikiFlames[i].src.bottom = 10;
		}

		Mac_CopyBits(g_savedMaps[g_tikiFlames[i].who].map, g_workSrcMap,
				&g_tikiFlames[i].src, &g_tikiFlames[i].dest, srcCopy, nil);

		AddRectToWorkRects(&g_tikiFlames[i].dest);
	}

	for (i = 0; i < g_numCoals; i++)
	{
		g_bbqCoals[i].mode++;
		g_bbqCoals[i].src.top += 9;
		g_bbqCoals[i].src.bottom += 9;
		if (g_bbqCoals[i].mode >= kNumBBQCoals)
		{
			g_bbqCoals[i].mode = 0;
			g_bbqCoals[i].src.top = 0;
			g_bbqCoals[i].src.bottom = 9;
		}

		Mac_CopyBits(g_savedMaps[g_bbqCoals[i].who].map, g_workSrcMap,
				&g_bbqCoals[i].src, &g_bbqCoals[i].dest, srcCopy, nil);

		AddRectToWorkRects(&g_bbqCoals[i].dest);
	}
}

//--------------------------------------------------------------  RenderPendulums

void RenderPendulums (void)
{
	SInt16		i;
	Boolean		playedTikTok;

	playedTikTok = false;

	if (g_numPendulums == 0)
		return;

	g_clockFrame++;
	if ((g_clockFrame == 10) || (g_clockFrame == 15))
	{
		if (g_clockFrame >= 15)
			g_clockFrame = 0;

		for (i = 0; i < g_numPendulums; i++)
		{
			if (g_pendulums[i].active)
			{
				if (g_pendulums[i].toOrFro)
				{
					g_pendulums[i].mode++;
					g_pendulums[i].src.top += 28;
					g_pendulums[i].src.bottom += 28;

					if (g_pendulums[i].mode >= 2)
					{
						g_pendulums[i].toOrFro = !g_pendulums[i].toOrFro;
						if (!playedTikTok)
						{
							PlayPrioritySound(kTikSound, kTikPriority);
							playedTikTok = true;
						}
					}
				}
				else
				{
					g_pendulums[i].mode--;
					g_pendulums[i].src.top -= 28;
					g_pendulums[i].src.bottom -= 28;

					if (g_pendulums[i].mode <= 0)
					{
						g_pendulums[i].toOrFro = !g_pendulums[i].toOrFro;
						if (!playedTikTok)
						{
							PlayPrioritySound(kTokSound, kTokPriority);
							playedTikTok = true;
						}
					}
				}

				Mac_CopyBits(g_savedMaps[g_pendulums[i].who].map, g_workSrcMap,
						&g_pendulums[i].src, &g_pendulums[i].dest, srcCopy, nil);

				AddRectToWorkRects(&g_pendulums[i].dest);
			}
		}
	}
}

//--------------------------------------------------------------  RenderFlyingPoints

void RenderFlyingPoints (void)
{
	SInt16		i;

	if (g_numFlyingPts == 0)
		return;

	for (i = 0; i < kMaxFlyingPts; i++)
	{
		if (g_flyingPoints[i].mode != -1)
		{
			if (g_flyingPoints[i].mode > g_flyingPoints[i].stop)
			{
				g_flyingPoints[i].mode = g_flyingPoints[i].start;
				g_flyingPoints[i].loops++;
			}

			if (g_flyingPoints[i].loops >= kMaxFlyingPointsLoop)
			{
				AddRectToWorkRects(&g_flyingPoints[i].dest);
				g_flyingPoints[i].mode = -1;
				g_numFlyingPts--;
			}
			else
			{
				g_flyingPoints[i].dest.left += g_flyingPoints[i].hVel;
				g_flyingPoints[i].dest.right += g_flyingPoints[i].hVel;

				if (g_flyingPoints[i].hVel > 0)
					g_flyingPoints[i].whole.right = g_flyingPoints[i].dest.right;
				else
					g_flyingPoints[i].whole.left = g_flyingPoints[i].dest.left;

				g_flyingPoints[i].dest.top += g_flyingPoints[i].vVel;
				g_flyingPoints[i].dest.bottom += g_flyingPoints[i].vVel;

				if (g_flyingPoints[i].vVel > 0)
					g_flyingPoints[i].whole.bottom = g_flyingPoints[i].dest.bottom;
				else
					g_flyingPoints[i].whole.top = g_flyingPoints[i].dest.top;

				Mac_CopyMask(g_pointsSrcMap, g_pointsMaskMap, g_workSrcMap,
						&g_pointsSrc[g_flyingPoints[i].mode],
						&g_pointsSrc[g_flyingPoints[i].mode],
						&g_flyingPoints[i].dest);

				AddRectToWorkRects(&g_flyingPoints[i].whole);
				AddRectToBackRects(&g_flyingPoints[i].dest);
				g_flyingPoints[i].whole = g_flyingPoints[i].dest;
				g_flyingPoints[i].mode++;
			}
		}
	}
}

//--------------------------------------------------------------  RenderSparkles

void RenderSparkles (void)
{
	SInt16		i;

	if (g_numSparkles == 0)
		return;

	for (i = 0; i < kMaxSparkles; i++)
	{
		if (g_sparkles[i].mode != -1)
		{
			if (g_sparkles[i].mode >= kNumSparkleModes)
			{
				AddRectToWorkRects(&g_sparkles[i].bounds);
				g_sparkles[i].mode = -1;
				g_numSparkles--;
			}
			else
			{
				Mac_CopyMask(g_bonusSrcMap, g_bonusMaskMap, g_workSrcMap,
						&g_sparkleSrc[g_sparkles[i].mode],
						&g_sparkleSrc[g_sparkles[i].mode],
						&g_sparkles[i].bounds);

				AddRectToWorkRects(&g_sparkles[i].bounds);
				AddRectToBackRects(&g_sparkles[i].bounds);
				g_sparkles[i].mode++;
			}
		}
	}
}

//--------------------------------------------------------------  RenderStars

void RenderStars (void)
{
	SInt16		i;

	if (g_numStars == 0)
		return;

	for (i = 0; i < g_numStars; i++)
	{
		if (g_theStars[i].mode != -1)
		{
			g_theStars[i].mode++;
			g_theStars[i].src.top += 31;
			g_theStars[i].src.bottom += 31;
			if (g_theStars[i].mode >= 6)
			{
				g_theStars[i].mode = 0;
				g_theStars[i].src.top = 0;
				g_theStars[i].src.bottom = 31;
			}

			Mac_CopyBits(g_savedMaps[g_theStars[i].who].map, g_workSrcMap,
					&g_theStars[i].src, &g_theStars[i].dest, srcCopy, nil);

			AddRectToWorkRects(&g_theStars[i].dest);
		}
	}
}

//--------------------------------------------------------------  RenderGlider

void RenderGlider (const gliderType *thisGlider, Boolean oneOrTwo)
{
	Rect		src, dest;
	SInt16		which;

	if (thisGlider->dontDraw)
		return;

	if (thisGlider->facing == kFaceRight)
		which = 0;
	else
		which = 1;

	if (g_shadowVisible)
	{
		dest = thisGlider->destShadow;
		QOffsetRect(&dest, g_playOriginH, g_playOriginV);

		if ((thisGlider->mode == kGliderComingUp) ||
				(thisGlider->mode == kGliderGoingDown))
		{
			src = g_shadowSrc[which];
			src.right = src.left + (dest.right - dest.left);

			Mac_CopyMask(g_shadowSrcMap, g_shadowMaskMap, g_workSrcMap,
					&src, &src, &dest);
		}
		else if (thisGlider->mode == kGliderComingDown)
		{
			src = g_shadowSrc[which];
			src.left = src.right - (dest.right - dest.left);

			Mac_CopyMask(g_shadowSrcMap, g_shadowMaskMap, g_workSrcMap,
					&src, &src, &dest);
		}
		else
			Mac_CopyMask(g_shadowSrcMap, g_shadowMaskMap, g_workSrcMap,
					&g_shadowSrc[which], &g_shadowSrc[which], &dest);
		src = thisGlider->wholeShadow;
		QOffsetRect(&src, g_playOriginH, g_playOriginV);
		AddRectToWorkRects(&src);
		AddRectToBackRects(&dest);
	}

	dest = thisGlider->dest;
	QOffsetRect(&dest, g_playOriginH, g_playOriginV);

	if (oneOrTwo)
	{
		if ((!g_twoPlayerGame) && (g_showFoil))
			Mac_CopyMask(g_glid2SrcMap, g_glidMaskMap, g_workSrcMap,
					&thisGlider->src, &thisGlider->mask, &dest);
		else
			Mac_CopyMask(g_glidSrcMap, g_glidMaskMap, g_workSrcMap,
					&thisGlider->src, &thisGlider->mask, &dest);
	}
	else
	{
		Mac_CopyMask(g_glid2SrcMap, g_glidMaskMap, g_workSrcMap,
				&thisGlider->src, &thisGlider->mask, &dest);
	}

	src = thisGlider->whole;
	QOffsetRect(&src, g_playOriginH, g_playOriginV);
	AddRectToWorkRects(&src);
	AddRectToBackRects(&dest);
}

//--------------------------------------------------------------  RenderBands

void RenderBands (void)
{
	Rect		dest;
	SInt16		i;

	if (g_numBands == 0)
		return;

	for (i = 0; i < g_numBands; i++)
	{
		dest = g_bands[i].dest;
		QOffsetRect(&dest, g_playOriginH, g_playOriginV);
		Mac_CopyMask(g_bandsSrcMap, g_bandsMaskMap, g_workSrcMap,
				&g_bandRects[g_bands[i].mode],
				&g_bandRects[g_bands[i].mode], &dest);

		AddRectToWorkRects(&dest);
		AddRectToBackRects(&dest);
	}
}

//--------------------------------------------------------------  RenderShreds

void RenderShreds (void)
{
	Rect		src, dest;
	SInt16		i, high;

	if (g_numShredded > 0)
	{
		for (i = 0; i < g_numShredded; i++)
		{
			if (g_shreds[i].frame == 0)
			{
				g_shreds[i].bounds.bottom += 1;
				high = g_shreds[i].bounds.bottom - g_shreds[i].bounds.top;
				if (high >= 35)
					g_shreds[i].frame = 1;
				src = g_shredSrcRect;
				src.top = src.bottom - high;
				dest = g_shreds[i].bounds;
				QOffsetRect(&dest, g_playOriginH, g_playOriginV);
				Mac_CopyMask(g_shredSrcMap, g_shredMaskMap, g_workSrcMap,
						&src, &src, &dest);
				AddRectToBackRects(&dest);
				dest.top--;
				AddRectToWorkRects(&dest);
				PlayPrioritySound(kShredSound, kShredPriority);
			}
			else if (g_shreds[i].frame < 20)
			{
				g_shreds[i].bounds.top += 4;
				g_shreds[i].bounds.bottom += 4;
				dest = g_shreds[i].bounds;
				QOffsetRect(&dest, g_playOriginH, g_playOriginV);
				g_shreds[i].frame++;
				if (g_shreds[i].frame < 20)
				{
					Mac_CopyMask(g_shredSrcMap, g_shredMaskMap, g_workSrcMap,
							&g_shredSrcRect, &g_shredSrcRect, &dest);
				}
				else
				{
					AddSparkle(&g_shreds[i].bounds);
					PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
				}
				AddRectToBackRects(&dest);
				dest.top -= 4;
				AddRectToWorkRects(&dest);
			}
		}
	}
}

//--------------------------------------------------------------  CreateClipRgnFromRects

HRGN CreateClipRgnFromRects (HDC hdcDest, const Rect *theRects, size_t numRects)
{
	HRGN clipRgn;
	HRGN tempRgn;
	POINT points[2];
	size_t index;

	clipRgn = CreateRectRgn(0, 0, 0, 0);
	tempRgn = CreateRectRgn(0, 0, 0, 0);
	for (index = 0; index < numRects; ++index)
	{
		if ((theRects[index].left >= theRects[index].right) ||
			(theRects[index].top >= theRects[index].bottom))
		{
			continue;
		}
		points[0].x = theRects[index].left;
		points[0].y = theRects[index].top;
		points[1].x = theRects[index].right;
		points[1].y = theRects[index].bottom;
		// Clip region coordinates are specified in device units, so
		// the input rectangles need to be converted accordingly.
		LPtoDP(hdcDest, points, ARRAYSIZE(points));
		SetRectRgn(tempRgn, points[0].x, points[0].y, points[1].x, points[1].y);
		CombineRgn(clipRgn, clipRgn, tempRgn, RGN_OR);
	}
	DeleteObject(tempRgn);

	return clipRgn;
}

//--------------------------------------------------------------  CopyRectsQD

void CopyRectsQD (void)
{
	HDC mainWindowDC;
	HRGN blitRgn;
	SInt16 i;

	mainWindowDC = GetMainWindowDC();
	blitRgn = CreateClipRgnFromRects(mainWindowDC, g_work2MainRects, g_numWork2Main);
	Mac_CopyBits(g_workSrcMap, mainWindowDC, &g_workSrcRect, &g_workSrcRect, srcCopy, blitRgn);
	DeleteObject(blitRgn);
	ReleaseMainWindowDC(mainWindowDC);

	for (i = 0; i < g_numBack2Work; i++)
	{
		Mac_CopyBits(g_backSrcMap, g_workSrcMap,
				&g_back2WorkRects[i], &g_back2WorkRects[i],
				srcCopy, nil);
	}
}

//--------------------------------------------------------------  RenderFrame

void RenderFrame (void)
{
	MSG msg;

	if (g_hasMirror)
	{
		DrawReflection(&g_theGlider, true);
		if (g_twoPlayerGame)
			DrawReflection(&g_theGlider2, false);
	}
	HandleGrease();
	RenderPendulums();
	if (g_evenFrame)
		RenderFlames();
	else
		RenderStars();
	RenderDynamics();
	RenderFlyingPoints();
	RenderSparkles();
	RenderGlider(&g_theGlider, true);
	if (g_twoPlayerGame)
		RenderGlider(&g_theGlider2, false);
	RenderShreds();
	RenderBands();

	while (PeekMessageOrWaitForFrame(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			PostQuitMessage((int)msg.wParam);
			g_quitting = true;
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	CopyRectsQD();

	g_numWork2Main = 0;
	g_numBack2Work = 0;
}

//--------------------------------------------------------------  InitGarbageRects

void InitGarbageRects (void)
{
	SInt16		i;

	g_numWork2Main = 0;
	g_numBack2Work = 0;

	g_numSparkles = 0;
	for (i = 0; i < kMaxSparkles; i++)
		g_sparkles[i].mode = -1;

	g_numFlyingPts = 0;
	for (i = 0; i < kMaxFlyingPts; i++)
		g_flyingPoints[i].mode = -1;
}

//--------------------------------------------------------------  CopyRectBackToWork

void CopyRectBackToWork (const Rect *theRect)
{
	Mac_CopyBits(g_backSrcMap, g_workSrcMap,
			theRect, theRect, srcCopy, nil);
}

//--------------------------------------------------------------  CopyRectWorkToBack

void CopyRectWorkToBack (const Rect *theRect)
{
	Mac_CopyBits(g_workSrcMap, g_backSrcMap,
			theRect, theRect, srcCopy, nil);
}

//--------------------------------------------------------------  CopyRectWorkToMain

void CopyRectWorkToMain (const Rect *theRect)
{
	HDC			mainWindowDC;

	mainWindowDC = GetMainWindowDC();
	Mac_CopyBits(g_workSrcMap, mainWindowDC,
			theRect, theRect, srcCopy, nil);
	ReleaseMainWindowDC(mainWindowDC);
}

//--------------------------------------------------------------  CopyRectMainToWork

void CopyRectMainToWork (const Rect *theRect)
{
	HDC			mainWindowDC;

	mainWindowDC = GetMainWindowDC();
	Mac_CopyBits(mainWindowDC, g_workSrcMap,
			theRect, theRect, srcCopy, nil);
	ReleaseMainWindowDC(mainWindowDC);
}

//--------------------------------------------------------------  CopyRectMainToBack

void CopyRectMainToBack (const Rect *theRect)
{
	HDC			mainWindowDC;

	mainWindowDC = GetMainWindowDC();
	Mac_CopyBits(mainWindowDC, g_backSrcMap,
			theRect, theRect, srcCopy, nil);
	ReleaseMainWindowDC(mainWindowDC);
}

//--------------------------------------------------------------  AddToMirrorRegion

void AddToMirrorRegion (const Rect *theRect)
{
	HRGN		tempRgn;

	if (theRect->left >= theRect->right || theRect->top >= theRect->bottom)
		return;

	if (g_mirrorRgn == NULL)
	{
		g_mirrorRgn = CreateRectRgn(theRect->left, theRect->top,
				theRect->right, theRect->bottom);
	}
	else
	{
		tempRgn = CreateRectRgn(theRect->left, theRect->top,
				theRect->right, theRect->bottom);
		if (tempRgn != NULL)
		{
			CombineRgn(g_mirrorRgn, g_mirrorRgn, tempRgn, RGN_OR);
			DeleteObject(tempRgn);
		}
	}
	g_hasMirror = true;
}

//--------------------------------------------------------------  ZeroMirrorRegion

void ZeroMirrorRegion (void)
{
	if (g_mirrorRgn != NULL)
		DeleteObject(g_mirrorRgn);
	g_mirrorRgn = NULL;
	g_hasMirror = false;
}
