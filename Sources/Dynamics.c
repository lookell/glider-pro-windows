#include "Dynamics.h"

//============================================================================
//----------------------------------------------------------------------------
//                                 Dynamics.c
//----------------------------------------------------------------------------
//============================================================================

#include "DynamicMaps.h"
#include "Environ.h"
#include "HouseIO.h"
#include "Interactions.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "Modes.h"
#include "Music.h"
#include "Objects.h"
#include "Play.h"
#include "Player.h"
#include "RectUtils.h"
#include "Render.h"
#include "Room.h"
#include "RoomGraphics.h"
#include "RubberBands.h"
#include "Sound.h"
#include "Utilities.h"

#define kShoveVelocity      8
#define kBalloonStop        8
#define kBalloonStart       310
#define kCopterStart        8
#define kCopterStop         310
#define kDartVelocity       6
#define kDartStop           310
#define kEnemyDropSpeed     8

void CheckDynamicCollision (const dynaType *theDinah, gliderPtr thisGlider, Boolean doOffset);
void CheckForPlayerCollisions (const dynaType *theDinah, Boolean doOffset);
Boolean DidBandHitDynamic (const dynaType *theDinah);
void RenderToast (const dynaType *theDinah);
void RenderBalloon (const dynaType *theDinah);
void RenderCopter (const dynaType *theDinah);
void RenderDart (const dynaType *theDinah);
void RenderBall (const dynaType *theDinah);
void RenderDrip (const dynaType *theDinah);
void RenderFish (const dynaType *theDinah);
void HandleSparkleObject (dynaType *theDinah);
void HandleToast (dynaType *theDinah);
void HandleMacPlus (dynaType *theDinah);
void HandleTV (dynaType *theDinah, SInt16 who);
void HandleCoffee (dynaType *theDinah);
void HandleOutlet (dynaType *theDinah);
void HandleVCR (dynaType *theDinah);
void HandleStereo (dynaType *theDinah);
void HandleMicrowave (dynaType *theDinah);
void HandleBalloon (dynaType *theDinah);
void HandleCopter (dynaType *theDinah);
void HandleDart (dynaType *theDinah);
void HandleBall (dynaType *theDinah);
void HandleDrip (dynaType *theDinah);
void HandleFish (dynaType *theDinah);

Rect breadSrc[kNumBreadPicts];
dynaType dinahs[kMaxDynamicObs];
SInt16 numDynamics;

//==============================================================  Functions
//--------------------------------------------------------------  CheckDynamicCollision
// Checks for a collision betwen the glider and one of the dynamic objects.
// For example, did the glider hit a flying piece of toast?

void CheckDynamicCollision (const dynaType *theDinah, gliderPtr thisGlider, Boolean doOffset)
{
	Rect dinahRect;

	dinahRect = theDinah->dest;
	if (doOffset)
		QOffsetRect(&dinahRect, -playOriginH, -playOriginV);

	if (SectGlider(thisGlider, &dinahRect, true))
	{
		if ((thisGlider->mode == kGliderNormal) ||
				(thisGlider->mode == kGliderFaceLeft) ||
				(thisGlider->mode == kGliderFaceRight) ||
				(thisGlider->mode == kGliderBurning) ||
				(thisGlider->mode == kGliderGoingFoil) ||
				(thisGlider->mode == kGliderLosingFoil))
		{
			if ((foilTotal > 0) || (thisGlider->mode == kGliderLosingFoil))
			{
				if (IsRectLeftOfRect(&dinahRect, &thisGlider->dest))
					thisGlider->hDesiredVel = kShoveVelocity;
				else
					thisGlider->hDesiredVel = -kShoveVelocity;
				if (theDinah->vVel < 0)
					thisGlider->vDesiredVel = theDinah->vVel;
				PlayPrioritySound(kFoilHitSound, kFoilHitPriority);
				if ((evenFrame) && (foilTotal > 0))
				{
					foilTotal--;
					if (foilTotal <= 0)
						StartGliderFoilLosing(thisGlider);
				}
			}
			else
			{
				StartGliderFadingOut(thisGlider);
				PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
			}
		}
	}
}

//--------------------------------------------------------------  CheckForPlayerCollisions
// Checks for collisions between the one or two gliders and one of the dynamic objects.

void CheckForPlayerCollisions (const dynaType *theDinah, Boolean doOffset)
{
	if (twoPlayerGame)
	{
		if (onePlayerLeft)
		{
			if (playerDead == theGlider.which)
			{
				CheckDynamicCollision(theDinah, &theGlider2, doOffset);
			}
			else
			{
				CheckDynamicCollision(theDinah, &theGlider, doOffset);
			}
		}
		else
		{
			CheckDynamicCollision(theDinah, &theGlider, doOffset);
			CheckDynamicCollision(theDinah, &theGlider2, doOffset);
		}
	}
	else
	{
		CheckDynamicCollision(theDinah, &theGlider, doOffset);
	}
}

//--------------------------------------------------------------  DidBandHitDynamic
// Checks to see if a rubber band struck a dynamic.

Boolean DidBandHitDynamic (const dynaType *theDinah)
{
	Rect dinahRect;
	SInt16 i;
	Boolean collided;

	dinahRect = theDinah->dest;

	collided = false;
	for (i = 0; i < numBands; i++)
	{
		if (bands[i].dest.bottom < dinahRect.top)
			collided = false;
		else if (bands[i].dest.top > dinahRect.bottom)
			collided = false;
		else if (bands[i].dest.right < dinahRect.left)
			collided = false;
		else if (bands[i].dest.left > dinahRect.right)
			collided = false;
		else
			collided = true;

		if (collided)
			break;
	}

	return (collided);
}

//
// The following handful of functions handle drawing specific "dynamic" objecsts.
//

//--------------------------------------------------------------  RenderToast

void RenderToast (const dynaType *theDinah)
{
	Rect src;
	Rect dest;
	SInt16 vClip;

	if (theDinah->moving)
	{
		dest = theDinah->dest;
		QOffsetRect(&dest, playOriginH, playOriginV);
		src = breadSrc[theDinah->frame];
		vClip = theDinah->dest.bottom - theDinah->hVel;
		if (vClip > 0)
		{
			src.bottom -= vClip;
			dest.bottom -= vClip;
		}

		Mac_CopyMask(toastSrcMap, toastMaskMap, workSrcMap,
				&src, &src, &dest);

		AddRectToBackRects(&dest);
		dest = theDinah->whole;
		QOffsetRect(&dest, playOriginH, playOriginV);
		AddRectToWorkRects(&dest);
	}
}

//--------------------------------------------------------------  RenderBalloon

void RenderBalloon (const dynaType *theDinah)
{
	Rect src;
	Rect dest;

	if (theDinah->moving)
	{
		dest = theDinah->dest;
		QOffsetRect(&dest, playOriginH, playOriginV);
		src = balloonSrc[theDinah->frame];

		Mac_CopyMask(balloonSrcMap, balloonMaskMap, workSrcMap,
				&src, &src, &dest);

		AddRectToBackRects(&dest);
		dest = theDinah->whole;
		QOffsetRect(&dest, playOriginH, playOriginV);
		AddRectToWorkRects(&dest);
	}
}

//--------------------------------------------------------------  RenderCopter

void RenderCopter (const dynaType *theDinah)
{
	Rect src;
	Rect dest;

	if (theDinah->moving)
	{
		dest = theDinah->dest;
		QOffsetRect(&dest, playOriginH, playOriginV);
		src = copterSrc[theDinah->frame];

		Mac_CopyMask(copterSrcMap, copterMaskMap, workSrcMap,
				&src, &src, &dest);

		AddRectToBackRects(&dest);
		dest = theDinah->whole;
		QOffsetRect(&dest, playOriginH, playOriginV);
		AddRectToWorkRects(&dest);
	}
}

//--------------------------------------------------------------  RenderDart

void RenderDart (const dynaType *theDinah)
{
	Rect src;
	Rect dest;

	if (theDinah->moving)
	{
		dest = theDinah->dest;
		QOffsetRect(&dest, playOriginH, playOriginV);
		src = dartSrc[theDinah->frame];

		Mac_CopyMask(dartSrcMap, dartMaskMap, workSrcMap,
				&src, &src, &dest);

		AddRectToBackRects(&dest);
		dest = theDinah->whole;
		QOffsetRect(&dest, playOriginH, playOriginV);
		AddRectToWorkRects(&dest);
	}
}

//--------------------------------------------------------------  RenderBall

void RenderBall (const dynaType *theDinah)
{
	Rect src;
	Rect dest;

	dest = theDinah->dest;
	QOffsetRect(&dest, playOriginH, playOriginV);
	src = ballSrc[theDinah->frame];

	Mac_CopyMask(ballSrcMap, ballMaskMap, workSrcMap,
			&src, &src, &dest);

	AddRectToBackRects(&dest);
	dest = theDinah->whole;
	QOffsetRect(&dest, playOriginH, playOriginV);
	AddRectToWorkRects(&dest);
}

//--------------------------------------------------------------  RenderDrip

void RenderDrip (const dynaType *theDinah)
{
	Rect src;
	Rect dest;

	dest = theDinah->dest;
	QOffsetRect(&dest, playOriginH, playOriginV);
	src = dripSrc[theDinah->frame];

	Mac_CopyMask(dripSrcMap, dripMaskMap, workSrcMap,
			&src, &src, &dest);

	AddRectToBackRects(&dest);
	dest = theDinah->whole;
	QOffsetRect(&dest, playOriginH, playOriginV);
	AddRectToWorkRects(&dest);
}

//--------------------------------------------------------------  RenderFish

void RenderFish (const dynaType *theDinah)
{
	Rect src;
	Rect dest;

	dest = theDinah->dest;
	QOffsetRect(&dest, playOriginH, playOriginV);
	src = fishSrc[theDinah->frame];

	if (theDinah->moving)
	{
		Mac_CopyMask(fishSrcMap, fishMaskMap, workSrcMap,
				&src, &src, &dest);
		AddRectToBackRects(&dest);
		dest = theDinah->whole;
		QOffsetRect(&dest, playOriginH, playOriginV);
		AddRectToWorkRects(&dest);
	}
	else
	{
		Mac_CopyBits(fishSrcMap, workSrcMap,
				&src, &dest, srcCopy, nil);
		AddRectToBackRects(&dest);
		dest = theDinah->whole;
		QOffsetRect(&dest, playOriginH, playOriginV);
		AddRectToWorkRects(&dest);
	}
}

//
// The following handful of functions are called each game frame and handle
// the movement and state of the various types of dynamic objects.
//

//--------------------------------------------------------------  HandleSparkleObject

void HandleSparkleObject (dynaType *theDinah)
{
	Rect tempRect;

	if (theDinah->active)  // is it on?
	{
		if (theDinah->frame <= 0)  // is it idle?
		{  // it is idle
			theDinah->timer--;
			if (theDinah->timer <= 0)
			{
				theDinah->timer = RandomInt(240) + 60;  // reset timer
				theDinah->frame = kNumSparkleModes;  // time to sparkle
				tempRect = theDinah->dest;
				AddSparkle(&tempRect);
				PlayPrioritySound(kMysticSound, kMysticPriority);
			}
		}
		else  // it's sparkling
		{
			theDinah->frame--;
		}
	}
	else
	{
	}
}

//--------------------------------------------------------------  HandleToast

void HandleToast (dynaType *theDinah)
{
	Rect dest;

	if (theDinah->moving)
	{
		if (evenFrame)
		{
			theDinah->frame++;
			if (theDinah->frame >= kNumBreadPicts)
				theDinah->frame = 0;
		}
		CheckForPlayerCollisions(theDinah, false);
		VOffsetRect(&theDinah->dest, theDinah->vVel);
		theDinah->whole = theDinah->dest;
		if (theDinah->vVel > 0)
			theDinah->whole.top -= theDinah->vVel;
		else
			theDinah->whole.bottom -= theDinah->vVel;
		theDinah->vVel++;  // falls
		if (theDinah->vVel > theDinah->count)
		{
			dest = theDinah->whole;
			QOffsetRect(&dest, playOriginH, playOriginV);
			AddRectToWorkRects(&dest);
			theDinah->moving = false;
			theDinah->frame = theDinah->timer;
			PlayPrioritySound(kToastLandSound, kToastLandPriority);
		}
	}
	else
	{
		if (theDinah->active)
			theDinah->frame--;
		if (theDinah->frame <= 0)
		{
			if (theDinah->active)
			{
				theDinah->vVel = (SInt16)-theDinah->count;
				theDinah->frame = 0;
				theDinah->moving = true;
				PlayPrioritySound(kToastLaunchSound, kToastLaunchPriority);
			}
			else
			{
				theDinah->frame = theDinah->timer;
			}
		}
	}
}

//--------------------------------------------------------------  HandleMacPlus

void HandleMacPlus (dynaType *theDinah)
{
	if (theDinah->timer > 0)
	{
		theDinah->timer--;
		if (theDinah->active)
		{
			if (theDinah->timer == 0)
			{
				AddRectToWorkRects(&theDinah->dest);
			}
			else if (theDinah->timer == 1)
			{
				PlayPrioritySound(kMacBeepSound, kMacBeepPriority);
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&plusScreen2, &theDinah->dest,
						srcCopy, nil);
				AddRectToBackRects(&theDinah->dest);
			}
			else if (theDinah->timer == 30)
			{
				PlayPrioritySound(kMacOnSound, kMacOnPriority);
			}
		}
		else
		{
			if (theDinah->timer == 0)
			{
				AddRectToWorkRects(&theDinah->dest);
			}
			else if (theDinah->timer == 1)
			{
				PlayPrioritySound(kMacOffSound, kMacOffPriority);
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&plusScreen1, &theDinah->dest,
						srcCopy, nil);
				AddRectToBackRects(&theDinah->dest);
			}
		}
	}
}

//--------------------------------------------------------------  HandleTV

void HandleTV (dynaType *theDinah, SInt16 who)
{
	if (theDinah->timer > 0)
	{
		theDinah->timer--;
		if (theDinah->active)
		{
			if (theDinah->timer == 0)
			{
				if ((thisMac.hasQT) && (hasMovie) && (tvInRoom) &&
						(who == tvWithMovieNumber))
				{
				}
				else
				{
					AddRectToWorkRects(&theDinah->dest);
				}
			}
			else if (theDinah->timer == 1)
			{
				PlayPrioritySound(kTVOnSound, kTVOnPriority);
				if ((thisMac.hasQT) && (hasMovie) && (tvInRoom) &&
						(who == tvWithMovieNumber))
				{
				}
				else
				{
					Mac_CopyBits(applianceSrcMap, backSrcMap,
							&tvScreen2, &theDinah->dest,
							srcCopy, nil);
					AddRectToBackRects(&theDinah->dest);
				}
			}
		}
		else
		{
			if (theDinah->timer == 0)
			{
				AddRectToWorkRects(&theDinah->dest);
			}
			else if (theDinah->timer == 1)
			{
				PlayPrioritySound(kTVOffSound, kTVOffPriority);
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&tvScreen1, &theDinah->dest,
						srcCopy, nil);
				AddRectToBackRects(&theDinah->dest);
			}
		}
	}
}

//--------------------------------------------------------------  HandleCoffee

void HandleCoffee (dynaType *theDinah)
{
	if (theDinah->timer > 0)
	{
		theDinah->timer--;
		if (theDinah->active)
		{
			if (theDinah->timer == 0)
			{
				AddRectToWorkRects(&theDinah->dest);
				theDinah->timer = 200 + RandomInt(200);
			}
			else if (theDinah->timer == 1)
			{
				PlayPrioritySound(kMacOnSound, kMacOnPriority);
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&coffeeLight2, &theDinah->dest,
						srcCopy, nil);
				AddRectToBackRects(&theDinah->dest);
			}
			else if (theDinah->timer == 100)
			{
				PlayPrioritySound(kCoffeeSound, kCoffeePriority);
				theDinah->timer = 200 + RandomInt(200);
			}
		}
		else
		{
			if (theDinah->timer == 0)
			{
				AddRectToWorkRects(&theDinah->dest);
			}
			else if (theDinah->timer == 1)
			{
				PlayPrioritySound(kMacOffSound, kMacOffPriority);
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&coffeeLight1, &theDinah->dest,
						srcCopy, nil);
				AddRectToBackRects(&theDinah->dest);
			}
		}
	}
}

//--------------------------------------------------------------  HandleOutlet

void HandleOutlet (dynaType *theDinah)
{
	if (theDinah->position != 0)
	{
		theDinah->timer--;

		CheckForPlayerCollisions(theDinah, true);

		if (theDinah->timer <= 0)
		{
			theDinah->frame = 0;
			theDinah->position = 0;
			theDinah->timer = theDinah->count;
		}
		else
		{
			if ((theDinah->timer % 5) == 0)
				PlayPrioritySound(kZapSound, kZapPriority);
			theDinah->frame++;
			if (theDinah->frame >= kNumOutletPicts)
				theDinah->frame = 1;
		}

		if ((theDinah->position != 0) || (theDinah->hVel > 0))
		{
			Mac_CopyBits(applianceSrcMap, workSrcMap,
					&outletSrc[theDinah->frame],
					&theDinah->dest,
					srcCopy, nil);
		}
		else
		{
			Mac_PaintRect(workSrcMap, &theDinah->dest, (HBRUSH)GetStockObject(BLACK_BRUSH));
		}
		AddRectToWorkRects(&theDinah->dest);
	}
	else
	{
		if (theDinah->active)
			theDinah->timer--;

		if (theDinah->timer <= 0)
		{
			if (theDinah->active)
			{
				theDinah->position = 1;
				theDinah->timer = kLengthOfZap;
				PlayPrioritySound(kZapSound, kZapPriority);
			}
			else
			{
				theDinah->timer = theDinah->count;
			}
		}
	}
}

//--------------------------------------------------------------  HandleVCR

void HandleVCR (dynaType *theDinah)
{
	if (theDinah->timer > 0)
	{
		theDinah->timer--;
		if (theDinah->active)
		{
			if (theDinah->timer == 0)
			{
				AddRectToWorkRects(&theDinah->dest);
				theDinah->timer = 115;
			}
			else if (theDinah->timer == 5)
			{
				PlayPrioritySound(kMacOnSound, kMacOnPriority);
			}
			else if (theDinah->timer == 1)
			{
				PlayPrioritySound(kVCRSound, kVCRPriority);
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&vcrTime2, &theDinah->dest,
						srcCopy, nil);
				AddRectToBackRects(&theDinah->dest);
			}
			else if (theDinah->timer == 100)
			{
				AddRectToWorkRects(&theDinah->dest);
				theDinah->timer = 115;
				theDinah->frame = 1 - theDinah->frame;
			}
			else if (theDinah->timer == 101)
			{
				if (theDinah->frame == 0)
				{
					Mac_CopyBits(applianceSrcMap, backSrcMap,
							&vcrTime2, &theDinah->dest,
							srcCopy, nil);
					AddRectToBackRects(&theDinah->dest);
				}
				else
				{
					Mac_CopyBits(applianceSrcMap, backSrcMap,
							&vcrTime1, &theDinah->dest,
							srcCopy, nil);
					AddRectToBackRects(&theDinah->dest);
				}
			}
		}
		else
		{
			if (theDinah->timer == 0)
			{
				AddRectToWorkRects(&theDinah->dest);
			}
			else if (theDinah->timer == 1)
			{
				PlayPrioritySound(kMacOffSound, kMacOffPriority);
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&vcrTime1, &theDinah->dest,
						srcCopy, nil);
				AddRectToBackRects(&theDinah->dest);
			}
		}
	}
}

//--------------------------------------------------------------  HandleStereo

void HandleStereo (dynaType *theDinah)
{
	if (theDinah->timer > 0)
	{
		theDinah->timer--;
		if (theDinah->active)
		{
			if (theDinah->timer == 0)
			{
				AddRectToWorkRects(&theDinah->dest);
				ToggleMusicWhilePlaying();
			}
			else if (theDinah->timer == 1)
			{
				PlayPrioritySound(kMacOnSound, kMacOnPriority);
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&stereoLight2, &theDinah->dest,
						srcCopy, nil);
				AddRectToBackRects(&theDinah->dest);
			}
		}
		else
		{
			if (theDinah->timer == 0)
			{
				AddRectToWorkRects(&theDinah->dest);
				ToggleMusicWhilePlaying();
			}
			else if (theDinah->timer == 1)
			{
				PlayPrioritySound(kMacOffSound, kMacOffPriority);
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&stereoLight1, &theDinah->dest,
						srcCopy, nil);
				AddRectToBackRects(&theDinah->dest);
			}
		}
	}
}

//--------------------------------------------------------------  HandleMicrowave

void HandleMicrowave (dynaType *theDinah)
{
	Rect dest;

	if (theDinah->timer > 0)
	{
		theDinah->timer--;
		if (theDinah->active)
		{
			if (theDinah->timer == 0)
			{
				AddRectToWorkRects(&theDinah->dest);
			}
			else if (theDinah->timer == 1)
			{
				PlayPrioritySound(kMacOnSound, kMacOnPriority);
				dest = theDinah->dest;
				dest.right = dest.left + 16;
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&microOn, &dest,
						srcCopy, nil);
				QOffsetRect(&dest, 16, 0);
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&microOn, &dest,
						srcCopy, nil);
				QOffsetRect(&dest, 16, 0);
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&microOn, &dest,
						srcCopy, nil);
				AddRectToBackRects(&theDinah->dest);
			}
		}
		else
		{
			if (theDinah->timer == 0)
			{
				AddRectToWorkRects(&theDinah->dest);
			}
			else if (theDinah->timer == 1)
			{
				PlayPrioritySound(kMacOffSound, kMacOffPriority);
				dest = theDinah->dest;
				dest.right = dest.left + 16;
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&microOff, &dest,
						srcCopy, nil);
				QOffsetRect(&dest, 16, 0);
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&microOff, &dest,
						srcCopy, nil);
				QOffsetRect(&dest, 16, 0);
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&microOff, &dest,
						srcCopy, nil);
				AddRectToBackRects(&theDinah->dest);
			}
		}
	}
}

//--------------------------------------------------------------  HandleBalloon

void HandleBalloon (dynaType *theDinah)
{
	Rect dest;

	if (theDinah->moving)
	{
		if (theDinah->vVel < 0)
		{
			if (evenFrame)
			{
				theDinah->frame++;
				if (theDinah->frame >= 6)
					theDinah->frame = 0;
			}
			CheckForPlayerCollisions(theDinah, false);

			if ((numBands > 0) && (DidBandHitDynamic(theDinah)))
			{
				theDinah->frame = 6;
				theDinah->vVel = kEnemyDropSpeed;
				PlayPrioritySound(kPopSound, kPopPriority);
			}
			else
			{
				VOffsetRect(&theDinah->dest, theDinah->vVel);
				theDinah->whole = theDinah->dest;
				theDinah->whole.bottom -= theDinah->vVel;
			}
		}
		else
		{
			if (evenFrame)
			{
				theDinah->frame++;
				if (theDinah->frame >= 8)
					theDinah->frame = 6;
			}

			VOffsetRect(&theDinah->dest, theDinah->vVel);
			theDinah->whole = theDinah->dest;
			theDinah->whole.top -= theDinah->vVel;
		}

		if ((theDinah->dest.top <= kBalloonStop) ||
				(theDinah->dest.bottom >= kBalloonStart))
		{
			dest = theDinah->whole;
			QOffsetRect(&dest, playOriginH, playOriginV);
			AddRectToWorkRects(&dest);
			dest = theDinah->dest;
			AddSparkle(&dest);
			PlayPrioritySound(kEnemyOutSound, kEnemyOutPriority);
			theDinah->moving = false;
			theDinah->vVel = -2;
			theDinah->timer = theDinah->count;
			theDinah->dest.bottom = kBalloonStart;
			theDinah->dest.top = theDinah->dest.bottom - RectTall(&balloonSrc[0]);
			theDinah->whole = theDinah->dest;
		}
	}
	else  // balloon is idle, waiting to appear
	{
		if (theDinah->active)
		{
			theDinah->timer--;
			if (theDinah->timer <= 0)
			{
				theDinah->moving = true;
				if (theDinah->count < kStartSparkle)
				{
					dest = theDinah->dest;
					AddSparkle(&dest);
					PlayPrioritySound(kEnemyInSound, kEnemyInPriority);
				}
			}
			else if (theDinah->timer == kStartSparkle)
			{
				dest = theDinah->dest;
				AddSparkle(&dest);
				PlayPrioritySound(kEnemyInSound, kEnemyInPriority);
			}
		}
	}
}

//--------------------------------------------------------------  HandleCopter

void HandleCopter (dynaType *theDinah)
{
	Rect dest;

	if (theDinah->moving)  // is 'copter about?
	{
		if (theDinah->hVel != 0)  // 'copter was not shot
		{
			theDinah->frame++;
			if (theDinah->frame >= 8)
				theDinah->frame = 0;
			CheckForPlayerCollisions(theDinah, false);
			if ((numBands > 0) && (DidBandHitDynamic(theDinah)))
			{
				theDinah->frame = 8;
				theDinah->hVel = 0;
				theDinah->vVel = kEnemyDropSpeed;
				PlayPrioritySound(kPaperCrunchSound, kPaperCrunchPriority);
			}
			else
			{
				HOffsetRect(&theDinah->dest, theDinah->hVel);
				VOffsetRect(&theDinah->dest, theDinah->vVel);
				theDinah->whole = theDinah->dest;
				theDinah->whole.top -= theDinah->vVel;
				if (theDinah->hVel < 0)
					theDinah->whole.right -= theDinah->hVel;
				else
					theDinah->whole.left -= theDinah->hVel;
			}
		}
		else  // 'copter was shot
		{
			theDinah->frame++;
			if (theDinah->frame >= 10)
				theDinah->frame = 8;
			VOffsetRect(&theDinah->dest, theDinah->vVel);
			theDinah->whole = theDinah->dest;
			theDinah->whole.top -= theDinah->vVel;
		}

		if ((theDinah->dest.top <= kCopterStart) ||
				(theDinah->dest.bottom >= kCopterStop))
		{
			dest = theDinah->whole;
			QOffsetRect(&dest, playOriginH, playOriginV);
			AddRectToWorkRects(&dest);
			dest = theDinah->dest;
			AddSparkle(&dest);
			PlayPrioritySound(kEnemyOutSound, kEnemyOutPriority);
			theDinah->moving = false;
			theDinah->vVel = 2;
			if (theDinah->type == kCopterLf)
				theDinah->hVel = -1;
			else
				theDinah->hVel = 1;
			theDinah->timer = theDinah->count;
			theDinah->dest.top = kCopterStart;
			theDinah->dest.bottom = theDinah->dest.top + RectTall(&copterSrc[0]);
			theDinah->dest.left = theDinah->position;
			theDinah->dest.right = theDinah->dest.left + 32;
			theDinah->whole = theDinah->dest;
		}
	}
	else
	{
		if (theDinah->active)
		{
			theDinah->timer--;
			if (theDinah->timer <= 0)
			{
				theDinah->moving = true;
				if (theDinah->count < kStartSparkle)
				{
					dest = theDinah->dest;
					AddSparkle(&dest);
					PlayPrioritySound(kEnemyInSound, kEnemyInPriority);
				}
			}
			else if (theDinah->timer == kStartSparkle)
			{
				dest = theDinah->dest;
				AddSparkle(&dest);
				PlayPrioritySound(kEnemyInSound, kEnemyInPriority);
			}
		}
	}
}

//--------------------------------------------------------------  HandleDart

void HandleDart (dynaType *theDinah)
{
	Rect dest;

	if (theDinah->moving)  // Dart has appeared
	{
		if (theDinah->hVel != 0)  // meaning it isn't falling
		{
			CheckForPlayerCollisions(theDinah, false);
			if ((numBands > 0) && (DidBandHitDynamic(theDinah)))
			{
				if (theDinah->type == kDartLf)
					theDinah->frame = 1;
				else
					theDinah->frame = 3;
				theDinah->hVel = 0;
				theDinah->vVel = kEnemyDropSpeed;
				PlayPrioritySound(kPaperCrunchSound, kPaperCrunchPriority);
			}
			else
			{
				HOffsetRect(&theDinah->dest, theDinah->hVel);
				VOffsetRect(&theDinah->dest, theDinah->vVel);
				theDinah->whole = theDinah->dest;
				theDinah->whole.top -= theDinah->vVel;
				if (theDinah->hVel < 0)
					theDinah->whole.right -= theDinah->hVel;
				else
					theDinah->whole.left -= theDinah->hVel;
			}
		}
		else  // dart is falling straight down
		{
			VOffsetRect(&theDinah->dest, theDinah->vVel);
			theDinah->whole = theDinah->dest;
			theDinah->whole.top -= theDinah->vVel;
		}

		if ((theDinah->dest.left <= 0) ||
				(theDinah->dest.right >= kRoomWide) ||
				(theDinah->dest.bottom >= kDartStop))
		{
			dest = theDinah->whole;
			QOffsetRect(&dest, playOriginH, playOriginV);
			AddRectToWorkRects(&dest);
			dest = theDinah->dest;
			AddSparkle(&dest);
			PlayPrioritySound(kEnemyOutSound, kEnemyOutPriority);
			theDinah->moving = false;
			theDinah->vVel = 2;
			if (theDinah->type == kDartLf)
			{
				theDinah->frame = 0;
				theDinah->hVel = -kDartVelocity;
				theDinah->dest.right = kRoomWide;
				theDinah->dest.left = theDinah->dest.right - RectWide(&dartSrc[0]);
			}
			else
			{
				theDinah->frame = 2;
				theDinah->hVel = kDartVelocity;
				theDinah->dest.left = 0;
				theDinah->dest.right = theDinah->dest.left + RectWide(&dartSrc[0]);
			}
			theDinah->timer = theDinah->count;
			theDinah->dest.top = theDinah->position;
			theDinah->dest.bottom = theDinah->dest.top + RectTall(&dartSrc[0]);
			theDinah->whole = theDinah->dest;
		}
	}
	else
	{
		if (theDinah->active)
		{
			theDinah->timer--;
			if (theDinah->timer <= 0)
			{
				theDinah->moving = true;
				if (theDinah->count < kStartSparkle)
				{
					dest = theDinah->dest;
					AddSparkle(&dest);
					PlayPrioritySound(kEnemyInSound, kEnemyInPriority);
				}
			}
			else if (theDinah->timer == kStartSparkle)
			{
				dest = theDinah->dest;
				AddSparkle(&dest);
				PlayPrioritySound(kEnemyInSound, kEnemyInPriority);
			}
		}
	}
}

//--------------------------------------------------------------  HandleBall

void HandleBall (dynaType *theDinah)
{
	CheckForPlayerCollisions(theDinah, false);

	if (theDinah->moving)  // is ball bouncing?
	{
		VOffsetRect(&theDinah->dest, theDinah->vVel);
		if (theDinah->dest.bottom >= theDinah->position)  // bounce!
		{
			theDinah->whole = theDinah->dest;
			theDinah->whole.top -= theDinah->vVel;
			theDinah->whole.bottom = theDinah->position;
			theDinah->dest.bottom = theDinah->position;
			theDinah->dest.top = theDinah->dest.bottom - 32;
			if (theDinah->active)
			{
				theDinah->vVel = theDinah->count;
			}
			else
			{
				theDinah->vVel = -((theDinah->vVel * 3) / 4);
				if (theDinah->vVel == 0)
					theDinah->moving = false;  // stop bounce
			}
			if (theDinah->whole.bottom < theDinah->dest.bottom)
				theDinah->whole.bottom = theDinah->dest.bottom;
			PlayPrioritySound(kBounceSound, kBouncePriority);
			if (theDinah->moving)
				theDinah->frame = 1;
		}
		else
		{
			theDinah->whole = theDinah->dest;
			if (theDinah->vVel > 0)
				theDinah->whole.top -= theDinah->vVel;
			else
				theDinah->whole.bottom -= theDinah->vVel;
			if (evenFrame)
				theDinah->vVel++;
			theDinah->frame = 0;
		}
	}
	else
	{
		if (theDinah->active)
		{
			theDinah->vVel = theDinah->count;
			theDinah->moving = true;
			evenFrame = true;
		}
	}
}

//--------------------------------------------------------------  HandleDrip

void HandleDrip (dynaType *theDinah)
{
	Rect dest;

	if (theDinah->moving)
	{
		if (evenFrame)
			theDinah->frame = 9 - theDinah->frame;
		CheckForPlayerCollisions(theDinah, false);

		VOffsetRect(&theDinah->dest, theDinah->vVel);
		if (theDinah->dest.bottom >= theDinah->position)
		{
			dest = theDinah->whole;
			QOffsetRect(&dest, playOriginH, playOriginV);
			AddRectToWorkRects(&dest);
			theDinah->dest.top = theDinah->hVel;
			theDinah->dest.bottom = theDinah->dest.top + 12;
			PlayPrioritySound(kDropSound, kDropPriority);
			theDinah->vVel = 0;
			theDinah->timer = theDinah->count;
			theDinah->frame = 3;
			theDinah->moving = false;
		}
		else
		{
			theDinah->whole = theDinah->dest;
			theDinah->whole.top -= theDinah->vVel;
			if (evenFrame)
				theDinah->vVel++;
		}
	}
	else
	{
		if (theDinah->active)
		{
			theDinah->timer--;

			if (theDinah->timer == 6)
			{
				theDinah->frame = 0;
			}
			else if (theDinah->timer == 4)
			{
				theDinah->frame = 1;
			}
			else if (theDinah->timer == 2)
			{
				theDinah->frame = 2;
			}
			else if (theDinah->timer <= 0)
			{
				VOffsetRect(&theDinah->dest, 3);
				theDinah->whole = theDinah->dest;
				theDinah->moving = true;
				theDinah->frame = 4;
				PlayPrioritySound(kDripSound, kDripPriority);
			}
		}
	}
}

//--------------------------------------------------------------  HandleFish

void HandleFish (dynaType *theDinah)
{
	Rect dest;

	if (theDinah->moving)  // fish leaping
	{
		if ((theDinah->vVel >= 0) && (theDinah->frame < 7))
			theDinah->frame++;
		CheckForPlayerCollisions(theDinah, false);

		VOffsetRect(&theDinah->dest, theDinah->vVel);
		if (theDinah->dest.bottom >= theDinah->position)  // splash down
		{
			dest = theDinah->whole;
			QOffsetRect(&dest, playOriginH, playOriginV);
			AddRectToWorkRects(&dest);
			theDinah->dest.bottom = theDinah->position;
			theDinah->dest.top = theDinah->dest.bottom - 16;
			theDinah->whole = theDinah->dest;
			theDinah->whole.top -= 2;
			PlayPrioritySound(kDropSound, kDropPriority);
			theDinah->vVel = theDinah->count;
			theDinah->timer = theDinah->hVel;
			theDinah->frame = 0;
			theDinah->moving = false;
			PlayPrioritySound(kFishInSound, kFishInPriority);
		}
		else
		{
			theDinah->whole = theDinah->dest;
			if (theDinah->vVel > 0)
				theDinah->whole.top -= theDinah->vVel;
			else
				theDinah->whole.bottom -= theDinah->vVel;
			if (evenFrame)
				theDinah->vVel++;
		}
	}
	else  // fish idle
	{
		theDinah->whole = theDinah->dest;
		if ((theDinah->timer & 0x0003) == 0x0003)
		{
			theDinah->frame++;
			if (theDinah->frame > 3)
				theDinah->frame = 0;
			if ((theDinah->frame == 1) || (theDinah->frame == 2))
			{
				theDinah->dest.top++;
				theDinah->dest.bottom++;
				theDinah->whole.bottom++;
			}
			else
			{
				theDinah->dest.top--;
				theDinah->dest.bottom--;
				theDinah->whole.top--;
			}
		}
		if (theDinah->active)
		{
			theDinah->timer--;
			if (theDinah->timer <= 0)  // fish leaps
			{
				theDinah->whole = theDinah->dest;
				theDinah->moving = true;
				theDinah->frame = 4;
				PlayPrioritySound(kFishOutSound, kFishOutPriority);
			}
		}
	}
}

//--------------------------------------------------------------  HandleDynamics
// This is the master function that calls all the specific handlers above.

void HandleDynamics (void)
{
	dynaType *theDinah;
	SInt16 i;

	for (i = 0; i < numDynamics; i++)
	{
		theDinah = &dinahs[i];
		switch (theDinah->type)
		{
			case kSparkle:
			HandleSparkleObject(theDinah);
			break;

			case kToaster:
			HandleToast(theDinah);
			break;

			case kMacPlus:
			HandleMacPlus(theDinah);
			break;

			case kTV:
			HandleTV(theDinah, i);
			break;

			case kCoffee:
			HandleCoffee(theDinah);
			break;

			case kOutlet:
			HandleOutlet(theDinah);
			break;

			case kVCR:
			HandleVCR(theDinah);
			break;

			case kStereo:
			HandleStereo(theDinah);
			break;

			case kMicrowave:
			HandleMicrowave(theDinah);
			break;

			case kBalloon:
			HandleBalloon(theDinah);
			break;

			case kCopterLf:
			case kCopterRt:
			HandleCopter(theDinah);
			break;

			case kDartLf:
			case kDartRt:
			HandleDart(theDinah);
			break;

			case kBall:
			HandleBall(theDinah);
			break;

			case kDrip:
			HandleDrip(theDinah);
			break;

			case kFish:
			HandleFish(theDinah);
			break;

			default:
			break;
		}
	}
}

//--------------------------------------------------------------  HandleDynamics
// This is the master function that calls all the various rendering handlers
// above.

void RenderDynamics (void)
{
	const dynaType *theDinah;
	SInt16 i;

	for (i = 0; i < numDynamics; i++)
	{
		theDinah = &dinahs[i];
		switch (theDinah->type)
		{
			case kToaster:
			RenderToast(theDinah);
			break;

			case kBalloon:
			RenderBalloon(theDinah);
			break;

			case kCopterLf:
			case kCopterRt:
			RenderCopter(theDinah);
			break;

			case kDartLf:
			case kDartRt:
			RenderDart(theDinah);
			break;

			case kBall:
			RenderBall(theDinah);
			break;

			case kDrip:
			RenderDrip(theDinah);
			break;

			case kFish:
			RenderFish(theDinah);
			break;

			default:
			break;
		}
	}
}

//--------------------------------------------------------------  ZeroDinahs
// This clears all dynamics - zeros them all out.  Used to initialize them.

void ZeroDinahs (void)
{
	dynaType *theDinah;
	SInt16 i;

	for (i = 0; i < kMaxDynamicObs; i++)
	{
		theDinah = &dinahs[i];
		theDinah->type = kObjectIsEmpty;
		QSetRect(&theDinah->dest, 0, 0, 0, 0);
		QSetRect(&theDinah->whole, 0, 0, 0, 0);
		theDinah->hVel = 0;
		theDinah->vVel = 0;
		theDinah->count = 0;
		theDinah->frame = 0;
		theDinah->timer = 0;
		theDinah->position = 0;
		theDinah->room = 0;
		theDinah->byte0 = 0;
		theDinah->active = false;
	}
	numDynamics = 0;
}

//--------------------------------------------------------------  AddDynamicObject
// When a room is being drawn, various dynamic objects are pointed here.
// This function sets up the structures to handle them.

SInt16 AddDynamicObject (SInt16 what, const Rect *where, const objectType *who,
		SInt16 room, SInt16 index, Boolean isOn)
{
	SInt16 position;
	SInt16 velocity;
	Boolean lilFrame;
	dynaType *theDinah;

	if (numDynamics >= kMaxDynamicObs)
		return (-1);

	theDinah = &dinahs[numDynamics];

	theDinah->type = what;
	switch (what)
	{
		case kSparkle:
		theDinah->dest = sparkleSrc[0];
		ZeroRectCorner(&theDinah->dest);
		QOffsetRect(&theDinah->dest, where->left, where->top);
		theDinah->whole = theDinah->dest;
		theDinah->hVel = 0;
		theDinah->vVel = 0;
		theDinah->count = 0;
		theDinah->frame = 0;
		theDinah->timer = RandomInt(60) + 15;
		theDinah->position = 0;
		theDinah->room = room;
		theDinah->byte0 = (Byte)index;
		theDinah->byte1 = 0;
		theDinah->moving = false;
		theDinah->active = isOn;
		break;

		case kToaster:
		theDinah->dest = breadSrc[0];
		CenterRectInRect(&theDinah->dest, where);
		VOffsetRect(&theDinah->dest, where->top - theDinah->dest.top);
		theDinah->whole = theDinah->dest;
		theDinah->hVel = where->top + 2;  // hVel used as clip
		position = who->data.g.height;  // reverse engineer init. vel.
		velocity = 0;
		do
		{
			velocity++;
			position -= velocity;
		}
		while (position > 0);
		theDinah->vVel = -velocity;
		theDinah->count = velocity;  // count = initial velocity
		theDinah->frame = (SInt16)who->data.g.delay * 3;
		theDinah->timer = theDinah->frame;
		theDinah->position = 0;  // launch/idle state
		theDinah->room = room;
		theDinah->byte0 = (Byte)index;
		theDinah->byte1 = 0;
		theDinah->moving = false;
		theDinah->active = isOn;
		break;

		case kMacPlus:
		theDinah->dest = plusScreen1;
		ZeroRectCorner(&theDinah->dest);
		QOffsetRect(&theDinah->dest,
				where->left + playOriginH + 10,
				where->top + playOriginV + 7);
		theDinah->whole = theDinah->dest;
		theDinah->hVel = 0;
		theDinah->vVel = 0;
		theDinah->count = 0;
		theDinah->frame = 0;
		theDinah->timer = 0;
		theDinah->position = 0;
		theDinah->room = room;
		theDinah->byte0 = (Byte)index;
		theDinah->byte1 = 0;
		theDinah->moving = false;
		theDinah->active = isOn;
		break;

		case kTV:
		theDinah->dest = tvScreen1;
		ZeroRectCorner(&theDinah->dest);
		QOffsetRect(&theDinah->dest,
				where->left + playOriginH + 17,
				where->top + playOriginV + 10);
		theDinah->whole = theDinah->dest;
		theDinah->hVel = 0;
		theDinah->vVel = 0;
		theDinah->count = 0;
		theDinah->frame = 0;
		theDinah->timer = 0;
		theDinah->position = 0;
		theDinah->room = room;
		theDinah->byte0 = (Byte)index;
		theDinah->byte1 = 0;
		theDinah->moving = false;
		theDinah->active = isOn;
		break;

		case kCoffee:
		theDinah->dest = coffeeLight1;
		ZeroRectCorner(&theDinah->dest);
		QOffsetRect(&theDinah->dest,
				where->left + playOriginH + 32,
				where->top + playOriginV + 57);
		theDinah->whole = theDinah->dest;
		theDinah->hVel = 0;
		theDinah->vVel = 0;
		theDinah->count = 0;
		theDinah->frame = 0;
		if (isOn)
			theDinah->timer = 200;
		else
			theDinah->timer = 0;
		theDinah->position = 0;
		theDinah->room = room;
		theDinah->byte0 = (Byte)index;
		theDinah->byte1 = 0;
		theDinah->moving = false;
		theDinah->active = isOn;
		break;

		case kOutlet:
		theDinah->dest = outletSrc[0];
		ZeroRectCorner(&theDinah->dest);
		QOffsetRect(&theDinah->dest,
				where->left + playOriginH,
				where->top + playOriginV);
		theDinah->whole = theDinah->dest;
		theDinah->hVel = numLights;
		theDinah->vVel = 0;
		theDinah->count = ((SInt16)who->data.g.delay * 6) / kTicksPerFrame;
		theDinah->frame = 0;
		theDinah->timer = theDinah->count;
		theDinah->position = 0;  // launch/idle state
		theDinah->room = room;
		theDinah->byte0 = (Byte)index;
		theDinah->byte1 = 0;
		theDinah->moving = false;
		theDinah->active = isOn;
		break;

		case kVCR:
		theDinah->dest = vcrTime1;
		ZeroRectCorner(&theDinah->dest);
		QOffsetRect(&theDinah->dest,
				where->left + playOriginH + 64,
				where->top + playOriginV + 6);
		theDinah->whole = theDinah->dest;
		theDinah->hVel = 0;
		theDinah->vVel = 0;
		theDinah->count = 0;
		theDinah->frame = 0;
		if (isOn)
			theDinah->timer = 115;
		else
			theDinah->timer = 0;
		theDinah->position = 0;
		theDinah->room = room;
		theDinah->byte0 = (Byte)index;
		theDinah->byte1 = 0;
		theDinah->moving = false;
		theDinah->active = isOn;
		break;

		case kStereo:
		theDinah->dest = stereoLight1;
		ZeroRectCorner(&theDinah->dest);
		QOffsetRect(&theDinah->dest,
				where->left + playOriginH + 56,
				where->top + playOriginV + 20);
		theDinah->whole = theDinah->dest;
		theDinah->hVel = 0;
		theDinah->vVel = 0;
		theDinah->count = 0;
		theDinah->frame = 0;
		theDinah->timer = 0;
		theDinah->position = 0;
		theDinah->room = room;
		theDinah->byte0 = (Byte)index;
		theDinah->byte1 = 0;
		theDinah->moving = false;
		theDinah->active = isOn;
		break;

		case kMicrowave:
		theDinah->dest = microOn;
		ZeroRectCorner(&theDinah->dest);
		QOffsetRect(&theDinah->dest,
				where->left + playOriginH + 14,
				where->top + playOriginV + 13);
		theDinah->dest.right = theDinah->dest.left + 48;
		theDinah->whole = theDinah->dest;
		theDinah->hVel = 0;
		theDinah->vVel = 0;
		theDinah->count = 0;
		theDinah->frame = 0;
		theDinah->timer = 0;
		theDinah->position = 0;
		theDinah->room = room;
		theDinah->byte0 = (Byte)index;
		theDinah->byte1 = 0;
		theDinah->moving = false;
		theDinah->active = isOn;
		break;

		case kBalloon:
		theDinah->dest = balloonSrc[0];
		ZeroRectCorner(&theDinah->dest);
		QOffsetRect(&theDinah->dest, where->left, 0);
		theDinah->dest.bottom = kBalloonStart;
		theDinah->dest.top = theDinah->dest.bottom - RectTall(&balloonSrc[0]);
		theDinah->whole = theDinah->dest;
		theDinah->hVel = 0;
		theDinah->vVel = -2;
		theDinah->count = ((SInt16)who->data.h.delay * 6) / kTicksPerFrame;
		theDinah->frame = 0;
		theDinah->timer = theDinah->count;
		theDinah->position = 0;
		theDinah->room = room;
		theDinah->byte0 = (Byte)index;
		theDinah->byte1 = 0;
		theDinah->moving = false;
		theDinah->active = isOn;  // initially idle
		break;

		case kCopterLf:
		case kCopterRt:
		theDinah->dest = copterSrc[0];
		ZeroRectCorner(&theDinah->dest);
		QOffsetRect(&theDinah->dest, where->left, 0);
		theDinah->dest.top = kCopterStart;
		theDinah->dest.bottom = theDinah->dest.top + RectTall(&copterSrc[0]);
		theDinah->whole = theDinah->dest;
		if (what == kCopterLf)
			theDinah->hVel = -1;
		else
			theDinah->hVel = 1;
		theDinah->vVel = 2;
		theDinah->count = ((SInt16)who->data.h.delay * 6) / kTicksPerFrame;
		theDinah->frame = 0;
		theDinah->timer = theDinah->count;
		theDinah->position = theDinah->dest.left;
		theDinah->room = room;
		theDinah->byte0 = (Byte)index;
		theDinah->byte1 = 0;
		theDinah->moving = false;
		theDinah->active = isOn;  // initially idle
		break;

		case kDartLf:
		case kDartRt:
		theDinah->dest = dartSrc[0];
		ZeroRectCorner(&theDinah->dest);
		if (what == kDartLf)
		{
			QOffsetRect(&theDinah->dest, kRoomWide - RectWide(&dartSrc[0]), where->top);
			theDinah->hVel = -kDartVelocity;
			theDinah->frame = 0;
		}
		else
		{
			QOffsetRect(&theDinah->dest, 0, where->top);
			theDinah->hVel = kDartVelocity;
			theDinah->frame = 2;
		}
		theDinah->whole = theDinah->dest;
		theDinah->vVel = 2;
		theDinah->count = ((SInt16)who->data.h.delay * 6) / kTicksPerFrame;
		theDinah->timer = theDinah->count;
		theDinah->position = theDinah->dest.top;
		theDinah->byte0 = (Byte)index;
		theDinah->byte1 = 0;
		theDinah->moving = false;
		theDinah->active = isOn;  // initially idle
		break;

		case kBall:
		theDinah->dest = ballSrc[0];
		ZeroRectCorner(&theDinah->dest);
		QOffsetRect(&theDinah->dest, where->left, where->top);
		theDinah->whole = theDinah->dest;
		theDinah->hVel = 0;
		position = who->data.h.length;  // reverse engineer init. vel.
		velocity = 0;
		evenFrame = true;
		lilFrame = true;
		do
		{
			if (lilFrame)
				velocity++;
			lilFrame = !lilFrame;
			position -= velocity;
		}
		while (position > 0);
		theDinah->vVel = -velocity;
		theDinah->moving = false;
		theDinah->count = -velocity;  // count = initial velocity
		theDinah->frame = 0;
		theDinah->timer = 0;
		theDinah->position = theDinah->dest.bottom;
		theDinah->room = room;
		theDinah->byte0 = (Byte)index;
		theDinah->byte1 = 0;
		theDinah->active = isOn;
		break;

		case kDrip:
		theDinah->dest = dripSrc[0];
		CenterRectInRect(&theDinah->dest, where);
		VOffsetRect(&theDinah->dest, where->top - theDinah->dest.top);
		theDinah->whole = theDinah->dest;
		theDinah->hVel = theDinah->dest.top;  // remember
		theDinah->vVel = 0;
		theDinah->count = ((SInt16)who->data.h.delay * 6) / kTicksPerFrame;
		theDinah->frame = 3;
		theDinah->timer = theDinah->count;
		theDinah->position = theDinah->dest.top + who->data.h.length;
		theDinah->room = room;
		theDinah->byte0 = (Byte)index;
		theDinah->byte1 = 0;
		theDinah->moving = false;
		theDinah->active = isOn;
		break;

		case kFish:
		theDinah->dest = fishSrc[0];
		QOffsetRect(&theDinah->dest, where->left + 10, where->top + 8);
		theDinah->whole = theDinah->dest;
		theDinah->hVel = ((SInt16)who->data.h.delay * 6) / kTicksPerFrame;
		position = who->data.h.length;  // reverse engineer init. vel.
		velocity = 0;
		evenFrame = true;
		lilFrame = true;
		do
		{
			if (lilFrame)
				velocity++;
			lilFrame = !lilFrame;
			position -= velocity;
		}
		while (position > 0);
		theDinah->vVel = -velocity;
		theDinah->count = -velocity;  // count = initial velocity
		theDinah->frame = 0;
		theDinah->timer = theDinah->hVel;
		theDinah->position = theDinah->dest.bottom;
		theDinah->room = room;
		theDinah->byte0 = (Byte)index;
		theDinah->byte1 = 0;
		theDinah->moving = false;
		theDinah->active = isOn;
		break;

		default:
		return (-1);
		break;
	}

	numDynamics++;

	return (numDynamics - 1);
}
