#include "Trip.h"

//============================================================================
//----------------------------------------------------------------------------
//                                   Trip.c
//----------------------------------------------------------------------------
//============================================================================


#include "Dynamics.h"
#include "Environ.h"
#include "Interactions.h"
#include "Objects.h"
#include "Play.h"
#include "Sound.h"


//==============================================================  Functions
//--------------------------------------------------------------  ToggleToaster

void ToggleToaster (SInt16 index)
{
	dinahs[index].active = !dinahs[index].active;
}

//--------------------------------------------------------------  ToggleMacPlus

void ToggleMacPlus (SInt16 index)
{
	dinahs[index].active = !dinahs[index].active;
	if (dinahs[index].active)
		dinahs[index].timer = 40;
	else
		dinahs[index].timer = 10;
}

//--------------------------------------------------------------  ToggleTV

void ToggleTV (SInt16 index)
{
	dinahs[index].active = !dinahs[index].active;
#ifdef COMPILEQT
	if ((thisMac.hasQT) && (hasMovie) && (tvInRoom) && (tvWithMovieNumber == index))
	{
		if (dinahs[index].active)
		{
			GoToBeginningOfMovie(theMovie);
			StartMovie(theMovie);
			tvOn = true;
		}
		else
		{
			StopMovie(theMovie);
			tvOn = false;
		}
	}
#endif
	dinahs[index].timer = 4;
}

//--------------------------------------------------------------  ToggleCoffee

void ToggleCoffee (SInt16 index)
{
	dinahs[index].active = !dinahs[index].active;
	dinahs[index].timer = 4;
}

//--------------------------------------------------------------  ToggleOutlet

void ToggleOutlet (SInt16 index)
{
	dinahs[index].active = !dinahs[index].active;
}

//--------------------------------------------------------------  ToggleVCR

void ToggleVCR (SInt16 index)
{
	dinahs[index].active = !dinahs[index].active;
	dinahs[index].timer = 4;
}

//--------------------------------------------------------------  ToggleStereo

void ToggleStereos (SInt16 index)
{
	if (dinahs[index].timer == 0)
	{
		dinahs[index].active = !dinahs[index].active;
		dinahs[index].timer = 4;
	}
}

//--------------------------------------------------------------  ToggleMicrowave

void ToggleMicrowave (SInt16 index)
{
	dinahs[index].active = !dinahs[index].active;
	dinahs[index].timer = 4;
}

//--------------------------------------------------------------  ToggleBalloon

void ToggleBalloon (SInt16 index)
{
	dinahs[index].active = !dinahs[index].active;
}

//--------------------------------------------------------------  ToggleCopter

void ToggleCopter (SInt16 index)
{
	dinahs[index].active = !dinahs[index].active;
}

//--------------------------------------------------------------  ToggleDart

void ToggleDart (SInt16 index)
{
	dinahs[index].active = !dinahs[index].active;
}

//--------------------------------------------------------------  ToggleBall

void ToggleBall (SInt16 index)
{
	dinahs[index].active = !dinahs[index].active;
}

//--------------------------------------------------------------  ToggleDrip

void ToggleDrip (SInt16 index)
{
	dinahs[index].active = !dinahs[index].active;
}

//--------------------------------------------------------------  ToggleFish

void ToggleFish (SInt16 index)
{
	dinahs[index].active = !dinahs[index].active;
}

//--------------------------------------------------------------  TriggerSwitch

void TriggerSwitch (SInt16 who)
{
	HandleSwitches(&hotSpots[who]);
}

//--------------------------------------------------------------  TriggerToast

void TriggerToast (SInt16 who)
{
	if (!dinahs[who].moving)
	{
		if (dinahs[who].active)
		{
			dinahs[who].vVel = -dinahs[who].count;
			dinahs[who].frame = 0;
			dinahs[who].moving = true;
			PlayPrioritySound(kToastLaunchSound, kToastLaunchPriority);
		}
		else
			dinahs[who].frame = dinahs[who].timer;
	}
}

//--------------------------------------------------------------  TriggerOutlet

void TriggerOutlet (SInt16 who)
{
	if (dinahs[who].position == 0)
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

//--------------------------------------------------------------  TriggerDrip

void TriggerDrip (SInt16 who)
{
	if ((!dinahs[who].moving) && (dinahs[who].timer > 7))
		dinahs[who].timer = 7;		// kick off drip
}

//--------------------------------------------------------------  TriggerFish

void TriggerFish (SInt16 who)
{
	if ((dinahs[who].active) && (!dinahs[who].moving))
	{
		dinahs[who].whole = dinahs[who].dest;
		dinahs[who].moving = true;
		dinahs[who].frame = 4;
		PlayPrioritySound(kFishOutSound, kFishOutPriority);
	}
}

//--------------------------------------------------------------  TriggerBalloon

void TriggerBalloon (SInt16 who)
{
	if (!dinahs[who].moving)
		dinahs[who].timer = kStartSparkle + 1;
}


//--------------------------------------------------------------  TriggerCopter

void TriggerCopter (SInt16 who)
{
	if (!dinahs[who].moving)
		dinahs[who].timer = kStartSparkle + 1;
}


//--------------------------------------------------------------  TriggerDart

void TriggerDart (SInt16 who)
{
	if (!dinahs[who].moving)
		dinahs[who].timer = kStartSparkle + 1;
}

//--------------------------------------------------------------  UpdateOutletsLighting

void UpdateOutletsLighting (SInt16 room, SInt16 nLights)
{
	SInt16		i;

	for (i = 0; i < numDynamics; i++)
	{
		if ((dinahs[i].type == kOutlet) && (dinahs[i].room == room))
			dinahs[i].hVel = nLights;
	}
}

