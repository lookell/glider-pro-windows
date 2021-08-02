//============================================================================
//----------------------------------------------------------------------------
//                              Glider PRO 1.0.4
//                              by  john calhoun
//----------------------------------------------------------------------------
//============================================================================

#include "Main.h"

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
#include "MainWindow.h"
#include "Map.h"
#include "Marquee.h"
#include "Menu.h"
#include "Music.h"
#include "Player.h"
#include "Prefs.h"
#include "ResourceIDs.h"
#include "ResourceLoader.h"
#include "Room.h"
#include "RoomGraphics.h"
#include "SelectHouse.h"
#include "Sound.h"
#include "StringUtils.h"
#include "StructuresInit.h"
#include "Tools.h"
#include "Utilities.h"
#include "WinAPI.h"

#include <commctrl.h>
#include <mmsystem.h>
#include <objbase.h>
#include <strsafe.h>

void QuitIfLessThanWindowsXP (void);
void ReadInPrefs (HWND ownerWindow);
void WriteOutPrefs (HWND ownerWindow);

Boolean g_quitting;
Boolean g_quickerTransitions;
Boolean g_isUseSecondScreen;
UInt16 g_isViewportWidth;
UInt16 g_isViewportHeight;

//==============================================================  Functions
//--------------------------------------------------------------  QuitIfLessThanWindowsXP

void QuitIfLessThanWindowsXP (void)
{
	OSVERSIONINFOEX osvi;
	DWORD dwTypeMask;
	DWORDLONG dwlConditionMask;
	BOOL atLeastWinXP;

	ZeroMemory(&osvi, sizeof(osvi));
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	osvi.dwMajorVersion = HIBYTE(_WIN32_WINNT_WINXP);
	osvi.dwMinorVersion = LOBYTE(_WIN32_WINNT_WINXP);
	dwTypeMask = VER_MAJORVERSION | VER_MINORVERSION;
	dwlConditionMask = 0;
	dwlConditionMask = VerSetConditionMask(dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
	dwlConditionMask = VerSetConditionMask(dwlConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
	atLeastWinXP = VerifyVersionInfo(&osvi, dwTypeMask, dwlConditionMask);
	if (!atLeastWinXP)
	{
		RedAlert(kErrNeedWinXPOrLater);
	}
}

//--------------------------------------------------------------  ReadInPrefs
// Called only once when game launches - reads in the preferences saved
// from the last time Glider PRO was launched.  If no prefs are found,
// it assigns default settings.

void ReadInPrefs (HWND ownerWindow)
{
	prefsInfo thePrefs;
	SInt16 theVolume;

	if (LoadPrefs(ownerWindow, &thePrefs, kPrefsVersion))
	{
		g_isViewportWidth = thePrefs.wasViewportWidth;
		if (g_isViewportWidth < kMinScreenWidth)
		{
			g_isViewportWidth = kMinScreenWidth;
		}
		g_isViewportHeight = thePrefs.wasViewportHeight;
		if (g_isViewportHeight < kMinScreenHeight)
		{
			g_isViewportHeight = kMinScreenHeight;
		}
		PasStringCopy(thePrefs.wasHighName, g_highName, ARRAYSIZE(g_highName));
		PasStringCopy(thePrefs.wasHighBanner, g_highBanner, ARRAYSIZE(g_highBanner));
		g_theGlider.leftKey = thePrefs.wasLeftKeyOne;
		g_theGlider.rightKey = thePrefs.wasRightKeyOne;
		g_theGlider.battKey = thePrefs.wasBattKeyOne;
		g_theGlider.bandKey = thePrefs.wasBandKeyOne;
		g_theGlider2.leftKey = thePrefs.wasLeftKeyTwo;
		g_theGlider2.rightKey = thePrefs.wasRightKeyTwo;
		g_theGlider2.battKey = thePrefs.wasBattKeyTwo;
		g_theGlider2.bandKey = thePrefs.wasBandKeyTwo;
		theVolume = thePrefs.wasVolume;
		g_isMusicOn = thePrefs.wasMusicOn;
		g_quickerTransitions = thePrefs.wasQuickTrans;
		g_isDoColorFade = thePrefs.wasDoColorFade;
		g_isPlayMusicIdle = thePrefs.wasIdleMusic;
		g_isPlayMusicGame = thePrefs.wasGameMusic;
		g_isHouseChecks = thePrefs.wasHouseChecks;
		g_maxFiles = thePrefs.wasMaxFiles;
		if ((g_maxFiles < 12) || (g_maxFiles > 500))
			g_maxFiles = 12;
		g_isEditH = thePrefs.wasEditH;
		g_isEditV = thePrefs.wasEditV;
		g_isMapH = thePrefs.wasMapH;
		g_isMapV = thePrefs.wasMapV;
		g_mapRoomsWide = thePrefs.wasMapWide;
		g_mapRoomsHigh = thePrefs.wasMapHigh;
		g_isToolsH = thePrefs.wasToolsH;
		g_isToolsV = thePrefs.wasToolsV;
		g_isLinkH = thePrefs.wasLinkH;
		g_isLinkV = thePrefs.wasLinkV;
		g_isCoordH = thePrefs.wasCoordH;
		g_isCoordV = thePrefs.wasCoordV;
		g_mapLeftRoom = thePrefs.isMapLeft;
		g_mapTopRoom = thePrefs.isMapTop;
		g_wasFloor = thePrefs.wasFloor;
		g_wasSuite = thePrefs.wasSuite;
		g_autoRoomEdit = thePrefs.wasAutoEdit;
		g_isMapOpen = thePrefs.wasMapOpen;
		g_isToolsOpen = thePrefs.wasToolsOpen;
		g_isCoordOpen = thePrefs.wasCoordOpen;
		g_numNeighbors = thePrefs.wasNumNeighbors;
		g_toolMode = thePrefs.wasToolGroup;
		g_doAutoDemo = thePrefs.wasDoAutoDemo;
		g_isEscPauseKey = thePrefs.wasEscPauseKey;
		// TODO: implement support for fullscreen display and monitor selection
		g_isUseSecondScreen = false;
		g_doPrettyMap = thePrefs.wasPrettyMap;
		g_doBitchDialogs = thePrefs.wasBitchDialogs;
		if (COMPILEDEMO)
		{
			StringCchCopy(g_thisHouseName, ARRAYSIZE(g_thisHouseName), L"Demo House");
		}
		else
		{
			StringCchCopy(g_thisHouseName, ARRAYSIZE(g_thisHouseName), thePrefs.wasHouseName);
		}
	}
	else
	{
		g_isViewportWidth = 640;
		g_isViewportHeight = 480;
		PasStringCopyC("Your Name", g_highName, ARRAYSIZE(g_highName));
		PasStringCopyC("Your Message Here", g_highBanner, ARRAYSIZE(g_highBanner));
		g_theGlider.leftKey = VK_LEFT;
		g_theGlider.rightKey = VK_RIGHT;
		g_theGlider.battKey = VK_DOWN;
		g_theGlider.bandKey = VK_UP;
		g_theGlider2.leftKey = 'A';
		g_theGlider2.rightKey = 'D';
		g_theGlider2.battKey = 'S';
		g_theGlider2.bandKey = 'W';
		theVolume = 3;
		g_isSoundOn = true;
		g_isMusicOn = true;
		g_isPlayMusicIdle = true;
		g_isPlayMusicGame = true;
		g_isHouseChecks = true;
		g_quickerTransitions = false;
		g_numNeighbors = 9;
		g_isDoColorFade = true;
		g_maxFiles = 48;
		g_willMaxFiles = 48;
		g_isEditH = 3;
		g_isEditV = 41;
		g_isMapH = 3;
#if 0
		g_isMapV = qd.screenBits.bounds.bottom - 100;
#endif
		g_isMapV = 100;
		g_mapRoomsWide = 15;
		g_mapRoomsHigh = 4;
#if 0
		g_isToolsH = qd.screenBits.bounds.right - 120;
#endif
		g_isToolsH = 100;
		g_isToolsV = 35;
		g_isLinkH = 50;
		g_isLinkV = 80;
#if 0
		g_isCoordH = qd.screenBits.bounds.right - 55;
#endif
		g_isCoordH = 50;
		g_isCoordV = 204;
		g_mapLeftRoom = 60;
		g_mapTopRoom = 50;
		g_wasFloor = 0;
		g_wasSuite = 0;
		g_autoRoomEdit = true;
		g_isMapOpen = true;
		g_isToolsOpen = true;
		g_isCoordOpen = false;
		g_toolMode = kBlowerMode;
		g_doAutoDemo = true;
		g_isEscPauseKey = false;
		g_isUseSecondScreen = false;
		g_doPrettyMap = false;
		g_doBitchDialogs = true;
		if (COMPILEDEMO)
		{
			StringCchCopy(g_thisHouseName, ARRAYSIZE(g_thisHouseName), L"Demo House");
		}
		else
		{
			StringCchCopy(g_thisHouseName, ARRAYSIZE(g_thisHouseName), L"Slumberland");
		}
	}

	if ((g_numNeighbors > 1) && (g_isViewportWidth <= kMinScreenWidth))
		g_numNeighbors = 1;

	UnivSetSoundVolume(theVolume);

	g_isSoundOn = (theVolume != 0);
}

//--------------------------------------------------------------  WriteOutPrefs
// Called just before Glider PRO quits.  This function writes out
// the user preferences to disk.

void WriteOutPrefs (HWND ownerWindow)
{
	prefsInfo thePrefs = { 0 };
	SInt16 theVolume;

	UnivGetSoundVolume(&theVolume);

	thePrefs.wasViewportWidth = g_isViewportWidth;
	thePrefs.wasViewportHeight = g_isViewportHeight;
	PasStringCopy(g_highName, thePrefs.wasHighName, ARRAYSIZE(thePrefs.wasHighName));
	PasStringCopy(g_highBanner, thePrefs.wasHighBanner, ARRAYSIZE(thePrefs.wasHighBanner));
	thePrefs.wasLeftKeyOne = g_theGlider.leftKey;
	thePrefs.wasRightKeyOne = g_theGlider.rightKey;
	thePrefs.wasBattKeyOne = g_theGlider.battKey;
	thePrefs.wasBandKeyOne = g_theGlider.bandKey;
	thePrefs.wasLeftKeyTwo = g_theGlider2.leftKey;
	thePrefs.wasRightKeyTwo = g_theGlider2.rightKey;
	thePrefs.wasBattKeyTwo = g_theGlider2.battKey;
	thePrefs.wasBandKeyTwo = g_theGlider2.bandKey;
	thePrefs.wasVolume = theVolume;
	thePrefs.wasMusicOn = g_isMusicOn;
	thePrefs.wasQuickTrans = g_quickerTransitions;
	thePrefs.wasDoColorFade = g_isDoColorFade;
	thePrefs.wasIdleMusic = g_isPlayMusicIdle;
	thePrefs.wasGameMusic = g_isPlayMusicGame;
	thePrefs.wasHouseChecks = g_isHouseChecks;
	thePrefs.wasMaxFiles = g_willMaxFiles;
	thePrefs.wasEditH = g_isEditH;
	thePrefs.wasEditV = g_isEditV;
	thePrefs.wasMapH = g_isMapH;
	thePrefs.wasMapV = g_isMapV;
	thePrefs.wasMapWide = g_mapRoomsWide;
	thePrefs.wasMapHigh = g_mapRoomsHigh;
	thePrefs.wasToolsH = g_isToolsH;
	thePrefs.wasToolsV = g_isToolsV;
	thePrefs.isMapLeft = g_mapLeftRoom;
	thePrefs.isMapTop = g_mapTopRoom;
	thePrefs.wasFloor = g_wasFloor;
	thePrefs.wasSuite = g_wasSuite;
	thePrefs.wasLinkH = g_isLinkH;
	thePrefs.wasLinkV = g_isLinkV;
	thePrefs.wasCoordH = g_isCoordH;
	thePrefs.wasCoordV = g_isCoordV;
	thePrefs.wasAutoEdit = g_autoRoomEdit;
	thePrefs.wasMapOpen = g_isMapOpen;
	thePrefs.wasToolsOpen = g_isToolsOpen;
	thePrefs.wasCoordOpen = g_isCoordOpen;
	thePrefs.wasNumNeighbors = g_numNeighbors;
	thePrefs.wasToolGroup = g_toolMode;
	thePrefs.wasDoAutoDemo = g_doAutoDemo;
	thePrefs.wasEscPauseKey = g_isEscPauseKey;
	thePrefs.wasScreen2 = g_isUseSecondScreen;
	thePrefs.wasPrettyMap = g_doPrettyMap;
	thePrefs.wasBitchDialogs = g_doBitchDialogs;
	if (COMPILEDEMO)
	{
		StringCchCopy(
			thePrefs.wasHouseName,
			ARRAYSIZE(thePrefs.wasHouseName),
			L"Demo House"
		);
	}
	else
	{
		StringCchCopy(
			thePrefs.wasHouseName,
			ARRAYSIZE(thePrefs.wasHouseName),
			g_thisHouseName
		);
	}

	if (!SavePrefs(ownerWindow, &thePrefs, kPrefsVersion))
		MessageBeep(MB_ICONWARNING);
}

//--------------------------------------------------------------  wWinMain
// Here is the main function.  The first function called when Glider PRO comes up.

int WINAPI
wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
	HRESULT hr;
	Boolean whoCares;
	int audioInitialized;
	INITCOMMONCONTROLSEX icc;

	(void)hInstance;
	(void)hPrevInstance;
	(void)lpCmdLine;
	(void)nShowCmd;

	QuitIfLessThanWindowsXP();

	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(hr))
	{
		return 1;
	}

	audioInitialized = Audio_InitDevice();
	if (!audioInitialized)
	{
		g_dontLoadSounds = true;
		g_dontLoadMusic = true;
	}

	ZeroMemory(&icc, sizeof(icc));
	icc.dwSize = sizeof(icc);
	icc.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icc);
	RegisterMainWindowClass();
	RegisterMapWindowClass();
	InitRandomLongQUS();
	g_switchedOut = false;

	// NOTE: ReadInPrefs() must come before CheckOurEnvirons()
	ReadInPrefs(NULL);
	CheckOurEnvirons();
	if (FAILED(Gp_LoadBuiltInAssets()))
		RedAlert(kErrFailedResourceLoad);

	VariableInit();
	InitMarquee();
	CreatePointers();
	InitSrcRects();
	CreateOffscreens();
	InitSound(NULL);
	InitMusic(NULL);
	OpenMainWindow();

#ifdef COMPILEQT
	if (g_thisMac.hasQT)
	{
		theErr = EnterMovies();
		if (theErr != noErr)
			g_thisMac.hasQT = false;
	}
#endif

	BuildHouseList(g_mainWindow);
	if (OpenHouse(g_mainWindow))
		whoCares = ReadHouse(g_mainWindow, true);

	PlayPrioritySound(kBirdSound, kBirdPriority);
	InitializeMenus();

	if (g_isDoColorFade)
	{
		WashColorIn();
	}

	g_incrementModeTime = timeGetTime() + kIdleSplashTime;
	while (!g_quitting)  // this is the main loop
		HandleEvent();

	CloseMainWindow();
	KillMusic();
	KillSound();
	if (g_houseOpen)
	{
		if (!CloseHouse(NULL))
		{
			Gp_UnloadHouseFile(g_theHouseFile);
			g_theHouseFile = NULL;
			g_houseOpen = false;
		}
	}

	// Detach all submenus from the menu bar, and then destroy all menus.
	RemoveMenu(g_theMenuBar, kAppleMenuID, MF_BYCOMMAND);
	RemoveMenu(g_theMenuBar, kGameMenuID, MF_BYCOMMAND);
	RemoveMenu(g_theMenuBar, kOptionsMenuID, MF_BYCOMMAND);
	RemoveMenu(g_theMenuBar, kHouseMenuID, MF_BYCOMMAND);
	DestroyMenu(g_appleMenu);
	DestroyMenu(g_gameMenu);
	DestroyMenu(g_optionsMenu);
	DestroyMenu(g_houseMenu);
	DestroyMenu(g_theMenuBar);

	DestroyOffscreens();
	DestroyMarquee();

	DestroyAcceleratorTable(g_editAccelTable);
	DestroyAcceleratorTable(g_splashAccelTable);

	UnregisterMapWindowClass();
	UnregisterMainWindowClass();

	WriteOutPrefs(NULL);
	Gp_UnloadBuiltInAssets();

	if (audioInitialized)
	{
		Audio_KillDevice();
		audioInitialized = false;
	}

	CoUninitialize();

	return 0;
}
