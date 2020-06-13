#ifndef DEBUG_UTILITIES_H_
#define DEBUG_UTILITIES_H_

//============================================================================
//----------------------------------------------------------------------------
//                              DebugUtilities.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"

void MonitorWait (void);
void DisplayRect (Rect *theRect);
void FlashRect (Rect *theRect);
void CheckLegitRect(Rect *srcRect, Rect *inRect);
void DisplayLong (SInt32 theValue);
void DisplayShort (SInt16 theValue);
void FlashLong (SInt32 theValue);
void FlashShort (SInt16 theValue);
void DoBarGraph (SInt16 theValue, SInt16 downScreen, SInt16 maxDown, SInt16 scaleIt);
SInt16 BetaOkay (void);
void DebugNum (SInt32 theNum);
void DisplayCTSeed (CGrafPtr who);
void FillScreenRed (void);
void DumpToResEditFile (Ptr data, SInt32 dataSize);

#endif
