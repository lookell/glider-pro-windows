#include "Map.h"

//============================================================================
//----------------------------------------------------------------------------
//                                   Map.c
//----------------------------------------------------------------------------
//============================================================================


#include "ColorUtils.h"
#include "DialogUtils.h"
#include "Environ.h"
#include "GliderDefines.h"
#include "House.h"
#include "HouseIO.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "Menu.h"
#include "ObjectEdit.h"
#include "RectUtils.h"
#include "ResourceIDs.h"
#include "Room.h"
#include "Utilities.h"
#include "WinAPI.h"


#define kMapRoomsHigh			9	// was 7
#define kMapRoomsWide			9	// was 7
#define kMapScrollBarWidth		16
#define kHScrollRef				5L
#define kVScrollRef				27L
#define kMapGroundValue			56
#define kYesDoNewRoom			IDOK
#define WC_MAPWINDOW            L"GliderMapWindow"


void RegisterMapWindowClass (void);
void LoadGraphicPlus (HDC hdc, SInt16 resID, Rect *theRect);
void RedrawMapContents (HDC hdc);
void ResizeMapWindow (WINDOWPOS *windowPos);
LRESULT CALLBACK MapWindowProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void LiveHScrollAction (HWND hwnd, WORD scrollRequest);
void LiveVScrollAction (HWND hwnd, WORD scrollRequest);
Boolean QueryNewRoom (HWND ownerWindow);
void CreateNailOffscreen (void);
void KillNailOffscreen (void);

Rect			nailSrcRect, activeRoomRect, wasActiveRoomRect;
Rect			mapWindowRect;
HDC				nailSrcMap;
HWND			mapWindow;
SInt16			isMapH, isMapV, mapRoomsHigh, mapRoomsWide;
SInt16			mapLeftRoom, mapTopRoom;
Boolean			isMapOpen, doPrettyMap;


//==============================================================  Functions
//--------------------------------------------------------------  RegisterMapWindowClass

void RegisterMapWindowClass (void)
{
	WNDCLASSEX wcx;

	wcx.cbSize = sizeof(wcx);
	if (!GetClassInfoEx(HINST_THISCOMPONENT, WC_MAPWINDOW, &wcx))
	{
		wcx.cbSize = sizeof(wcx);
		wcx.style = CS_HREDRAW | CS_VREDRAW;
		wcx.lpfnWndProc = MapWindowProc;
		wcx.cbClsExtra = 0;
		wcx.cbWndExtra = 0;
		wcx.hInstance = HINST_THISCOMPONENT;
		wcx.hIcon = NULL;
		wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcx.lpszMenuName = NULL;
		wcx.lpszClassName = WC_MAPWINDOW;
		wcx.hIconSm = NULL;
		if (!RegisterClassEx(&wcx))
			RedAlert(kErrUnnaccounted);
	}
}

//--------------------------------------------------------------  ThisRoomVisibleOnMap

#ifndef COMPILEDEMO
Boolean ThisRoomVisibleOnMap (void)
{
	SInt16		h, v;

	h = thisRoom->suite;
	v = kMapGroundValue - thisRoom->floor;

	if ((h < mapLeftRoom) || (v < mapTopRoom) ||
			(h >= (mapLeftRoom + mapRoomsWide)) ||
			(v >= (mapTopRoom + mapRoomsHigh)))
		return (false);
	else
		return (true);
}
#endif

//--------------------------------------------------------------  CenterMapOnRoom

#ifndef COMPILEDEMO
void CenterMapOnRoom (SInt16 h, SInt16 v)
{
	SCROLLINFO scrollInfo;

	if (mapWindow == NULL)
		return;

	mapLeftRoom = h - (mapRoomsWide / 2);
	mapTopRoom = (kMapGroundValue - v) - (mapRoomsHigh / 2);

	if (mapLeftRoom < 0)
		mapLeftRoom = 0;
	else if (mapLeftRoom > (kMaxNumRoomsH - mapRoomsWide))
		mapLeftRoom = kMaxNumRoomsH - mapRoomsWide;

	if (mapTopRoom < 0)
		mapTopRoom = 0;
	else if (mapTopRoom > (kMaxNumRoomsV - mapRoomsHigh))
		mapTopRoom = kMaxNumRoomsV - mapRoomsHigh;

	if (mapWindow != NULL)
	{
		scrollInfo.cbSize = sizeof(scrollInfo);
		scrollInfo.fMask = SIF_POS;

		scrollInfo.nPos = mapLeftRoom;
		SetScrollInfo(mapWindow, SB_HORZ, &scrollInfo, TRUE);
		scrollInfo.nPos = mapTopRoom;
		SetScrollInfo(mapWindow, SB_VERT, &scrollInfo, TRUE);
	}
}
#endif

//--------------------------------------------------------------  FlagMapRoomsForUpdate

#ifndef COMPILEDEMO
void FlagMapRoomsForUpdate (void)
{
	if (mapWindow == NULL)
		return;

	Mac_InvalWindowRect(mapWindow, &wasActiveRoomRect);
	Mac_InvalWindowRect(mapWindow, &activeRoomRect);
}
#endif

//--------------------------------------------------------------  FindNewActiveRoomRect

#ifndef COMPILEDEMO
void FindNewActiveRoomRect (void)
{
	Rect		aRoom;
	SInt16		h, i;
	SInt16		floor, suite, whoCares;
	Boolean		activeRoomVisible;

	if (mapWindow == nil)
		return;

	activeRoomVisible = false;

	for (i = 0; i < mapRoomsHigh; i++)
	{
		for (h = 0; h < mapRoomsWide; h++)
		{
			QSetRect(&aRoom, 0, 0, kMapRoomWidth, kMapRoomHeight);
			QOffsetRect(&aRoom, kMapRoomWidth * h, kMapRoomHeight * i);

			suite = h + mapLeftRoom;
			floor = kMapGroundValue - (i + mapTopRoom);
			if ((RoomExists(suite, floor, &whoCares)) && (houseUnlocked))
			{
				if (whoCares == thisRoomNumber)
				{
					wasActiveRoomRect = activeRoomRect;
					activeRoomRect = aRoom;
					activeRoomVisible = true;
				}
			}
		}
	}

	if (activeRoomVisible)
	{
		activeRoomRect.right++;
		activeRoomRect.bottom++;
		QInsetRect(&activeRoomRect, -1, -1);
	}
}
#endif

//--------------------------------------------------------------  LoadGraphicPlus

void LoadGraphicPlus (HDC hdc, SInt16 resID, Rect *theRect)
{
	HBITMAP thePicture;

	thePicture = GetPicture(resID);
	if (thePicture == NULL)
	{
		return;
	}
	Mac_DrawPicture(hdc, thePicture, theRect);
	DeleteObject(thePicture);
}

//--------------------------------------------------------------  RedrawMapContents

void RedrawMapContents (HDC hdc)
{
#ifndef COMPILEDEMO
	Rect aRoom, src;
	SInt16 h, i, groundLevel;
	SInt16 floor, suite, roomNum, type;
	Boolean activeRoomVisible;

	if (mapWindow == NULL || hdc == NULL)
		return;

	SaveDC(hdc);

	activeRoomVisible = false;
	groundLevel = kMapGroundValue - mapTopRoom;

	for (i = 0; i < mapRoomsHigh; i++)
	{
		for (h = 0; h < mapRoomsWide; h++)
		{
			QSetRect(&aRoom, 0, 0, kMapRoomWidth, kMapRoomHeight);
			QOffsetRect(&aRoom, kMapRoomWidth * h, kMapRoomHeight * i);

			suite = h + mapLeftRoom;
			floor = kMapGroundValue - (i + mapTopRoom);
			if ((RoomExists(suite, floor, &roomNum)) && (houseUnlocked))
			{
				type = thisHouse.rooms[roomNum].background - kBaseBackgroundID;
				if (type > kNumBackgrounds)
				{
					if (!doPrettyMap)
						type = kNumBackgrounds;	// Draw "?" thumbnail.
				}
				if (type > kNumBackgrounds)		// Do a "pretty" thumbnail.
				{
					LoadGraphicPlus(hdc, type + kBaseBackgroundID, &aRoom);
				}
				else
				{
					QSetRect(&src, 0, 0, kMapRoomWidth, kMapRoomHeight);
					QOffsetRect(&src, 0, type * kMapRoomHeight);
					Mac_CopyBits(nailSrcMap, hdc, &src, &aRoom, srcCopy, nil);
				}

				if (roomNum == thisRoomNumber)
				{
					activeRoomRect = aRoom;
					activeRoomVisible = true;
				}
			}
			else
			{
				const WORD grayBits[8] = {
					0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA,
				};
				HBITMAP ditherPattern;
				HBRUSH ditherBrush;

				ditherPattern = CreateBitmap(8, 8, 1, 1, grayBits);
				ditherBrush = CreatePatternBrush(ditherPattern);
				SaveDC(hdc);
				SelectObject(hdc, ditherBrush);

				SetBkColor(hdc, whiteColor);
				if (i >= groundLevel)
					SetTextColor(hdc, greenColor);
				else
					SetTextColor(hdc, blueColor);
				Mac_PaintRect(hdc, &aRoom, ditherBrush);

				RestoreDC(hdc, -1);
				DeleteObject(ditherBrush);
				DeleteObject(ditherPattern);
			}
		}
	}

	SelectObject(hdc, GetStockObject(BLACK_PEN));

	for (i = 1; i < mapRoomsWide; i++)
	{
		MoveToEx(hdc, i * kMapRoomWidth, 0, NULL);
		Mac_Line(hdc, 0, mapRoomsHigh * kMapRoomHeight);
	}

	for (i = 1; i < mapRoomsHigh; i++)
	{
		MoveToEx(hdc, 0, i * kMapRoomHeight, NULL);
		Mac_Line(hdc, mapRoomsWide * kMapRoomWidth, 0);
	}

	if (activeRoomVisible)
	{
		SetDCBrushColor(hdc, redColor);
		activeRoomRect.right++;
		activeRoomRect.bottom++;
		Mac_FrameRect(hdc, &activeRoomRect, GetStockObject(DC_BRUSH), 1, 1);
		QInsetRect(&activeRoomRect, 1, 1);
		Mac_FrameRect(hdc, &activeRoomRect, GetStockObject(DC_BRUSH), 1, 1);
		QInsetRect(&activeRoomRect, -1, -1);
	}

	RestoreDC(hdc, -1);
#endif
}

//--------------------------------------------------------------  UpdateMapWindow

void UpdateMapWindow (void)
{
#ifndef COMPILEDEMO
	SCROLLINFO scrollInfo;

	if (mapWindow == NULL)
		return;

	scrollInfo.cbSize = sizeof(scrollInfo);

	scrollInfo.fMask = SIF_POS;
	scrollInfo.nPos = mapLeftRoom;
	SetScrollInfo(mapWindow, SB_HORZ, &scrollInfo, TRUE);
	scrollInfo.nPos = mapTopRoom;
	SetScrollInfo(mapWindow, SB_VERT, &scrollInfo, TRUE);

	RedrawWindow(mapWindow, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
#endif
}

//--------------------------------------------------------------  ResizeMapWindow

void ResizeMapWindow (WINDOWPOS *windowPos)
{
#ifndef COMPILEDEMO
	DWORD windowStyle, extendedStyle;
	RECT rectAdjust;
	LONG horzAdjust, vertAdjust;
	LONG newWidth, newHeight;
	SInt16 wasMapRoomsWide, wasMapRoomsHigh;
	SCROLLINFO scrollInfo;

	windowStyle = (DWORD)GetWindowLongPtr(mapWindow, GWL_STYLE);
	extendedStyle = (DWORD)GetWindowLongPtr(mapWindow, GWL_EXSTYLE);
	SetRectEmpty(&rectAdjust);
	AdjustWindowRectEx(&rectAdjust, windowStyle, FALSE, extendedStyle);
	horzAdjust = rectAdjust.right - rectAdjust.left + GetSystemMetrics(SM_CXVSCROLL);
	vertAdjust = rectAdjust.bottom - rectAdjust.top + GetSystemMetrics(SM_CYHSCROLL);

	newWidth = windowPos->cx - horzAdjust;
	newHeight = windowPos->cy - vertAdjust;
	if (newWidth <= 0 || newHeight <= 0)
		return;

	wasMapRoomsWide = mapRoomsWide;
	wasMapRoomsHigh = mapRoomsHigh;

	mapRoomsWide = (SInt16)(newWidth / kMapRoomWidth);
	if (mapRoomsWide < 3)
		mapRoomsWide = 3;
	mapRoomsHigh = (SInt16)(newHeight / kMapRoomHeight);
	if (mapRoomsHigh < 3)
		mapRoomsHigh = 3;
	QSetRect(&mapWindowRect, 0, 0,
		mapRoomsWide * kMapRoomWidth,
		mapRoomsHigh * kMapRoomHeight);
	windowPos->cx = mapWindowRect.right + horzAdjust;
	windowPos->cy = mapWindowRect.bottom + vertAdjust;

	scrollInfo.cbSize = sizeof(scrollInfo);

	scrollInfo.fMask = SIF_PAGE | SIF_DISABLENOSCROLL;
	scrollInfo.nPage = mapRoomsWide;
	SetScrollInfo(mapWindow, SB_HORZ, &scrollInfo, TRUE);
	scrollInfo.nPage = mapRoomsHigh;
	SetScrollInfo(mapWindow, SB_VERT, &scrollInfo, TRUE);

	scrollInfo.fMask = SIF_POS;
	GetScrollInfo(mapWindow, SB_HORZ, &scrollInfo);
	mapLeftRoom = (SInt16)scrollInfo.nPos;
	GetScrollInfo(mapWindow, SB_VERT, &scrollInfo);
	mapTopRoom = (SInt16)scrollInfo.nPos;

	if (mapRoomsWide != wasMapRoomsWide || mapRoomsHigh != wasMapRoomsHigh)
	{
		Mac_InvalWindowRect(mapWindow, &mapWindowRect);
	}
#endif
}

//--------------------------------------------------------------  OpenMapWindow

void OpenMapWindow (void)
{
#ifndef COMPILEDEMO
	RECT windowRect;
	DWORD windowStyle, extendedStyle;
	WINDOWPLACEMENT placement;
	SCROLLINFO scrollInfo;

	if (mapWindow == NULL)
	{
		RegisterMapWindowClass();
		CreateNailOffscreen();

		windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
			WS_HSCROLL | WS_VSCROLL;
		extendedStyle = WS_EX_DLGMODALFRAME;
		QSetRect(&mapWindowRect, 0, 0,
			mapRoomsWide * kMapRoomWidth,
			mapRoomsHigh * kMapRoomHeight);
		SetRect(&windowRect, 0, 0,
			mapRoomsWide * kMapRoomWidth + GetSystemMetrics(SM_CXVSCROLL),
			mapRoomsHigh * kMapRoomHeight + GetSystemMetrics(SM_CYHSCROLL));
		AdjustWindowRectEx(&windowRect, windowStyle, FALSE, extendedStyle);
		mapWindow = CreateWindowEx(extendedStyle, WC_MAPWINDOW, L"Map",
			windowStyle, 0, 0, 0, 0, mainWindow, NULL, HINST_THISCOMPONENT, NULL);

		if (mapWindow == NULL)
			RedAlert(kErrNoMemory);

//		if (OptionKeyDown())
//		{
//			isMapH = 3;
//			isMapV = qd.screenBits.bounds.bottom - 100;
//		}
		placement.length = sizeof(placement);
		GetWindowPlacement(mapWindow, &placement);
		placement.flags = 0;
		OffsetRect(&windowRect, -windowRect.left, -windowRect.top);
		OffsetRect(&windowRect, isMapH, isMapV);
		placement.rcNormalPosition = windowRect;
		placement.showCmd = SW_SHOWNOACTIVATE;
		SetWindowPlacement(mapWindow, &placement);

		QSetRect(&wasActiveRoomRect, 0, 0, 1, 1);
		QSetRect(&activeRoomRect, 0, 0, 1, 1);

		scrollInfo.cbSize = sizeof(scrollInfo);
		scrollInfo.fMask = SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL;

		scrollInfo.nMin = 0;
		scrollInfo.nMax = kMaxNumRoomsH - 1;
		scrollInfo.nPage = mapRoomsWide;
		SetScrollInfo(mapWindow, SB_HORZ, &scrollInfo, TRUE);

		scrollInfo.nMin = 0;
		scrollInfo.nMax = kMaxNumRoomsV - 1;
		scrollInfo.nPage = mapRoomsHigh;
		SetScrollInfo(mapWindow, SB_VERT, &scrollInfo, TRUE);

		CenterMapOnRoom(thisRoom->suite, thisRoom->floor);
	}

	UpdateMapCheckmark(true);
#endif
}

//--------------------------------------------------------------  CloseMapWindow

void CloseMapWindow (void)
{
#ifndef COMPILEDEMO
	if (mapWindow != NULL)
	{
		DestroyWindow(mapWindow);
		mapWindow = NULL;
	}
	UpdateMapCheckmark(false);
#endif
}

//--------------------------------------------------------------  ToggleMapWindow

void ToggleMapWindow (void)
{
#ifndef COMPILEDEMO
	if (mapWindow == NULL)
	{
		OpenMapWindow();
		isMapOpen = true;
	}
	else
	{
		CloseMapWindow();
		isMapOpen = false;
	}
#endif
}

//--------------------------------------------------------------  MapWindowProc

LRESULT CALLBACK MapWindowProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOVE:
	{
		WINDOWPLACEMENT placement;
		placement.length = sizeof(placement);
		GetWindowPlacement(hwnd, &placement);
		isMapH = (SInt16)placement.rcNormalPosition.left;
		isMapV = (SInt16)placement.rcNormalPosition.top;
		return 0;
	}

	case WM_WINDOWPOSCHANGING:
	{
		WINDOWPOS *windowPos = (WINDOWPOS *)lParam;
		if ((windowPos->flags & SWP_NOSIZE) == 0)
		{
			ResizeMapWindow((WINDOWPOS *)lParam);
		}
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		if (BeginPaint(hwnd, &ps))
		{
			RedrawMapContents(ps.hdc);
			EndPaint(hwnd, &ps);
		}
		return 0;
	}

	case WM_CLOSE:
		ToggleMapWindow();
		return 0;

	case WM_HSCROLL:
		LiveHScrollAction(hwnd, LOWORD(wParam));
		return 0;

	case WM_VSCROLL:
		LiveVScrollAction(hwnd, LOWORD(wParam));
		return 0;

	case WM_LBUTTONDOWN:
		HandleMapClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

//--------------------------------------------------------------  LiveHScrollAction
#ifndef COMPILEDEMO

void LiveHScrollAction (HWND hwnd, WORD scrollRequest)
{
	SCROLLINFO scrollInfo;
	int wasValue, newValue;

	scrollInfo.cbSize = sizeof(scrollInfo);
	scrollInfo.fMask = SIF_ALL;
	GetScrollInfo(hwnd, SB_HORZ, &scrollInfo);
	wasValue = scrollInfo.nPos;
	newValue = wasValue;

	switch (scrollRequest)
	{
	case SB_LINELEFT:
		newValue = wasValue - 1;
		break;

	case SB_LINERIGHT:
		newValue = wasValue + 1;
		break;

	case SB_PAGELEFT:
		newValue = wasValue - (mapRoomsWide / 2);
		break;

	case SB_PAGERIGHT:
		newValue = wasValue + (mapRoomsWide / 2);
		break;

	case SB_THUMBTRACK:
		newValue = scrollInfo.nTrackPos;
		break;

	case SB_LEFT:
		newValue = scrollInfo.nMin;
		break;

	case SB_RIGHT:
		newValue = scrollInfo.nMax;
		break;
	}

	scrollInfo.fMask = SIF_POS;
	scrollInfo.nPos = newValue;
	SetScrollInfo(hwnd, SB_HORZ, &scrollInfo, TRUE);
	GetScrollInfo(hwnd, SB_HORZ, &scrollInfo);
	if (scrollInfo.nPos != wasValue)
	{
		mapLeftRoom = (SInt16)scrollInfo.nPos;
		UpdateMapWindow();
	}
}
#endif

//--------------------------------------------------------------  LiveVScrollAction
#ifndef COMPILEDEMO

void LiveVScrollAction (HWND hwnd, WORD scrollRequest)
{
	SCROLLINFO scrollInfo;
	int wasValue, newValue;

	scrollInfo.cbSize = sizeof(scrollInfo);
	scrollInfo.fMask = SIF_ALL;
	GetScrollInfo(hwnd, SB_VERT, &scrollInfo);
	wasValue = scrollInfo.nPos;
	newValue = wasValue;

	switch (scrollRequest)
	{
	case SB_LINEUP:
		newValue = wasValue - 1;
		break;

	case SB_LINEDOWN:
		newValue = wasValue + 1;
		break;

	case SB_PAGEUP:
		newValue = wasValue - (mapRoomsHigh / 2);
		break;

	case SB_PAGEDOWN:
		newValue = wasValue + (mapRoomsHigh / 2);
		break;

	case SB_THUMBTRACK:
		newValue = scrollInfo.nTrackPos;
		break;

	case SB_TOP:
		newValue = scrollInfo.nMin;
		break;

	case SB_BOTTOM:
		newValue = scrollInfo.nMax;
		break;
	}

	scrollInfo.fMask = SIF_POS;
	scrollInfo.nPos = newValue;
	SetScrollInfo(hwnd, SB_VERT, &scrollInfo, TRUE);
	GetScrollInfo(hwnd, SB_VERT, &scrollInfo);
	newValue = scrollInfo.nPos;
	if (newValue != wasValue)
	{
		mapTopRoom = (SInt16)scrollInfo.nPos;
		UpdateMapWindow();
	}
}
#endif

//--------------------------------------------------------------  HandleMapClick

void HandleMapClick (SInt16 clickX, SInt16 clickY)
{
#ifndef COMPILEDEMO
	SInt16 localH, localV;
	SInt16 roomH, roomV, itsNumber;

	localH = clickX / kMapRoomWidth;
	localV = clickY / kMapRoomHeight;

	if ((localH >= mapRoomsWide) || (localV >= mapRoomsHigh))
		return;
	if ((localH < 0) || (localV < 0))
		return;

	roomH = localH + mapLeftRoom;
	roomV = kMapGroundValue - (localV + mapTopRoom);

	if (RoomExists(roomH, roomV, &itsNumber))
	{
		CopyRoomToThisRoom(itsNumber);
		DeselectObject();
		ReflectCurrentRoom(false);

		if (thisMac.hasDrag)
		{
//			Rect aRoom;
//			SetPortWindowPort(mainWindow);
//			QSetRect(&aRoom, 0, 0, kMapRoomWidth, kMapRoomHeight);
//          // convert room rect to screen coordinates
//			CenterRectOnPoint(&aRoom, globalWhere);
//			if (DragRoom(theEvent, &aRoom, itsNumber))
//			{		// TEMP disabled.
//			}
		}
	}
	else
	{
		if (doBitchDialogs)
		{
			if (QueryNewRoom(mainWindow))
			{
				if (!CreateNewRoom(mainWindow, roomH, roomV))
				{
					YellowAlert(mainWindow, kYellowUnaccounted, 11);
					return;
				}
				else
				{
					DeselectObject();
					ReflectCurrentRoom(false);
				}
			}
			else
				return;
		}
		else
		{
			if (!CreateNewRoom(mainWindow, roomH, roomV))
			{
				YellowAlert(mainWindow, kYellowUnaccounted, 11);
				return;
			}
			else
			{
				DeselectObject();
				ReflectCurrentRoom(false);
			}
		}
	}
#endif
}

//--------------------------------------------------------------  QueryNewRoom

#ifndef COMPILEDEMO
Boolean QueryNewRoom (HWND ownerWindow)
{
	SInt16 hitWhat;

	hitWhat = Alert(kNewRoomAlert, ownerWindow, NULL);
	if (hitWhat == kYesDoNewRoom)
		return (true);
	else
		return (false);
}
#endif

//--------------------------------------------------------------  CreateNailOffscreen

#ifndef COMPILEDEMO
void CreateNailOffscreen (void)
{
	OSErr		theErr;

	if (nailSrcMap == NULL)
	{
		QSetRect(&nailSrcRect, 0, 0, kMapRoomWidth, kMapRoomHeight * (kNumBackgrounds + 1));
		theErr = CreateOffScreenGWorld(&nailSrcMap, &nailSrcRect, kPreferredDepth);
		LoadGraphic(nailSrcMap, kThumbnailPictID);
	}
}
#endif

//--------------------------------------------------------------  KillNailOffscreen

#ifndef COMPILEDEMO
void KillNailOffscreen (void)
{
	if (nailSrcMap != NULL)
	{
		DisposeGWorld(nailSrcMap);
		nailSrcMap = NULL;
	}
}
#endif

//--------------------------------------------------------------  MoveRoom

void MoveRoom (Point wherePt)
{
	SInt16		localH, localV;
	SInt16		roomH, roomV, itsNumber;

	localH = wherePt.h / kMapRoomWidth;
	localV = wherePt.v / kMapRoomHeight;

	if ((localH >= mapRoomsWide) || (localV >= mapRoomsHigh))
		return;

	roomH = localH + mapLeftRoom;
	roomV = kMapGroundValue - (localV + mapTopRoom);

	if (RoomExists(roomH, roomV, &itsNumber))
	{

	}
	else
	{
		thisRoom->floor = roomV;
		thisRoom->suite = roomH;
		fileDirty = true;
		UpdateMenus(false);
		UpdateMapWindow();
	}
}

