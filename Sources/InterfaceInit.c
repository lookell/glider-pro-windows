
//============================================================================
//----------------------------------------------------------------------------
//								InterfaceInit.c
//----------------------------------------------------------------------------
//============================================================================


#include "Externs.h"
#include "Environ.h"
#include "Map.h"
#include "RectUtils.h"
#include "ResourceIDs.h"
#include "Tools.h"


#define kHandCursorID		128
#define kVertCursorID		129
#define kHoriCursorID		130
#define kDiagCursorID		131


static HMENU DetachPopupMenu(HMENU rootMenu, UINT id, LPWSTR *title);


extern	HRGN			mirrorRgn;
extern	HWND			mapWindow, toolsWindow, linkWindow;
extern	HWND			menuWindow;
extern	Rect			shieldRect, boardSrcRect, localRoomsDest[];
extern	HCURSOR			handCursor, beamCursor, vertCursor, horiCursor;
extern	HCURSOR			diagCursor;
extern	HMENU			theMenuBar, appleMenu, gameMenu, optionsMenu, houseMenu;
extern	LPWSTR			appleMenuTitle, gameMenuTitle, optionsMenuTitle, houseMenuTitle;
extern	Point			shieldPt;
extern	SInt32			incrementModeTime;
extern	UInt32			doubleTime;
extern	SInt16			fadeInSequence[], idleMode;
extern	SInt16			toolSelected, lastBackground, wasFlower, numExtraHouses;
extern	SInt16			lastHighScore, maxFiles, willMaxFiles;
extern	HMODULE			houseResFork;
extern	Boolean			quitting, playing, fadeGraysOut;
extern	Boolean			houseOpen, newRoomNow, evenFrame, menusUp, demoGoing;
extern	Boolean			twoPlayerGame, paused, hasMirror, splashDrawn;


//==============================================================  Functions
//--------------------------------------------------------------  DetachPopupMenu

// Given a menu containing popup menus, detach a menu and retrieve its
// handle and title. The title string must be released with the standard
// `free` function. If the function succeeds, the return value is the menu
// handle. If the function fails, the return value is NULL and the title
// output paramter is set to NULL.

static HMENU DetachPopupMenu(HMENU rootMenu, UINT id, LPWSTR *title)
{
	MENUITEMINFO mii;
	BOOL succeeded;

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
	mii.dwTypeData = calloc(mii.cch, sizeof(*mii.dwTypeData));
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

	appleMenu = DetachPopupMenu(rootMenu, kAppleMenuID, &appleMenuTitle);
	if (appleMenu == NULL)
		RedAlert(kErrFailedResourceLoad);
	mii.wID = kAppleMenuID;
	mii.hSubMenu = appleMenu;
	mii.dwTypeData = appleMenuTitle;
	InsertMenuItem(theMenuBar, GetMenuItemCount(theMenuBar), TRUE, &mii);

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

	menusUp = true;
	if (mainWindow != NULL)
		SetMenu(mainWindow, theMenuBar);

	houseMenu = DetachPopupMenu(rootMenu, kHouseMenuID, &houseMenuTitle);
	if (houseMenu == NULL)
		RedAlert(kErrFailedResourceLoad);

	UpdateMenus(false);
	DestroyMenu(rootMenu);
}

//--------------------------------------------------------------  GetExtraCursors

// Extra cursors (custom cursors) like the "hand" and various room…
// editing cursors are loaded up.

void GetExtraCursors (void)
{
	handCursor = LoadImage(
		HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kHandCursorID), // TODO: replace with OCR_HAND
		IMAGE_CURSOR,
		0,
		0,
		LR_DEFAULTCOLOR
	);
	if (handCursor == NULL)
		RedAlert(kErrFailedResourceLoad);

	beamCursor = LoadImage(
		NULL,
		MAKEINTRESOURCE(OCR_IBEAM),
		IMAGE_CURSOR,
		0,
		0,
		LR_DEFAULTCOLOR | LR_SHARED
	);
	if (beamCursor == NULL)
		RedAlert(kErrFailedResourceLoad);

	vertCursor = LoadImage(
		HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kVertCursorID), // TODO: replace with OCR_SIZENS
		IMAGE_CURSOR,
		0,
		0,
		LR_DEFAULTCOLOR
	);
	if (vertCursor == NULL)
		RedAlert(kErrFailedResourceLoad);

	horiCursor = LoadImage(
		HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kHoriCursorID), // TODO: replace with OCR_SIZEWE
		IMAGE_CURSOR,
		0,
		0,
		LR_DEFAULTCOLOR
	);
	if (horiCursor == NULL)
		RedAlert(kErrFailedResourceLoad);

	diagCursor = LoadImage(
		HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kDiagCursorID), // TODO: replace with OCR_SIZENWSE
		IMAGE_CURSOR,
		0,
		0,
		LR_DEFAULTCOLOR
	);
	if (diagCursor == NULL)
		RedAlert(kErrFailedResourceLoad);
}

//--------------------------------------------------------------  VariableInit

// All the simple interface variables are intialized here - Booleans,…
// shorts, a few Rects, etc.

void VariableInit (void)
{
	size_t		i;

	shieldPt.h = 0;
	shieldPt.v = 0;
	shieldRect = thisMac.screen;

	theMenuBar = NULL;
	menusUp = false;
	quitting = false;
	houseOpen = false;
	newRoomNow = false;
	playing = false;
	evenFrame = false;
	if (thisMac.isDepth == 8)
		fadeGraysOut = true;
	else
		fadeGraysOut = false;
	twoPlayerGame = false;
	paused = false;
	hasMirror = false;
	demoGoing = false;
//	scrapIsARoom = true;
	splashDrawn = false;

#ifndef COMPILEDEMO
//	SeeIfValidScrapAvailable(false);
#endif

	theGlider.which = kPlayer1;
	theGlider2.leftKey = 'A'; // was kControlKeyMap
	theGlider2.rightKey = 'D'; // was kCommandKeyMap
	theGlider2.battKey = 'S'; // was kOptionKeyMap
	theGlider2.bandKey = 'W'; // was kShiftKeyMap
	theGlider2.which = kPlayer2;

	theMode = kSplashMode;
	thisRoomNumber = 0;
	previousRoom = -1;
	toolSelected = kSelectTool;
	houseResFork = NULL;
	lastBackground = kBaseBackgroundID;
	wasFlower = RandomInt(kNumFlowers);
	lastHighScore = -1;
	idleMode = kIdleSplashMode;
	incrementModeTime = MillisToTicks(GetTickCount()) + kIdleSplashTicks;
	willMaxFiles = maxFiles;
	numExtraHouses = 0;

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

	//doubleTime = GetDblTime();
	doubleTime = MillisToTicks(GetDoubleClickTime());

	mirrorRgn = NULL;
	mainWindow = NULL;
	mapWindow = NULL;
	toolsWindow = NULL;
	linkWindow = NULL;
	coordWindow = NULL;
	toolSrcMap = NULL;
	nailSrcMap = NULL;
	menuWindow = NULL;

	houseRect = thisMac.screen;
	houseRect.bottom -= kScoreboardTall;
	if (houseRect.right > kMaxViewWidth)
		houseRect.right = kMaxViewWidth;
	if (houseRect.bottom > kMaxViewHeight)
		houseRect.bottom = kMaxViewHeight;

	playOriginH = (RectWide(&thisMac.screen) - kRoomWide) / 2;
	playOriginV = (RectTall(&thisMac.screen) - kTileHigh) / 2;

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

