//============================================================================
//----------------------------------------------------------------------------
//                                Scoreboard.h
//----------------------------------------------------------------------------
//============================================================================

#ifndef SCOREBOARD_H_
#define SCOREBOARD_H_

#include "MacTypes.h"
#include "WinAPI.h"

#define kFoilBadge              0
#define kBandsBadge             1
#define kBatteryBadge           2
#define kHeliumBadge            3
#define kNumBadges              4

extern HFONT g_scoreboardFont;
extern Rect g_boardSrcRect;
extern Rect g_badgeSrcRect;
extern Rect g_boardDestRect;
extern HDC g_boardSrcMap;
extern HDC g_badgeSrcMap;
extern Rect g_boardTSrcRect;
extern Rect g_boardTDestRect;
extern HDC g_boardTSrcMap;
extern Rect g_boardGSrcRect;
extern Rect g_boardGDestRect;
extern HDC g_boardGSrcMap;
extern Rect g_boardPSrcRect;
extern Rect g_boardPDestRect;
extern HDC g_boardPSrcMap;
extern Rect g_boardPQDestRect;
extern Rect g_boardGQDestRect;
extern Rect g_badgesBlankRects[kNumBadges];
extern Rect g_badgesBadgesRects[kNumBadges];
extern Rect g_badgesDestRects[kNumBadges];
extern SInt16 g_wasScoreboardMode;

void RefreshScoreboard (SInt16 mode);
void HandleDynamicScoreboard (void);
void QuickGlidersRefresh (void);
void QuickScoreRefresh (void);
void QuickBatteryRefresh (Boolean flash);
void QuickBandsRefresh (Boolean flash);
void QuickFoilRefresh (Boolean flash);
void AdjustScoreboardHeight (void);
SInt16 GetScoreboardMode (void);
SInt16 SetScoreboardMode (SInt16 newMode);

#endif
