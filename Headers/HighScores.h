#ifndef HIGH_SCORES_H_
#define HIGH_SCORES_H_

//============================================================================
//----------------------------------------------------------------------------
//                                HighScores.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderStructs.h"
#include "MacTypes.h"
#include "WinAPI.h"

extern Str31 g_highBanner;
extern Str15 g_highName;
extern SInt16 g_lastHighScore;

void DoHighScores (void);
void ZeroHighScores (housePtr house);
void ZeroAllButHighestScore (housePtr house);
Boolean TestHighScore (HWND ownerWindow);
Boolean WriteScoresToDisk (HWND ownerWindow);
Boolean ReadScoresFromDisk (HWND ownerWindow);

#endif
