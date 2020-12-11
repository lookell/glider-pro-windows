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
#include "WinAPI.h"

#define kMaxTempManholes        8

typedef struct savedType
{
	Rect dest;
	HDC map;
	SInt16 where;
	SInt16 who;
} savedType;

// Blowers
extern Rect blowerSrcRect;
extern HDC blowerSrcMap;
extern HDC blowerMaskMap;
extern Rect flame[kNumCandleFlames];
extern Rect tikiFlame[kNumTikiFlames];
extern Rect coals[kNumBBQCoals];

// Furniture
extern Rect furnitureSrcRect;
extern HDC furnitureSrcMap;
extern HDC furnitureMaskMap;
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
extern HDC bonusSrcMap;
extern HDC bonusMaskMap;
extern Rect pointsSrcRect;
extern HDC pointsSrcMap;
extern HDC pointsMaskMap;
extern Rect starSrc[6];
extern Rect sparkleSrc[kNumSparkleModes];
extern Rect digits[11];
extern Rect pendulumSrc[3];
extern Rect greaseSrcRt[4];
extern Rect greaseSrcLf[4];

// Transport
extern Rect transSrcRect;
extern HDC transSrcMap;
extern HDC transMaskMap;

// Switches
extern Rect switchSrcRect;
extern HDC switchSrcMap;
extern Rect lightSwitchSrc[2];
extern Rect machineSwitchSrc[2];
extern Rect thermostatSrc[2];
extern Rect powerSrc[2];
extern Rect knifeSwitchSrc[2];

// Lights
extern Rect lightSrcRect;
extern HDC lightSrcMap;
extern HDC lightMaskMap;
extern Rect flourescentSrc1;
extern Rect flourescentSrc2;
extern Rect trackLightSrc[kNumTrackLights];

// Appliances
extern Rect applianceSrcRect;
extern HDC applianceSrcMap;
extern HDC applianceMaskMap;
extern Rect toastSrcRect;
extern HDC toastSrcMap;
extern HDC toastMaskMap;
extern Rect shredSrcRect;
extern HDC shredSrcMap;
extern HDC shredMaskMap;
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
extern HDC balloonSrcMap;
extern HDC balloonMaskMap;
extern Rect balloonSrc[kNumBalloonFrames];
extern Rect copterSrcRect;
extern HDC copterSrcMap;
extern HDC copterMaskMap;
extern Rect copterSrc[kNumCopterFrames];
extern Rect dartSrcRect;
extern HDC dartSrcMap;
extern HDC dartMaskMap;
extern Rect dartSrc[kNumDartFrames];
extern Rect ballSrcRect;
extern HDC ballSrcMap;
extern HDC ballMaskMap;
extern Rect ballSrc[kNumBallFrames];
extern Rect dripSrcRect;
extern HDC dripSrcMap;
extern HDC dripMaskMap;
extern Rect dripSrc[kNumDripFrames];
extern Rect fishSrcRect;
extern HDC fishSrcMap;
extern HDC fishMaskMap;
extern Rect fishSrc[kNumFishFrames];
extern Rect enemySrcRect;
extern HDC enemySrcMap;
extern HDC enemyMaskMap;

// Clutter
extern Rect clutterSrcRect;
extern HDC clutterSrcMap;
extern HDC clutterMaskMap;
extern Rect flowerSrc[kNumFlowers];

extern Rect srcRects[kNumSrcRects];
extern Rect tempManholes[kMaxTempManholes];
extern savedType savedMaps[kMaxSavedMaps];
extern objDataType masterObjects[kMaxMasterObjects];
extern hotObject hotSpots[kMaxHotSpots];
extern SInt16 nHotSpots;
extern SInt16 numMasterObjects;
extern SInt16 numTempManholes;
extern SInt16 tvWithMovieNumber;
extern Boolean newState;

Boolean IsThisValid (SInt16 where, SInt16 who);
Boolean ObjectIsLinkTransport (const objectType *who);
Boolean ObjectIsLinkSwitch (const objectType *who);
void ListAllLocalObjects (void);
void AddTempManholeRect (const Rect *manHole);
Boolean SetObjectState (SInt16 room, SInt16 object, SInt16 action, SInt16 local);
Boolean GetObjectState (SInt16 room, SInt16 object);
void BringSendFrontBack (HWND ownerWindow, Boolean bringFront);

#endif
