//============================================================================
//----------------------------------------------------------------------------
//                                MainWindow.c
//----------------------------------------------------------------------------
//============================================================================

#include "MainWindow.h"

#include "ColorUtils.h"
#include "Coordinates.h"
#include "DialogUtils.h"
#include "Environ.h"
#include "Events.h"
#include "FrameTimer.h"
#include "GameOver.h"
#include "House.h"
#include "HouseIO.h"
#include "Input.h"
#include "Link.h"
#include "Macintosh.h"
#include "Map.h"
#include "Marquee.h"
#include "Menu.h"
#include "Music.h"
#include "ObjectEdit.h"
#include "Play.h"
#include "RectUtils.h"
#include "Render.h"
#include "ResourceIDs.h"
#include "ResourceLoader.h"
#include "Room.h"
#include "Scoreboard.h"
#include "SelectHouse.h"
#include "StringUtils.h"
#include "Tools.h"
#include "Transitions.h"
#include "Utilities.h"

#include <mmsystem.h>
#include <strsafe.h>

#include <stdlib.h>

#define WC_MAINWINDOW  TEXT("GliderMainWindow")

void PaintMainWindow (HDC hdc);
void AdjustMainWindowDC (HWND hwnd, HDC hdc);
void HandleMainClick (HWND hwnd, Point wherePt, Boolean isDoubleClick);
void SetPaletteToGrays (RGBQUAD *colors, UINT numColors, int saturation,
	int maxSaturation);
LRESULT CALLBACK MainWindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void MainWindow_OnActivateApp (HWND hwnd, BOOL fActivate);
LRESULT MainWindow_OnCommand (HWND hwnd, WPARAM wParam, LPARAM lParam);

Rect g_workSrcRect;
HDC g_workSrcMap;
HWND g_mainWindow;
SInt16 g_isEditH;
SInt16 g_isEditV;
SInt16 g_playOriginH;
SInt16 g_playOriginV;
SInt16 g_splashOriginH;
SInt16 g_splashOriginV;
SInt16 g_theMode;
Boolean g_fadeGraysOut;
Boolean g_isDoColorFade;
Boolean g_splashDrawn;
HDC g_splashSrcMap;
Rect g_splashSrcRect;

//==============================================================  Functions
//--------------------------------------------------------------  RegisterMainWindowClass

void RegisterMainWindowClass (void)
{
	WNDCLASSEX wcx = { 0 };

	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcx.lpfnWndProc = MainWindowProc;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = HINST_THISCOMPONENT;
	wcx.hIcon = LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDI_APPL));
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground = NULL;
	wcx.lpszMenuName = NULL;
	wcx.lpszClassName = WC_MAINWINDOW;
	wcx.hIconSm = NULL;
	RegisterClassEx(&wcx);
}

//--------------------------------------------------------------  UnregisterMainWindowClass

void UnregisterMainWindowClass (void)
{
	UnregisterClass(WC_MAINWINDOW, HINST_THISCOMPONENT);
}

//--------------------------------------------------------------  RedrawSplashScreen

void RedrawSplashScreen (void)
{
	Rect tempRect;

	Mac_PaintRect(g_workSrcMap, &g_workSrcRect, GetStockBrush(BLACK_BRUSH));
	tempRect = g_splashSrcRect;
	ZeroRectCorner(&tempRect);
	QOffsetRect(&tempRect, g_splashOriginH, g_splashOriginV);
	Mac_CopyBits(g_splashSrcMap, g_workSrcMap, &g_splashSrcRect, &tempRect, srcCopy, nil);
	DissolveScreenOn(&g_workSrcRect);
	CopyRectMainToWork(&g_workSrcRect);
}

//--------------------------------------------------------------  PaintMainWindow

void PaintMainWindow (HDC hdc)
{
	RECT clientRect;
	RECT unpaintedRect;
	HRGN unpaintedRgn;
	HRGN justPaintedRgn;
	Rect tempRect;

	GetClientRect(g_mainWindow, &clientRect);
	unpaintedRect = clientRect;
	DPtoLP(hdc, (POINT *)&unpaintedRect, 2);
	unpaintedRgn = CreateRectRgnIndirect(&unpaintedRect);
	justPaintedRgn = CreateRectRgn(0, 0, 0, 0);

	if (g_theMode == kEditMode)
	{
		tempRect.left = (SInt16)clientRect.left;
		tempRect.top = (SInt16)clientRect.top;
		tempRect.right = (SInt16)clientRect.right;
		tempRect.bottom = (SInt16)clientRect.bottom;
		PauseMarquee();
		Mac_CopyBits(g_workSrcMap, hdc, &tempRect, &tempRect, srcCopy, nil);
		ResumeMarquee();

		SetRectRgn(
			justPaintedRgn,
			clientRect.left,
			clientRect.top,
			clientRect.right,
			clientRect.bottom
		);
		CombineRgn(unpaintedRgn, unpaintedRgn, justPaintedRgn, RGN_DIFF);
	}
	else if (g_theMode == kPlayMode)
	{
		Mac_CopyBits(g_workSrcMap, hdc, &g_justRoomsRect, &g_justRoomsRect, srcCopy, nil);
		RefreshScoreboard(kNormalTitleMode);

		SetRectRgn(
			justPaintedRgn,
			g_justRoomsRect.left,
			g_justRoomsRect.top,
			g_justRoomsRect.right,
			g_justRoomsRect.bottom
		);
		CombineRgn(unpaintedRgn, unpaintedRgn, justPaintedRgn, RGN_DIFF);
		SetRectRgn(
			justPaintedRgn,
			g_boardDestRect.left,
			g_boardDestRect.top,
			g_boardDestRect.right,
			g_boardDestRect.bottom
		);
		CombineRgn(unpaintedRgn, unpaintedRgn, justPaintedRgn, RGN_DIFF);
	}
	else if (g_theMode == kSplashMode)
	{
		Mac_PaintRect(g_workSrcMap, &g_workSrcRect, GetStockBrush(BLACK_BRUSH));
		tempRect = g_splashSrcRect;
		ZeroRectCorner(&tempRect);
		QOffsetRect(&tempRect, g_splashOriginH, g_splashOriginV);
		Mac_CopyBits(g_splashSrcMap, g_workSrcMap, &g_splashSrcRect, &tempRect, srcCopy, nil);
		Mac_CopyBits(g_workSrcMap, hdc, &g_workSrcRect, &g_workSrcRect, srcCopy, nil);

		SetRectRgn(
			justPaintedRgn,
			g_workSrcRect.left,
			g_workSrcRect.top,
			g_workSrcRect.right,
			g_workSrcRect.bottom
		);
		CombineRgn(unpaintedRgn, unpaintedRgn, justPaintedRgn, RGN_DIFF);
	}

	FillRgn(hdc, unpaintedRgn, GetStockBrush(BLACK_BRUSH));
	DeleteRgn(justPaintedRgn);
	DeleteRgn(unpaintedRgn);

	g_splashDrawn = true;
}

//--------------------------------------------------------------  AdjustMainWindowDC

void AdjustMainWindowDC (HWND hwnd, HDC hdc)
{
	if (GetMenu(hwnd) == NULL)
	{
		SetWindowOrgEx(hdc, 0, -kScoreboardTall, NULL);
	}
}

//--------------------------------------------------------------  OpenMainWindow
// Opens up the main window (how it does this depends on mode were in).

void OpenMainWindow (void)
{
	PWSTR windowTitleBuffer;
	PCWSTR windowTitle;
	SInt16 whichRoom;
	RECT rcClient;
	LONG width, height;
	WINDOWPLACEMENT placement;
	DWORD windowStyle;
	Rect tempRect;

	if (g_mainWindow != NULL)
	{
		YellowAlert(g_mainWindow, kYellowUnaccounted, 6);
		return;
	}

	AllocLoadString(HINST_THISCOMPONENT, IDS_APPLICATION_TITLE, &windowTitleBuffer);
	windowTitle = (windowTitleBuffer != NULL) ? windowTitleBuffer : L"";

	if (g_theMode == kEditMode)
	{
		SetRect(&rcClient, 0, 0, kRoomWide, kTileHigh);
		windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		AdjustWindowRect(&rcClient, windowStyle, TRUE);
		width = rcClient.right - rcClient.left;
		height = rcClient.bottom - rcClient.top;
		g_mainWindow = CreateWindow(WC_MAINWINDOW, windowTitle,
				windowStyle, 0, 0, width, height,
				NULL, g_theMenuBar, HINST_THISCOMPONENT, NULL);
		if (g_mainWindow == NULL)
			RedAlert(kErrDialogDidntLoad);

		if (OptionKeyDown())
		{
			g_isEditH = 3;
			g_isEditV = 41;
		}

		placement.length = sizeof(placement);
		GetWindowPlacement(g_mainWindow, &placement);
		OffsetRect(&placement.rcNormalPosition,
				-placement.rcNormalPosition.left,
				-placement.rcNormalPosition.top);
		OffsetRect(&placement.rcNormalPosition, g_isEditH, g_isEditV);
		placement.showCmd = SW_SHOWNORMAL;
		SetWindowPlacement(g_mainWindow, &placement);

		whichRoom = GetFirstRoomNumber();
		CopyRoomToThisRoom(whichRoom);
		ReflectCurrentRoom(false);
	}
	else
	{
		rcClient.left = 0;
		rcClient.top = 0;
		rcClient.right = RectWide(&g_thisMac.screen);
		rcClient.bottom = RectTall(&g_thisMac.screen);
		windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		AdjustWindowRect(&rcClient, windowStyle, FALSE);
		width = rcClient.right - rcClient.left;
		height = rcClient.bottom - rcClient.top;
		g_mainWindow = CreateWindow(WC_MAINWINDOW, windowTitle,
				windowStyle, CW_USEDEFAULT, CW_USEDEFAULT, width, height,
				NULL, g_theMenuBar, HINST_THISCOMPONENT, NULL);
		if (g_mainWindow == NULL)
			RedAlert(kErrDialogDidntLoad);
		CenterWindowOverOwner(g_mainWindow);
		ShowWindow(g_mainWindow, SW_SHOWNORMAL);

		{
			RECT clientRect;
			HDC clientHDC;

			GetClientRect(g_mainWindow, &clientRect);
			clientHDC = GetDC(g_mainWindow);
			PatBlt(clientHDC, 0, 0, clientRect.right, clientRect.bottom, BLACKNESS);
			ReleaseDC(g_mainWindow, clientHDC);
		}

		g_splashOriginH = (RectWide(&g_workSrcRect) - RectWide(&g_splashSrcRect)) / 2;
		if (g_splashOriginH < 0)
			g_splashOriginH = 0;
		g_splashOriginV = (RectTall(&g_workSrcRect) - RectTall(&g_splashSrcRect)) / 2;
		if (g_splashOriginV < 0)
			g_splashOriginV = 0;

		Mac_PaintRect(g_workSrcMap, &g_workSrcRect, GetStockBrush(BLACK_BRUSH));
		tempRect = g_splashSrcRect;
		ZeroRectCorner(&tempRect);
		Mac_CopyBits(g_splashSrcMap, g_workSrcMap, &g_splashSrcRect, &tempRect, srcCopy, nil);

		if ((g_fadeGraysOut) && (g_isDoColorFade))
		{
			g_fadeGraysOut = false;
		}
	}

	free(windowTitleBuffer);
}

//--------------------------------------------------------------  CloseMainWindow
// Closes the main window.

void CloseMainWindow (void)
{
	if (g_mainWindow != NULL)
		DestroyWindow(g_mainWindow);
	g_mainWindow = NULL;
}

//--------------------------------------------------------------  UpdateEditWindowTitle
// Handles changing the title across the top of the main window.  Only
// relevant when editing a house (room title displayed in window title).

void UpdateEditWindowTitle (void)
{
	wchar_t houseName[64];
	wchar_t roomName[32];
	wchar_t newTitle[256];

	if (g_mainWindow == NULL)
	{
		return;
	}

	WinFromMacString(houseName, ARRAYSIZE(houseName), g_thisHouseName);
	if (g_noRoomAtAll)
	{
		StringCchPrintf(newTitle, ARRAYSIZE(newTitle), L"%s - No rooms", houseName);
	}
	else if (g_houseUnlocked)
	{
		WinFromMacString(roomName, ARRAYSIZE(roomName), g_thisRoom->name);
		StringCchPrintf(newTitle, ARRAYSIZE(newTitle), L"%s - %s (%d, %d)",
			houseName, roomName, (int)g_thisRoom->floor, (int)g_thisRoom->suite);
	}
	else
	{
		StringCchPrintf(newTitle, ARRAYSIZE(newTitle), L"%s - House Locked", houseName);
	}
	SetWindowText(g_mainWindow, newTitle);
}

//--------------------------------------------------------------  HandleMainClick
// Handle a mouse click in the main window (relevant only when editing).

void HandleMainClick (HWND hwnd, Point wherePt, Boolean isDoubleClick)
{
	if ((g_theMode != kEditMode) || (g_mainWindow == NULL) || (!g_houseUnlocked))
		return;

	if (g_toolSelected == kSelectTool)
		DoSelectionClick(hwnd, wherePt, isDoubleClick);
	else
		DoNewObjectClick(hwnd, wherePt);

	if (GetKeyState(VK_SHIFT) >= 0) // if shift key up
	{
		SelectTool(kSelectTool);
	}
}

//--------------------------------------------------------------  GetMainWindowDC

HDC GetMainWindowDC (HWND hwnd)
{
	HDC hdc;

	if (hwnd != NULL)
	{
		hdc = GetDC(hwnd);
		AdjustMainWindowDC(hwnd, hdc);
		return hdc;
	}
	else
	{
		return NULL;
	}
}

//--------------------------------------------------------------  ReleaseMainWindowDC

void ReleaseMainWindowDC (HWND hwnd, HDC hdc)
{
	if (hdc != NULL)
	{
		ReleaseDC(hwnd, hdc);
	}
}

//--------------------------------------------------------------  SetPaletteToGrays
// Sets up a gray palette corresponding in luminance to the given color
// palette. This is to facilitate the gray->color fade when the game comes up.
// The ratio given by the 'numer' and 'denom' parameters is the satuaration
// of the resulting palette, ranging from 0 (full gray) to 1 (original color).

void SetPaletteToGrays (RGBQUAD *colors, UINT numColors, int saturation,
	int maxSaturation)
{
	// The magic wand's color is kept the same.
	const COLORREF magicWandColor = RGB(0xFF, 0xFF, 0x00);
	int grayscale, maxGrayscale;
	int red, green, blue, gray;
	COLORREF thisColor;
	UINT i;

	grayscale = maxSaturation - saturation;
	maxGrayscale = maxSaturation;

	for (i = 0; i < numColors; i++)
	{
		red = colors[i].rgbRed;
		green = colors[i].rgbGreen;
		blue = colors[i].rgbBlue;
		thisColor = RGB(red, green, blue);
		if (thisColor != magicWandColor)
		{
			gray = ((red * 3) + (green * 6) + (blue * 1)) / 10;
			red -= MulDiv(red - gray, grayscale, maxGrayscale);
			green -= MulDiv(green - gray, grayscale, maxGrayscale);
			blue -= MulDiv(blue - gray, grayscale, maxGrayscale);
		}
		colors[i].rgbRed = (BYTE)red;
		colors[i].rgbGreen = (BYTE)green;
		colors[i].rgbBlue = (BYTE)blue;
	}
}

//--------------------------------------------------------------  WashColorIn
// Slowly walks the palette from its gray luminance state to the full color
// palette.  In this way, color appears to slowly wash in.

void WashColorIn (void)
{
	const int kGray2ColorSteps = 180;
	MMRESULT mmResult;
	RGBQUAD wasColors[256];
	RGBQUAD newColors[256];
	UINT numColors;
	HBITMAP splashDIB;
	HDC splashDC;
	HDC hdc;
	RECT clientRect;
	BOOL fading;
	MSG msg;
	DWORD wasFPS;
	int i;
	int c;

	mmResult = timeBeginPeriod(1);

	DisableMenuBar();

	splashDIB = Gp_LoadBuiltInImageAsDIB(kSplash8BitPICT);
	if (splashDIB == NULL)
		RedAlert(kErrFailedGraphicLoad);

	GetClientRect(g_mainWindow, &clientRect);

	splashDC = CreateCompatibleDC(NULL);
	SaveDC(splashDC);
	SelectBitmap(splashDC, splashDIB);

	numColors = GetDIBColorTable(splashDC, 0, ARRAYSIZE(wasColors), wasColors);
	if (numColors != ARRAYSIZE(wasColors))
		RedAlert(kErrUnnaccounted);

	wasFPS = GetFrameRate();
	SetFrameRate(60);

	fading = TRUE;
	for (i = 0; fading && (i < kGray2ColorSteps); i++)
	{
		for (c = 0; c < ARRAYSIZE(newColors); c++)
		{
			newColors[c] = wasColors[c];
		}
		SetPaletteToGrays(newColors, ARRAYSIZE(newColors), i, kGray2ColorSteps);
		SetDIBColorTable(splashDC, 0, ARRAYSIZE(newColors), newColors);

		hdc = GetMainWindowDC(g_mainWindow);
		BitBlt(hdc, g_splashOriginH, g_splashOriginV, 640, 460, splashDC, 0, 0, SRCCOPY);
		ReleaseMainWindowDC(g_mainWindow, hdc);
		ValidateRect(g_mainWindow, NULL);

		while (PeekMessageOrWaitForFrame(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				PostQuitMessage((int)msg.wParam);
				fading = FALSE;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			switch (msg.message)
			{
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
			case WM_LBUTTONDOWN:
				fading = FALSE;
				break;
			}
		}
	}

	SetFrameRate(wasFPS);
	RestoreDC(splashDC, -1);
	DeleteDC(splashDC);
	DeleteBitmap(splashDIB);
	EnableMenuBar();
	InvalidateRect(g_mainWindow, NULL, TRUE);

	if (mmResult == TIMERR_NOERROR)
	{
		timeEndPeriod(1);
	}
}

//--------------------------------------------------------------  MainWindowProc

LRESULT CALLBACK MainWindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_ACTIVATEAPP:
		MainWindow_OnActivateApp(hwnd, !!wParam);
		return 0;

	case WM_CLOSE:
		if (g_theMode == kPlayMode && g_playing && !g_gameOver)
		{
			DoCommandKeyQuit(hwnd);
		}
		DoMenuChoice(hwnd, ID_QUIT);
		return 0;

	case WM_COMMAND:
		return MainWindow_OnCommand(hwnd, wParam, lParam);

	case WM_DESTROY:
		// Remove the menu bar from the window so that it isn't destroyed
		// with the window. It will be reattached to the main window if
		// it is recreated.
		SetMenu(hwnd, NULL);
		return 0;

	case WM_ENABLE:
		if (g_mapWindow != NULL && IsWindow(g_mapWindow))
		{
			EnableWindow(g_mapWindow, !!wParam);
		}
		if (g_toolsWindow != NULL && IsWindow(g_toolsWindow))
		{
			EnableWindow(g_toolsWindow, !!wParam);
		}
		if (g_linkWindow != NULL && IsWindow(g_linkWindow))
		{
			EnableWindow(g_linkWindow, !!wParam);
		}
		if (g_coordWindow != NULL && IsWindow(g_coordWindow))
		{
			EnableWindow(g_coordWindow, !!wParam);
		}
		return 0;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	{
		Point wherePt;
		wherePt.h = GET_X_LPARAM(lParam);
		wherePt.v = GET_Y_LPARAM(lParam);
		if (uMsg == WM_LBUTTONDBLCLK && !g_ignoreDoubleClick)
		{
			HandleMainClick(hwnd, wherePt, true);
		}
		else
		{
			HandleMainClick(hwnd, wherePt, false);
		}
		g_ignoreDoubleClick = false;
		return 0;
	}

	case WM_MOVE:
		if (g_theMode == kEditMode)
		{
			WINDOWPLACEMENT placement;
			placement.length = sizeof(placement);
			GetWindowPlacement(hwnd, &placement);
			g_isEditH = (SInt16)placement.rcNormalPosition.left;
			g_isEditV = (SInt16)placement.rcNormalPosition.top;
		}
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		if (BeginPaint(hwnd, &ps))
		{
			AdjustMainWindowDC(hwnd, ps.hdc);
			PaintMainWindow(ps.hdc);
			EndPaint(hwnd, &ps);
		}
		return 0;
	}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

//--------------------------------------------------------------  MainWindow_OnActivateApp

void MainWindow_OnActivateApp (HWND hwnd, BOOL fActivate)
{
	if (g_theMode == kPlayMode)
	{
		if (fActivate)
		{
			g_switchedOut = false;
			if (g_isPlayMusicGame && !g_isMusicOn)
				StartMusic();
		}
		else
		{
			g_switchedOut = true;
			if (g_isPlayMusicGame && g_isMusicOn)
				StopTheMusic();
		}
	}
	else
	{
		if (fActivate)
		{
			g_switchedOut = false;
			if ((g_isPlayMusicIdle) && (g_theMode != kEditMode))
			{
				OSErr theErr = StartMusic();
				if (theErr != noErr)
				{
					YellowAlert(hwnd, kYellowNoMusic, theErr);
					g_failedMusic = true;
				}
			}
			g_incrementModeTime = timeGetTime() + kIdleSplashTime;

			if (!COMPILEDEMO)
			{
#if 0
				if (g_theMode == kEditMode)
					SeeIfValidScrapAvailable(true);
#endif
			}
		}
		else
		{
			g_switchedOut = true;
			if ((g_isMusicOn) && (g_theMode != kEditMode))
				StopTheMusic();
		}
	}
}

//--------------------------------------------------------------  MainWindow_OnCommand

LRESULT MainWindow_OnCommand (HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	WORD command;

	(void)lParam;

	command = LOWORD(wParam);
	switch (command)
	{
	case ID_EDIT_PREV_TOOL_MODE:
		if (g_houseUnlocked)
			PrevToolMode();
		break;

	case ID_EDIT_NEXT_TOOL_MODE:
		if (g_houseUnlocked)
			NextToolMode();
		break;

	case ID_EDIT_MOVE_THIS_LEFT:
	case ID_EDIT_MOVE_THIS_LEFT_LARGE:
		if (g_houseUnlocked)
		{
			if (g_objActive == kNoObjectSelected)
				SelectNeighborRoom(kRoomToLeft);
			else
				MoveObject(kBumpLeft, (command == ID_EDIT_MOVE_THIS_LEFT_LARGE));
		}
		break;

	case ID_EDIT_MOVE_THIS_RIGHT:
	case ID_EDIT_MOVE_THIS_RIGHT_LARGE:
		if (g_houseUnlocked)
		{
			if (g_objActive == kNoObjectSelected)
				SelectNeighborRoom(kRoomToRight);
			else
				MoveObject(kBumpRight, (command == ID_EDIT_MOVE_THIS_RIGHT_LARGE));
		}
		break;

	case ID_EDIT_MOVE_THIS_UP:
	case ID_EDIT_MOVE_THIS_UP_LARGE:
		if (g_houseUnlocked)
		{
			if (g_objActive == kNoObjectSelected)
				SelectNeighborRoom(kRoomAbove);
			else
				MoveObject(kBumpUp, (command == ID_EDIT_MOVE_THIS_UP_LARGE));
		}
		break;

	case ID_EDIT_MOVE_THIS_DOWN:
	case ID_EDIT_MOVE_THIS_DOWN_LARGE:
		if (g_houseUnlocked)
		{
			if (g_objActive == kNoObjectSelected)
				SelectNeighborRoom(kRoomBelow);
			else
				MoveObject(kBumpDown, (command == ID_EDIT_MOVE_THIS_DOWN_LARGE));
		}
		break;

	case ID_EDIT_DELETE_THIS:
		if (g_houseUnlocked)
		{
			if (g_objActive == kNoObjectSelected)
				DeleteRoom(hwnd, true);
			else
				Gp_DeleteObject();
		}
		break;

	case ID_EDIT_SELECT_NEXT_OBJECT:
		if ((g_theMode == kEditMode) && (g_houseUnlocked))
			SelectNextObject();
		break;

	case ID_EDIT_SELECT_PREV_OBJECT:
		if ((g_theMode == kEditMode) && (g_houseUnlocked))
			SelectPrevObject();
		break;

	case ID_EDIT_DESELECT_OBJECT:
		if ((g_theMode == kEditMode) && (g_houseUnlocked))
			DeselectObject();
		break;

	case ID_EDIT_TOOL_MODE_APPLIANCE:
		if ((g_theMode == kEditMode) && (g_houseUnlocked))
			SetSpecificToolMode(kApplianceMode);
		break;

	case ID_EDIT_TOOL_MODE_BLOWER:
		if ((g_theMode == kEditMode) && (g_houseUnlocked))
			SetSpecificToolMode(kBlowerMode);
		break;

	case ID_EDIT_TOOL_MODE_CLUTTER:
		if ((g_theMode == kEditMode) && (g_houseUnlocked))
			SetSpecificToolMode(kClutterMode);
		break;

	case ID_EDIT_TOOL_MODE_ENEMY:
		if ((g_theMode == kEditMode) && (g_houseUnlocked))
			SetSpecificToolMode(kEnemyMode);
		break;

	case ID_EDIT_TOOL_MODE_FURNITURE:
		if ((g_theMode == kEditMode) && (g_houseUnlocked))
			SetSpecificToolMode(kFurnitureMode);
		break;

	case ID_EDIT_TOOL_MODE_LIGHT:
		if ((g_theMode == kEditMode) && (g_houseUnlocked))
			SetSpecificToolMode(kLightMode);
		break;

	case ID_EDIT_TOOL_MODE_PRIZE:
		if ((g_theMode == kEditMode) && (g_houseUnlocked))
			SetSpecificToolMode(kBonusMode);
		break;

	case ID_EDIT_TOOL_MODE_SWITCH:
		if ((g_theMode == kEditMode) && (g_houseUnlocked))
			SetSpecificToolMode(kSwitchMode);
		break;

	case ID_EDIT_TOOL_MODE_TRANSPORT:
		if ((g_theMode == kEditMode) && (g_houseUnlocked))
			SetSpecificToolMode(kTransportMode);
		break;

	default:
		DoMenuChoice(hwnd, command);
		break;
	}

	return 0;
}
