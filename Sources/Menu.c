
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


#define kSheWantsNewGame		1
#define kSheWantsResumeGame		2


void UpdateMenusEditMode (void);
void UpdateMenusNonEditMode (void);
void UpdateMenusHouseOpen (void);
void UpdateMenusHouseClosed (void);
void UpdateResumeDialog (DialogPtr);
Boolean ResumeFilter (DialogPtr, EventRecord *, SInt16 *);
SInt16 QueryResumeGame (void);
void HeyYourPissingAHighScore (void);


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

		EnableMenuItem(houseMenu, ID_CUT, MF_ENABLED);
		EnableMenuItem(houseMenu, ID_COPY, MF_ENABLED);
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
	MENUITEMINFO	mii;

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
			InsertMenuItem(theMenuBar, GetMenuItemCount(theMenuBar), TRUE, &mii);
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

void DoAppleMenu (SInt16 theItem)
{
//	Str255		daName;
//	GrafPtr		wasPort;
//	short		daNumber;

	switch (theItem)
	{
		case iAbout:
		DoAbout();
		break;

		default:
//		GetMenuItemText(appleMenu, theItem, daName);
//		GetPort(&wasPort);
//		daNumber = OpenDeskAccesory(daName);
//		SetPort((GrafPtr)wasPort);
		break;
	}
}

//--------------------------------------------------------------  DoGameMenu
// Handle the user selecting an item from the Game menu.

void DoGameMenu (SInt16 theItem)
{
	switch (theItem)
	{
		case iNewGame:
		twoPlayerGame = false;
		resumedSavedGame = false;
		NewGame(kNewGameMode);
		break;

		case iTwoPlayer:
		twoPlayerGame = true;
		resumedSavedGame = false;
		NewGame(kNewGameMode);
		break;

		case iOpenSavedGame:
		resumedSavedGame = true;
		HeyYourPissingAHighScore();
		if (OpenSavedGame())
		{
			twoPlayerGame = false;
			NewGame(kResumeGameMode);
		}
		break;

		case iLoadHouse:
#ifdef COMPILEDEMO
		DoNotInDemo();
#else
		if (splashDrawn)
		{
			DoLoadHouse();
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
		if (!QuerySaveChanges())
			quitting = false;
#else
		quitting = true;
#endif
		break;

		default:
		break;
	}
}

//--------------------------------------------------------------  DoOptionsMenu
// Handle the user selecting an item from the Options menu.

void DoOptionsMenu (SInt16 theItem)
{
#ifndef COMPILEDEMO
	OSErr		theErr;
#endif

	switch (theItem)
	{
		case iEditor:
#ifdef COMPILEDEMO
		DoNotInDemo();
#else
		if (theMode == kEditMode)			// switching to splash mode
		{
			if (fileDirty)
				SortHouseObjects();
			if (!QuerySaveChanges())
				break;
			theMode = kSplashMode;
			CloseMapWindow();
			CloseToolsWindow();
			CloseCoordWindow();
			CloseLinkWindow();
			DeselectObject();
			StopMarquee();
			if (isPlayMusicIdle)
			{
				theErr = StartMusic();
				if (theErr != noErr)
				{
					YellowAlert(kYellowNoMusic, theErr);
					failedMusic = true;
				}
			}
			CloseMainWindow();
			OpenMainWindow();
			incrementModeTime = MillisToTicks(GetTickCount()) + kIdleSplashTicks;
		}
		else if (theMode == kSplashMode)	// switching to edit mode
		{
			theMode = kEditMode;
			StopTheMusic();
			CloseMainWindow();
			OpenMainWindow();
			OpenCloseEditWindows();
		}
		//InitCursor();
		UpdateMenus(true);
#endif
		break;

		case iHighScores:
		DoHighScores();
		incrementModeTime = MillisToTicks(GetTickCount()) + kIdleSplashTicks;
		break;

		case iPrefs:
		DoSettingsMain();
		incrementModeTime = MillisToTicks(GetTickCount()) + kIdleSplashTicks;
		break;

		case iHelp:
		DoDemoGame();
		break;
	}
}

//--------------------------------------------------------------  DoHouseMenu
// Handle the user selecting an item from the House menu (only in Edit mode).

void DoHouseMenu (SInt16 theItem)
{
#ifndef COMPILEDEMO
	SInt16		direction, dist;
	Boolean		whoCares;

	switch (theItem)
	{
		case iNewHouse:
		if (CreateNewHouse())
		{
			whoCares = InitializeEmptyHouse();
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
			whoCares = WriteHouse(true);
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
			DoHouseInfo();
		break;

		case iRoom:
		if (houseUnlocked)
			DoRoomInfo();
		break;

		case iObject:
		if (houseUnlocked)
		{
			DoObjectInfo();
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
				DeleteRoom(false);
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
				DeleteRoom(false);
			UpdateClipboardMenus();
		}
		break;

		case iDuplicate:
		if (houseUnlocked)
			DuplicateObject();
		break;

		case iBringForward:
		if (houseUnlocked)
			BringSendFrontBack(true);
		break;

		case iSendBack:
		if (houseUnlocked)
			BringSendFrontBack(false);
		break;

		case iGoToRoom:
		if (houseUnlocked)
			DoGoToDialog();
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

void DoMenuChoice (WORD menuChoice)
{
	switch (menuChoice)
	{
		case ID_ABOUT:
		DoAppleMenu(iAbout);
		break;

		case ID_NEW_GAME:
		DoGameMenu(iNewGame);
		break;

		case ID_TWO_PLAYER:
		DoGameMenu(iTwoPlayer);
		break;

		case ID_OPEN_SAVED_GAME:
		DoGameMenu(iOpenSavedGame);
		break;

		case ID_LOAD_HOUSE:
		DoGameMenu(iLoadHouse);
		break;

		case ID_QUIT:
		DoGameMenu(iQuit);
		break;

		case ID_EDITOR:
		DoOptionsMenu(iEditor);
		break;

		case ID_HIGH_SCORES:
		DoOptionsMenu(iHighScores);
		break;

		case ID_PREFS:
		DoOptionsMenu(iPrefs);
		break;

		case ID_DEMO:
		DoOptionsMenu(iHelp);
		break;

		case ID_NEW_HOUSE:
		DoHouseMenu(iNewHouse);
		break;

		case ID_SAVE_HOUSE:
		DoHouseMenu(iSave);
		break;

		case ID_HOUSE_INFO:
		DoHouseMenu(iHouse);
		break;

		case ID_ROOM_INFO:
		DoHouseMenu(iRoom);
		break;

		case ID_OBJECT_INFO:
		DoHouseMenu(iObject);
		break;

		case ID_CUT:
		DoHouseMenu(iCut);
		break;

		case ID_COPY:
		DoHouseMenu(iCopy);
		break;

		case ID_PASTE:
		DoHouseMenu(iPaste);
		break;

		case ID_CLEAR:
		DoHouseMenu(iClear);
		break;

		case ID_DUPLICATE:
		DoHouseMenu(iDuplicate);
		break;

		case ID_BRING_FORWARD:
		DoHouseMenu(iBringForward);
		break;

		case ID_SEND_BACK:
		DoHouseMenu(iSendBack);
		break;

		case ID_GO_TO_ROOM:
		DoHouseMenu(iGoToRoom);
		break;

		case ID_MAP_WINDOW:
		DoHouseMenu(iMapWindow);
		break;

		case ID_OBJECT_WINDOW:
		DoHouseMenu(iObjectWindow);
		break;

		case ID_COORDINATE_WINDOW:
		DoHouseMenu(iCoordinateWindow);
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

//--------------------------------------------------------------  UpdateResumeDialog
// Update function for Resume dialog (below).

void UpdateResumeDialog (DialogPtr theDialog)
{
	return;
#if 0
	DrawDialog(theDialog);
	DrawDefaultButton(theDialog);
#endif
}

//--------------------------------------------------------------  ResumeFilter
// Dialog filter for the Resume dialog (below).

Boolean ResumeFilter (DialogPtr dial, EventRecord *event, SInt16 *item)
{
	return false;
#if 0
	switch (event->what)
	{
		case keyDown:
		switch ((event->message) & charCodeMask)
		{
			case kReturnKeyASCII:
			case kEnterKeyASCII:
			FlashDialogButton(dial, kOkayButton);
			*item = kOkayButton;
			return(true);
			break;

			default:
			return(false);
		}
		break;

		case updateEvt:
		if ((WindowPtr)event->message == GetDialogWindow(dial))
		{
			SetPort((GrafPtr)dial);
			BeginUpdate(GetDialogWindow(dial));
			UpdateResumeDialog(dial);
			EndUpdate(GetDialogWindow(dial));
			event->what = nullEvent;
		}
		return(false);
		break;

		default:
		return(false);
		break;
	}
#endif
}

//--------------------------------------------------------------  QueryResumeGame
// Dialog that asks user whether they want to resume a saved game or…
// begin a new one.  It displays a little info on the state of their…
// saved game (number of glider left, points, etc.).

SInt16 QueryResumeGame (void)
{
	return kSheWantsNewGame;
#if 0
	#define			kResumeGameDial		1025
	DialogPtr		theDial;
	houseType		*thisHousePtr;
	Str255			scoreStr, glidStr;
	long			hadPoints;
	short			hitWhat, hadGliders;
	char			wasState;
	Boolean			leaving;
	ModalFilterUPP	resumeFilterUPP;

	resumeFilterUPP = NewModalFilterUPP(ResumeFilter);

	wasState = HGetState((Handle)thisHouse);	// get score & num. gliders
	HLock((Handle)thisHouse);
	thisHousePtr = *thisHouse;
	hadPoints = thisHousePtr->savedGame.score;
	hadGliders = thisHousePtr->savedGame.numGliders;
	HSetState((Handle)thisHouse, wasState);
	NumToString(hadPoints, scoreStr);			// param text strings
	NumToString((long)hadGliders, glidStr);
	if (hadGliders == 1)
		ParamText(glidStr, "\p", scoreStr, "\p");
	else
		ParamText(glidStr, "\ps", scoreStr, "\p");

//	CenterDialog(kResumeGameDial);
	theDial = GetNewDialog(kResumeGameDial, nil, kPutInFront);
	if (theDial == nil)
		RedAlert(kErrDialogDidntLoad);
	SetPort((GrafPtr)theDial);

	ShowWindow(GetDialogWindow(theDial));
	DrawDefaultButton(theDial);
	leaving = false;

	while (!leaving)
	{
		ModalDialog(resumeFilterUPP, &hitWhat);
		if ((hitWhat == kSheWantsNewGame) || (hitWhat == kSheWantsResumeGame))
		{
			leaving = true;
		}
	}
	DisposeDialog(theDial);
	DisposeModalFilterUPP(resumeFilterUPP);

	return (hitWhat);
#endif
}

//--------------------------------------------------------------  DoNotInDemo
// Only compiled for "demo version" of Glider PRO.  It brings up a…
// dialog that says, essentially, "x" feature is  not implemented in…
// the demo version.

#ifdef COMPILEDEMO
void DoNotInDemo (void)
{
	return;
#if 0
	#define		kNotInDemoAlert		1037
	short		whoCares;

//	CenterAlert(kNotInDemoAlert);
	whoCares = Alert(kNotInDemoAlert, nil);
#endif
}
#endif

//--------------------------------------------------------------  HeyYourPissingAHighScore

void HeyYourPissingAHighScore (void)
{
	return;
#if 0
	#define		kNoHighScoreAlert	1046
	short		whoCares;

//	CenterAlert(kNoHighScoreAlert);
	whoCares = Alert(kNoHighScoreAlert, nil);
#endif
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

