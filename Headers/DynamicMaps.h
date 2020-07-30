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

extern sparkleType sparkles[kMaxSparkles];
extern flyingPtType flyingPoints[kMaxFlyingPts];
extern flameType flames[kMaxCandles];
extern flameType tikiFlames[kMaxTikis];
extern flameType bbqCoals[kMaxCoals];
extern pendulumType pendulums[kMaxPendulums];
extern starType theStars[kMaxStars];
extern shredType shreds[kMaxShredded];
extern Rect pointsSrc[15];
extern SInt16 numSparkles;
extern SInt16 numFlyingPts;
extern SInt16 numChimes;
extern SInt16 numFlames;
extern SInt16 numSavedMaps;
extern SInt16 numTikiFlames;
extern SInt16 numCoals;
extern SInt16 numPendulums;
extern SInt16 clockFrame;
extern SInt16 numStars;
extern SInt16 numShredded;

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
