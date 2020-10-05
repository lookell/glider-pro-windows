#define GP_USE_WINAPI_H

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
#include "Link.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "Menu.h"
#include "ObjectEdit.h"
#include "Objects.h"
#include "RectUtils.h"
#include "ResourceIDs.h"
#include "Room.h"
#include "Scrap.h"
#include "Utilities.h"
#include "WinAPI.h"


#define kMapRoomsHigh			9	// was 7
#define kMapRoomsWide			9	// was 7
#define kMapGroundValue			56
#define kYesDoNewRoom			IDOK
#define WC_MAPWINDOW            L"GliderMapWindow"


void RegisterMapWindowClass (void);
void RedrawMapContents (HDC hdc);
void HandleMapSizingMessage (HWND hwnd, WPARAM sizedEdge, RECT *windowRect);
void HandleMapSizeMessage (HWND hwnd);
LRESULT CALLBACK MapWindowProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void LiveHScrollAction (HWND hwnd, WORD scrollRequest);
void LiveVScrollAction (HWND hwnd, WORD scrollRequest);
void HandleMapClick (SInt16 clickX, SInt16 clickY);
Boolean QueryNewRoom (HWND ownerWindow);
void CreateNailOffscreen (void);
void KillNailOffscreen (void);

HWND mapWindow;
SInt16 isMapH;
SInt16 isMapV;
SInt16 mapRoomsWide;
SInt16 mapRoomsHigh;
SInt16 mapLeftRoom;
SInt16 mapTopRoom;
Boolean isMapOpen;
Boolean doPrettyMap;

static Rect nailSrcRect;
static HDC nailSrcMap = NULL;
static Rect activeRoomRect;
static Rect wasActiveRoomRect;


//==============================================================  Functions
//--------------------------------------------------------------  RegisterMapWindowClass

#ifndef COMPILEDEMO
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
		wcx.hbrBackground = NULL;
		wcx.lpszMenuName = NULL;
		wcx.lpszClassName = WC_MAPWINDOW;
		wcx.hIconSm = NULL;
		if (!RegisterClassEx(&wcx))
			RedAlert(kErrUnnaccounted);
	}
}
#endif

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

void LoadGraphicPlus (HDC hdc, SInt16 resID, const Rect *theRect)
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
	HBRUSH ditherBrush;

	if (mapWindow == NULL || hdc == NULL)
		return;

	SaveDC(hdc);
	SetStretchBltMode(hdc, HALFTONE);
	SetBrushOrgEx(hdc, 0, 0, NULL);

	ditherBrush = CreateShadowBrush();

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
				SaveDC(hdc);

				SetBkColor(hdc, whiteColor);
				if (i >= groundLevel)
					SetTextColor(hdc, greenColor);
				else
					SetTextColor(hdc, blueColor);
				Mac_PaintRect(hdc, &aRoom, ditherBrush);

				RestoreDC(hdc, -1);
			}
		}
	}

	DeleteObject(ditherBrush);
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
		Mac_FrameRect(hdc, &activeRoomRect, (HBRUSH)GetStockObject(DC_BRUSH), 1, 1);
		QInsetRect(&activeRoomRect, 1, 1);
		Mac_FrameRect(hdc, &activeRoomRect, (HBRUSH)GetStockObject(DC_BRUSH), 1, 1);
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

//--------------------------------------------------------------  HandleMapSizingMessage

void HandleMapSizingMessage (HWND hwnd, WPARAM sizedEdge, RECT *windowRect)
{
	DWORD windowStyle, extendedStyle;
	RECT rectAdjust;
	LONG horzAdjust, vertAdjust;
	LONG wasClientWidth, wasClientHeight;
	LONG newMapRoomsWide, newMapRoomsHigh;
	LONG newWindowWidth, newWindowHeight;

	windowStyle = (DWORD)GetWindowLongPtr(hwnd, GWL_STYLE);
	extendedStyle = (DWORD)GetWindowLongPtr(hwnd, GWL_EXSTYLE);
	SetRectEmpty(&rectAdjust);
	AdjustWindowRectEx(&rectAdjust, windowStyle, FALSE, extendedStyle);
	horzAdjust = (rectAdjust.right - rectAdjust.left) + GetSystemMetrics(SM_CXVSCROLL);
	vertAdjust = (rectAdjust.bottom - rectAdjust.top) + GetSystemMetrics(SM_CYHSCROLL);

	wasClientWidth = (windowRect->right - windowRect->left) - horzAdjust;
	wasClientHeight = (windowRect->bottom - windowRect->top) - vertAdjust;

	newMapRoomsWide = (wasClientWidth + kMapRoomWidth / 2) / kMapRoomWidth;
	if (newMapRoomsWide < 3)
	{
		newMapRoomsWide = 3;
	}
	newMapRoomsHigh = (wasClientHeight + kMapRoomHeight / 2) / kMapRoomHeight;
	if (newMapRoomsHigh < 3)
	{
		newMapRoomsHigh = 3;
	}

	newWindowWidth = (newMapRoomsWide * kMapRoomWidth) + horzAdjust;
	newWindowHeight = (newMapRoomsHigh * kMapRoomHeight) + vertAdjust;

	switch (sizedEdge)
	{
	case WMSZ_LEFT:
		windowRect->left = windowRect->right - newWindowWidth;
		break;
	case WMSZ_RIGHT:
		windowRect->right = windowRect->left + newWindowWidth;
		break;
	case WMSZ_TOP:
		windowRect->top = windowRect->bottom - newWindowHeight;
		break;
	case WMSZ_TOPLEFT:
		windowRect->top = windowRect->bottom - newWindowHeight;
		windowRect->left = windowRect->right - newWindowWidth;
		break;
	case WMSZ_TOPRIGHT:
		windowRect->top = windowRect->bottom - newWindowHeight;
		windowRect->right = windowRect->left + newWindowWidth;
		break;
	case WMSZ_BOTTOM:
		windowRect->bottom = windowRect->top + newWindowHeight;
		break;
	case WMSZ_BOTTOMLEFT:
		windowRect->bottom = windowRect->top + newWindowHeight;
		windowRect->left = windowRect->right - newWindowWidth;
		break;
	case WMSZ_BOTTOMRIGHT:
		windowRect->bottom = windowRect->top + newWindowHeight;
		windowRect->right = windowRect->left + newWindowWidth;
		break;
	default:
		break; // this case shouldn't happen, so do nothing here
	}
}

//--------------------------------------------------------------  HandleMapSizeMessage

void HandleMapSizeMessage (HWND hwnd)
{
	RECT clientRect;
	LONG clientWidth, clientHeight;
	SInt16 wasMapRoomsWide, wasMapRoomsHigh;
	SCROLLINFO scrollInfo;

	GetClientRect(hwnd, &clientRect);
	clientWidth = clientRect.right;
	clientHeight = clientRect.bottom;

	wasMapRoomsWide = mapRoomsWide;
	wasMapRoomsHigh = mapRoomsHigh;
	mapRoomsWide = (SInt16)(clientWidth / kMapRoomWidth);
	mapRoomsHigh = (SInt16)(clientHeight / kMapRoomHeight);

	scrollInfo.cbSize = sizeof(scrollInfo);

	scrollInfo.fMask = SIF_PAGE | SIF_DISABLENOSCROLL;
	scrollInfo.nPage = mapRoomsWide;
	SetScrollInfo(hwnd, SB_HORZ, &scrollInfo, TRUE);
	scrollInfo.nPage = mapRoomsHigh;
	SetScrollInfo(hwnd, SB_VERT, &scrollInfo, TRUE);

	scrollInfo.fMask = SIF_POS;
	GetScrollInfo(hwnd, SB_HORZ, &scrollInfo);
	mapLeftRoom = (SInt16)scrollInfo.nPos;
	GetScrollInfo(hwnd, SB_VERT, &scrollInfo);
	mapTopRoom = (SInt16)scrollInfo.nPos;

	if (mapRoomsWide != wasMapRoomsWide || mapRoomsHigh != wasMapRoomsHigh)
	{
		InvalidateRect(hwnd, NULL, TRUE);
	}
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

#ifndef COMPILEDEMO
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

	case WM_SIZING:
		HandleMapSizingMessage(hwnd, wParam, (RECT *)lParam);
		return TRUE;

	case WM_SIZE:
		HandleMapSizeMessage(hwnd);
		return 0;

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
#endif

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
	Point clickPoint;
	Rect aRoom;

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

		clickPoint.h = clickX;
		clickPoint.v = clickY;
		QSetRect(&aRoom, 0, 0, kMapRoomWidth, kMapRoomHeight);
		CenterRectOnPoint(&aRoom, clickPoint);
		DragRoom(clickX, clickY, &aRoom, itsNumber);
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
	if (nailSrcMap == NULL)
	{
		QSetRect(&nailSrcRect, 0, 0, kMapRoomWidth, kMapRoomHeight * (kNumBackgrounds + 1));
		nailSrcMap = CreateOffScreenGWorld(&nailSrcRect, kPreferredDepth);
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
	SInt16 localH;
	SInt16 localV;
	SInt16 roomH;
	SInt16 roomV;
	SInt16 itsNumber;
	SInt16 srcFloor;
	SInt16 srcSuite;
	SInt16 packedSrcCombo;
	SInt16 destFloor;
	SInt16 destSuite;
	SInt16 packedDestCombo;
	objectType *theObject;
	SInt16 r;
	SInt16 i;

	localH = wherePt.h / kMapRoomWidth;
	localV = wherePt.v / kMapRoomHeight;

	if ((localH >= mapRoomsWide) || (localV >= mapRoomsHigh))
		return;

	roomH = localH + mapLeftRoom;
	roomV = kMapGroundValue - (localV + mapTopRoom);

	if (RoomExists(roomH, roomV, &itsNumber) == false)
	{
		srcFloor = thisRoom->floor;
		srcSuite = thisRoom->suite;
		packedSrcCombo = MergeFloorSuite(srcFloor + kNumUndergroundFloors, srcSuite);
		destFloor = roomV;
		destSuite = roomH;
		packedDestCombo = MergeFloorSuite(destFloor + kNumUndergroundFloors, destSuite);

		thisRoom->floor = destFloor;
		thisRoom->suite = destSuite;

		// fix up links within this room
		for (i = 0; i < kMaxRoomObs; i++)
		{
			theObject = &thisRoom->objects[i];
			if (ObjectIsLinkSwitch(theObject))
			{
				if (theObject->data.e.where == packedSrcCombo)
				{
					theObject->data.e.where = packedDestCombo;
				}
			}
			else if (ObjectIsLinkTransport(theObject))
			{
				if (theObject->data.d.where == packedSrcCombo)
				{
					theObject->data.d.where = packedDestCombo;
				}
			}
		}

		// fix up links from other rooms to this room
		for (r = 0; r < thisHouse.nRooms; r++)
		{
			for (i = 0; i < kMaxRoomObs; i++)
			{
				theObject = &thisHouse.rooms[r].objects[i];
				if (ObjectIsLinkSwitch(theObject))
				{
					if (theObject->data.e.where == packedSrcCombo)
					{
						theObject->data.e.where = packedDestCombo;
					}
				}
				else if (ObjectIsLinkTransport(theObject))
				{
					if (theObject->data.d.where == packedSrcCombo)
					{
						theObject->data.d.where = packedDestCombo;
					}
				}
			}
		}

		CopyThisRoomToRoom();
		ReflectCurrentRoom(false);
		fileDirty = true;
		UpdateMenus(false);
		UpdateMapWindow();
	}
}

