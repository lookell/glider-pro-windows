#include "Transit.h"

//============================================================================
//----------------------------------------------------------------------------
//                                 Transit.c
//----------------------------------------------------------------------------
//============================================================================

#include "Environ.h"
#include "House.h"
#include "HouseIO.h"
#include "Interactions.h"
#include "Modes.h"
#include "Music.h"
#include "Play.h"
#include "Player.h"
#include "RectUtils.h"
#include "Render.h"
#include "Room.h"
#include "RoomGraphics.h"
#include "Scoreboard.h"
#include "Sound.h"
#include "Transitions.h"

void ReadyGliderFromTransit (gliderPtr thisGlider, SInt16 toWhat);
void HandleRoomVisitation (void);

SInt16 g_linkedToWhat;
Boolean g_takingTheStairs;
Boolean g_firstPlayer;

//==============================================================  Functions
//--------------------------------------------------------------  WhatAreWeLinkedTo

SInt16 WhatAreWeLinkedTo (SInt16 where, Byte who)
{
	SInt16 what;
	SInt16 whatType;

	if (where < 0 || where >= g_thisHouse.nRooms)
	{
		return kLinkedToOther;
	}
	if (who < 0 || who >= kMaxRoomObs)
	{
		return kLinkedToOther;
	}

	what = g_thisHouse.rooms[where].objects[who].what;

	switch (what)
	{
		case kMailboxLf:
		whatType = kLinkedToLeftMailbox;
		break;

		case kMailboxRt:
		whatType = kLinkedToRightMailbox;
		break;

		case kCeilingTrans:
		whatType = kLinkedToCeilingDuct;
		break;

		default:
		whatType = kLinkedToOther;
		break;
	}

	return (whatType);
}

//--------------------------------------------------------------  ReadyGliderFromTransit

void ReadyGliderFromTransit (gliderPtr thisGlider, SInt16 toWhat)
{
	Rect		tempRect;

	if ((g_twoPlayerGame) && (g_onePlayerLeft) && (thisGlider->which == g_playerDead))
		return;

	FlagGliderNormal(thisGlider);

	switch (toWhat)
	{
		case kLinkedToOther:
		StartGliderTransportingIn(thisGlider);	// set glider's mode
		tempRect = thisGlider->dest;			// position glider
		CenterRectInRect(&tempRect, &g_transRect);
		thisGlider->dest.left = tempRect.left;
		thisGlider->dest.right = tempRect.right;
		thisGlider->dest.top = tempRect.top;
		thisGlider->dest.bottom = tempRect.bottom;
		thisGlider->destShadow.left = tempRect.left;
		thisGlider->destShadow.right = tempRect.right;
		thisGlider->whole = thisGlider->dest;
		thisGlider->wholeShadow = thisGlider->destShadow;
		thisGlider->enteredRect = thisGlider->dest;
		break;

		case kLinkedToLeftMailbox:
		StartGliderMailingOut(thisGlider);
		thisGlider->clip = g_transRect;		// fix clip
		thisGlider->clip.right -= 64;
		thisGlider->clip.bottom -= 25;
		tempRect = thisGlider->dest;
		thisGlider->dest.left = thisGlider->clip.right;
		thisGlider->dest.right = thisGlider->dest.left;
		thisGlider->dest.bottom = thisGlider->clip.bottom - 4;
		thisGlider->dest.top = thisGlider->dest.bottom - RectTall(&tempRect);
		thisGlider->destShadow.left = thisGlider->dest.left;
		thisGlider->destShadow.right = thisGlider->dest.right;
		thisGlider->whole = thisGlider->dest;
		thisGlider->wholeShadow = thisGlider->destShadow;
		break;

		case kLinkedToRightMailbox:
		StartGliderMailingOut(thisGlider);
		thisGlider->clip = g_transRect;		// fix clip
		thisGlider->clip.left += 79;
		thisGlider->clip.bottom -= 25;
		tempRect = thisGlider->dest;
		thisGlider->dest.right = thisGlider->clip.left;
		thisGlider->dest.left = thisGlider->dest.right;
		thisGlider->dest.bottom = thisGlider->clip.bottom - 4;
		thisGlider->dest.top = thisGlider->dest.bottom - RectTall(&tempRect);
		thisGlider->destShadow.left = thisGlider->dest.left;
		thisGlider->destShadow.right = thisGlider->dest.right;
		thisGlider->whole = thisGlider->dest;
		thisGlider->wholeShadow = thisGlider->destShadow;
		break;

		case kLinkedToCeilingDuct:
		StartGliderDuctingIn(thisGlider);
		tempRect = thisGlider->dest;		// set glider's position
		CenterRectInRect(&tempRect, &g_transRect);
		thisGlider->dest.left = tempRect.left;
		thisGlider->dest.right = tempRect.right;
		thisGlider->dest.top = tempRect.top;
		thisGlider->dest.bottom = thisGlider->dest.top;
		QOffsetRect(&thisGlider->dest, 0, -RectTall(&tempRect));
		thisGlider->destShadow.left = tempRect.left;
		thisGlider->destShadow.right = tempRect.right;
		thisGlider->whole = thisGlider->dest;
		thisGlider->wholeShadow = thisGlider->destShadow;
		break;

		case kLinkedToFloorDuct:
		break;

		default:
		break;
	}

	if ((g_twoPlayerGame) && (thisGlider->which != g_firstPlayer))
		TagGliderIdle(thisGlider);
}

//--------------------------------------------------------------  MoveRoomToRoom

void MoveRoomToRoom (gliderPtr thisGlider, SInt16 where)
{
	Rect		enterRect;

	HandleRoomVisitation();
	switch (where)
	{
		case kToRight:
		SetMusicalMode(kProdGameScoreMode);
		if (g_twoPlayerGame)
		{
			UndoGliderLimbo(&g_theGlider);
			UndoGliderLimbo(&g_theGlider2);
			InsureGliderFacingRight(&g_theGlider);
			InsureGliderFacingRight(&g_theGlider2);
		}
		else
			InsureGliderFacingRight(thisGlider);
		ForceThisRoom(g_localNumbers[kEastRoom]);
		if (g_twoPlayerGame)
		{
			OffsetGlider(&g_theGlider, kToLeft);
			OffsetGlider(&g_theGlider2, kToLeft);
			QSetRect(&enterRect, 0, 0, 48, 20);
			QOffsetRect(&enterRect, 0,
					kGliderStartsDown + (SInt16)g_thisRoom->leftStart - 2);
			g_theGlider.enteredRect = enterRect;
			g_theGlider2.enteredRect = enterRect;
		}
		else
		{
			OffsetGlider(thisGlider, kToLeft);
			QSetRect(&enterRect, 0, 0, 48, 20);
			QOffsetRect(&enterRect, 0,
					kGliderStartsDown + (SInt16)g_thisRoom->leftStart - 2);
			thisGlider->enteredRect = enterRect;
		}
		break;

		case kToLeft:
		SetMusicalMode(kProdGameScoreMode);
		if (g_twoPlayerGame)
		{
			UndoGliderLimbo(&g_theGlider);
			UndoGliderLimbo(&g_theGlider2);
			InsureGliderFacingLeft(&g_theGlider);
			InsureGliderFacingLeft(&g_theGlider2);
		}
		else
			InsureGliderFacingLeft(thisGlider);
		ForceThisRoom(g_localNumbers[kWestRoom]);
		if (g_twoPlayerGame)
		{
			OffsetGlider(&g_theGlider, kToRight);
			OffsetGlider(&g_theGlider2, kToRight);
			QSetRect(&enterRect, 0, 0, 48, 20);
			QOffsetRect(&enterRect, kRoomWide - 48,
					kGliderStartsDown + (SInt16)g_thisRoom->rightStart - 2);
			g_theGlider.enteredRect = enterRect;
			g_theGlider2.enteredRect = enterRect;
		}
		else
		{
			OffsetGlider(thisGlider, kToRight);
			QSetRect(&enterRect, 0, 0, 48, 20);
			QOffsetRect(&enterRect, kRoomWide - 48,
					kGliderStartsDown + (SInt16)g_thisRoom->rightStart - 2);
			thisGlider->enteredRect = enterRect;
		}
		break;

		case kAbove:
		SetMusicalMode(kKickGameScoreMode);
		ForceThisRoom(g_localNumbers[kNorthRoom]);
		if (!g_takingTheStairs)
		{
			if (g_twoPlayerGame)
			{
				UndoGliderLimbo(&g_theGlider);
				UndoGliderLimbo(&g_theGlider2);
				OffsetGlider(&g_theGlider, kBelow);
				OffsetGlider(&g_theGlider2, kBelow);
				g_theGlider.enteredRect = g_theGlider.dest;
				g_theGlider2.enteredRect = g_theGlider2.dest;
			}
			else
			{
				OffsetGlider(thisGlider, kBelow);
				thisGlider->enteredRect = thisGlider->dest;
			}
		}
		else
		{
			if (g_twoPlayerGame)
			{
				ReadyGliderForTripUpStairs(&g_theGlider);
				ReadyGliderForTripUpStairs(&g_theGlider2);
			}
			else
				ReadyGliderForTripUpStairs(thisGlider);
		}
		break;

		case kBelow:
		SetMusicalMode(kKickGameScoreMode);
		ForceThisRoom(g_localNumbers[kSouthRoom]);
		if (!g_takingTheStairs)
		{
			if (g_twoPlayerGame)
			{
				UndoGliderLimbo(&g_theGlider);
				UndoGliderLimbo(&g_theGlider2);
				OffsetGlider(&g_theGlider, kAbove);
				OffsetGlider(&g_theGlider2, kAbove);
				g_theGlider.enteredRect = g_theGlider.dest;
				g_theGlider2.enteredRect = g_theGlider2.dest;
			}
			else
			{
				OffsetGlider(thisGlider, kAbove);
				thisGlider->enteredRect = thisGlider->dest;
			}
		}
		else
		{
			if (g_twoPlayerGame)
			{
				ReadyGliderForTripDownStairs(&g_theGlider);
				ReadyGliderForTripDownStairs(&g_theGlider2);
			}
			else
				ReadyGliderForTripDownStairs(thisGlider);
		}
		break;

		default:
		break;
	}

	if ((g_twoPlayerGame) && (!g_onePlayerLeft))
	{
		if (g_firstPlayer == kPlayer1)
			TagGliderIdle(&g_theGlider2);
		else
			TagGliderIdle(&g_theGlider);
	}

	ReadyLevel();
	RefreshScoreboard(kNormalTitleMode);
	WipeScreenOn(where, &g_justRoomsRect);

#ifdef COMPILEQT
	RenderFrame();
	if ((g_thisMac.hasQT) && (g_hasMovie) && (g_tvInRoom) && (g_tvOn))
	{
		GoToBeginningOfMovie(g_theMovie);
		StartMovie(g_theMovie);
	}
#endif
}

//--------------------------------------------------------------  TransportRoomToRoom

void TransportRoomToRoom (gliderPtr thisGlider)
{
	Boolean		sameRoom;

	SetMusicalMode(kKickGameScoreMode);
	HandleRoomVisitation();

	sameRoom = (g_transRoom == g_thisRoomNumber);
	if (!sameRoom)
		ForceThisRoom(g_transRoom);
	if (g_twoPlayerGame)
	{
		UndoGliderLimbo(&g_theGlider);			// turn off limbo if needed
		UndoGliderLimbo(&g_theGlider2);			// turn off limbo if needed
		ReadyGliderFromTransit(&g_theGlider, g_linkedToWhat);
		ReadyGliderFromTransit(&g_theGlider2, g_linkedToWhat);
	}
	else
		ReadyGliderFromTransit(thisGlider, g_linkedToWhat);

	if (!sameRoom)
		ReadyLevel();
	RefreshScoreboard(kNormalTitleMode);
	if (!sameRoom)
		WipeScreenOn(kAbove, &g_justRoomsRect);

#ifdef COMPILEQT
	RenderFrame();
	if ((g_thisMac.hasQT) && (g_hasMovie) && (g_tvInRoom) && (g_tvOn))
	{
		GoToBeginningOfMovie(g_theMovie);
		StartMovie(g_theMovie);
	}
#endif
}

//--------------------------------------------------------------  MoveDuctToDuct

void MoveDuctToDuct (gliderPtr thisGlider)
{
	Boolean		sameRoom;

	SetMusicalMode(kKickGameScoreMode);
	HandleRoomVisitation();

	sameRoom = (g_transRoom == g_thisRoomNumber);
	if (!sameRoom)
		ForceThisRoom(g_transRoom);

	if (g_twoPlayerGame)
	{
		UndoGliderLimbo(&g_theGlider);			// turn off limbo if needed
		UndoGliderLimbo(&g_theGlider2);			// turn off limbo if needed
		ReadyGliderFromTransit(&g_theGlider, g_linkedToWhat);
		ReadyGliderFromTransit(&g_theGlider2, g_linkedToWhat);
	}
	else
		ReadyGliderFromTransit(thisGlider, g_linkedToWhat);

	if (!sameRoom)
		ReadyLevel();
	RefreshScoreboard(kNormalTitleMode);
	if (!sameRoom)
		WipeScreenOn(kAbove, &g_justRoomsRect);

#ifdef COMPILEQT
	RenderFrame();
	if ((g_thisMac.hasQT) && (g_hasMovie) && (g_tvInRoom) && (g_tvOn))
	{
		GoToBeginningOfMovie(g_theMovie);
		StartMovie(g_theMovie);
	}
#endif
}

//--------------------------------------------------------------  MoveMailToMail

void MoveMailToMail (gliderPtr thisGlider)
{
	Boolean		sameRoom;

	SetMusicalMode(kKickGameScoreMode);
	HandleRoomVisitation();

	sameRoom = (g_transRoom == g_thisRoomNumber);
	if (!sameRoom)
		ForceThisRoom(g_transRoom);

	if (g_twoPlayerGame)
	{
		UndoGliderLimbo(&g_theGlider);			// turn off limbo if needed
		UndoGliderLimbo(&g_theGlider2);			// turn off limbo if needed
		ReadyGliderFromTransit(&g_theGlider, g_linkedToWhat);
		ReadyGliderFromTransit(&g_theGlider2, g_linkedToWhat);
	}
	else
		ReadyGliderFromTransit(thisGlider, g_linkedToWhat);

	if (!sameRoom)
		ReadyLevel();
	RefreshScoreboard(kNormalTitleMode);
	if (!sameRoom)
		WipeScreenOn(kAbove, &g_justRoomsRect);

#ifdef COMPILEQT
	RenderFrame();
	if ((g_thisMac.hasQT) && (g_hasMovie) && (g_tvInRoom) && (g_tvOn))
	{
		GoToBeginningOfMovie(g_theMovie);
		StartMovie(g_theMovie);
	}
#endif
}

//--------------------------------------------------------------  HandleRoomVisitation

void HandleRoomVisitation (void)
{
	if (!g_thisRoom->visited)
	{
		g_thisHouse.rooms[g_localNumbers[kCentralRoom]].visited = true;
		g_theScore += kRoomVisitScore;
		g_thisRoom->visited = true;
	}
}

//--------------------------------------------------------------  ForceKillGlider

void ForceKillGlider (void)
{
	if (g_theGlider.mode == kGliderInLimbo)
	{
		if (g_theGlider2.mode != kGliderFadingOut)
		{
			StartGliderFadingOut(&g_theGlider2);
			PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
			g_playerSuicide = true;
		}
	}
	else if (g_theGlider2.mode == kGliderInLimbo)
	{
		if (g_theGlider.mode != kGliderFadingOut)
		{
			StartGliderFadingOut(&g_theGlider);
			PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
			g_playerSuicide = true;
		}
	}
}

//--------------------------------------------------------------  FollowTheLeader

void FollowTheLeader (void)
{
	SInt16		wasEscaped;
	Boolean		oneOrTwo;

	g_playerSuicide = false;
	wasEscaped = g_otherPlayerEscaped;
	g_otherPlayerEscaped = kNoOneEscaped;

	if (g_theGlider.mode == kGliderInLimbo)
	{
		oneOrTwo = true;
		g_theGlider2.dest = g_theGlider.dest;
		g_theGlider2.destShadow = g_theGlider.destShadow;
		g_theGlider2.whole = g_theGlider2.dest;
		g_theGlider2.wholeShadow = g_theGlider2.destShadow;
	}
	else if (g_theGlider2.mode == kGliderInLimbo)
	{
		oneOrTwo = false;
		g_theGlider.dest = g_theGlider2.dest;
		g_theGlider.destShadow = g_theGlider2.destShadow;
		g_theGlider.whole = g_theGlider.dest;
		g_theGlider.wholeShadow = g_theGlider.destShadow;
	}
	else
	{
		return;
	}

	switch (wasEscaped)
	{
		case kPlayerEscapedUp:
		case kPlayerEscapingUpStairs:
		case kPlayerEscapedUpStairs:
		if (oneOrTwo)
			MoveRoomToRoom(&g_theGlider2, kAbove);
		else
			MoveRoomToRoom(&g_theGlider, kAbove);
		break;

		case kPlayerEscapedDown:
		case kPlayerEscapingDownStairs:
		case kPlayerEscapedDownStairs:
		if (oneOrTwo)
			MoveRoomToRoom(&g_theGlider2, kBelow);
		else
			MoveRoomToRoom(&g_theGlider, kBelow);
		break;

		case kPlayerEscapedLeft:
		if (oneOrTwo)
			MoveRoomToRoom(&g_theGlider2, kToLeft);
		else
			MoveRoomToRoom(&g_theGlider, kToLeft);
		break;

		case kPlayerEscapedRight:
		if (oneOrTwo)
			MoveRoomToRoom(&g_theGlider2, kToRight);
		else
			MoveRoomToRoom(&g_theGlider, kToRight);
		break;

		case kPlayerTransportedOut:
		if (oneOrTwo)
			TransportRoomToRoom(&g_theGlider2);
		else
			TransportRoomToRoom(&g_theGlider);
		break;

		case kPlayerMailedOut:
		if (oneOrTwo)
			MoveMailToMail(&g_theGlider2);
		else
			MoveMailToMail(&g_theGlider);
		break;

		case kPlayerDuckedOut:
		if (oneOrTwo)
			MoveDuctToDuct(&g_theGlider2);
		else
			MoveDuctToDuct(&g_theGlider);
		break;

		default:
		break;
	}
}
