#include "Grease.h"

//============================================================================
//----------------------------------------------------------------------------
//                                  Grease.c
//----------------------------------------------------------------------------
//============================================================================

#include "DynamicMaps.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "Objects.h"
#include "RectUtils.h"
#include "Render.h"
#include "Room.h"
#include "Sound.h"

#define kGreaseIdle			0
#define kGreaseFalling		1
#define kGreaseSpreading	2
#define kGreaseSpiltIdle	3

typedef struct greaseType
{
	Rect dest;
	SInt16 mapNum;
	SInt16 mode;
	SInt16 who;
	SInt16 where;
	SInt16 start;
	SInt16 stop;
	SInt16 frame;
	SInt16 hotNum;
	Boolean isRight;
} greaseType;

void BackupGrease (Rect *src, SInt16 index, Boolean isRight);

static greaseType g_grease[kMaxGrease];
static SInt16 g_numGrease;

//==============================================================  Functions
//--------------------------------------------------------------  HandleGrease
// Goes through all grease objects currently on screen and handles
// them.  If they're upright, nothing happens, but if they're in
// the course of falling or spilling, this function will handle
// advancing the spill, etc.

void HandleGrease (void)
{
	Rect src;
	SInt16 i;
	greaseType *thisGrease;

	for (i = 0; i < g_numGrease; i++)
	{
		thisGrease = &g_grease[i];
		if (thisGrease->mode == kGreaseFalling)
		{
			thisGrease->frame++;
			if (thisGrease->frame >= 3)  // grease completely tipped
			{
				thisGrease->frame = 3;
				thisGrease->mode = kGreaseSpreading;
				g_hotSpots[thisGrease->hotNum].action = kSlideIt;
				g_hotSpots[thisGrease->hotNum].isOn = true;
				if (thisGrease->isRight)
					QSetRect(&src, 0, -2, 2, 0);
				else
					QSetRect(&src, -2, -2, 0, 0);
				QOffsetRect(&src, -g_playOriginH, -g_playOriginV);
				QOffsetRect(&src, thisGrease->start, thisGrease->dest.bottom);
				g_hotSpots[thisGrease->hotNum].bounds = src;
			}

			QSetRect(&src, 0, 0, 32, 27);
			QOffsetRect(&src, 0, thisGrease->frame * 27);
			Mac_CopyBits(g_savedMaps[thisGrease->mapNum].map, g_workSrcMap,
					&src, &thisGrease->dest,
					srcCopy, nil);
			Mac_CopyBits(g_savedMaps[thisGrease->mapNum].map, g_backSrcMap,
					&src, &thisGrease->dest,
					srcCopy, nil);

			AddRectToWorkRects(&thisGrease->dest);
			if (thisGrease->isRight)
				QOffsetRect(&thisGrease->dest, 2, 0);
			else
				QOffsetRect(&thisGrease->dest, -2, 0);
		}
		else if (thisGrease->mode == kGreaseSpreading)
		{
			if (thisGrease->isRight)
			{
				QSetRect(&src, 0, -2, 2, 0);
				QOffsetRect(&src, thisGrease->start, thisGrease->dest.bottom);
				thisGrease->start += 2;
				g_hotSpots[thisGrease->hotNum].bounds.right += 2;
			}
			else
			{
				QSetRect(&src, -2, -2, 0, 0);
				QOffsetRect(&src, thisGrease->start, thisGrease->dest.bottom);
				thisGrease->start -= 2;
				g_hotSpots[thisGrease->hotNum].bounds.left -= 2;
			}

			Mac_PaintRect(g_backSrcMap, &src, (HBRUSH)GetStockObject(BLACK_BRUSH));
			Mac_PaintRect(g_workSrcMap, &src, (HBRUSH)GetStockObject(BLACK_BRUSH));
			AddRectToWorkRects(&src);

			if (thisGrease->isRight)
			{
				if (thisGrease->start >= thisGrease->stop)
					thisGrease->mode = kGreaseSpiltIdle;
			}
			else
			{
				if (thisGrease->start <= thisGrease->stop)
					thisGrease->mode = kGreaseSpiltIdle;
			}
		}
	}
}

//--------------------------------------------------------------  BackupGrease
// This makes copies of the region of the screen the grease is about
// to be drawn to.  It is called in the "set up" when a player has
// just entered a new room.  The "grease jar falling over" animation
// is set up here.

void BackupGrease (Rect *src, SInt16 index, Boolean isRight)
{
	Rect		dest;
	SInt16		i;

	if (index < 0 || index >= g_numSavedMaps)
		return;

	QSetRect(&dest, 0, 0, 32, 27);
	for (i = 0; i < 4; i++)
	{
		Mac_CopyBits(g_backSrcMap, g_savedMaps[index].map,
				src, &dest, srcCopy, nil);

		if (isRight)
		{
			Mac_CopyMask(g_bonusSrcMap, g_bonusMaskMap, g_savedMaps[index].map,
					&g_greaseSrcRt[i], &g_greaseSrcRt[i], &dest);
			QOffsetRect(src, 2, 0);
		}
		else
		{
			Mac_CopyMask(g_bonusSrcMap, g_bonusMaskMap, g_savedMaps[index].map,
					&g_greaseSrcLf[i], &g_greaseSrcLf[i], &dest);
			QOffsetRect(src, -2, 0);
		}
		QOffsetRect(&dest, 0, 27);
	}

}

//--------------------------------------------------------------  ReBackUpGrease
// Just like th eabove function but it is called while the player is
// active in a room and has changed the lighting situation (like turned
// off or on the lights).  It assumes certain data strucutures are
// already declared from an earlier call to the above funciton.

SInt16 ReBackUpGrease (SInt16 where, SInt16 who)
{
	Rect src;
	SInt16 i;
	const greaseType *thisGrease;

	for (i = 0; i < g_numGrease; i++)
	{
		thisGrease = &g_grease[i];
		if ((thisGrease->where == where) && (thisGrease->who == who))
		{
			if ((thisGrease->mode == kGreaseIdle) || (thisGrease->mode == kGreaseFalling))
			{
				src = thisGrease->dest;
				BackupGrease(&src, thisGrease->mapNum, thisGrease->isRight);
			}
			return (i);
		}
	}

	return (-1);
}

//--------------------------------------------------------------  AddGrease
// Called when a new room is being set up during a game.  This adds
// another jar of grease to the queue of jars to be handled.

SInt16 AddGrease (SInt16 where, SInt16 who, SInt16 h, SInt16 v,
		SInt16 distance, Boolean isRight)
{
	Rect src;
	Rect bounds;
	SInt16 savedNum;
	greaseType *thisGrease;

	if (g_numGrease >= kMaxGrease)
		return (-1);

	QSetRect(&src, 0, 0, 32, 27);
	QOffsetRect(&src, h, v);

	QSetRect(&bounds, 0, 0, 32, 27 * 4);
	savedNum = BackUpToSavedMap(&bounds, where, who);
	if (savedNum != -1)
	{
		thisGrease = &g_grease[g_numGrease];
		BackupGrease(&src, savedNum, isRight);
		if (isRight)
			QOffsetRect(&src, -8, 0);
		else
			QOffsetRect(&src, 8, 0);
		thisGrease->who = who;
		thisGrease->where = where;
		thisGrease->dest = src;
		thisGrease->mapNum = savedNum;
		thisGrease->mode = kGreaseIdle;
		thisGrease->frame = -1;
		thisGrease->hotNum = -1;
		if (isRight)
		{
			thisGrease->isRight = true;
			thisGrease->start = src.right + 4;
			thisGrease->stop = src.right + distance;
		}
		else
		{
			thisGrease->isRight = false;
			thisGrease->start = src.left - 4;
			thisGrease->stop = src.left - distance;
		}
		g_numGrease++;

		return (g_numGrease - 1);
	}
	else
		return (-1);
}

//--------------------------------------------------------------  SpillGrease
// A player has knocked a jar of grease over - this function flags that.

void SpillGrease (SInt16 who, SInt16 index)
{
	greaseType *thisGrease;

	if (who < 0 || who >= g_numGrease)
		return;
	if (index < 0 || index >= g_nHotSpots)
		return;

	thisGrease = &g_grease[who];
	if (thisGrease->mode == kGreaseIdle)
	{
		thisGrease->mode = kGreaseFalling;
		thisGrease->hotNum = index;
		PlayPrioritySound(kGreaseSpillSound, kGreaseSpillPriority);
	}
}

//--------------------------------------------------------------  RedrawAllGrease
// Called to redraw all the black lines of spilt grease.

void RedrawAllGrease (void)
{
	Rect src;
	SInt16 i;
	const greaseType *thisGrease;

	for (i = 0; i < g_numGrease; i++)
	{
		thisGrease = &g_grease[i];
		if (thisGrease->hotNum < 0 || thisGrease->hotNum >= g_nHotSpots)
			continue;

		src = g_hotSpots[thisGrease->hotNum].bounds;
		if ((thisGrease->where == g_thisRoomNumber) &&
				((src.bottom - src.top) == 2) &&
				(thisGrease->mode != kGreaseIdle))
		{
			QOffsetRect(&src, g_playOriginH, g_playOriginV);
			Mac_PaintRect(g_backSrcMap, &src, (HBRUSH)GetStockObject(BLACK_BRUSH));
			Mac_PaintRect(g_workSrcMap, &src, (HBRUSH)GetStockObject(BLACK_BRUSH));
			AddRectToWorkRects(&src);
		}
	}
}

//--------------------------------------------------------------  ZeroGrease
// Reset the number of grease jars to zero.

void ZeroGrease (void)
{
	g_numGrease = 0;
}
