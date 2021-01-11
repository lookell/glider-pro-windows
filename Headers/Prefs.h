//============================================================================
//----------------------------------------------------------------------------
//                                  Prefs.h
//----------------------------------------------------------------------------
//============================================================================

#ifndef PREFS_H_
#define PREFS_H_

#include "MacTypes.h"
#include "WinAPI.h"

#define kPrefsVersion           0x0036

typedef struct prefsInfo
{
	Str32 wasDefaultName;
	UInt16 wasViewportWidth;
	UInt16 wasViewportHeight;
	Byte unusedBytes[44];
	Str15 wasHighName;
	Str31 wasHighBanner;
	SInt32 wasLeftKeyOne;
	SInt32 wasRightKeyOne;
	SInt32 wasBattKeyOne;
	SInt32 wasBandKeyOne;
	SInt32 wasLeftKeyTwo;
	SInt32 wasRightKeyTwo;
	SInt32 wasBattKeyTwo;
	SInt32 wasBandKeyTwo;
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
	SInt16 wasToolGroup;
	SInt16 wasFloor;
	SInt16 wasSuite;
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
	Boolean wasHouseChecks;
	Boolean wasPrettyMap;
	Boolean wasBitchDialogs;
} prefsInfo;

Boolean SavePrefs (HWND ownerWindow, prefsInfo *thePrefs, SInt16 versionNow);
Boolean LoadPrefs (HWND ownerWindow, prefsInfo *thePrefs, SInt16 versionNeed);

#endif
