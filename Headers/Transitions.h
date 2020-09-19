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
void DissBits (const Rect *theRect);
void DissBitsChunky (const Rect *theRect);

#endif
