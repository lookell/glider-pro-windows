#include "RoomInfo.h"

//============================================================================
//----------------------------------------------------------------------------
//                                 RoomInfo.c
//----------------------------------------------------------------------------
//============================================================================


#include "DialogUtils.h"
#include "GliderDefines.h"
#include "House.h"
#include "HouseIO.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "Menu.h"
#include "RectUtils.h"
#include "Room.h"
#include "ResourceIDs.h"
#include "Utilities.h"


#define kRoomNameItem               1003
#define kRoomLocationBox            1006
#define kRoomTilesBox               1010
#define kRoomPopupItem              1011
#define kRoomDividerLine            1012
#define kRoomTilesBox2              1015
#define kRoomFirstCheck             1017
#define kLitUnlitText               1018
#define kBoundsButton               1019

#define kPICTIDItem                 1003
#define kFloorSupportCheck          1005
#define kLeftWallCheck              1006
#define kTopWallCheck               1007
#define kRightWallCheck             1008
#define kBottomWallCheck            1009
#define kLeftWallLine               1010
#define kTopWallLine                1011
#define kRightWallLine              1012
#define kBottomWallLine             1013

#define kMiniTileWide               16


void LoadTileSrcGraphic (HDC, SInt16, Rect *);
void UpdateRoomInfoDialog (HWND, HDC);
void DragMiniTile (HWND, Point, SInt16 *);
void HiliteTileOver (HWND, Point);
void RoomInfo_InitDialog (HWND);
INT_PTR CALLBACK RoomFilter (HWND, UINT, WPARAM, LPARAM);
SInt16 ChooseOriginalArt (HWND, SInt16);
LRESULT SendButtonClickedCommand (HWND, int);
INT_PTR CALLBACK OriginalArtFilter (HWND, UINT, WPARAM, LPARAM);
Boolean PictIDExists (SInt16);
SInt16 GetFirstPICT (void);
void BitchAboutPICTNotFound (HWND);


Rect tileSrcRect;
HDC tileSrcMap;
SInt16 tempTiles[kNumTiles];

static Rect tileSrc;
static Rect tileDest;
static SInt16 tileOver;
static SInt16 tempBack;
static Boolean showHandCursor = false;


//==============================================================  Functions
//--------------------------------------------------------------  LoadTileSrcGraphic

void LoadTileSrcGraphic (HDC hdc, SInt16 backID, Rect *theRect)
{
	switch (backID)
	{
	case kBasement:
		LoadScaledGraphic(hdc, kBasementTileSrc, theRect);
		break;

	case kDirt:
		LoadScaledGraphic(hdc, kDirtTileSrc, theRect);
		break;

	case kStratosphere:
		LoadScaledGraphic(hdc, kStratosphereTileSrc, theRect);
		break;

	case kStars:
		LoadScaledGraphic(hdc, kStarsTileSrc, theRect);
		break;

	default:
		LoadScaledGraphic(hdc, backID, theRect);
		break;
	}
}

//--------------------------------------------------------------  UpdateRoomInfoDialog

#ifndef COMPILEDEMO
void UpdateRoomInfoDialog (HWND hDlg, HDC hdc)
{
	Rect src, dest;
	SInt16 i;

	Mac_CopyBits(tileSrcMap, hdc, &tileSrcRect, &tileSrc, srcCopy, nil);

	dest = tileDest;
	dest.right = dest.left + kMiniTileWide;
	for (i = 0; i < kNumTiles; i++)
	{
		QSetRect(&src, 0, 0, kMiniTileWide, 80);
		HOffsetRect(&src, tempTiles[i] * kMiniTileWide);
		Mac_CopyBits(tileSrcMap, hdc, &src, &dest, srcCopy, nil);
		HOffsetRect(&dest, kMiniTileWide);
	}

	if (GetNumberOfLights(thisRoomNumber) == 0)
		SetDlgItemText(hDlg, kLitUnlitText, L"(Room Is Dark)");
	else
		SetDlgItemText(hDlg, kLitUnlitText, L"(Room Is Lit)");

	Mac_FrameRect(hdc, &tileSrc, GetStockObject(BLACK_BRUSH), 1, 1);
	Mac_FrameRect(hdc, &tileDest, GetStockObject(BLACK_BRUSH), 1, 1);
}
#endif

//--------------------------------------------------------------  DragMiniTile

#ifndef COMPILEDEMO
void DragMiniTile (HWND hDlg, Point mouseIs, SInt16 *newTileOver)
{
	RECT dragRect;
	Rect hiliteRect;
	Point mouseWas;
	SInt16 wasTileOver;
	HBRUSH hiliteBrush;
	HDC hdc;
	MSG msg;

	SetCapture(hDlg);
	SetCursor(handCursor);

	hiliteBrush = CreateSolidBrush(blueColor);

	tileOver = (mouseIs.h - tileSrc.left) / kMiniTileWide;
	wasTileOver = -1;
	SetRect(&dragRect, 0, 0, kMiniTileWide, 80);
	OffsetRect(&dragRect,
		tileSrc.left + (tileOver * kMiniTileWide),
		tileSrc.top);

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

			// is cursor in the drop rect?
			if (Mac_PtInRect(mouseIs, &tileDest))
			{
				*newTileOver = (mouseIs.h - tileDest.left) / kMiniTileWide;
				if (*newTileOver != wasTileOver)
				{
					QSetRect(&hiliteRect, 0, 0, kMiniTileWide, 2);
					QOffsetRect(&hiliteRect,
						tileDest.left + (*newTileOver * kMiniTileWide),
						tileDest.top - 3);
					Mac_PaintRect(hdc, &hiliteRect, hiliteBrush);

					QSetRect(&hiliteRect, 0, 0, kMiniTileWide, 2);
					QOffsetRect(&hiliteRect,
						tileDest.left + (*newTileOver * kMiniTileWide),
						tileDest.bottom + 1);
					Mac_PaintRect(hdc, &hiliteRect, hiliteBrush);

					if (wasTileOver != -1)
					{
						QSetRect(&hiliteRect, 0, 0, kMiniTileWide, 2);
						QOffsetRect(&hiliteRect,
							tileDest.left + (wasTileOver * kMiniTileWide),
							tileDest.top - 3);
						Mac_PaintRect(hdc, &hiliteRect, GetSysColorBrush(COLOR_BTNFACE));

						QSetRect(&hiliteRect, 0, 0, kMiniTileWide, 2);
						QOffsetRect(&hiliteRect,
							tileDest.left + (wasTileOver * kMiniTileWide),
							tileDest.bottom + 1);
						Mac_PaintRect(hdc, &hiliteRect, GetSysColorBrush(COLOR_BTNFACE));
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
					QSetRect(&hiliteRect, 0, 0, kMiniTileWide, 2);
					QOffsetRect(&hiliteRect,
						tileDest.left + (wasTileOver * kMiniTileWide),
						tileDest.top - 3);
					Mac_PaintRect(hdc, &hiliteRect, GetSysColorBrush(COLOR_BTNFACE));

					QSetRect(&hiliteRect, 0, 0, kMiniTileWide, 2);
					QOffsetRect(&hiliteRect,
						tileDest.left + (wasTileOver * kMiniTileWide),
						tileDest.bottom + 1);
					Mac_PaintRect(hdc, &hiliteRect, GetSysColorBrush(COLOR_BTNFACE));

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

	DeleteObject(hiliteBrush);

	hdc = GetDC(hDlg);
	if (wasTileOver != -1)
	{
		QSetRect(&hiliteRect, 0, 0, kMiniTileWide, 2);
		QOffsetRect(&hiliteRect,
			tileDest.left + (wasTileOver * kMiniTileWide),
			tileDest.top - 3);
		Mac_PaintRect(hdc, &hiliteRect, GetSysColorBrush(COLOR_BTNFACE));

		QSetRect(&hiliteRect, 0, 0, kMiniTileWide, 2);
		QOffsetRect(&hiliteRect,
			tileDest.left + (wasTileOver * kMiniTileWide),
			tileDest.bottom + 1);
		Mac_PaintRect(hdc, &hiliteRect, GetSysColorBrush(COLOR_BTNFACE));

		wasTileOver = -1;
	}
	DrawFocusRect(hdc, &dragRect);
	ReleaseDC(hDlg, hdc);
}
#endif

//--------------------------------------------------------------  HiliteTileOver

#ifndef COMPILEDEMO
void HiliteTileOver (HWND hDlg, Point mouseIs)
{
	SInt16 newTileOver;
	HDC hdc;
	Rect hiliteRect;
	COLORREF oldColor;

	if (Mac_PtInRect(mouseIs, &tileSrc))
	{
		newTileOver = (mouseIs.h - tileSrc.left) / kMiniTileWide;
		if (newTileOver != tileOver)
		{
			hdc = GetDC(hDlg);

			oldColor = SetDCBrushColor(hdc, redColor);
			QSetRect(&hiliteRect, 0, 0, kMiniTileWide, 2);
			QOffsetRect(&hiliteRect,
				tileSrc.left + (newTileOver * kMiniTileWide),
				tileSrc.top - 3);
			Mac_PaintRect(hdc, &hiliteRect, GetStockObject(DC_BRUSH));

			QSetRect(&hiliteRect, 0, 0, kMiniTileWide, 2);
			QOffsetRect(&hiliteRect,
				tileSrc.left + (newTileOver * kMiniTileWide),
				tileSrc.bottom + 1);
			Mac_PaintRect(hdc, &hiliteRect, GetStockObject(DC_BRUSH));
			SetDCBrushColor(hdc, oldColor);

			if (tileOver != -1)
			{
				QSetRect(&hiliteRect, 0, 0, kMiniTileWide, 2);
				QOffsetRect(&hiliteRect,
					tileSrc.left + (tileOver * kMiniTileWide),
					tileSrc.top - 3);
				Mac_PaintRect(hdc, &hiliteRect, GetSysColorBrush(COLOR_BTNFACE));
				QSetRect(&hiliteRect, 0, 0, kMiniTileWide, 2);
				QOffsetRect(&hiliteRect,
					tileSrc.left + (tileOver * kMiniTileWide),
					tileSrc.bottom + 1);
				Mac_PaintRect(hdc, &hiliteRect, GetSysColorBrush(COLOR_BTNFACE));
			}

			ReleaseDC(hDlg, hdc);

			tileOver = newTileOver;
		}
	}
	else
	{
		if (tileOver != -1)
		{
			hdc = GetDC(hDlg);

			QSetRect(&hiliteRect, 0, 0, kMiniTileWide, 2);
			QOffsetRect(&hiliteRect,
				tileSrc.left + (tileOver * kMiniTileWide),
				tileSrc.top - 3);
			Mac_PaintRect(hdc, &hiliteRect, GetSysColorBrush(COLOR_BTNFACE));
			QSetRect(&hiliteRect, 0, 0, kMiniTileWide, 2);
			QOffsetRect(&hiliteRect,
				tileSrc.left + (tileOver * kMiniTileWide),
				tileSrc.bottom + 1);
			Mac_PaintRect(hdc, &hiliteRect, GetSysColorBrush(COLOR_BTNFACE));

			ReleaseDC(hDlg, hdc);

			tileOver = -1;
		}
	}
}
#endif

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
	OSErr theErr;
	SInt16 i;

	CenterOverOwner(hDlg);

	showHandCursor = false;
	tileOver = -1;
	tempBack = thisRoom->background;

	rootMenu = LoadMenu(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDM_ROOT));
	if (rootMenu == NULL)
	{
		RedAlert(kErrFailedResourceLoad);
	}
	menuItem.cbSize = sizeof(menuItem);
	menuItem.fMask = MIIM_SUBMENU;
	if (!GetMenuItemInfo(rootMenu, IDM_BACKGROUNDS, FALSE, &menuItem))
	{
		RedAlert(kErrFailedResourceLoad);
	}
	backgroundsMenu = menuItem.hSubMenu;
	if (!HouseHasOriginalPicts())
	{
		DeleteMenu(backgroundsMenu, kUserBackground, MF_BYCOMMAND);
	}
	AddMenuToComboBox(hDlg, kRoomPopupItem, backgroundsMenu);
	DestroyMenu(rootMenu);

	StringCchPrintf(floorStr, ARRAYSIZE(floorStr), L"%d", (int)thisRoom->floor);
	StringCchPrintf(suiteStr, ARRAYSIZE(suiteStr), L"%d", (int)thisRoom->suite);
	StringCchPrintf(objectsStr, ARRAYSIZE(objectsStr), L"%d", (int)thisRoom->numObjects);

	params.arg[0] = floorStr;
	params.arg[1] = suiteStr;
	params.arg[2] = objectsStr;
	ParamDialogText(hDlg, &params);

	theErr = CreateOffScreenGWorld(&tileSrcMap, &tileSrcRect, kPreferredDepth);
	if ((tempBack > kStars) && (!PictIDExists(tempBack)))
	{
		BitchAboutPICTNotFound(hDlg);
		tempBack = kSimpleRoom;
	}
	LoadTileSrcGraphic(tileSrcMap, tempBack, &tileSrcRect);

	for (i = 0; i < kNumTiles; i++)
		tempTiles[i] = thisRoom->tiles[i];

	if (tempBack >= kUserBackground)
		SetComboBoxMenuValue(hDlg, kRoomPopupItem, kUserBackground);
	else
		SetComboBoxMenuValue(hDlg, kRoomPopupItem, tempBack);

	SetDialogString(hDlg, kRoomNameItem, thisRoom->name);

	QSetRect(&tileSrc, 0, 0, 128, 80);
	GetDialogItemRect(hDlg, kRoomTilesBox, &tileBoxRect);
	CenterRectInRect(&tileSrc, &tileBoxRect);

	QSetRect(&tileDest, 0, 0, 128, 80);
	GetDialogItemRect(hDlg, kRoomTilesBox2, &tileBoxRect);
	CenterRectInRect(&tileDest, &tileBoxRect);

	if (thisHouse->firstRoom == thisRoomNumber)
		CheckDlgButton(hDlg, kRoomFirstCheck, BST_CHECKED);
	else
		CheckDlgButton(hDlg, kRoomFirstCheck, BST_UNCHECKED);

	if (tempBack >= kUserBackground)
		EnableWindow(GetDlgItem(hDlg, kBoundsButton), TRUE);
	else
		EnableWindow(GetDlgItem(hDlg, kBoundsButton), FALSE);
}

//--------------------------------------------------------------  RoomFilter

#ifndef COMPILEDEMO
INT_PTR CALLBACK RoomFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	Point mouseIs;
	SInt16 newBack, newTileOver, i;
	Boolean forceDraw;

	switch (message)
	{
	case WM_INITDIALOG:
		RoomInfo_InitDialog(hDlg);
		return TRUE;

	case WM_DESTROY:
		DisposeGWorld(tileSrcMap);
		tileSrcMap = NULL;
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			for (i = 0; i < kNumTiles; i++)
				thisRoom->tiles[i] = tempTiles[i];

			GetDialogString(hDlg, kRoomNameItem,
				thisRoom->name, ARRAYSIZE(thisRoom->name));
			if (IsDlgButtonChecked(hDlg, kRoomFirstCheck))
				thisHouse->firstRoom = thisRoomNumber;

			thisRoom->background = tempBack;
			if (tempBack < kUserBackground)
				lastBackground = tempBack;

			CopyThisRoomToRoom();
			ReflectCurrentRoom(false);
			fileDirty = true;
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
					if (tempBack < kUserBackground)
					{
						// then assign 1st PICT
						tempBack = GetFirstPICT();
						forceDraw = true;
					}
					else
					{
						forceDraw = false;
					}
					newBack = ChooseOriginalArt(hDlg, tempBack);
					if ((tempBack != newBack) || (forceDraw))
					{
						tempBack = newBack;
						LoadTileSrcGraphic(tileSrcMap, tempBack, &tileSrcRect);
						Mac_InvalWindowRect(hDlg, &tileSrc);
						Mac_InvalWindowRect(hDlg, &tileDest);
					}
				}
				else
				{
					// if background has changed
					if (newBack != tempBack)
						SetInitialTiles(newBack, false);
				}

				if (newBack >= kUserBackground)
				{
					EnableWindow(GetDlgItem(hDlg, kBoundsButton), TRUE);
					// if background has changed
					if (newBack != tempBack)
						SetInitialTiles(newBack, false);
				}
				else
				{
					EnableWindow(GetDlgItem(hDlg, kBoundsButton), FALSE);
				}

				if (newBack != tempBack)
				{
					tempBack = newBack;
					LoadTileSrcGraphic(tileSrcMap, tempBack, &tileSrcRect);
					Mac_InvalWindowRect(hDlg, &tileSrc);
					Mac_InvalWindowRect(hDlg, &tileDest);
				}
			}
			break;

		case kBoundsButton:
			newBack = ChooseOriginalArt(hDlg, tempBack);
			if (tempBack != newBack)
			{
				tempBack = newBack;
				LoadTileSrcGraphic(tileSrcMap, tempBack, &tileSrcRect);
				Mac_InvalWindowRect(hDlg, &tileSrc);
				Mac_InvalWindowRect(hDlg, &tileDest);
			}
			break;
		}
		return TRUE;

	case WM_SETCURSOR:
		if (showHandCursor)
		{
			SetCursor(handCursor);
			SetWindowLongPtr(hDlg, DWLP_MSGRESULT, TRUE);
			return TRUE;
		}
		return FALSE;

	case WM_LBUTTONDOWN:
		mouseIs.h = GET_X_LPARAM(lParam);
		mouseIs.v = GET_Y_LPARAM(lParam);
		if (Mac_PtInRect(mouseIs, &tileSrc))
		{
			POINT pt;
			pt.x = mouseIs.h;
			pt.y = mouseIs.v;
			ClientToScreen(hDlg, &pt);
			if (DragDetect(hDlg, pt))
			{
				DragMiniTile(hDlg, mouseIs, &newTileOver);
				if ((newTileOver >= 0) && (newTileOver < kNumTiles))
				{
					tempTiles[newTileOver] = tileOver;
					InvalidateRect(hDlg, NULL, TRUE);
				}
			}
		}
		return FALSE;

	case WM_MOUSEMOVE:
		mouseIs.h = GET_X_LPARAM(lParam);
		mouseIs.v = GET_Y_LPARAM(lParam);
		showHandCursor = Mac_PtInRect(mouseIs, &tileSrc);
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
#endif

//--------------------------------------------------------------  DoRoomInfo

void DoRoomInfo (HWND hwndOwner)
{
#ifndef COMPILEDEMO
	DialogBox(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(kRoomInfoDialogID),
		hwndOwner, RoomFilter);
#endif
}

//--------------------------------------------------------------  SendButtonClickedCommand

#ifndef COMPILEDEMO

LRESULT SendButtonClickedCommand (HWND hDlg, int ctrlID)
{
	return SendMessage(hDlg, WM_COMMAND,
		MAKEWPARAM(ctrlID, BN_CLICKED),
		(LPARAM)GetDlgItem(hDlg, ctrlID));
}

#endif

//--------------------------------------------------------------  OriginalArtFilter

#ifndef COMPILEDEMO

INT_PTR CALLBACK OriginalArtFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	SInt16 *pPictID;
	HWND hwndLine;
	SInt16 tempShort, tempID;

	pPictID = (SInt16 *)GetWindowLongPtr(hDlg, DWLP_USER);

	switch (message)
	{
	case WM_INITDIALOG:
		CenterOverOwner(hDlg);
		SetWindowLongPtr(hDlg, DWLP_USER, lParam);
		pPictID = (SInt16 *)GetWindowLongPtr(hDlg, DWLP_USER);
		if (pPictID == NULL)
		{
			EndDialog(hDlg, IDCANCEL);
			return FALSE;
		}

		SetDlgItemInt(hDlg, kPICTIDItem, *pPictID, FALSE);

		// TODO: should version 1.0 house bounds be supported as well?
		tempShort = thisRoom->bounds >> 1; // version 2.0 house
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
					SetInitialTiles(tempBack, false);
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
				thisRoom->bounds = tempShort;

				fileDirty = true;
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
#endif

//--------------------------------------------------------------  ChooseOriginalArt

#ifndef COMPILEDEMO
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
#endif

//--------------------------------------------------------------  PictIDExists

Boolean PictIDExists (SInt16 theID)
{
	if (houseResFork != NULL)
	{
		return (FindResource(houseResFork, MAKEINTRESOURCE(theID), RT_BITMAP) != NULL);
	}
	else
	{
		return false;
	}
}

//--------------------------------------------------------------  GetFirstPICT

BOOL CALLBACK GetFirstPICT_EnumProc (HMODULE hModule, LPCWSTR lpszType,
	LPWSTR lpszName, LONG_PTR lParam)
{
	SInt16 *pID;

	pID = (SInt16 *)lParam;
	if (*pID == -1 && IS_INTRESOURCE(lpszName))
	{
		*pID = (SInt16)LOWORD(lpszName);
	}
	return TRUE;
}

SInt16 GetFirstPICT (void)
{
	SInt16 resID;

	resID = -1;
	if (houseResFork != NULL)
	{
		EnumResourceNames(houseResFork, RT_BITMAP,
			GetFirstPICT_EnumProc, (LONG_PTR)&resID);
	}
	return resID;
}

//--------------------------------------------------------------  BitchAboutPICTNotFound

#ifndef COMPILEDEMO
void BitchAboutPICTNotFound (HWND ownerWindow)
{
	Alert(kNoPICTFoundAlert, ownerWindow, NULL);
}
#endif

