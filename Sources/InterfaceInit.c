//============================================================================
//----------------------------------------------------------------------------
//                              InterfaceInit.c
//----------------------------------------------------------------------------
//============================================================================

#include "InterfaceInit.h"

#include "Coordinates.h"
#include "Environ.h"
#include "Events.h"
#include "HighScores.h"
#include "HouseIO.h"
#include "Input.h"
#include "Link.h"
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

#include <mmsystem.h>

#include <stdlib.h>

static HMENU DetachPopupMenu (HMENU rootMenu, UINT id, LPWSTR *title);

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
	g_theMenuBar = CreateMenu();
	rootMenu = LoadMenu(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDM_ROOT));
	if (rootMenu == NULL)
		RedAlert(kErrFailedResourceLoad);

	g_gameMenu = DetachPopupMenu(rootMenu, kGameMenuID, &g_gameMenuTitle);
	if (g_gameMenu == NULL)
		RedAlert(kErrFailedResourceLoad);
	mii.wID = kGameMenuID;
	mii.hSubMenu = g_gameMenu;
	mii.dwTypeData = g_gameMenuTitle;
	InsertMenuItem(g_theMenuBar, GetMenuItemCount(g_theMenuBar), TRUE, &mii);

	g_optionsMenu = DetachPopupMenu(rootMenu, kOptionsMenuID, &g_optionsMenuTitle);
	if (g_optionsMenu == NULL)
		RedAlert(kErrFailedResourceLoad);
	mii.wID = kOptionsMenuID;
	mii.hSubMenu = g_optionsMenu;
	mii.dwTypeData = g_optionsMenuTitle;
	InsertMenuItem(g_theMenuBar, GetMenuItemCount(g_theMenuBar), TRUE, &mii);

	g_appleMenu = DetachPopupMenu(rootMenu, kAppleMenuID, &g_appleMenuTitle);
	if (g_appleMenu == NULL)
		RedAlert(kErrFailedResourceLoad);
	mii.wID = kAppleMenuID;
	mii.hSubMenu = g_appleMenu;
	mii.dwTypeData = g_appleMenuTitle;
	InsertMenuItem(g_theMenuBar, GetMenuItemCount(g_theMenuBar), TRUE, &mii);

	g_menusUp = true;
	if (g_mainWindow != NULL)
		SetMenu(g_mainWindow, g_theMenuBar);

	g_houseMenu = DetachPopupMenu(rootMenu, kHouseMenuID, &g_houseMenuTitle);
	if (g_houseMenu == NULL)
		RedAlert(kErrFailedResourceLoad);

	UpdateMenus(false);
	DestroyMenu(rootMenu);
}

//--------------------------------------------------------------  VariableInit
// All the simple interface variables are intialized here - Booleans,
// shorts, a few Rects, etc.

void VariableInit (void)
{
	size_t		i;

	g_theMenuBar = NULL;
	g_menusUp = false;
	g_quitting = false;
	g_houseOpen = false;
	g_newRoomNow = false;
	g_playing = false;
	g_evenFrame = false;
	g_fadeGraysOut = true;
	g_twoPlayerGame = false;
	g_paused = false;
	g_hasMirror = false;
	g_demoGoing = false;
//	g_scrapIsARoom = true;
	g_splashDrawn = false;

//	if (!COMPILEDEMO)
//		SeeIfValidScrapAvailable(false);

	g_theGlider.which = kPlayer1;
	g_theGlider2.which = kPlayer2;

	g_splashAccelTable = LoadAccelerators(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDA_SPLASH));
	if (g_splashAccelTable == NULL)
		RedAlert(kErrFailedResourceLoad);
	g_editAccelTable = LoadAccelerators(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDA_EDIT));
	if (g_editAccelTable == NULL)
		RedAlert(kErrFailedResourceLoad);

	g_theMode = kSplashMode;
	g_thisRoomNumber = 0;
	g_previousRoom = -1;
	g_toolSelected = kSelectTool;
	g_lastBackground = kBaseBackgroundID;
	g_wasFlower = RandomInt(kNumFlowers);
	g_lastHighScore = -1;
	g_incrementModeTime = timeGetTime() + kIdleSplashTime;
	g_willMaxFiles = g_maxFiles;

	g_fadeInSequence[0] = 4;	// 4
	g_fadeInSequence[1] = 5;
	g_fadeInSequence[2] = 6;
	g_fadeInSequence[3] = 7;
	g_fadeInSequence[4] = 5;	// 5
	g_fadeInSequence[5] = 6;
	g_fadeInSequence[6] = 7;
	g_fadeInSequence[7] = 8;
	g_fadeInSequence[8] = 6;	// 6
	g_fadeInSequence[9] = 7;
	g_fadeInSequence[10] = 8;
	g_fadeInSequence[11] = 9;
	g_fadeInSequence[12] = 7;	// 7
	g_fadeInSequence[13] = 8;
	g_fadeInSequence[14] = 9;
	g_fadeInSequence[15] = 10;

	g_mainWindow = NULL;
	g_mapWindow = NULL;
	g_toolsWindow = NULL;
	g_linkWindow = NULL;
	g_coordWindow = NULL;

	g_houseRect = g_thisMac.screen;
	ZeroRectCorner(&g_houseRect);
	g_houseRect.bottom -= kScoreboardTall;
	if (g_houseRect.right > kMaxViewWidth)
		g_houseRect.right = kMaxViewWidth;
	if (g_houseRect.bottom > kMaxViewHeight)
		g_houseRect.bottom = kMaxViewHeight;

	g_playOriginH = (RectWide(&g_houseRect) - kRoomWide) / 2;
	g_playOriginV = (RectTall(&g_houseRect) - kTileHigh) / 2;

	for (i = 0; i < 9; i++)
	{
		QSetRect(&g_localRoomsDest[i], 0, 0, kRoomWide, kTileHigh);
		QOffsetRect(&g_localRoomsDest[i], g_playOriginH, g_playOriginV);
	}
	QOffsetRect(&g_localRoomsDest[kNorthRoom], 0, -kVertLocalOffset);
	QOffsetRect(&g_localRoomsDest[kNorthEastRoom], kRoomWide, -kVertLocalOffset);
	QOffsetRect(&g_localRoomsDest[kEastRoom], kRoomWide, 0);
	QOffsetRect(&g_localRoomsDest[kSouthEastRoom], kRoomWide, kVertLocalOffset);
	QOffsetRect(&g_localRoomsDest[kSouthRoom], 0, kVertLocalOffset);
	QOffsetRect(&g_localRoomsDest[kSouthWestRoom], -kRoomWide, kVertLocalOffset);
	QOffsetRect(&g_localRoomsDest[kWestRoom], -kRoomWide, 0);
	QOffsetRect(&g_localRoomsDest[kNorthWestRoom], -kRoomWide, -kVertLocalOffset);
}
