//============================================================================
//----------------------------------------------------------------------------
//                                  Events.c
//----------------------------------------------------------------------------
//============================================================================

#include "Events.h"

#include "Coordinates.h"
#include "FrameTimer.h"
#include "House.h"
#include "Link.h"
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

UInt32 g_incrementModeTime;
Boolean g_doAutoDemo;
Boolean g_switchedOut;
Boolean g_ignoreDoubleClick;
HACCEL g_splashAccelTable;
HACCEL g_editAccelTable;

//==============================================================  Functions
//--------------------------------------------------------------  HandleIdleTask
// Handle some processing during event lulls.

void HandleIdleTask (void)
{
	if (g_theMode == kEditMode)
	{
		DoMarquee();

		if ((g_autoRoomEdit) && (g_newRoomNow))
		{
			if (g_theMode == kEditMode)
				DoRoomInfo(g_mainWindow);
			g_newRoomNow = false;
		}
	}
}

//--------------------------------------------------------------  HandleTheMessage
// This handles an individual message and dispatches it to the appropriate
// destination.

void HandleTheMessage (MSG *message)
{
	if (IsWindow(g_coordWindow) && IsDialogMessage(g_coordWindow, message))
		return;
	if (IsWindow(g_linkWindow) && IsDialogMessage(g_linkWindow, message))
		return;
	if (IsWindow(g_toolsWindow) && IsDialogMessage(g_toolsWindow, message))
		return;

	if (g_theMode == kSplashMode)
	{
		if (TranslateAccelerator(g_mainWindow, g_splashAccelTable, message))
			return;
	}
	else if (g_theMode == kEditMode)
	{
		if (TranslateAccelerator(g_mainWindow, g_editAccelTable, message))
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

	if (g_mainWindow != NULL && GetActiveWindow() == g_mainWindow)
	{
		// TODO: Maybe don't use the Alt key as an Option key substitute, since
		// it already has meaning for Windows (activating the menu bar, in this
		// scenario).
		if ((GetAsyncKeyState(VK_CONTROL) < 0) &&
				(GetAsyncKeyState(VK_MENU) < 0))
		{
			HiliteAllObjects();
		}
		else if ((GetAsyncKeyState(VK_MENU) < 0) && (g_theMode == kEditMode) &&
				(g_houseUnlocked))
		{
			SelectTool(kSelectTool);
		}
	}

	if (g_switchedOut)
	{
		while (GetMessage(&msg, NULL, 0, 0))
		{
			if (msg.message == WM_QUIT)
			{
				g_quitting = true;
				return;
			}
			HandleTheMessage(&msg);
			if (!g_switchedOut)
				break;
		}
	}

	while (PeekMessageOrWaitForFrame(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			g_quitting = true;
			return;
		}
		HandleTheMessage(&msg);
	}
	HandleIdleTask();

	if ((g_theMode == kSplashMode) && g_doAutoDemo && !g_switchedOut && (g_demoHouseIndex >= 0))
	{
		startMillis = g_incrementModeTime - kIdleSplashTime;
		nowMillis = timeGetTime();
		if (nowMillis - startMillis >= kIdleSplashTime)
		{
			DisableMenuBar();
			DoDemoGame(g_mainWindow);
			EnableMenuBar();
		}
	}
}

//--------------------------------------------------------------  IgnoreThisClick
// An inelegant kludge designed to temporarily prevent an unwanted
// double-click to be registered.

void IgnoreThisClick (void)
{
	g_ignoreDoubleClick = true;
}
