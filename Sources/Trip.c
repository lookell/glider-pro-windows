#include "Trip.h"

//============================================================================
//----------------------------------------------------------------------------
//                                   Trip.c
//----------------------------------------------------------------------------
//============================================================================

#include "Dynamics.h"
#include "Environ.h"
#include "HouseIO.h"
#include "Interactions.h"
#include "Objects.h"
#include "Play.h"
#include "Sound.h"

//==============================================================  Functions
//--------------------------------------------------------------  ToggleToaster

void ToggleToaster (SInt16 index)
{
	if (index < 0 || index >= g_numDynamics)
		return;

	g_dinahs[index].active = !g_dinahs[index].active;
}

//--------------------------------------------------------------  ToggleMacPlus

void ToggleMacPlus (SInt16 index)
{
	if (index < 0 || index >= g_numDynamics)
		return;

	g_dinahs[index].active = !g_dinahs[index].active;
	if (g_dinahs[index].active)
		g_dinahs[index].timer = 40;
	else
		g_dinahs[index].timer = 10;
}

//--------------------------------------------------------------  ToggleTV

void ToggleTV (SInt16 index)
{
	if (index < 0 || index >= g_numDynamics)
		return;

	g_dinahs[index].active = !g_dinahs[index].active;
#ifdef COMPILEQT
	if ((g_thisMac.hasQT) && (g_hasMovie) && (g_tvInRoom) && (g_tvWithMovieNumber == index))
	{
		if (g_dinahs[index].active)
		{
			GoToBeginningOfMovie(g_theMovie);
			StartMovie(g_theMovie);
			g_tvOn = true;
		}
		else
		{
			StopMovie(g_theMovie);
			g_tvOn = false;
		}
	}
#endif
	g_dinahs[index].timer = 4;
}

//--------------------------------------------------------------  ToggleCoffee

void ToggleCoffee (SInt16 index)
{
	if (index < 0 || index >= g_numDynamics)
		return;

	g_dinahs[index].active = !g_dinahs[index].active;
	g_dinahs[index].timer = 4;
}

//--------------------------------------------------------------  ToggleOutlet

void ToggleOutlet (SInt16 index)
{
	if (index < 0 || index >= g_numDynamics)
		return;

	g_dinahs[index].active = !g_dinahs[index].active;
}

//--------------------------------------------------------------  ToggleVCR

void ToggleVCR (SInt16 index)
{
	if (index < 0 || index >= g_numDynamics)
		return;

	g_dinahs[index].active = !g_dinahs[index].active;
	g_dinahs[index].timer = 4;
}

//--------------------------------------------------------------  ToggleStereo

void ToggleStereos (SInt16 index)
{
	if (index < 0 || index >= g_numDynamics)
		return;

	if (g_dinahs[index].timer == 0)
	{
		g_dinahs[index].active = !g_dinahs[index].active;
		g_dinahs[index].timer = 4;
	}
}

//--------------------------------------------------------------  ToggleMicrowave

void ToggleMicrowave (SInt16 index)
{
	if (index < 0 || index >= g_numDynamics)
		return;

	g_dinahs[index].active = !g_dinahs[index].active;
	g_dinahs[index].timer = 4;
}

//--------------------------------------------------------------  ToggleBalloon

void ToggleBalloon (SInt16 index)
{
	if (index < 0 || index >= g_numDynamics)
		return;

	g_dinahs[index].active = !g_dinahs[index].active;
}

//--------------------------------------------------------------  ToggleCopter

void ToggleCopter (SInt16 index)
{
	if (index < 0 || index >= g_numDynamics)
		return;

	g_dinahs[index].active = !g_dinahs[index].active;
}

//--------------------------------------------------------------  ToggleDart

void ToggleDart (SInt16 index)
{
	if (index < 0 || index >= g_numDynamics)
		return;

	g_dinahs[index].active = !g_dinahs[index].active;
}

//--------------------------------------------------------------  ToggleBall

void ToggleBall (SInt16 index)
{
	if (index < 0 || index >= g_numDynamics)
		return;

	g_dinahs[index].active = !g_dinahs[index].active;
}

//--------------------------------------------------------------  ToggleDrip

void ToggleDrip (SInt16 index)
{
	if (index < 0 || index >= g_numDynamics)
		return;

	g_dinahs[index].active = !g_dinahs[index].active;
}

//--------------------------------------------------------------  ToggleFish

void ToggleFish (SInt16 index)
{
	if (index < 0 || index >= g_numDynamics)
		return;

	g_dinahs[index].active = !g_dinahs[index].active;
}

//--------------------------------------------------------------  TriggerSwitch

void TriggerSwitch (SInt16 who)
{
	if (who < 0 || who >= g_nHotSpots)
		return;

	HandleSwitches(&g_hotSpots[who]);
}

//--------------------------------------------------------------  TriggerToast

void TriggerToast (SInt16 who)
{
	if (who < 0 || who >= g_numDynamics)
		return;

	if (!g_dinahs[who].moving)
	{
		if (g_dinahs[who].active)
		{
			g_dinahs[who].vVel = -g_dinahs[who].count;
			g_dinahs[who].frame = 0;
			g_dinahs[who].moving = true;
			PlayPrioritySound(kToastLaunchSound, kToastLaunchPriority);
		}
		else
		{
			g_dinahs[who].frame = g_dinahs[who].timer;
		}
	}
}

//--------------------------------------------------------------  TriggerOutlet

void TriggerOutlet (SInt16 who)
{
	if (who < 0 || who >= g_numDynamics)
		return;

	if (g_dinahs[who].position == 0)
	{
		if (g_dinahs[who].active)
		{
			g_dinahs[who].position = 1;
			g_dinahs[who].timer = kLengthOfZap;
			PlayPrioritySound(kZapSound, kZapPriority);
		}
		else
		{
			g_dinahs[who].timer = g_dinahs[who].count;
		}
	}
}

//--------------------------------------------------------------  TriggerDrip

void TriggerDrip (SInt16 who)
{
	if (who < 0 || who >= g_numDynamics)
		return;

	if ((!g_dinahs[who].moving) && (g_dinahs[who].timer > 7))
		g_dinahs[who].timer = 7;		// kick off drip
}

//--------------------------------------------------------------  TriggerFish

void TriggerFish (SInt16 who)
{
	if (who < 0 || who >= g_numDynamics)
		return;

	if ((g_dinahs[who].active) && (!g_dinahs[who].moving))
	{
		g_dinahs[who].whole = g_dinahs[who].dest;
		g_dinahs[who].moving = true;
		g_dinahs[who].frame = 4;
		PlayPrioritySound(kFishOutSound, kFishOutPriority);
	}
}

//--------------------------------------------------------------  TriggerBalloon

void TriggerBalloon (SInt16 who)
{
	if (who < 0 || who >= g_numDynamics)
		return;

	if (!g_dinahs[who].moving)
		g_dinahs[who].timer = kStartSparkle + 1;
}

//--------------------------------------------------------------  TriggerCopter

void TriggerCopter (SInt16 who)
{
	if (who < 0 || who >= g_numDynamics)
		return;

	if (!g_dinahs[who].moving)
		g_dinahs[who].timer = kStartSparkle + 1;
}

//--------------------------------------------------------------  TriggerDart

void TriggerDart (SInt16 who)
{
	if (who < 0 || who >= g_numDynamics)
		return;

	if (!g_dinahs[who].moving)
		g_dinahs[who].timer = kStartSparkle + 1;
}

//--------------------------------------------------------------  UpdateOutletsLighting

void UpdateOutletsLighting (SInt16 room, SInt16 nLights)
{
	SInt16 i;

	for (i = 0; i < g_numDynamics; i++)
	{
		if ((g_dinahs[i].type == kOutlet) && (g_dinahs[i].room == room))
			g_dinahs[i].hVel = nLights;
	}
}
