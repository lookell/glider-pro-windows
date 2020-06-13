#ifndef HIGH_SCORES_H_
#define HIGH_SCORES_H_

//============================================================================
//----------------------------------------------------------------------------
//                                HighScores.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

void DoHighScores (void);
void SortHighScores (void);
void ZeroHighScores (void);
void ZeroAllButHighestScore (void);
Boolean TestHighScore (HWND ownerWindow);
Boolean WriteScoresToDisk (HWND ownerWindow);
Boolean ReadScoresFromDisk (HWND ownerWindow);

#endif
