#ifndef OBJECTS_H_
#define OBJECTS_H_

//============================================================================
//----------------------------------------------------------------------------
//                                 Objects.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderDefines.h"
#include "GliderStructs.h"
#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

#define kMaxTempManholes        8

// Blowers
extern Rect blowerSrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC blowerSrcMap;
extern HDC blowerMaskMap;
#endif
extern Rect flame[kNumCandleFlames];
extern Rect tikiFlame[kNumTikiFlames];
extern Rect coals[kNumBBQCoals];

// Furniture
extern Rect furnitureSrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC furnitureSrcMap;
extern HDC furnitureMaskMap;
#endif
extern Rect tableSrc;
extern Rect shelfSrc;
extern Rect hingeSrc;
extern Rect handleSrc;
extern Rect knobSrc;
extern Rect leftFootSrc;
extern Rect rightFootSrc;
extern Rect deckSrc;

// Bonuses
extern Rect bonusSrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC bonusSrcMap;
extern HDC bonusMaskMap;
#endif
extern Rect pointsSrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC pointsSrcMap;
extern HDC pointsMaskMap;
#endif
extern Rect starSrc[6];
extern Rect sparkleSrc[kNumSparkleModes];
extern Rect digits[11];
extern Rect pendulumSrc[3];
extern Rect greaseSrcRt[4];
extern Rect greaseSrcLf[4];

// Transport
extern Rect transSrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC transSrcMap;
extern HDC transMaskMap;
#endif

// Switches
extern Rect switchSrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC switchSrcMap;
#endif
extern Rect lightSwitchSrc[2];
extern Rect machineSwitchSrc[2];
extern Rect thermostatSrc[2];
extern Rect powerSrc[2];
extern Rect knifeSwitchSrc[2];

// Lights
extern Rect lightSrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC lightSrcMap;
extern HDC lightMaskMap;
#endif
extern Rect flourescentSrc1;
extern Rect flourescentSrc2;
extern Rect trackLightSrc[kNumTrackLights];

// Appliances
extern Rect applianceSrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC applianceSrcMap;
extern HDC applianceMaskMap;
#endif
extern Rect toastSrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC toastSrcMap;
extern HDC toastMaskMap;
#endif
extern Rect shredSrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC shredSrcMap;
extern HDC shredMaskMap;
#endif
extern Rect plusScreen1;
extern Rect plusScreen2;
extern Rect tvScreen1;
extern Rect tvScreen2;
extern Rect coffeeLight1;
extern Rect coffeeLight2;
extern Rect vcrTime1;
extern Rect vcrTime2;
extern Rect stereoLight1;
extern Rect stereoLight2;
extern Rect microOn;
extern Rect microOff;
extern Rect outletSrc[kNumOutletPicts];

// Enemies
extern Rect balloonSrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC balloonSrcMap;
extern HDC balloonMaskMap;
#endif
extern Rect balloonSrc[kNumBalloonFrames];
extern Rect copterSrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC copterSrcMap;
extern HDC copterMaskMap;
#endif
extern Rect copterSrc[kNumCopterFrames];
extern Rect dartSrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC dartSrcMap;
extern HDC dartMaskMap;
#endif
extern Rect dartSrc[kNumDartFrames];
extern Rect ballSrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC ballSrcMap;
extern HDC ballMaskMap;
#endif
extern Rect ballSrc[kNumBallFrames];
extern Rect dripSrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC dripSrcMap;
extern HDC dripMaskMap;
#endif
extern Rect dripSrc[kNumDripFrames];
extern Rect fishSrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC fishSrcMap;
extern HDC fishMaskMap;
#endif
extern Rect fishSrc[kNumFishFrames];
extern Rect enemySrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC enemySrcMap;
extern HDC enemyMaskMap;
#endif

// Clutter
extern Rect clutterSrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC clutterSrcMap;
extern HDC clutterMaskMap;
#endif
extern Rect flowerSrc[kNumFlowers];

extern Rect srcRects[kNumSrcRects];
extern Rect tempManholes[kMaxTempManholes];
#ifdef GP_USE_WINAPI_H
extern savedType savedMaps[kMaxSavedMaps];
#endif
extern objDataType masterObjects[kMaxMasterObjects];
extern hotObject hotSpots[kMaxHotSpots];
extern SInt16 nHotSpots;
extern SInt16 numMasterObjects;
extern SInt16 numTempManholes;
extern SInt16 tvWithMovieNumber;
extern Boolean newState;

Boolean IsThisValid (SInt16 where, SInt16 who);
SInt16 GetRoomLinked (const objectType *who);
Boolean ObjectIsLinkTransport (const objectType *who);
Boolean ObjectIsLinkSwitch (const objectType *who);
void ListAllLocalObjects (void);
void AddTempManholeRect (const Rect *manHole);
Boolean SetObjectState (SInt16 room, SInt16 object, SInt16 action, SInt16 local);
Boolean GetObjectState (SInt16 room, SInt16 object);
#ifdef GP_USE_WINAPI_H
void BringSendFrontBack (HWND ownerWindow, Boolean bringFront);
#endif

#endif
