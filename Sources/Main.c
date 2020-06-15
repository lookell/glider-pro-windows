#include "Main.h"

//============================================================================
//----------------------------------------------------------------------------
//                              Glider PRO 1.0.4
//                              by  john calhoun
//----------------------------------------------------------------------------
//============================================================================


#include "AnimCursor.h"
#include "AppleEvents.h"
#include "Audio.h"
#include "Coordinates.h"
#include "Environ.h"
#include "Events.h"
#include "HighScores.h"
#include "House.h"
#include "HouseIO.h"
#include "HouseLegal.h"
#include "Input.h"
#include "InterfaceInit.h"
#include "Link.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "Map.h"
#include "Marquee.h"
#include "Menu.h"
#include "Music.h"
#include "Play.h"
#include "Player.h"
#include "Prefs.h"
#include "Room.h"
#include "RoomGraphics.h"
#include "SelectHouse.h"
#include "Settings.h"
#include "Sound.h"
#include "StringUtils.h"
#include "StructuresInit.h"
#include "Tools.h"
#include "Utilities.h"
#include "Validate.h"
#include "WinAPI.h"


#define kPrefsVersion			0x0034


void ReadInPrefs (HWND);
void WriteOutPrefs (HWND);
int WINAPI wWinMain (HINSTANCE, HINSTANCE, LPWSTR, int);


SInt16		isVolume, wasVolume;
SInt16		isDepthPref, dataResFile, numSMWarnings;
Boolean		quitting, doZooms, quickerTransitions, isUseSecondScreen;


//==============================================================  Functions
//--------------------------------------------------------------  ReadInPrefs

// Called only once when game launches - reads in the preferences saved…
// from the last time Glider PRO was launched.  If no prefs are found,…
// it assigns default settings.

void ReadInPrefs (HWND ownerWindow)
{
	prefsInfo	thePrefs;

	if (LoadPrefs(ownerWindow, &thePrefs, kPrefsVersion))
	{
#ifdef COMPILEDEMO
		PasStringCopyC("Demo House", thisHouseName);
#else
		PasStringCopy(thePrefs.wasDefaultName, thisHouseName);
#endif
		PasStringCopy(thePrefs.wasLeftName, leftName);
		PasStringCopy(thePrefs.wasRightName, rightName);
		PasStringCopy(thePrefs.wasBattName, batteryName);
		PasStringCopy(thePrefs.wasBandName, bandName);
		PasStringCopy(thePrefs.wasHighName, highName);
		PasStringCopy(thePrefs.wasHighBanner, highBanner);
		theGlider.leftKey = thePrefs.wasLeftMap;
		theGlider.rightKey = thePrefs.wasRightMap;
		theGlider.battKey = thePrefs.wasBattMap;
		theGlider.bandKey = thePrefs.wasBandMap;
#ifndef COMPILEDEMO
#ifndef COMPILENOCP
		encryptedNumber = thePrefs.encrypted;
#endif			// COMPILENOCP
#endif			// COMPILEDEMO
		isVolume = thePrefs.wasVolume;
		isDepthPref = thePrefs.wasDepthPref;
		isMusicOn = thePrefs.wasMusicOn;
		doZooms = thePrefs.wasZooms;
		quickerTransitions = thePrefs.wasQuickTrans;
		isDoColorFade = thePrefs.wasDoColorFade;
		isPlayMusicIdle = thePrefs.wasIdleMusic;
		isPlayMusicGame = thePrefs.wasGameMusic;
		isHouseChecks = thePrefs.wasHouseChecks;
		maxFiles = thePrefs.wasMaxFiles;
		if ((maxFiles < 12) || (maxFiles > 500))
			maxFiles = 12;
		isEditH = thePrefs.wasEditH;
		isEditV = thePrefs.wasEditV;
		isMapH = thePrefs.wasMapH;
		isMapV = thePrefs.wasMapV;
		mapRoomsWide = thePrefs.wasMapWide;
		mapRoomsHigh = thePrefs.wasMapHigh;
		isToolsH = thePrefs.wasToolsH;
		isToolsV = thePrefs.wasToolsV;
		isLinkH = thePrefs.wasLinkH;
		isLinkV = thePrefs.wasLinkV;
		isCoordH = thePrefs.wasCoordH;
		isCoordV = thePrefs.wasCoordV;
		mapLeftRoom = thePrefs.isMapLeft;
		mapTopRoom = thePrefs.isMapTop;
		wasFloor = thePrefs.wasFloor;
		wasSuite = thePrefs.wasSuite;
		numSMWarnings = thePrefs.smWarnings;
		autoRoomEdit = thePrefs.wasAutoEdit;
		isMapOpen = thePrefs.wasMapOpen;
		isToolsOpen = thePrefs.wasToolsOpen;
		isCoordOpen = thePrefs.wasCoordOpen;
		numNeighbors = thePrefs.wasNumNeighbors;
		toolMode = thePrefs.wasToolGroup;
		doAutoDemo = thePrefs.wasDoAutoDemo;
		isEscPauseKey = thePrefs.wasEscPauseKey;
		isUseSecondScreen = thePrefs.wasScreen2;
		if (thisMac.numScreens < 2)
			isUseSecondScreen = false;
		doBackground = thePrefs.wasDoBackground;
		doPrettyMap = thePrefs.wasPrettyMap;
		doBitchDialogs = thePrefs.wasBitchDialogs;
	}
	else
	{
#ifdef COMPILEDEMO
		PasStringCopyC("Demo House", thisHouseName);
#else
		PasStringCopyC("Slumberland", thisHouseName);
#endif
		PasStringCopyC("lf arrow", leftName);
		PasStringCopyC("rt arrow", rightName);
		PasStringCopyC("dn arrow", batteryName);
		PasStringCopyC("up arrow", bandName);
		PasStringCopyC("Your Name", highName);
		PasStringCopyC("Your Message Here", highBanner);
		theGlider.leftKey = VK_LEFT;
		theGlider.rightKey = VK_RIGHT;
		theGlider.battKey = VK_DOWN;
		theGlider.bandKey = VK_UP;

		UnivGetSoundVolume(&isVolume, thisMac.hasSM3);
		if (isVolume < 1)
			isVolume = 1;
		else if (isVolume > 3)
			isVolume = 3;

		isDepthPref = kSwitchIfNeeded;
		isSoundOn = true;
		isMusicOn = true;
		isPlayMusicIdle = true;
		isPlayMusicGame = true;
		isHouseChecks = true;
		doZooms = true;
		quickerTransitions = false;
		numNeighbors = 9;
		isDoColorFade = true;
		maxFiles = 48;
		willMaxFiles = 48;
		isEditH = 3;
		isEditV = 41;
		isMapH = 3;
//		isMapV = qd.screenBits.bounds.bottom - 100;
		isMapV = 100;
		mapRoomsWide = 15;
		mapRoomsHigh = 4;
//		isToolsH = qd.screenBits.bounds.right - 120;
		isToolsH = 100;
		isToolsV = 35;
		isLinkH = 50;
		isLinkV = 80;
//		isCoordH = qd.screenBits.bounds.right - 55;
		isCoordH = 50;
		isCoordV = 204;
		mapLeftRoom = 60;
		mapTopRoom = 50;
		wasFloor = 0;
		wasSuite = 0;
		numSMWarnings = 0;
		autoRoomEdit = true;
		isMapOpen = true;
		isToolsOpen = true;
		isCoordOpen = false;
		toolMode = kBlowerMode;
		doAutoDemo = true;
		isEscPauseKey = false;
		isUseSecondScreen = false;
		doBackground = false;
		doPrettyMap = false;
		doBitchDialogs = true;
	}

	if ((numNeighbors > 1) && (thisMac.screen.right <= 512))
		numNeighbors = 1;

	UnivGetSoundVolume(&wasVolume, thisMac.hasSM3);
	UnivSetSoundVolume(isVolume, thisMac.hasSM3);

	if (isVolume == 0)
		isSoundOn = false;
	else
		isSoundOn = true;
}

//--------------------------------------------------------------  WriteOutPrefs

// Called just before Glider PRO quits.  This function writes out…
// the user preferences to disk.

void WriteOutPrefs (HWND ownerWindow)
{
	prefsInfo	thePrefs;

	UnivGetSoundVolume(&isVolume, thisMac.hasSM3);

#ifdef COMPILEDEMO
	PasStringCopyC("Demo House", thePrefs.wasDefaultName);
#else
	PasStringCopy(thisHouseName, thePrefs.wasDefaultName);
#endif
	PasStringCopy(leftName, thePrefs.wasLeftName);
	PasStringCopy(rightName, thePrefs.wasRightName);
	PasStringCopy(batteryName, thePrefs.wasBattName);
	PasStringCopy(bandName, thePrefs.wasBandName);
	PasStringCopy(highName, thePrefs.wasHighName);
	PasStringCopy(highBanner, thePrefs.wasHighBanner);
	thePrefs.wasLeftMap = theGlider.leftKey;
	thePrefs.wasRightMap = theGlider.rightKey;
	thePrefs.wasBattMap = theGlider.battKey;
	thePrefs.wasBandMap = theGlider.bandKey;
#ifndef COMPILEDEMO
#ifndef COMPILENOCP
	thePrefs.encrypted = encryptedNumber;
	thePrefs.fakeLong = Random();
#endif			// COMPILENOCP
#endif			// COMPILEDEMO
	thePrefs.wasVolume = isVolume;
	thePrefs.wasDepthPref = isDepthPref;
	thePrefs.wasMusicOn = isMusicOn;
	thePrefs.wasZooms = doZooms;
	thePrefs.wasQuickTrans = quickerTransitions;
	thePrefs.wasDoColorFade = isDoColorFade;
	thePrefs.wasIdleMusic = isPlayMusicIdle;
	thePrefs.wasGameMusic = isPlayMusicGame;
	thePrefs.wasHouseChecks = isHouseChecks;
	thePrefs.wasMaxFiles = willMaxFiles;
	thePrefs.wasEditH = isEditH;
	thePrefs.wasEditV = isEditV;
	thePrefs.wasMapH = isMapH;
	thePrefs.wasMapV = isMapV;
	thePrefs.wasMapWide = mapRoomsWide;
	thePrefs.wasMapHigh = mapRoomsHigh;
	thePrefs.wasToolsH = isToolsH;
	thePrefs.wasToolsV = isToolsV;
	thePrefs.isMapLeft = mapLeftRoom;
	thePrefs.isMapTop = mapTopRoom;
	thePrefs.wasFloor = wasFloor;
	thePrefs.wasSuite = wasSuite;
	thePrefs.wasLinkH = isLinkH;
	thePrefs.wasLinkV = isLinkV;
	thePrefs.wasCoordH = isCoordH;
	thePrefs.wasCoordV = isCoordV;
	thePrefs.smWarnings = numSMWarnings;
	thePrefs.wasAutoEdit = autoRoomEdit;
	thePrefs.wasMapOpen = isMapOpen;
	thePrefs.wasToolsOpen = isToolsOpen;
	thePrefs.wasCoordOpen = isCoordOpen;
	thePrefs.wasNumNeighbors = numNeighbors;
	thePrefs.wasToolGroup = toolMode;
	thePrefs.wasDoAutoDemo = doAutoDemo;
	thePrefs.wasEscPauseKey = isEscPauseKey;
	thePrefs.wasScreen2 = isUseSecondScreen;
	thePrefs.wasDoBackground = doBackground;
	thePrefs.wasPrettyMap = doPrettyMap;
	thePrefs.wasBitchDialogs = doBitchDialogs;

	if (!SavePrefs(ownerWindow, &thePrefs, kPrefsVersion))
		MessageBeep(MB_ICONWARNING);

	UnivSetSoundVolume(wasVolume, thisMac.hasSM3);
}

//--------------------------------------------------------------  main
// Here is main().  The first function called when Glider PRO comes up.

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		LPWSTR lpCmdLine, int nShowCmd)
{
//	SInt32		wasSeed;
//	SInt32		theErr;
	Boolean		whoCares, copyGood, audioInitialized;
	HRESULT		hr;

	hr = Audio_InitDevice();
	audioInitialized = SUCCEEDED(hr);
	if (FAILED(hr))
	{
		dontLoadSounds = true;
		dontLoadMusic = true;
	}

	ToolBoxInit();
	CheckOurEnvirons();
	if (!thisMac.hasColor)
		RedAlert(kErrNeedColorQD);
	if (!thisMac.hasSystem7)
		RedAlert(kErrNeedSystem7);
	if (thisMac.numScreens == 0)
		RedAlert(kErrNeed16Or256Colors);
//	dataResFile = OpenResFile("\pMermaid");
	SetUpAppleEvents();
	LoadCursors();
	ReadInPrefs(NULL);

#if defined COMPILEDEMO
	copyGood = true;
#elif defined COMPILENOCP
//	didValidation = false;
	copyGood = true;
#else
	didValidation = false;
	copyGood = ValidInstallation(true);
	if (!copyGood)
		encryptedNumber = 0L;
	else if (didValidation)
		WriteOutPrefs();				SpinCursor(3);
#endif

//	if ((thisMac.numScreens > 1) && (isUseSecondScreen))
//		ReflectSecondMonitorEnvirons(false, true, true);
	HandleDepthSwitching(NULL);
	VariableInit();						SpinCursor(2);
	CheckMemorySize(NULL);
	GetExtraCursors();					SpinCursor(2);
	InitMarquee();
	CreatePointers();					SpinCursor(2);
	InitSrcRects();
	CreateOffscreens();					SpinCursor(2);
	InitSound(NULL);					SpinCursor(2);
	InitMusic(NULL);					SpinCursor(2);
	OpenMainWindow();

#ifdef COMPILEQT
	if (thisMac.hasQT)
	{
		theErr = EnterMovies();
		if (theErr != noErr)
			thisMac.hasQT = false;
	}
#endif

	BuildHouseList(mainWindow);
	if (OpenHouse(mainWindow))
		whoCares = ReadHouse(mainWindow);

	PlayPrioritySound(kBirdSound, kBirdPriority);
	DelayTicks(6);
	InitializeMenus();					InitCursor();

#if BUILD_ARCADE_VERSION
//	HideMenuBarOld();
#endif

//	if ((isDoColorFade) && (thisMac.isDepth == 8))
//	{
//		wasSeed = ExtractCTSeed((CGrafPtr)mainWindow);
//		WashColorIn();
//		ForceCTSeed((CGrafPtr)mainWindow, wasSeed);
//	}
//	if ((!thisMac.hasSM3) && (numSMWarnings < 3))
//	{
//		numSMWarnings++;
//		BitchAboutSM3();
//	}

	incrementModeTime = MillisToTicks(GetTickCount()) + kIdleSplashTicks;
	while (!quitting)		// this is the main loop
		HandleEvent();
/*
#if BUILD_ARCADE_VERSION
	ShowMenuBarOld();
#endif
*/
	CloseMainWindow();
	KillMusic();
	KillSound();
	if (houseOpen)
	{
		if (!CloseHouse(NULL))
		{
			CloseHouseResFork();
			if (houseRefNum != INVALID_HANDLE_VALUE && houseRefNum != NULL)
				CloseHandle(houseRefNum);
			houseRefNum = INVALID_HANDLE_VALUE;
			houseOpen = false;
		}
	}

	// Detach all submenus from the menu bar, and then destroy all menus.
	RemoveMenu(theMenuBar, kAppleMenuID, MF_BYCOMMAND);
	RemoveMenu(theMenuBar, kGameMenuID, MF_BYCOMMAND);
	RemoveMenu(theMenuBar, kOptionsMenuID, MF_BYCOMMAND);
	RemoveMenu(theMenuBar, kHouseMenuID, MF_BYCOMMAND);
	DestroyMenu(appleMenu);
	DestroyMenu(gameMenu);
	DestroyMenu(optionsMenu);
	DestroyMenu(houseMenu);
	DestroyMenu(theMenuBar);

	WriteOutPrefs(NULL);
	RestoreColorDepth();
//	FlushEvents(everyEvent, 0);
//	theErr = LoadScrap();

	if (audioInitialized)
	{
		Audio_KillDevice();
		audioInitialized = false;
	}

	return 0;
}

