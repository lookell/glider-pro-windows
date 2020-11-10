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
#include "Marquee.h"
#include "Menu.h"
#include "ObjectEdit.h"
#include "Play.h"
#include "Room.h"
#include "RoomInfo.h"
#include "SelectHouse.h"
#include "Tools.h"

#include <mmsystem.h>


void HandleIdleTask (void);


UInt32 incrementModeTime;
Boolean doAutoDemo;
Boolean switchedOut;
Boolean ignoreDoubleClick;
HACCEL splashAccelTable;
HACCEL editAccelTable;


//==============================================================  Functions
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
// "Master" function that tests for events and calls the above functions to
// handle each event type.  Not called during and actual game.

void HandleEvent (void)
{
	MSG msg;
	DWORD startMillis;
	DWORD nowMillis;

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
		while (GetMessage(&msg, NULL, 0, 0))
		{
			if (msg.message == WM_QUIT)
			{
				quitting = true;
				return;
			}
			HandleTheMessage(&msg);
			if (!switchedOut)
				break;
		}
	}

	while (PeekMessageOrWaitForFrame(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			quitting = true;
			return;
		}
		HandleTheMessage(&msg);
	}
	HandleIdleTask();

	if ((theMode == kSplashMode) && doAutoDemo && !switchedOut && (demoHouseIndex >= 0))
	{
		startMillis = incrementModeTime - TicksToMillis(kIdleSplashTicks);
		nowMillis = timeGetTime();
		if (nowMillis - startMillis >= TicksToMillis(kIdleSplashTicks))
		{
			DisableMenuBar();
			DoDemoGame(mainWindow);
			EnableMenuBar();
		}
	}
}

//--------------------------------------------------------------  IgnoreThisClick
// An inelegant kludge designed to temporarily prevent an unwanted
// double-click to be registered.

void IgnoreThisClick (void)
{
	ignoreDoubleClick = true;
}

