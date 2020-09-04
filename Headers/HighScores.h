#ifndef HIGH_SCORES_H_
#define HIGH_SCORES_H_

//============================================================================
//----------------------------------------------------------------------------
//                                HighScores.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderStructs.h"
#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

extern Str31 highBanner;
extern Str15 highName;
extern SInt16 lastHighScore;

void DoHighScores (void);
void SortHighScores (void);
void ZeroHighScores (housePtr house);
void ZeroAllButHighestScore (housePtr house);
#ifdef GP_USE_WINAPI_H
Boolean TestHighScore (HWND ownerWindow);
Boolean WriteScoresToDisk (HWND ownerWindow);
Boolean ReadScoresFromDisk (HWND ownerWindow);
#endif

#endif
