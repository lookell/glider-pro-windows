#ifndef GAME_OVER_H_
#define GAME_OVER_H_

//============================================================================
//----------------------------------------------------------------------------
//                                 GameOver.h
//----------------------------------------------------------------------------
//============================================================================

#include "WinAPI.h"

extern HDC	angelSrcMap;
extern HDC	angelMaskMap;

void DoGameOver (void);
void FlagGameOver (void);
void DoDiedGameOver (void);

#endif
