
//============================================================================
//----------------------------------------------------------------------------
//									Menu.c
//----------------------------------------------------------------------------
//============================================================================


//#include <Balloons.h>
//#include <NumberFormatting.h>
//#include <ToolUtils.h>
#include "Macintosh.h"
#include "DialogUtils.h"
#include "Externs.h"
#include "House.h"
#include "ObjectEdit.h"
#include "RectUtils.h"
#include "ResourceIDs.h"


#define kSheWantsNewGame		1001
#define kSheWantsResumeGame		1002


void UpdateMenusEditMode (void);
void UpdateMenusNonEditMode (void);
void UpdateMenusHouseOpen (void);
void UpdateMenusHouseClosed (void);
INT_PTR CALLBACK ResumeFilter (HWND, UINT, WPARAM, LPARAM);
SInt16 QueryResumeGame (HWND);
void HeyYourPissingAHighScore (HWND);


HMENU		theMenuBar, appleMenu, gameMenu, optionsMenu, houseMenu;
LPWSTR		appleMenuTitle, gameMenuTitle, optionsMenuTitle, houseMenuTitle;
Boolean		menusUp, resumedSavedGame;


extern	SInt32		incrementModeTime;
extern	SInt16		demoHouseIndex, wasHouseVersion;
extern	SInt16		splashOriginH, splashOriginV, numberRooms;
extern	Boolean		quitting, noRoomAtAll, twoPlayerGame;
extern	Boolean		isMapOpen, isToolsOpen, isPlayMusicIdle;
extern	Boolean		isCoordOpen, failedMusic, splashDrawn;
extern	Boolean		houseOpen;


//==============================================================  Functions
//--------------------------------------------------------------  UpdateMenusEditMode
// Sets the menus to reflect that user is in edit mode.

void UpdateMenusEditMode (void)
{
	EnableMenuItem(gameMenu, ID_NEW_GAME, MF_GRAYED);
	EnableMenuItem(gameMenu, ID_TWO_PLAYER, MF_GRAYED);
	EnableMenuItem(gameMenu, ID_OPEN_SAVED_GAME, MF_GRAYED);
	EnableMenuItem(optionsMenu, ID_HIGH_SCORES, MF_GRAYED);
	EnableMenuItem(optionsMenu, ID_DEMO, MF_GRAYED);
	CheckMenuItem(optionsMenu, ID_EDITOR, MF_CHECKED);
}

//--------------------------------------------------------------  UpdateMenusNonEditMode
// Sets the menus to reflect that user is NOT in edit mode.

void UpdateMenusNonEditMode (void)
{
	if ((noRoomAtAll) || (!houseOpen) || (numberRooms <= 0))
	{
		EnableMenuItem(gameMenu, ID_NEW_GAME, MF_GRAYED);
		EnableMenuItem(gameMenu, ID_TWO_PLAYER, MF_GRAYED);
		EnableMenuItem(gameMenu, ID_OPEN_SAVED_GAME, MF_GRAYED);
		if (houseOpen)
		{
			EnableMenuItem(gameMenu, ID_LOAD_HOUSE, MF_ENABLED);
			EnableMenuItem(optionsMenu, ID_HIGH_SCORES, MF_ENABLED);
		}
		else
		{
			EnableMenuItem(gameMenu, ID_LOAD_HOUSE, MF_GRAYED);
			EnableMenuItem(optionsMenu, ID_HIGH_SCORES, MF_GRAYED);
		}
	}
	else
	{
		EnableMenuItem(gameMenu, ID_NEW_GAME, MF_ENABLED);
		EnableMenuItem(gameMenu, ID_TWO_PLAYER, MF_ENABLED);
		EnableMenuItem(gameMenu, ID_OPEN_SAVED_GAME, MF_ENABLED);
		EnableMenuItem(gameMenu, ID_LOAD_HOUSE, MF_ENABLED);
		EnableMenuItem(optionsMenu, ID_HIGH_SCORES, MF_ENABLED);
	}
	if (demoHouseIndex == -1)
		EnableMenuItem(optionsMenu, ID_DEMO, MF_GRAYED);
	else
		EnableMenuItem(optionsMenu, ID_DEMO, MF_ENABLED);
	CheckMenuItem(optionsMenu, ID_EDITOR, MF_UNCHECKED);
}

//--------------------------------------------------------------  UpdateMenusHouseOpen
// Sets the menus to reflect that a house is currently open.

void UpdateMenusHouseOpen (void)
{
	EnableMenuItem(gameMenu, ID_LOAD_HOUSE, MF_ENABLED);
	if ((fileDirty) && (houseUnlocked))
		EnableMenuItem(houseMenu, ID_SAVE_HOUSE, MF_ENABLED);
	else
		EnableMenuItem(houseMenu, ID_SAVE_HOUSE, MF_GRAYED);
	if (houseUnlocked)
	{
		// EnableMenuItem(houseMenu, iSaveAs);
		EnableMenuItem(houseMenu, ID_HOUSE_INFO, MF_ENABLED);
	}
	else
	{
		// DisableMenuItem(houseMenu, iSaveAs);
		EnableMenuItem(houseMenu, ID_HOUSE_INFO, MF_GRAYED);
	}
	if ((noRoomAtAll) || (!houseUnlocked))
		EnableMenuItem(houseMenu, ID_ROOM_INFO, MF_GRAYED);
	else
		EnableMenuItem(houseMenu, ID_ROOM_INFO, MF_ENABLED);
	if ((objActive == kNoObjectSelected) || (!houseUnlocked))
	{
		EnableMenuItem(houseMenu, ID_OBJECT_INFO, MF_GRAYED);
		EnableMenuItem(houseMenu, ID_BRING_FORWARD, MF_GRAYED);
		EnableMenuItem(houseMenu, ID_SEND_BACK, MF_GRAYED);
	}
	else
	{
		EnableMenuItem(houseMenu, ID_OBJECT_INFO, MF_ENABLED);
		if ((objActive == kInitialGliderSelected) ||
				(objActive == kLeftGliderSelected) ||
				(objActive == kRightGliderSelected))
		{
			EnableMenuItem(houseMenu, ID_BRING_FORWARD, MF_GRAYED);
			EnableMenuItem(houseMenu, ID_SEND_BACK, MF_GRAYED);
		}
		else
		{
			EnableMenuItem(houseMenu, ID_BRING_FORWARD, MF_ENABLED);
			EnableMenuItem(houseMenu, ID_SEND_BACK, MF_ENABLED);
		}
	}
}

//--------------------------------------------------------------  UpdateMenusHouseClosed
// Sets the menus to reflect that a house is NOT currently open.

void UpdateMenusHouseClosed (void)
{
	EnableMenuItem(gameMenu, ID_LOAD_HOUSE, MF_GRAYED);
	EnableMenuItem(houseMenu, ID_SAVE_HOUSE, MF_GRAYED);
	// DisableMenuItem(houseMenu, iSaveAs);
	EnableMenuItem(houseMenu, ID_HOUSE_INFO, MF_GRAYED);
	EnableMenuItem(houseMenu, ID_ROOM_INFO, MF_GRAYED);
	EnableMenuItem(houseMenu, ID_OBJECT_INFO, MF_GRAYED);
	EnableMenuItem(houseMenu, ID_CUT, MF_GRAYED);
	EnableMenuItem(houseMenu, ID_COPY, MF_GRAYED);
	EnableMenuItem(houseMenu, ID_PASTE, MF_GRAYED);
	EnableMenuItem(houseMenu, ID_CLEAR, MF_GRAYED);
	EnableMenuItem(houseMenu, ID_DUPLICATE, MF_GRAYED);
}

//--------------------------------------------------------------  UpdateClipboardMenus
// Set the Cut/Copy/Paste menus to reflect if we have data in the…
// Mac's "clipboard" or not.

static BOOL SetMenuItemText(HMENU hMenu, UINT uID, StringPtr newTitle)
{
	MENUITEMINFO mii;
	WCHAR theString[256];

	WinFromMacString(theString, ARRAYSIZE(theString), newTitle);
	mii.cbSize = sizeof(mii);
	mii.fMask = MIIM_STRING;
	mii.dwTypeData = theString;
	return SetMenuItemInfo(hMenu, uID, FALSE, &mii);
}

void UpdateClipboardMenus (void)
{
	Str255		title;

	if (!houseOpen)
		return;

	if (houseUnlocked)
	{
		if (objActive > kNoObjectSelected)
		{
			GetLocalizedString(36, title);
			SetMenuItemText(houseMenu, ID_CUT, title);
			GetLocalizedString(37, title);
			SetMenuItemText(houseMenu, ID_COPY, title);
			GetLocalizedString(38, title);
			SetMenuItemText(houseMenu, ID_CLEAR, title);
			EnableMenuItem(houseMenu, ID_DUPLICATE, MF_ENABLED);
		}
		else
		{
			GetLocalizedString(39, title);
			SetMenuItemText(houseMenu, ID_CUT, title);
			GetLocalizedString(40, title);
			SetMenuItemText(houseMenu, ID_COPY, title);
			GetLocalizedString(41, title);
			SetMenuItemText(houseMenu, ID_CLEAR, title);
			EnableMenuItem(houseMenu, ID_DUPLICATE, MF_GRAYED);
		}

		// TODO: enable these menu items when the clipboard can actually be used
		// EnableMenuItem(houseMenu, ID_CUT, MF_ENABLED);
		// EnableMenuItem(houseMenu, ID_COPY, MF_ENABLED);
		EnableMenuItem(houseMenu, ID_CUT, MF_GRAYED);
		EnableMenuItem(houseMenu, ID_COPY, MF_GRAYED);
//		if (hasScrap)
//		{
//			EnableMenuItem(houseMenu, iPaste);
//			if (scrapIsARoom)
//			{
//				GetLocalizedString(42, title);
//				SetMenuItemText(houseMenu, iPaste, title);
//			}
//			else
//			{
//				GetLocalizedString(43, title);
//				SetMenuItemText(houseMenu, iPaste, title);
//			}
//		}
//		else
		{
			EnableMenuItem(houseMenu, ID_PASTE, MF_GRAYED);
			GetLocalizedString(44, title);
			SetMenuItemText(houseMenu, ID_PASTE, title);
		}
		EnableMenuItem(houseMenu, ID_CLEAR, MF_ENABLED);
		EnableMenuItem(houseMenu, ID_GO_TO_ROOM, MF_ENABLED);
		EnableMenuItem(houseMenu, ID_MAP_WINDOW, MF_ENABLED);
		EnableMenuItem(houseMenu, ID_OBJECT_WINDOW, MF_ENABLED);
		EnableMenuItem(houseMenu, ID_COORDINATE_WINDOW, MF_ENABLED);
	}
	else
	{
		EnableMenuItem(houseMenu, ID_CUT, MF_GRAYED);
		EnableMenuItem(houseMenu, ID_COPY, MF_GRAYED);
		EnableMenuItem(houseMenu, ID_PASTE, MF_GRAYED);
		EnableMenuItem(houseMenu, ID_CLEAR, MF_GRAYED);
		EnableMenuItem(houseMenu, ID_DUPLICATE, MF_GRAYED);
		EnableMenuItem(houseMenu, ID_GO_TO_ROOM, MF_GRAYED);
		EnableMenuItem(houseMenu, ID_MAP_WINDOW, MF_GRAYED);
		EnableMenuItem(houseMenu, ID_OBJECT_WINDOW, MF_GRAYED);
		EnableMenuItem(houseMenu, ID_COORDINATE_WINDOW, MF_GRAYED);
	}
}

//--------------------------------------------------------------  UpdateMenus
// Called whenever a significant change to the environment has taken…
// place and some of the menu states may have changes (for example,…
// a menui was grayed out before becuase it wasn't an option - now…
// perhaps the situation has changed and we want the menu to be "usable").

void UpdateMenus (Boolean newMode)
{
	MENUITEMINFO mii;

	if (!menusUp)
		return;

	if (newMode)
	{
		if (theMode == kEditMode)
		{
			mii.cbSize = sizeof(mii);
			mii.fMask = MIIM_ID | MIIM_STRING | MIIM_SUBMENU;
			mii.wID = kHouseMenuID;
			mii.hSubMenu = houseMenu;
			mii.dwTypeData = houseMenuTitle;
			InsertMenuItem(theMenuBar, kAppleMenuID, FALSE, &mii);
		}
		else
		{
			RemoveMenu(theMenuBar, kHouseMenuID, MF_BYCOMMAND);
		}
	}

	if (theMode == kEditMode)
	{
		UpdateMenusEditMode();
		if (houseOpen)
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

	if (mainWindow != NULL)
		DrawMenuBar(mainWindow);
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
		twoPlayerGame = false;
		resumedSavedGame = false;
		DisableMenuBar();
		NewGame(hwnd, kNewGameMode);
		EnableMenuBar();
		break;

		case iTwoPlayer:
		twoPlayerGame = true;
		resumedSavedGame = false;
		DisableMenuBar();
		NewGame(hwnd, kNewGameMode);
		EnableMenuBar();
		break;

		case iOpenSavedGame:
		resumedSavedGame = true;
		HeyYourPissingAHighScore(hwnd);
		if (OpenSavedGame(hwnd))
		{
			twoPlayerGame = false;
			DisableMenuBar();
			NewGame(hwnd, kResumeGameMode);
			EnableMenuBar();
		}
		break;

		case iLoadHouse:
#ifdef COMPILEDEMO
		DoNotInDemo(hwnd);
#else
		if (splashDrawn)
		{
			DoLoadHouse(hwnd);
			OpenCloseEditWindows();
			UpdateMenus(false);
			incrementModeTime = MillisToTicks(GetTickCount()) + kIdleSplashTicks;
			if ((theMode == kSplashMode) || (theMode == kPlayMode))
			{
				Rect		updateRect;

				QSetRect(&updateRect, splashOriginH + 474, splashOriginV + 304,
						splashOriginH + 474 + 166, splashOriginV + 304 + 12);
				Mac_InvalWindowRect(mainWindow, &updateRect);
			}
		}
#endif
		break;

		case iQuit:
#ifndef COMPILEDEMO
		quitting = true;
		if (!QuerySaveChanges(hwnd))
			quitting = false;
#else
		quitting = true;
#endif
		if (quitting)
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
#ifdef COMPILEDEMO
		DoNotInDemo(hwnd);
#else
		if (theMode == kEditMode)			// switching to splash mode
		{
			if (fileDirty)
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
			theMode = kSplashMode;
			OpenMainWindow();
			incrementModeTime = MillisToTicks(GetTickCount()) + kIdleSplashTicks;
		}
		else if (theMode == kSplashMode)	// switching to edit mode
		{
			StopTheMusic();
			CloseMainWindow();
			theMode = kEditMode;
			OpenMainWindow();
			OpenCloseEditWindows();
		}
		//InitCursor();
		UpdateMenus(true);
#endif
		break;

		case iHighScores:
		DisableMenuBar();
		DoHighScores();
		EnableMenuBar();
		incrementModeTime = MillisToTicks(GetTickCount()) + kIdleSplashTicks;
		break;

		case iPrefs:
		DoSettingsMain(hwnd);
		incrementModeTime = MillisToTicks(GetTickCount()) + kIdleSplashTicks;
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
#ifndef COMPILEDEMO
	SInt16		direction, dist;
	Boolean		whoCares;

	switch (theItem)
	{
		case iNewHouse:
		if (CreateNewHouse())
		{
			whoCares = InitializeEmptyHouse(hwnd);
			OpenCloseEditWindows();
		}
		break;

		case iSave:
		DeselectObject();
		if (fileDirty)
			SortHouseObjects();
		if ((fileDirty) && (houseUnlocked))
		{
//			SaveGame(false);
			if (wasHouseVersion < kHouseVersion)
				ConvertHouseVer1To2();
			wasHouseVersion = kHouseVersion;
			whoCares = WriteHouse(hwnd, true);
			ForceThisRoom(thisRoomNumber);
			ReadyBackground(thisRoom->background, thisRoom->tiles);
			GetThisRoomsObjRects();
			DrawThisRoomsObjects();
		}
		break;

//		case iSaveAs:
//		whoCares = SaveHouseAs();
//		break;

		case iHouse:
		if (houseUnlocked)
			DoHouseInfo(hwnd);
		break;

		case iRoom:
		if (houseUnlocked)
			DoRoomInfo();
		break;

		case iObject:
		if (houseUnlocked)
		{
			DoObjectInfo(hwnd);
			if (ObjectHasHandle(&direction, &dist))
			{
				StartMarqueeHandled(&roomObjectRects[objActive], direction, dist);
				HandleBlowerGlider();
			}
			else
				StartMarquee(&roomObjectRects[objActive]);
		}
		break;

		case iCut:
		if (houseUnlocked)
		{
			if (objActive > kNoObjectSelected)
			{
//				PutObjectScrap();
				Gp_DeleteObject();
			}
			else
			{
//				PutRoomScrap();
				DeleteRoom(hwnd, false);
			}
			UpdateClipboardMenus();
		}
		break;

		case iCopy:
		if (houseUnlocked)
		{
//			if (objActive > kNoObjectSelected)
//				PutObjectScrap();
//			else
//				PutRoomScrap();
			UpdateClipboardMenus();
		}
		break;

		case iPaste:
		if (houseUnlocked)
		{
/*			if (scrapIsARoom)
				GetRoomScrap();
			else
				GetObjectScrap();
			UpdateClipboardMenus();
*/
		}
		break;

		case iClear:
		if (houseUnlocked)
		{
			if (objActive > kNoObjectSelected)
				Gp_DeleteObject();
			else
				DeleteRoom(hwnd, false);
			UpdateClipboardMenus();
		}
		break;

		case iDuplicate:
		if (houseUnlocked)
			DuplicateObject(hwnd);
		break;

		case iBringForward:
		if (houseUnlocked)
			BringSendFrontBack(hwnd, true);
		break;

		case iSendBack:
		if (houseUnlocked)
			BringSendFrontBack(hwnd, false);
		break;

		case iGoToRoom:
		if (houseUnlocked)
			DoGoToDialog(hwnd);
		break;

		case iMapWindow:
		if (houseUnlocked)
			ToggleMapWindow();
		break;

		case iObjectWindow:
		if (houseUnlocked)
			ToggleToolsWindow();
		break;

		case iCoordinateWindow:
		if (houseUnlocked)
			ToggleCoordinateWindow();
		break;
	}
#endif
}

//--------------------------------------------------------------  DoMenuChoice
// Users has selected a menu item - determin which menu was selected…
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
	if (!menusUp)
		return;

	if (checkIt)
		CheckMenuItem(houseMenu, ID_MAP_WINDOW, MF_CHECKED);
	else
		CheckMenuItem(houseMenu, ID_MAP_WINDOW, MF_UNCHECKED);
}

//--------------------------------------------------------------  UpdateToolsCheckmark
// Checks or unchecks the Tools Window item (to indicate if open or not).

void UpdateToolsCheckmark (Boolean checkIt)
{
	if (!menusUp)
		return;

	if (checkIt)
		CheckMenuItem(houseMenu, ID_OBJECT_WINDOW, MF_CHECKED);
	else
		CheckMenuItem(houseMenu, ID_OBJECT_WINDOW, MF_UNCHECKED);
}

//--------------------------------------------------------------  UpdateCoordinateCheckmark
// Checks or unchecks the Coordinates Window item (to indicate if open or not).

void UpdateCoordinateCheckmark (Boolean checkIt)
{
	if (!menusUp)
		return;

	if (checkIt)
		CheckMenuItem(houseMenu, ID_COORDINATE_WINDOW, MF_CHECKED);
	else
		CheckMenuItem(houseMenu, ID_COORDINATE_WINDOW, MF_UNCHECKED);
}

//--------------------------------------------------------------  ResumeFilter
// Dialog filter for the Resume dialog (below).

INT_PTR CALLBACK ResumeFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		CenterOverOwner(hDlg);
		ParamDialogText(hDlg, (const DialogParams *)lParam);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case kSheWantsNewGame:
			EndDialog(hDlg, kSheWantsNewGame);
			break;

		case kSheWantsResumeGame:
			EndDialog(hDlg, kSheWantsResumeGame);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  QueryResumeGame
// Dialog that asks user whether they want to resume a saved game or…
// begin a new one.  It displays a little info on the state of their…
// saved game (number of glider left, points, etc.).

SInt16 QueryResumeGame (HWND ownerWindow)
{
	DialogParams params;
	Str255 scoreStr, glidStr;
	SInt32 hadPoints;
	SInt16 hadGliders;

	hadPoints = thisHouse->savedGame.score;
	hadGliders = thisHouse->savedGame.numGliders;
	Mac_NumToString(hadPoints, scoreStr);
	Mac_NumToString(hadGliders, glidStr);
	WinFromMacString(params.arg[0], ARRAYSIZE(params.arg[0]), glidStr);
	if (hadGliders == 1)
		StringCchCopy(params.arg[1], ARRAYSIZE(params.arg[1]), L"");
	else
		StringCchCopy(params.arg[1], ARRAYSIZE(params.arg[1]), L"s");
	WinFromMacString(params.arg[2], ARRAYSIZE(params.arg[2]), scoreStr);

	return (SInt16)DialogBoxParam(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(kResumeGameDial), ownerWindow,
			ResumeFilter, (LPARAM)&params);
}

//--------------------------------------------------------------  DoNotInDemo
// Only compiled for "demo version" of Glider PRO.  It brings up a…
// dialog that says, essentially, "x" feature is  not implemented in…
// the demo version.

#ifdef COMPILEDEMO
void DoNotInDemo (HWND ownerWindow)
{
	Alert(kNotInDemoAlert, ownerWindow, NULL);
}
#endif

//--------------------------------------------------------------  HeyYourPissingAHighScore

void HeyYourPissingAHighScore (HWND ownerWindow)
{
	Alert(kNoHighScoreAlert, ownerWindow, NULL);
}

//--------------------------------------------------------------  OpenCloseEditWindows
// Function goes through and either closes or opens all the various…
// editing windows (in response to switching in or out of editor).

void OpenCloseEditWindows (void)
{
	if (theMode == kEditMode)
	{
		if (houseUnlocked)
		{
			if (isMapOpen)
				OpenMapWindow();
			if (isToolsOpen)
				OpenToolsWindow();
			if (isCoordOpen)
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

	n = GetMenuItemCount(theMenuBar);
	for (i = 0; i < n; i++)
		EnableMenuItem(theMenuBar, i, MF_BYPOSITION | MF_ENABLED);
	DrawMenuBar(mainWindow);
}

//--------------------------------------------------------------  EnableMenuBar
// Similar to the above function, but this one disables all of the menu bar's
// items, insteading of enabling them.

void DisableMenuBar(void)
{
	int i, n;

	n = GetMenuItemCount(theMenuBar);
	for (i = 0; i < n; i++)
		EnableMenuItem(theMenuBar, i, MF_BYPOSITION | MF_GRAYED);
	DrawMenuBar(mainWindow);
}


