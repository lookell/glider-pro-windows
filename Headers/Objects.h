#ifndef OBJECTS_H_
#define OBJECTS_H_

//============================================================================
//----------------------------------------------------------------------------
//                                 Objects.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderStructs.h"
#include "MacTypes.h"
#include "WinAPI.h"

extern HDC				blowerSrcMap;
extern HDC				blowerMaskMap;
extern HDC				furnitureSrcMap;
extern HDC				furnitureMaskMap;
extern HDC				bonusSrcMap;
extern HDC				bonusMaskMap;
extern HDC				pointsSrcMap;
extern HDC				pointsMaskMap;
extern HDC				transSrcMap;
extern HDC				transMaskMap;
extern HDC				switchSrcMap;
extern HDC				lightSrcMap;
extern HDC				lightMaskMap;
extern HDC				applianceSrcMap;
extern HDC				applianceMaskMap;
extern HDC				toastSrcMap;
extern HDC				toastMaskMap;
extern HDC				shredSrcMap;
extern HDC				shredMaskMap;
extern HDC				balloonSrcMap;
extern HDC				balloonMaskMap;
extern HDC				copterSrcMap;
extern HDC				copterMaskMap;
extern HDC				dartSrcMap;
extern HDC				dartMaskMap;
extern HDC				ballSrcMap;
extern HDC				ballMaskMap;
extern HDC				dripSrcMap;
extern HDC				dripMaskMap;
extern HDC				enemySrcMap;
extern HDC				enemyMaskMap;
extern HDC				fishSrcMap;
extern HDC				fishMaskMap;
extern HDC				clutterSrcMap;
extern HDC				clutterMaskMap;

Boolean IsThisValid (SInt16 where, SInt16 who);
SInt16 GetRoomLinked (objectType *who);
Boolean ObjectIsLinkTransport (objectType *who);
Boolean ObjectIsLinkSwitch (objectType *who);
void ListAllLocalObjects (void);
void AddTempManholeRect (Rect *manHole);
Boolean SetObjectState (SInt16 room, SInt16 object, SInt16 action, SInt16 local);
Boolean GetObjectState (SInt16 room, SInt16 object);
void BringSendFrontBack (HWND ownerWindow, Boolean bringFront);

#endif
