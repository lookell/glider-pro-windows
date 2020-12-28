#ifndef DYNAMIC_MAPS_H_
#define DYNAMIC_MAPS_H_

//============================================================================
//----------------------------------------------------------------------------
//                               DynamicMaps.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderDefines.h"
#include "GliderStructs.h"
#include "MacTypes.h"

extern sparkleType g_sparkles[kMaxSparkles];
extern flyingPtType g_flyingPoints[kMaxFlyingPts];
extern flameType g_flames[kMaxCandles];
extern flameType g_tikiFlames[kMaxTikis];
extern flameType g_bbqCoals[kMaxCoals];
extern pendulumType g_pendulums[kMaxPendulums];
extern starType g_theStars[kMaxStars];
extern shredType g_shreds[kMaxShredded];
extern Rect g_pointsSrc[15];
extern SInt16 g_numSparkles;
extern SInt16 g_numFlyingPts;
extern SInt16 g_numChimes;
extern SInt16 g_numFlames;
extern SInt16 g_numSavedMaps;
extern SInt16 g_numTikiFlames;
extern SInt16 g_numCoals;
extern SInt16 g_numPendulums;
extern SInt16 g_clockFrame;
extern SInt16 g_numStars;
extern SInt16 g_numShredded;

void NilSavedMaps (void);
SInt16 BackUpToSavedMap (const Rect *theRect, SInt16 where, SInt16 who);
SInt16 ReBackUpSavedMap (const Rect *theRect, SInt16 where, SInt16 who);
void RestoreFromSavedMap (SInt16 where, SInt16 who, Boolean doSparkle);
void AddSparkle (const Rect *theRect);
void AddFlyingPoint (const Rect *theRect, SInt16 points, SInt16 hVel, SInt16 vVel);
void ReBackUpFlames (SInt16 where, SInt16 who);
void AddCandleFlame (SInt16 where, SInt16 who, SInt16 h, SInt16 v);
void ReBackUpTikiFlames (SInt16 where, SInt16 who);
void AddTikiFlame (SInt16 where, SInt16 who, SInt16 h, SInt16 v);
void ReBackUpBBQCoals (SInt16 where, SInt16 who);
void AddBBQCoals (SInt16 where, SInt16 who, SInt16 h, SInt16 v);
void ReBackUpPendulum (SInt16 where, SInt16 who);
void AddPendulum (SInt16 where, SInt16 who, SInt16 h, SInt16 v);
void ReBackUpStar (SInt16 where, SInt16 who);
void AddStar (SInt16 where, SInt16 who, SInt16 h, SInt16 v);
void StopPendulum (SInt16 where, SInt16 who);
void StopStar (SInt16 where, SInt16 who);
void AddAShreddedGlider (const Rect *bounds);
void RemoveShreds (void);
void ZeroFlamesAndTheLike (void);

#endif
