#ifndef DYNAMIC_MAPS_H_
#define DYNAMIC_MAPS_H_

//============================================================================
//----------------------------------------------------------------------------
//                               DynamicMaps.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"

void NilSavedMaps (void);
SInt16 BackUpToSavedMap (Rect *theRect, SInt16 where, SInt16 who);
SInt16 ReBackUpSavedMap (Rect *theRect, SInt16 where, SInt16 who);
void RestoreFromSavedMap (SInt16 where, SInt16 who, Boolean doSparkle);
void AddSparkle (Rect *theRect);
void AddFlyingPoint (Rect *theRect, SInt16 points, SInt16 hVel, SInt16 vVel);
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
void AddAShreddedGlider (Rect *bounds);
void RemoveShreds (void);
void ZeroFlamesAndTheLike (void);

#endif
