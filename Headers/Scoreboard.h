#ifndef SCOREBOARD_H_
#define SCOREBOARD_H_

//============================================================================
//----------------------------------------------------------------------------
//                                Scoreboard.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

#define kFoilBadge              0
#define kBandsBadge             1
#define kBatteryBadge           2
#define kHeliumBadge            3
#define kNumBadges              4

extern Rect boardSrcRect;
extern Rect badgeSrcRect;
extern Rect boardDestRect;
#ifdef GP_USE_WINAPI_H
extern HDC boardSrcMap;
extern HDC badgeSrcMap;
#endif
extern Rect boardTSrcRect;
extern Rect boardTDestRect;
#ifdef GP_USE_WINAPI_H
extern HDC boardTSrcMap;
#endif
extern Rect boardGSrcRect;
extern Rect boardGDestRect;
#ifdef GP_USE_WINAPI_H
extern HDC boardGSrcMap;
#endif
extern Rect boardPSrcRect;
extern Rect boardPDestRect;
#ifdef GP_USE_WINAPI_H
extern HDC boardPSrcMap;
#endif
extern Rect boardPQDestRect;
extern Rect boardGQDestRect;
extern Rect badgesBlankRects[kNumBadges];
extern Rect badgesBadgesRects[kNumBadges];
extern Rect badgesDestRects[kNumBadges];
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
