#include "Triggers.h"

//============================================================================
//----------------------------------------------------------------------------
//                                 Triggers.c
//----------------------------------------------------------------------------
//============================================================================


#include "Grease.h"
#include "House.h"
#include "Objects.h"
#include "Sound.h"
#include "Trip.h"


#define kMaxTriggers			16


typedef struct trigType
{
	SInt16 object;
	SInt16 room;
	SInt16 index;
	SInt16 timer;
	SInt16 what;
	Boolean armed;
} trigType;


SInt16 FindEmptyTriggerSlot (void);
void FireTrigger (SInt16 index);


static trigType triggers[kMaxTriggers];


//==============================================================  Functions
//--------------------------------------------------------------  ArmTrigger

void ArmTrigger (hotPtr who)
{
	SInt16		where, whoLinked;

	if (who->stillOver)
		return;

	where = FindEmptyTriggerSlot();

	if (where != -1)
	{
		whoLinked = who->who;				// what is trigger's obj. #
		triggers[where].room = masterObjects[whoLinked].roomLink;
		triggers[where].object = masterObjects[whoLinked].objectLink;
		triggers[where].index = whoLinked;
		triggers[where].timer = masterObjects[whoLinked].theObject.data.e.delay * 3;
		triggers[where].what = masterObjects[triggers[where].object].theObject.what;
		triggers[where].armed = true;
	}

	who->stillOver = true;
}

//--------------------------------------------------------------  FindEmptyTriggerSlot

SInt16 FindEmptyTriggerSlot (void)
{
	SInt16		where, i;

	where = -1;

	for (i = 0; i < kMaxTriggers; i++)
	{
		if (!triggers[i].armed)
		{
			where = i;
			break;
		}
	}

	return (where);
}

//--------------------------------------------------------------  HandleTriggers

void HandleTriggers (void)
{
	SInt16		i;

	for (i = 0; i < kMaxTriggers; i++)
	{
		if (triggers[i].armed)
		{
			triggers[i].timer--;
			if (triggers[i].timer <= 0)
			{
				triggers[i].timer = 0;
				triggers[i].armed = false;
				FireTrigger(i);
			}
		}
	}
}

//--------------------------------------------------------------  FireTrigger

void FireTrigger (SInt16 index)
{
	SInt16		triggerIs, triggeredIs;

	triggerIs = triggers[index].index;

	if (masterObjects[triggerIs].localLink != -1)
	{
		triggeredIs = masterObjects[triggerIs].localLink;
		switch (masterObjects[triggeredIs].theObject.what)
		{
			case kGreaseRt:
			case kGreaseLf:
			if (SetObjectState(triggers[index].room, triggers[index].object,
					kForceOn, triggeredIs))
			{
				SpillGrease(masterObjects[triggeredIs].dynaNum,
						masterObjects[triggeredIs].hotNum);
			}
			break;

			case kLightSwitch:
			case kMachineSwitch:
			case kThermostat:
			case kPowerSwitch:
			case kKnifeSwitch:
			case kInvisSwitch:
			TriggerSwitch(masterObjects[triggeredIs].dynaNum);
			break;

			case kSoundTrigger:
			PlayPrioritySound(kChordSound, kChordPriority);	// Change me
			break;

			case kToaster:
			TriggerToast(masterObjects[triggeredIs].dynaNum);
			break;

			case kGuitar:
			PlayPrioritySound(kChordSound, kChordPriority);
			break;

			case kCoffee:
			PlayPrioritySound(kCoffeeSound, kCoffeePriority);
			break;

			case kOutlet:
			TriggerOutlet(masterObjects[triggeredIs].dynaNum);
			break;

			case kBalloon:
			TriggerBalloon(masterObjects[triggeredIs].dynaNum);
			break;

			case kCopterLf:
			case kCopterRt:
			TriggerCopter(masterObjects[triggeredIs].dynaNum);
			break;

			case kDartLf:
			case kDartRt:
			TriggerDart(masterObjects[triggeredIs].dynaNum);
			break;

			case kDrip:
			TriggerDrip(masterObjects[triggeredIs].dynaNum);
			break;

			case kFish:
			TriggerFish(masterObjects[triggeredIs].dynaNum);
			break;
		}
	}
}

//--------------------------------------------------------------  ZeroTriggers

void ZeroTriggers (void)
{
	SInt16		i;

	for (i = 0; i < kMaxTriggers; i++)
		triggers[i].armed = false;
}

