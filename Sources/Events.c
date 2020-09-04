#define GP_USE_WINAPI_H

#include "Events.h"

//============================================================================
//----------------------------------------------------------------------------
//                                  Events.c
//----------------------------------------------------------------------------
//============================================================================


#include "Coordinates.h"
#include "FrameTimer.h"
#include "House.h"
#include "Link.h"
#include "Macintosh.h"
#include "Main.h"
#include "MainWindow.h"
#include "Map.h"
#include "Marquee.h"
#include "Menu.h"
#include "Music.h"
#include "ObjectEdit.h"
#include "Play.h"
#include "Room.h"
#include "RoomInfo.h"
#include "SelectHouse.h"
#include "Tools.h"


void HandleIdleTask (void);


UInt32 incrementModeTime;
Boolean doAutoDemo;
Boolean switchedOut;
Boolean ignoreDoubleClick;
HACCEL splashAccelTable;
HACCEL editAccelTable;


//==============================================================  Functions
//--------------------------------------------------------------  HandleKeyEvent
// Handle a key-down event.

void HandleKeyEvent (HWND hwnd, BYTE vKey)
{
	Boolean shiftDown, commandDown, optionDown;

	shiftDown = (GetKeyState(VK_SHIFT) < 0);
	commandDown = (GetKeyState(VK_CONTROL) < 0);
	optionDown = (GetKeyState(VK_MENU) < 0);

	switch (vKey)
	{
		case VK_PRIOR: // page up
		if (houseUnlocked)
			PrevToolMode();
		break;

		case VK_NEXT: // page down
		if (houseUnlocked)
			NextToolMode();
		break;

#if BUILD_ARCADE_VERSION

		case VK_LEFT:
		DoOptionsMenu(hwnd, iHighScores);
		break;

		case VK_RIGHT:
		DoOptionsMenu(hwnd, iHelp);
		break;

		case VK_UP:
		DoGameMenu(hwnd, iNewGame);
		break;

		case VK_DOWN:
		DoGameMenu(hwnd, iNewGame);
		break;

#else

		case VK_LEFT:
		if (houseUnlocked)
		{
			if (objActive == kNoObjectSelected)
				SelectNeighborRoom(kRoomToLeft);
			else
				MoveObject(kBumpLeft, shiftDown);
		}
		break;

		case VK_RIGHT:
		if (houseUnlocked)
		{
			if (objActive == kNoObjectSelected)
				SelectNeighborRoom(kRoomToRight);
			else
				MoveObject(kBumpRight, shiftDown);
		}
		break;

		case VK_UP:
		if (houseUnlocked)
		{
			if (objActive == kNoObjectSelected)
				SelectNeighborRoom(kRoomAbove);
			else
				MoveObject(kBumpUp, shiftDown);
		}
		break;

		case VK_DOWN:
		if (houseUnlocked)
		{
			if (objActive == kNoObjectSelected)
				SelectNeighborRoom(kRoomBelow);
			else
				MoveObject(kBumpDown, shiftDown);
		}
		break;

#endif

		case VK_DELETE:
		if (houseUnlocked)
		{
			if (objActive == kNoObjectSelected)
				DeleteRoom(hwnd, true);
			else
				Gp_DeleteObject();
		}
		break;

		case VK_TAB:
		if ((theMode == kEditMode) && (houseUnlocked))
		{
			if (shiftDown)
				SelectPrevObject();
			else
				SelectNextObject();
		}
		break;

		case VK_ESCAPE:
		if ((theMode == kEditMode) && (houseUnlocked))
			DeselectObject();
		break;

		case 'A':
		if ((theMode == kEditMode) && (houseUnlocked))
			SetSpecificToolMode(kApplianceMode);
		break;

		case 'B':
		if ((theMode == kEditMode) && (houseUnlocked))
			SetSpecificToolMode(kBlowerMode);
		break;

		case 'C':
		if ((theMode == kEditMode) && (houseUnlocked))
			SetSpecificToolMode(kClutterMode);
		break;

		case 'E':
		if ((theMode == kEditMode) && (houseUnlocked))
			SetSpecificToolMode(kEnemyMode);
		break;

		case 'F':
		if ((theMode == kEditMode) && (houseUnlocked))
			SetSpecificToolMode(kFurnitureMode);
		break;

		case 'L':
		if ((theMode == kEditMode) && (houseUnlocked))
			SetSpecificToolMode(kLightMode);
		break;

		case 'P':
		if ((theMode == kEditMode) && (houseUnlocked))
			SetSpecificToolMode(kBonusMode);
		break;

		case 'S':
		if ((theMode == kEditMode) && (houseUnlocked))
			SetSpecificToolMode(kSwitchMode);
		break;

		case 'T':
		if ((theMode == kEditMode) && (houseUnlocked))
			SetSpecificToolMode(kTransportMode);
		break;

		default:
		break;
	}
}

//--------------------------------------------------------------  HandleIdleTask
// Handle some processing during event lulls.

void HandleIdleTask (void)
{
	if (theMode == kEditMode)
	{
		DoMarquee();

		if ((autoRoomEdit) && (newRoomNow))
		{
			if (theMode == kEditMode)
				DoRoomInfo(mainWindow);
			newRoomNow = false;
		}
	}
}

//--------------------------------------------------------------  HandleTheMessage
// This handles an individual message and dispatches it to the appropriate
// destination.

void HandleTheMessage (MSG *message)
{
	if (IsWindow(coordWindow) && IsDialogMessage(coordWindow, message))
		return;
	if (IsWindow(linkWindow) && IsDialogMessage(linkWindow, message))
		return;
	if (IsWindow(toolsWindow) && IsDialogMessage(toolsWindow, message))
		return;

	if (theMode == kSplashMode)
	{
		if (TranslateAccelerator(mainWindow, splashAccelTable, message))
			return;
	}
	else if (theMode == kEditMode)
	{
		if (TranslateAccelerator(mainWindow, editAccelTable, message))
			return;
	}
	TranslateMessage(message);
	DispatchMessage(message);
}

//--------------------------------------------------------------  HandleEvent
// "Master" function that tests for events and calls the above functions to…
// handle each event type.  Not called during and actual game.

void HandleEvent (void)
{
	MSG theEvent;
	DWORD startMillis, nowMillis;
	BOOL messageReceived;

	if (mainWindow != NULL && GetActiveWindow() == mainWindow)
	{
		// TODO: Maybe don't use the Alt key as an Option key substitute, since
		// it already has meaning for Windows (activating the menu bar, in this
		// scenario).
		if ((GetAsyncKeyState(VK_CONTROL) < 0) &&
				(GetAsyncKeyState(VK_MENU) < 0))
		{
			HiliteAllObjects();
		}
		else if ((GetAsyncKeyState(VK_MENU) < 0) && (theMode == kEditMode) &&
				(houseUnlocked))
		{
			SelectTool(kSelectTool);
		}
	}

	if (switchedOut)
	{
		while (GetMessage(&theEvent, NULL, 0, 0))
		{
			if (theEvent.message == WM_QUIT)
			{
				quitting = true;
				return;
			}
			HandleTheMessage(&theEvent);
			if (!switchedOut)
				break;
		}
	}

	while (1)
	{
		WaitUntilNextFrameOrMessage(&messageReceived);
		if (messageReceived == FALSE)
		{
			break;
		}
		while (PeekMessage(&theEvent, NULL, 0, 0, PM_REMOVE))
		{
			if (theEvent.message == WM_QUIT)
			{
				quitting = true;
				return;
			}
			HandleTheMessage(&theEvent);
		}
	}
	HandleIdleTask();

	if ((theMode == kSplashMode) && doAutoDemo && !switchedOut && (demoHouseIndex >= 0))
	{
		startMillis = incrementModeTime - TicksToMillis(kIdleSplashTicks);
		nowMillis = GetTickCount();
		if (nowMillis - startMillis >= TicksToMillis(kIdleSplashTicks))
		{
			DisableMenuBar();
			DoDemoGame(mainWindow);
			EnableMenuBar();
		}
	}
}

//--------------------------------------------------------------  IgnoreThisClick

// An inelegant kludge designed to temporarily prevent an unwanted…
// double-click to be registered.

void IgnoreThisClick (void)
{
	ignoreDoubleClick = true;
}

