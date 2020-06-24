#ifndef SCOREBOARD_H_
#define SCOREBOARD_H_

//============================================================================
//----------------------------------------------------------------------------
//                                Scoreboard.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

extern Rect boardSrcRect;
extern Rect badgeSrcRect;
extern Rect boardDestRect;
extern HDC boardSrcMap;
extern HDC badgeSrcMap;
extern Rect boardTSrcRect;
extern Rect boardTDestRect;
extern HDC boardTSrcMap;
extern Rect boardGSrcRect;
extern Rect boardGDestRect;
extern HDC boardGSrcMap;
extern Rect boardPSrcRect;
extern Rect boardPDestRect;
extern Rect boardPQDestRect;
extern Rect boardGQDestRect;
extern Rect badgesBlankRects[4];
extern Rect badgesBadgesRects[4];
extern Rect badgesDestRects[4];
extern HDC boardPSrcMap;
extern SInt16 wasScoreboardMode;

void RefreshScoreboard (SInt16 mode);
void HandleDynamicScoreboard (void);
void QuickGlidersRefresh (void);
void QuickScoreRefresh (void);
void QuickBatteryRefresh (Boolean flash);
void QuickBandsRefresh (Boolean flash);
void QuickFoilRefresh (Boolean flash);
void AdjustScoreboardHeight (void);

//void HandleScore (void);

#endif
