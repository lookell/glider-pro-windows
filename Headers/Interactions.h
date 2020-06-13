#ifndef INTERACTIONS_H_
#define INTERACTIONS_H_

//============================================================================
//----------------------------------------------------------------------------
//                               Interactions.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderStructs.h"
#include "MacTypes.h"

Boolean SectGlider (gliderPtr thisGlider, Rect *theRect, Boolean scrutinize);
void HandleSwitches (hotPtr who);
void HandleInteraction (void);
void FlagStillOvers (gliderPtr thisGlider);

#endif
