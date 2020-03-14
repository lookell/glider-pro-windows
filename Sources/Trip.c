
//============================================================================
//----------------------------------------------------------------------------
//								   	Trip.c
//----------------------------------------------------------------------------
//============================================================================


#include "Externs.h"
#include "Environ.h"


extern	dynaPtr		dinahs;
extern	hotPtr		hotSpots;
extern	SInt16		numDynamics, tvWithMovieNumber;
extern	Boolean		tvOn;


//==============================================================  Functions
//--------------------------------------------------------------  ToggleToaster

void ToggleToaster (SInt16 index)
{
	return;
#if 0
	dinahs[index].active = !dinahs[index].active;
#endif
}

//--------------------------------------------------------------  ToggleMacPlus

void ToggleMacPlus (SInt16 index)
{
	return;
#if 0
	dinahs[index].active = !dinahs[index].active;
	if (dinahs[index].active)
		dinahs[index].timer = 40;
	else
		dinahs[index].timer = 10;
#endif
}

//--------------------------------------------------------------  ToggleTV

void ToggleTV (SInt16 index)
{
	return;
#if 0
	dinahs[index].active = !dinahs[index].active;
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
	dinahs[index].timer = 4;
#endif
}

//--------------------------------------------------------------  ToggleCoffee

void ToggleCoffee (SInt16 index)
{
	return;
#if 0
	dinahs[index].active = !dinahs[index].active;
	dinahs[index].timer = 4;
#endif
}

//--------------------------------------------------------------  ToggleOutlet

void ToggleOutlet (SInt16 index)
{
	return;
#if 0
	dinahs[index].active = !dinahs[index].active;
#endif
}

//--------------------------------------------------------------  ToggleVCR

void ToggleVCR (SInt16 index)
{
	return;
#if 0
	dinahs[index].active = !dinahs[index].active;
	dinahs[index].timer = 4;
#endif
}

//--------------------------------------------------------------  ToggleStereo

void ToggleStereos (SInt16 index)
{
	return;
#if 0
	if (dinahs[index].timer == 0)
	{
		dinahs[index].active = !dinahs[index].active;
		dinahs[index].timer = 4;
	}
#endif
}

//--------------------------------------------------------------  ToggleMicrowave

void ToggleMicrowave (SInt16 index)
{
	return;
#if 0
	dinahs[index].active = !dinahs[index].active;
	dinahs[index].timer = 4;
#endif
}

//--------------------------------------------------------------  ToggleBalloon

void ToggleBalloon (SInt16 index)
{
	return;
#if 0
	dinahs[index].active = !dinahs[index].active;
#endif
}

//--------------------------------------------------------------  ToggleCopter

void ToggleCopter (SInt16 index)
{
	return;
#if 0
	dinahs[index].active = !dinahs[index].active;
#endif
}

//--------------------------------------------------------------  ToggleDart

void ToggleDart (SInt16 index)
{
	return;
#if 0
	dinahs[index].active = !dinahs[index].active;
#endif
}

//--------------------------------------------------------------  ToggleBall

void ToggleBall (SInt16 index)
{
	return;
#if 0
	dinahs[index].active = !dinahs[index].active;
#endif
}

//--------------------------------------------------------------  ToggleDrip

void ToggleDrip (SInt16 index)
{
	return;
#if 0
	dinahs[index].active = !dinahs[index].active;
#endif
}

//--------------------------------------------------------------  ToggleFish

void ToggleFish (SInt16 index)
{
	return;
#if 0
	dinahs[index].active = !dinahs[index].active;
#endif
}

//--------------------------------------------------------------  TriggerSwitch

void TriggerSwitch (SInt16 who)
{
	return;
#if 0
	HandleSwitches(&hotSpots[who]);
#endif
}

//--------------------------------------------------------------  TriggerToast

void TriggerToast (SInt16 who)
{
	return;
#if 0
	if (!dinahs[who].moving)
	{
		if (dinahs[who].active)
		{
			dinahs[who].vVel = (short)-dinahs[who].count;
			dinahs[who].frame = 0;
			dinahs[who].moving = true;
			PlayPrioritySound(kToastLaunchSound, kToastLaunchPriority);
		}
		else
			dinahs[who].frame = dinahs[who].timer;
	}
#endif
}

//--------------------------------------------------------------  TriggerOutlet

void TriggerOutlet (SInt16 who)
{
	return;
#if 0
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
#endif
}

//--------------------------------------------------------------  TriggerDrip

void TriggerDrip (SInt16 who)
{
	return;
#if 0
	if ((!dinahs[who].moving) && (dinahs[who].timer > 7))
		dinahs[who].timer = 7;		// kick off drip
#endif
}

//--------------------------------------------------------------  TriggerFish

void TriggerFish (SInt16 who)
{
	return;
#if 0
	if ((dinahs[who].active) && (!dinahs[who].moving))
	{
		dinahs[who].whole = dinahs[who].dest;
		dinahs[who].moving = true;
		dinahs[who].frame = 4;
		PlayPrioritySound(kFishOutSound, kFishOutPriority);
	}
#endif
}

//--------------------------------------------------------------  TriggerBalloon

void TriggerBalloon (SInt16 who)
{
	return;
#if 0
	if (!dinahs[who].moving)
		dinahs[who].timer = kStartSparkle + 1;
#endif
}


//--------------------------------------------------------------  TriggerCopter

void TriggerCopter (SInt16 who)
{
	return;
#if 0
	if (!dinahs[who].moving)
		dinahs[who].timer = kStartSparkle + 1;
#endif
}


//--------------------------------------------------------------  TriggerDart

void TriggerDart (SInt16 who)
{
	return;
#if 0
	if (!dinahs[who].moving)
		dinahs[who].timer = kStartSparkle + 1;
#endif
}

//--------------------------------------------------------------  UpdateOutletsLighting

void UpdateOutletsLighting (SInt16 room, SInt16 nLights)
{
	return;
#if 0
	short		i;

	for (i = 0; i < numDynamics; i++)
	{
		if ((dinahs[i].type == kOutlet) && (dinahs[i].room == room))
			dinahs[i].hVel = nLights;
	}
#endif
}

