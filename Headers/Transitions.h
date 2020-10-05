#ifndef TRANSITIONS_H_
#define TRANSITIONS_H_

//============================================================================
//----------------------------------------------------------------------------
//                               Transitions.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"

void WipeScreenOn (SInt16 direction, const Rect *theRect);
void DumpScreenOn (const Rect *theRect);
void DissolveScreenOn (const Rect *theRect);

#endif
