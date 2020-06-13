#ifndef RUBBER_BANDS_H_
#define RUBBER_BANDS_H_

//============================================================================
//----------------------------------------------------------------------------
//                               RubberBands.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderStructs.h"
#include "MacTypes.h"
#include "WinAPI.h"

extern HDC		bandsSrcMap;
extern HDC		bandsMaskMap;

void HandleBands (void);
Boolean AddBand (gliderPtr thisGlider, SInt16 h, SInt16 v, Boolean direction);
void KillAllBands (void);

#endif
