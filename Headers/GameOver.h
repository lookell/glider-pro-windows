#ifndef GAME_OVER_H_
#define GAME_OVER_H_

//============================================================================
//----------------------------------------------------------------------------
//                                 GameOver.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

extern Rect angelSrcRect;
extern HDC angelSrcMap;
extern HDC angelMaskMap;
extern SInt16 countDown;
extern Boolean gameOver;

void DoGameOver (void);
void FlagGameOver (void);
void DoDiedGameOver (void);

#endif
