//============================================================================
//----------------------------------------------------------------------------
//                                   Map.c
//----------------------------------------------------------------------------
//============================================================================

#include "Map.h"

#include "ColorUtils.h"
#include "DialogUtils.h"
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
#include "ResourceLoader.h"
#include "Room.h"
#include "Scrap.h"
#include "Utilities.h"

#define kMapRoomsHigh           9   // was 7
#define kMapRoomsWide           9   // was 7
#define kMapGroundValue         56
#define kYesDoNewRoom           IDOK
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

HWND g_mapWindow;
SInt16 g_isMapH;
SInt16 g_isMapV;
SInt16 g_mapRoomsWide;
SInt16 g_mapRoomsHigh;
SInt16 g_mapLeftRoom;
SInt16 g_mapTopRoom;
Boolean g_isMapOpen;
Boolean g_doPrettyMap;

static Rect g_nailSrcRect;
static HDC g_nailSrcMap = NULL;
static Rect g_activeRoomRect;
static Rect g_wasActiveRoomRect;

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
		wcx.hbrBackground = NULL;
		wcx.lpszMenuName = NULL;
		wcx.lpszClassName = WC_MAPWINDOW;
		wcx.hIconSm = NULL;
		if (!RegisterClassEx(&wcx))
			RedAlert(kErrUnnaccounted);
	}
}

//--------------------------------------------------------------  ThisRoomVisibleOnMap

Boolean ThisRoomVisibleOnMap (void)
{
	SInt16 h, v;

	h = g_thisRoom->suite;
	v = kMapGroundValue - g_thisRoom->floor;

	if ((h < g_mapLeftRoom) || (v < g_mapTopRoom) ||
			(h >= (g_mapLeftRoom + g_mapRoomsWide)) ||
			(v >= (g_mapTopRoom + g_mapRoomsHigh)))
		return (false);
	else
		return (true);
}

//--------------------------------------------------------------  CenterMapOnRoom

void CenterMapOnRoom (SInt16 h, SInt16 v)
{
	SCROLLINFO scrollInfo;

	if (g_mapWindow == NULL)
		return;

	g_mapLeftRoom = h - (g_mapRoomsWide / 2);
	g_mapTopRoom = (kMapGroundValue - v) - (g_mapRoomsHigh / 2);

	if (g_mapLeftRoom < 0)
		g_mapLeftRoom = 0;
	else if (g_mapLeftRoom > (kMaxNumRoomsH - g_mapRoomsWide))
		g_mapLeftRoom = kMaxNumRoomsH - g_mapRoomsWide;

	if (g_mapTopRoom < 0)
		g_mapTopRoom = 0;
	else if (g_mapTopRoom > (kMaxNumRoomsV - g_mapRoomsHigh))
		g_mapTopRoom = kMaxNumRoomsV - g_mapRoomsHigh;

	if (g_mapWindow != NULL)
	{
		scrollInfo.cbSize = sizeof(scrollInfo);
		scrollInfo.fMask = SIF_POS;

		scrollInfo.nPos = g_mapLeftRoom;
		SetScrollInfo(g_mapWindow, SB_HORZ, &scrollInfo, TRUE);
		scrollInfo.nPos = g_mapTopRoom;
		SetScrollInfo(g_mapWindow, SB_VERT, &scrollInfo, TRUE);
	}
}

//--------------------------------------------------------------  FlagMapRoomsForUpdate

void FlagMapRoomsForUpdate (void)
{
	if (g_mapWindow == NULL)
		return;

	Mac_InvalWindowRect(g_mapWindow, &g_wasActiveRoomRect);
	Mac_InvalWindowRect(g_mapWindow, &g_activeRoomRect);
}

//--------------------------------------------------------------  FindNewActiveRoomRect

void FindNewActiveRoomRect (void)
{
	Rect aRoom;
	SInt16 h, i;
	SInt16 floor, suite, whoCares;
	Boolean activeRoomVisible;

	if (g_mapWindow == nil)
		return;

	activeRoomVisible = false;

	for (i = 0; i < g_mapRoomsHigh; i++)
	{
		for (h = 0; h < g_mapRoomsWide; h++)
		{
			QSetRect(&aRoom, 0, 0, kMapRoomWidth, kMapRoomHeight);
			QOffsetRect(&aRoom, kMapRoomWidth * h, kMapRoomHeight * i);

			suite = h + g_mapLeftRoom;
			floor = kMapGroundValue - (i + g_mapTopRoom);
			if ((RoomExists(suite, floor, &whoCares)) && (g_houseUnlocked))
			{
				if (whoCares == g_thisRoomNumber)
				{
					g_wasActiveRoomRect = g_activeRoomRect;
					g_activeRoomRect = aRoom;
					activeRoomVisible = true;
				}
			}
		}
	}

	if (activeRoomVisible)
	{
		g_activeRoomRect.right++;
		g_activeRoomRect.bottom++;
		QInsetRect(&g_activeRoomRect, -1, -1);
	}
}

//--------------------------------------------------------------  DrawMapRoomImage

void DrawMapRoomImage (HDC hdc, SInt16 backgroundID, const Rect *dstRect)
{
	SInt16 type;
	HBITMAP hBitmap;
	Rect srcRect;

	type = backgroundID - kBaseBackgroundID;
	hBitmap = NULL;
	if (type < 0 || type >= kNumBackgrounds)
	{
		type = kNumBackgrounds;
		if (g_doPrettyMap)
		{
			hBitmap = Gp_LoadImage(g_theHouseFile, backgroundID);
		}
	}

	if (hBitmap != NULL)
	{
		Mac_DrawPicture(hdc, hBitmap, dstRect);
		DeleteObject(hBitmap);
	}
	else
	{
		QSetRect(&srcRect, 0, 0, kMapRoomWidth, kMapRoomHeight);
		QOffsetRect(&srcRect, 0, type * kMapRoomHeight);
		Mac_CopyBits(g_nailSrcMap, hdc, &srcRect, dstRect, srcCopy, nil);
	}
}

//--------------------------------------------------------------  RedrawMapContents

void RedrawMapContents (HDC hdc)
{
	Rect aRoom;
	SInt16 h, i, groundLevel;
	SInt16 floor, suite, roomNum;
	Boolean activeRoomVisible;
	HBITMAP ditherBitmap;
	HBRUSH ditherBrush;
	HRGN skyRgn, groundRgn, tempRgn;
	COLORREF wasBkColor, wasTextColor;

	if (COMPILEDEMO)
		return;

	if (g_mapWindow == NULL || hdc == NULL)
		return;

	SaveDC(hdc);
	SetStretchBltMode(hdc, HALFTONE);
	SetBrushOrgEx(hdc, 0, 0, NULL);

	skyRgn = CreateRectRgn(0, 0, 0, 0);
	groundRgn = CreateRectRgn(0, 0, 0, 0);
	tempRgn = CreateRectRgn(0, 0, 0, 0);

	activeRoomVisible = false;
	groundLevel = kMapGroundValue - g_mapTopRoom;

	for (i = 0; i < g_mapRoomsHigh; i++)
	{
		for (h = 0; h < g_mapRoomsWide; h++)
		{
			QSetRect(&aRoom, 0, 0, kMapRoomWidth, kMapRoomHeight);
			QOffsetRect(&aRoom, kMapRoomWidth * h, kMapRoomHeight * i);

			suite = h + g_mapLeftRoom;
			floor = kMapGroundValue - (i + g_mapTopRoom);
			if ((RoomExists(suite, floor, &roomNum)) && (g_houseUnlocked))
			{
				DrawMapRoomImage(hdc, g_thisHouse.rooms[roomNum].background, &aRoom);
				if (roomNum == g_thisRoomNumber)
				{
					g_activeRoomRect = aRoom;
					activeRoomVisible = true;
				}
			}
			else
			{
				SetRectRgn(tempRgn, aRoom.left, aRoom.top, aRoom.right, aRoom.bottom);
				if (i >= groundLevel)
				{
					CombineRgn(groundRgn, groundRgn, tempRgn, RGN_OR);
				}
				else
				{
					CombineRgn(skyRgn, skyRgn, tempRgn, RGN_OR);
				}
			}
		}
	}

	ditherBitmap = CreateShadowBitmap();
	ditherBrush = CreatePatternBrush(ditherBitmap);
	wasBkColor = SetBkColor(hdc, whiteColor);
	wasTextColor = SetTextColor(hdc, blueColor);
	FillRgn(hdc, skyRgn, ditherBrush);  // draw the blue sky
	SetBkColor(hdc, whiteColor);
	SetTextColor(hdc, greenColor);
	FillRgn(hdc, groundRgn, ditherBrush);  // draw the green ground
	SetBkColor(hdc, wasBkColor);
	SetTextColor(hdc, wasTextColor);
	DeleteObject(ditherBrush);
	DeleteObject(ditherBitmap);

	DeleteObject(tempRgn);
	DeleteObject(groundRgn);
	DeleteObject(skyRgn);

	SelectObject(hdc, GetStockObject(BLACK_PEN));

	for (i = 1; i < g_mapRoomsWide; i++)
	{
		MoveToEx(hdc, i * kMapRoomWidth, 0, NULL);
		Mac_Line(hdc, 0, g_mapRoomsHigh * kMapRoomHeight);
	}

	for (i = 1; i < g_mapRoomsHigh; i++)
	{
		MoveToEx(hdc, 0, i * kMapRoomHeight, NULL);
		Mac_Line(hdc, g_mapRoomsWide * kMapRoomWidth, 0);
	}

	if (activeRoomVisible)
	{
		SetDCBrushColor(hdc, redColor);
		g_activeRoomRect.right++;
		g_activeRoomRect.bottom++;
		Mac_FrameRect(hdc, &g_activeRoomRect, (HBRUSH)GetStockObject(DC_BRUSH), 1, 1);
		QInsetRect(&g_activeRoomRect, 1, 1);
		Mac_FrameRect(hdc, &g_activeRoomRect, (HBRUSH)GetStockObject(DC_BRUSH), 1, 1);
		QInsetRect(&g_activeRoomRect, -1, -1);
	}

	RestoreDC(hdc, -1);
}

//--------------------------------------------------------------  UpdateMapWindow

void UpdateMapWindow (void)
{
	SCROLLINFO scrollInfo;

	if (COMPILEDEMO)
		return;

	if (g_mapWindow == NULL)
		return;

	scrollInfo.cbSize = sizeof(scrollInfo);

	scrollInfo.fMask = SIF_POS;
	scrollInfo.nPos = g_mapLeftRoom;
	SetScrollInfo(g_mapWindow, SB_HORZ, &scrollInfo, TRUE);
	scrollInfo.nPos = g_mapTopRoom;
	SetScrollInfo(g_mapWindow, SB_VERT, &scrollInfo, TRUE);

	RedrawWindow(g_mapWindow, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
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

	wasMapRoomsWide = g_mapRoomsWide;
	wasMapRoomsHigh = g_mapRoomsHigh;
	g_mapRoomsWide = (SInt16)(clientWidth / kMapRoomWidth);
	g_mapRoomsHigh = (SInt16)(clientHeight / kMapRoomHeight);

	scrollInfo.cbSize = sizeof(scrollInfo);

	scrollInfo.fMask = SIF_PAGE | SIF_DISABLENOSCROLL;
	scrollInfo.nPage = g_mapRoomsWide;
	SetScrollInfo(hwnd, SB_HORZ, &scrollInfo, TRUE);
	scrollInfo.nPage = g_mapRoomsHigh;
	SetScrollInfo(hwnd, SB_VERT, &scrollInfo, TRUE);

	scrollInfo.fMask = SIF_POS;
	GetScrollInfo(hwnd, SB_HORZ, &scrollInfo);
	g_mapLeftRoom = (SInt16)scrollInfo.nPos;
	GetScrollInfo(hwnd, SB_VERT, &scrollInfo);
	g_mapTopRoom = (SInt16)scrollInfo.nPos;

	if (g_mapRoomsWide != wasMapRoomsWide || g_mapRoomsHigh != wasMapRoomsHigh)
	{
		InvalidateRect(hwnd, NULL, TRUE);
	}
}

//--------------------------------------------------------------  OpenMapWindow

void OpenMapWindow (void)
{
	RECT windowRect;
	DWORD windowStyle, extendedStyle;
	WINDOWPLACEMENT placement;
	SCROLLINFO scrollInfo;

	if (COMPILEDEMO)
		return;

	if (g_mapWindow == NULL)
	{
		RegisterMapWindowClass();
		CreateNailOffscreen();

		windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
			WS_HSCROLL | WS_VSCROLL;
		extendedStyle = WS_EX_DLGMODALFRAME;
		SetRect(&windowRect, 0, 0,
			g_mapRoomsWide * kMapRoomWidth + GetSystemMetrics(SM_CXVSCROLL),
			g_mapRoomsHigh * kMapRoomHeight + GetSystemMetrics(SM_CYHSCROLL));
		AdjustWindowRectEx(&windowRect, windowStyle, FALSE, extendedStyle);
		g_mapWindow = CreateWindowEx(extendedStyle, WC_MAPWINDOW, L"Map",
			windowStyle, 0, 0, 0, 0, g_mainWindow, NULL, HINST_THISCOMPONENT, NULL);

		if (g_mapWindow == NULL)
			RedAlert(kErrNoMemory);

#if 0
		if (OptionKeyDown())
		{
			g_isMapH = 3;
			g_isMapV = qd.screenBits.bounds.bottom - 100;
		}
#endif
		placement.length = sizeof(placement);
		GetWindowPlacement(g_mapWindow, &placement);
		placement.flags = 0;
		OffsetRect(&windowRect, -windowRect.left, -windowRect.top);
		OffsetRect(&windowRect, g_isMapH, g_isMapV);
		placement.rcNormalPosition = windowRect;
		placement.showCmd = SW_SHOWNOACTIVATE;
		SetWindowPlacement(g_mapWindow, &placement);

		QSetRect(&g_wasActiveRoomRect, 0, 0, 1, 1);
		QSetRect(&g_activeRoomRect, 0, 0, 1, 1);

		scrollInfo.cbSize = sizeof(scrollInfo);
		scrollInfo.fMask = SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL;

		scrollInfo.nMin = 0;
		scrollInfo.nMax = kMaxNumRoomsH - 1;
		scrollInfo.nPage = g_mapRoomsWide;
		SetScrollInfo(g_mapWindow, SB_HORZ, &scrollInfo, TRUE);

		scrollInfo.nMin = 0;
		scrollInfo.nMax = kMaxNumRoomsV - 1;
		scrollInfo.nPage = g_mapRoomsHigh;
		SetScrollInfo(g_mapWindow, SB_VERT, &scrollInfo, TRUE);

		CenterMapOnRoom(g_thisRoom->suite, g_thisRoom->floor);
	}

	UpdateMapCheckmark(true);
}

//--------------------------------------------------------------  CloseMapWindow

void CloseMapWindow (void)
{
	if (COMPILEDEMO)
		return;

	if (g_mapWindow != NULL)
	{
		DestroyWindow(g_mapWindow);
		g_mapWindow = NULL;
	}
	UpdateMapCheckmark(false);
}

//--------------------------------------------------------------  ToggleMapWindow

void ToggleMapWindow (void)
{
	if (COMPILEDEMO)
		return;

	if (g_mapWindow == NULL)
	{
		OpenMapWindow();
		g_isMapOpen = true;
	}
	else
	{
		CloseMapWindow();
		g_isMapOpen = false;
	}
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
		g_isMapH = (SInt16)placement.rcNormalPosition.left;
		g_isMapV = (SInt16)placement.rcNormalPosition.top;
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

//--------------------------------------------------------------  LiveHScrollAction

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
		newValue = wasValue - (g_mapRoomsWide / 2);
		break;
	case SB_PAGERIGHT:
		newValue = wasValue + (g_mapRoomsWide / 2);
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
		g_mapLeftRoom = (SInt16)scrollInfo.nPos;
		UpdateMapWindow();
	}
}

//--------------------------------------------------------------  LiveVScrollAction

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
		newValue = wasValue - (g_mapRoomsHigh / 2);
		break;
	case SB_PAGEDOWN:
		newValue = wasValue + (g_mapRoomsHigh / 2);
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
		g_mapTopRoom = (SInt16)scrollInfo.nPos;
		UpdateMapWindow();
	}
}

//--------------------------------------------------------------  HandleMapClick

void HandleMapClick (SInt16 clickX, SInt16 clickY)
{
	SInt16 localH, localV;
	SInt16 roomH, roomV, itsNumber;
	Point clickPoint;
	Rect aRoom;

	if (COMPILEDEMO)
		return;

	localH = clickX / kMapRoomWidth;
	localV = clickY / kMapRoomHeight;

	if ((localH >= g_mapRoomsWide) || (localV >= g_mapRoomsHigh))
		return;
	if ((localH < 0) || (localV < 0))
		return;

	roomH = localH + g_mapLeftRoom;
	roomV = kMapGroundValue - (localV + g_mapTopRoom);

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
		if (g_doBitchDialogs)
		{
			if (QueryNewRoom(g_mainWindow))
			{
				if (!CreateNewRoom(g_mainWindow, roomH, roomV))
				{
					YellowAlert(g_mainWindow, kYellowUnaccounted, 11);
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
			if (!CreateNewRoom(g_mainWindow, roomH, roomV))
			{
				YellowAlert(g_mainWindow, kYellowUnaccounted, 11);
				return;
			}
			else
			{
				DeselectObject();
				ReflectCurrentRoom(false);
			}
		}
	}
}

//--------------------------------------------------------------  QueryNewRoom

Boolean QueryNewRoom (HWND ownerWindow)
{
	SInt16 hitWhat;

	hitWhat = Alert(kNewRoomAlert, ownerWindow, NULL);
	if (hitWhat == kYesDoNewRoom)
		return (true);
	else
		return (false);
}

//--------------------------------------------------------------  CreateNailOffscreen

void CreateNailOffscreen (void)
{
	if (g_nailSrcMap == NULL)
	{
		QSetRect(&g_nailSrcRect, 0, 0, kMapRoomWidth, kMapRoomHeight * (kNumBackgrounds + 1));
		g_nailSrcMap = CreateOffScreenGWorld(&g_nailSrcRect, kPreferredDepth);
		LoadGraphic(g_nailSrcMap, g_theHouseFile, kThumbnailPictID);
	}
}

//--------------------------------------------------------------  KillNailOffscreen

void KillNailOffscreen (void)
{
	if (g_nailSrcMap != NULL)
	{
		DisposeGWorld(g_nailSrcMap);
		g_nailSrcMap = NULL;
	}
}

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

	if ((localH >= g_mapRoomsWide) || (localV >= g_mapRoomsHigh))
		return;

	roomH = localH + g_mapLeftRoom;
	roomV = kMapGroundValue - (localV + g_mapTopRoom);

	if (RoomExists(roomH, roomV, &itsNumber) == false)
	{
		srcFloor = g_thisRoom->floor;
		srcSuite = g_thisRoom->suite;
		packedSrcCombo = MergeFloorSuite(srcFloor, srcSuite);
		destFloor = roomV;
		destSuite = roomH;
		packedDestCombo = MergeFloorSuite(destFloor, destSuite);

		g_thisRoom->floor = destFloor;
		g_thisRoom->suite = destSuite;

		// fix up links within this room
		for (i = 0; i < kMaxRoomObs; i++)
		{
			theObject = &g_thisRoom->objects[i];
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
		for (r = 0; r < g_thisHouse.nRooms; r++)
		{
			for (i = 0; i < kMaxRoomObs; i++)
			{
				theObject = &g_thisHouse.rooms[r].objects[i];
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
		g_fileDirty = true;
		UpdateMenus(false);
		UpdateMapWindow();
	}
}
