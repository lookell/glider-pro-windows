//============================================================================
//----------------------------------------------------------------------------
//                               Interactions.c
//----------------------------------------------------------------------------
//============================================================================

#include "Interactions.h"

#include "Banner.h"
#include "DynamicMaps.h"
#include "GameOver.h"
#include "Grease.h"
#include "MainWindow.h"
#include "Modes.h"
#include "ObjectDraw.h"
#include "Objects.h"
#include "Play.h"
#include "Player.h"
#include "RectUtils.h"
#include "Render.h"
#include "Room.h"
#include "RoomGraphics.h"
#include "Scoreboard.h"
#include "Sound.h"
#include "Transit.h"
#include "Triggers.h"
#include "Trip.h"

#define kFloorVentLift          (-6)
#define kCeilingVentDrop        8
#define kFanStrength            12
#define kBatterySupply          50      // about 2 rooms worth of thrust
#define kHeliumSupply           150
#define kBandsSupply            8
#define kFoilSupply             8

Boolean GliderHitTop (gliderPtr thisGlider, const Rect *theRect);
Boolean GliderInRect (const gliderType *thisGlider, const Rect *theRect);
void BounceGlider (gliderPtr thisGlider, const Rect *theRect);
void CheckEscapeUpTwo (gliderPtr thisGlider);
void CheckEscapeUp (gliderPtr thisGlider);
void CheckEscapeDownTwo (gliderPtr thisGlider);
void CheckEscapeDown (gliderPtr thisGlider);
void CheckRoofCollision (gliderPtr thisGlider);
void CheckEscapeLeftTwo (gliderPtr thisGlider);
void CheckEscapeLeft (gliderPtr thisGlider);
void CheckEscapeRightTwo (gliderPtr thisGlider);
void CheckEscapeRight (gliderPtr thisGlider);
void CheckGliderInRoom (gliderPtr thisGlider);
void HandleRewards (gliderPtr thisGlider, hotPtr who);
void HandleMicrowaveAction (hotPtr who, gliderPtr thisGlider);
void HandleHotSpotCollision (gliderPtr thisGlider, hotPtr who, SInt16 index);
void CheckForHotSpots (void);
void WebGlider (gliderPtr thisGlider, const Rect *webBounds);

SInt16 g_otherPlayerEscaped;

static SInt16 g_activeRectEscaped;

//==============================================================  Functions
//--------------------------------------------------------------  GliderHitSides

Boolean GliderHitTop (gliderPtr thisGlider, const Rect *theRect)
{
	Rect glideBounds;
	SInt16 offset;
	Boolean hitTop;

	glideBounds.left = thisGlider->dest.left + 5;
	glideBounds.top = thisGlider->dest.top + 5;
	glideBounds.right = thisGlider->dest.right - 5;
	glideBounds.bottom = thisGlider->dest.bottom - 5;

	glideBounds.left -= thisGlider->wasHVel;
	glideBounds.right -= thisGlider->wasHVel;

	if (theRect->bottom < glideBounds.top)
		hitTop = false;
	else if (theRect->top > glideBounds.bottom)
		hitTop = false;
	else if (theRect->right < glideBounds.left)
		hitTop = false;
	else if (theRect->left > glideBounds.right)
		hitTop = false;
	else
		hitTop = true;

	if (!hitTop)
	{
		PlayPrioritySound(kFoilHitSound, kFoilHitPriority);
		g_foilTotal--;
		if (g_foilTotal <= 0)
			StartGliderFoilLosing(thisGlider);

		glideBounds.left += thisGlider->wasHVel;
		glideBounds.right += thisGlider->wasHVel;
		if (thisGlider->hVel > 0)
			offset = 2 + glideBounds.right - theRect->left;
		else
			offset = 2 + glideBounds.left - theRect->right;

		thisGlider->hVel = -thisGlider->hVel - offset;
	}

	return (hitTop);
}

//--------------------------------------------------------------  SectGlider

Boolean SectGlider (const gliderType *thisGlider, const Rect *theRect, Boolean scrutinize)
{
	Rect glideBounds;
	Boolean itHit;

	glideBounds = thisGlider->dest;
	if (thisGlider->mode == kGliderBurning)
		glideBounds.top += 6;

	if (scrutinize)
	{
		glideBounds.left += 5;
		glideBounds.top += 5;
		glideBounds.right -= 5;
		glideBounds.bottom -= 5;
	}

	if (theRect->bottom < glideBounds.top)
		itHit = false;
	else if (theRect->top > glideBounds.bottom)
		itHit = false;
	else if (theRect->right < glideBounds.left)
		itHit = false;
	else if (theRect->left > glideBounds.right)
		itHit = false;
	else
		itHit = true;

	return (itHit);
}

//--------------------------------------------------------------  GliderInRect

Boolean GliderInRect (const gliderType *thisGlider, const Rect *theRect)
{
	Rect glideBounds;

	glideBounds = thisGlider->dest;

	if (glideBounds.top < theRect->top)
		return (false);
	else if (glideBounds.bottom > theRect->bottom)
		return (false);
	else if (glideBounds.left < theRect->left)
		return (false);
	else if (glideBounds.right > theRect->right)
		return (false);
	else
		return (true);
}

//--------------------------------------------------------------  BounceGlider

void BounceGlider (gliderPtr thisGlider, const Rect *theRect)
{
	Rect glideBounds;

	glideBounds = thisGlider->dest;
	if ((theRect->right - glideBounds.left) < (glideBounds.right - theRect->left))
		thisGlider->hVel = theRect->right - glideBounds.left;
	else
		thisGlider->hVel = theRect->left - glideBounds.right;
	if (g_foilTotal > 0)
		PlayPrioritySound(kFoilHitSound, kFoilHitPriority);
	else
		PlayPrioritySound(kHitWallSound, kHitWallPriority);
}

//--------------------------------------------------------------  CheckEscapeUpTwo

void CheckEscapeUpTwo (gliderPtr thisGlider)
{
	SInt16 offset, leftTile, rightTile;

	if (g_topOpen)
	{
		if (thisGlider->dest.top < kNoCeilingLimit)
		{
			if (g_otherPlayerEscaped == kNoOneEscaped)
			{
				g_otherPlayerEscaped = kPlayerEscapedUp;
				RefreshScoreboard(kEscapedTitleMode);
				FlagGliderInLimbo(thisGlider, true);
			}
			else if (g_otherPlayerEscaped == kPlayerEscapedUp)
			{
				g_otherPlayerEscaped = kNoOneEscaped;
				MoveRoomToRoom(thisGlider, kAbove);
			}
			else
			{
				PlayPrioritySound(kDontExitSound, kDontExitPriority);
				offset = kNoCeilingLimit - thisGlider->dest.top;
				thisGlider->vVel = -thisGlider->vVel + offset;
			}
		}
	}
	else if (g_thisBackground == kDirt)
	{
		leftTile = thisGlider->dest.left >> 6;  // ÷ 64
		rightTile = thisGlider->dest.right >> 6;  // ÷ 64

		if ((leftTile >= 0) && (leftTile < 8) &&
				(rightTile >= 0) && (rightTile < 8))
		{
			if (((g_thisTiles[leftTile] == 5) ||
					(g_thisTiles[leftTile] == 6)) &&
					((g_thisTiles[rightTile] == 5) ||
					(g_thisTiles[rightTile] == 6)))
			{
				if (thisGlider->dest.top < kNoCeilingLimit)
				{
					if (g_otherPlayerEscaped == kNoOneEscaped)
					{
						g_otherPlayerEscaped = kPlayerEscapedUp;
						RefreshScoreboard(kEscapedTitleMode);
						FlagGliderInLimbo(thisGlider, true);
					}
					else if (g_otherPlayerEscaped == kPlayerEscapedUp)
					{
						g_otherPlayerEscaped = kNoOneEscaped;
						MoveRoomToRoom(thisGlider, kAbove);
					}
					else
					{
						PlayPrioritySound(kDontExitSound, kDontExitPriority);
						offset = kNoCeilingLimit - thisGlider->dest.top;
						thisGlider->vVel = -thisGlider->vVel + offset;
					}
				}
			}
			else
				thisGlider->vVel = kCeilingLimit - thisGlider->dest.top;
		}
		else
			thisGlider->vVel = kCeilingLimit - thisGlider->dest.top;
	}
	else
		thisGlider->vVel = kCeilingLimit - thisGlider->dest.top;
}

//--------------------------------------------------------------  CheckEscapeUp

void CheckEscapeUp (gliderPtr thisGlider)
{
	SInt16 leftTile, rightTile;

	if (g_topOpen)
	{
		if (thisGlider->dest.top < kNoCeilingLimit)
		{
			MoveRoomToRoom(thisGlider, kAbove);
		}
	}
	else if (g_thisBackground == kDirt)
	{
		leftTile = thisGlider->dest.left >> 6;  // ÷ 64
		rightTile = thisGlider->dest.right >> 6;  // ÷ 64

		if ((leftTile >= 0) && (leftTile < 8) &&
				(rightTile >= 0) && (rightTile < 8))
		{
			if (((g_thisTiles[leftTile] == 5) ||
					(g_thisTiles[leftTile] == 6)) &&
					((g_thisTiles[rightTile] == 5) ||
					(g_thisTiles[rightTile] == 6)))
			{
				if (thisGlider->dest.top < kNoCeilingLimit)
					MoveRoomToRoom(thisGlider, kAbove);
			}
			else
				thisGlider->vVel = kCeilingLimit - thisGlider->dest.top;
		}
		else
			thisGlider->vVel = kCeilingLimit - thisGlider->dest.top;
	}
	else
		thisGlider->vVel = kCeilingLimit - thisGlider->dest.top;
}

//--------------------------------------------------------------  CheckEscapeDownTwo

void CheckEscapeDownTwo (gliderPtr thisGlider)
{
	SInt16 offset, leftTile, rightTile;

	if (g_bottomOpen)
	{
		if (thisGlider->dest.bottom > kNoFloorLimit)
		{
			if (g_otherPlayerEscaped == kNoOneEscaped)
			{
				g_otherPlayerEscaped = kPlayerEscapedDown;
				RefreshScoreboard(kEscapedTitleMode);
				FlagGliderInLimbo(thisGlider, true);
			}
			else if (g_otherPlayerEscaped == kPlayerEscapedDown)
			{
				g_otherPlayerEscaped = kNoOneEscaped;
				MoveRoomToRoom(thisGlider, kBelow);
			}
			else
			{
				PlayPrioritySound(kDontExitSound, kDontExitPriority);
				offset = kNoFloorLimit - thisGlider->dest.bottom;
				thisGlider->vVel = -thisGlider->vVel + offset;
			}
		}
	}
	else if (g_thisBackground == kDirt)
	{
		leftTile = thisGlider->dest.left >> 6;  // ÷ 64
		rightTile = thisGlider->dest.right >> 6;  // ÷ 64

		if ((leftTile >= 0) && (leftTile < 8) &&
				(rightTile >= 0) && (rightTile < 8))
		{
			if (((g_thisTiles[leftTile] == 2) ||
					(g_thisTiles[leftTile] == 3)) &&
					((g_thisTiles[rightTile] == 2) ||
					(g_thisTiles[rightTile] == 3)))
			{
				if (thisGlider->dest.bottom > kNoFloorLimit)
				{
					if (g_otherPlayerEscaped == kNoOneEscaped)
					{
						g_otherPlayerEscaped = kPlayerEscapedDown;
						RefreshScoreboard(kEscapedTitleMode);
						FlagGliderInLimbo(thisGlider, true);
					}
					else if (g_otherPlayerEscaped == kPlayerEscapedDown)
					{
						g_otherPlayerEscaped = kNoOneEscaped;
						MoveRoomToRoom(thisGlider, kBelow);
					}
					else
					{
						PlayPrioritySound(kDontExitSound, kDontExitPriority);
						offset = kNoFloorLimit - thisGlider->dest.bottom;
						thisGlider->vVel = -thisGlider->vVel + offset;
					}
				}
			}
			else
			{
				if (thisGlider->ignoreGround)
				{
					if (thisGlider->dest.bottom > kNoFloorLimit)
						MoveRoomToRoom(thisGlider, kBelow);
				}
				else
				{
					thisGlider->vVel = kFloorLimit - thisGlider->dest.bottom;
					StartGliderFadingOut(thisGlider);
					PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
				}
			}
		}
	}
	else
	{
		if (thisGlider->ignoreGround)
		{
			if (thisGlider->dest.bottom > kNoFloorLimit)
				MoveRoomToRoom(thisGlider, kBelow);
		}
		else
		{
			thisGlider->vVel = kFloorLimit - thisGlider->dest.bottom;
			StartGliderFadingOut(thisGlider);
			PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
		}
	}
}

//--------------------------------------------------------------  CheckEscapeDown

void CheckEscapeDown (gliderPtr thisGlider)
{
	SInt16 leftTile, rightTile;

	if (g_bottomOpen)
	{
		if (thisGlider->dest.bottom > kNoFloorLimit)
		{
			MoveRoomToRoom(thisGlider, kBelow);
		}
	}
	else if (g_thisBackground == kDirt)
	{
		leftTile = thisGlider->dest.left >> 6;  // ÷ 64
		rightTile = thisGlider->dest.right >> 6;  // ÷ 64

		if ((leftTile >= 0) && (leftTile < 8) && (rightTile >= 0) && (rightTile < 8))
		{
			if (((g_thisTiles[leftTile] == 2) || (g_thisTiles[leftTile] == 3)) &&
					((g_thisTiles[rightTile] == 2) || (g_thisTiles[rightTile] == 3)))
			{
				if (thisGlider->dest.bottom > kNoFloorLimit)
					MoveRoomToRoom(thisGlider, kBelow);
			}
			else
			{
				if (thisGlider->ignoreGround)
				{
					if (thisGlider->dest.bottom > kNoFloorLimit)
						MoveRoomToRoom(thisGlider, kBelow);
				}
				else
				{
					thisGlider->vVel = kFloorLimit - thisGlider->dest.bottom;
					StartGliderFadingOut(thisGlider);
					PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
				}
			}
		}
		else
		{
			if (thisGlider->ignoreGround)
			{
				if (thisGlider->dest.bottom > kNoFloorLimit)
					MoveRoomToRoom(thisGlider, kBelow);
			}
			else
			{
				thisGlider->vVel = kFloorLimit - thisGlider->dest.bottom;
				StartGliderFadingOut(thisGlider);
				PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
			}
		}
	}
	else
	{
		if (thisGlider->ignoreGround)
		{
			if (thisGlider->dest.bottom > kNoFloorLimit)
				MoveRoomToRoom(thisGlider, kBelow);
		}
		else
		{
			thisGlider->vVel = kFloorLimit - thisGlider->dest.bottom;
			StartGliderFadingOut(thisGlider);
			PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
		}
	}
}

//--------------------------------------------------------------  CheckRoofCollision

void CheckRoofCollision (gliderPtr thisGlider)
{
	SInt16 offset, tileOver;

	offset = (thisGlider->dest.left + kHalfGliderWide) >> 6;  // ÷ 64
	if ((offset >= 0) && (offset <= 7) && (!thisGlider->sliding))
	{
		tileOver = g_thisTiles[offset];
		if (tileOver == 1)
		{
			if (((thisGlider->dest.left + kHalfGliderWide) - (offset << 6)) >
					(250 - thisGlider->dest.bottom))
			{
				thisGlider->vVel = kFloorLimit - thisGlider->dest.bottom;
				StartGliderFadingOut(thisGlider);
				PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
			}
		}
		else if (tileOver == 2)
		{
			if (((thisGlider->dest.left + kHalfGliderWide) - (offset << 6)) >
					(186 - thisGlider->dest.bottom))
			{
				thisGlider->vVel = kFloorLimit - thisGlider->dest.bottom;
				StartGliderFadingOut(thisGlider);
				PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
			}
		}
		else if (tileOver == 5)
		{
			if ((64 - ((thisGlider->dest.left + kHalfGliderWide) - (offset << 6))) >
					(186 - thisGlider->dest.bottom))
			{
				thisGlider->vVel = kFloorLimit - thisGlider->dest.bottom;
				StartGliderFadingOut(thisGlider);
				PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
			}
		}
		else if (tileOver == 6)
		{
			if ((64 - ((thisGlider->dest.left + kHalfGliderWide) - (offset << 6))) >
					(250 - thisGlider->dest.bottom))
			{
				thisGlider->vVel = kFloorLimit - thisGlider->dest.bottom;
				StartGliderFadingOut(thisGlider);
				PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
			}
		}
		else
		{
			thisGlider->vVel = kFloorLimit - thisGlider->dest.bottom;
			StartGliderFadingOut(thisGlider);
			PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
		}
	}
}

//--------------------------------------------------------------  CheckEscapeLeftTwo

void CheckEscapeLeftTwo (gliderPtr thisGlider)
{
	SInt16 offset;

	if (g_leftThresh == kLeftWallLimit)
	{
		if (thisGlider->ignoreLeft)
		{
			if (thisGlider->dest.left < kNoLeftWallLimit)
			{
				if (g_otherPlayerEscaped == kNoOneEscaped)
				{
					g_otherPlayerEscaped = kPlayerEscapedLeft;
					RefreshScoreboard(kEscapedTitleMode);
					FlagGliderInLimbo(thisGlider, true);
				}
				else if (g_otherPlayerEscaped == kPlayerEscapedLeft)
				{
					g_otherPlayerEscaped = kNoOneEscaped;
					MoveRoomToRoom(thisGlider, kToLeft);
				}
				else
				{
					PlayPrioritySound(kDontExitSound, kDontExitPriority);
					offset = kNoLeftWallLimit - thisGlider->dest.left;
					thisGlider->hVel = -thisGlider->hVel + offset;
				}
			}
		}
		else
		{
			if (g_foilTotal > 0)
				PlayPrioritySound(kFoilHitSound, kFoilHitPriority);
			else
				PlayPrioritySound(kHitWallSound, kHitWallPriority);
			offset = kLeftWallLimit - thisGlider->dest.left;
			thisGlider->hVel = -thisGlider->hVel + offset;
		}
	}
	else
	{
		if (g_otherPlayerEscaped == kNoOneEscaped)
		{
			g_otherPlayerEscaped = kPlayerEscapedLeft;
			RefreshScoreboard(kEscapedTitleMode);
			FlagGliderInLimbo(thisGlider, true);
		}
		else if (g_otherPlayerEscaped == kPlayerEscapedLeft)
		{
			g_otherPlayerEscaped = kNoOneEscaped;
			MoveRoomToRoom(thisGlider, kToLeft);
		}
		else
		{
			PlayPrioritySound(kDontExitSound, kDontExitPriority);
			offset = kNoLeftWallLimit - thisGlider->dest.left;
			thisGlider->hVel = -thisGlider->hVel + offset;
		}
	}
}

//--------------------------------------------------------------  CheckEscapeLeft

void CheckEscapeLeft (gliderPtr thisGlider)
{
	SInt16 offset;

	if (g_leftThresh == kLeftWallLimit)
	{
		if (thisGlider->ignoreLeft)
		{
			if (thisGlider->dest.left < kNoLeftWallLimit)
				MoveRoomToRoom(thisGlider, kToLeft);
		}
		else
		{
			if (g_foilTotal > 0)
				PlayPrioritySound(kFoilHitSound, kFoilHitPriority);
			else
				PlayPrioritySound(kHitWallSound, kHitWallPriority);
			offset = kLeftWallLimit - thisGlider->dest.left;
			thisGlider->hVel = -thisGlider->hVel + offset;
		}
	}
	else
		MoveRoomToRoom(thisGlider, kToLeft);
}

//--------------------------------------------------------------  CheckEscapeRightTwo

void CheckEscapeRightTwo (gliderPtr thisGlider)
{
	SInt16 offset;

	if (g_rightThresh == kRightWallLimit)
	{
		if (thisGlider->ignoreRight)
		{
			if (thisGlider->dest.right > kNoRightWallLimit)
			{
				if (g_otherPlayerEscaped == kNoOneEscaped)
				{
					g_otherPlayerEscaped = kPlayerEscapedRight;
					RefreshScoreboard(kEscapedTitleMode);
					FlagGliderInLimbo(thisGlider, true);
				}
				else if (g_otherPlayerEscaped == kPlayerEscapedRight)
				{
					g_otherPlayerEscaped = kNoOneEscaped;
					MoveRoomToRoom(thisGlider, kToRight);
				}
				else
				{
					PlayPrioritySound(kDontExitSound, kDontExitPriority);
					offset = kNoRightWallLimit - thisGlider->dest.right;
					thisGlider->hVel = -thisGlider->hVel + offset;
				}
			}
		}
		else
		{
			if (g_foilTotal > 0)
				PlayPrioritySound(kFoilHitSound, kFoilHitPriority);
			else
				PlayPrioritySound(kHitWallSound, kHitWallPriority);
			offset = kRightWallLimit - thisGlider->dest.right;
			thisGlider->hVel = -thisGlider->hVel + offset;
		}
	}
	else
	{
		if (g_otherPlayerEscaped == kNoOneEscaped)
		{
			g_otherPlayerEscaped = kPlayerEscapedRight;
			RefreshScoreboard(kEscapedTitleMode);
			FlagGliderInLimbo(thisGlider, true);
		}
		else if (g_otherPlayerEscaped == kPlayerEscapedRight)
		{
			g_otherPlayerEscaped = kNoOneEscaped;
			MoveRoomToRoom(thisGlider, kToRight);
		}
		else
		{
			PlayPrioritySound(kDontExitSound, kDontExitPriority);
			offset = kNoRightWallLimit - thisGlider->dest.right;
			thisGlider->hVel = -thisGlider->hVel + offset;
		}
	}
}

//--------------------------------------------------------------  CheckEscapeRight

void CheckEscapeRight (gliderPtr thisGlider)
{
	SInt16 offset;

	if (g_rightThresh == kRightWallLimit)
	{
		if (thisGlider->ignoreRight)
		{
			if (thisGlider->dest.right > kNoRightWallLimit)
				MoveRoomToRoom(thisGlider, kToRight);
		}
		else
		{
			if (g_foilTotal > 0)
				PlayPrioritySound(kFoilHitSound, kFoilHitPriority);
			else
				PlayPrioritySound(kHitWallSound, kHitWallPriority);
			offset = kRightWallLimit - thisGlider->dest.right;
			thisGlider->hVel = -thisGlider->hVel + offset;
		}
	}
	else
		MoveRoomToRoom(thisGlider, kToRight);
}

//--------------------------------------------------------------  CheckGliderInRoom

void CheckGliderInRoom (gliderPtr thisGlider)
{
	if ((thisGlider->mode == kGliderNormal) ||
			(thisGlider->mode == kGliderFaceLeft) ||
			(thisGlider->mode == kGliderFaceRight) ||
			(thisGlider->mode == kGliderBurning))
	{
		if (thisGlider->dest.top < kCeilingLimit)
		{
			if (thisGlider->mode == kGliderBurning)
			{
				thisGlider->wasMode = 0;
				StartGliderFadingOut(thisGlider);
				PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
			}
			else if ((g_twoPlayerGame) && (!g_onePlayerLeft))
				CheckEscapeUpTwo(thisGlider);
			else
				CheckEscapeUp(thisGlider);
		}
		else if (thisGlider->dest.bottom > kFloorLimit)
		{
			if (thisGlider->mode == kGliderBurning)
			{
				thisGlider->wasMode = 0;
				StartGliderFadingOut(thisGlider);
				PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
			}
			else if ((g_twoPlayerGame) && (!g_onePlayerLeft))
				CheckEscapeDownTwo(thisGlider);
			else
				CheckEscapeDown(thisGlider);
		}
		else if ((g_thisBackground == kRoof) && (thisGlider->dest.bottom > kRoofLimit))
			CheckRoofCollision(thisGlider);

		if (thisGlider->dest.left < g_leftThresh)
		{
			if (thisGlider->mode == kGliderBurning)
			{
				thisGlider->wasMode = 0;
				StartGliderFadingOut(thisGlider);
				PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
			}
			else if((g_twoPlayerGame) && (!g_onePlayerLeft))
				CheckEscapeLeftTwo(thisGlider);
			else
				CheckEscapeLeft(thisGlider);
		}
		else if (thisGlider->dest.right > g_rightThresh)
		{
			if (thisGlider->mode == kGliderBurning)
			{
				thisGlider->wasMode = 0;
				StartGliderFadingOut(thisGlider);
				PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
			}
			else if ((g_twoPlayerGame) && (!g_onePlayerLeft))
				CheckEscapeRightTwo(thisGlider);
			else
				CheckEscapeRight(thisGlider);
		}
	}
}

//--------------------------------------------------------------  HandleRewards

void HandleRewards (gliderPtr thisGlider, hotPtr who)
{
	Rect bounds;
	SInt16 whoLinked, points;

	whoLinked = who->who;
	bounds =  who->bounds;

	switch (g_masterObjects[whoLinked].theObject.what)
	{
		case kRedClock:
		if (SetObjectState(g_thisRoomNumber, g_masterObjects[whoLinked].objectNum,
				0, whoLinked))
		{
			PlayPrioritySound(kBeepsSound, kBeepsPriority);
			RestoreFromSavedMap(g_thisRoomNumber,
					g_masterObjects[whoLinked].objectNum, false);
			AddFlyingPoint(&bounds, 100, thisGlider->hVel / 2, thisGlider->vVel / 2);
			thisGlider->hVel /= 4;
			thisGlider->vVel /= 4;
			g_theScore += kRedClockPoints;
			RedrawAllGrease();
		}
		who->isOn = false;
		break;

		case kBlueClock:
		if (SetObjectState(g_thisRoomNumber, g_masterObjects[whoLinked].objectNum,
				0, whoLinked))
		{
			PlayPrioritySound(kBuzzerSound, kBuzzerPriority);
			RestoreFromSavedMap(g_thisRoomNumber,
					g_masterObjects[whoLinked].objectNum, false);
			AddFlyingPoint(&bounds, 300, thisGlider->hVel / 2, thisGlider->vVel / 2);
			thisGlider->hVel /= 4;
			thisGlider->vVel /= 4;
			g_theScore += kBlueClockPoints;
			RedrawAllGrease();
		}
		who->isOn = false;
		break;

		case kYellowClock:
		if (SetObjectState(g_thisRoomNumber, g_masterObjects[whoLinked].objectNum,
				0, whoLinked))
		{
			PlayPrioritySound(kDingSound, kDingPriority);
			RestoreFromSavedMap(g_thisRoomNumber,
					g_masterObjects[whoLinked].objectNum, false);
			AddFlyingPoint(&bounds, 500, thisGlider->hVel / 2, thisGlider->vVel / 2);
			thisGlider->hVel /= 4;
			thisGlider->vVel /= 4;
			g_theScore += kYellowClockPoints;
			RedrawAllGrease();
		}
		who->isOn = false;
		break;

		case kCuckoo:
		if (SetObjectState(g_thisRoomNumber, g_masterObjects[whoLinked].objectNum,
				0, whoLinked))
		{
			PlayPrioritySound(kCuckooSound, kCuckooPriority);
			RestoreFromSavedMap(g_thisRoomNumber,
					g_masterObjects[whoLinked].objectNum, false);
			StopPendulum(g_thisRoomNumber, g_masterObjects[whoLinked].objectNum);
			AddFlyingPoint(&bounds, 1000, thisGlider->hVel / 2, thisGlider->vVel / 2);
			thisGlider->hVel /= 4;
			thisGlider->vVel /= 4;
			g_theScore += kCuckooClockPoints;
			RedrawAllGrease();
		}
		who->isOn = false;
		break;

		case kPaper:
		if (SetObjectState(g_thisRoomNumber, g_masterObjects[whoLinked].objectNum,
				0, whoLinked))
		{
			PlayPrioritySound(kEnergizeSound, kEnergizePriority);
			RestoreFromSavedMap(g_thisRoomNumber,
					g_masterObjects[whoLinked].objectNum, false);
			AddSparkle(&bounds);
			thisGlider->hVel /= 2;
			thisGlider->vVel /= 2;
			g_mortals++;
			if ((g_twoPlayerGame) && (!g_onePlayerLeft))
				g_mortals++;
			QuickGlidersRefresh();
			RedrawAllGrease();
		}
		who->isOn = false;
		break;

		case kBattery:
		if (SetObjectState(g_thisRoomNumber, g_masterObjects[whoLinked].objectNum,
				0, whoLinked))
		{
			PlayPrioritySound(kEnergizeSound, kEnergizePriority);
			RestoreFromSavedMap(g_thisRoomNumber,
					g_masterObjects[whoLinked].objectNum, false);
			AddSparkle(&bounds);
			thisGlider->hVel /= 2;
			thisGlider->vVel /= 2;
			if (g_batteryTotal > 0)  // positive number means battery power
				g_batteryTotal += kBatterySupply;
			else  // negative number means helium gas
				g_batteryTotal = kBatterySupply;
			if ((g_twoPlayerGame) && (!g_onePlayerLeft))
				g_batteryTotal += kBatterySupply;
			QuickBatteryRefresh(false);
			RedrawAllGrease();
		}
		who->isOn = false;
		break;

		case kBands:
		if (SetObjectState(g_thisRoomNumber, g_masterObjects[whoLinked].objectNum,
				0, whoLinked))
		{
			PlayPrioritySound(kEnergizeSound, kEnergizePriority);
			RestoreFromSavedMap(g_thisRoomNumber,
					g_masterObjects[whoLinked].objectNum, false);
			AddSparkle(&bounds);
			thisGlider->hVel /= 2;
			thisGlider->vVel /= 2;
			g_bandsTotal += kBandsSupply;
			if ((g_twoPlayerGame) && (!g_onePlayerLeft))
				g_bandsTotal += kBandsSupply;
			QuickBandsRefresh(false);
			RedrawAllGrease();
		}
		who->isOn = false;
		break;

		case kGreaseRt:
		case kGreaseLf:
		if (SetObjectState(g_thisRoomNumber, g_masterObjects[whoLinked].objectNum,
				0, whoLinked))
			SpillGrease(g_masterObjects[whoLinked].dynaNum,
					g_masterObjects[whoLinked].hotNum);
		who->isOn = false;
		break;

		case kFoil:
		if (SetObjectState(g_thisRoomNumber, g_masterObjects[whoLinked].objectNum,
				0, whoLinked))
		{
			PlayPrioritySound(kEnergizeSound, kEnergizePriority);
			RestoreFromSavedMap(g_thisRoomNumber,
					g_masterObjects[whoLinked].objectNum, false);
			AddSparkle(&bounds);
			thisGlider->hVel /= 2;
			thisGlider->vVel /= 2;
			g_foilTotal += kFoilSupply;
			if ((g_twoPlayerGame) && (!g_onePlayerLeft))
				g_foilTotal += kFoilSupply;
			StartGliderFoilGoing(thisGlider);
			RedrawAllGrease();
		}
		who->isOn = false;
		break;

		case kInvisBonus:
		points = g_masterObjects[whoLinked].theObject.data.c.points;
		if (SetObjectState(g_thisRoomNumber, g_masterObjects[whoLinked].objectNum,
				0, whoLinked))
		{
			PlayPrioritySound(kBonusSound, kBonusPriority);
			AddFlyingPoint(&bounds, points, thisGlider->hVel / 2, thisGlider->vVel / 2);
			thisGlider->hVel /= 4;
			thisGlider->vVel /= 4;
			g_theScore += points;
		}
		who->isOn = false;
		break;

		case kStar:
		if (SetObjectState(g_thisRoomNumber, g_masterObjects[whoLinked].objectNum,
				0, whoLinked))
		{
			PlayPrioritySound(kEnergizeSound, kEnergizePriority);
			RestoreFromSavedMap(g_thisRoomNumber,
					g_masterObjects[whoLinked].objectNum, false);
			AddSparkle(&bounds);
			StopStar(g_thisRoomNumber, g_masterObjects[whoLinked].objectNum);
			g_numStarsRemaining--;
			if (g_numStarsRemaining <= 0)
				FlagGameOver();
			else
				DisplayStarsRemaining();
			RedrawAllGrease();
			g_theScore += kStarPoints;
		}
		who->isOn = false;
		break;

		case kSparkle:
		break;

		case kHelium:
		if (SetObjectState(g_thisRoomNumber, g_masterObjects[whoLinked].objectNum,
				0, whoLinked))
		{
			PlayPrioritySound(kEnergizeSound, kEnergizePriority);
			RestoreFromSavedMap(g_thisRoomNumber,
					g_masterObjects[whoLinked].objectNum, false);
			AddSparkle(&bounds);
			thisGlider->hVel /= 2;
			thisGlider->vVel /= 2;
			if (g_batteryTotal < 0)  // if negative, it is already helium gas
				g_batteryTotal -= kHeliumSupply;
			else  // if positive, it is battery power
				g_batteryTotal = -kHeliumSupply;
			if ((g_twoPlayerGame) && (!g_onePlayerLeft))
				g_batteryTotal -= kHeliumSupply;
			QuickBatteryRefresh(false);
			RedrawAllGrease();
		}
		who->isOn = false;
		break;

		case kSlider:
		break;
	}
}

//--------------------------------------------------------------  HandleSwitches

void HandleSwitches (hotPtr who)
{
	Rect newRect;
	SInt16 whoLinked, roomLinked, objectLinked, linkIndex;

	if (who->stillOver)
		return;

	whoLinked = who->who;  // what is switch's obj. #
	roomLinked = g_masterObjects[whoLinked].roomLink;
	objectLinked = g_masterObjects[whoLinked].objectLink;
	linkIndex = g_masterObjects[whoLinked].localLink;
	// change state of linked obj.
	if (SetObjectState(roomLinked, objectLinked,
			g_masterObjects[whoLinked].theObject.data.e.type, linkIndex))
	{
		newRect = who->bounds;
		QOffsetRect(&newRect, g_playOriginH, g_playOriginV);
		switch (g_masterObjects[whoLinked].theObject.what)
		{
			case kLightSwitch:
			PlayPrioritySound(kSwitchSound, kSwitchPriority);
			DrawLightSwitch(g_backSrcMap, &newRect, g_newState);
			break;

			case kMachineSwitch:
			PlayPrioritySound(kSwitchSound, kSwitchPriority);
			DrawMachineSwitch(g_backSrcMap, &newRect, g_newState);
			break;

			case kThermostat:
			PlayPrioritySound(kSwitchSound, kSwitchPriority);
			DrawThermostat(g_backSrcMap, &newRect, g_newState);
			break;

			case kPowerSwitch:
			PlayPrioritySound(kSwitchSound, kSwitchPriority);
			DrawPowerSwitch(g_backSrcMap, &newRect, g_newState);
			break;

			case kKnifeSwitch:
			PlayPrioritySound(kSwitchSound, kSwitchPriority);
			DrawKnifeSwitch(g_backSrcMap, &newRect, g_newState);
			break;

			case kInvisSwitch:
			break;
		}
		CopyRectBackToWork(&newRect);
		AddRectToWorkRects(&newRect);

		if (linkIndex != -1)
		{
			switch (g_masterObjects[linkIndex].theObject.what)
			{
				case kRedClock:
				case kBlueClock:
				case kYellowClock:
				case kPaper:
				case kBattery:
				case kBands:
				case kFoil:
				case kStar:
				case kHelium:
				RestoreFromSavedMap(roomLinked, objectLinked, true);
				break;

				case kCuckoo:
				RestoreFromSavedMap(roomLinked, objectLinked, true);
				StopPendulum(roomLinked, objectLinked);
				break;

				case kGreaseRt:
				case kGreaseLf:
				SpillGrease(g_masterObjects[linkIndex].dynaNum,
						g_masterObjects[linkIndex].hotNum);
				break;

				case kInvisBonus:
				case kSlider:
				break;

				case kDeluxeTrans:
				break;

				case kSoundTrigger:
				PlayPrioritySound(kTriggerSound, kTriggerPriority);
				break;

				case kCeilingLight:
				case kLightBulb:
				case kTableLamp:
				case kHipLamp:
				case kDecoLamp:
				case kFlourescent:
				case kTrackLight:
				case kInvisLight:
				RedrawRoomLighting();
				break;

				case kShredder:
				break;

				case kToaster:
				ToggleToaster(g_masterObjects[linkIndex].dynaNum);
				break;

				case kMacPlus:
				ToggleMacPlus(g_masterObjects[linkIndex].dynaNum);
				break;

				case kGuitar:
				PlayPrioritySound(kChordSound, kChordPriority);
				break;

				case kTV:
				ToggleTV(g_masterObjects[linkIndex].dynaNum);
				break;

				case kCoffee:
				ToggleCoffee(g_masterObjects[linkIndex].dynaNum);
				break;

				case kOutlet:
				ToggleOutlet(g_masterObjects[linkIndex].dynaNum);
				break;

				case kVCR:
				ToggleVCR(g_masterObjects[linkIndex].dynaNum);
				break;

				case kStereo:
				ToggleStereos(g_masterObjects[linkIndex].dynaNum);
				break;

				case kMicrowave:
				ToggleMicrowave(g_masterObjects[linkIndex].dynaNum);
				break;

				case kBalloon:
				ToggleBalloon(g_masterObjects[linkIndex].dynaNum);
				break;

				case kCopterLf:
				case kCopterRt:
				ToggleCopter(g_masterObjects[linkIndex].dynaNum);
				break;

				case kDartLf:
				case kDartRt:
				ToggleDart(g_masterObjects[linkIndex].dynaNum);
				break;

				case kBall:
				ToggleBall(g_masterObjects[linkIndex].dynaNum);
				break;

				case kDrip:
				ToggleDrip(g_masterObjects[linkIndex].dynaNum);
				break;

				case kFish:
				ToggleFish(g_masterObjects[linkIndex].dynaNum);
				break;
			}
		}
	}

	who->stillOver = true;
}

//--------------------------------------------------------------  HandleMicrowaveAction

void HandleMicrowaveAction (hotPtr who, gliderPtr thisGlider)
{
	SInt16 whoLinked, kills;
	Boolean killed;

	if (who->stillOver)
		return;

	killed = false;
	whoLinked = who->who;  // what is microwave's obj. #
	if (g_masterObjects[whoLinked].theObject.data.g.state)
	{
		kills = (SInt16)g_masterObjects[whoLinked].theObject.data.g.byte0;
		if (((kills & 0x0001) == 0x0001) && (g_bandsTotal > 0))
		{
			g_bandsTotal = 0;
			killed = true;
			QuickBandsRefresh(false);
		}
		if (((kills & 0x0002) == 0x0002) && (g_batteryTotal != 0))
		{
			g_batteryTotal = 0;
			killed = true;
			QuickBatteryRefresh(false);
		}
		if (((kills & 0x0004) == 0x0004) && (g_foilTotal > 0))
		{
			g_foilTotal = 0;
			killed = true;
			StartGliderFoilLosing(thisGlider);
		}
	}

	if (killed)
		PlayPrioritySound(kMicrowavedSound, kMicrowavedPriority);
}

//--------------------------------------------------------------  HandleHotSpotCollision

void HandleHotSpotCollision (gliderPtr thisGlider, hotPtr who, SInt16 index)
{
	switch (who->action)
	{
		case kLiftIt:
		thisGlider->vDesiredVel = kFloorVentLift;
		break;

		case kDropIt:
		thisGlider->vDesiredVel = kCeilingVentDrop;
		break;

		case kPushItLeft:
		thisGlider->hDesiredVel += -kFanStrength;
		break;

		case kPushItRight:
		thisGlider->hDesiredVel += kFanStrength;
		break;

		case kDissolveIt:
		if (thisGlider->mode != kGliderFadingOut)
		{
			if ((g_foilTotal > 0) || (thisGlider->mode == kGliderLosingFoil))
			{
				if (GliderHitTop(thisGlider, &(who->bounds)))
				{
					StartGliderFadingOut(thisGlider);
					PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
				}
				else
				{
					if (g_foilTotal > 0)
					{
						g_foilTotal--;
						if (g_foilTotal <= 0)
							StartGliderFoilLosing(thisGlider);
					}
				}
			}
			else
			{
				StartGliderFadingOut(thisGlider);
				PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
			}
		}
		break;

		case kRewardIt:
		HandleRewards(thisGlider, who);
		break;

		case kMoveItUp:
		if (!thisGlider->heldRight && GliderInRect(thisGlider, &who->bounds))
		{
			if (thisGlider->mode == kGliderBurning)
			{
				thisGlider->wasMode = 0;
				StartGliderFadingOut(thisGlider);
				PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
			}
			else if ((g_twoPlayerGame) && (!g_onePlayerLeft))
			{
				if (g_otherPlayerEscaped == kNoOneEscaped)
				{
					if ((thisGlider->mode != kGliderGoingUp) &&
							(thisGlider->mode != kGliderInLimbo))
					{
						g_otherPlayerEscaped = kPlayerEscapingUpStairs;
						RefreshScoreboard(kEscapedTitleMode);
						StartGliderGoingUpStairs(thisGlider);
					}
				}
				else if (g_otherPlayerEscaped == kPlayerEscapedUpStairs)
				{
					if ((thisGlider->mode != kGliderGoingUp) &&
							(thisGlider->mode != kGliderInLimbo))
					{
						StartGliderGoingUpStairs(thisGlider);
					}
				}
			}
			else
				StartGliderGoingUpStairs(thisGlider);
		}
		break;

		case kMoveItDown:
		if (!thisGlider->heldLeft && GliderInRect(thisGlider, &who->bounds))
		{
			if (thisGlider->mode == kGliderBurning)
			{
				thisGlider->wasMode = 0;
				StartGliderFadingOut(thisGlider);
				PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
			}
			else if ((g_twoPlayerGame) && (!g_onePlayerLeft))
			{
				if (g_otherPlayerEscaped == kNoOneEscaped)
				{
					if ((thisGlider->mode != kGliderGoingDown) &&
							(thisGlider->mode != kGliderInLimbo))
					{
						g_otherPlayerEscaped = kPlayerEscapingDownStairs;
						RefreshScoreboard(kEscapedTitleMode);
						StartGliderGoingDownStairs(thisGlider);
					}
				}
				else if (g_otherPlayerEscaped == kPlayerEscapedDownStairs)
				{
					if ((thisGlider->mode != kGliderGoingDown) &&
							(thisGlider->mode != kGliderInLimbo))
					{
						StartGliderGoingDownStairs(thisGlider);
					}
				}
			}
			else
				StartGliderGoingDownStairs(thisGlider);
		}
		break;

		case kSwitchIt:
		HandleSwitches(who);
		break;

		case kShredIt:
		if ((thisGlider->mode != kGliderShredding) &&
				(GliderInRect(thisGlider, &who->bounds)))
		{
			if ((g_foilTotal > 0) || (thisGlider->mode == kGliderLosingFoil))
			{
				PlayPrioritySound(kFoilHitSound, kFoilHitPriority);
				if (g_foilTotal > 0)
				{
					g_foilTotal--;
					if (g_foilTotal <= 0)
						StartGliderFoilLosing(thisGlider);
				}
			}
			else
				FlagGliderShredding(thisGlider, &who->bounds);
		}
		break;

		case kStrumIt:
		if (!who->stillOver)
		{
			PlayPrioritySound(kChordSound, kChordPriority);
			who->stillOver = true;
		}
		break;

		case kTriggerIt:
		case kLgTrigger:
		ArmTrigger(who);
		break;

		case kBurnIt:
		if ((thisGlider->mode != kGliderBurning) &&
				(thisGlider->mode != kGliderFadingOut))
		{
			if ((g_foilTotal > 0) || (thisGlider->mode == kGliderLosingFoil))
			{
				thisGlider->vDesiredVel = kFloorVentLift;
				if (g_foilTotal > 0)
				{
					PlayPrioritySound(kSizzleSound, kSizzlePriority);
					g_foilTotal--;
					if (g_foilTotal <= 0)
						StartGliderFoilLosing(thisGlider);
				}
			}
			else
				FlagGliderBurning(thisGlider);
		}
		break;

		case kSlideIt:
		thisGlider->sliding = true;
		thisGlider->vVel = who->bounds.top - thisGlider->dest.bottom;
		break;

		case kTransportIt:
		if (thisGlider->mode == kGliderBurning)
		{
			thisGlider->wasMode = 0;
			StartGliderFadingOut(thisGlider);
			PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
		}
		else if ((GliderInRect(thisGlider, &who->bounds)) &&
				(thisGlider->mode != kGliderTransporting) &&
				(thisGlider->mode != kGliderFadingOut))
		{
			if ((g_twoPlayerGame) && (!g_onePlayerLeft))
			{
				if (g_otherPlayerEscaped == kNoOneEscaped)
				{
					if (thisGlider->mode != kGliderInLimbo)
					{
						g_activeRectEscaped = index;
						StartGliderTransporting(thisGlider, who);
					}
				}
				else if (g_otherPlayerEscaped == kPlayerTransportedOut)
				{
					if ((thisGlider->mode != kGliderInLimbo) &&
							(g_activeRectEscaped == index))
					{
						StartGliderTransporting(thisGlider, who);
					}
				}
			}
			else
				StartGliderTransporting(thisGlider, who);
		}
		break;

		case kIgnoreLeftWall:
		thisGlider->ignoreLeft = true;
		break;

		case kIgnoreRightWall:
		thisGlider->ignoreRight = true;
		break;

		case kMailItLeft:  // mailbox open to right
		if (thisGlider->mode == kGliderBurning)
		{
			thisGlider->wasMode = 0;
			StartGliderFadingOut(thisGlider);
			PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
		}
		else if ((GliderInRect(thisGlider, &who->bounds)) &&
				(thisGlider->mode != kGliderMailOutRight) &&
				(thisGlider->mode != kGliderMailInLeft) &&
				(thisGlider->mode != kGliderFadingOut) &&
				(((thisGlider->facing == kFaceRight) && (!thisGlider->tipped)) ||
				((thisGlider->facing == kFaceLeft) && (thisGlider->tipped))))
		{
			if ((g_twoPlayerGame) && (!g_onePlayerLeft))  // two gliders to handle
			{
				if (g_otherPlayerEscaped == kNoOneEscaped)  // other glider in room
				{
					if (thisGlider->mode != kGliderInLimbo)  // this glider is active
					{
						g_activeRectEscaped = index;
						StartGliderMailingIn(thisGlider, &who->bounds, who);
						thisGlider->mode = kGliderMailInLeft;
					}
				}
				else if (g_otherPlayerEscaped == kPlayerMailedOut)
				{
					// other glider left here
					if ((thisGlider->mode != kGliderInLimbo) &&
							(g_activeRectEscaped == index))
					{
						// []_ <--G
						StartGliderMailingIn(thisGlider, &who->bounds, who);
						thisGlider->mode = kGliderMailInLeft;
					}
				}
			}
			else  // only 1 glider in game
			{
				StartGliderMailingIn(thisGlider, &who->bounds, who);
				thisGlider->mode = kGliderMailInLeft;
			}
		}
		break;

		case kMailItRight:  // mailbox open to left
		if (thisGlider->mode == kGliderBurning)
		{
			thisGlider->wasMode = 0;
			StartGliderFadingOut(thisGlider);
			PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
		}
		else if ((GliderInRect(thisGlider, &who->bounds)) &&
				(thisGlider->mode != kGliderMailOutLeft) &&
				(thisGlider->mode != kGliderMailInRight) &&
				(thisGlider->mode != kGliderFadingOut) &&
				(((thisGlider->facing == kFaceRight) && (thisGlider->tipped)) ||
				((thisGlider->facing == kFaceLeft) && (!thisGlider->tipped))))
		{
			if ((g_twoPlayerGame) && (!g_onePlayerLeft))
			{
				if (g_otherPlayerEscaped == kNoOneEscaped)
				{
					if (thisGlider->mode != kGliderInLimbo)
					{
						g_activeRectEscaped = index;
						StartGliderMailingIn(thisGlider, &who->bounds, who);
						thisGlider->mode = kGliderMailInRight;
					}
				}
				else if (g_otherPlayerEscaped == kPlayerMailedOut)
				{
					if ((thisGlider->mode != kGliderInLimbo) &&
							(g_activeRectEscaped == index))
					{
						StartGliderMailingIn(thisGlider, &who->bounds, who);
						thisGlider->mode = kGliderMailInRight;
					}
				}
			}
			else
			{
				StartGliderMailingIn(thisGlider, &who->bounds, who);
				thisGlider->mode = kGliderMailInRight;
			}
		}
		break;

		case kDuctItDown:
		if (thisGlider->mode == kGliderBurning)
		{
			thisGlider->wasMode = 0;
			StartGliderFadingOut(thisGlider);
			PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
		}
		else if ((GliderInRect(thisGlider, &who->bounds)) &&
				(thisGlider->mode != kGliderDuctingDown) &&
				(thisGlider->mode != kGliderFadingOut))
		{
			if ((g_twoPlayerGame) && (!g_onePlayerLeft))
			{
				if (g_otherPlayerEscaped == kNoOneEscaped)
				{
					if (thisGlider->mode != kGliderInLimbo)
					{
						g_activeRectEscaped = index;
						StartGliderDuctingDown(thisGlider, &who->bounds, who);
					}
				}
				else if (g_otherPlayerEscaped == kPlayerDuckedOut)
				{
					if ((thisGlider->mode != kGliderInLimbo) &&
							(g_activeRectEscaped == index))
						StartGliderDuctingDown(thisGlider, &who->bounds, who);
				}
			}
			else
				StartGliderDuctingDown(thisGlider, &who->bounds, who);
		}
		break;

		case kDuctItUp:
		if (thisGlider->mode == kGliderBurning)
		{
			thisGlider->wasMode = 0;
			StartGliderFadingOut(thisGlider);
			PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
		}
		else if ((GliderInRect(thisGlider, &who->bounds)) &&
				(thisGlider->mode != kGliderDuctingUp) &&
				(thisGlider->mode != kGliderDuctingIn) &&
				(thisGlider->mode != kGliderFadingOut) &&
				(!who->stillOver))
		{
			if ((g_twoPlayerGame) && (!g_onePlayerLeft))
			{
				if (g_otherPlayerEscaped == kNoOneEscaped)
				{
					if (thisGlider->mode != kGliderInLimbo)
					{
						g_activeRectEscaped = index;
						StartGliderDuctingUp(thisGlider, &who->bounds, who);
					}
				}
				else if (g_otherPlayerEscaped == kPlayerDuckedOut)
				{
					if ((thisGlider->mode != kGliderInLimbo) &&
							(g_activeRectEscaped == index))
						StartGliderDuctingUp(thisGlider, &who->bounds, who);
				}
			}
			else
			{
				StartGliderDuctingUp(thisGlider, &who->bounds, who);
				who->stillOver = true;
			}
		}
		break;

		case kMicrowaveIt:
		if (GliderInRect(thisGlider, &who->bounds))
			HandleMicrowaveAction(who, thisGlider);
		break;

		case kIgnoreGround:
		thisGlider->ignoreGround = true;
		break;

		case kBounceIt:
		BounceGlider(thisGlider, &who->bounds);
		break;

		case kChimeIt:
		if (!who->stillOver)
		{
			StrikeChime();
			who->stillOver = true;
		}
		break;

		case kWebIt:
		if ((GliderInRect(thisGlider, &who->bounds)) &&
				(thisGlider->mode != kGliderBurning))
			WebGlider(thisGlider, &who->bounds);
		else if ((thisGlider->mode == kGliderBurning) &&
				(GliderInRect(thisGlider, &who->bounds)))
		{
			thisGlider->wasMode = 0;
			StartGliderFadingOut(thisGlider);
			PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
		}
		break;

		case kSoundIt:
		if (!who->stillOver)
		{
			PlayPrioritySound(kTriggerSound, kTriggerPriority);
			who->stillOver = true;
		}
		break;
	}
}

//--------------------------------------------------------------  CheckForHotSpots

void CheckForHotSpots (void)
{
	SInt16 i;
	Boolean hitObject;

	for (i = 0; i < g_nHotSpots; i++)
	{
		if (g_hotSpots[i].isOn)
		{
			if (g_twoPlayerGame)
			{
				hitObject = false;
				if (SectGlider(&g_theGlider, &g_hotSpots[i].bounds,
						g_hotSpots[i].doScrutinize))
				{
					if (g_onePlayerLeft)
					{
						if (g_playerDead == kPlayer2)
						{
							HandleHotSpotCollision(&g_theGlider, &g_hotSpots[i], i);
							hitObject = true;
						}
					}
					else
					{
						HandleHotSpotCollision(&g_theGlider, &g_hotSpots[i], i);
						hitObject = true;
					}
				}

				if (SectGlider(&g_theGlider2, &g_hotSpots[i].bounds,
						g_hotSpots[i].doScrutinize))
				{
					if (g_onePlayerLeft)
					{
						if (g_playerDead == kPlayer1)
						{
							HandleHotSpotCollision(&g_theGlider2, &g_hotSpots[i], i);
							hitObject = true;
						}
					}
					else
					{
						HandleHotSpotCollision(&g_theGlider2, &g_hotSpots[i], i);
						hitObject = true;
					}
				}
				if (!hitObject)
					g_hotSpots[i].stillOver = false;
			}
			else
			{
				if (SectGlider(&g_theGlider, &g_hotSpots[i].bounds,
						g_hotSpots[i].doScrutinize))
					HandleHotSpotCollision(&g_theGlider, &g_hotSpots[i], i);
				else
					g_hotSpots[i].stillOver = false;
			}
		}
	}
}

//--------------------------------------------------------------  HandleInteraction

void HandleInteraction (void)
{
	CheckForHotSpots();
	if (g_twoPlayerGame)
	{
		if (g_onePlayerLeft)
		{
			if (g_playerDead == kPlayer1)
				CheckGliderInRoom(&g_theGlider2);
			else
				CheckGliderInRoom(&g_theGlider);
		}
		else
		{
			CheckGliderInRoom(&g_theGlider);
			CheckGliderInRoom(&g_theGlider2);
		}
	}
	else
		CheckGliderInRoom(&g_theGlider);
}

//--------------------------------------------------------------  FlagStillOvers

void FlagStillOvers (const gliderType *thisGlider)
{
	SInt16 i;

	for (i = 0; i < g_nHotSpots; i++)
	{
		if (g_hotSpots[i].isOn)
		{
			if (SectGlider(thisGlider, &g_hotSpots[i].bounds,
					g_hotSpots[i].doScrutinize))
				g_hotSpots[i].stillOver = true;
			else
				g_hotSpots[i].stillOver = false;
		}
		else
			g_hotSpots[i].stillOver = false;
	}
}

//--------------------------------------------------------------  WebGlider

void WebGlider (gliderPtr thisGlider, const Rect *webBounds)
{
	#define kKillWebbedGlider  150
	SInt16 hDist, vDist;

	if ((thisGlider->mode == kGliderBurning) && (GliderInRect(thisGlider, webBounds)))
	{
		thisGlider->wasMode = 0;
		StartGliderFadingOut(thisGlider);
		PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
		return;
	}

	hDist = ((webBounds->right - thisGlider->dest.right) +
			(webBounds->left - thisGlider->dest.left)) >> 3;
	vDist = ((webBounds->bottom - thisGlider->dest.bottom) +
			(webBounds->top - thisGlider->dest.top)) >> 3;

	if (thisGlider->hDesiredVel != 0)
	{
		if (g_evenFrame)
		{
			thisGlider->hVel = hDist;
			thisGlider->vVel = vDist;
			PlayPrioritySound(kWebTwangSound, kWebTwangPriority);
		}
	}
	else
	{
		thisGlider->hDesiredVel = 0;
		thisGlider->vDesiredVel = 0;
	}

	thisGlider->wasMode++;
	if (thisGlider->wasMode >= kKillWebbedGlider)
	{
		thisGlider->wasMode = 0;
		StartGliderFadingOut(thisGlider);
		PlayPrioritySound(kFadeOutSound, kFadeOutPriority);
	}
}
