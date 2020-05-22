//============================================================================
//----------------------------------------------------------------------------
//								MainWindow.c
//----------------------------------------------------------------------------
//============================================================================


#define _CRT_SECURE_NO_WARNINGS
//#include <NumberFormatting.h>
//#include <ToolUtils.h>
#include <wchar.h>
#include "Macintosh.h"
#include "Externs.h"
#include "Environ.h"
#include "House.h"
#include "RectUtils.h"
#include "ResourceIDs.h"


#define kMainWindowID			128
#define kEditWindowID			129
#define kMenuWindowID			130


void DrawOnSplash (HDC);
void SetPaletteToGrays (void);
void HardDrawMainWindow (void);
void RestoreColorsSlam (void);


CTabHandle		theCTab;
PixMapHandle	thePMap;
ColorSpec *		wasColors;
ColorSpec *		newColors;
HCURSOR			handCursor, beamCursor, vertCursor, horiCursor;
HCURSOR			diagCursor;
Rect			workSrcRect;
HDC				workSrcMap;
Rect			mainWindowRect;
HWND			mainWindow, menuWindow;
SInt16			isEditH, isEditV;
SInt16			playOriginH, playOriginV;
SInt16			splashOriginH, splashOriginV;
SInt16			theMode;
Boolean			fadeGraysOut, isDoColorFade, splashDrawn;

extern	GDHandle	thisGDevice;
extern	SInt16		toolSelected;
extern	Rect		justRoomsRect;
extern	Boolean		noRoomAtAll, isUseSecondScreen;
extern	Boolean		quickerTransitions, houseIsReadOnly;
extern	HMENU		theMenuBar;


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
	wcscpy(lfHouse.lfFaceName, L"Tahoma");
	lfNative = lfHouse;
	lfNative.lfHeight = -12;
	lfNative.lfWeight = FW_BOLD;
	wcscpy(lfNative.lfFaceName, L"Tahoma");

	PasStringCopyC("House: ", houseLoadedStr);
	PasStringConcat(houseLoadedStr, thisHouseName);
	if ((thisMac.hasQT) && (hasMovie))
		PasStringConcatC(houseLoadedStr, " (QT)");
	MoveToEx(hdc, splashOriginH + 436, splashOriginV + 314, NULL);
	theFont = CreateFontIndirect(&lfHouse);
	wasFont = SelectObject(hdc, theFont);
	if (thisMac.isDepth == 4)
	{
		wasTextColor = SetTextColor(hdc, whiteColor);
		Mac_DrawString(hdc, houseLoadedStr);
		SetTextColor(hdc, wasTextColor);
	}
	else
	{
		if (houseIsReadOnly)
			ColorText(hdc, houseLoadedStr, 5L);
		else
			ColorText(hdc, houseLoadedStr, 28L);
	}
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

//	SetPort((GrafPtr)workSrcMap);
	Mac_PaintRect(workSrcMap, &workSrcRect, GetStockObject(BLACK_BRUSH));
	QSetRect(&tempRect, 0, 0, 640, 460);
	QOffsetRect(&tempRect, splashOriginH, splashOriginV);
	LoadScaledGraphic(workSrcMap, kSplash8BitPICT, &tempRect);
	DrawOnSplash(workSrcMap);
//	SetPortWindowPort(mainWindow);
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

//	SetPortWindowPort(mainWindow);
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
//		SetPort((GrafPtr)workSrcMap);
		Mac_PaintRect(workSrcMap, &workSrcRect, GetStockObject(BLACK_BRUSH));
		QSetRect(&tempRect, 0, 0, 640, 460);
		QOffsetRect(&tempRect, splashOriginH, splashOriginV);
		LoadScaledGraphic(workSrcMap, kSplash8BitPICT, &tempRect);
		Mac_CopyBits(workSrcMap, mainWindowDC,
				&workSrcRect, &mainWindowRect, srcCopy, nil);
//		SetPortWindowPort(mainWindow);

		DrawOnSplash(mainWindowDC);
	}

	ReleaseMainWindowDC(mainWindowDC);
	splashDrawn = true;
}

//--------------------------------------------------------------  UpdateMenuBarWindow
// Ugly kludge to cover over the menu bar when playing game on 2nd monitor.

void UpdateMenuBarWindow (void)
{
	return;
#if 0
	Rect		bounds;

	if (menuWindow == nil)
		return;

	GetLocalWindowRect(menuWindow, &bounds);
	PaintRect(&bounds);
#endif
}

//--------------------------------------------------------------  OpenMainWindow
// Opens up the main window (how it does this depends on mode were in).

void OpenMainWindow (void)
{
//	SInt32		wasSeed;
	SInt16		whichRoom;
	RECT		rcClient;
	DWORD		windowStyle;
	HDC			mainWindowDC;

	if (mainWindow != NULL)
	{
		YellowAlert(mainWindow, kYellowUnaccounted, 6);
		return;
	}

	if (theMode == kEditMode)
	{
		if (menuWindow != NULL)
			DestroyWindow(menuWindow);
		menuWindow = NULL;

		QSetRect(&mainWindowRect, 0, 0, 512, 322);
		rcClient.left = mainWindowRect.left;
		rcClient.top = mainWindowRect.top;
		rcClient.right = mainWindowRect.right;
		rcClient.bottom = mainWindowRect.bottom;
		windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		AdjustWindowRect(&rcClient, windowStyle, TRUE);
		mainWindow = CreateWindow(
			WC_MAINWINDOW,
			L"Main Window",
			windowStyle,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			rcClient.right - rcClient.left,
			rcClient.bottom - rcClient.top,
			NULL,
			theMenuBar,
			HINST_THISCOMPONENT,
			NULL
		);

		if (OptionKeyDown())
		{
			isEditH = 3;
			isEditV = 41;
		}
#if 0
		MoveWindow(mainWindow, isEditH, isEditV, true);
#endif
		ShowWindow(mainWindow, SW_SHOWDEFAULT);
#if 0
		SetPortWindowPort(mainWindow);
		ClipRect(&mainWindowRect);
		ForeColor(blackColor);
		BackColor(whiteColor);
#endif

		whichRoom = GetFirstRoomNumber();
		CopyRoomToThisRoom(whichRoom);
		ReflectCurrentRoom(false);
	}
	else
	{
#if 0
		if (menuWindow == nil)
		{
			menuWindow = GetNewCWindow(kMenuWindowID, nil, kPutInFront);
			SizeWindow(menuWindow, RectWide(&thisMac.screen), 20, false);
			MoveWindow(menuWindow, thisMac.screen.left,
					thisMac.screen.top, true);
			ShowWindow(menuWindow);
		}
#endif
		mainWindowRect = thisMac.screen;
		ZeroRectCorner(&mainWindowRect);
		mainWindowRect.bottom -= 20;		// thisMac.menuHigh
		windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		SetRect(&rcClient, 0, 0, mainWindowRect.right, mainWindowRect.bottom);
		AdjustWindowRect(&rcClient, windowStyle, TRUE);
		mainWindow = CreateWindow(
			WC_MAINWINDOW,
			L"Main Window",
			windowStyle,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			rcClient.right - rcClient.left,
			rcClient.bottom - rcClient.top,
			NULL,
			theMenuBar,
			HINST_THISCOMPONENT,
			NULL
		);
		if (mainWindow == NULL)
			RedAlert(kErrDialogDidntLoad);
		ShowWindow(mainWindow, SW_SHOWDEFAULT);
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

#if 0
//		if ((fadeGraysOut) && (isDoColorFade))
//		{
//			wasSeed = ExtractCTSeed((CGrafPtr)mainWindow);
//			SetPortWindowPort(mainWindow);
//			SetPaletteToGrays();
//			HardDrawMainWindow();
//			fadeGraysOut = false;
//			ForceCTSeed((CGrafPtr)mainWindow, wasSeed);
//		}

		SetPortWindowPort(mainWindow);
#endif
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

//--------------------------------------------------------------  ZoomBetweenWindows
// Zooms from one window size to another.  Just for effect.

/*
#ifndef COMPILEDEMO
void ZoomBetweenWindows (void)
{
	Rect		aRect;
	short		h, v;

	if (theMode == kEditMode)
	{
		QSetRect(&aRect, 0, 0, 512, 342);
		QOffsetRect(&aRect, isEditH, isEditV);
		ZoomRectToRect(&(thisMac.screen), &aRect);
	}
	else
	{
		aRect = mainWindow->portRect;
		GetWindowLeftTop(mainWindow, &h, &v);
		QOffsetRect(&aRect, h, v);
		ZoomRectToRect(&aRect, &(thisMac.screen));
	}
}
#endif
*/

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
		Mac_NumToString((SInt32)thisRoom->floor, tempStr);
		PasStringConcat(newTitle, tempStr);
		PasStringConcatC(newTitle, ", ");
		Mac_NumToString((SInt32)thisRoom->suite, tempStr);
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

	//SetPortWindowPort(mainWindow);
	//GlobalToLocal(&wherePt); // window procedures already get client coordinates

	if (toolSelected == kSelectTool)
		DoSelectionClick(wherePt, isDoubleClick);
	else
		DoNewObjectClick(hwnd, wherePt);

	if (GetKeyState(VK_SHIFT) >= 0) // if shift key up
	{
		EraseSelectedTool();
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

// Sets up a gray palette corresponding in luminance to the standard color…
// palette.  This is to facilitate the gray->color fade when the game comes up.
/*
void SetPaletteToGrays (void)
{
	GDHandle	theDevice;
	long		longGray;
	short		i;
	char		wasState;

	wasState = HGetState((Handle)thisGDevice);
	HLock((Handle)thisGDevice);
	thePMap = (*thisGDevice)->gdPMap;
	HSetState((Handle)thisGDevice, wasState);

	theCTab = (*thePMap)->pmTable;
	wasColors = nil;
	wasColors = (ColorSpec*)NewPtr(sizeof(ColorSpec) * 256);
	if (wasColors == nil)
		RedAlert(kErrNoMemory);

	newColors = nil;
	newColors = (ColorSpec*)NewPtr(sizeof(ColorSpec) * 256);
	if (newColors == nil)
		RedAlert(kErrNoMemory);

	for (i = 0; i < 256; i++)
	{
		wasColors[i] = (*theCTab)->ctTable[i];
		newColors[i] = (*theCTab)->ctTable[i];

		if (i != 5)
		{
			longGray = ((long)newColors[i].rgb.red * 3L) / 10L +
				((long)newColors[i].rgb.green * 6L) / 10L +
				((long)newColors[i].rgb.blue * 1L) / 10L;

			newColors[i].rgb.red = (unsigned short)longGray;
			newColors[i].rgb.green = (unsigned short)longGray;
			newColors[i].rgb.blue = (unsigned short)longGray;
		}
	}

	theDevice = GetGDevice();
	SetGDevice(thisGDevice);
	SetEntries(0, 255, newColors);
	SetGDevice(theDevice);
}
*/
//--------------------------------------------------------------  HardDrawMainWindow
// Ignores the ToolBox - this function draws direct to screen in order to…
// circumvent the Toolbox's attempt to color-match to the current palette.
/*
void HardDrawMainWindow (void)
{
	PixMapHandle	pixMapH;
	Point			offsetPt;
	long			srcRowBytes, destRowBytes;
	long			src;
	long			dest;
	short			i, w;
	SInt8			mode;
	char			wasState;

	wasState = HGetState((Handle)thisGDevice);
	HLock((Handle)thisGDevice);
	pixMapH = (**thisGDevice).gdPMap;
	HSetState((Handle)thisGDevice, wasState);

	srcRowBytes = (long)((*(workSrcMap->portPixMap))->rowBytes & 0x7FFF);
	destRowBytes = (**pixMapH).rowBytes & 0x7FFF;
	src = (long)((*(workSrcMap->portPixMap))->baseAddr);
	dest = (long)((**pixMapH).baseAddr) + splashOriginH +
			((splashOriginV + thisMac.menuHigh) * destRowBytes);

	offsetPt.h = 0;
	offsetPt.v = 0;
	ShieldCursor(&mainWindowRect, offsetPt);
	mode = true32b;
	SwapMMUMode(&mode);
	for (i = 0; i < 460; i++)
	{
		for (w = 0; w < 160; w++)
		{
			*(long *)dest = *(long *)src;
			dest += 4L;
			src += 4L;
		}
		src -= 640;
		dest -= 640;
		src += srcRowBytes;
		dest += destRowBytes;
	}
	SwapMMUMode(&mode);
	ShowCursor();
}
*/
//--------------------------------------------------------------  WashColorIn
// Slowly walks the palette from its gray luminance state to the full color…
// palette.  In this way, color appears to slowly wash in.
/*
void WashColorIn (void)
{
	#define		kGray2ColorSteps	180
	GDHandle	theDevice;
	long		longDelta;
	short		i, c;

	theDevice = GetGDevice();
	SetGDevice(thisGDevice);

	for (i = 0; i < kGray2ColorSteps; i++)
	{
		for (c = 0; c < 256; c++)
		{
			if (c != 5)
			{
				longDelta = (((long)wasColors[c].rgb.red -
						(long)newColors[c].rgb.red) /
						(long)(kGray2ColorSteps - i)) + (long)newColors[c].rgb.red;
				newColors[c].rgb.red = (unsigned short)longDelta;

				longDelta = (((long)wasColors[c].rgb.green -
						(long)newColors[c].rgb.green) /
						(long)(kGray2ColorSteps - i)) +
						(long)newColors[c].rgb.green;
				newColors[c].rgb.green = (unsigned short)longDelta;

				longDelta = (((long)wasColors[c].rgb.blue -
						(long)newColors[c].rgb.blue) /
						(long)(kGray2ColorSteps - i)) +
						(long)newColors[c].rgb.blue;
				newColors[c].rgb.blue = (unsigned short)longDelta;
			}
		}
		SetEntries(0, 255, newColors);
		if (Button())
			break;
	}

	SetEntries(0, 255, wasColors);
	SetGDevice(theDevice);

	RestoreColorsSlam();

	if (wasColors != nil)
		DisposePtr((Ptr)wasColors);
	if (newColors != nil)
		DisposePtr((Ptr)newColors);
}
*/
//--------------------------------------------------------------  MainWindowProc

LRESULT CALLBACK MainWindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		SendMessage(hwnd, WM_COMMAND, ID_QUIT, 0);
		return 0;

	case WM_COMMAND:
		DoMenuChoice(hwnd, LOWORD(wParam));
		return 0;

	case WM_DESTROY:
		// Remove the menu bar from the window so that it isn't destroyed
		// with the window. It will be reattached to the main window if
		// it is recreated.
		SetMenu(hwnd, NULL);
		return 0;

	case WM_KEYDOWN:
		HandleKeyEvent(hwnd, (BYTE)wParam);
		return 0;

	case WM_LBUTTONDOWN:
	{
		Point wherePt;
		wherePt.h = GET_X_LPARAM(lParam);
		wherePt.v = GET_Y_LPARAM(lParam);
		HandleMainClick(hwnd, wherePt, false);
		return 0;
	}

	case WM_LBUTTONDBLCLK:
	{
		Point wherePt;
		wherePt.h = GET_X_LPARAM(lParam);
		wherePt.v = GET_Y_LPARAM(lParam);
		HandleMainClick(hwnd, wherePt, true);
		return 0;
	}

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
