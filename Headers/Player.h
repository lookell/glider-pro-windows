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
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

extern gliderType theGlider;
extern gliderType theGlider2;
extern Rect shadowSrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC shadowSrcMap;
extern HDC shadowMaskMap;
#endif
extern Rect shadowSrc[kNumShadowSrcRects];
extern Rect gliderSrc[kNumGliderSrcRects];
extern Rect transRect;
extern SInt32 theScore;
extern SInt16 fadeInSequence[kLastFadeSequence];
extern SInt16 rightClip;
extern SInt16 leftClip;
extern SInt16 transRoom;
extern Boolean shadowVisible;
extern Boolean onePlayerLeft;
extern Boolean playerDead;

void FinishGliderUpStairs (gliderPtr thisGlider);
void FinishGliderDownStairs (gliderPtr thisGlider);
void FinishGliderDuctingIn (gliderPtr thisGlider);
void DeckGliderInFoil (gliderPtr thisGlider);
void RemoveFoilFromGlider (gliderPtr thisGlider);
void HandleGlider (gliderPtr thisGlider);
void OffsetGlider (gliderPtr thisGlider, SInt16 where);
void OffAMortal (gliderPtr thisGlider);

#endif
