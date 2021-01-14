//============================================================================
//----------------------------------------------------------------------------
//                               RubberBands.c
//----------------------------------------------------------------------------
//============================================================================

#include "RubberBands.h"

#include "Grease.h"
#include "Interactions.h"
#include "MainWindow.h"
#include "Objects.h"
#include "Play.h"
#include "Player.h"
#include "RectUtils.h"
#include "Render.h"
#include "Room.h"
#include "Sound.h"
#include "Triggers.h"

#define kRubberBandVelocity     20
#define kBandFallCount          4
#define kKillBandMode           -1

void CheckBandCollision (SInt16 who);
void KillBand (SInt16 which);

bandType g_bands[kMaxRubberBands];
Rect g_bandsSrcRect;
Rect g_bandRects[3];
HDC g_bandsSrcMap;
HDC g_bandsMaskMap;
SInt16 g_numBands;

static SInt16 g_bandHitLast;

//==============================================================  Functions
//--------------------------------------------------------------  CheckBandCollision

void CheckBandCollision (SInt16 who)
{
	SInt16 i, action, whoLinked;
	Boolean collided, nothingCollided;

	nothingCollided = true;

	if ((g_leftThresh == kLeftWallLimit) && (g_bands[who].dest.left < kLeftWallLimit))
	{
		if (g_bands[who].hVel < 0)
			g_bands[who].hVel = -g_bands[who].hVel;
		g_bands[who].dest.left = kLeftWallLimit;
		g_bands[who].dest.right = g_bands[who].dest.left + 16;
		PlayPrioritySound(kBandReboundSound, kBandReboundPriority);
		collided = true;
	}
	else if ((g_rightThresh == kRightWallLimit) && (g_bands[who].dest.right > kRightWallLimit))
	{
		if (g_bands[who].hVel > 0)
			g_bands[who].hVel = -g_bands[who].hVel;
		g_bands[who].dest.right = kRightWallLimit;
		g_bands[who].dest.left = g_bands[who].dest.right - 16;
		PlayPrioritySound(kBandReboundSound, kBandReboundPriority);
		collided = true;
	}

	for (i = 0; i < g_nHotSpots; i++)
	{
		if (g_hotSpots[i].isOn)
		{
			action = g_hotSpots[i].action;
			if ((action == kDissolveIt) || (action == kRewardIt) ||
					(action == kSwitchIt) || (action == kTriggerIt) ||
					(action == kBounceIt))
			{
				if (g_bands[who].dest.bottom < g_hotSpots[i].bounds.top)
					collided = false;
				else if (g_bands[who].dest.top > g_hotSpots[i].bounds.bottom)
					collided = false;
				else if (g_bands[who].dest.right < g_hotSpots[i].bounds.left)
					collided = false;
				else if (g_bands[who].dest.left > g_hotSpots[i].bounds.right)
					collided = false;
				else
					collided = true;

				if (collided)
				{
					// we have detected a collision
					nothingCollided = false;
					// don't count it if same as last frame
					// we don't want rapid on/off toggles
					if (g_bandHitLast != i)
					{
						// note who so we don't double-toggle it
						g_bandHitLast = i;
						if ((action == kDissolveIt) || (action == kBounceIt))
						{
							if (g_bands[who].hVel > 0)
							{
								if ((g_bands[who].dest.right - g_bands[who].hVel) <
										g_hotSpots[i].bounds.left)
								{
									g_bands[who].hVel = -g_bands[who].hVel;
									g_bands[who].dest.right = g_hotSpots[i].bounds.left;
									g_bands[who].dest.left = g_bands[who].dest.right - 16;
								}
								else
									g_bands[who].mode = kKillBandMode;
							}
							else
							{
								if ((g_bands[who].dest.left - g_bands[who].hVel) >
										g_hotSpots[i].bounds.right)
								{
									g_bands[who].hVel = -g_bands[who].hVel;
									g_bands[who].dest.left = g_hotSpots[i].bounds.right;
									g_bands[who].dest.right = g_bands[who].dest.left + 16;
								}
								else
									g_bands[who].mode = kKillBandMode;
							}
							PlayPrioritySound(kBandReboundSound, kBandReboundPriority);
							break;
						}
						else if (action == kRewardIt)
						{
							whoLinked = g_hotSpots[i].who;
							if ((g_masterObjects[whoLinked].theObject.what == kGreaseRt) ||
									(g_masterObjects[whoLinked].theObject.what == kGreaseLf))
							{
								if (SetObjectState(g_thisRoomNumber,
										g_masterObjects[whoLinked].objectNum, 0, whoLinked))
									SpillGrease(g_masterObjects[whoLinked].dynaNum,
											g_masterObjects[whoLinked].hotNum);
								g_hotSpots[i].isOn = false;
							}
						}
						else if (action == kSwitchIt)
						{
							HandleSwitches(&g_hotSpots[i]);
						}
						else if (action == kTriggerIt)
						{
							ArmTrigger(&g_hotSpots[i]);
						}
					}
				}
			}
		}
	}

	if (nothingCollided)  // the rubberband has hit nothing
		g_bandHitLast = -1;  // so make note of that for the next time

	if (g_bands[who].hVel != 0)
	{
		if (g_bands[who].dest.bottom < g_theGlider.dest.top)
			collided = false;
		else if (g_bands[who].dest.top > g_theGlider.dest.bottom)
			collided = false;
		else if (g_bands[who].dest.right < g_theGlider.dest.left)
			collided = false;
		else if (g_bands[who].dest.left > g_theGlider.dest.right)
			collided = false;
		else
			collided = true;

		if (collided)
		{
			if ((!g_twoPlayerGame) || (!g_onePlayerLeft) || (g_playerDead == kPlayer2))
			{
				g_theGlider.hVel += (g_bands[who].hVel / 2);
				g_bands[who].hVel = 0;
				PlayPrioritySound(kHitWallSound, kHitWallPriority);
			}
		}

		if (g_twoPlayerGame)
		{
			if (g_bands[who].dest.bottom < g_theGlider2.dest.top)
				collided = false;
			else if (g_bands[who].dest.top > g_theGlider2.dest.bottom)
				collided = false;
			else if (g_bands[who].dest.right < g_theGlider2.dest.left)
				collided = false;
			else if (g_bands[who].dest.left > g_theGlider2.dest.right)
				collided = false;
			else
				collided = true;

			if (collided)
			{
				if ((!g_onePlayerLeft) || (g_playerDead == kPlayer1))
				{
					g_theGlider2.hVel += (g_bands[who].hVel / 2);
					g_bands[who].hVel = 0;
					PlayPrioritySound(kHitWallSound, kHitWallPriority);
				}
			}
		}
	}
	if ((g_bands[who].dest.left < kLeftWallLimit) ||
			(g_bands[who].dest.right > kRightWallLimit))
	{
		g_bands[who].mode = kKillBandMode;
	}
	else if (g_bands[who].dest.bottom > kFloorLimit)
	{
		g_bands[who].mode = kKillBandMode;
	}
}

//--------------------------------------------------------------  HandleBands

void HandleBands (void)
{
	Rect dest;
	SInt16 i, count;

	if (g_numBands == 0)
		return;

	for (i = 0; i < g_numBands; i++)
	{
		g_bands[i].mode++;
		if (g_bands[i].mode > 2)
			g_bands[i].mode = 0;

		g_bands[i].count++;
		if (g_bands[i].count >= kBandFallCount)
		{
			g_bands[i].vVel++;
			g_bands[i].count = 0;
		}

		dest = g_bands[i].dest;
		QOffsetRect(&dest, g_playOriginH, g_playOriginV);
		AddRectToWorkRects(&dest);

		g_bands[i].dest.left += g_bands[i].hVel;
		g_bands[i].dest.right += g_bands[i].hVel;
		g_bands[i].dest.top += g_bands[i].vVel;
		g_bands[i].dest.bottom += g_bands[i].vVel;

		CheckBandCollision(i);
	}

	count = 0;
	do
	{
		while (g_bands[count].mode == kKillBandMode)
		{
			g_bands[count].mode = 0;
			KillBand(count);
		}
		count++;
	}
	while (count < g_numBands);
}

//--------------------------------------------------------------  AddBand

Boolean AddBand (gliderPtr thisGlider, SInt16 h, SInt16 v, Boolean direction)
{
	if (g_numBands >= kMaxRubberBands)
		return (false);

	g_bands[g_numBands].mode = 0;
	g_bands[g_numBands].count = 0;
	if (thisGlider->tipped)
		g_bands[g_numBands].vVel = -2;
	else
		g_bands[g_numBands].vVel = 0;
	g_bands[g_numBands].dest.left = h - 8;
	g_bands[g_numBands].dest.right = h + 8;
	g_bands[g_numBands].dest.top = v - 3;
	g_bands[g_numBands].dest.bottom = v + 3;

	if (direction == kFaceLeft)
	{
		g_bands[g_numBands].dest.left -= 32;
		g_bands[g_numBands].dest.right -= 32;
		g_bands[g_numBands].hVel = -kRubberBandVelocity;
	}
	else
	{
		g_bands[g_numBands].dest.left += 32;
		g_bands[g_numBands].dest.right += 32;
		g_bands[g_numBands].hVel = kRubberBandVelocity;
	}

	thisGlider->hVel -= (g_bands[g_numBands].hVel / 2);
	g_numBands++;

	PlayPrioritySound(kFireBandSound, kFireBandPriority);
	return (true);
}

//--------------------------------------------------------------  KillBand

void KillBand (SInt16 which)
{
	SInt16 lastBand;

	lastBand = g_numBands - 1;
	if (which != lastBand)
		g_bands[which] = g_bands[lastBand];

	g_numBands--;
}

//--------------------------------------------------------------  KillAllBands

void KillAllBands (void)
{
	SInt16 i;

	for (i = 0; i < kMaxRubberBands; i++)
	{
		g_bands[i].mode = 0;
	}

	g_numBands = 0;
}
