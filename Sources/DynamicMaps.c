#include "DynamicMaps.h"

//============================================================================
//----------------------------------------------------------------------------
//                               DynamicMaps.c
//----------------------------------------------------------------------------
//============================================================================

#include "Grease.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "Objects.h"
#include "RectUtils.h"
#include "Render.h"
#include "Room.h"
#include "Sound.h"
#include "Utilities.h"

void BackUpFlames (const Rect *src, SInt16 index);
void BackUpTikiFlames (const Rect *src, SInt16 index);
void BackUpBBQCoals (const Rect *src, SInt16 index);
void BackUpPendulum (const Rect *src, SInt16 index);
void BackUpStar (const Rect *src, SInt16 index);

sparkleType g_sparkles[kMaxSparkles];
flyingPtType g_flyingPoints[kMaxFlyingPts];
flameType g_flames[kMaxCandles];
flameType g_tikiFlames[kMaxTikis];
flameType g_bbqCoals[kMaxCoals];
pendulumType g_pendulums[kMaxPendulums];
starType g_theStars[kMaxStars];
shredType g_shreds[kMaxShredded];
Rect g_pointsSrc[15];
SInt16 g_numSparkles;
SInt16 g_numFlyingPts;
SInt16 g_numChimes;
SInt16 g_numFlames;
SInt16 g_numSavedMaps;
SInt16 g_numTikiFlames;
SInt16 g_numCoals;
SInt16 g_numPendulums;
SInt16 g_clockFrame;
SInt16 g_numStars;
SInt16 g_numShredded;

//==============================================================  Functions
//--------------------------------------------------------------  NilSavedMaps
// Deletes array of "dyanmics" offscreen pixmaps.

void NilSavedMaps (void)
{
	size_t		i;

	for (i = 0; i < kMaxSavedMaps; i++)
	{
		if (g_savedMaps[i].map != NULL)
		{
			DisposeGWorld(g_savedMaps[i].map);
			g_savedMaps[i].map = NULL;
		}
		g_savedMaps[i].where = -1;
		g_savedMaps[i].who = -1;
	}
	g_numSavedMaps = 0;
}

//--------------------------------------------------------------  BackUpToSavedMap
// Saves a copy of the room behind an object to an array of pixmaps.
// Then when the object in question is drawn, there is a copy of the
// room that it obscured so that, should the player get the object,
// it can be made to "disappear".

SInt16 BackUpToSavedMap (const Rect *theRect, SInt16 where, SInt16 who)
{
	Rect		mapRect;

	if (g_numSavedMaps >= kMaxSavedMaps)
		return(-1);

	mapRect = *theRect;
	ZeroRectCorner(&mapRect);
	g_savedMaps[g_numSavedMaps].dest = *theRect;
	g_savedMaps[g_numSavedMaps].map = CreateOffScreenGWorld(&mapRect, kPreferredDepth);

	Mac_CopyBits(g_backSrcMap, g_savedMaps[g_numSavedMaps].map,
			theRect, &mapRect, srcCopy, nil);

	g_savedMaps[g_numSavedMaps].where = where;
	g_savedMaps[g_numSavedMaps].who = who;
	g_numSavedMaps++;

	return (g_numSavedMaps - 1);	// return array index
}

//--------------------------------------------------------------  ReBackUpSavedMap
// This function is similar to the above, but assumes there is already
// a slot in the pixmap array for the object.  It re-copies the background
// and is needed when the lights in the room go on or off.

SInt16 ReBackUpSavedMap (const Rect *theRect, SInt16 where, SInt16 who)
{
	Rect		mapRect;
	SInt16		i, foundIndex;

	foundIndex = -1;

	for (i = 0; i < g_numSavedMaps; i++)
	{
		if ((g_savedMaps[i].where == where) && (g_savedMaps[i].who == who))
		{
			foundIndex = i;
			mapRect = *theRect;
			ZeroRectCorner(&mapRect);

			Mac_CopyBits(g_backSrcMap, g_savedMaps[foundIndex].map,
					theRect, &mapRect, srcCopy, nil);

			return (foundIndex);
		}
	}

	return (foundIndex);
}

//--------------------------------------------------------------  RestoreFromSavedMap
// This copies the saved background swatch to the screen - effectively
// covering up or "erasing" the object.

void RestoreFromSavedMap (SInt16 where, SInt16 who, Boolean doSparkle)
{
	Rect		mapRect, bounds;
	SInt16		i;

	for (i = 0; i < g_numSavedMaps; i++)
	{
		if ((g_savedMaps[i].where == where) && (g_savedMaps[i].who == who) &&
				(g_savedMaps[i].map != NULL))
		{
			mapRect = g_savedMaps[i].dest;
			ZeroRectCorner(&mapRect);

			Mac_CopyBits(g_savedMaps[i].map, g_backSrcMap,
					&mapRect, &g_savedMaps[i].dest, srcCopy, nil);
			Mac_CopyBits(g_savedMaps[i].map, g_workSrcMap,
					&mapRect, &g_savedMaps[i].dest, srcCopy, nil);

			AddRectToWorkRects(&g_savedMaps[i].dest);

			if (doSparkle)
			{
				bounds = g_savedMaps[i].dest;
				QOffsetRect(&bounds, -g_playOriginH, -g_playOriginV);
				AddSparkle(&bounds);
				PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
			}
			break;
		}
	}
}

//--------------------------------------------------------------  AddSparkle
// This adds a "sparkle" object to the fixed array of sparkles.

void AddSparkle (const Rect *theRect)
{
	Rect		centeredRect;
	SInt16		i;

	if (g_numSparkles < kMaxSparkles)
	{
		centeredRect = g_sparkleSrc[0];
		CenterRectInRect(&centeredRect, theRect);
		QOffsetRect(&centeredRect, g_playOriginH, g_playOriginV);

		for (i = 0; i < kMaxSparkles; i++)
			if (g_sparkles[i].mode == -1)
			{
				g_sparkles[i].bounds = centeredRect;
				g_sparkles[i].mode = 0;
				g_numSparkles++;
				break;
			}
	}
}

//--------------------------------------------------------------  AddFlyingPoint
// This adds a "flying point" object to the array of flying points.

void AddFlyingPoint (const Rect *theRect, SInt16 points, SInt16 hVel, SInt16 vVel)
{
	Rect		centeredRect;
	SInt16		i;

	if (g_numFlyingPts < kMaxFlyingPts)
	{
		centeredRect = g_pointsSrc[0];
		CenterRectInRect(&centeredRect, theRect);
		QOffsetRect(&centeredRect, g_playOriginH, g_playOriginV);

		for (i = 0; i < kMaxFlyingPts; i++)
			if (g_flyingPoints[i].mode == -1)
			{
				g_flyingPoints[i].dest = centeredRect;
				g_flyingPoints[i].whole = centeredRect;
				g_flyingPoints[i].loops = 0;
				g_flyingPoints[i].hVel = hVel;
				g_flyingPoints[i].vVel = vVel;
				switch (points)
				{
					case 100:
					g_flyingPoints[i].start = 12;
					g_flyingPoints[i].stop = 14;
					break;

					case 250:
					g_flyingPoints[i].start = 9;
					g_flyingPoints[i].stop = 11;
					break;

					case 300:
					g_flyingPoints[i].start = 6;
					g_flyingPoints[i].stop = 8;
					break;

					case 500:
					g_flyingPoints[i].start = 3;
					g_flyingPoints[i].stop = 5;
					break;

					default:
					g_flyingPoints[i].start = 0;
					g_flyingPoints[i].stop = 2;
					break;
				}
				g_flyingPoints[i].mode = g_flyingPoints[i].start;
				g_numFlyingPts++;
				break;
			}
	}
}

//--------------------------------------------------------------  BackUpFlames
// This makes copies of the area of the room behind a flame.  The flame
// graphic can be "copy masked" to this pixmap then and then simple
// CopyBits() calls will properly draw the flame on screen with the
// proper background.

void BackUpFlames (const Rect *src, SInt16 index)
{
	Rect		dest;
	SInt16		i;

	if (index < 0 || index >= g_numSavedMaps)
		return;

	QSetRect(&dest, 0, 0, 16, 15);
	for (i = 0; i < kNumCandleFlames; i++)
	{
				// Copy background to map.
		Mac_CopyBits(g_backSrcMap, g_savedMaps[index].map,
				src, &dest, srcCopy, nil);

				// Copy flame to map.
		Mac_CopyMask(g_blowerSrcMap, g_blowerMaskMap, g_savedMaps[index].map,
				&g_flame[i], &g_flame[i], &dest);

		QOffsetRect(&dest, 0, 15);
	}
}

//--------------------------------------------------------------  ReBackUpFlames
// Like the above function but this is called when the lighting changes
// in a room (lights go on or off).

void ReBackUpFlames (SInt16 where, SInt16 who)
{
	SInt16		i, f;

	for (i = 0; i < g_numSavedMaps; i++)
	{
		if ((g_savedMaps[i].where == where) && (g_savedMaps[i].who == who))
		{
			for (f = 0; f < g_numFlames; f++)
			{
				if (g_flames[f].who == i)
				{
					BackUpFlames(&g_flames[f].dest, i);
					return;
				}
			}
		}
	}
}

//--------------------------------------------------------------  AddCandleFlame
// This adds a candle flame to tha array of flames.

void AddCandleFlame (SInt16 where, SInt16 who, SInt16 h, SInt16 v)
{
	Rect		src, bounds;
	SInt16		savedNum;

	if ((g_numFlames >= kMaxCandles) || (h < 16) || (v < 15))
		return;

	QSetRect(&src, 0, 0, 16, 15);
	QOffsetRect(&src, h - 8, v - 15);
	QSetRect(&bounds, 0, 0, 16, 15 * kNumCandleFlames);
	savedNum = BackUpToSavedMap(&bounds, where, who);
	if (savedNum != -1)
	{
		BackUpFlames(&src, savedNum);
		g_flames[g_numFlames].dest = src;
		g_flames[g_numFlames].mode = RandomInt(kNumCandleFlames);
		QSetRect(&g_flames[g_numFlames].src, 0, 0, 16, 15);
		QOffsetRect(&g_flames[g_numFlames].src, 0, g_flames[g_numFlames].mode * 15);
		g_flames[g_numFlames].who = savedNum;
		g_numFlames++;
	}
}

//--------------------------------------------------------------  BackUpTikiFlames
// This is like the function BackUpFlames() but customized for Tiki torches.

void BackUpTikiFlames (const Rect *src, SInt16 index)
{
	Rect		dest;
	SInt16		i;

	if (index < 0 || index >= g_numSavedMaps)
		return;

	QSetRect(&dest, 0, 0, 8, 10);
	for (i = 0; i < kNumTikiFlames; i++)
	{
				// copy background to map
		Mac_CopyBits(g_backSrcMap, g_savedMaps[index].map,
				src, &dest, srcCopy, nil);

				// copy flame to map
		Mac_CopyMask(g_blowerSrcMap, g_blowerMaskMap, g_savedMaps[index].map,
				&g_tikiFlame[i], &g_tikiFlame[i], &dest);

		QOffsetRect(&dest, 0, 10);
	}
}

//--------------------------------------------------------------  ReBackUpTikiFlames
// This is like the function ReBackUpFlames() but customized for Tiki torches.

void ReBackUpTikiFlames (SInt16 where, SInt16 who)
{
	SInt16		i, f;

	for (i = 0; i < g_numSavedMaps; i++)
	{
		if ((g_savedMaps[i].where == where) && (g_savedMaps[i].who == who))
		{
			for (f = 0; f < g_numTikiFlames; f++)
			{
				if (g_tikiFlames[f].who == i)
				{
					BackUpTikiFlames(&g_tikiFlames[f].dest, i);
					return;
				}
			}
		}
	}
}

//--------------------------------------------------------------  AddTikiFlame
// This adds a tiki flame to the array of tiki flames.

void AddTikiFlame (SInt16 where, SInt16 who, SInt16 h, SInt16 v)
{
	Rect		src, bounds;
	SInt16		savedNum;

	if ((g_numTikiFlames >= kMaxTikis) || (h < 8) || (v < 10))
		return;

	QSetRect(&src, 0, 0, 8, 10);
	QOffsetRect(&src, h, v);
	QSetRect(&bounds, 0, 0, 8, 10 * kNumTikiFlames);
	savedNum = BackUpToSavedMap(&bounds, where, who);
	if (savedNum != -1)
	{
		BackUpTikiFlames(&src, savedNum);
		g_tikiFlames[g_numTikiFlames].dest = src;
		g_tikiFlames[g_numTikiFlames].mode = RandomInt(kNumTikiFlames);
		QSetRect(&g_tikiFlames[g_numTikiFlames].src, 0, 0, 8, 10);
		QOffsetRect(&g_tikiFlames[g_numTikiFlames].src, 0,
				g_tikiFlames[g_numTikiFlames].mode * 10);
		g_tikiFlames[g_numTikiFlames].who = savedNum;
		g_numTikiFlames++;
	}
}

//--------------------------------------------------------------  BackUpBBQCoals
// Another one - but for BBQ coals.

void BackUpBBQCoals (const Rect *src, SInt16 index)
{
	Rect		dest;
	SInt16		i;

	if (index < 0 || index >= g_numSavedMaps)
		return;

	QSetRect(&dest, 0, 0, 32, 9);
	for (i = 0; i < kNumBBQCoals; i++)
	{
				// copy background to map
		Mac_CopyBits(g_backSrcMap, g_savedMaps[index].map,
				src, &dest, srcCopy, nil);

				// copy flame to map
		Mac_CopyMask(g_blowerSrcMap, g_blowerMaskMap, g_savedMaps[index].map,
				&g_coals[i], &g_coals[i], &dest);

		QOffsetRect(&dest, 0, 9);
	}
}

//--------------------------------------------------------------  ReBackUpBBQCoals
// Sense a pattern here?

void ReBackUpBBQCoals (SInt16 where, SInt16 who)
{
	SInt16		i, f;

	for (i = 0; i < g_numSavedMaps; i++)
	{
		if ((g_savedMaps[i].where == where) && (g_savedMaps[i].who == who))
		{
			for (f = 0; f < g_numCoals; f++)
			{
				if (g_bbqCoals[f].who == i)
				{
					BackUpBBQCoals(&g_bbqCoals[f].dest, i);
					return;
				}
			}
		}
	}
}

//--------------------------------------------------------------  AddBBQCoals
// Adds BBQ coals to the array of BBQ coals.

void AddBBQCoals (SInt16 where, SInt16 who, SInt16 h, SInt16 v)
{
	Rect		src, bounds;
	SInt16		savedNum;

	if ((g_numCoals >= kMaxCoals) || (h < 32) || (v < 9))
		return;

	QSetRect(&src, 0, 0, 32, 9);
	QOffsetRect(&src, h, v);
	QSetRect(&bounds, 0, 0, 32, 9 * kNumBBQCoals);
	savedNum = BackUpToSavedMap(&bounds, where, who);
	if (savedNum != -1)
	{
		BackUpBBQCoals(&src, savedNum);
		g_bbqCoals[g_numCoals].dest = src;
		g_bbqCoals[g_numCoals].mode = RandomInt(kNumBBQCoals);
		QSetRect(&g_bbqCoals[g_numCoals].src, 0, 0, 32, 9);
		QOffsetRect(&g_bbqCoals[g_numCoals].src, 0, g_bbqCoals[g_numCoals].mode * 9);
		g_bbqCoals[g_numCoals].who = savedNum;

		g_numCoals++;
	}
}

//--------------------------------------------------------------  BackUpPendulum
// Just like many of the previous functions, but for the pendulum on the
// cuckoo clock.

void BackUpPendulum (const Rect *src, SInt16 index)
{
	Rect		dest;
	SInt16		i;

	if (index < 0 || index >= g_numSavedMaps)
		return;

	QSetRect(&dest, 0, 0, 32, 28);
	for (i = 0; i < kNumPendulums; i++)
	{
		Mac_CopyBits(g_backSrcMap, g_savedMaps[index].map,
				src, &dest, srcCopy, nil);

		Mac_CopyMask(g_bonusSrcMap, g_bonusMaskMap, g_savedMaps[index].map,
				&g_pendulumSrc[i], &g_pendulumSrc[i], &dest);

		QOffsetRect(&dest, 0, 28);
	}
}

//--------------------------------------------------------------  ReBackUpPendulum
// Backs up the pendulums in the event of lights going on or off.

void ReBackUpPendulum (SInt16 where, SInt16 who)
{
	SInt16		i, f;

	for (i = 0; i < g_numSavedMaps; i++)
	{
		if ((g_savedMaps[i].where == where) && (g_savedMaps[i].who == who))
		{
			for (f = 0; f < g_numPendulums; f++)
			{
				if (g_pendulums[f].who == i)
				{
					BackUpPendulum(&g_pendulums[f].dest, i);
					return;
				}
			}
		}
	}
}

//--------------------------------------------------------------  AddPendulum
// Adds a pendulum to the array of pendulums.

void AddPendulum (SInt16 where, SInt16 who, SInt16 h, SInt16 v)
{
	Rect		src, bounds;
	SInt16		savedNum;

	if ((g_numPendulums >= kMaxPendulums) || (h < 32) || (v < 28))
		return;

	g_clockFrame = 10;
	QSetRect(&bounds, 0, 0, 32, 28 * kNumPendulums);
	savedNum = BackUpToSavedMap(&bounds, where, who);
	if (savedNum != -1)
	{
		QSetRect(&src, 0, 0, 32, 28);
		QOffsetRect(&src, h, v);
		BackUpPendulum(&src, savedNum);
		g_pendulums[g_numPendulums].dest = src;
		g_pendulums[g_numPendulums].mode = 1;
		if (RandomInt(2) == 0)
			g_pendulums[g_numPendulums].toOrFro = true;
		else
			g_pendulums[g_numPendulums].toOrFro = false;
		g_pendulums[g_numPendulums].active = true;
		QSetRect(&g_pendulums[g_numPendulums].src, 0, 0, 32, 28);
		QOffsetRect(&g_pendulums[g_numPendulums].src, 0, 28);
		g_pendulums[g_numPendulums].who = savedNum;
		g_pendulums[g_numPendulums].where = where;
		g_pendulums[g_numPendulums].link = who;
		g_numPendulums++;
	}
}

//--------------------------------------------------------------  BackUpStar
// Makes a copy of background beneath a star.

void BackUpStar (const Rect *src, SInt16 index)
{
	Rect		dest;
	SInt16		i;

	if (index < 0 || index >= g_numSavedMaps)
		return;

	QSetRect(&dest, 0, 0, 32, 31);
	for (i = 0; i < 6; i++)
	{
		Mac_CopyBits(g_backSrcMap, g_savedMaps[index].map,
				src, &dest, srcCopy, nil);

				// copy flame to map
		Mac_CopyMask(g_bonusSrcMap, g_bonusMaskMap, g_savedMaps[index].map,
				&g_starSrc[i], &g_starSrc[i], &dest);

		QOffsetRect(&dest, 0, 31);
	}
}

//--------------------------------------------------------------  ReBackUpStar
// Re-backs up the stars - in the event of lighting switch.

void ReBackUpStar (SInt16 where, SInt16 who)
{
	SInt16		i, f;

	for (i = 0; i < g_numSavedMaps; i++)
	{
		if ((g_savedMaps[i].where == where) && (g_savedMaps[i].who == who))
		{
			for (f = 0; f < g_numStars; f++)
			{
				if (g_theStars[f].who == i)
				{
					BackUpStar(&g_theStars[f].dest, i);
					return;
				}
			}
		}
	}
}

//--------------------------------------------------------------  AddStar
// Adds a star to the star array.

void AddStar (SInt16 where, SInt16 who, SInt16 h, SInt16 v)
{
	Rect		src, bounds;
	SInt16		savedNum;

	if (g_numStars >= kMaxStars)
		return;

	QSetRect(&src, 0, 0, 32, 31);
	QOffsetRect(&src, h, v);

	QSetRect(&bounds, 0, 0, 32, 31 * 6);
	savedNum = BackUpToSavedMap(&bounds, where, who);
	if (savedNum != -1)
	{
		BackUpStar(&src, savedNum);
		g_theStars[g_numStars].dest = src;
		g_theStars[g_numStars].mode = RandomInt(6);
		QSetRect(&g_theStars[g_numStars].src, 0, 0, 32, 31);
		QOffsetRect(&g_theStars[g_numStars].src, 0, g_theStars[g_numStars].mode * 31);
		g_theStars[g_numStars].who = savedNum;
		g_theStars[g_numStars].link = who;
		g_theStars[g_numStars].where = where;

		g_numStars++;
	}
}

//--------------------------------------------------------------  StopPendulum
// Will set a flag to kill a pendulum.

void StopPendulum (SInt16 where, SInt16 who)
{
	SInt16		i;

	for (i = 0; i < g_numPendulums; i++)
	{
		if ((g_pendulums[i].link == who) && (g_pendulums[i].where == where))
			g_pendulums[i].active = false;
	}
}

//--------------------------------------------------------------  StopStar
// Will set a flag to kill a star.

void StopStar (SInt16 where, SInt16 who)
{
	SInt16		i;

	for (i = 0; i < g_numStars; i++)
	{
		if ((g_theStars[i].link == who) && (g_theStars[i].where == where))
			g_theStars[i].mode = -1;
	}
}

//--------------------------------------------------------------  AddAShreddedGlider
// Adds a shredded glider.

void AddAShreddedGlider (const Rect *bounds)
{
	if (g_numShredded >= kMaxShredded)
		return;

	g_shreds[g_numShredded].bounds.left = bounds->left + 4;
	g_shreds[g_numShredded].bounds.right = g_shreds[g_numShredded].bounds.left + 40;
	g_shreds[g_numShredded].bounds.top = bounds->top + 14;
	g_shreds[g_numShredded].bounds.bottom = g_shreds[g_numShredded].bounds.top;
	g_shreds[g_numShredded].frame = 0;

	g_numShredded++;
}

//--------------------------------------------------------------  RemoveShreds
// Remves the shredded glider.

void RemoveShreds (void)
{
	SInt16		largest, who, i;

	largest = 0;
	who = -1;
	for (i = 0; i < g_numShredded; i++)
	{
		if (g_shreds[i].frame > largest)
		{
			largest = g_shreds[i].frame;
			who = i;
		}
	}

	if (who != -1)
	{
		if (who == (g_numShredded - 1))
		{
			g_numShredded--;
			g_shreds[who].frame = 0;
		}
		else
		{
			g_numShredded--;
			g_shreds[who].bounds = g_shreds[g_numShredded].bounds;
			g_shreds[who].frame = g_shreds[g_numShredded].frame;
			g_shreds[g_numShredded].frame = 0;
		}
	}
}

//--------------------------------------------------------------  ZeroFlamesAndTheLike
// Zeroes all counters that indicate the number of flames, pendulums, etc there
// are in a room.  Called before a room is drawn.  As the room is drawn, the
// above functions are called and the counters incremented as objects of the
// specified types are drawn.

void ZeroFlamesAndTheLike (void)
{
	g_numFlames = 0;
	g_numTikiFlames = 0;
	g_numCoals = 0;
	g_numPendulums = 0;
	ZeroGrease();
	g_numStars = 0;
	g_numShredded = 0;
	g_numChimes = 0;
}
