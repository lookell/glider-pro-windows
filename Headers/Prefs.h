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

typedef struct prefsInfo
{
	Str32 wasDefaultName;
	Str15 wasLeftName; // TODO: remove on next prefs format change
	Str15 wasRightName; // TODO: remove on next prefs format change
	Str15 wasBattName; // TODO: remove on next prefs format change
	Str15 wasBandName; // TODO: remove on next prefs format change
	Str15 wasHighName;
	Str31 wasHighBanner;
	SInt32 wasLeftMap;
	SInt32 wasRightMap;
	SInt32 wasBattMap;
	SInt32 wasBandMap;
	SInt16 wasVolume;
	SInt16 prefVersion;
	SInt16 wasMaxFiles;
	SInt16 wasEditH;
	SInt16 wasEditV;
	SInt16 wasMapH;
	SInt16 wasMapV;
	SInt16 wasMapWide;
	SInt16 wasMapHigh;
	SInt16 wasToolsH;
	SInt16 wasToolsV;
	SInt16 wasLinkH;
	SInt16 wasLinkV;
	SInt16 wasCoordH;
	SInt16 wasCoordV;
	SInt16 isMapLeft;
	SInt16 isMapTop;
	SInt16 wasNumNeighbors;
	SInt16 wasDepthPref; // TODO: remove on next prefs format change
	SInt16 wasToolGroup;
	SInt16 smWarnings; // TODO: remove on next prefs format change
	SInt16 wasFloor;
	SInt16 wasSuite;
	Boolean wasZooms; // TODO: remove on next prefs format change
	Boolean wasMusicOn;
	Boolean wasAutoEdit;
	Boolean wasDoColorFade;
	Boolean wasMapOpen;
	Boolean wasToolsOpen;
	Boolean wasCoordOpen;
	Boolean wasQuickTrans;
	Boolean wasIdleMusic;
	Boolean wasGameMusic;
	Boolean wasEscPauseKey;
	Boolean wasDoAutoDemo;
	Boolean wasScreen2;
	Boolean wasDoBackground; // TODO: remove on next prefs format change
	Boolean wasHouseChecks;
	Boolean wasPrettyMap;
	Boolean wasBitchDialogs;
} prefsInfo;

Boolean SavePrefs (HWND ownerWindow, prefsInfo *thePrefs, SInt16 versionNow);
Boolean LoadPrefs (HWND ownerWindow, prefsInfo *thePrefs, SInt16 versionNeed);

#endif
