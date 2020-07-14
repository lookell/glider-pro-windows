#ifndef PLAY_H_
#define PLAY_H_

//============================================================================
//----------------------------------------------------------------------------
//                                   Play.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderStructs.h"
#include "MacTypes.h"
#include "WinAPI.h"

extern Rect glidSrcRect;
extern Rect justRoomsRect;
extern HDC glidSrcMap;
extern HDC glid2SrcMap;
extern HDC glidMaskMap;
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
extern Boolean doBackground;
extern Boolean playerSuicide;
extern Boolean phoneBitSet;
extern Boolean tvOn;

void NewGame (HWND ownerWindow, SInt16 mode);
void DoDemoGame (HWND ownerWindow);
void HideGlider (const gliderType *thisGlider);
void StrikeChime (void);
void RestoreEntireGameScreen (void);

#endif
