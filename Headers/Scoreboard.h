#ifndef SCOREBOARD_H_
#define SCOREBOARD_H_

//============================================================================
//----------------------------------------------------------------------------
//                                Scoreboard.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

#define kFoilBadge              0
#define kBandsBadge             1
#define kBatteryBadge           2
#define kHeliumBadge            3
#define kNumBadges              4

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
extern Rect badgesBlankRects[kNumBadges];
extern Rect badgesBadgesRects[kNumBadges];
extern Rect badgesDestRects[kNumBadges];
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

#endif
