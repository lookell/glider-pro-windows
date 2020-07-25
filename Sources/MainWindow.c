#include "MainWindow.h"

//============================================================================
//----------------------------------------------------------------------------
//                                MainWindow.c
//----------------------------------------------------------------------------
//============================================================================


#include "AnimCursor.h"
#include "ColorUtils.h"
#include "Coordinates.h"
#include "DialogUtils.h"
#include "Environ.h"
#include "Events.h"
#include "FrameTimer.h"
#include "House.h"
#include "HouseIO.h"
#include "Link.h"
#include "Macintosh.h"
#include "Main.h"
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
#include "StringUtils.h"
#include "Tools.h"
#include "Transitions.h"
#include "Utilities.h"


void DrawOnSplash (HDC hdc);
void SetPaletteToGrays (RGBQUAD *colors, UINT numColors, int saturation,
	int maxSaturation);
void MainWindow_OnActivateApp (HWND hwnd, BOOL fActivate);


HCURSOR handCursor;
HCURSOR vertCursor;
HCURSOR horiCursor;
HCURSOR diagBotCursor;
HCURSOR diagTopCursor;
Rect workSrcRect;
HDC workSrcMap;
Rect mainWindowRect;
HWND mainWindow;
SInt16 isEditH;
SInt16 isEditV;
SInt16 playOriginH;
SInt16 playOriginV;
SInt16 splashOriginH;
SInt16 splashOriginV;
SInt16 theMode;
Boolean fadeGraysOut;
Boolean isDoColorFade;
Boolean splashDrawn;

static HCURSOR mainWindowCursor;


//==============================================================  Functions
//--------------------------------------------------------------  DrawOnSplash

// Draws additional text on top of splash screen.

void DrawOnSplash (HDC hdc)
{
	Str255		houseLoadedStr, tempStr;
	LOGFONT		lfHouse, lfNative;
	HGDIOBJ		theFont, wasFont;
	COLORREF	wasTextColor;

	lfHouse.lfHeight = -9;
	lfHouse.lfWidth = 0;
	lfHouse.lfEscapement = 0;
	lfHouse.lfOrientation = 0;
	lfHouse.lfWeight = FW_BOLD;
	lfHouse.lfItalic = FALSE;
	lfHouse.lfUnderline = FALSE;
	lfHouse.lfStrikeOut = FALSE;
	lfHouse.lfCharSet = DEFAULT_CHARSET;
	lfHouse.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lfHouse.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lfHouse.lfQuality = DEFAULT_QUALITY;
	lfHouse.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	StringCchCopy(lfHouse.lfFaceName, ARRAYSIZE(lfHouse.lfFaceName), L"Tahoma");

	lfNative = lfHouse;
	lfNative.lfHeight = -12;
	lfNative.lfWeight = FW_BOLD;
	StringCchCopy(lfNative.lfFaceName, ARRAYSIZE(lfNative.lfFaceName), L"Tahoma");

	PasStringCopyC("House: ", houseLoadedStr);
	PasStringConcat(houseLoadedStr, thisHouseName);
	if ((thisMac.hasQT) && (hasMovie))
		PasStringConcatC(houseLoadedStr, " (QT)");
	MoveToEx(hdc, splashOriginH + 436, splashOriginV + 314, NULL);
	theFont = CreateFontIndirect(&lfHouse);
	wasFont = SelectObject(hdc, theFont);
	if (houseIsReadOnly)
		ColorText(hdc, houseLoadedStr, 5L);
	else
		ColorText(hdc, houseLoadedStr, 28L);
	SelectObject(hdc, wasFont);
	DeleteObject(theFont);

	#if defined(powerc) || defined(__powerc) // TODO: Change message to "Windows Native!"?
	theFont = CreateFontIndirect(&lfNative);
	wasFont = SelectObject(hdc, theFont);
	PasStringCopyC("PowerPC Native!", tempStr);
	wasTextColor = SetTextColor(hdc, blackColor);
	MoveToEx(hdc, splashOriginH + 5, splashOriginV + 457, NULL);
	Mac_DrawString(hdc, tempStr);
	SetTextColor(hdc, whiteColor);
	MoveToEx(hdc, splashOriginH + 4, splashOriginV + 456, NULL);
	Mac_DrawString(hdc, tempStr);
	SetTextColor(hdc, wasTextColor);
	SelectObject(hdc, wasFont);
	DeleteObject(theFont);
	#else
	UNREFERENCED_PARAMETER(tempStr);
	#endif
}

//--------------------------------------------------------------  RedrawSplashScreen

void RedrawSplashScreen (void)
{
	Rect		tempRect;

	Mac_PaintRect(workSrcMap, &workSrcRect, GetStockObject(BLACK_BRUSH));
	QSetRect(&tempRect, 0, 0, 640, 460);
	QOffsetRect(&tempRect, splashOriginH, splashOriginV);
	LoadScaledGraphic(workSrcMap, kSplash8BitPICT, &tempRect);
	DrawOnSplash(workSrcMap);
	if (quickerTransitions)
		DissBitsChunky(&workSrcRect);
	else
		DissBits(&workSrcRect);
	CopyRectMainToWork(&workSrcRect);
}

//--------------------------------------------------------------  UpdateMainWindow

// Redraws the main window (depends on mode were in - splash, editing, playing).

void UpdateMainWindow (void)
{
	Rect		tempRect;
	HDC			mainWindowDC;

	mainWindowDC = GetMainWindowDC();

	if (theMode == kEditMode)
	{
		PauseMarquee();
		Mac_CopyBits(workSrcMap, mainWindowDC,
				&mainWindowRect, &mainWindowRect, srcCopy, nil);
		ResumeMarquee();
	}
	else if (theMode == kPlayMode)
	{
		Mac_CopyBits(workSrcMap, mainWindowDC,
				&justRoomsRect, &justRoomsRect, srcCopy, nil);
		RefreshScoreboard(kNormalTitleMode);
	}
	else if (theMode == kSplashMode)
	{
		Mac_PaintRect(workSrcMap, &workSrcRect, GetStockObject(BLACK_BRUSH));
		QSetRect(&tempRect, 0, 0, 640, 460);
		QOffsetRect(&tempRect, splashOriginH, splashOriginV);
		LoadScaledGraphic(workSrcMap, kSplash8BitPICT, &tempRect);
		Mac_CopyBits(workSrcMap, mainWindowDC,
				&workSrcRect, &mainWindowRect, srcCopy, nil);

		DrawOnSplash(mainWindowDC);
	}

	ReleaseMainWindowDC(mainWindowDC);
	splashDrawn = true;
}

//--------------------------------------------------------------  OpenMainWindow
// Opens up the main window (how it does this depends on mode were in).

void OpenMainWindow (void)
{
	SInt16 whichRoom;
	RECT rcClient;
	LONG width, height;
	SInt16 workspaceX, workspaceY;
	WINDOWPLACEMENT placement;
	DWORD windowStyle;
	HDC mainWindowDC;

	if (mainWindow != NULL)
	{
		YellowAlert(mainWindow, kYellowUnaccounted, 6);
		return;
	}

	if (theMode == kEditMode)
	{
		// These assignments must happen before the CreateWindow call, or else the
		// WM_MOVE message handler overwrites 'isEditH' and 'isEditV' during the
		// main window's creation.
		workspaceX = isEditH;
		workspaceY = isEditV;

		QSetRect(&mainWindowRect, 0, 0, 512, 322);
		SetRect(&rcClient, 0, 0, 512, 322);
		windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		AdjustWindowRect(&rcClient, windowStyle, TRUE);
		width = rcClient.right - rcClient.left;
		height = rcClient.bottom - rcClient.top;
		mainWindow = CreateWindow(WC_MAINWINDOW, L"Main Window",
				windowStyle, 0, 0, width, height,
				NULL, theMenuBar, HINST_THISCOMPONENT, NULL);
		if (mainWindow == NULL)
			RedAlert(kErrDialogDidntLoad);

		if (OptionKeyDown())
		{
			isEditH = 3;
			isEditV = 41;
		}
		else
		{
			isEditH = workspaceX;
			isEditV = workspaceY;
		}

		placement.length = sizeof(placement);
		GetWindowPlacement(mainWindow, &placement);
		OffsetRect(&placement.rcNormalPosition,
				-placement.rcNormalPosition.left,
				-placement.rcNormalPosition.top);
		OffsetRect(&placement.rcNormalPosition, isEditH, isEditV);
		placement.showCmd = SW_SHOWNORMAL;
		SetWindowPlacement(mainWindow, &placement);

		whichRoom = GetFirstRoomNumber();
		CopyRoomToThisRoom(whichRoom);
		ReflectCurrentRoom(false);
	}
	else
	{
		mainWindowRect = thisMac.screen;
		ZeroRectCorner(&mainWindowRect);
		mainWindowRect.bottom -= 20;		// thisMac.menuHigh
		windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		SetRect(&rcClient, 0, 0, mainWindowRect.right, mainWindowRect.bottom);
		AdjustWindowRect(&rcClient, windowStyle, TRUE);
		width = rcClient.right - rcClient.left;
		height = rcClient.bottom - rcClient.top;
		mainWindow = CreateWindow(WC_MAINWINDOW, L"Main Window",
				windowStyle, CW_USEDEFAULT, CW_USEDEFAULT, width, height,
				NULL, theMenuBar, HINST_THISCOMPONENT, NULL);
		if (mainWindow == NULL)
			RedAlert(kErrDialogDidntLoad);
		CenterOverOwner(mainWindow);
		ShowWindow(mainWindow, SW_SHOWNORMAL);

		mainWindowDC = GetMainWindowDC();
		Mac_PaintRect(mainWindowDC, &mainWindowRect, GetStockObject(BLACK_BRUSH));
		ReleaseMainWindowDC(mainWindowDC);

		splashOriginH = ((thisMac.screen.right - thisMac.screen.left) - 640) / 2;
		if (splashOriginH < 0)
			splashOriginH = 0;
		splashOriginV = ((thisMac.screen.bottom - thisMac.screen.top) - 480) / 2;
		if (splashOriginV < 0)
			splashOriginV = 0;

		Mac_PaintRect(workSrcMap, &workSrcRect, GetStockObject(BLACK_BRUSH));
		LoadGraphic(workSrcMap, kSplash8BitPICT);

		if ((fadeGraysOut) && (isDoColorFade))
		{
			fadeGraysOut = false;
		}
	}
}

//--------------------------------------------------------------  CloseMainWindow

// Closes the main window.

void CloseMainWindow (void)
{
	if (mainWindow != NULL)
		DestroyWindow(mainWindow);
	mainWindow = NULL;
}

//--------------------------------------------------------------  UpdateEditWindowTitle
// Handles changing the title across the top of the main window.  Only…
// relevant when editing a house (room title displayed in window title).

#ifndef COMPILEDEMO
void UpdateEditWindowTitle (void)
{
	Str255		newTitle, tempStr;
	WCHAR		windowText[256];

	if (mainWindow == NULL)
		return;

	PasStringCopy(thisHouseName, newTitle);
	PasStringConcatC(newTitle, " - ");
	if (noRoomAtAll)
		PasStringConcatC(newTitle, "No rooms");
	else if (houseUnlocked)
	{
		PasStringConcat(newTitle, thisRoom->name);
		PasStringConcatC(newTitle, " (");
		NumToString((SInt32)thisRoom->floor, tempStr);
		PasStringConcat(newTitle, tempStr);
		PasStringConcatC(newTitle, ", ");
		NumToString((SInt32)thisRoom->suite, tempStr);
		PasStringConcat(newTitle, tempStr);
		PasStringConcatC(newTitle, ")");
	}
	else
		PasStringConcatC(newTitle, "House Locked");
	WinFromMacString(windowText, ARRAYSIZE(windowText), newTitle);
	SetWindowText(mainWindow, windowText);
}
#endif

//--------------------------------------------------------------  HandleMainClick

// Handle a mouse click in the main window (relevant only when editing).

void HandleMainClick (HWND hwnd, Point wherePt, Boolean isDoubleClick)
{
	if ((theMode != kEditMode) || (mainWindow == NULL) || (!houseUnlocked))
		return;

	if (toolSelected == kSelectTool)
		DoSelectionClick(hwnd, wherePt, isDoubleClick);
	else
		DoNewObjectClick(hwnd, wherePt);

	if (GetKeyState(VK_SHIFT) >= 0) // if shift key up
	{
		SelectTool(kSelectTool);
	}
}

//--------------------------------------------------------------  GetMainWindowDC

HDC GetMainWindowDC (void)
{
	HDC hdc;

	if (mainWindow != NULL)
		hdc = GetDC(mainWindow);
	else
		return NULL;

	if (GetMenu(mainWindow) == NULL)
		SetWindowOrgEx(hdc, 0, -kScoreboardTall, NULL);

	return hdc;
}

//--------------------------------------------------------------  ReleaseMainWindowDC

void ReleaseMainWindowDC (HDC hdc)
{
	if (hdc != NULL)
		ReleaseDC(mainWindow, hdc);
}

//--------------------------------------------------------------  ShowMenuBarOld
// Displays the menu bar (after having been hidden).
/*
void ShowMenuBarOld (void)
{
	Rect			theRect;
	GrafPtr			wasPort, tempPort;
	RgnHandle		worldRgn, menuBarRgn;

	if (LMGetMBarHeight() == 0)
	{
		GetPort(&wasPort);
		tempPort = (GrafPtr)NewPtrClear(sizeof(GrafPort));
		OpenPort(tempPort);
		SetPort((GrafPtr)tempPort);

		LMSetMBarHeight(thisMac.menuHigh);

		theRect = (**GetGrayRgn()).rgnBBox;
		UnionRect(&theRect, &qd.screenBits.bounds, &theRect);
		worldRgn = NewRgn();
		OpenRgn();
		FrameRoundRect(&theRect, 16, 16);
		CloseRgn(worldRgn);

		theRect = qd.screenBits.bounds;
		theRect.bottom = theRect.top + thisMac.menuHigh;
		menuBarRgn = NewRgn();
		RectRgn(menuBarRgn, &theRect);

		SectRgn(worldRgn, menuBarRgn, menuBarRgn);	//	/------------------\
		DisposeRgn(worldRgn);						//	|__________________|

		UnionRgn(tempPort->visRgn, menuBarRgn, tempPort->visRgn);
		DiffRgn(tempPort->visRgn, menuBarRgn, tempPort->visRgn);
		DisposeRgn(menuBarRgn);

		ClosePort(tempPort);
		SetPort((GrafPtr)wasPort);

		DrawMenuBar();
	}
}
*/
//--------------------------------------------------------------  HideMenuBarOld
// Hides the menu bar - completely erasing it from the screen.
/*
void HideMenuBarOld (void)
{
	Rect			theRect;
	RgnHandle		worldRgn, menuBarRgn;
	GrafPtr			wasPort, tempPort;

	if (LMGetMBarHeight() != 0)
	{
		GetPort(&wasPort);
		tempPort = (GrafPtr)NewPtrClear(sizeof(GrafPort));
		OpenPort(tempPort);
		SetPort((GrafPtr)tempPort);

		LMSetMBarHeight(0);

		theRect = (**GetGrayRgn()).rgnBBox;
		UnionRect(&theRect, &qd.screenBits.bounds, &theRect);
		worldRgn = NewRgn();
		OpenRgn();
		FrameRoundRect(&theRect, 16, 16);
		CloseRgn(worldRgn);

		theRect = qd.screenBits.bounds;
		theRect.bottom = theRect.top + thisMac.menuHigh;
		menuBarRgn = NewRgn();
		RectRgn(menuBarRgn, &theRect);

		SectRgn(worldRgn, menuBarRgn, menuBarRgn);	//	/------------------\
		DisposeRgn(worldRgn);						//	|__________________|

		UnionRgn(tempPort->visRgn, menuBarRgn, tempPort->visRgn);
		DisposeRgn(menuBarRgn);

		PaintRect(&theRect);

		ClosePort(tempPort);
		SetPort((GrafPtr)wasPort);
	}
}
*/
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
// Slowly walks the palette from its gray luminance state to the full color…
// palette.  In this way, color appears to slowly wash in.

void WashColorIn (void)
{
	const int kGray2ColorSteps = 180;
	RGBQUAD wasColors[256];
	RGBQUAD newColors[256];
	UINT numColors;
	HBITMAP splashDIB;
	HDC splashDC, hdc;
	RECT clientRect;
	BOOL messageReceived, fading;
	MSG msg;
	DWORD wasFPS;
	int i, c;

	DisableMenuBar();

	if (FAILED(Gp_LoadBuiltInImageAsDIB(kSplash8BitPICT, &splashDIB)))
		RedAlert(kErrFailedGraphicLoad);

	GetClientRect(mainWindow, &clientRect);

	splashDC = CreateCompatibleDC(NULL);
	SaveDC(splashDC);
	SelectObject(splashDC, splashDIB);

	numColors = GetDIBColorTable(splashDC, 0, ARRAYSIZE(wasColors), wasColors);
	if (numColors != ARRAYSIZE(wasColors))
		RedAlert(kErrUnnaccounted);

	wasFPS = GetFrameRate();
	SetFrameRate(60);

	fading = TRUE;
	for (i = 0; fading && (i < kGray2ColorSteps); i++)
	{
		do
		{
			WaitUntilNextFrameOrMessage(&messageReceived);
			if (messageReceived)
			{
				while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					switch (msg.message)
					{
					case WM_KEYDOWN:
					case WM_SYSKEYDOWN:
					case WM_LBUTTONDOWN:
						fading = FALSE;
						break;
					}
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		} while (messageReceived);

		for (c = 0; c < ARRAYSIZE(newColors); c++)
		{
			newColors[c] = wasColors[c];
		}
		SetPaletteToGrays(newColors, ARRAYSIZE(newColors), i, kGray2ColorSteps);
		SetDIBColorTable(splashDC, 0, ARRAYSIZE(newColors), newColors);

		hdc = GetMainWindowDC();
		BitBlt(hdc, splashOriginH, splashOriginV, 640, 460, splashDC, 0, 0, SRCCOPY);
		ReleaseMainWindowDC(hdc);
		ValidateRect(mainWindow, NULL);
	}

	SetFrameRate(wasFPS);
	RestoreDC(splashDC, -1);
	DeleteDC(splashDC);
	DeleteObject(splashDIB);
	EnableMenuBar();
	InvalidateRect(mainWindow, NULL, TRUE);
}

//--------------------------------------------------------------  InitMainWindowCursor

void InitMainWindowCursor (void)
{
	SetMainWindowCursor(LoadCursor(NULL, IDC_ARROW));
}

//--------------------------------------------------------------  GetMainWindowCursor

HCURSOR GetMainWindowCursor (void)
{
	return mainWindowCursor;
}

//--------------------------------------------------------------  SetMainWindowCursor

void SetMainWindowCursor (HCURSOR hCursor)
{
	mainWindowCursor = hCursor;
}

//--------------------------------------------------------------  MainWindowProc

LRESULT CALLBACK MainWindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_ACTIVATEAPP:
		MainWindow_OnActivateApp(hwnd, (BOOL)wParam);
		return 0;

	case WM_CLOSE:
		SendMessage(hwnd, WM_COMMAND, ID_QUIT, 0);
		return 0;

	case WM_COMMAND:
		DoMenuChoice(hwnd, LOWORD(wParam));
		return 0;

	case WM_CREATE:
		SetMainWindowCursor(LoadCursor(NULL, IDC_ARROW));
		return 0;

	case WM_DESTROY:
		// Remove the menu bar from the window so that it isn't destroyed
		// with the window. It will be reattached to the main window if
		// it is recreated.
		SetMenu(hwnd, NULL);
		return 0;

	case WM_ENABLE:
		if (mapWindow != NULL && IsWindow(mapWindow))
		{
			EnableWindow(mapWindow, !!wParam);
		}
		if (toolsWindow != NULL && IsWindow(toolsWindow))
		{
			EnableWindow(toolsWindow, !!wParam);
		}
		if (linkWindow != NULL && IsWindow(linkWindow))
		{
			EnableWindow(linkWindow, !!wParam);
		}
		if (coordWindow != NULL && IsWindow(coordWindow))
		{
			EnableWindow(coordWindow, !!wParam);
		}
		return 0;

	case WM_KEYDOWN:
		HandleKeyEvent(hwnd, (BYTE)wParam);
		return 0;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	{
		Point wherePt;
		wherePt.h = GET_X_LPARAM(lParam);
		wherePt.v = GET_Y_LPARAM(lParam);
		if (uMsg == WM_LBUTTONDBLCLK && !ignoreDoubleClick)
		{
			HandleMainClick(hwnd, wherePt, true);
		}
		else
		{
			HandleMainClick(hwnd, wherePt, false);
		}
		ignoreDoubleClick = false;
		return 0;
	}

	case WM_MOVE:
		if (theMode == kEditMode)
		{
			WINDOWPLACEMENT placement;
			placement.length = sizeof(placement);
			GetWindowPlacement(hwnd, &placement);
			isEditH = (SInt16)placement.rcNormalPosition.left;
			isEditV = (SInt16)placement.rcNormalPosition.top;
		}
		return 0;

	case WM_SETCURSOR:
		if (LOWORD(lParam) == HTCLIENT)
		{
			SetCursor(GetMainWindowCursor());
			return TRUE;
		}
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		UpdateMainWindow();
		return 0;
	}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

//--------------------------------------------------------------  MainWindow_OnActivateApp

void MainWindow_OnActivateApp (HWND hwnd, BOOL fActivate)
{
	if (theMode == kPlayMode)
	{
		if (fActivate)
		{
			switchedOut = false;
			if (isPlayMusicGame && !isMusicOn)
				StartMusic();
			//HideCursor();
		}
		else
		{
			InitCursor();
			switchedOut = true;
			if (isPlayMusicGame && isMusicOn)
				StopTheMusic();
		}
	}
	else
	{
		if (fActivate)
		{
			switchedOut = false;
			InitCursor();
			if ((isPlayMusicIdle) && (theMode != kEditMode))
			{
				OSErr theErr = StartMusic();
				if (theErr != noErr)
				{
					YellowAlert(hwnd, kYellowNoMusic, theErr);
					failedMusic = true;
				}
			}
			incrementModeTime = MillisToTicks(GetTickCount()) + kIdleSplashTicks;

#ifndef COMPILEDEMO
//			if (theMode == kEditMode)
//				SeeIfValidScrapAvailable(true);
#endif
		}
		else
		{
			switchedOut = true;
			InitCursor();
			if ((isMusicOn) && (theMode != kEditMode))
				StopTheMusic();
		}
	}
}
