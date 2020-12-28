#include "Modes.h"

//============================================================================
//----------------------------------------------------------------------------
//                                  Modes.c
//----------------------------------------------------------------------------
//============================================================================

#include "House.h"
#include "MainWindow.h"
#include "ObjectRects.h"
#include "Objects.h"
#include "Play.h"
#include "Player.h"
#include "RectUtils.h"
#include "Render.h"
#include "Room.h"
#include "Scoreboard.h"
#include "Sound.h"
#include "Transit.h"

void FlagGliderFaceLeft (gliderPtr thisGlider);
void FlagGliderFaceRight (gliderPtr thisGlider);

SInt16 g_saidFollow;

//==============================================================  Functions
//--------------------------------------------------------------  StartGliderFadingIn

void StartGliderFadingIn (gliderPtr thisGlider)
{
	if (g_foilTotal <= 0)
		g_showFoil = false;

	thisGlider->mode = kGliderFadingIn;
	thisGlider->whole = thisGlider->dest;
	thisGlider->frame = 0;
	thisGlider->dontDraw = false;
	if (thisGlider->facing == kFaceLeft)
	{
		thisGlider->src =
				g_gliderSrc[g_fadeInSequence[thisGlider->frame] + kLeftFadeOffset];
		thisGlider->mask =
				g_gliderSrc[g_fadeInSequence[thisGlider->frame] + kLeftFadeOffset];
	}
	else
	{
		thisGlider->src = g_gliderSrc[g_fadeInSequence[thisGlider->frame]];
		thisGlider->mask = g_gliderSrc[g_fadeInSequence[thisGlider->frame]];
	}
}

//--------------------------------------------------------------  StartGliderTransportingIn

void StartGliderTransportingIn (gliderPtr thisGlider)
{
	if (g_foilTotal <= 0)
		g_showFoil = false;

	thisGlider->mode = kGliderTransportingIn;
	thisGlider->whole = thisGlider->dest;
	thisGlider->frame = 0;
	thisGlider->dontDraw = false;
	if (thisGlider->facing == kFaceLeft)
	{
		thisGlider->src =
				g_gliderSrc[g_fadeInSequence[thisGlider->frame] + kLeftFadeOffset];
		thisGlider->mask =
				g_gliderSrc[g_fadeInSequence[thisGlider->frame] + kLeftFadeOffset];
	}
	else
	{
		thisGlider->src = g_gliderSrc[g_fadeInSequence[thisGlider->frame]];
		thisGlider->mask = g_gliderSrc[g_fadeInSequence[thisGlider->frame]];
	}
}

//--------------------------------------------------------------  StartGliderFadingOut

void StartGliderFadingOut (gliderPtr thisGlider)
{
	Rect		tempBounds;

	if (thisGlider->mode == kGliderFadingOut)
		return;

	if (thisGlider->mode == kGliderGoingFoil)
		DeckGliderInFoil(thisGlider);
	else if (thisGlider->mode == kGliderLosingFoil)
		RemoveFoilFromGlider(thisGlider);

	if (RectTall(&thisGlider->dest) > kGliderHigh)
	{
		tempBounds = thisGlider->dest;
		QOffsetRect(&tempBounds, g_playOriginH, g_playOriginV);
		AddRectToWorkRects(&tempBounds);
		if (g_hasMirror)
		{
			tempBounds = thisGlider->dest;
			QOffsetRect(&tempBounds, g_playOriginH, g_playOriginV);
			QOffsetRect(&tempBounds, kReflectionOffsetH, KReflectionOffsetV);
			AddRectToWorkRects(&tempBounds);
		}
		thisGlider->dest.right = thisGlider->dest.left + kGliderWide;
		thisGlider->dest.top = thisGlider->dest.bottom - kGliderHigh;
	}
	thisGlider->mode = kGliderFadingOut;
	thisGlider->whole = thisGlider->dest;
	thisGlider->frame = kLastFadeSequence - 1;
	if (thisGlider->facing == kFaceLeft)
	{
		thisGlider->src =
				g_gliderSrc[g_fadeInSequence[thisGlider->frame] + kLeftFadeOffset];
		thisGlider->mask =
				g_gliderSrc[g_fadeInSequence[thisGlider->frame] + kLeftFadeOffset];
	}
	else
	{
		thisGlider->src = g_gliderSrc[g_fadeInSequence[thisGlider->frame]];
		thisGlider->mask = g_gliderSrc[g_fadeInSequence[thisGlider->frame]];
	}
}

//--------------------------------------------------------------  StartGliderGoingUpStairs

void StartGliderGoingUpStairs (gliderPtr thisGlider)
{
	if (thisGlider->mode == kGliderGoingFoil)
		DeckGliderInFoil(thisGlider);
	else if (thisGlider->mode == kGliderLosingFoil)
		RemoveFoilFromGlider(thisGlider);

	if (thisGlider->mode == kGliderBurning)
		thisGlider->frame = kWasBurning;
	else
		thisGlider->frame = 0;

	thisGlider->mode = kGliderGoingUp;
}

//--------------------------------------------------------------  StartGliderGoingDownStairs

void StartGliderGoingDownStairs (gliderPtr thisGlider)
{
	if (thisGlider->mode == kGliderGoingFoil)
		DeckGliderInFoil(thisGlider);
	else if (thisGlider->mode == kGliderLosingFoil)
		RemoveFoilFromGlider(thisGlider);

	if (thisGlider->mode == kGliderBurning)
		thisGlider->frame = kWasBurning;
	else
		thisGlider->frame = 0;

	thisGlider->mode = kGliderGoingDown;
	g_rightClip = GetUpStairsRightEdge();
}

//--------------------------------------------------------------  StartGliderMailingIn

void StartGliderMailingIn (gliderPtr thisGlider, const Rect *bounds, const hotObject *who)
{
	SInt16		topSought, whoLinked;
	Byte		objLinked;

	PlayPrioritySound(kTransOutSound, kTransOutPriority);

	whoLinked = who->who;
	g_transRoom = g_masterObjects[whoLinked].roomLink;
	objLinked = (Byte)g_masterObjects[whoLinked].objectLink;
	g_linkedToWhat = WhatAreWeLinkedTo(g_transRoom, objLinked);

	GetObjectRect(&g_thisHouse.rooms[g_transRoom].objects[objLinked], &g_transRect);

	thisGlider->frame = 0;
	thisGlider->clip = *bounds;
	topSought = bounds->bottom - RectTall(&thisGlider->dest);
	thisGlider->clip.top = topSought;
}

//--------------------------------------------------------------  StartGliderMailingOut

void StartGliderMailingOut (gliderPtr thisGlider)
{
	if (thisGlider->mode == kGliderGoingFoil)
		DeckGliderInFoil(thisGlider);
	else if (thisGlider->mode == kGliderLosingFoil)
		RemoveFoilFromGlider(thisGlider);

	if (g_linkedToWhat == kLinkedToLeftMailbox)
	{
		thisGlider->facing = kFaceLeft;
		thisGlider->mode = kGliderMailOutLeft;
		thisGlider->src = g_gliderSrc[2];
		thisGlider->mask = g_gliderSrc[2];
	}
	else
	{
		thisGlider->facing = kFaceRight;
		thisGlider->mode = kGliderMailOutRight;
		thisGlider->src = g_gliderSrc[0];
		thisGlider->mask = g_gliderSrc[0];
	}

	thisGlider->hVel = 0;
	thisGlider->vVel = 0;
	thisGlider->hDesiredVel = 0;
	thisGlider->vDesiredVel = 0;
	thisGlider->tipped = false;
	thisGlider->dontDraw = false;
}

//--------------------------------------------------------------  StartGliderDuctingDown

void StartGliderDuctingDown (gliderPtr thisGlider, const Rect *bounds, const hotObject *who)
{
	SInt16		leftSought, whoLinked;
	Byte		objLinked;

	PlayPrioritySound(kTransOutSound, kTransOutPriority);

	if (thisGlider->mode == kGliderGoingFoil)
		DeckGliderInFoil(thisGlider);
	else if (thisGlider->mode == kGliderLosingFoil)
		RemoveFoilFromGlider(thisGlider);

	whoLinked = who->who;
	g_transRoom = g_masterObjects[whoLinked].roomLink;
	objLinked = (Byte)g_masterObjects[whoLinked].objectLink;
	g_linkedToWhat = WhatAreWeLinkedTo(g_transRoom, objLinked);

	GetObjectRect(&g_thisHouse.rooms[g_transRoom].objects[objLinked], &g_transRect);

	thisGlider->frame = 0;
	thisGlider->clip = *bounds;
	leftSought = bounds->left + ((RectWide(bounds) - kGliderWide) / 2);
	thisGlider->clip.left = leftSought;

	thisGlider->mode = kGliderDuctingDown;
}

//--------------------------------------------------------------  StartGliderDuctingUp

void StartGliderDuctingUp (gliderPtr thisGlider, const Rect *bounds, const hotObject *who)
{
	SInt16		leftSought, whoLinked;
	Byte		objLinked;

	PlayPrioritySound(kTransOutSound, kTransOutPriority);

	if (thisGlider->mode == kGliderGoingFoil)
		DeckGliderInFoil(thisGlider);
	else if (thisGlider->mode == kGliderLosingFoil)
		RemoveFoilFromGlider(thisGlider);

	whoLinked = who->who;
	g_transRoom = g_masterObjects[whoLinked].roomLink;
	objLinked = (Byte)g_masterObjects[whoLinked].objectLink;
	g_linkedToWhat = WhatAreWeLinkedTo(g_transRoom, objLinked);

	GetObjectRect(&g_thisHouse.rooms[g_transRoom].objects[objLinked], &g_transRect);

	thisGlider->frame = 0;
	thisGlider->clip = *bounds;
	leftSought = bounds->left + ((RectWide(bounds) - kGliderWide) / 2);
	thisGlider->clip.left = leftSought;

	thisGlider->mode = kGliderDuctingUp;
}

//--------------------------------------------------------------  StartGliderDuctingIn

void StartGliderDuctingIn (gliderPtr thisGlider)
{
	thisGlider->mode = kGliderDuctingIn;
	thisGlider->whole = thisGlider->dest;
	thisGlider->dontDraw = false;
}

//--------------------------------------------------------------  StartGliderTransporting

void StartGliderTransporting (gliderPtr thisGlider, const hotObject *who)
{
	SInt16		whoLinked;
	Byte		objLinked;

	PlayPrioritySound(kTransOutSound, kTransOutPriority);

	if (thisGlider->mode == kGliderGoingFoil)
		DeckGliderInFoil(thisGlider);
	else if (thisGlider->mode == kGliderLosingFoil)
		RemoveFoilFromGlider(thisGlider);

	whoLinked = who->who;
	g_transRoom = g_masterObjects[whoLinked].roomLink;
	objLinked = (Byte)g_masterObjects[whoLinked].objectLink;
	g_linkedToWhat = WhatAreWeLinkedTo(g_transRoom, objLinked);

	GetObjectRect(&g_thisHouse.rooms[g_transRoom].objects[objLinked], &g_transRect);

	thisGlider->dest.right = thisGlider->dest.left + kGliderWide;
	thisGlider->dest.bottom = thisGlider->dest.top + kGliderHigh;
	thisGlider->destShadow.right = thisGlider->destShadow.left + kGliderWide;
	thisGlider->destShadow.bottom = thisGlider->destShadow.top + kShadowHigh;
	thisGlider->mode = kGliderTransporting;
	thisGlider->whole = thisGlider->dest;
	thisGlider->frame = kLastFadeSequence - 1;
	if (thisGlider->facing == kFaceLeft)
	{
		thisGlider->src =
				g_gliderSrc[g_fadeInSequence[thisGlider->frame] + kLeftFadeOffset];
		thisGlider->mask =
				g_gliderSrc[g_fadeInSequence[thisGlider->frame] + kLeftFadeOffset];
	}
	else
	{
		thisGlider->src = g_gliderSrc[g_fadeInSequence[thisGlider->frame]];
		thisGlider->mask = g_gliderSrc[g_fadeInSequence[thisGlider->frame]];
	}
}

//--------------------------------------------------------------  FlagGliderNormal

void FlagGliderNormal (gliderPtr thisGlider)
{
	thisGlider->dest.right = thisGlider->dest.left + kGliderWide;
	thisGlider->dest.bottom = thisGlider->dest.top + kGliderHigh;
	thisGlider->destShadow.right = thisGlider->destShadow.left + kGliderWide;
	thisGlider->destShadow.bottom = thisGlider->destShadow.top + kShadowHigh;
	thisGlider->mode = kGliderNormal;
	if (thisGlider->facing == kFaceLeft)
	{
		thisGlider->src = g_gliderSrc[2];
		thisGlider->mask = g_gliderSrc[2];
	}
	else
	{
		thisGlider->src = g_gliderSrc[0];
		thisGlider->mask = g_gliderSrc[0];
	}
	thisGlider->hVel = 0;
	thisGlider->vVel = 0;
	thisGlider->hDesiredVel = 0;
	thisGlider->vDesiredVel = 0;
	thisGlider->tipped = false;
	thisGlider->ignoreLeft = false;
	thisGlider->ignoreRight = false;
	thisGlider->ignoreGround = false;
	thisGlider->dontDraw = false;
	thisGlider->frame = 0;
	g_shadowVisible = IsShadowVisible();
}

//--------------------------------------------------------------  FlagGliderShredding

void FlagGliderShredding (gliderPtr thisGlider, const Rect *bounds)
{
	PlayPrioritySound(kCaughtFireSound, kCaughtFirePriority);
	thisGlider->dest.left = bounds->left + 36;
	thisGlider->dest.right = thisGlider->dest.left + kGliderWide;
	thisGlider->dest.bottom = thisGlider->dest.top + kGliderHigh;
	if (thisGlider->dest.left > thisGlider->whole.left)
	{
		thisGlider->whole.right = thisGlider->dest.right;
		thisGlider->wholeShadow.right = thisGlider->dest.right;
	}
	else
	{
		thisGlider->whole.left = thisGlider->dest.left;
		thisGlider->wholeShadow.left = thisGlider->dest.left;
	}
	thisGlider->destShadow.left = thisGlider->dest.left;
	thisGlider->destShadow.right = thisGlider->destShadow.left + kGliderWide;
	thisGlider->destShadow.bottom = thisGlider->destShadow.top + kShadowHigh;
	thisGlider->mode = kGliderShredding;
	if (thisGlider->facing == kFaceLeft)
	{
		thisGlider->src = g_gliderSrc[2];
		thisGlider->mask = g_gliderSrc[2];
	}
	else
	{
		thisGlider->src = g_gliderSrc[0];
		thisGlider->mask = g_gliderSrc[0];
	}
	thisGlider->hVel = 0;
	thisGlider->vVel = 0;
	thisGlider->hDesiredVel = 0;
	thisGlider->vDesiredVel = 0;
	thisGlider->frame = bounds->bottom - 3;
	thisGlider->tipped = false;
}

//--------------------------------------------------------------  FlagGliderBurning

void FlagGliderBurning (gliderPtr thisGlider)
{
	#define		kFramesToBurn	60

	PlayPrioritySound(kCaughtFireSound, kCaughtFirePriority);

	thisGlider->dest.right = thisGlider->dest.left + kGliderWide;
	thisGlider->dest.top = thisGlider->dest.bottom - kGliderBurningHigh;
	thisGlider->destShadow.right = thisGlider->destShadow.left + kGliderWide;
	thisGlider->destShadow.bottom = thisGlider->destShadow.top + kShadowHigh;
	thisGlider->mode = kGliderBurning;
	if (thisGlider->facing == kFaceLeft)
	{
		thisGlider->src = g_gliderSrc[25];
		thisGlider->mask = g_gliderSrc[25];
	}
	else
	{
		thisGlider->src = g_gliderSrc[21];
		thisGlider->mask = g_gliderSrc[21];
	}
	thisGlider->hVel = 0;
	thisGlider->vVel = 0;
	thisGlider->hDesiredVel = 0;
	thisGlider->vDesiredVel = 0;
	thisGlider->frame = 0;
	thisGlider->wasMode = kFramesToBurn;		// used to count down burning
	thisGlider->tipped = false;
}

//--------------------------------------------------------------  FlagGliderFaceLeft

void FlagGliderFaceLeft (gliderPtr thisGlider)
{
	thisGlider->mode = kGliderFaceLeft;
	thisGlider->frame = kLastAboutFaceFrame;
	thisGlider->src = g_gliderSrc[kLastAboutFaceFrame];
	thisGlider->mask = g_gliderSrc[kLastAboutFaceFrame];
}

//--------------------------------------------------------------  FlagGliderFaceRight

void FlagGliderFaceRight (gliderPtr thisGlider)
{
	thisGlider->mode = kGliderFaceRight;
	thisGlider->frame = kFirstAboutFaceFrame;
	thisGlider->src = g_gliderSrc[kFirstAboutFaceFrame];
	thisGlider->mask = g_gliderSrc[kFirstAboutFaceFrame];
}

//--------------------------------------------------------------  FlagGliderInLimbo

void FlagGliderInLimbo (gliderPtr thisGlider, Boolean sayIt)
{
	thisGlider->wasMode = thisGlider->mode;
	thisGlider->mode = kGliderInLimbo;
	if ((sayIt) && (g_saidFollow < 3))
	{
		PlayPrioritySound(kFollowSound, kFollowPriority);
		g_saidFollow++;
	}
	g_firstPlayer = thisGlider->which;
}

//--------------------------------------------------------------  UndoGliderLimbo

void UndoGliderLimbo (gliderPtr thisGlider)
{
	if ((g_twoPlayerGame) && (g_onePlayerLeft) && (thisGlider->which == g_playerDead))
		return;

	if (thisGlider->mode == kGliderInLimbo)
		thisGlider->mode = thisGlider->wasMode;
	thisGlider->dontDraw = false;
}

//--------------------------------------------------------------  ToggleGliderFacing

void ToggleGliderFacing (gliderPtr thisGlider)
{
	if (thisGlider->mode != kGliderNormal)
		return;

	if (thisGlider->facing == kFaceLeft)
		FlagGliderFaceRight(thisGlider);
	else
		FlagGliderFaceLeft(thisGlider);
}

//--------------------------------------------------------------  InsureGliderFacingRight

void InsureGliderFacingRight (gliderPtr thisGlider)
{
	if ((g_twoPlayerGame) && (g_onePlayerLeft) && (thisGlider->which == g_playerDead))
		return;

	if ((thisGlider->facing == kFaceLeft) && (thisGlider->mode != kGliderBurning))
		FlagGliderFaceRight(thisGlider);
}

//--------------------------------------------------------------  InsureGliderFacingLeft

void InsureGliderFacingLeft (gliderPtr thisGlider)
{
	if ((g_twoPlayerGame) && (g_onePlayerLeft) && (thisGlider->which == g_playerDead))
		return;

	if ((thisGlider->facing == kFaceRight) && (thisGlider->mode != kGliderBurning))
		FlagGliderFaceLeft(thisGlider);
}

//--------------------------------------------------------------  ReadyGliderForTripUpStairs

void ReadyGliderForTripUpStairs (gliderPtr thisGlider)
{
	#define kVGliderAppearsComingUp		100

	if ((g_twoPlayerGame) && (thisGlider->which == g_playerDead) && (g_onePlayerLeft))
		return;

	thisGlider->facing = kFaceLeft;
	thisGlider->mode = kGliderComingUp;
	thisGlider->src = g_gliderSrc[2];
	thisGlider->mask = g_gliderSrc[2];
	thisGlider->hVel = 0;
	thisGlider->vVel = 0;
	thisGlider->hDesiredVel = 0;
	thisGlider->vDesiredVel = 0;
	thisGlider->tipped = false;

	g_rightClip = GetUpStairsRightEdge();
	thisGlider->dest = thisGlider->src;
	ZeroRectCorner(&thisGlider->dest);
	QOffsetRect(&thisGlider->dest, g_rightClip, kVGliderAppearsComingUp);
	thisGlider->whole = thisGlider->dest;
	thisGlider->destShadow.left = thisGlider->dest.left;
	thisGlider->destShadow.right = thisGlider->dest.right;
	thisGlider->wholeShadow = thisGlider->destShadow;

	FinishGliderUpStairs(thisGlider);
}

//--------------------------------------------------------------  ReadyGliderForTripDownStairs

void ReadyGliderForTripDownStairs (gliderPtr thisGlider)
{
	#define kVGliderAppearsComingDown		100

	if ((g_twoPlayerGame) && (thisGlider->which == g_playerDead) && (g_onePlayerLeft))
		return;

	thisGlider->facing = kFaceRight;
	thisGlider->mode = kGliderComingDown;
	thisGlider->src = g_gliderSrc[0];
	thisGlider->mask = g_gliderSrc[0];
	thisGlider->hVel = 0;
	thisGlider->vVel = 0;
	thisGlider->hDesiredVel = 0;
	thisGlider->vDesiredVel = 0;
	thisGlider->tipped = false;

	g_leftClip = GetDownStairsLeftEdge();
	thisGlider->dest = thisGlider->src;
	ZeroRectCorner(&thisGlider->dest);
	QOffsetRect(&thisGlider->dest, g_leftClip - kGliderWide, kVGliderAppearsComingDown);
	thisGlider->whole = thisGlider->dest;
	thisGlider->destShadow.left = thisGlider->dest.left;
	thisGlider->destShadow.right = thisGlider->dest.right;
	thisGlider->wholeShadow = thisGlider->destShadow;

	FinishGliderDownStairs(thisGlider);
}

//--------------------------------------------------------------  StartGliderFoilGoing

void StartGliderFoilGoing (gliderPtr thisGlider)
{
	if ((thisGlider->mode == kGliderGoingFoil) || (thisGlider->mode == kGliderInLimbo))
		return;

	QuickFoilRefresh(false);

	thisGlider->mode = kGliderGoingFoil;
	thisGlider->whole = thisGlider->dest;
	thisGlider->frame = 0;
	if (thisGlider->facing == kFaceLeft)
	{
		thisGlider->src = g_gliderSrc[(10 - thisGlider->frame) + kLeftFadeOffset];
		thisGlider->mask = g_gliderSrc[(10 - thisGlider->frame) + kLeftFadeOffset];
	}
	else
	{
		thisGlider->src = g_gliderSrc[10 - thisGlider->frame];
		thisGlider->mask = g_gliderSrc[10 - thisGlider->frame];
	}
}

//--------------------------------------------------------------  StartGliderFoilLosing

void StartGliderFoilLosing (gliderPtr thisGlider)
{
	if ((thisGlider->mode == kGliderLosingFoil) ||
			(thisGlider->mode == kGliderInLimbo))
		return;

	QuickFoilRefresh(false);
	PlayPrioritySound(kFizzleSound, kFizzlePriority);

	thisGlider->mode = kGliderLosingFoil;
	thisGlider->whole = thisGlider->dest;
	thisGlider->frame = 0;
	if (thisGlider->facing == kFaceLeft)
	{
		thisGlider->src = g_gliderSrc[(10 - thisGlider->frame) + kLeftFadeOffset];
		thisGlider->mask = g_gliderSrc[(10 - thisGlider->frame) + kLeftFadeOffset];
	}
	else
	{
		thisGlider->src = g_gliderSrc[10 - thisGlider->frame];
		thisGlider->mask = g_gliderSrc[10 - thisGlider->frame];
	}
}

//--------------------------------------------------------------  TagGliderIdle

void TagGliderIdle (gliderPtr thisGlider)
{
	if ((g_twoPlayerGame) && (g_onePlayerLeft) && (thisGlider->which == g_playerDead))
		return;

	thisGlider->wasMode = thisGlider->mode;
	thisGlider->mode = kGliderIdle;
	thisGlider->hVel = 30;			// used for 30 frame delay
}
