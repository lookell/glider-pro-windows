#ifndef SCOREBOARD_H_
#define SCOREBOARD_H_

//============================================================================
//----------------------------------------------------------------------------
//                                Scoreboard.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

extern HDC		boardSrcMap;
extern HDC		badgeSrcMap;
extern HDC		boardTSrcMap;
extern HDC		boardGSrcMap;
extern HDC		boardPSrcMap;

void RefreshScoreboard (SInt16 mode);
void HandleDynamicScoreboard (void);
void QuickGlidersRefresh (void);
void QuickScoreRefresh (void);
void QuickBatteryRefresh (Boolean flash);
void QuickBandsRefresh (Boolean flash);
void QuickFoilRefresh (Boolean flash);
void AdjustScoreboardHeight (void);
void BlackenScoreboard (void);

//void HandleScore (void);

#endif
