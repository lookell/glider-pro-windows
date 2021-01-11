//============================================================================
//----------------------------------------------------------------------------
//                               Interactions.h
//----------------------------------------------------------------------------
//============================================================================

#ifndef INTERACTIONS_H_
#define INTERACTIONS_H_

#include "GliderStructs.h"
#include "MacTypes.h"

extern SInt16 g_otherPlayerEscaped;

Boolean SectGlider (const gliderType *thisGlider, const Rect *theRect, Boolean scrutinize);
void HandleSwitches (hotPtr who);
void HandleInteraction (void);
void FlagStillOvers (const gliderType *thisGlider);

#endif
