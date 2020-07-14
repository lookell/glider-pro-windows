#ifndef DYNAMICS_H_
#define DYNAMICS_H_

//============================================================================
//----------------------------------------------------------------------------
//                                 Dynamics.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderDefines.h"
#include "GliderStructs.h"
#include "MacTypes.h"

extern Rect breadSrc[kNumBreadPicts];
extern dynaPtr dinahs;
extern SInt16 numDynamics;

void CheckDynamicCollision (SInt16 who, gliderPtr thisGlider, Boolean doOffset);
Boolean DidBandHitDynamic (SInt16 who);
void RenderToast (SInt16 who);
void RenderBalloon (SInt16 who);
void RenderCopter (SInt16 who);
void RenderDart (SInt16 who);
void RenderBall (SInt16 who);
void RenderDrip (SInt16 who);
void RenderFish (SInt16 who);
void HandleSparkleObject (SInt16 who);
void HandleToast (SInt16 who);
void HandleMacPlus (SInt16 who);
void HandleTV (SInt16 who);
void HandleCoffee (SInt16 who);
void HandleOutlet (SInt16 who);
void HandleVCR (SInt16 who);
void HandleStereo (SInt16 who);
void HandleMicrowave (SInt16 who);
void HandleBalloon (SInt16 who);
void HandleCopter (SInt16 who);
void HandleDart (SInt16 who);
void HandleBall (SInt16 who);
void HandleDrip (SInt16 who);
void HandleFish (SInt16 who);
void HandleDynamics (void);
void RenderDynamics (void);
void ZeroDinahs (void);
SInt16 AddDynamicObject (SInt16 what, const Rect *where, const objectType *who,
	SInt16 room, SInt16 index, Boolean isOn);

#endif
