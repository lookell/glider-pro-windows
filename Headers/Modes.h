#ifndef MODES_H_
#define MODES_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Modes.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderStructs.h"
#include "MacTypes.h"

extern SInt16 saidFollow;

void StartGliderFadingIn (gliderPtr thisGlider);
void StartGliderTransportingIn (gliderPtr thisGlider);
void StartGliderFadingOut (gliderPtr thisGlider);
void StartGliderGoingUpStairs (gliderPtr thisGlider);
void StartGliderGoingDownStairs (gliderPtr thisGlider);
void StartGliderMailingIn (gliderPtr thisGlider, const Rect *bounds, const hotObject *who);
void StartGliderMailingOut (gliderPtr thisGlider);
void StartGliderDuctingDown (gliderPtr thisGlider, const Rect *bounds, const hotObject *who);
void StartGliderDuctingUp (gliderPtr thisGlider, const Rect *bounds, const hotObject *who);
void StartGliderDuctingIn (gliderPtr thisGlider);
void StartGliderTransporting (gliderPtr thisGlider, const hotObject *who);
void FlagGliderNormal (gliderPtr thisGlider);
void FlagGliderShredding (gliderPtr thisGlider, const Rect *bounds);
void FlagGliderBurning (gliderPtr thisGlider);
void FlagGliderFaceLeft (gliderPtr thisGlider);
void FlagGliderFaceRight (gliderPtr thisGlider);
void FlagGliderInLimbo (gliderPtr thisGlider, Boolean sayIt);
void UndoGliderLimbo (gliderPtr thisGlider);
void ToggleGliderFacing (gliderPtr thisGlider);
void InsureGliderFacingRight (gliderPtr thisGlider);
void InsureGliderFacingLeft (gliderPtr thisGlider);
void ReadyGliderForTripUpStairs (gliderPtr thisGlider);
void ReadyGliderForTripDownStairs (gliderPtr thisGlider);
void StartGliderFoilGoing (gliderPtr thisGlider);
void StartGliderFoilLosing (gliderPtr thisGlider);
void TagGliderIdle (gliderPtr thisGlider);

#endif
