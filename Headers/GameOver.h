#ifndef GAME_OVER_H_
#define GAME_OVER_H_

//============================================================================
//----------------------------------------------------------------------------
//                                 GameOver.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

extern Rect angelSrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC angelSrcMap;
extern HDC angelMaskMap;
#endif
extern SInt16 countDown;
extern Boolean gameOver;

void DoGameOver (void);
void FlagGameOver (void);
void DoDiedGameOver (void);

#endif
