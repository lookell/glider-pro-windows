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


#define kShoveVelocity		8
#define kBalloonStop		8
#define kBalloonStart		310
#define kCopterStart		8
#define kCopterStop			310
#define kDartVelocity		6
#define kDartStop			310
#define kEnemyDropSpeed		8


Rect		breadSrc[kNumBreadPicts];
dynaType	dinahs[kMaxDynamicObs];
SInt16		numDynamics;


//==============================================================  Functions
//--------------------------------------------------------------  CheckDynamicCollision

// Checks for a collision betwen the glider and one of the dynamic objects.
// For example, did the glider hit a flying piece of toast?

void CheckDynamicCollision (SInt16 who, gliderPtr thisGlider, Boolean doOffset)
{
	Rect		dinahRect;

	dinahRect = dinahs[who].dest;
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
				if (dinahs[who].vVel < 0)
					thisGlider->vDesiredVel = dinahs[who].vVel;
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

//--------------------------------------------------------------  DidBandHitDynamic

// Checks to see if a rubber band struck a dynamic.

Boolean DidBandHitDynamic (SInt16 who)
{
	Rect		dinahRect;
	SInt16		i;
	Boolean		collided;

	dinahRect = dinahs[who].dest;

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

//--------------------------------------------------------------  RenderToast

// The following handful of functions handle drawing specific "dynamic" objecsts.

void RenderToast (SInt16 who)
{
	Rect		src, dest;
	SInt16		vClip;

	if (dinahs[who].moving)
	{
		dest = dinahs[who].dest;
		QOffsetRect(&dest, playOriginH, playOriginV);
		src = breadSrc[dinahs[who].frame];
		vClip = dinahs[who].dest.bottom - dinahs[who].hVel;
		if (vClip > 0)
		{
			src.bottom -= vClip;
			dest.bottom -= vClip;
		}

		Mac_CopyMask(toastSrcMap, toastMaskMap, workSrcMap,
				&src, &src, &dest);

		AddRectToBackRects(&dest);
		dest = dinahs[who].whole;
		QOffsetRect(&dest, playOriginH, playOriginV);
		AddRectToWorkRects(&dest);
	}
}

//--------------------------------------------------------------  RenderBalloon

void RenderBalloon (SInt16 who)
{
	Rect		src, dest;

	if (dinahs[who].moving)
	{
		dest = dinahs[who].dest;
		QOffsetRect(&dest, playOriginH, playOriginV);
		src = balloonSrc[dinahs[who].frame];

		Mac_CopyMask(balloonSrcMap, balloonMaskMap, workSrcMap,
				&src, &src, &dest);

		AddRectToBackRects(&dest);
		dest = dinahs[who].whole;
		QOffsetRect(&dest, playOriginH, playOriginV);
		AddRectToWorkRects(&dest);
	}
}

//--------------------------------------------------------------  RenderCopter

void RenderCopter (SInt16 who)
{
	Rect		src, dest;

	if (dinahs[who].moving)
	{
		dest = dinahs[who].dest;
		QOffsetRect(&dest, playOriginH, playOriginV);
		src = copterSrc[dinahs[who].frame];

		Mac_CopyMask(copterSrcMap, copterMaskMap, workSrcMap,
				&src, &src, &dest);

		AddRectToBackRects(&dest);
		dest = dinahs[who].whole;
		QOffsetRect(&dest, playOriginH, playOriginV);
		AddRectToWorkRects(&dest);
	}
}

//--------------------------------------------------------------  RenderDart

void RenderDart (SInt16 who)
{
	Rect		src, dest;

	if (dinahs[who].moving)
	{
		dest = dinahs[who].dest;
		QOffsetRect(&dest, playOriginH, playOriginV);
		src = dartSrc[dinahs[who].frame];

		Mac_CopyMask(dartSrcMap, dartMaskMap, workSrcMap,
				&src, &src, &dest);

		AddRectToBackRects(&dest);
		dest = dinahs[who].whole;
		QOffsetRect(&dest, playOriginH, playOriginV);
		AddRectToWorkRects(&dest);
	}
}

//--------------------------------------------------------------  RenderBall

void RenderBall (SInt16 who)
{
	Rect		src, dest;

	dest = dinahs[who].dest;
	QOffsetRect(&dest, playOriginH, playOriginV);
	src = ballSrc[dinahs[who].frame];

	Mac_CopyMask(ballSrcMap, ballMaskMap, workSrcMap,
			&src, &src, &dest);

	AddRectToBackRects(&dest);
	dest = dinahs[who].whole;
	QOffsetRect(&dest, playOriginH, playOriginV);
	AddRectToWorkRects(&dest);
}

//--------------------------------------------------------------  RenderDrip

void RenderDrip (SInt16 who)
{
	Rect		src, dest;

	dest = dinahs[who].dest;
	QOffsetRect(&dest, playOriginH, playOriginV);
	src = dripSrc[dinahs[who].frame];

	Mac_CopyMask(dripSrcMap, dripMaskMap, workSrcMap,
			&src, &src, &dest);

	AddRectToBackRects(&dest);
	dest = dinahs[who].whole;
	QOffsetRect(&dest, playOriginH, playOriginV);
	AddRectToWorkRects(&dest);
}

//--------------------------------------------------------------  RenderFish

void RenderFish (SInt16 who)
{
	Rect		src, dest;

	dest = dinahs[who].dest;
	QOffsetRect(&dest, playOriginH, playOriginV);
	src = fishSrc[dinahs[who].frame];

	if (dinahs[who].moving)
	{
		Mac_CopyMask(fishSrcMap, fishMaskMap, workSrcMap,
				&src, &src, &dest);
		AddRectToBackRects(&dest);
		dest = dinahs[who].whole;
		QOffsetRect(&dest, playOriginH, playOriginV);
		AddRectToWorkRects(&dest);
	}
	else
	{
		Mac_CopyBits(fishSrcMap, workSrcMap,
				&src, &dest, srcCopy, NULL);
		AddRectToBackRects(&dest);
		dest = dinahs[who].whole;
		QOffsetRect(&dest, playOriginH, playOriginV);
		AddRectToWorkRects(&dest);
	}
}

//--------------------------------------------------------------  HandleSparkleObject

// The following handful of functions are called each game frame and handle…
// the movement and state of the various types of dynamic objects.

void HandleSparkleObject (SInt16 who)
{
	Rect		tempRect;

	if (dinahs[who].active)								// is it on?
	{
		if (dinahs[who].frame <= 0)						// is it idle?
		{												// it is idle
			dinahs[who].timer--;
			if (dinahs[who].timer <= 0)
			{
				dinahs[who].timer = RandomInt(240) + 60;// reset timer
				dinahs[who].frame = kNumSparkleModes;	// time to sparkle
				tempRect = dinahs[who].dest;
				AddSparkle(&tempRect);
				PlayPrioritySound(kMysticSound, kMysticPriority);
			}
		}
		else											// it's sparkling
			dinahs[who].frame--;
	}
	else
	{
	}
}

//--------------------------------------------------------------  HandleToast

void HandleToast (SInt16 who)
{
	Rect		dest;

	if (dinahs[who].moving)
	{
		if (evenFrame)
		{
			dinahs[who].frame++;
			if (dinahs[who].frame >= kNumBreadPicts)
				dinahs[who].frame = 0;
		}
		if (twoPlayerGame)
		{
			if (onePlayerLeft)
			{
				if (playerDead == theGlider.which)
					CheckDynamicCollision(who, &theGlider2, false);
				else
					CheckDynamicCollision(who, &theGlider, false);
			}
			else
			{
				CheckDynamicCollision(who, &theGlider, false);
				CheckDynamicCollision(who, &theGlider2, false);
			}
		}
		else
			CheckDynamicCollision(who, &theGlider, false);
		VOffsetRect(&dinahs[who].dest, dinahs[who].vVel);
		dinahs[who].whole = dinahs[who].dest;
		if (dinahs[who].vVel > 0)
			dinahs[who].whole.top -= dinahs[who].vVel;
		else
			dinahs[who].whole.bottom -= dinahs[who].vVel;
		dinahs[who].vVel++;									// falls
		if (dinahs[who].vVel > dinahs[who].count)
		{
			dest = dinahs[who].whole;
			QOffsetRect(&dest, playOriginH, playOriginV);
			AddRectToWorkRects(&dest);
			dinahs[who].moving = false;
			dinahs[who].frame = dinahs[who].timer;
			PlayPrioritySound(kToastLandSound, kToastLandPriority);
		}
	}
	else
	{
		if (dinahs[who].active)
			dinahs[who].frame--;
		if (dinahs[who].frame <= 0)
		{
			if (dinahs[who].active)
			{
				dinahs[who].vVel = (SInt16)-dinahs[who].count;
				dinahs[who].frame = 0;
				dinahs[who].moving = true;
				PlayPrioritySound(kToastLaunchSound, kToastLaunchPriority);
			}
			else
				dinahs[who].frame = dinahs[who].timer;
		}
	}
}

//--------------------------------------------------------------  HandleMacPlus

void HandleMacPlus (SInt16 who)
{
	if (dinahs[who].timer > 0)
	{
		dinahs[who].timer--;
		if (dinahs[who].active)
		{
			if (dinahs[who].timer == 0)
				AddRectToWorkRects(&dinahs[who].dest);
			else if (dinahs[who].timer == 1)
			{
				PlayPrioritySound(kMacBeepSound, kMacBeepPriority);
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&plusScreen2, &dinahs[who].dest,
						srcCopy, nil);
				AddRectToBackRects(&dinahs[who].dest);
			}
			else if (dinahs[who].timer == 30)
				PlayPrioritySound(kMacOnSound, kMacOnPriority);
		}
		else
		{
			if (dinahs[who].timer == 0)
				AddRectToWorkRects(&dinahs[who].dest);
			else if (dinahs[who].timer == 1)
			{
				PlayPrioritySound(kMacOffSound, kMacOffPriority);
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&plusScreen1, &dinahs[who].dest,
						srcCopy, nil);
				AddRectToBackRects(&dinahs[who].dest);
			}
		}
	}
}

//--------------------------------------------------------------  HandleTV

void HandleTV (SInt16 who)
{
	if (dinahs[who].timer > 0)
	{
		dinahs[who].timer--;
		if (dinahs[who].active)
		{
			if (dinahs[who].timer == 0)
			{
				if ((thisMac.hasQT) && (hasMovie) && (tvInRoom) &&
						(who == tvWithMovieNumber))
				{
				}
				else
				{
					AddRectToWorkRects(&dinahs[who].dest);
				}
			}
			else if (dinahs[who].timer == 1)
			{
				PlayPrioritySound(kTVOnSound, kTVOnPriority);
				if ((thisMac.hasQT) && (hasMovie) && (tvInRoom) &&
						(who == tvWithMovieNumber))
				{
				}
				else
				{
					Mac_CopyBits(applianceSrcMap, backSrcMap,
							&tvScreen2, &dinahs[who].dest,
							srcCopy, nil);
					AddRectToBackRects(&dinahs[who].dest);
				}
			}
		}
		else
		{
			if (dinahs[who].timer == 0)
				AddRectToWorkRects(&dinahs[who].dest);
			else if (dinahs[who].timer == 1)
			{
				PlayPrioritySound(kTVOffSound, kTVOffPriority);
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&tvScreen1, &dinahs[who].dest,
						srcCopy, nil);
				AddRectToBackRects(&dinahs[who].dest);
			}
		}
	}
}

//--------------------------------------------------------------  HandleCoffee

void HandleCoffee (SInt16 who)
{
	if (dinahs[who].timer > 0)
	{
		dinahs[who].timer--;
		if (dinahs[who].active)
		{
			if (dinahs[who].timer == 0)
			{
				AddRectToWorkRects(&dinahs[who].dest);
				dinahs[who].timer = 200 + RandomInt(200);
			}
			else if (dinahs[who].timer == 1)
			{
				PlayPrioritySound(kMacOnSound, kMacOnPriority);
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&coffeeLight2, &dinahs[who].dest,
						srcCopy, nil);
				AddRectToBackRects(&dinahs[who].dest);
			}
			else if (dinahs[who].timer == 100)
			{
				PlayPrioritySound(kCoffeeSound, kCoffeePriority);
				dinahs[who].timer = 200 + RandomInt(200);
			}
		}
		else
		{
			if (dinahs[who].timer == 0)
				AddRectToWorkRects(&dinahs[who].dest);
			else if (dinahs[who].timer == 1)
			{
				PlayPrioritySound(kMacOffSound, kMacOffPriority);
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&coffeeLight1, &dinahs[who].dest,
						srcCopy, nil);
				AddRectToBackRects(&dinahs[who].dest);
			}
		}
	}
}

//--------------------------------------------------------------  HandleOutlet

void HandleOutlet (SInt16 who)
{
	if (dinahs[who].position != 0)
	{
		dinahs[who].timer--;

		if (twoPlayerGame)
		{
			if (onePlayerLeft)
			{
				if (playerDead == theGlider.which)
					CheckDynamicCollision(who, &theGlider2, false);
				else
					CheckDynamicCollision(who, &theGlider, false);
			}
			else
			{
				CheckDynamicCollision(who, &theGlider, true);
				CheckDynamicCollision(who, &theGlider2, true);
			}
		}
		else
			CheckDynamicCollision(who, &theGlider, true);

		if (dinahs[who].timer <= 0)
		{
			dinahs[who].frame = 0;
			dinahs[who].position = 0;
			dinahs[who].timer = dinahs[who].count;
		}
		else
		{
			if ((dinahs[who].timer % 5) == 0)
				PlayPrioritySound(kZapSound, kZapPriority);
			dinahs[who].frame++;
			if (dinahs[who].frame >= kNumOutletPicts)
				dinahs[who].frame = 1;
		}

		if ((dinahs[who].position != 0) || (dinahs[who].hVel > 0))
		{
			Mac_CopyBits(applianceSrcMap, workSrcMap,
					&outletSrc[dinahs[who].frame],
					&dinahs[who].dest,
					srcCopy, nil);
		}
		else
		{
			Mac_PaintRect(workSrcMap, &dinahs[who].dest, GetStockObject(BLACK_BRUSH));
		}
		AddRectToWorkRects(&dinahs[who].dest);
	}
	else
	{
		if (dinahs[who].active)
			dinahs[who].timer--;

		if (dinahs[who].timer <= 0)
		{
			if (dinahs[who].active)
			{
				dinahs[who].position = 1;
				dinahs[who].timer = kLengthOfZap;
				PlayPrioritySound(kZapSound, kZapPriority);
			}
			else
				dinahs[who].timer = dinahs[who].count;
		}
	}
}

//--------------------------------------------------------------  HandleVCR

void HandleVCR (SInt16 who)
{
	if (dinahs[who].timer > 0)
	{
		dinahs[who].timer--;
		if (dinahs[who].active)
		{
			if (dinahs[who].timer == 0)
			{
				AddRectToWorkRects(&dinahs[who].dest);
				dinahs[who].timer = 115;
			}
			else if (dinahs[who].timer == 5)
				PlayPrioritySound(kMacOnSound, kMacOnPriority);
			else if (dinahs[who].timer == 1)
			{
				PlayPrioritySound(kVCRSound, kVCRPriority);
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&vcrTime2, &dinahs[who].dest,
						srcCopy, nil);
				AddRectToBackRects(&dinahs[who].dest);
			}
			else if (dinahs[who].timer == 100)
			{
				AddRectToWorkRects(&dinahs[who].dest);
				dinahs[who].timer = 115;
				dinahs[who].frame = 1 - dinahs[who].frame;
			}
			else if (dinahs[who].timer == 101)
			{
				if (dinahs[who].frame == 0)
				{
					Mac_CopyBits(applianceSrcMap, backSrcMap,
							&vcrTime2, &dinahs[who].dest,
							srcCopy, nil);
					AddRectToBackRects(&dinahs[who].dest);
				}
				else
				{
					Mac_CopyBits(applianceSrcMap, backSrcMap,
							&vcrTime1, &dinahs[who].dest,
							srcCopy, nil);
					AddRectToBackRects(&dinahs[who].dest);
				}
			}
		}
		else
		{
			if (dinahs[who].timer == 0)
				AddRectToWorkRects(&dinahs[who].dest);
			else if (dinahs[who].timer == 1)
			{
				PlayPrioritySound(kMacOffSound, kMacOffPriority);
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&vcrTime1, &dinahs[who].dest,
						srcCopy, nil);
				AddRectToBackRects(&dinahs[who].dest);
			}
		}
	}
}

//--------------------------------------------------------------  HandleStereo

void HandleStereo (SInt16 who)
{
	if (dinahs[who].timer > 0)
	{
		dinahs[who].timer--;
		if (dinahs[who].active)
		{
			if (dinahs[who].timer == 0)
			{
				AddRectToWorkRects(&dinahs[who].dest);
				ToggleMusicWhilePlaying();
			}
			else if (dinahs[who].timer == 1)
			{
				PlayPrioritySound(kMacOnSound, kMacOnPriority);
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&stereoLight2, &dinahs[who].dest,
						srcCopy, nil);
				AddRectToBackRects(&dinahs[who].dest);
			}
		}
		else
		{
			if (dinahs[who].timer == 0)
			{
				AddRectToWorkRects(&dinahs[who].dest);
				ToggleMusicWhilePlaying();
			}
			else if (dinahs[who].timer == 1)
			{
				PlayPrioritySound(kMacOffSound, kMacOffPriority);
				Mac_CopyBits(applianceSrcMap, backSrcMap,
						&stereoLight1, &dinahs[who].dest,
						srcCopy, nil);
				AddRectToBackRects(&dinahs[who].dest);
			}
		}
	}
}

//--------------------------------------------------------------  HandleMicrowave

void HandleMicrowave (SInt16 who)
{
	Rect		dest;

	if (dinahs[who].timer > 0)
	{
		dinahs[who].timer--;
		if (dinahs[who].active)
		{
			if (dinahs[who].timer == 0)
				AddRectToWorkRects(&dinahs[who].dest);
			else if (dinahs[who].timer == 1)
			{
				PlayPrioritySound(kMacOnSound, kMacOnPriority);
				dest = dinahs[who].dest;
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
				AddRectToBackRects(&dinahs[who].dest);
			}
		}
		else
		{
			if (dinahs[who].timer == 0)
				AddRectToWorkRects(&dinahs[who].dest);
			else if (dinahs[who].timer == 1)
			{
				PlayPrioritySound(kMacOffSound, kMacOffPriority);
				dest = dinahs[who].dest;
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
				AddRectToBackRects(&dinahs[who].dest);
			}
		}
	}
}

//--------------------------------------------------------------  HandleBalloon

void HandleBalloon (SInt16 who)
{
	Rect		dest;

	if (dinahs[who].moving)
	{
		if (dinahs[who].vVel < 0)
		{
			if (evenFrame)
			{
				dinahs[who].frame++;
				if (dinahs[who].frame >= 6)
					dinahs[who].frame = 0;
			}
			if (twoPlayerGame)
			{
				if (onePlayerLeft)
				{
					if (playerDead == theGlider.which)
						CheckDynamicCollision(who, &theGlider2, false);
					else
						CheckDynamicCollision(who, &theGlider, false);
				}
				else
				{
					CheckDynamicCollision(who, &theGlider, false);
					CheckDynamicCollision(who, &theGlider2, false);
				}
			}
			else
				CheckDynamicCollision(who, &theGlider, false);

			if ((numBands > 0) && (DidBandHitDynamic(who)))
			{
				dinahs[who].frame = 6;
				dinahs[who].vVel = kEnemyDropSpeed;
				PlayPrioritySound(kPopSound, kPopPriority);
			}
			else
			{
				VOffsetRect(&dinahs[who].dest, dinahs[who].vVel);
				dinahs[who].whole = dinahs[who].dest;
				dinahs[who].whole.bottom -= dinahs[who].vVel;
			}
		}
		else
		{
			if (evenFrame)
			{
				dinahs[who].frame++;
				if (dinahs[who].frame >= 8)
					dinahs[who].frame = 6;
			}

			VOffsetRect(&dinahs[who].dest, dinahs[who].vVel);
			dinahs[who].whole = dinahs[who].dest;
			dinahs[who].whole.top -= dinahs[who].vVel;
		}

		if ((dinahs[who].dest.top <= kBalloonStop) ||
				(dinahs[who].dest.bottom >= kBalloonStart))
		{
			dest = dinahs[who].whole;
			QOffsetRect(&dest, playOriginH, playOriginV);
			AddRectToWorkRects(&dest);
			dest = dinahs[who].dest;
			AddSparkle(&dest);
			PlayPrioritySound(kEnemyOutSound, kEnemyOutPriority);
			dinahs[who].moving = false;
			dinahs[who].vVel = -2;
			dinahs[who].timer = dinahs[who].count;
			dinahs[who].dest.bottom = kBalloonStart;
			dinahs[who].dest.top = dinahs[who].dest.bottom -
					RectTall(&balloonSrc[0]);
			dinahs[who].whole = dinahs[who].dest;
		}
	}
	else		// balloon is idle, waiting to appear
	{
		if (dinahs[who].active)
		{
			dinahs[who].timer--;
			if (dinahs[who].timer <= 0)
			{
				dinahs[who].moving = true;
				if (dinahs[who].count < kStartSparkle)
				{
					dest = dinahs[who].dest;
					AddSparkle(&dest);
					PlayPrioritySound(kEnemyInSound, kEnemyInPriority);
				}
			}
			else if (dinahs[who].timer == kStartSparkle)
			{
				dest = dinahs[who].dest;
				AddSparkle(&dest);
				PlayPrioritySound(kEnemyInSound, kEnemyInPriority);
			}
		}
	}
}

//--------------------------------------------------------------  HandleCopter

void HandleCopter (SInt16 who)
{
	Rect		dest;

	if (dinahs[who].moving)					// is 'copter about?
	{
		if (dinahs[who].hVel != 0)			// 'copter was not shot
		{
			dinahs[who].frame++;
			if (dinahs[who].frame >= 8)
				dinahs[who].frame = 0;
			if (twoPlayerGame)
			{
				if (onePlayerLeft)
				{
					if (playerDead == theGlider.which)
						CheckDynamicCollision(who, &theGlider2, false);
					else
						CheckDynamicCollision(who, &theGlider, false);
				}
				else
				{
					CheckDynamicCollision(who, &theGlider, false);
					CheckDynamicCollision(who, &theGlider2, false);
				}
			}
			else
				CheckDynamicCollision(who, &theGlider, false);
			if ((numBands > 0) && (DidBandHitDynamic(who)))
			{
				dinahs[who].frame = 8;
				dinahs[who].hVel = 0;
				dinahs[who].vVel = kEnemyDropSpeed;
				PlayPrioritySound(kPaperCrunchSound, kPaperCrunchPriority);
			}
			else
			{
				HOffsetRect(&dinahs[who].dest, dinahs[who].hVel);
				VOffsetRect(&dinahs[who].dest, dinahs[who].vVel);
				dinahs[who].whole = dinahs[who].dest;
				dinahs[who].whole.top -= dinahs[who].vVel;
				if (dinahs[who].hVel < 0)
					dinahs[who].whole.right -= dinahs[who].hVel;
				else
					dinahs[who].whole.left -= dinahs[who].hVel;
			}
		}
		else								// 'copter was shot
		{
			dinahs[who].frame++;
			if (dinahs[who].frame >= 10)
				dinahs[who].frame = 8;
			VOffsetRect(&dinahs[who].dest, dinahs[who].vVel);
			dinahs[who].whole = dinahs[who].dest;
			dinahs[who].whole.top -= dinahs[who].vVel;
		}

		if ((dinahs[who].dest.top <= kCopterStart) ||
				(dinahs[who].dest.bottom >= kCopterStop))
		{
			dest = dinahs[who].whole;
			QOffsetRect(&dest, playOriginH, playOriginV);
			AddRectToWorkRects(&dest);
			dest = dinahs[who].dest;
			AddSparkle(&dest);
			PlayPrioritySound(kEnemyOutSound, kEnemyOutPriority);
			dinahs[who].moving = false;
			dinahs[who].vVel = 2;
			if (dinahs[who].type == kCopterLf)
				dinahs[who].hVel = -1;
			else
				dinahs[who].hVel = 1;
			dinahs[who].timer = dinahs[who].count;
			dinahs[who].dest.top = kCopterStart;
			dinahs[who].dest.bottom = dinahs[who].dest.top +
					RectTall(&copterSrc[0]);
			dinahs[who].dest.left = dinahs[who].position;
			dinahs[who].dest.right = dinahs[who].dest.left + 32;
			dinahs[who].whole = dinahs[who].dest;
		}
	}
	else
	{
		if (dinahs[who].active)
		{
			dinahs[who].timer--;
			if (dinahs[who].timer <= 0)
			{
				dinahs[who].moving = true;
				if (dinahs[who].count < kStartSparkle)
				{
					dest = dinahs[who].dest;
					AddSparkle(&dest);
					PlayPrioritySound(kEnemyInSound, kEnemyInPriority);
				}
			}
			else if (dinahs[who].timer == kStartSparkle)
			{
				dest = dinahs[who].dest;
				AddSparkle(&dest);
				PlayPrioritySound(kEnemyInSound, kEnemyInPriority);
			}
		}
	}
}

//--------------------------------------------------------------  HandleDart

void HandleDart (SInt16 who)
{
	Rect		dest;

	if (dinahs[who].moving)					// Dart has appeared
	{
		if (dinahs[who].hVel != 0)			// meaning it isn't falling
		{
			if (twoPlayerGame)
			{
				if (onePlayerLeft)
				{
					if (playerDead == theGlider.which)
						CheckDynamicCollision(who, &theGlider2, false);
					else
						CheckDynamicCollision(who, &theGlider, false);
				}
				else
				{
					CheckDynamicCollision(who, &theGlider, false);
					CheckDynamicCollision(who, &theGlider2, false);
				}
			}
			else
				CheckDynamicCollision(who, &theGlider, false);
			if ((numBands > 0) && (DidBandHitDynamic(who)))
			{
				if (dinahs[who].type == kDartLf)
					dinahs[who].frame = 1;
				else
					dinahs[who].frame = 3;
				dinahs[who].hVel = 0;
				dinahs[who].vVel = kEnemyDropSpeed;
				PlayPrioritySound(kPaperCrunchSound, kPaperCrunchPriority);
			}
			else
			{
				HOffsetRect(&dinahs[who].dest, dinahs[who].hVel);
				VOffsetRect(&dinahs[who].dest, dinahs[who].vVel);
				dinahs[who].whole = dinahs[who].dest;
				dinahs[who].whole.top -= dinahs[who].vVel;
				if (dinahs[who].hVel < 0)
					dinahs[who].whole.right -= dinahs[who].hVel;
				else
					dinahs[who].whole.left -= dinahs[who].hVel;
			}
		}
		else							// dart is falling straight down
		{
			VOffsetRect(&dinahs[who].dest, dinahs[who].vVel);
			dinahs[who].whole = dinahs[who].dest;
			dinahs[who].whole.top -= dinahs[who].vVel;
		}

		if ((dinahs[who].dest.left <= 0) ||
				(dinahs[who].dest.right >= kRoomWide) ||
				(dinahs[who].dest.bottom >= kDartStop))
		{
			dest = dinahs[who].whole;
			QOffsetRect(&dest, playOriginH, playOriginV);
			AddRectToWorkRects(&dest);
			dest = dinahs[who].dest;
			AddSparkle(&dest);
			PlayPrioritySound(kEnemyOutSound, kEnemyOutPriority);
			dinahs[who].moving = false;
			dinahs[who].vVel = 2;
			if (dinahs[who].type == kDartLf)
			{
				dinahs[who].frame = 0;
				dinahs[who].hVel = -kDartVelocity;
				dinahs[who].dest.right = kRoomWide;
				dinahs[who].dest.left = dinahs[who].dest.right -
						RectWide(&dartSrc[0]);
			}
			else
			{
				dinahs[who].frame = 2;
				dinahs[who].hVel = kDartVelocity;
				dinahs[who].dest.left = 0;
				dinahs[who].dest.right = dinahs[who].dest.left +
						RectWide(&dartSrc[0]);
			}
			dinahs[who].timer = dinahs[who].count;
			dinahs[who].dest.top = dinahs[who].position;
			dinahs[who].dest.bottom = dinahs[who].dest.top +
					RectTall(&dartSrc[0]);
			dinahs[who].whole = dinahs[who].dest;
		}
	}
	else
	{
		if (dinahs[who].active)
		{
			dinahs[who].timer--;
			if (dinahs[who].timer <= 0)
			{
				dinahs[who].moving = true;
				if (dinahs[who].count < kStartSparkle)
				{
					dest = dinahs[who].dest;
					AddSparkle(&dest);
					PlayPrioritySound(kEnemyInSound, kEnemyInPriority);
				}
			}
			else if (dinahs[who].timer == kStartSparkle)
			{
				dest = dinahs[who].dest;
				AddSparkle(&dest);
				PlayPrioritySound(kEnemyInSound, kEnemyInPriority);
			}
		}
	}
}

//--------------------------------------------------------------  HandleBall

void HandleBall (SInt16 who)
{
	if (twoPlayerGame)
	{
		if (onePlayerLeft)
		{
			if (playerDead == theGlider.which)
				CheckDynamicCollision(who, &theGlider2, false);
			else
				CheckDynamicCollision(who, &theGlider, false);
		}
		else
		{
			CheckDynamicCollision(who, &theGlider, false);
			CheckDynamicCollision(who, &theGlider2, false);
		}
	}
	else
		CheckDynamicCollision(who, &theGlider, false);

	if (dinahs[who].moving)										// is ball bouncing?
	{
		VOffsetRect(&dinahs[who].dest, dinahs[who].vVel);
		if (dinahs[who].dest.bottom >= dinahs[who].position)	// bounce!
		{
			dinahs[who].whole = dinahs[who].dest;
			dinahs[who].whole.top -= dinahs[who].vVel;
			dinahs[who].whole.bottom = dinahs[who].position;
			dinahs[who].dest.bottom = dinahs[who].position;
			dinahs[who].dest.top = dinahs[who].dest.bottom - 32;
			if (dinahs[who].active)
				dinahs[who].vVel = dinahs[who].count;
			else
			{
				dinahs[who].vVel = -((dinahs[who].vVel * 3) / 4);
				if (dinahs[who].vVel == 0)
					dinahs[who].moving = false;					// stop bounce
			}
			if (dinahs[who].whole.bottom < dinahs[who].dest.bottom)
				dinahs[who].whole.bottom = dinahs[who].dest.bottom;
			PlayPrioritySound(kBounceSound, kBouncePriority);
			if (dinahs[who].moving)
				dinahs[who].frame = 1;
		}
		else
		{
			dinahs[who].whole = dinahs[who].dest;
			if (dinahs[who].vVel > 0)
				dinahs[who].whole.top -= dinahs[who].vVel;
			else
				dinahs[who].whole.bottom -= dinahs[who].vVel;
			if (evenFrame)
				dinahs[who].vVel++;
			dinahs[who].frame = 0;
		}
	}
	else
	{
		if (dinahs[who].active)
		{
			dinahs[who].vVel = dinahs[who].count;
			dinahs[who].moving = true;
			evenFrame = true;
		}
	}
}

//--------------------------------------------------------------  HandleDrip

void HandleDrip (SInt16 who)
{
	Rect		dest;

	if (dinahs[who].moving)
	{
		if (evenFrame)
			dinahs[who].frame = 9 - dinahs[who].frame;
		if (twoPlayerGame)
		{
			if (onePlayerLeft)
			{
				if (playerDead == theGlider.which)
					CheckDynamicCollision(who, &theGlider2, false);
				else
					CheckDynamicCollision(who, &theGlider, false);
			}
			else
			{
				CheckDynamicCollision(who, &theGlider, false);
				CheckDynamicCollision(who, &theGlider2, false);
			}
		}
		else
			CheckDynamicCollision(who, &theGlider, false);

		VOffsetRect(&dinahs[who].dest, dinahs[who].vVel);
		if (dinahs[who].dest.bottom >= dinahs[who].position)
		{
			dest = dinahs[who].whole;
			QOffsetRect(&dest, playOriginH, playOriginV);
			AddRectToWorkRects(&dest);
			dinahs[who].dest.top = dinahs[who].hVel;
			dinahs[who].dest.bottom = dinahs[who].dest.top + 12;
			PlayPrioritySound(kDropSound, kDropPriority);
			dinahs[who].vVel = 0;
			dinahs[who].timer = dinahs[who].count;
			dinahs[who].frame = 3;
			dinahs[who].moving = false;
		}
		else
		{
			dinahs[who].whole = dinahs[who].dest;
			dinahs[who].whole.top -= dinahs[who].vVel;
			if (evenFrame)
				dinahs[who].vVel++;
		}
	}
	else
	{
		if (dinahs[who].active)
		{
			dinahs[who].timer--;

			if (dinahs[who].timer == 6)
				dinahs[who].frame = 0;
			else if (dinahs[who].timer == 4)
				dinahs[who].frame = 1;
			else if (dinahs[who].timer == 2)
				dinahs[who].frame = 2;
			else if (dinahs[who].timer <= 0)
			{
				VOffsetRect(&dinahs[who].dest, 3);
				dinahs[who].whole = dinahs[who].dest;
				dinahs[who].moving = true;
				dinahs[who].frame = 4;
				PlayPrioritySound(kDripSound, kDripPriority);
			}
		}
	}
}

//--------------------------------------------------------------  HandleFish

void HandleFish (SInt16 who)
{
	Rect		dest;

	if (dinahs[who].moving)										// fish leaping
	{
		if ((dinahs[who].vVel >= 0) && (dinahs[who].frame < 7))
			dinahs[who].frame++;
		if (twoPlayerGame)
		{
			if (onePlayerLeft)
			{
				if (playerDead == theGlider.which)
					CheckDynamicCollision(who, &theGlider2, false);
				else
					CheckDynamicCollision(who, &theGlider, false);
			}
			else
			{
				CheckDynamicCollision(who, &theGlider, false);
				CheckDynamicCollision(who, &theGlider2, false);
			}
		}
		else
			CheckDynamicCollision(who, &theGlider, false);

		VOffsetRect(&dinahs[who].dest, dinahs[who].vVel);
		if (dinahs[who].dest.bottom >= dinahs[who].position)	// splash down
		{
			dest = dinahs[who].whole;
			QOffsetRect(&dest, playOriginH, playOriginV);
			AddRectToWorkRects(&dest);
			dinahs[who].dest.bottom = dinahs[who].position;
			dinahs[who].dest.top = dinahs[who].dest.bottom - 16;
			dinahs[who].whole = dinahs[who].dest;
			dinahs[who].whole.top -= 2;
			PlayPrioritySound(kDropSound, kDropPriority);
			dinahs[who].vVel = dinahs[who].count;
			dinahs[who].timer = dinahs[who].hVel;
			dinahs[who].frame = 0;
			dinahs[who].moving = false;
			PlayPrioritySound(kFishInSound, kFishInPriority);
		}
		else
		{
			dinahs[who].whole = dinahs[who].dest;
			if (dinahs[who].vVel > 0)
				dinahs[who].whole.top -= dinahs[who].vVel;
			else
				dinahs[who].whole.bottom -= dinahs[who].vVel;
			if (evenFrame)
				dinahs[who].vVel++;
		}
	}
	else														// fish idle
	{
		dinahs[who].whole = dinahs[who].dest;
		if ((dinahs[who].timer & 0x0003) == 0x0003)
		{
			dinahs[who].frame++;
			if (dinahs[who].frame > 3)
				dinahs[who].frame = 0;
			if ((dinahs[who].frame == 1) || (dinahs[who].frame == 2))
			{
				dinahs[who].dest.top++;
				dinahs[who].dest.bottom++;
				dinahs[who].whole.bottom++;
			}
			else
			{
				dinahs[who].dest.top--;
				dinahs[who].dest.bottom--;
				dinahs[who].whole.top--;
			}
		}
		if (dinahs[who].active)
		{
			dinahs[who].timer--;
			if (dinahs[who].timer <= 0)								// fish leaps
			{
				dinahs[who].whole = dinahs[who].dest;
				dinahs[who].moving = true;
				dinahs[who].frame = 4;
				PlayPrioritySound(kFishOutSound, kFishOutPriority);
			}
		}
	}
}

//--------------------------------------------------------------  HandleDynamics

// This is the master function that calls all the specific handlers above.

void HandleDynamics (void)
{
	SInt16		i;

	for (i = 0; i < numDynamics; i++)
	{
		switch (dinahs[i].type)
		{
			case kSparkle:
			HandleSparkleObject(i);
			break;

			case kToaster:
			HandleToast(i);
			break;

			case kMacPlus:
			HandleMacPlus(i);
			break;

			case kTV:
			HandleTV(i);
			break;

			case kCoffee:
			HandleCoffee(i);
			break;

			case kOutlet:
			HandleOutlet(i);
			break;

			case kVCR:
			HandleVCR(i);
			break;

			case kStereo:
			HandleStereo(i);
			break;

			case kMicrowave:
			HandleMicrowave(i);
			break;

			case kBalloon:
			HandleBalloon(i);
			break;

			case kCopterLf:
			case kCopterRt:
			HandleCopter(i);
			break;

			case kDartLf:
			case kDartRt:
			HandleDart(i);
			break;

			case kBall:
			HandleBall(i);
			break;

			case kDrip:
			HandleDrip(i);
			break;

			case kFish:
			HandleFish(i);
			break;

			default:
			break;
		}
	}
}

//--------------------------------------------------------------  HandleDynamics

// This is the master function that calls all the various rendering handlers…
// above.

void RenderDynamics (void)
{
	SInt16		i;

	for (i = 0; i < numDynamics; i++)
	{
		switch (dinahs[i].type)
		{
			case kToaster:
			RenderToast(i);
			break;

			case kBalloon:
			RenderBalloon(i);
			break;

			case kCopterLf:
			case kCopterRt:
			RenderCopter(i);
			break;

			case kDartLf:
			case kDartRt:
			RenderDart(i);
			break;

			case kBall:
			RenderBall(i);
			break;

			case kDrip:
			RenderDrip(i);
			break;

			case kFish:
			RenderFish(i);
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
	SInt16		i;

	for (i = 0; i < kMaxDynamicObs; i++)
	{
		dinahs[i].type = kObjectIsEmpty;
		QSetRect(&dinahs[i].dest, 0, 0, 0, 0);
		QSetRect(&dinahs[i].whole, 0, 0, 0, 0);
		dinahs[i].hVel = 0;
		dinahs[i].vVel = 0;
		dinahs[i].count = 0;
		dinahs[i].frame = 0;
		dinahs[i].timer = 0;
		dinahs[i].position = 0;
		dinahs[i].room = 0;
		dinahs[i].byte0 = 0;
		dinahs[i].active = false;
	}
	numDynamics = 0;
}

//--------------------------------------------------------------  AddDynamicObject

// When a room is being drawn, various dynamic objects are pointed here.
// This function sets up the structures to handle them.

SInt16 AddDynamicObject (SInt16 what, const Rect *where, const objectType *who,
		SInt16 room, SInt16 index, Boolean isOn)
{
	SInt16		position, velocity;
	Boolean		lilFrame;

	if (numDynamics >= kMaxDynamicObs)
		return (-1);

	dinahs[numDynamics].type = what;
	switch (what)
	{
		case kSparkle:
		dinahs[numDynamics].dest = sparkleSrc[0];
		ZeroRectCorner(&dinahs[numDynamics].dest);
		QOffsetRect(&dinahs[numDynamics].dest, where->left, where->top);
		dinahs[numDynamics].whole = dinahs[numDynamics].dest;
		dinahs[numDynamics].hVel = 0;
		dinahs[numDynamics].vVel = 0;
		dinahs[numDynamics].count = 0;
		dinahs[numDynamics].frame = 0;
		dinahs[numDynamics].timer = RandomInt(60) + 15;
		dinahs[numDynamics].position = 0;
		dinahs[numDynamics].room = room;
		dinahs[numDynamics].byte0 = (Byte)index;
		dinahs[numDynamics].byte1 = 0;
		dinahs[numDynamics].moving = false;
		dinahs[numDynamics].active = isOn;
		break;

		case kToaster:
		dinahs[numDynamics].dest = breadSrc[0];
		CenterRectInRect(&dinahs[numDynamics].dest, where);
		VOffsetRect(&dinahs[numDynamics].dest,
				where->top - dinahs[numDynamics].dest.top);
		dinahs[numDynamics].whole = dinahs[numDynamics].dest;
		dinahs[numDynamics].hVel = where->top + 2;	// hVel used as clip
		position = who->data.g.height;				// reverse engineer init. vel.
		velocity = 0;
		do
		{
			velocity++;
			position -= velocity;
		}
		while (position > 0);
		dinahs[numDynamics].vVel = -velocity;
		dinahs[numDynamics].count = velocity;		// count = initial velocity
		dinahs[numDynamics].frame = (SInt16)who->data.g.delay * 3;
		dinahs[numDynamics].timer = dinahs[numDynamics].frame;
		dinahs[numDynamics].position = 0;			// launch/idle state
		dinahs[numDynamics].room = room;
		dinahs[numDynamics].byte0 = (Byte)index;
		dinahs[numDynamics].byte1 = 0;
		dinahs[numDynamics].moving = false;
		dinahs[numDynamics].active = isOn;
		break;

		case kMacPlus:
		dinahs[numDynamics].dest = plusScreen1;
		ZeroRectCorner(&dinahs[numDynamics].dest);
		QOffsetRect(&dinahs[numDynamics].dest,
				where->left + playOriginH + 10,
				where->top + playOriginV + 7);
		dinahs[numDynamics].whole = dinahs[numDynamics].dest;
		dinahs[numDynamics].hVel = 0;
		dinahs[numDynamics].vVel = 0;
		dinahs[numDynamics].count = 0;
		dinahs[numDynamics].frame = 0;
		dinahs[numDynamics].timer = 0;
		dinahs[numDynamics].position = 0;
		dinahs[numDynamics].room = room;
		dinahs[numDynamics].byte0 = (Byte)index;
		dinahs[numDynamics].byte1 = 0;
		dinahs[numDynamics].moving = false;
		dinahs[numDynamics].active = isOn;
		break;

		case kTV:
		dinahs[numDynamics].dest = tvScreen1;
		ZeroRectCorner(&dinahs[numDynamics].dest);
		QOffsetRect(&dinahs[numDynamics].dest,
				where->left + playOriginH + 17,
				where->top + playOriginV + 10);
		dinahs[numDynamics].whole = dinahs[numDynamics].dest;
		dinahs[numDynamics].hVel = 0;
		dinahs[numDynamics].vVel = 0;
		dinahs[numDynamics].count = 0;
		dinahs[numDynamics].frame = 0;
		dinahs[numDynamics].timer = 0;
		dinahs[numDynamics].position = 0;
		dinahs[numDynamics].room = room;
		dinahs[numDynamics].byte0 = (Byte)index;
		dinahs[numDynamics].byte1 = 0;
		dinahs[numDynamics].moving = false;
		dinahs[numDynamics].active = isOn;
		break;

		case kCoffee:
		dinahs[numDynamics].dest = coffeeLight1;
		ZeroRectCorner(&dinahs[numDynamics].dest);
		QOffsetRect(&dinahs[numDynamics].dest,
				where->left + playOriginH + 32,
				where->top + playOriginV + 57);
		dinahs[numDynamics].whole = dinahs[numDynamics].dest;
		dinahs[numDynamics].hVel = 0;
		dinahs[numDynamics].vVel = 0;
		dinahs[numDynamics].count = 0;
		dinahs[numDynamics].frame = 0;
		if (isOn)
			dinahs[numDynamics].timer = 200;
		else
			dinahs[numDynamics].timer = 0;
		dinahs[numDynamics].position = 0;
		dinahs[numDynamics].room = room;
		dinahs[numDynamics].byte0 = (Byte)index;
		dinahs[numDynamics].byte1 = 0;
		dinahs[numDynamics].moving = false;
		dinahs[numDynamics].active = isOn;
		break;

		case kOutlet:
		dinahs[numDynamics].dest = outletSrc[0];
		ZeroRectCorner(&dinahs[numDynamics].dest);
		QOffsetRect(&dinahs[numDynamics].dest,
				where->left + playOriginH,
				where->top + playOriginV);
		dinahs[numDynamics].whole = dinahs[numDynamics].dest;
		dinahs[numDynamics].hVel = numLights;
		dinahs[numDynamics].vVel = 0;
		dinahs[numDynamics].count = ((SInt16)who->data.g.delay * 6) / kTicksPerFrame;
		dinahs[numDynamics].frame = 0;
		dinahs[numDynamics].timer = dinahs[numDynamics].count;
		dinahs[numDynamics].position = 0;			// launch/idle state
		dinahs[numDynamics].room = room;
		dinahs[numDynamics].byte0 = (Byte)index;
		dinahs[numDynamics].byte1 = 0;
		dinahs[numDynamics].moving = false;
		dinahs[numDynamics].active = isOn;
		break;

		case kVCR:
		dinahs[numDynamics].dest = vcrTime1;
		ZeroRectCorner(&dinahs[numDynamics].dest);
		QOffsetRect(&dinahs[numDynamics].dest,
				where->left + playOriginH + 64,
				where->top + playOriginV + 6);
		dinahs[numDynamics].whole = dinahs[numDynamics].dest;
		dinahs[numDynamics].hVel = 0;
		dinahs[numDynamics].vVel = 0;
		dinahs[numDynamics].count = 0;
		dinahs[numDynamics].frame = 0;
		if (isOn)
			dinahs[numDynamics].timer = 115;
		else
			dinahs[numDynamics].timer = 0;
		dinahs[numDynamics].position = 0;
		dinahs[numDynamics].room = room;
		dinahs[numDynamics].byte0 = (Byte)index;
		dinahs[numDynamics].byte1 = 0;
		dinahs[numDynamics].moving = false;
		dinahs[numDynamics].active = isOn;
		break;

		case kStereo:
		dinahs[numDynamics].dest = stereoLight1;
		ZeroRectCorner(&dinahs[numDynamics].dest);
		QOffsetRect(&dinahs[numDynamics].dest,
				where->left + playOriginH + 56,
				where->top + playOriginV + 20);
		dinahs[numDynamics].whole = dinahs[numDynamics].dest;
		dinahs[numDynamics].hVel = 0;
		dinahs[numDynamics].vVel = 0;
		dinahs[numDynamics].count = 0;
		dinahs[numDynamics].frame = 0;
		dinahs[numDynamics].timer = 0;
		dinahs[numDynamics].position = 0;
		dinahs[numDynamics].room = room;
		dinahs[numDynamics].byte0 = (Byte)index;
		dinahs[numDynamics].byte1 = 0;
		dinahs[numDynamics].moving = false;
		dinahs[numDynamics].active = isOn;
		break;

		case kMicrowave:
		dinahs[numDynamics].dest = microOn;
		ZeroRectCorner(&dinahs[numDynamics].dest);
		QOffsetRect(&dinahs[numDynamics].dest,
				where->left + playOriginH + 14,
				where->top + playOriginV + 13);
		dinahs[numDynamics].dest.right = dinahs[numDynamics].dest.left + 48;
		dinahs[numDynamics].whole = dinahs[numDynamics].dest;
		dinahs[numDynamics].hVel = 0;
		dinahs[numDynamics].vVel = 0;
		dinahs[numDynamics].count = 0;
		dinahs[numDynamics].frame = 0;
		dinahs[numDynamics].timer = 0;
		dinahs[numDynamics].position = 0;
		dinahs[numDynamics].room = room;
		dinahs[numDynamics].byte0 = (Byte)index;
		dinahs[numDynamics].byte1 = 0;
		dinahs[numDynamics].moving = false;
		dinahs[numDynamics].active = isOn;
		break;

		case kBalloon:
		dinahs[numDynamics].dest = balloonSrc[0];
		ZeroRectCorner(&dinahs[numDynamics].dest);
		QOffsetRect(&dinahs[numDynamics].dest, where->left, 0);
		dinahs[numDynamics].dest.bottom = kBalloonStart;
		dinahs[numDynamics].dest.top = dinahs[numDynamics].dest.bottom -
				RectTall(&balloonSrc[0]);
		dinahs[numDynamics].whole = dinahs[numDynamics].dest;
		dinahs[numDynamics].hVel = 0;
		dinahs[numDynamics].vVel = -2;
		dinahs[numDynamics].count = ((SInt16)who->data.h.delay * 6) / kTicksPerFrame;
		dinahs[numDynamics].frame = 0;
		dinahs[numDynamics].timer = dinahs[numDynamics].count;
		dinahs[numDynamics].position = 0;
		dinahs[numDynamics].room = room;
		dinahs[numDynamics].byte0 = (Byte)index;
		dinahs[numDynamics].byte1 = 0;
		dinahs[numDynamics].moving = false;
		dinahs[numDynamics].active = isOn;			// initially idle
		break;

		case kCopterLf:
		case kCopterRt:
		dinahs[numDynamics].dest = copterSrc[0];
		ZeroRectCorner(&dinahs[numDynamics].dest);
		QOffsetRect(&dinahs[numDynamics].dest, where->left, 0);
		dinahs[numDynamics].dest.top = kCopterStart;
		dinahs[numDynamics].dest.bottom = dinahs[numDynamics].dest.top +
				RectTall(&copterSrc[0]);
		dinahs[numDynamics].whole = dinahs[numDynamics].dest;
		if (what == kCopterLf)
			dinahs[numDynamics].hVel = -1;
		else
			dinahs[numDynamics].hVel = 1;
		dinahs[numDynamics].vVel = 2;
		dinahs[numDynamics].count = ((SInt16)who->data.h.delay * 6) / kTicksPerFrame;
		dinahs[numDynamics].frame = 0;
		dinahs[numDynamics].timer = dinahs[numDynamics].count;
		dinahs[numDynamics].position = dinahs[numDynamics].dest.left;
		dinahs[numDynamics].room = room;
		dinahs[numDynamics].byte0 = (Byte)index;
		dinahs[numDynamics].byte1 = 0;
		dinahs[numDynamics].moving = false;
		dinahs[numDynamics].active = isOn;			// initially idle
		break;

		case kDartLf:
		case kDartRt:
		dinahs[numDynamics].dest = dartSrc[0];
		ZeroRectCorner(&dinahs[numDynamics].dest);
		if (what == kDartLf)
		{
			QOffsetRect(&dinahs[numDynamics].dest,
					kRoomWide - RectWide(&dartSrc[0]), where->top);
			dinahs[numDynamics].hVel = -kDartVelocity;
			dinahs[numDynamics].frame = 0;
		}
		else
		{
			QOffsetRect(&dinahs[numDynamics].dest, 0, where->top);
			dinahs[numDynamics].hVel = kDartVelocity;
			dinahs[numDynamics].frame = 2;
		}
		dinahs[numDynamics].whole = dinahs[numDynamics].dest;
		dinahs[numDynamics].vVel = 2;
		dinahs[numDynamics].count = ((SInt16)who->data.h.delay * 6) / kTicksPerFrame;
		dinahs[numDynamics].timer = dinahs[numDynamics].count;
		dinahs[numDynamics].position = dinahs[numDynamics].dest.top;
		dinahs[numDynamics].byte0 = (Byte)index;
		dinahs[numDynamics].byte1 = 0;
		dinahs[numDynamics].moving = false;
		dinahs[numDynamics].active = isOn;			// initially idle
		break;

		case kBall:
		dinahs[numDynamics].dest = ballSrc[0];
		ZeroRectCorner(&dinahs[numDynamics].dest);
		QOffsetRect(&dinahs[numDynamics].dest,
				where->left, where->top);
		dinahs[numDynamics].whole = dinahs[numDynamics].dest;
		dinahs[numDynamics].hVel = 0;
		position = who->data.h.length;			// reverse engineer init. vel.
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
		dinahs[numDynamics].vVel = -velocity;
		dinahs[numDynamics].moving = false;
		dinahs[numDynamics].count = -velocity;	// count = initial velocity
		dinahs[numDynamics].frame = 0;
		dinahs[numDynamics].timer = 0;
		dinahs[numDynamics].position = dinahs[numDynamics].dest.bottom;
		dinahs[numDynamics].room = room;
		dinahs[numDynamics].byte0 = (Byte)index;
		dinahs[numDynamics].byte1 = 0;
		dinahs[numDynamics].active = isOn;
		break;

		case kDrip:
		dinahs[numDynamics].dest = dripSrc[0];
		CenterRectInRect(&dinahs[numDynamics].dest, where);
		VOffsetRect(&dinahs[numDynamics].dest,
				where->top - dinahs[numDynamics].dest.top);
		dinahs[numDynamics].whole = dinahs[numDynamics].dest;
		dinahs[numDynamics].hVel = dinahs[numDynamics].dest.top;	// remember
		dinahs[numDynamics].vVel = 0;
		dinahs[numDynamics].count = ((SInt16)who->data.h.delay * 6) / kTicksPerFrame;
		dinahs[numDynamics].frame = 3;
		dinahs[numDynamics].timer = dinahs[numDynamics].count;
		dinahs[numDynamics].position = dinahs[numDynamics].dest.top +
				who->data.h.length;
		dinahs[numDynamics].room = room;
		dinahs[numDynamics].byte0 = (Byte)index;
		dinahs[numDynamics].byte1 = 0;
		dinahs[numDynamics].moving = false;
		dinahs[numDynamics].active = isOn;
		break;

		case kFish:
		dinahs[numDynamics].dest = fishSrc[0];
		QOffsetRect(&dinahs[numDynamics].dest,
				where->left + 10, where->top + 8);
		dinahs[numDynamics].whole = dinahs[numDynamics].dest;
		dinahs[numDynamics].hVel = ((SInt16)who->data.h.delay * 6) / kTicksPerFrame;
		position = who->data.g.height;			// reverse engineer init. vel.
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
		dinahs[numDynamics].vVel = -velocity;
		dinahs[numDynamics].count = -velocity;	// count = initial velocity
		dinahs[numDynamics].frame = 0;
		dinahs[numDynamics].timer = dinahs[numDynamics].hVel;
		dinahs[numDynamics].position = dinahs[numDynamics].dest.bottom;
		dinahs[numDynamics].room = room;
		dinahs[numDynamics].byte0 = (Byte)index;
		dinahs[numDynamics].byte1 = 0;
		dinahs[numDynamics].moving = false;
		dinahs[numDynamics].active = isOn;
		break;

		default:
		return (-1);
		break;
	}

	numDynamics++;

	return (numDynamics - 1);
}

