//============================================================================
//----------------------------------------------------------------------------
//                                 RoomInfo.c
//----------------------------------------------------------------------------
//============================================================================

#include "RoomInfo.h"

#include "DialogUtils.h"
#include "GliderDefines.h"
#include "House.h"
#include "HouseIO.h"
#include "Macintosh.h"
#include "Menu.h"
#include "RectUtils.h"
#include "Room.h"
#include "ResourceIDs.h"
#include "ResourceLoader.h"
#include "StringUtils.h"
#include "Utilities.h"

#define kMiniTileWide               16

void LoadTileSrcGraphic (HDC hdc, SInt16 backID, const Rect *theRect);
void UpdateRoomInfoDialog (HWND hDlg, HDC hdc);
void DrawTileOverMarkings (
	HDC hdcDest,
	SInt16 whichTile,
	const Rect *tileRect,
	COLORREF hiliteColor);
void DragMiniTile (HWND hDlg, Point mouseIs, SInt16 *newTileOver);
void HiliteTileOver (HWND hDlg, Point mouseIs);
void RoomInfo_InitDialog (HWND hDlg);
INT_PTR CALLBACK RoomFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
SInt16 ChooseOriginalArt (HWND hwndOwner, SInt16 wasPictID);
LRESULT SendButtonClickedCommand (HWND hDlg, int ctrlID);
INT_PTR CALLBACK OriginalArtFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
Boolean PictIDExists (SInt16 theID);
SInt16 GetFirstPICT (void);
void BitchAboutPICTNotFound (HWND ownerWindow);

Rect g_tileSrcRect;
HDC g_tileSrcMap;

static SInt16 g_tempTiles[kNumTiles];
static Rect g_tileSrc;
static Rect g_tileDest;
static SInt16 g_tileOver;
static SInt16 g_tempBack;
static Boolean g_showHandCursor = false;

//==============================================================  Functions
//--------------------------------------------------------------  LoadTileSrcGraphic

void LoadTileSrcGraphic (HDC hdc, SInt16 backID, const Rect *theRect)
{
	int hdcState;

	hdcState = SaveDC(hdc);
	SetStretchBltMode(hdc, HALFTONE);
	SetBrushOrgEx(hdc, 0, 0, NULL);

	switch (backID)
	{
	case kBasement:
		LoadScaledGraphic(hdc, g_theHouseFile, kBasementTileSrc, theRect);
		break;

	case kDirt:
		LoadScaledGraphic(hdc, g_theHouseFile, kDirtTileSrc, theRect);
		break;

	case kStratosphere:
		LoadScaledGraphic(hdc, g_theHouseFile, kStratosphereTileSrc, theRect);
		break;

	case kStars:
		LoadScaledGraphic(hdc, g_theHouseFile, kStarsTileSrc, theRect);
		break;

	default:
		LoadScaledGraphic(hdc, g_theHouseFile, backID, theRect);
		break;
	}

	RestoreDC(hdc, hdcState);
}

//--------------------------------------------------------------  UpdateRoomInfoDialog

void UpdateRoomInfoDialog (HWND hDlg, HDC hdc)
{
	Rect src, dest;
	SInt16 i;

	Mac_CopyBits(g_tileSrcMap, hdc, &g_tileSrcRect, &g_tileSrc, srcCopy, nil);

	dest = g_tileDest;
	dest.right = dest.left + kMiniTileWide;
	for (i = 0; i < kNumTiles; i++)
	{
		QSetRect(&src, 0, 0, kMiniTileWide, 80);
		HOffsetRect(&src, g_tempTiles[i] * kMiniTileWide);
		Mac_CopyBits(g_tileSrcMap, hdc, &src, &dest, srcCopy, nil);
		HOffsetRect(&dest, kMiniTileWide);
	}

	if (GetNumberOfLights(g_thisRoomNumber) == 0)
		SetDlgItemText(hDlg, kLitUnlitText, L"(Room Is Dark)");
	else
		SetDlgItemText(hDlg, kLitUnlitText, L"(Room Is Lit)");

	Mac_FrameRect(hdc, &g_tileSrc, (HBRUSH)GetStockObject(BLACK_BRUSH), 1, 1);
	Mac_FrameRect(hdc, &g_tileDest, (HBRUSH)GetStockObject(BLACK_BRUSH), 1, 1);
}

//--------------------------------------------------------------  DrawTileOverMarkings

void DrawTileOverMarkings (
	HDC hdcDest,
	SInt16 whichTile,
	const Rect *tileRect,
	COLORREF hiliteColor)
{
	HGDIOBJ prevBrush;
	COLORREF prevBrushColor;
	int markLeft;

	prevBrush = SelectObject(hdcDest, GetStockObject(DC_BRUSH));
	prevBrushColor = SetDCBrushColor(hdcDest, hiliteColor);
	markLeft = tileRect->left + (whichTile * kMiniTileWide);
	PatBlt(hdcDest, markLeft, tileRect->top - 3, kMiniTileWide, 2, PATCOPY);
	PatBlt(hdcDest, markLeft, tileRect->bottom + 1, kMiniTileWide, 2, PATCOPY);
	SetDCBrushColor(hdcDest, prevBrushColor);
	SelectObject(hdcDest, prevBrush);
}

//--------------------------------------------------------------  DragMiniTile

void DragMiniTile (HWND hDlg, Point mouseIs, SInt16 *newTileOver)
{
	HCURSOR dragCursor = NULL;
	HCURSOR oldCursor = NULL;
	RECT dragRect;
	Point mouseWas;
	SInt16 wasTileOver;
	COLORREF btnFaceColor;
	HDC hdc;
	MSG msg;

	SetCapture(hDlg);

	dragCursor = LoadCursor(NULL, IDC_SIZEALL);
	if (dragCursor != NULL)
	{
		oldCursor = SetCursor(dragCursor);
	}

	g_tileOver = (mouseIs.h - g_tileSrc.left) / kMiniTileWide;
	wasTileOver = -1;
	*newTileOver = -1;
	SetRect(&dragRect, 0, 0, kMiniTileWide, 80);
	OffsetRect(&dragRect,
		g_tileSrc.left + (g_tileOver * kMiniTileWide),
		g_tileSrc.top);

	hdc = GetDC(hDlg);
	DrawFocusRect(hdc, &dragRect);
	ReleaseDC(hDlg, hdc);
	mouseWas = mouseIs;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			mouseIs.h = GET_X_LPARAM(msg.lParam);
			mouseIs.v = GET_Y_LPARAM(msg.lParam);
			hdc = GetDC(hDlg);

			DrawFocusRect(hdc, &dragRect);
			OffsetRect(&dragRect, mouseIs.h - mouseWas.h, 0);
			DrawFocusRect(hdc, &dragRect);

			btnFaceColor = GetSysColor(COLOR_BTNFACE);

			// is cursor in the drop rect?
			if (QPtInRect(mouseIs, &g_tileDest))
			{
				*newTileOver = (mouseIs.h - g_tileDest.left) / kMiniTileWide;
				if (*newTileOver != wasTileOver)
				{
					DrawTileOverMarkings(hdc, *newTileOver, &g_tileDest, blueColor);
					if (wasTileOver != -1)
					{
						DrawTileOverMarkings(hdc, wasTileOver, &g_tileDest, btnFaceColor);
					}
					wasTileOver = *newTileOver;
				}
			}
			else
			{
				// we're not in the drop zone
				*newTileOver = -1;
				if (wasTileOver != -1)
				{
					DrawTileOverMarkings(hdc, wasTileOver, &g_tileDest, btnFaceColor);
					wasTileOver = -1;
				}
			}

			ReleaseDC(hDlg, hdc);
			mouseWas = mouseIs;
			break;

		case WM_LBUTTONUP:
			if (GetCapture() == hDlg)
				ReleaseCapture();
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			// ignore these events during tile drag
			break;

		default:
			if (!IsDialogMessage(hDlg, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			break;
		}
		if (GetCapture() != hDlg)
			break;
	}
	if (msg.message == WM_QUIT)
	{
		PostQuitMessage((int)msg.wParam);
		if (GetCapture() == hDlg)
			ReleaseCapture();
	}

	hdc = GetDC(hDlg);
	if (wasTileOver != -1)
	{
		btnFaceColor = GetSysColor(COLOR_BTNFACE);
		DrawTileOverMarkings(hdc, wasTileOver, &g_tileDest, btnFaceColor);
		wasTileOver = -1;
	}
	DrawFocusRect(hdc, &dragRect);
	ReleaseDC(hDlg, hdc);

	if (dragCursor != NULL)
	{
		SetCursor(oldCursor);
	}
}

//--------------------------------------------------------------  HiliteTileOver

void HiliteTileOver (HWND hDlg, Point mouseIs)
{
	COLORREF btnFaceColor;
	SInt16 newTileOver;
	HDC hdc;

	btnFaceColor = GetSysColor(COLOR_BTNFACE);
	if (QPtInRect(mouseIs, &g_tileSrc))
	{
		newTileOver = (mouseIs.h - g_tileSrc.left) / kMiniTileWide;
		if (newTileOver != g_tileOver)
		{
			hdc = GetDC(hDlg);
			DrawTileOverMarkings(hdc, newTileOver, &g_tileSrc, redColor);
			if (g_tileOver != -1)
			{
				DrawTileOverMarkings(hdc, g_tileOver, &g_tileSrc, btnFaceColor);
			}
			ReleaseDC(hDlg, hdc);
			g_tileOver = newTileOver;
		}
	}
	else
	{
		if (g_tileOver != -1)
		{
			hdc = GetDC(hDlg);
			DrawTileOverMarkings(hdc, g_tileOver, &g_tileSrc, btnFaceColor);
			ReleaseDC(hDlg, hdc);
			g_tileOver = -1;
		}
	}
}

//--------------------------------------------------------------  RoomInfo_InitDialog

void RoomInfo_InitDialog (HWND hDlg)
{
	HMENU rootMenu, backgroundsMenu;
	MENUITEMINFO menuItem;
	DialogParams params = { 0 };
	wchar_t floorStr[16];
	wchar_t suiteStr[16];
	wchar_t objectsStr[16];
	Rect tileBoxRect;
	SInt16 i;

	CenterDialogOverOwner(hDlg);

	g_showHandCursor = false;
	g_tileOver = -1;
	g_tempBack = g_thisRoom->background;

	rootMenu = LoadMenu(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDM_ROOT));
	if (rootMenu == NULL)
	{
		RedAlert(kErrFailedResourceLoad);
	}
	menuItem.cbSize = sizeof(menuItem);
	menuItem.fMask = MIIM_SUBMENU;
	if (!GetMenuItemInfo(rootMenu, kBackgroundsMenuID, FALSE, &menuItem))
	{
		RedAlert(kErrFailedResourceLoad);
	}
	backgroundsMenu = menuItem.hSubMenu;
	if (GetFirstPICT() == -1)
	{
		DeleteMenu(backgroundsMenu, kUserBackground, MF_BYCOMMAND);
	}
	AddMenuToComboBox(hDlg, kRoomPopupItem, backgroundsMenu);
	DestroyMenu(rootMenu);

	NumToString(g_thisRoom->floor, floorStr, ARRAYSIZE(floorStr));
	NumToString(g_thisRoom->suite, suiteStr, ARRAYSIZE(suiteStr));
	NumToString(g_thisRoom->numObjects, objectsStr, ARRAYSIZE(objectsStr));

	params.arg[0] = floorStr;
	params.arg[1] = suiteStr;
	params.arg[2] = objectsStr;
	ParamDialogText(hDlg, &params);

	g_tileSrcMap = CreateOffScreenGWorld(&g_tileSrcRect, kPreferredDepth);
	if ((g_tempBack > kStars) && (!PictIDExists(g_tempBack)))
	{
		BitchAboutPICTNotFound(hDlg);
		g_tempBack = kSimpleRoom;
	}
	LoadTileSrcGraphic(g_tileSrcMap, g_tempBack, &g_tileSrcRect);

	for (i = 0; i < kNumTiles; i++)
		g_tempTiles[i] = g_thisRoom->tiles[i];

	if (g_tempBack >= kUserBackground)
		SetComboBoxMenuValue(hDlg, kRoomPopupItem, kUserBackground);
	else
		SetComboBoxMenuValue(hDlg, kRoomPopupItem, g_tempBack);

	SetDialogString(hDlg, kRoomNameItem, g_thisRoom->name);

	QSetRect(&g_tileSrc, 0, 0, 128, 80);
	GetDialogItemRect(hDlg, kRoomTilesBox, &tileBoxRect);
	CenterRectInRect(&g_tileSrc, &tileBoxRect);

	QSetRect(&g_tileDest, 0, 0, 128, 80);
	GetDialogItemRect(hDlg, kRoomTilesBox2, &tileBoxRect);
	CenterRectInRect(&g_tileDest, &tileBoxRect);

	if (g_thisHouse.firstRoom == g_thisRoomNumber)
		CheckDlgButton(hDlg, kRoomFirstCheck, BST_CHECKED);
	else
		CheckDlgButton(hDlg, kRoomFirstCheck, BST_UNCHECKED);

	if (g_tempBack >= kUserBackground)
		EnableWindow(GetDlgItem(hDlg, kBoundsButton), TRUE);
	else
		EnableWindow(GetDlgItem(hDlg, kBoundsButton), FALSE);
}

//--------------------------------------------------------------  RoomFilter

INT_PTR CALLBACK RoomFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	Point mouseIs;
	SInt16 newBack, i;
	Boolean forceDraw;

	switch (message)
	{
	case WM_INITDIALOG:
		RoomInfo_InitDialog(hDlg);
		return TRUE;

	case WM_DESTROY:
		DisposeGWorld(g_tileSrcMap);
		g_tileSrcMap = NULL;
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			for (i = 0; i < kNumTiles; i++)
				g_thisRoom->tiles[i] = g_tempTiles[i];

			GetDialogString(hDlg, kRoomNameItem,
				g_thisRoom->name, ARRAYSIZE(g_thisRoom->name));
			if (IsDlgButtonChecked(hDlg, kRoomFirstCheck))
				g_thisHouse.firstRoom = g_thisRoomNumber;

			g_thisRoom->background = g_tempBack;
			if (g_tempBack < kUserBackground)
				g_lastBackground = g_tempBack;

			CopyThisRoomToRoom();
			ReflectCurrentRoom(false);
			g_fileDirty = true;
			UpdateMenus(false);
			EndDialog(hDlg, IDOK);
			break;

		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;

		case kRoomPopupItem:
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				GetComboBoxMenuValue(hDlg, kRoomPopupItem, &newBack);
				// original art item selected?
				if (newBack == kUserBackground)
				{
					// was previous bg built-in?
					if (g_tempBack < kUserBackground)
					{
						// then assign 1st PICT
						g_tempBack = GetFirstPICT();
						forceDraw = true;
					}
					else
					{
						forceDraw = false;
					}
					newBack = ChooseOriginalArt(hDlg, g_tempBack);
					if ((g_tempBack != newBack) || (forceDraw))
					{
						g_tempBack = newBack;
						LoadTileSrcGraphic(g_tileSrcMap, g_tempBack, &g_tileSrcRect);
						Mac_InvalWindowRect(hDlg, &g_tileSrc);
						Mac_InvalWindowRect(hDlg, &g_tileDest);
					}
				}
				else
				{
					// if background has changed
					if (newBack != g_tempBack)
						SetInitialTiles(newBack, g_tempTiles);
				}

				if (newBack >= kUserBackground)
				{
					EnableWindow(GetDlgItem(hDlg, kBoundsButton), TRUE);
					// if background has changed
					if (newBack != g_tempBack)
						SetInitialTiles(newBack, g_tempTiles);
				}
				else
				{
					EnableWindow(GetDlgItem(hDlg, kBoundsButton), FALSE);
				}

				if (newBack != g_tempBack)
				{
					g_tempBack = newBack;
					LoadTileSrcGraphic(g_tileSrcMap, g_tempBack, &g_tileSrcRect);
					Mac_InvalWindowRect(hDlg, &g_tileSrc);
					Mac_InvalWindowRect(hDlg, &g_tileDest);
				}
			}
			break;

		case kBoundsButton:
			newBack = ChooseOriginalArt(hDlg, g_tempBack);
			if (g_tempBack != newBack)
			{
				g_tempBack = newBack;
				LoadTileSrcGraphic(g_tileSrcMap, g_tempBack, &g_tileSrcRect);
				Mac_InvalWindowRect(hDlg, &g_tileSrc);
				Mac_InvalWindowRect(hDlg, &g_tileDest);
			}
			break;
		}
		return TRUE;

	case WM_SETCURSOR:
		if (g_showHandCursor)
		{
			HCURSOR handCursor;

			handCursor = LoadCursor(NULL, IDC_SIZEALL);
			SetCursor(handCursor);
			SetWindowLongPtr(hDlg, DWLP_MSGRESULT, TRUE);
			return TRUE;
		}
		return FALSE;

	case WM_LBUTTONDOWN:
		mouseIs.h = GET_X_LPARAM(lParam);
		mouseIs.v = GET_Y_LPARAM(lParam);
		if (QPtInRect(mouseIs, &g_tileSrc))
		{
			POINT pt;
			pt.x = mouseIs.h;
			pt.y = mouseIs.v;
			ClientToScreen(hDlg, &pt);
			if (DragDetect(hDlg, pt))
			{
				SInt16 newTileOver;

				DragMiniTile(hDlg, mouseIs, &newTileOver);
				if ((newTileOver >= 0) && (newTileOver < kNumTiles))
				{
					g_tempTiles[newTileOver] = g_tileOver;
					InvalidateRect(hDlg, NULL, TRUE);
				}
			}
		}
		return FALSE;

	case WM_MOUSEMOVE:
		mouseIs.h = GET_X_LPARAM(lParam);
		mouseIs.v = GET_Y_LPARAM(lParam);
		g_showHandCursor = QPtInRect(mouseIs, &g_tileSrc);
		HiliteTileOver(hDlg, mouseIs);
		return FALSE;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		if (BeginPaint(hDlg, &ps))
		{
			UpdateRoomInfoDialog(hDlg, ps.hdc);
			EndPaint(hDlg, &ps);
		}
		return TRUE;
	}
	}
	return FALSE;
}

//--------------------------------------------------------------  DoRoomInfo

void DoRoomInfo (HWND hwndOwner)
{
	if (COMPILEDEMO)
		return;

	DialogBox(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kRoomInfoDialogID),
		hwndOwner, RoomFilter);
}

//--------------------------------------------------------------  SendButtonClickedCommand

LRESULT SendButtonClickedCommand (HWND hDlg, int ctrlID)
{
	return SendMessage(hDlg, WM_COMMAND,
		MAKEWPARAM(ctrlID, BN_CLICKED),
		(LPARAM)GetDlgItem(hDlg, ctrlID));
}

//--------------------------------------------------------------  OriginalArtFilter

INT_PTR CALLBACK OriginalArtFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	SInt16 *pPictID;
	HWND hwndLine;
	SInt16 tempShort, tempID;

	pPictID = (SInt16 *)GetWindowLongPtr(hDlg, DWLP_USER);

	switch (message)
	{
	case WM_INITDIALOG:
		CenterDialogOverOwner(hDlg);
		SetWindowLongPtr(hDlg, DWLP_USER, lParam);
		pPictID = (SInt16 *)GetWindowLongPtr(hDlg, DWLP_USER);
		if (pPictID == NULL)
		{
			EndDialog(hDlg, IDCANCEL);
			return FALSE;
		}

		SetDlgItemInt(hDlg, kPICTIDItem, *pPictID, FALSE);

		// TODO: should version 1.0 house bounds be supported as well?
		tempShort = g_thisRoom->bounds >> 1; // version 2.0 house
		CheckDlgButton(hDlg, kLeftWallCheck, ((tempShort & 1) == 0));
		CheckDlgButton(hDlg, kTopWallCheck, ((tempShort & 2) == 0));
		CheckDlgButton(hDlg, kRightWallCheck, ((tempShort & 4) == 0));
		CheckDlgButton(hDlg, kBottomWallCheck, ((tempShort & 8) == 0));
		CheckDlgButton(hDlg, kFloorSupportCheck, ((tempShort & 16) == 16));

		// Synthesize BN_CLICKED command messages to show/hide the wall lines.
		// (This keeps that code in a single place: in the WM_COMMAND handler.)
		SendButtonClickedCommand(hDlg, kLeftWallCheck);
		SendButtonClickedCommand(hDlg, kTopWallCheck);
		SendButtonClickedCommand(hDlg, kRightWallCheck);
		SendButtonClickedCommand(hDlg, kBottomWallCheck);

		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			tempID = (SInt16)GetDlgItemInt(hDlg, kPICTIDItem, NULL, FALSE);
			if ((tempID >= 3000) && (tempID < 3800) && (PictIDExists(tempID)))
			{
				if (tempID != *pPictID)
					SetInitialTiles(g_tempBack, g_tempTiles);
				*pPictID = tempID;
				tempShort = 0;
				if (IsDlgButtonChecked(hDlg, kLeftWallCheck) == BST_UNCHECKED)
					tempShort += 1;
				if (IsDlgButtonChecked(hDlg, kTopWallCheck) == BST_UNCHECKED)
					tempShort += 2;
				if (IsDlgButtonChecked(hDlg, kRightWallCheck) == BST_UNCHECKED)
					tempShort += 4;
				if (IsDlgButtonChecked(hDlg, kBottomWallCheck) == BST_UNCHECKED)
					tempShort += 8;
				if (IsDlgButtonChecked(hDlg, kFloorSupportCheck) == BST_CHECKED)
					tempShort += 16;
				tempShort = tempShort << 1;  // shift left 1 bit
				tempShort += 1;  // flag that says orginal bounds used
				g_thisRoom->bounds = tempShort;

				g_fileDirty = true;
				UpdateMenus(false);
				EndDialog(hDlg, IDOK);
			}
			else
			{
				MessageBeep(MB_ICONWARNING);
				SetDlgItemInt(hDlg, kPICTIDItem, (UINT)*pPictID, FALSE);
			}
			break;

		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;

		case kLeftWallCheck:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				hwndLine = GetDlgItem(hDlg, kLeftWallLine);
				if (IsDlgButtonChecked(hDlg, kLeftWallCheck))
					ShowWindow(hwndLine, SW_SHOW);
				else
					ShowWindow(hwndLine, SW_HIDE);
			}
			break;

		case kTopWallCheck:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				hwndLine = GetDlgItem(hDlg, kTopWallLine);
				if (IsDlgButtonChecked(hDlg, kTopWallCheck))
					ShowWindow(hwndLine, SW_SHOW);
				else
					ShowWindow(hwndLine, SW_HIDE);
			}
			break;

		case kRightWallCheck:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				hwndLine = GetDlgItem(hDlg, kRightWallLine);
				if (IsDlgButtonChecked(hDlg, kRightWallCheck))
					ShowWindow(hwndLine, SW_SHOW);
				else
					ShowWindow(hwndLine, SW_HIDE);
			}
			break;

		case kBottomWallCheck:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				hwndLine = GetDlgItem(hDlg, kBottomWallLine);
				if (IsDlgButtonChecked(hDlg, kBottomWallCheck))
					ShowWindow(hwndLine, SW_SHOW);
				else
					ShowWindow(hwndLine, SW_HIDE);
			}
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  ChooseOriginalArt

SInt16 ChooseOriginalArt (HWND hwndOwner, SInt16 wasPictID)
{
	SInt16 newPictID;
	INT_PTR result;

	newPictID = wasPictID;
	if (newPictID < kUserBackground)
	{
		newPictID = kUserBackground;
	}

	result = DialogBoxParam(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kOriginalArtDialogID),
		hwndOwner, OriginalArtFilter, (LPARAM)&newPictID);

	if (result == IDOK)
	{
		return newPictID;
	}
	else
	{
		return wasPictID;
	}
}

//--------------------------------------------------------------  PictIDExists

Boolean PictIDExists (SInt16 theID)
{
	return Gp_HouseImageExists(g_theHouseFile, theID);
}

//--------------------------------------------------------------  GetFirstPICT

SInt16 GetFirstPICT (void)
{
	return Gp_GetFirstHouseImageID(g_theHouseFile, kUserBackground, 3799, -1);
}

//--------------------------------------------------------------  BitchAboutPICTNotFound

void BitchAboutPICTNotFound (HWND ownerWindow)
{
	Alert(kNoPICTFoundAlert, ownerWindow, NULL);
}
