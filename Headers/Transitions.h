#ifndef TRANSITIONS_H_
#define TRANSITIONS_H_

//============================================================================
//----------------------------------------------------------------------------
//                               Transitions.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"

void PourScreenOn (Rect *theRect);
void WipeScreenOn (SInt16 direction, Rect *theRect);
void DumpScreenOn (Rect *theRect);
void DissBits (Rect *theRect);
void DissBitsChunky (Rect *theRect);
//void FillColorNoise (Rect *theRect);
//void FillSnow (Rect *theRect);

#endif
