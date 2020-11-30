#define GP_USE_WINAPI_H

#include "InterfaceInit.h"

//============================================================================
//----------------------------------------------------------------------------
//                              InterfaceInit.c
//----------------------------------------------------------------------------
//============================================================================


#include "Coordinates.h"
#include "Environ.h"
#include "Events.h"
#include "HighScores.h"
#include "HouseIO.h"
#include "Input.h"
#include "Link.h"
#include "Macintosh.h"
#include "Main.h"
#include "MainWindow.h"
#include "Map.h"
#include "Menu.h"
#include "ObjectAdd.h"
#include "Play.h"
#include "Player.h"
#include "RectUtils.h"
#include "Render.h"
#include "ResourceIDs.h"
#include "Room.h"
#include "RoomGraphics.h"
#include "SelectHouse.h"
#include "Tools.h"
#include "Utilities.h"

#include <stdlib.h>

#include <mmsystem.h>


static HMENU DetachPopupMenu (HMENU rootMenu, UINT id, LPWSTR *title);
static HCURSOR LoadSystemCursor (DWORD id);


//==============================================================  Functions
//--------------------------------------------------------------  DetachPopupMenu
// Given a menu containing popup menus, detach a menu and retrieve its
// handle and title. The title string must be released with the standard
// `free` function. If the function succeeds, the return value is the menu
// handle. If the function fails, the return value is NULL and the title
// output paramter is set to NULL.

static HMENU DetachPopupMenu (HMENU rootMenu, UINT id, LPWSTR *title)
{
	MENUITEMINFO mii;

	if (title == NULL)
		return NULL;
	*title = NULL;

	mii.cbSize = sizeof(mii);
	mii.fMask = MIIM_STRING | MIIM_SUBMENU;
	mii.dwTypeData = NULL;
	mii.cch = 0;
	if (!GetMenuItemInfo(rootMenu, id, FALSE, &mii))
		return NULL;
	mii.cch += 1;
	mii.dwTypeData = (LPWSTR)calloc(mii.cch, sizeof(*mii.dwTypeData));
	if (mii.dwTypeData == NULL)
		return NULL;
	if (!GetMenuItemInfo(rootMenu, id, FALSE, &mii))
	{
		free(mii.dwTypeData);
		return NULL;
	}
	if (!RemoveMenu(rootMenu, id, MF_BYCOMMAND))
	{
		free(mii.dwTypeData);
		return NULL;
	}
	*title = mii.dwTypeData;
	return mii.hSubMenu;
}

//--------------------------------------------------------------  InitializeMenus
// The menus are loaded from disk and the menu bar set up and drawn.

void InitializeMenus (void)
{
	MENUITEMINFO mii;
	HMENU rootMenu;

	mii.cbSize = sizeof(mii);
	mii.fMask = MIIM_ID | MIIM_STRING | MIIM_SUBMENU;
	theMenuBar = CreateMenu();
	rootMenu = LoadMenu(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDM_ROOT));
	if (rootMenu == NULL)
		RedAlert(kErrFailedResourceLoad);

	gameMenu = DetachPopupMenu(rootMenu, kGameMenuID, &gameMenuTitle);
	if (gameMenu == NULL)
		RedAlert(kErrFailedResourceLoad);
	mii.wID = kGameMenuID;
	mii.hSubMenu = gameMenu;
	mii.dwTypeData = gameMenuTitle;
	InsertMenuItem(theMenuBar, GetMenuItemCount(theMenuBar), TRUE, &mii);

	optionsMenu = DetachPopupMenu(rootMenu, kOptionsMenuID, &optionsMenuTitle);
	if (optionsMenu == NULL)
		RedAlert(kErrFailedResourceLoad);
	mii.wID = kOptionsMenuID;
	mii.hSubMenu = optionsMenu;
	mii.dwTypeData = optionsMenuTitle;
	InsertMenuItem(theMenuBar, GetMenuItemCount(theMenuBar), TRUE, &mii);

	appleMenu = DetachPopupMenu(rootMenu, kAppleMenuID, &appleMenuTitle);
	if (appleMenu == NULL)
		RedAlert(kErrFailedResourceLoad);
	mii.wID = kAppleMenuID;
	mii.hSubMenu = appleMenu;
	mii.dwTypeData = appleMenuTitle;
	InsertMenuItem(theMenuBar, GetMenuItemCount(theMenuBar), TRUE, &mii);

	menusUp = true;
	if (mainWindow != NULL)
		SetMenu(mainWindow, theMenuBar);

	houseMenu = DetachPopupMenu(rootMenu, kHouseMenuID, &houseMenuTitle);
	if (houseMenu == NULL)
		RedAlert(kErrFailedResourceLoad);

	UpdateMenus(false);
	DestroyMenu(rootMenu);
}

//--------------------------------------------------------------  LoadSystemCursor
// Load up a system cursor and return a handle to it. Exits the program
// if the cursor fails to load (this should never happen).

static HCURSOR LoadSystemCursor (DWORD id)
{
	return (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(id), IMAGE_CURSOR, 0, 0, LR_SHARED);
}

//--------------------------------------------------------------  GetExtraCursors
// Extra cursors (custom cursors) like the "hand" and various room
// editing cursors are loaded up.

void GetExtraCursors (void)
{
	handCursor = LoadSystemCursor(OCR_SIZEALL);
	if (handCursor == NULL)
	{
		RedAlert(kErrFailedResourceLoad);
	}
	vertCursor = LoadSystemCursor(OCR_SIZENS);
	if (vertCursor == NULL)
	{
		RedAlert(kErrFailedResourceLoad);
	}
	horiCursor = LoadSystemCursor(OCR_SIZEWE);
	if (horiCursor == NULL)
	{
		RedAlert(kErrFailedResourceLoad);
	}
	diagBotCursor = LoadSystemCursor(OCR_SIZENWSE);
	if (diagBotCursor == NULL)
	{
		RedAlert(kErrFailedResourceLoad);
	}
	diagTopCursor = LoadSystemCursor(OCR_SIZENESW);
	if (diagTopCursor == NULL)
	{
		RedAlert(kErrFailedResourceLoad);
	}
}

//--------------------------------------------------------------  VariableInit
// All the simple interface variables are intialized here - Booleans,
// shorts, a few Rects, etc.

void VariableInit (void)
{
	size_t		i;

	theMenuBar = NULL;
	menusUp = false;
	quitting = false;
	houseOpen = false;
	newRoomNow = false;
	playing = false;
	evenFrame = false;
	fadeGraysOut = true;
	twoPlayerGame = false;
	paused = false;
	hasMirror = false;
	demoGoing = false;
//	scrapIsARoom = true;
	splashDrawn = false;

//	if (!COMPILEDEMO)
//		SeeIfValidScrapAvailable(false);

	theGlider.which = kPlayer1;
	theGlider2.which = kPlayer2;

	splashAccelTable = LoadAccelerators(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDA_SPLASH));
	if (splashAccelTable == NULL)
		RedAlert(kErrFailedResourceLoad);
	editAccelTable = LoadAccelerators(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDA_EDIT));
	if (editAccelTable == NULL)
		RedAlert(kErrFailedResourceLoad);

	theMode = kSplashMode;
	thisRoomNumber = 0;
	previousRoom = -1;
	toolSelected = kSelectTool;
	lastBackground = kBaseBackgroundID;
	wasFlower = RandomInt(kNumFlowers);
	lastHighScore = -1;
	incrementModeTime = timeGetTime() + TicksToMillis(kIdleSplashTicks);
	willMaxFiles = maxFiles;

	fadeInSequence[0] = 4;	// 4
	fadeInSequence[1] = 5;
	fadeInSequence[2] = 6;
	fadeInSequence[3] = 7;
	fadeInSequence[4] = 5;	// 5
	fadeInSequence[5] = 6;
	fadeInSequence[6] = 7;
	fadeInSequence[7] = 8;
	fadeInSequence[8] = 6;	// 6
	fadeInSequence[9] = 7;
	fadeInSequence[10] = 8;
	fadeInSequence[11] = 9;
	fadeInSequence[12] = 7;	// 7
	fadeInSequence[13] = 8;
	fadeInSequence[14] = 9;
	fadeInSequence[15] = 10;

	mainWindow = NULL;
	mapWindow = NULL;
	toolsWindow = NULL;
	linkWindow = NULL;
	coordWindow = NULL;

	houseRect = thisMac.screen;
	ZeroRectCorner(&houseRect);
	houseRect.bottom -= kScoreboardTall;
	if (houseRect.right > kMaxViewWidth)
		houseRect.right = kMaxViewWidth;
	if (houseRect.bottom > kMaxViewHeight)
		houseRect.bottom = kMaxViewHeight;

	playOriginH = (RectWide(&houseRect) - kRoomWide) / 2;
	playOriginV = (RectTall(&houseRect) - kTileHigh) / 2;

	for (i = 0; i < 9; i++)
	{
		QSetRect(&localRoomsDest[i], 0, 0, kRoomWide, kTileHigh);
		QOffsetRect(&localRoomsDest[i], playOriginH, playOriginV);
	}
	QOffsetRect(&localRoomsDest[kNorthRoom], 0, -kVertLocalOffset);
	QOffsetRect(&localRoomsDest[kNorthEastRoom], kRoomWide, -kVertLocalOffset);
	QOffsetRect(&localRoomsDest[kEastRoom], kRoomWide, 0);
	QOffsetRect(&localRoomsDest[kSouthEastRoom], kRoomWide, kVertLocalOffset);
	QOffsetRect(&localRoomsDest[kSouthRoom], 0, kVertLocalOffset);
	QOffsetRect(&localRoomsDest[kSouthWestRoom], -kRoomWide, kVertLocalOffset);
	QOffsetRect(&localRoomsDest[kWestRoom], -kRoomWide, 0);
	QOffsetRect(&localRoomsDest[kNorthWestRoom], -kRoomWide, -kVertLocalOffset);
}

