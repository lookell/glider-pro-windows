#include "Events.h"

//============================================================================
//----------------------------------------------------------------------------
//									Events.c
//----------------------------------------------------------------------------
//============================================================================


#include "DialogUtils.h"
#include "Environ.h"
#include "Externs.h"
#include "House.h"
#include "Macintosh.h"
#include "ObjectEdit.h"
#include "ResourceIDs.h"


void HandleMouseEvent (EventRecord *);
void HandleUpdateEvent (EventRecord *);
void HandleOSEvent (EventRecord *);
void HandleHighLevelEvent (EventRecord *);
void HandleIdleTask (void);
void IncrementMode (void);


SInt32			lastUp, incrementModeTime;
UInt32			doubleTime;
Point			lastWhere;
SInt16			idleMode;
Boolean			doAutoDemo, switchedOut;
HACCEL			splashAccelTable, editAccelTable;

extern	HWND		mapWindow, toolsWindow, linkWindow;
extern	HWND		menuWindow;
extern	SInt16		isEditH, isEditV, isMapH, isMapV, isToolsH, isToolsV;
extern	SInt16		isLinkH, isLinkV, isCoordH, isCoordV;
extern	SInt16		demoHouseIndex;
extern	Boolean		quitting, isMusicOn, failedMusic;
extern	Boolean		autoRoomEdit, newRoomNow, isPlayMusicIdle;


//==============================================================  Functions
//--------------------------------------------------------------  BitchAboutColorDepth
// Display a dialog that alerts the user that they have switched the bit…
// depth of the monitor under our noses.  They must return it to previous.

SInt16 BitchAboutColorDepth (HWND ownerWindow)
{
	SInt16 sheSaid;

	sheSaid = Alert(kColorSwitchedAlert, ownerWindow, NULL);
	return (sheSaid - 1000);
}

//--------------------------------------------------------------  HandleMouseEvent
// Handle a mouse click event.

void HandleMouseEvent (EventRecord *theEvent)
{
	return;
#if 0
	WindowPtr	whichWindow;
	long		menuChoice, newSize;
	short		thePart, hDelta, vDelta;
	Boolean		isDoubleClick;

	thePart = FindWindow(theEvent->where, &whichWindow);

	switch (thePart)
	{
		case inSysWindow:
//		SystemClick(theEvent, whichWindow);
		break;

		case inMenuBar:
		menuChoice = MenuSelect(theEvent->where);
		DoMenuChoice(menuChoice);
		break;

		case inDrag:
		DragWindow(whichWindow, theEvent->where, &thisMac.screen);
		if (whichWindow == mainWindow)
		{
			SendBehind(mainWindow, (WindowPtr)0L);
			GetWindowLeftTop(whichWindow, &isEditH, &isEditV);
		}
		else if (whichWindow == mapWindow)
			GetWindowLeftTop(whichWindow, &isMapH, &isMapV);
		else if (whichWindow == toolsWindow)
			GetWindowLeftTop(whichWindow, &isToolsH, &isToolsV);
		else if (whichWindow == linkWindow)
			GetWindowLeftTop(whichWindow, &isLinkH, &isLinkV);
		else if (whichWindow == coordWindow)
			GetWindowLeftTop(whichWindow, &isCoordH, &isCoordV);
		HiliteAllWindows();
		break;

		case inGoAway:
		if (TrackGoAway(whichWindow,theEvent->where))
		{
			if (whichWindow == mapWindow)
				ToggleMapWindow();
			else if (whichWindow == toolsWindow)
				ToggleToolsWindow();
			else if (whichWindow == linkWindow)
				CloseLinkWindow();
			else if (whichWindow == coordWindow)
				ToggleCoordinateWindow();
		}
		break;

		case inGrow:
		if (whichWindow == mapWindow)
		{
			newSize = GrowWindow(mapWindow, theEvent->where, &thisMac.gray);
			ResizeMapWindow(LoWord(newSize), HiWord(newSize));
		}
		break;

		case inZoomIn:
		case inZoomOut:
		if (TrackBox(whichWindow, theEvent->where, thePart))
			ZoomWindow(whichWindow, thePart, true);
		break;

		case inContent:
		if (whichWindow == mainWindow)
		{
			hDelta = theEvent->where.h - lastWhere.h;
			if (hDelta < 0)
				hDelta = -hDelta;
			vDelta = theEvent->where.v - lastWhere.v;
			if (vDelta < 0)
				vDelta = -vDelta;
			if (((theEvent->when - lastUp) < doubleTime) && (hDelta < 5) &&
					(vDelta < 5))
				isDoubleClick = true;
			else
			{
				isDoubleClick = false;
				lastUp = theEvent->when;
				lastWhere = theEvent->where;
			}
			HandleMainClick(theEvent->where, isDoubleClick);
		}
		else if (whichWindow == mapWindow)
			HandleMapClick(theEvent);
		else if (whichWindow == toolsWindow)
			HandleToolsClick(theEvent->where);
		else if (whichWindow == linkWindow)
			HandleLinkClick(theEvent->where);
		break;

		default:
		break;
	}
#endif
}

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

//--------------------------------------------------------------  HandleUpdateEvent
// Handle an update event.

void HandleUpdateEvent (EventRecord *theEvent)
{
	return;
#if 0
	if ((WindowPtr)theEvent->message == mainWindow)
	{
		SetPort((GrafPtr)mainWindow);
		BeginUpdate(mainWindow);
		UpdateMainWindow();
		EndUpdate(mainWindow);
	}
	else if ((WindowPtr)theEvent->message == mapWindow)
	{
		SetPort((GrafPtr)mapWindow);
		BeginUpdate(mapWindow);
		UpdateMapWindow();
		EndUpdate(mapWindow);
	}
	else if ((WindowPtr)theEvent->message == toolsWindow)
	{
		SetPort((GrafPtr)toolsWindow);
		BeginUpdate(toolsWindow);
		UpdateToolsWindow();
		EndUpdate(toolsWindow);
	}
	else if ((WindowPtr)theEvent->message == linkWindow)
	{
		SetPort((GrafPtr)linkWindow);
		BeginUpdate(linkWindow);
		UpdateLinkWindow();
		EndUpdate(linkWindow);
	}
	else if ((WindowPtr)theEvent->message == coordWindow)
	{
		SetPort((GrafPtr)coordWindow);
		BeginUpdate(coordWindow);
		UpdateCoordWindow();
		EndUpdate(coordWindow);
	}
	else if ((WindowPtr)theEvent->message == menuWindow)
	{
		SetPort((GrafPtr)menuWindow);
		BeginUpdate(menuWindow);
		UpdateMenuBarWindow();
		EndUpdate(menuWindow);
	}
#endif
}

//--------------------------------------------------------------  HandleOSEvent
// Handle an OS Event (MultiFinder - user has switched in or out of app).

void HandleOSEvent (EventRecord *theEvent)
{
	return;
#if 0
	OSErr		theErr;
	short		buttonHit;

	if (theEvent->message & 0x01000000)		// suspend or resume event
	{
		if (theEvent->message & 0x00000001)	// resume event
		{
			if (WhatsOurDepth() != thisMac.isDepth)
			{
				buttonHit = BitchAboutColorDepth();
				if (buttonHit == 1)			// player wants to Quit
				{
#ifndef COMPILEDEMO
					if (QuerySaveChanges())
						quitting = true;
#else
					quitting = true;
#endif
				}
				else
				{
					SwitchToDepth(thisMac.isDepth, thisMac.wasColorOrGray);
				}
			}
			switchedOut = false;
			InitCursor();
			if ((isPlayMusicIdle) && (theMode != kEditMode))
			{
				theErr = StartMusic();
				if (theErr != noErr)
				{
					YellowAlert(kYellowNoMusic, theErr);
					failedMusic = true;
				}
			}
			incrementModeTime = TickCount() + kIdleSplashTicks;

#ifndef COMPILEDEMO
//			if (theMode == kEditMode)
//				SeeIfValidScrapAvailable(true);
#endif
		}
		else								// suspend event
		{
			switchedOut = true;
			InitCursor();
			if ((isMusicOn) && (theMode != kEditMode))
				StopTheMusic();
		}
	}
#endif
}

//--------------------------------------------------------------  HandleHighLevelEvent
// Handle an AppleEvent (Open Document, Quit Application, etc.).

void HandleHighLevelEvent (EventRecord *theEvent)
{
	return;
#if 0
	OSErr		theErr;

	theErr = AEProcessAppleEvent(theEvent);
	if ((theErr != noErr) && (theErr != errAEEventNotHandled))
		YellowAlert(kYellowAppleEventErr, theErr);
#endif
}

//--------------------------------------------------------------  HandleIdleTask
// Handle some processing during event lulls.

void HandleIdleTask (void)
{
	if (theMode == kEditMode)
	{
		//SetPort((GrafPtr)mainWindow);
		DoMarquee();

		if ((autoRoomEdit) && (newRoomNow))
		{
			if (theMode == kEditMode)
				DoRoomInfo();
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
	MSG			theEvent;
	DWORD		result, startMillis;
	DWORD		msgWaitTime, msgWaitStart, msgWaitElapsed;

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
			EraseSelectedTool();
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

	msgWaitTime = TicksToMillis(kTicksPerFrame);
	msgWaitStart = timeGetTime();
	while (1)
	{
		msgWaitElapsed = timeGetTime() - msgWaitStart;
		if (msgWaitElapsed >= msgWaitTime)
			break;
		result = MsgWaitForMultipleObjects(0, NULL, FALSE,
			msgWaitTime - msgWaitElapsed, QS_ALLINPUT);
		if (result == WAIT_OBJECT_0)
		{
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
	}
	HandleIdleTask();

	if ((theMode == kSplashMode) && doAutoDemo && !switchedOut && (demoHouseIndex >= 0))
	{
		startMillis = TicksToMillis((UInt32)incrementModeTime - kIdleSplashTicks);
		if (GetTickCount() - startMillis >= TicksToMillis(kIdleSplashTicks))
		{
			DisableMenuBar();
			DoDemoGame(mainWindow);
			EnableMenuBar();
		}
	}
}

//--------------------------------------------------------------  HiliteAllWindows

// Ugly kludge in order to keep "floating windows" (palettes) on top of…
// the main window.

void HiliteAllWindows (void)
{
	return;
#if 0
	if (mainWindow != nil)
		HiliteWindow(mainWindow, true);
	if (mapWindow != nil)
		HiliteWindow(mapWindow, true);
	if (toolsWindow != nil)
		HiliteWindow(toolsWindow, true);
	if (coordWindow != nil)
		HiliteWindow(coordWindow, true);
	if (linkWindow != nil)
		HiliteWindow(linkWindow, true);
#endif
}

//--------------------------------------------------------------  IgnoreThisClick

// Another inelegant kludge designed to temporarily prevent an unwanted…
// double-click to be registered.

void IgnoreThisClick (void)
{
	return;
#if 0
	lastUp -= doubleTime;
	lastWhere.h = -100;
	lastWhere.v = -100;
#endif
}

