#ifndef PLAYER_H_
#define PLAYER_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Player.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderDefines.h"
#include "GliderStructs.h"
#include "MacTypes.h"
#include "WinAPI.h"

extern gliderType g_theGlider;
extern gliderType g_theGlider2;
extern Rect g_shadowSrcRect;
extern HDC g_shadowSrcMap;
extern HDC g_shadowMaskMap;
extern Rect g_shadowSrc[kNumShadowSrcRects];
extern Rect g_gliderSrc[kNumGliderSrcRects];
extern Rect g_transRect;
extern SInt32 g_theScore;
extern SInt16 g_fadeInSequence[kLastFadeSequence];
extern SInt16 g_rightClip;
extern SInt16 g_leftClip;
extern SInt16 g_transRoom;
extern Boolean g_shadowVisible;
extern Boolean g_onePlayerLeft;
extern Boolean g_playerDead;

void FinishGliderUpStairs (gliderPtr thisGlider);
void FinishGliderDownStairs (gliderPtr thisGlider);
void DeckGliderInFoil (gliderPtr thisGlider);
void RemoveFoilFromGlider (gliderPtr thisGlider);
void HandleGlider (gliderPtr thisGlider);
void OffsetGlider (gliderPtr thisGlider, SInt16 where);

#endif
