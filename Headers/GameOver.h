#ifndef GAME_OVER_H_
#define GAME_OVER_H_

//============================================================================
//----------------------------------------------------------------------------
//                                 GameOver.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

extern Rect g_angelSrcRect;
extern HDC g_angelSrcMap;
extern HDC g_angelMaskMap;
extern SInt16 g_countDown;
extern Boolean g_gameOver;

void DoGameOver (void);
void FlagGameOver (void);
void DoDiedGameOver (void);

#endif
