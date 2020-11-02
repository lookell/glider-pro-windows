#ifndef PLAY_H_
#define PLAY_H_

//============================================================================
//----------------------------------------------------------------------------
//                                   Play.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderStructs.h"
#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

extern Rect glidSrcRect;
extern Rect justRoomsRect;
#ifdef GP_USE_WINAPI_H
extern HDC glidSrcMap;
extern HDC glid2SrcMap;
extern HDC glidMaskMap;
#endif
extern SInt32 gameFrame;
extern SInt16 batteryTotal;
extern SInt16 bandsTotal;
extern SInt16 foilTotal;
extern SInt16 mortals;
extern Boolean playing;
extern Boolean evenFrame;
extern Boolean twoPlayerGame;
extern Boolean showFoil;
extern Boolean demoGoing;
extern Boolean playerSuicide;
extern Boolean phoneBitSet;
extern Boolean tvOn;

#ifdef GP_USE_WINAPI_H
void NewGame (HWND ownerWindow, SInt16 mode, SInt16 splashHouseIndex);
void DoDemoGame (HWND ownerWindow);
#endif
void HideGlider (const gliderType *thisGlider);
void StrikeChime (void);

#endif
