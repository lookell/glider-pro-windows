#ifndef PLAYER_H_
#define PLAYER_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Player.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderStructs.h"
#include "MacTypes.h"
#include "WinAPI.h"

extern HDC		shadowSrcMap;
extern HDC		shadowMaskMap;

void FinishGliderUpStairs (gliderPtr thisGlider);
void FinishGliderDownStairs (gliderPtr thisGlider);
void FinishGliderDuctingIn (gliderPtr thisGlider);
void DeckGliderInFoil (gliderPtr thisGlider);
void RemoveFoilFromGlider (gliderPtr thisGlider);
void HandleGlider (gliderPtr thisGlider);
void OffsetGlider (gliderPtr thisGlider, SInt16 where);
void OffAMortal (gliderPtr thisGlider);

#endif
