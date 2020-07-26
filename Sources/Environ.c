#include "Environ.h"

//============================================================================
//----------------------------------------------------------------------------
//                                 Environ.c
//----------------------------------------------------------------------------
//============================================================================


#include "AnimCursor.h"
#include "DialogUtils.h"
#include "GliderStructs.h"
#include "MacTypes.h"
#include "Music.h"
#include "ResourceIDs.h"
#include "RoomGraphics.h"
#include "Sound.h"
#include "Utilities.h"


Boolean DoWeHaveQuickTime (void);
Boolean DoWeHaveDragManager (void);
SInt16 WhatsOurDepth (void);
void GetDeviceRect (Rect *theRect);


macEnviron thisMac;


//==============================================================  Functions
//--------------------------------------------------------------  DoWeHaveQuickTime

Boolean DoWeHaveQuickTime (void)
{
	// QuickTime movies are not currently supported in this port.
	return false;
}

//--------------------------------------------------------------  DoWeHaveDragManager

Boolean DoWeHaveDragManager (void)
{
	return false;
}

//--------------------------------------------------------------  WhatsOurDepth

// Determines the pixel bit depth for current device (monitor).

SInt16 WhatsOurDepth (void)
{
	// Only 8-bit color mode is supported, for things that require an
	// underlying system palette.
	return 8;
}

//--------------------------------------------------------------  GetDeviceRect

// Returns the bounding rectangle for the current device (monitor).

void GetDeviceRect (Rect *theRect)
{
	// TODO: Load/save the viewport rectangle in the prefs file.
	theRect->left = 0;
	theRect->top = 0;
	theRect->right = 960;
	theRect->bottom = 540;
}

//--------------------------------------------------------------  CheckOurEnvirons

// Calls all the above functions in order to fill out a sort of "spec sheet"…
// for the current Mac.

void CheckOurEnvirons (void)
{
	GetDeviceRect(&thisMac.screen);
	thisMac.isDepth = WhatsOurDepth();
	thisMac.hasQT = DoWeHaveQuickTime();
	thisMac.hasDrag = DoWeHaveDragManager();
}

//--------------------------------------------------------------  CheckMemorySize

// Tests for a specific amount of memory available.  If the required memory…
// is not available, attempts to turn off various game features (music, etc.)…
// in order to accomodate the constrained memory available.

void CheckMemorySize (HWND ownerWindow)
{
	#define		kBaseBytesNeeded	614400L					// 600K Base memory
	#define		kPaddingBytes		204800L					// 200K Padding
	DialogParams	params = { 0 };
	SInt32			bytesNeeded, bytesAvail;
	SInt32			soundBytes, musicBytes;
	SInt16			hitWhat;
	wchar_t			sizeStr[256];
	MEMORYSTATUSEX	memoryStatus;

	dontLoadMusic = false;
	dontLoadSounds = false;

	bytesNeeded = kBaseBytesNeeded;							// base memory
	soundBytes = SoundBytesNeeded();						// sound memory
	if (soundBytes <= 0L)
		RedAlert(kErrNoMemory);
	else
		bytesNeeded += soundBytes;
	musicBytes = MusicBytesNeeded();						// music memory
	if (musicBytes <= 0L)
		RedAlert(kErrNoMemory);
	else
		bytesNeeded += musicBytes;
	bytesNeeded += 4L * (SInt32)thisMac.screen.bottom;		// main screen
	bytesNeeded += (((SInt32)houseRect.right - (SInt32)houseRect.left) *
			((SInt32)houseRect.bottom + 1 - (SInt32)houseRect.top) *
			(SInt32)thisMac.isDepth) / 8L;					// work map
	bytesNeeded += 4L * (SInt32)houseRect.bottom;
	bytesNeeded += (((SInt32)houseRect.right - (SInt32)houseRect.left) *
			((SInt32)houseRect.bottom + 1 - (SInt32)houseRect.top) *
			(SInt32)thisMac.isDepth) / 8L;					// back map
	bytesNeeded += 4L * houseRect.bottom;
	bytesNeeded += (((SInt32)houseRect.right - (SInt32)houseRect.left) * 21 *
			(SInt32)thisMac.isDepth) / 8L;					// scoreboard map
	bytesNeeded += (6396L * (SInt32)thisMac.isDepth) / 8L;	// more scoreboard
	bytesNeeded += (32112L * (SInt32)thisMac.isDepth) / 8L;	// glider map
	bytesNeeded += (32112L * (SInt32)thisMac.isDepth) / 8L;	// glider2 map
	bytesNeeded += 32064L / 8L;								// glider mask
	bytesNeeded += (912L * (SInt32)thisMac.isDepth) / 8L;	// glider shadow
	bytesNeeded += 864L / 8L;								// shadow mask
	bytesNeeded += (304L * (SInt32)thisMac.isDepth) / 8L;	// rubber bands
	bytesNeeded += 288L / 8L;								// bands mask
	bytesNeeded += (19344L * (SInt32)thisMac.isDepth) / 8L;	// blower map
	bytesNeeded += 19344L / 8L;								// blower mask
	bytesNeeded += (17856L * (SInt32)thisMac.isDepth) / 8L;	// furniture map
	bytesNeeded += 17792L / 8L;								// furniture mask
	bytesNeeded += (33264L * (SInt32)thisMac.isDepth) / 8L;	// prizes map
	bytesNeeded += 33176L / 8L;								// prizes mask
	bytesNeeded += (2904L * (SInt32)thisMac.isDepth) / 8L;	// points map
	bytesNeeded += 2880L / 8L;								// points mask
	bytesNeeded += (1848L * (SInt32)thisMac.isDepth) / 8L;	// transport map
	bytesNeeded += 1792L / 8L;								// transport mask
	bytesNeeded += (3360L * (SInt32)thisMac.isDepth) / 8L;	// switches map
	bytesNeeded += (9144L * (SInt32)thisMac.isDepth) / 8L;	// lights map
	bytesNeeded += 9072L / 8L;								// lights mask
	bytesNeeded += (21600L * (SInt32)thisMac.isDepth) / 8L;	// appliances map
	bytesNeeded += 21520L / 8L;								// appliances mask
	bytesNeeded += (5600L * (SInt32)thisMac.isDepth) / 8L;	// toast map
	bytesNeeded += 5568L / 8L;								// toast mask
	bytesNeeded += (1440L * (SInt32)thisMac.isDepth) / 8L;	// shredded map
	bytesNeeded += 1400L / 8L;								// shredded mask
	bytesNeeded += (5784L * (SInt32)thisMac.isDepth) / 8L;	// balloon map
	bytesNeeded += 5760L / 8L;								// balloon mask
	bytesNeeded += (9632L * (SInt32)thisMac.isDepth) / 8L;	// copter map
	bytesNeeded += 9600L / 8L;								// copter mask
	bytesNeeded += (4928L * (SInt32)thisMac.isDepth) / 8L;	// dart map
	bytesNeeded += 4864L / 8L;								// dart mask
	bytesNeeded += (2080L * (SInt32)thisMac.isDepth) / 8L;	// ball map
	bytesNeeded += 2048L / 8L;								// ball mask
	bytesNeeded += (1168L * (SInt32)thisMac.isDepth) / 8L;	// drip map
	bytesNeeded += 1152L / 8L;								// drip mask
	bytesNeeded += (1224L * (SInt32)thisMac.isDepth) / 8L;	// enemy map
	bytesNeeded += 1188L / 8L;								// enemy mask
	bytesNeeded += (2064L * (SInt32)thisMac.isDepth) / 8L;	// fish map
	bytesNeeded += 2048L / 8L;								// fish mask
	bytesNeeded += (8960L * (SInt32)thisMac.isDepth) / 8L;	// clutter map
	bytesNeeded += 8832L / 8L;								// clutter mask
	bytesNeeded += (23040L * (SInt32)thisMac.isDepth) / 8L;	// support map
	bytesNeeded += (4320L * (SInt32)thisMac.isDepth) / 8L;	// angel map
	bytesNeeded += 4224L / 8L;								// angel mask
	bytesNeeded += sizeof(roomType);
	bytesNeeded += sizeof(hotObject) * kMaxHotSpots;
	bytesNeeded += sizeof(sparkleType) * kMaxSparkles;
	bytesNeeded += sizeof(flyingPtType) * kMaxFlyingPts;
	bytesNeeded += sizeof(flameType) * kMaxCandles;
	bytesNeeded += sizeof(flameType) * kMaxTikis;
	bytesNeeded += sizeof(flameType) * kMaxCoals;
	bytesNeeded += sizeof(pendulumType) * kMaxPendulums;
	bytesNeeded += sizeof(savedType) * kMaxSavedMaps;
	bytesNeeded += sizeof(bandType) * kMaxRubberBands;
	bytesNeeded += sizeof(greaseType) * kMaxGrease;
	bytesNeeded += sizeof(starType) * kMaxStars;
	bytesNeeded += sizeof(shredType) * kMaxShredded;
	bytesNeeded += sizeof(dynaType) * kMaxDynamicObs;
	bytesNeeded += sizeof(objDataType) * kMaxMasterObjects;
	bytesNeeded += kDemoLength;		SpinCursor(1);

//	bytesAvail = FreeMem();			SpinCursor(1);
	memoryStatus.dwLength = sizeof(memoryStatus);
	if (!GlobalMemoryStatusEx(&memoryStatus))
		return;
	if (memoryStatus.ullAvailPhys > INT32_MAX)
		return;
	bytesAvail = (SInt32)memoryStatus.ullAvailPhys;
	SpinCursor(1);

	if (bytesAvail < bytesNeeded)
	{
		if (bytesAvail >= (bytesNeeded - musicBytes))
		{	// if we don't load the music we can run
			TellHerNoMusic(ownerWindow);
			dontLoadMusic = true;
			return;
		}
		else if (bytesAvail >= (bytesNeeded - (musicBytes + soundBytes)))
		{	// if we don't load the music AND sounds, we can run
			TellHerNoSounds(ownerWindow);
			dontLoadMusic = true;
			dontLoadSounds = true;
			return;
		}

#ifdef COMPILEDEMO
		StringCchPrintf(sizeStr, ARRAYSIZE(sizeStr), L"%ld",
				(long)((bytesNeeded + kPaddingBytes) / 1024L));
		params.arg[0] = sizeStr;
		hitWhat = Alert(kLowMemoryAlert, ownerWindow, &params);
#else
		StringCchPrintf(sizeStr, ARRAYSIZE(sizeStr), L"%ld",
				(long)((bytesNeeded + kPaddingBytes) / 1024L));
		params.arg[0] = sizeStr;
		hitWhat = Alert(kSetMemoryAlert, ownerWindow, &params);
#endif
		exit(0);
	}
}

