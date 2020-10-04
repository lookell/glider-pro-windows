#define GP_USE_WINAPI_H

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


static greaseType grease[kMaxGrease];
static SInt16 numGrease;


//==============================================================  Functions
//--------------------------------------------------------------  HandleGrease
// Goes through all grease objects currently on screen and handles…
// them.  If they're upright, nothing happens, but if they're in…
// the course of falling or spilling, this function will handle…
// advancing the spill, etc.

void HandleGrease (void)
{
	Rect		src;
	SInt16		i;

	if (numGrease == 0)
		return;

	for (i = 0; i < numGrease; i++)
	{
		if (grease[i].mode == kGreaseFalling)
		{
			grease[i].frame++;
			if (grease[i].frame >= 3)		// grease completely tipped
			{
				grease[i].frame = 3;
				grease[i].mode = kGreaseSpreading;
				hotSpots[grease[i].hotNum].action = kSlideIt;
				hotSpots[grease[i].hotNum].isOn = true;
				if (grease[i].isRight)
					QSetRect(&src, 0, -2, 2, 0);
				else
					QSetRect(&src, -2, -2, 0, 0);
				QOffsetRect(&src, -playOriginH, -playOriginV);
				QOffsetRect(&src, grease[i].start, grease[i].dest.bottom);
				hotSpots[grease[i].hotNum].bounds = src;
			}

			QSetRect(&src, 0, 0, 32, 27);
			QOffsetRect(&src, 0, grease[i].frame * 27);
			Mac_CopyBits(savedMaps[grease[i].mapNum].map, workSrcMap,
					&src, &grease[i].dest,
					srcCopy, nil);
			Mac_CopyBits(savedMaps[grease[i].mapNum].map, backSrcMap,
					&src, &grease[i].dest,
					srcCopy, nil);

			AddRectToWorkRects(&grease[i].dest);
			if (grease[i].isRight)
				QOffsetRect(&grease[i].dest, 2, 0);
			else
				QOffsetRect(&grease[i].dest, -2, 0);
		}
		else if (grease[i].mode == kGreaseSpreading)
		{
			if (grease[i].isRight)
			{
				QSetRect(&src, 0, -2, 2, 0);
				QOffsetRect(&src, grease[i].start, grease[i].dest.bottom);
				grease[i].start += 2;
				hotSpots[grease[i].hotNum].bounds.right += 2;
			}
			else
			{
				QSetRect(&src, -2, -2, 0, 0);
				QOffsetRect(&src, grease[i].start, grease[i].dest.bottom);
				grease[i].start -= 2;
				hotSpots[grease[i].hotNum].bounds.left -= 2;
			}

			Mac_PaintRect(backSrcMap, &src, (HBRUSH)GetStockObject(BLACK_BRUSH));
			Mac_PaintRect(workSrcMap, &src, (HBRUSH)GetStockObject(BLACK_BRUSH));
			AddRectToWorkRects(&src);

			if (grease[i].isRight)
			{
				if (grease[i].start >= grease[i].stop)
					grease[i].mode = kGreaseSpiltIdle;
			}
			else
			{
				if (grease[i].start <= grease[i].stop)
					grease[i].mode = kGreaseSpiltIdle;
			}
		}
	}
}

//--------------------------------------------------------------  BackupGrease
// This makes copies of the region of the screen the grease is about…
// to be drawn to.  It is called in the "set up" when a player has…
// just entered a new room.  The "grease jar falling over" animation…
// is set up here.

void BackupGrease (Rect *src, SInt16 index, Boolean isRight)
{
	Rect		dest;
	SInt16		i;

	if (index < 0 || index >= numSavedMaps)
		return;

	QSetRect(&dest, 0, 0, 32, 27);
	for (i = 0; i < 4; i++)
	{
		Mac_CopyBits(backSrcMap, savedMaps[index].map,
				src, &dest, srcCopy, nil);

		if (isRight)
		{
			Mac_CopyMask(bonusSrcMap, bonusMaskMap, savedMaps[index].map,
					&greaseSrcRt[i], &greaseSrcRt[i], &dest);
			QOffsetRect(src, 2, 0);
		}
		else
		{
			Mac_CopyMask(bonusSrcMap, bonusMaskMap, savedMaps[index].map,
					&greaseSrcLf[i], &greaseSrcLf[i], &dest);
			QOffsetRect(src, -2, 0);
		}
		QOffsetRect(&dest, 0, 27);
	}

}

//--------------------------------------------------------------  ReBackUpGrease
// Just like th eabove function but it is called while the player is…
// active in a room and has changed the lighting situation (like turned…
// off or on the lights).  It assumes certain data strucutures are…
// already declared from an earlier call to the above funciton.

SInt16 ReBackUpGrease (SInt16 where, SInt16 who)
{
	Rect		src;
	SInt16		i;

	for (i = 0; i < numGrease; i++)
	{
		if ((grease[i].where == where) && (grease[i].who == who))
		{
			if ((grease[i].mode == kGreaseIdle) || (grease[i].mode == kGreaseFalling))
			{
				src = grease[i].dest;
				BackupGrease(&src, grease[i].mapNum, grease[i].isRight);
			}
			return (i);
		}
	}

	return (-1);
}

//--------------------------------------------------------------  AddGrease
// Called when a new room is being set up during a game.  This adds…
// another jar of grease to the queue of jars to be handled.

SInt16 AddGrease (SInt16 where, SInt16 who, SInt16 h, SInt16 v,
		SInt16 distance, Boolean isRight)
{
	Rect		src, bounds;
	SInt16		savedNum;

	if (numGrease >= kMaxGrease)
		return (-1);

	QSetRect(&src, 0, 0, 32, 27);
	QOffsetRect(&src, h, v);

	QSetRect(&bounds, 0, 0, 32, 27 * 4);
	savedNum = BackUpToSavedMap(&bounds, where, who);
	if (savedNum != -1)
	{
		BackupGrease (&src, savedNum, isRight);
		if (isRight)
			QOffsetRect(&src, -8, 0);
		else
			QOffsetRect(&src, 8, 0);
		grease[numGrease].who = who;
		grease[numGrease].where = where;
		grease[numGrease].dest = src;
		grease[numGrease].mapNum = savedNum;
		grease[numGrease].mode = kGreaseIdle;
		grease[numGrease].frame = -1;
		grease[numGrease].hotNum = -1;
		if (isRight)
		{
			grease[numGrease].isRight = true;
			grease[numGrease].start = src.right + 4;
			grease[numGrease].stop = src.right + distance;
		}
		else
		{
			grease[numGrease].isRight = false;
			grease[numGrease].start = src.left - 4;
			grease[numGrease].stop = src.left - distance;
		}
		numGrease++;

		return (numGrease - 1);
	}
	else
		return (-1);
}

//--------------------------------------------------------------  SpillGrease
// A player has knocked a jar of grease over - this function flags that.

void SpillGrease (SInt16 who, SInt16 index)
{
	if (who < 0 || who >= numGrease)
		return;
	if (index < 0 || index >= nHotSpots)
		return;

	if (grease[who].mode == kGreaseIdle)
	{
		grease[who].mode = kGreaseFalling;
		grease[who].hotNum = index;
		PlayPrioritySound(kGreaseSpillSound, kGreaseSpillPriority);
	}
}

//--------------------------------------------------------------  RedrawAllGrease
// Called to redraw all the black lines of spilt grease.

void RedrawAllGrease (void)
{
	Rect		src;
	SInt16		i;

	if (numGrease == 0)
		return;

	for (i = 0; i < numGrease; i++)
	{
		if (grease[i].hotNum < 0 || grease[i].hotNum >= nHotSpots)
			continue;

		src = hotSpots[grease[i].hotNum].bounds;
		if ((grease[i].where == thisRoomNumber) &&
				((src.bottom - src.top) == 2) &&
				(grease[i].mode != kGreaseIdle))
		{
			QOffsetRect(&src, playOriginH, playOriginV);
			Mac_PaintRect(backSrcMap, &src, (HBRUSH)GetStockObject(BLACK_BRUSH));
			Mac_PaintRect(workSrcMap, &src, (HBRUSH)GetStockObject(BLACK_BRUSH));
			AddRectToWorkRects(&src);
		}
	}
}

//--------------------------------------------------------------  ZeroGrease
// Reset the number of grease jars to zero.

void ZeroGrease (void)
{
	numGrease = 0;
}

