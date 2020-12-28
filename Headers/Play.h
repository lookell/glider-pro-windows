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

extern Rect g_glidSrcRect;
extern Rect g_justRoomsRect;
extern HDC g_glidSrcMap;
extern HDC g_glid2SrcMap;
extern HDC g_glidMaskMap;
extern SInt32 g_gameFrame;
extern SInt16 g_batteryTotal;
extern SInt16 g_bandsTotal;
extern SInt16 g_foilTotal;
extern SInt16 g_mortals;
extern Boolean g_playing;
extern Boolean g_evenFrame;
extern Boolean g_twoPlayerGame;
extern Boolean g_showFoil;
extern Boolean g_demoGoing;
extern Boolean g_playerSuicide;
extern Boolean g_phoneBitSet;
extern Boolean g_tvOn;

void NewGame (HWND ownerWindow, SInt16 mode, SInt16 splashHouseIndex);
void DoDemoGame (HWND ownerWindow);
void HideGlider (const gliderType *thisGlider);
void StrikeChime (void);

#endif
