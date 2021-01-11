//============================================================================
//----------------------------------------------------------------------------
//                                 Triggers.c
//----------------------------------------------------------------------------
//============================================================================

#include "Triggers.h"

#include "Grease.h"
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

static trigType g_triggers[kMaxTriggers];

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
		g_triggers[where].room = g_masterObjects[whoLinked].roomLink;
		g_triggers[where].object = g_masterObjects[whoLinked].objectLink;
		g_triggers[where].index = whoLinked;
		g_triggers[where].timer = g_masterObjects[whoLinked].theObject.data.e.delay * 3;
		g_triggers[where].what = g_masterObjects[g_triggers[where].object].theObject.what;
		g_triggers[where].armed = true;
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
		if (!g_triggers[i].armed)
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
		if (g_triggers[i].armed)
		{
			g_triggers[i].timer--;
			if (g_triggers[i].timer <= 0)
			{
				g_triggers[i].timer = 0;
				g_triggers[i].armed = false;
				FireTrigger(i);
			}
		}
	}
}

//--------------------------------------------------------------  FireTrigger

void FireTrigger (SInt16 index)
{
	SInt16		triggerIs, triggeredIs;

	triggerIs = g_triggers[index].index;

	if (g_masterObjects[triggerIs].localLink != -1)
	{
		triggeredIs = g_masterObjects[triggerIs].localLink;
		switch (g_masterObjects[triggeredIs].theObject.what)
		{
			case kGreaseRt:
			case kGreaseLf:
			if (SetObjectState(g_triggers[index].room, g_triggers[index].object,
					kForceOn, triggeredIs))
			{
				SpillGrease(g_masterObjects[triggeredIs].dynaNum,
						g_masterObjects[triggeredIs].hotNum);
			}
			break;

			case kLightSwitch:
			case kMachineSwitch:
			case kThermostat:
			case kPowerSwitch:
			case kKnifeSwitch:
			case kInvisSwitch:
			TriggerSwitch(g_masterObjects[triggeredIs].dynaNum);
			break;

			case kSoundTrigger:
			PlayPrioritySound(kChordSound, kChordPriority);	// Change me
			break;

			case kToaster:
			TriggerToast(g_masterObjects[triggeredIs].dynaNum);
			break;

			case kGuitar:
			PlayPrioritySound(kChordSound, kChordPriority);
			break;

			case kCoffee:
			PlayPrioritySound(kCoffeeSound, kCoffeePriority);
			break;

			case kOutlet:
			TriggerOutlet(g_masterObjects[triggeredIs].dynaNum);
			break;

			case kBalloon:
			TriggerBalloon(g_masterObjects[triggeredIs].dynaNum);
			break;

			case kCopterLf:
			case kCopterRt:
			TriggerCopter(g_masterObjects[triggeredIs].dynaNum);
			break;

			case kDartLf:
			case kDartRt:
			TriggerDart(g_masterObjects[triggeredIs].dynaNum);
			break;

			case kDrip:
			TriggerDrip(g_masterObjects[triggeredIs].dynaNum);
			break;

			case kFish:
			TriggerFish(g_masterObjects[triggeredIs].dynaNum);
			break;
		}
	}
}

//--------------------------------------------------------------  ZeroTriggers

void ZeroTriggers (void)
{
	SInt16		i;

	for (i = 0; i < kMaxTriggers; i++)
		g_triggers[i].armed = false;
}
