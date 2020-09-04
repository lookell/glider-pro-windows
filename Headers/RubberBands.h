#ifndef RUBBER_BANDS_H_
#define RUBBER_BANDS_H_

//============================================================================
//----------------------------------------------------------------------------
//                               RubberBands.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderDefines.h"
#include "GliderStructs.h"
#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

extern bandType bands[kMaxRubberBands];
extern Rect bandsSrcRect;
extern Rect bandRects[3];
#ifdef GP_USE_WINAPI_H
extern HDC bandsSrcMap;
extern HDC bandsMaskMap;
#endif
extern SInt16 numBands;

void HandleBands (void);
Boolean AddBand (gliderPtr thisGlider, SInt16 h, SInt16 v, Boolean direction);
void KillAllBands (void);

#endif
