//============================================================================
//----------------------------------------------------------------------------
//                               RubberBands.h
//----------------------------------------------------------------------------
//============================================================================

#ifndef RUBBER_BANDS_H_
#define RUBBER_BANDS_H_

#include "GliderDefines.h"
#include "GliderStructs.h"
#include "MacTypes.h"
#include "WinAPI.h"

extern bandType g_bands[kMaxRubberBands];
extern Rect g_bandsSrcRect;
extern Rect g_bandRects[3];
extern HDC g_bandsSrcMap;
extern HDC g_bandsMaskMap;
extern SInt16 g_numBands;

void HandleBands (void);
Boolean AddBand (gliderPtr thisGlider, SInt16 h, SInt16 v, Boolean direction);
void KillAllBands (void);

#endif
