#ifndef PREFS_H_
#define PREFS_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Prefs.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

#define kPrefsVersion           0x0034

typedef struct
{
	Str32		wasDefaultName;
	Str15		wasLeftName, wasRightName;
	Str15		wasBattName, wasBandName;
	Str15		wasHighName;
	Str31		wasHighBanner;
	SInt32		wasLeftMap, wasRightMap;
	SInt32		wasBattMap, wasBandMap;
	SInt16		wasVolume;
	SInt16		prefVersion;
	SInt16		wasMaxFiles;
	SInt16		wasEditH, wasEditV;
	SInt16		wasMapH, wasMapV;
	SInt16		wasMapWide, wasMapHigh;
	SInt16		wasToolsH, wasToolsV;
	SInt16		wasLinkH, wasLinkV;
	SInt16		wasCoordH, wasCoordV;
	SInt16		isMapLeft, isMapTop;
	SInt16		wasNumNeighbors;
	SInt16		wasDepthPref;
	SInt16		wasToolGroup;
	SInt16		smWarnings;
	SInt16		wasFloor, wasSuite;
	Boolean		wasZooms, wasMusicOn;
	Boolean		wasAutoEdit, wasDoColorFade;
	Boolean		wasMapOpen, wasToolsOpen;
	Boolean		wasCoordOpen, wasQuickTrans;
	Boolean		wasIdleMusic, wasGameMusic;
	Boolean		wasEscPauseKey;
	Boolean		wasDoAutoDemo, wasScreen2;
	Boolean		wasDoBackground, wasHouseChecks;
	Boolean		wasPrettyMap, wasBitchDialogs;
} prefsInfo;

Boolean SavePrefs (HWND ownerWindow, prefsInfo *thePrefs, SInt16 versionNow);
Boolean LoadPrefs (HWND ownerWindow, prefsInfo *thePrefs, SInt16 versionNeed);

#endif
