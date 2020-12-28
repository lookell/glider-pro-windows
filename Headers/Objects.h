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
extern Rect g_blowerSrcRect;
extern HDC g_blowerSrcMap;
extern HDC g_blowerMaskMap;
extern Rect g_flame[kNumCandleFlames];
extern Rect g_tikiFlame[kNumTikiFlames];
extern Rect g_coals[kNumBBQCoals];

// Furniture
extern Rect g_furnitureSrcRect;
extern HDC g_furnitureSrcMap;
extern HDC g_furnitureMaskMap;
extern Rect g_tableSrc;
extern Rect g_shelfSrc;
extern Rect g_hingeSrc;
extern Rect g_handleSrc;
extern Rect g_knobSrc;
extern Rect g_leftFootSrc;
extern Rect g_rightFootSrc;
extern Rect g_deckSrc;

// Bonuses
extern Rect g_bonusSrcRect;
extern HDC g_bonusSrcMap;
extern HDC g_bonusMaskMap;
extern Rect g_pointsSrcRect;
extern HDC g_pointsSrcMap;
extern HDC g_pointsMaskMap;
extern Rect g_starSrc[6];
extern Rect g_sparkleSrc[kNumSparkleModes];
extern Rect g_digits[11];
extern Rect g_pendulumSrc[3];
extern Rect g_greaseSrcRt[4];
extern Rect g_greaseSrcLf[4];

// Transport
extern Rect g_transSrcRect;
extern HDC g_transSrcMap;
extern HDC g_transMaskMap;

// Switches
extern Rect g_switchSrcRect;
extern HDC g_switchSrcMap;
extern Rect g_lightSwitchSrc[2];
extern Rect g_machineSwitchSrc[2];
extern Rect g_thermostatSrc[2];
extern Rect g_powerSrc[2];
extern Rect g_knifeSwitchSrc[2];

// Lights
extern Rect g_lightSrcRect;
extern HDC g_lightSrcMap;
extern HDC g_lightMaskMap;
extern Rect g_flourescentSrc1;
extern Rect g_flourescentSrc2;
extern Rect g_trackLightSrc[kNumTrackLights];

// Appliances
extern Rect g_applianceSrcRect;
extern HDC g_applianceSrcMap;
extern HDC g_applianceMaskMap;
extern Rect g_toastSrcRect;
extern HDC g_toastSrcMap;
extern HDC g_toastMaskMap;
extern Rect g_shredSrcRect;
extern HDC g_shredSrcMap;
extern HDC g_shredMaskMap;
extern Rect g_plusScreen1;
extern Rect g_plusScreen2;
extern Rect g_tvScreen1;
extern Rect g_tvScreen2;
extern Rect g_coffeeLight1;
extern Rect g_coffeeLight2;
extern Rect g_vcrTime1;
extern Rect g_vcrTime2;
extern Rect g_stereoLight1;
extern Rect g_stereoLight2;
extern Rect g_microOn;
extern Rect g_microOff;
extern Rect g_outletSrc[kNumOutletPicts];

// Enemies
extern Rect g_balloonSrcRect;
extern HDC g_balloonSrcMap;
extern HDC g_balloonMaskMap;
extern Rect g_balloonSrc[kNumBalloonFrames];
extern Rect g_copterSrcRect;
extern HDC g_copterSrcMap;
extern HDC g_copterMaskMap;
extern Rect g_copterSrc[kNumCopterFrames];
extern Rect g_dartSrcRect;
extern HDC g_dartSrcMap;
extern HDC g_dartMaskMap;
extern Rect g_dartSrc[kNumDartFrames];
extern Rect g_ballSrcRect;
extern HDC g_ballSrcMap;
extern HDC g_ballMaskMap;
extern Rect g_ballSrc[kNumBallFrames];
extern Rect g_dripSrcRect;
extern HDC g_dripSrcMap;
extern HDC g_dripMaskMap;
extern Rect g_dripSrc[kNumDripFrames];
extern Rect g_fishSrcRect;
extern HDC g_fishSrcMap;
extern HDC g_fishMaskMap;
extern Rect g_fishSrc[kNumFishFrames];
extern Rect g_enemySrcRect;
extern HDC g_enemySrcMap;
extern HDC g_enemyMaskMap;

// Clutter
extern Rect g_clutterSrcRect;
extern HDC g_clutterSrcMap;
extern HDC g_clutterMaskMap;
extern Rect g_flowerSrc[kNumFlowers];

extern Rect g_srcRects[kNumSrcRects];
extern Rect g_tempManholes[kMaxTempManholes];
extern savedType g_savedMaps[kMaxSavedMaps];
extern objDataType g_masterObjects[kMaxMasterObjects];
extern hotObject g_hotSpots[kMaxHotSpots];
extern SInt16 g_nHotSpots;
extern SInt16 g_numMasterObjects;
extern SInt16 g_numTempManholes;
extern SInt16 g_tvWithMovieNumber;
extern Boolean g_newState;

Boolean IsThisValid (SInt16 where, SInt16 who);
Boolean ObjectIsLinkTransport (const objectType *who);
Boolean ObjectIsLinkSwitch (const objectType *who);
void ListAllLocalObjects (void);
void AddTempManholeRect (const Rect *manHole);
Boolean SetObjectState (SInt16 room, SInt16 object, SInt16 action, SInt16 local);
Boolean GetObjectState (SInt16 room, SInt16 object);
void BringSendFrontBack (HWND ownerWindow, Boolean bringFront);

#endif
