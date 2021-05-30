//============================================================================
//----------------------------------------------------------------------------
//                                   Menu.c
//----------------------------------------------------------------------------
//============================================================================

#include "Menu.h"

#include "About.h"
#include "Coordinates.h"
#include "DialogUtils.h"
#include "Events.h"
#include "HighScores.h"
#include "House.h"
#include "HouseInfo.h"
#include "HouseIO.h"
#include "Link.h"
#include "Main.h"
#include "MainWindow.h"
#include "Map.h"
#include "Marquee.h"
#include "Music.h"
#include "ObjectEdit.h"
#include "ObjectInfo.h"
#include "Objects.h"
#include "Play.h"
#include "RectUtils.h"
#include "ResourceIDs.h"
#include "Room.h"
#include "RoomInfo.h"
#include "SavedGames.h"
#include "SelectHouse.h"
#include "Settings.h"
#include "Scrap.h"
#include "StringUtils.h"
#include "Tools.h"

#include <mmsystem.h>
#include <strsafe.h>

void UpdateMenusEditMode (void);
void UpdateMenusNonEditMode (void);
void UpdateMenusHouseOpen (void);
void UpdateMenusHouseClosed (void);
void DoNotInDemo (HWND ownerWindow);
void HeyYourPissingAHighScore (HWND ownerWindow);
void OpenCloseEditWindows (void);

HMENU g_theMenuBar;
HMENU g_appleMenu;
HMENU g_gameMenu;
HMENU g_optionsMenu;
HMENU g_houseMenu;
LPWSTR g_appleMenuTitle;
LPWSTR g_gameMenuTitle;
LPWSTR g_optionsMenuTitle;
LPWSTR g_houseMenuTitle;
Boolean g_menusUp;
Boolean g_resumedSavedGame;

//==============================================================  Functions
//--------------------------------------------------------------  UpdateMenusEditMode
// Sets the menus to reflect that user is in edit mode.

void UpdateMenusEditMode (void)
{
	EnableMenuItem(g_gameMenu, ID_NEW_GAME, MF_GRAYED);
	EnableMenuItem(g_gameMenu, ID_TWO_PLAYER, MF_GRAYED);
	EnableMenuItem(g_gameMenu, ID_OPEN_SAVED_GAME, MF_GRAYED);
	EnableMenuItem(g_optionsMenu, ID_HIGH_SCORES, MF_GRAYED);
	EnableMenuItem(g_optionsMenu, ID_DEMO, MF_GRAYED);
	CheckMenuItem(g_optionsMenu, ID_EDITOR, MF_CHECKED);
}

//--------------------------------------------------------------  UpdateMenusNonEditMode
// Sets the menus to reflect that user is NOT in edit mode.

void UpdateMenusNonEditMode (void)
{
	if ((g_noRoomAtAll) || (!g_houseOpen) || (g_thisHouse.nRooms <= 0))
	{
		EnableMenuItem(g_gameMenu, ID_NEW_GAME, MF_GRAYED);
		EnableMenuItem(g_gameMenu, ID_TWO_PLAYER, MF_GRAYED);
		EnableMenuItem(g_gameMenu, ID_OPEN_SAVED_GAME, MF_GRAYED);
		if (g_houseOpen)
		{
			EnableMenuItem(g_gameMenu, ID_LOAD_HOUSE, MF_ENABLED);
			EnableMenuItem(g_optionsMenu, ID_HIGH_SCORES, MF_ENABLED);
		}
		else
		{
			EnableMenuItem(g_gameMenu, ID_LOAD_HOUSE, MF_GRAYED);
			EnableMenuItem(g_optionsMenu, ID_HIGH_SCORES, MF_GRAYED);
		}
	}
	else
	{
		EnableMenuItem(g_gameMenu, ID_NEW_GAME, MF_ENABLED);
		EnableMenuItem(g_gameMenu, ID_TWO_PLAYER, MF_ENABLED);
		EnableMenuItem(g_gameMenu, ID_OPEN_SAVED_GAME, MF_ENABLED);
		EnableMenuItem(g_gameMenu, ID_LOAD_HOUSE, MF_ENABLED);
		EnableMenuItem(g_optionsMenu, ID_HIGH_SCORES, MF_ENABLED);
	}
	if (g_demoHouseIndex == -1)
		EnableMenuItem(g_optionsMenu, ID_DEMO, MF_GRAYED);
	else
		EnableMenuItem(g_optionsMenu, ID_DEMO, MF_ENABLED);
	CheckMenuItem(g_optionsMenu, ID_EDITOR, MF_UNCHECKED);
}

//--------------------------------------------------------------  UpdateMenusHouseOpen
// Sets the menus to reflect that a house is currently open.

void UpdateMenusHouseOpen (void)
{
	EnableMenuItem(g_gameMenu, ID_LOAD_HOUSE, MF_ENABLED);
	if ((g_fileDirty) && (g_houseUnlocked))
		EnableMenuItem(g_houseMenu, ID_SAVE_HOUSE, MF_ENABLED);
	else
		EnableMenuItem(g_houseMenu, ID_SAVE_HOUSE, MF_GRAYED);
	if (g_houseUnlocked)
	{
		// EnableMenuItem(g_houseMenu, iSaveAs);
		EnableMenuItem(g_houseMenu, ID_HOUSE_INFO, MF_ENABLED);
	}
	else
	{
		// DisableMenuItem(g_houseMenu, iSaveAs);
		EnableMenuItem(g_houseMenu, ID_HOUSE_INFO, MF_GRAYED);
	}
	if ((g_noRoomAtAll) || (!g_houseUnlocked))
		EnableMenuItem(g_houseMenu, ID_ROOM_INFO, MF_GRAYED);
	else
		EnableMenuItem(g_houseMenu, ID_ROOM_INFO, MF_ENABLED);
	if ((g_objActive == kNoObjectSelected) || (!g_houseUnlocked))
	{
		EnableMenuItem(g_houseMenu, ID_OBJECT_INFO, MF_GRAYED);
		EnableMenuItem(g_houseMenu, ID_BRING_FORWARD, MF_GRAYED);
		EnableMenuItem(g_houseMenu, ID_SEND_BACK, MF_GRAYED);
	}
	else
	{
		EnableMenuItem(g_houseMenu, ID_OBJECT_INFO, MF_ENABLED);
		if ((g_objActive == kInitialGliderSelected) ||
				(g_objActive == kLeftGliderSelected) ||
				(g_objActive == kRightGliderSelected))
		{
			EnableMenuItem(g_houseMenu, ID_BRING_FORWARD, MF_GRAYED);
			EnableMenuItem(g_houseMenu, ID_SEND_BACK, MF_GRAYED);
		}
		else
		{
			EnableMenuItem(g_houseMenu, ID_BRING_FORWARD, MF_ENABLED);
			EnableMenuItem(g_houseMenu, ID_SEND_BACK, MF_ENABLED);
		}
	}
}

//--------------------------------------------------------------  UpdateMenusHouseClosed
// Sets the menus to reflect that a house is NOT currently open.

void UpdateMenusHouseClosed (void)
{
	EnableMenuItem(g_gameMenu, ID_LOAD_HOUSE, MF_GRAYED);
	EnableMenuItem(g_houseMenu, ID_SAVE_HOUSE, MF_GRAYED);
	// DisableMenuItem(g_houseMenu, iSaveAs);
	EnableMenuItem(g_houseMenu, ID_HOUSE_INFO, MF_GRAYED);
	EnableMenuItem(g_houseMenu, ID_ROOM_INFO, MF_GRAYED);
	EnableMenuItem(g_houseMenu, ID_OBJECT_INFO, MF_GRAYED);
	EnableMenuItem(g_houseMenu, ID_CUT, MF_GRAYED);
	EnableMenuItem(g_houseMenu, ID_COPY, MF_GRAYED);
	EnableMenuItem(g_houseMenu, ID_PASTE, MF_GRAYED);
	EnableMenuItem(g_houseMenu, ID_CLEAR, MF_GRAYED);
	EnableMenuItem(g_houseMenu, ID_DUPLICATE, MF_GRAYED);
}

//--------------------------------------------------------------  UpdateClipboardMenus
// Set the Cut/Copy/Paste menus to reflect if we have data in the
// Mac's "clipboard" or not.

static BOOL SetMenuItemText(HMENU hMenu, UINT uID, PCWSTR newTitle)
{
	MENUITEMINFO itemInfo;
	wchar_t theString[256];

	StringCchCopy(theString, ARRAYSIZE(theString), newTitle);
	itemInfo.cbSize = sizeof(itemInfo);
	itemInfo.fMask = MIIM_STRING;
	itemInfo.dwTypeData = theString;
	return SetMenuItemInfo(hMenu, uID, FALSE, &itemInfo);
}

void UpdateClipboardMenus (void)
{
	wchar_t title[256];

	if (!g_houseOpen)
		return;

	if (g_houseUnlocked)
	{
		if (g_objActive > kNoObjectSelected)
		{
			GetLocalizedString(36, title, ARRAYSIZE(title));
			SetMenuItemText(g_houseMenu, ID_CUT, title);
			GetLocalizedString(37, title, ARRAYSIZE(title));
			SetMenuItemText(g_houseMenu, ID_COPY, title);
			GetLocalizedString(38, title, ARRAYSIZE(title));
			SetMenuItemText(g_houseMenu, ID_CLEAR, title);
			EnableMenuItem(g_houseMenu, ID_DUPLICATE, MF_ENABLED);
		}
		else
		{
			GetLocalizedString(39, title, ARRAYSIZE(title));
			SetMenuItemText(g_houseMenu, ID_CUT, title);
			GetLocalizedString(40, title, ARRAYSIZE(title));
			SetMenuItemText(g_houseMenu, ID_COPY, title);
			GetLocalizedString(41, title, ARRAYSIZE(title));
			SetMenuItemText(g_houseMenu, ID_CLEAR, title);
			EnableMenuItem(g_houseMenu, ID_DUPLICATE, MF_GRAYED);
		}

		EnableMenuItem(g_houseMenu, ID_CUT, MF_ENABLED);
		EnableMenuItem(g_houseMenu, ID_COPY, MF_ENABLED);
		if (g_hasScrap)
		{
			EnableMenuItem(g_houseMenu, ID_PASTE, MF_ENABLED);
			if (g_scrapIsARoom)
			{
				GetLocalizedString(42, title, ARRAYSIZE(title));
				SetMenuItemText(g_houseMenu, ID_PASTE, title);
			}
			else
			{
				GetLocalizedString(43, title, ARRAYSIZE(title));
				SetMenuItemText(g_houseMenu, ID_PASTE, title);
			}
		}
		else
		{
			EnableMenuItem(g_houseMenu, ID_PASTE, MF_GRAYED);
			GetLocalizedString(44, title, ARRAYSIZE(title));
			SetMenuItemText(g_houseMenu, ID_PASTE, title);
		}
		EnableMenuItem(g_houseMenu, ID_CLEAR, MF_ENABLED);
		EnableMenuItem(g_houseMenu, ID_GO_TO_ROOM, MF_ENABLED);
		EnableMenuItem(g_houseMenu, ID_MAP_WINDOW, MF_ENABLED);
		EnableMenuItem(g_houseMenu, ID_OBJECT_WINDOW, MF_ENABLED);
		EnableMenuItem(g_houseMenu, ID_COORDINATE_WINDOW, MF_ENABLED);
	}
	else
	{
		EnableMenuItem(g_houseMenu, ID_CUT, MF_GRAYED);
		EnableMenuItem(g_houseMenu, ID_COPY, MF_GRAYED);
		EnableMenuItem(g_houseMenu, ID_PASTE, MF_GRAYED);
		EnableMenuItem(g_houseMenu, ID_CLEAR, MF_GRAYED);
		EnableMenuItem(g_houseMenu, ID_DUPLICATE, MF_GRAYED);
		EnableMenuItem(g_houseMenu, ID_GO_TO_ROOM, MF_GRAYED);
		EnableMenuItem(g_houseMenu, ID_MAP_WINDOW, MF_GRAYED);
		EnableMenuItem(g_houseMenu, ID_OBJECT_WINDOW, MF_GRAYED);
		EnableMenuItem(g_houseMenu, ID_COORDINATE_WINDOW, MF_GRAYED);
	}
}

//--------------------------------------------------------------  UpdateMenus
// Called whenever a significant change to the environment has taken
// place and some of the menu states may have changes (for example,
// a menui was grayed out before becuase it wasn't an option - now
// perhaps the situation has changed and we want the menu to be "usable").

void UpdateMenus (Boolean newMode)
{
	MENUITEMINFO mii;

	if (!g_menusUp)
		return;

	if (newMode)
	{
		if (g_theMode == kEditMode)
		{
			mii.cbSize = sizeof(mii);
			mii.fMask = MIIM_ID | MIIM_STRING | MIIM_SUBMENU;
			mii.wID = kHouseMenuID;
			mii.hSubMenu = g_houseMenu;
			mii.dwTypeData = g_houseMenuTitle;
			InsertMenuItem(g_theMenuBar, kAppleMenuID, FALSE, &mii);
		}
		else
		{
			RemoveMenu(g_theMenuBar, kHouseMenuID, MF_BYCOMMAND);
		}
	}

	if (g_theMode == kEditMode)
	{
		UpdateMenusEditMode();
		if (g_houseOpen)
		{
			UpdateMenusHouseOpen();
			UpdateClipboardMenus();
		}
		else
			UpdateMenusHouseClosed();
		UpdateLinkControl();
	}
	else
		UpdateMenusNonEditMode();

	if (g_mainWindow != NULL)
		DrawMenuBar(g_mainWindow);
}

//--------------------------------------------------------------  DoAppleMenu
// Handle the Apple menu (About box and desk accessories).

void DoAppleMenu (HWND hwnd, SInt16 theItem)
{
	switch (theItem)
	{
	case iAbout:
		DoAbout(hwnd);
		break;
	}
}

//--------------------------------------------------------------  DoGameMenu
// Handle the user selecting an item from the Game menu.

void DoGameMenu (HWND hwnd, SInt16 theItem)
{
	switch (theItem)
	{
		case iNewGame:
		g_twoPlayerGame = false;
		g_resumedSavedGame = false;
		DisableMenuBar();
		NewGame(hwnd, kNewGameMode);
		EnableMenuBar();
		break;

		case iTwoPlayer:
		g_twoPlayerGame = true;
		g_resumedSavedGame = false;
		DisableMenuBar();
		NewGame(hwnd, kNewGameMode);
		EnableMenuBar();
		break;

		case iOpenSavedGame:
		g_resumedSavedGame = true;
		HeyYourPissingAHighScore(hwnd);
		if (OpenSavedGame(hwnd))
		{
			g_twoPlayerGame = false;
			DisableMenuBar();
			NewGame(hwnd, kResumeGameMode);
			EnableMenuBar();
		}
		break;

		case iLoadHouse:
		if (COMPILEDEMO)
		{
			DoNotInDemo(hwnd);
			return;
		}
		if (g_splashDrawn)
		{
			DoLoadHouse(hwnd);
			OpenCloseEditWindows();
			UpdateMenus(false);
			g_incrementModeTime = timeGetTime() + kIdleSplashTime;
			if ((g_theMode == kSplashMode) || (g_theMode == kPlayMode))
			{
				// Mark the main window as dirty, in case a new house
				// was loaded and the splash screen needs to be redrawn.
				InvalidateRect(g_mainWindow, NULL, TRUE);
			}
		}
		break;

		case iQuit:
		g_quitting = true;
		if (!COMPILEDEMO)
		{
			if (!QuerySaveChanges(hwnd))
				g_quitting = false;
		}
		if (g_quitting)
			PostQuitMessage(0);
		break;

		default:
		break;
	}
}

//--------------------------------------------------------------  DoOptionsMenu
// Handle the user selecting an item from the Options menu.

void DoOptionsMenu (HWND hwnd, SInt16 theItem)
{
	switch (theItem)
	{
		case iEditor:
		if (COMPILEDEMO)
		{
			DoNotInDemo(hwnd);
			return;
		}
		if (g_theMode == kEditMode)  // switching to splash mode
		{
			if (g_fileDirty)
				SortHouseObjects();
			if (!QuerySaveChanges(hwnd))
				break;
			CloseMapWindow();
			CloseToolsWindow();
			CloseCoordWindow();
			CloseLinkWindow();
			DeselectObject();
			StopMarquee();
			CloseMainWindow();
			g_theMode = kSplashMode;
			OpenMainWindow();
			g_incrementModeTime = timeGetTime() + kIdleSplashTime;
		}
		else if (g_theMode == kSplashMode)  // switching to edit mode
		{
			StopTheMusic();
			CloseMainWindow();
			g_theMode = kEditMode;
			OpenMainWindow();
			OpenCloseEditWindows();
		}
		UpdateMenus(true);
		break;

		case iHighScores:
		DisableMenuBar();
		DoHighScores();
		RedrawSplashScreen();
		EnableMenuBar();
		g_incrementModeTime = timeGetTime() + kIdleSplashTime;
		break;

		case iPrefs:
		DoSettingsMain(hwnd);
		g_incrementModeTime = timeGetTime() + kIdleSplashTime;
		break;

		case iHelp:
		DisableMenuBar();
		DoDemoGame(hwnd);
		EnableMenuBar();
		break;
	}
}

//--------------------------------------------------------------  DoHouseMenu
// Handle the user selecting an item from the House menu (only in Edit mode).

void DoHouseMenu (HWND hwnd, SInt16 theItem)
{
	Boolean whoCares;

	if (COMPILEDEMO)
		return;

	if (g_theMode != kEditMode)
		return;

	switch (theItem)
	{
		case iNewHouse:
		if (CreateNewHouse(hwnd))
		{
			InitializeEmptyHouse();
			OpenCloseEditWindows();
		}
		break;

		case iSave:
		DeselectObject();
		if (g_fileDirty)
			SortHouseObjects();
		if ((g_fileDirty) && (g_houseUnlocked))
		{
			if (g_wasHouseVersion < kHouseVersion)
				ConvertHouseVer1To2();
			g_wasHouseVersion = kHouseVersion;
			whoCares = WriteHouse(hwnd, true);
			ForceThisRoom(g_thisRoomNumber);
			ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
			GetThisRoomsObjRects();
			DrawThisRoomsObjects();
		}
		break;

#if 0
		case iSaveAs:
		whoCares = SaveHouseAs(hwnd);
		break;
#endif

		case iHouse:
		if (g_houseUnlocked)
			DoHouseInfo(hwnd);
		break;

		case iRoom:
		if (g_houseUnlocked)
			DoRoomInfo(hwnd);
		break;

		case iObject:
		if (g_houseUnlocked)
		{
			DoObjectInfo(hwnd);
			StartMarqueeForActiveObject();
		}
		break;

		case iCut:
		if (g_houseUnlocked)
		{
			if (g_objActive > kNoObjectSelected)
			{
				PutObjectScrap();
				Gp_DeleteObject();
			}
			else
			{
				PutRoomScrap();
				DeleteRoom(hwnd, false);
			}
			UpdateClipboardMenus();
		}
		break;

		case iCopy:
		if (g_houseUnlocked)
		{
			if (g_objActive > kNoObjectSelected)
				PutObjectScrap();
			else
				PutRoomScrap();
			UpdateClipboardMenus();
		}
		break;

		case iPaste:
		if (g_houseUnlocked)
		{
			if (g_scrapIsARoom)
				GetRoomScrap();
			else
				GetObjectScrap();
			UpdateClipboardMenus();
		}
		break;

		case iClear:
		if (g_houseUnlocked)
		{
			if (g_objActive > kNoObjectSelected)
				Gp_DeleteObject();
			else
				DeleteRoom(hwnd, false);
			UpdateClipboardMenus();
		}
		break;

		case iDuplicate:
		if (g_houseUnlocked)
			DuplicateObject(hwnd);
		break;

		case iBringForward:
		if (g_houseUnlocked)
			BringSendFrontBack(hwnd, true);
		break;

		case iSendBack:
		if (g_houseUnlocked)
			BringSendFrontBack(hwnd, false);
		break;

		case iGoToRoom:
		if (g_houseUnlocked)
			DoGoToDialog(hwnd);
		break;

		case iMapWindow:
		if (g_houseUnlocked)
			ToggleMapWindow();
		break;

		case iObjectWindow:
		if (g_houseUnlocked)
			ToggleToolsWindow();
		break;

		case iCoordinateWindow:
		if (g_houseUnlocked)
			ToggleCoordinateWindow();
		break;
	}
}

//--------------------------------------------------------------  DoMenuChoice
// Users has selected a menu item - determin which menu was selected
// and call the appropriate function above.

void DoMenuChoice (HWND hwnd, WORD menuChoice)
{
	switch (menuChoice)
	{
		case ID_ABOUT:
		DoAppleMenu(hwnd, iAbout);
		break;

		case ID_NEW_GAME:
		DoGameMenu(hwnd, iNewGame);
		break;

		case ID_TWO_PLAYER:
		DoGameMenu(hwnd, iTwoPlayer);
		break;

		case ID_OPEN_SAVED_GAME:
		DoGameMenu(hwnd, iOpenSavedGame);
		break;

		case ID_LOAD_HOUSE:
		DoGameMenu(hwnd, iLoadHouse);
		break;

		case ID_QUIT:
		DoGameMenu(hwnd, iQuit);
		break;

		case ID_EDITOR:
		DoOptionsMenu(hwnd, iEditor);
		break;

		case ID_HIGH_SCORES:
		DoOptionsMenu(hwnd, iHighScores);
		break;

		case ID_PREFS:
		DoOptionsMenu(hwnd, iPrefs);
		break;

		case ID_DEMO:
		DoOptionsMenu(hwnd, iHelp);
		break;

		case ID_NEW_HOUSE:
		DoHouseMenu(hwnd, iNewHouse);
		break;

		case ID_SAVE_HOUSE:
		DoHouseMenu(hwnd, iSave);
		break;

		case ID_HOUSE_INFO:
		DoHouseMenu(hwnd, iHouse);
		break;

		case ID_ROOM_INFO:
		DoHouseMenu(hwnd, iRoom);
		break;

		case ID_OBJECT_INFO:
		DoHouseMenu(hwnd, iObject);
		break;

		case ID_CUT:
		DoHouseMenu(hwnd, iCut);
		break;

		case ID_COPY:
		DoHouseMenu(hwnd, iCopy);
		break;

		case ID_PASTE:
		DoHouseMenu(hwnd, iPaste);
		break;

		case ID_CLEAR:
		DoHouseMenu(hwnd, iClear);
		break;

		case ID_DUPLICATE:
		DoHouseMenu(hwnd, iDuplicate);
		break;

		case ID_BRING_FORWARD:
		DoHouseMenu(hwnd, iBringForward);
		break;

		case ID_SEND_BACK:
		DoHouseMenu(hwnd, iSendBack);
		break;

		case ID_GO_TO_ROOM:
		DoHouseMenu(hwnd, iGoToRoom);
		break;

		case ID_MAP_WINDOW:
		DoHouseMenu(hwnd, iMapWindow);
		break;

		case ID_OBJECT_WINDOW:
		DoHouseMenu(hwnd, iObjectWindow);
		break;

		case ID_COORDINATE_WINDOW:
		DoHouseMenu(hwnd, iCoordinateWindow);
		break;
	}
}

//--------------------------------------------------------------  UpdateMapCheckmark
// Checks or unchecks the Map Window item (to indicate if open or not).

void UpdateMapCheckmark (Boolean checkIt)
{
	if (!g_menusUp)
		return;

	if (checkIt)
		CheckMenuItem(g_houseMenu, ID_MAP_WINDOW, MF_CHECKED);
	else
		CheckMenuItem(g_houseMenu, ID_MAP_WINDOW, MF_UNCHECKED);
}

//--------------------------------------------------------------  UpdateToolsCheckmark
// Checks or unchecks the Tools Window item (to indicate if open or not).

void UpdateToolsCheckmark (Boolean checkIt)
{
	if (!g_menusUp)
		return;

	if (checkIt)
		CheckMenuItem(g_houseMenu, ID_OBJECT_WINDOW, MF_CHECKED);
	else
		CheckMenuItem(g_houseMenu, ID_OBJECT_WINDOW, MF_UNCHECKED);
}

//--------------------------------------------------------------  UpdateCoordinateCheckmark
// Checks or unchecks the Coordinates Window item (to indicate if open or not).

void UpdateCoordinateCheckmark (Boolean checkIt)
{
	if (!g_menusUp)
		return;

	if (checkIt)
		CheckMenuItem(g_houseMenu, ID_COORDINATE_WINDOW, MF_CHECKED);
	else
		CheckMenuItem(g_houseMenu, ID_COORDINATE_WINDOW, MF_UNCHECKED);
}

//--------------------------------------------------------------  DoNotInDemo
// Only compiled for "demo version" of Glider PRO.  It brings up a
// dialog that says, essentially, "x" feature is  not implemented in
// the demo version.

void DoNotInDemo (HWND ownerWindow)
{
	Alert(kNotInDemoAlert, ownerWindow, NULL);
}

//--------------------------------------------------------------  HeyYourPissingAHighScore

void HeyYourPissingAHighScore (HWND ownerWindow)
{
	Alert(kNoHighScoreAlert, ownerWindow, NULL);
}

//--------------------------------------------------------------  OpenCloseEditWindows
// Function goes through and either closes or opens all the various
// editing windows (in response to switching in or out of editor).

void OpenCloseEditWindows (void)
{
	if (g_theMode == kEditMode)
	{
		if (g_houseUnlocked)
		{
			if (g_isMapOpen)
				OpenMapWindow();
			if (g_isToolsOpen)
				OpenToolsWindow();
			if (g_isCoordOpen)
				OpenCoordWindow();
		}
		else
		{
			CloseMapWindow();
			CloseToolsWindow();
			CloseCoordWindow();
		}
	}
}

//--------------------------------------------------------------  EnableMenuBar
// Iterate over the menu bar's items and enable all of them. This is to
// prevent interaction and a weird state being set (for example, clicking
// "Options > High Scores" and then clicking "Options > High Scores" again
// while the high scores are still being shown.

void EnableMenuBar (void)
{
	int i, n;

	n = GetMenuItemCount(g_theMenuBar);
	for (i = 0; i < n; i++)
		EnableMenuItem(g_theMenuBar, i, MF_BYPOSITION | MF_ENABLED);
	DrawMenuBar(g_mainWindow);
}

//--------------------------------------------------------------  EnableMenuBar
// Similar to the above function, but this one disables all of the menu bar's
// items, insteading of enabling them.

void DisableMenuBar(void)
{
	int i, n;

	n = GetMenuItemCount(g_theMenuBar);
	for (i = 0; i < n; i++)
		EnableMenuItem(g_theMenuBar, i, MF_BYPOSITION | MF_GRAYED);
	DrawMenuBar(g_mainWindow);
}
