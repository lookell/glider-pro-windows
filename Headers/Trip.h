#ifndef TRIP_H_
#define TRIP_H_

//============================================================================
//----------------------------------------------------------------------------
//                                   Trip.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"

void ToggleToaster (SInt16 index);
void ToggleMacPlus (SInt16 index);
void ToggleTV (SInt16 index);
void ToggleCoffee (SInt16 index);
void ToggleOutlet (SInt16 index);
void ToggleVCR (SInt16 index);
void ToggleStereos (SInt16 index);
void ToggleMicrowave (SInt16 index);
void ToggleBalloon (SInt16 index);
void ToggleCopter (SInt16 index);
void ToggleDart (SInt16 index);
void ToggleBall (SInt16 index);
void ToggleDrip (SInt16 index);
void ToggleFish (SInt16 index);
void TriggerSwitch (SInt16 who);
void TriggerToast (SInt16 who);
void TriggerOutlet (SInt16 who);
void TriggerDrip (SInt16 who);
void TriggerFish (SInt16 who);
void TriggerBalloon (SInt16 who);
void TriggerCopter (SInt16 who);
void TriggerDart (SInt16 who);
void UpdateOutletsLighting (SInt16 room, SInt16 nLights);

#endif
