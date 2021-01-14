//============================================================================
//----------------------------------------------------------------------------
//                                  Tools.c
//----------------------------------------------------------------------------
//============================================================================

#include "Tools.h"

#include "DialogUtils.h"
#include "GliderDefines.h"
#include "HouseIO.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "Menu.h"
#include "ObjectInfo.h"
#include "RectUtils.h"
#include "ResourceIDs.h"
#include "Utilities.h"

#include <commctrl.h>
#include <strsafe.h>

#define kToolModeCombo      1000
#define kToolButtonBaseID   2000
#define kToolButtonFirstID  2000
#define kToolButtonLastID   2015
#define kToolNameText       3000

#define kToolsHigh          4
#define kToolsWide          4
#define kTotalTools         16  // kToolsHigh * kToolsWide
#define kFirstBlower        1
#define kLastBlower         15
#define kBlowerBase         1
#define kFirstFurniture     1
#define kLastFurniture      15
#define kFurnitureBase      21
#define kFirstBonus         1
#define kLastBonus          15
#define kBonusBase          41
#define kFirstTransport     1
#define kLastTransport      12
#define kTransportBase      61
#define kFirstSwitch        1
#define kLastSwitch         9
#define kSwitchBase         81
#define kFirstLight         1
#define kLastLight          8
#define kLightBase          101
#define kFirstAppliance     1
#define kLastAppliance      14
#define kApplianceBase      121
#define kFirstEnemy         1
#define kLastEnemy          9
#define kEnemyBase          141
#define kFirstClutter       1
#define kLastClutter        15
#define kClutterBase        161

void CreateToolsOffscreen (void);
void KillToolsOffscreen (void);
void GetToolName (PWSTR buffer, size_t length, SInt16 selected, SInt16 mode);
void UpdateToolName (void);
void UpdateToolTiles (void);
INT_PTR CALLBACK ToolsWindowProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR Tools_OnInitDialog (HWND hwnd);
void Tools_OnDestroy (HWND hwnd);
void Tools_OnMove (HWND hwnd);
void Tools_OnToolSelChange (HWND hwnd);
void Tools_OnButtonClick (HWND hwnd, WORD buttonID);
void UpdateToolTips (HWND hwnd);
void SwitchToolModes (SInt16 newMode);

HWND g_toolsWindow;
SInt16 g_isToolsH;
SInt16 g_isToolsV;
SInt16 g_toolSelected;
SInt16 g_toolMode;
Boolean g_isToolsOpen;

static HDC g_toolSrcMap = NULL;
static SInt16 g_firstTool;
static SInt16 g_lastTool;
static SInt16 g_objectBase;
static HWND g_toolButtonTooltip;

//==============================================================  Functions
//--------------------------------------------------------------  CreateToolsOffscreen

void CreateToolsOffscreen (void)
{
	Rect toolSrcRect;

	if (g_toolSrcMap == NULL)
	{
		QSetRect(&toolSrcRect, 0, 0, 360, 216);
		g_toolSrcMap = CreateOffScreenGWorld(&toolSrcRect, kPreferredDepth);
		LoadGraphic(g_toolSrcMap, g_theHouseFile, kToolsPictID);
	}
}

//--------------------------------------------------------------  KillToolsOffscreen

void KillToolsOffscreen (void)
{
	if (g_toolSrcMap != NULL)
	{
		DisposeGWorld(g_toolSrcMap);
		g_toolSrcMap = NULL;
	}
}

//--------------------------------------------------------------  GetToolName

void GetToolName (PWSTR buffer, size_t length, SInt16 selected, SInt16 mode)
{
	if (selected == kSelectTool)
	{
		StringCchCopy(buffer, length, L"Selection Tool");
	}
	else
	{
		GetObjectName(buffer, length, selected + ((mode - 1) * 0x0010));
	}
}

//--------------------------------------------------------------  UpdateToolName

void UpdateToolName (void)
{
	wchar_t theString[256];

	GetToolName(theString, ARRAYSIZE(theString), g_toolSelected, g_toolMode);
	SetDlgItemText(g_toolsWindow, kToolNameText, theString);
}

//--------------------------------------------------------------  UpdateToolTiles

void UpdateToolTiles (void)
{
	Rect srcRect, destRect;
	SInt16 i;
	HDC hdc, displayDC;
	HBITMAP buttonBitmap, prevButtonBitmap;
	HBITMAP prevBitmap;

	displayDC = GetDC(NULL);
	hdc = CreateCompatibleDC(displayDC);

	// Selection Tool
	buttonBitmap = CreateCompatibleBitmap(displayDC, 22, 22);
	if (buttonBitmap != NULL)
	{
		prevBitmap = (HBITMAP)SelectObject(hdc, buttonBitmap);
		DrawCIcon(hdc, kSelectToolIcon, -3, -3);
		SelectObject(hdc, prevBitmap);

		prevButtonBitmap = (HBITMAP)SendDlgItemMessage(g_toolsWindow,
			kToolButtonBaseID + kSelectTool, BM_SETIMAGE,
			IMAGE_BITMAP, (LPARAM)buttonBitmap);
		if (prevButtonBitmap != NULL)
		{
			DeleteObject(prevButtonBitmap);
		}
	}

	// Other tools
	for (i = 0; i < 15; i++)
	{
		QSetRect(&srcRect, 0, 0, 22, 22);
		QOffsetRect(&srcRect, 1, 1);
		QOffsetRect(&srcRect, i * 24, (g_toolMode - 1) * 24);

		QSetRect(&destRect, 0, 0, 22, 22);

		buttonBitmap = CreateCompatibleBitmap(displayDC, 22, 22);
		if (buttonBitmap != NULL)
		{
			prevBitmap = (HBITMAP)SelectObject(hdc, buttonBitmap);
			Mac_CopyBits(g_toolSrcMap, hdc, &srcRect, &destRect, srcCopy, nil);
			SelectObject(hdc, prevBitmap);

			prevButtonBitmap = (HBITMAP)SendDlgItemMessage(g_toolsWindow,
				kToolButtonBaseID + i + 1, BM_SETIMAGE,
				IMAGE_BITMAP, (LPARAM)buttonBitmap);
			if (prevButtonBitmap != NULL)
			{
				DeleteObject(prevButtonBitmap);
			}
		}
	}

	DeleteDC(hdc);
	ReleaseDC(NULL, displayDC);
}

//--------------------------------------------------------------  SelectTool

void SelectTool (SInt16 which)
{
	SInt16 toolIcon;

	if (COMPILEDEMO)
		return;

	toolIcon = which;
	if ((g_toolMode == kBlowerMode) && (toolIcon >= 7))
	{
		toolIcon--;
	}
	else if ((g_toolMode == kTransportMode) && (toolIcon >= 7))
	{
		if (toolIcon >= 15)
			toolIcon -= 4;
		else
			toolIcon = ((toolIcon - 7) / 2) + 7;
	}

	CheckRadioButton(g_toolsWindow, kToolButtonFirstID, kToolButtonLastID,
		kToolButtonBaseID + toolIcon);

	g_toolSelected = which;
	UpdateToolName();
}

//--------------------------------------------------------------  ToolsWindowProc

INT_PTR CALLBACK ToolsWindowProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	(void)lParam;

	switch (message)
	{
	case WM_INITDIALOG:
		return Tools_OnInitDialog(hDlg);

	case WM_DESTROY:
		Tools_OnDestroy(hDlg);
		return FALSE; // perform default processing

	case WM_MOVE:
		Tools_OnMove(hDlg);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			ToggleToolsWindow();
			break;

		case kToolModeCombo:
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				Tools_OnToolSelChange(hDlg);
			}
			break;

		case kToolButtonBaseID + 0:
		case kToolButtonBaseID + 1:
		case kToolButtonBaseID + 2:
		case kToolButtonBaseID + 3:
		case kToolButtonBaseID + 4:
		case kToolButtonBaseID + 5:
		case kToolButtonBaseID + 6:
		case kToolButtonBaseID + 7:
		case kToolButtonBaseID + 8:
		case kToolButtonBaseID + 9:
		case kToolButtonBaseID + 10:
		case kToolButtonBaseID + 11:
		case kToolButtonBaseID + 12:
		case kToolButtonBaseID + 13:
		case kToolButtonBaseID + 14:
		case kToolButtonBaseID + 15:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				Tools_OnButtonClick(hDlg, LOWORD(wParam));
			}
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  Tools_OnInitDialog

INT_PTR Tools_OnInitDialog (HWND hwnd)
{
	HMENU rootMenu;
	MENUITEMINFO menuItemInfo;
	HFONT dialogFont;
	SInt16 buttonID;

	rootMenu = LoadMenu(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDM_ROOT));
	if (rootMenu == NULL)
		RedAlert(kErrFailedResourceLoad);

	menuItemInfo.cbSize = sizeof(menuItemInfo);
	menuItemInfo.fMask = MIIM_SUBMENU;
	if (!GetMenuItemInfo(rootMenu, kToolsMenuID, FALSE, &menuItemInfo))
		RedAlert(kErrFailedResourceLoad);

	AddMenuToComboBox(hwnd, kToolModeCombo, menuItemInfo.hSubMenu);
	DestroyMenu(rootMenu);

	g_toolButtonTooltip = CreateWindowEx(WS_EX_TOOLWINDOW, TOOLTIPS_CLASS, TEXT(""),
		WS_POPUP | TTS_NOPREFIX, 0, 0, 0, 0, hwnd, NULL, HINST_THISCOMPONENT, NULL);
	SetWindowPos(g_toolButtonTooltip, HWND_TOPMOST, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	dialogFont = (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0);
	SendMessage(g_toolButtonTooltip, WM_SETFONT, (WPARAM)dialogFont, FALSE);
	for (buttonID = kToolButtonFirstID; buttonID <= kToolButtonLastID; ++buttonID)
	{
		TOOLINFO toolInfo;
		WCHAR text[256] = L"";

		ZeroMemory(&toolInfo, sizeof(toolInfo));
		toolInfo.cbSize = sizeof(toolInfo);
		toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		toolInfo.hwnd = hwnd;
		toolInfo.uId = (UINT_PTR)GetDlgItem(hwnd, buttonID);
		SetRectEmpty(&toolInfo.rect);
		toolInfo.hinst = NULL;
		toolInfo.lpszText = text;
		SendMessage(g_toolButtonTooltip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
	}

	return FALSE; // don't change the focus
}

//--------------------------------------------------------------  Tools_OnDestroy

void Tools_OnDestroy (HWND hwnd)
{
	HBITMAP buttonBitmap;
	int buttonID;

	for (buttonID = kToolButtonFirstID; buttonID <= kToolButtonLastID; buttonID++)
	{
		buttonBitmap = (HBITMAP)SendDlgItemMessage(hwnd, buttonID,
			BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)NULL);
		if (buttonBitmap)
		{
			DeleteObject(buttonBitmap);
		}
	}
	if (g_toolButtonTooltip != NULL)
	{
		DestroyWindow(g_toolButtonTooltip);
		g_toolButtonTooltip = NULL;
	}
}

//--------------------------------------------------------------  Tools_OnMove

void Tools_OnMove (HWND hwnd)
{
	WINDOWPLACEMENT placement;

	placement.length = sizeof(placement);
	GetWindowPlacement(hwnd, &placement);
	g_isToolsH = (SInt16)placement.rcNormalPosition.left;
	g_isToolsV = (SInt16)placement.rcNormalPosition.top;
}

//--------------------------------------------------------------  Tools_OnToolSelChange

void Tools_OnToolSelChange (HWND hwnd)
{
	SInt16 newMode;

	GetComboBoxMenuValue(hwnd, kToolModeCombo, &newMode);
	SwitchToolModes(newMode);
}

//--------------------------------------------------------------  Tools_OnButtonClick

void Tools_OnButtonClick (HWND hwnd, WORD clickedID)
{
	SInt16 toolIcon;
	SInt16 buttonID;

	UNREFERENCED_PARAMETER(clickedID);

	// BN_CLICKED is received when a radio button is focused, but
	// being focused doesn't mean being checked. Find out which
	// button has actually last been checked.
	toolIcon = kSelectTool;
	for (buttonID = kToolButtonFirstID; buttonID <= kToolButtonLastID; buttonID++)
	{
		if (IsDlgButtonChecked(hwnd, buttonID))
		{
			toolIcon = buttonID - kToolButtonBaseID;
			break;
		}
	}

	if ((g_toolMode == kBlowerMode) && (toolIcon >= 7))
	{
		toolIcon++;
	}
	if ((g_toolMode == kTransportMode) && (toolIcon >= 7))
	{
		if (toolIcon >= 11)
			toolIcon += 4;
		else
			toolIcon = ((toolIcon - 7) * 2) + 7;
	}
	SelectTool(toolIcon);
}

//--------------------------------------------------------------  UpdateToolTips

void UpdateToolTips (HWND hwnd)
{
	SInt16 buttonID;
	SInt16 toolIcon;
	WCHAR buffer[256];
	TOOLINFO toolInfo;

	for (buttonID = kToolButtonFirstID; buttonID <= kToolButtonLastID; buttonID++)
	{
		toolIcon = buttonID - kToolButtonBaseID;
		if ((g_toolMode == kBlowerMode) && (toolIcon >= 7))
		{
			toolIcon++;
		}
		if ((g_toolMode == kTransportMode) && (toolIcon >= 7))
		{
			if (toolIcon >= 11)
				toolIcon += 4;
			else
				toolIcon = ((toolIcon - 7) * 2) + 7;
		}
		GetToolName(buffer, ARRAYSIZE(buffer), toolIcon, g_toolMode);

		ZeroMemory(&toolInfo, sizeof(toolInfo));
		toolInfo.cbSize = sizeof(toolInfo);
		toolInfo.hwnd = hwnd;
		toolInfo.uId = (UINT_PTR)GetDlgItem(hwnd, buttonID);
		toolInfo.hinst = NULL;
		toolInfo.lpszText = buffer;
		SendMessage(g_toolButtonTooltip, TTM_UPDATETIPTEXT, 0, (LPARAM)&toolInfo);
	}
}

//--------------------------------------------------------------  OpenToolsWindow

void OpenToolsWindow (void)
{
	WINDOWPLACEMENT placement;

	if (COMPILEDEMO)
		return;

	if (g_toolsWindow == NULL)
	{
		g_toolsWindow = CreateDialog(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(kToolsWindowID),
			g_mainWindow, ToolsWindowProc);
		if (g_toolsWindow == NULL)
			RedAlert(kErrNoMemory);

#if 0
		if (OptionKeyDown())
		{
			g_isToolsH = qd.screenBits.bounds.right - 120;
			g_isToolsV = 35;
		}
#endif
		placement.length = sizeof(placement);
		GetWindowPlacement(g_toolsWindow, &placement);
		OffsetRect(&placement.rcNormalPosition,
			-placement.rcNormalPosition.left,
			-placement.rcNormalPosition.top);
		OffsetRect(&placement.rcNormalPosition, g_isToolsH, g_isToolsV);
		placement.showCmd = SW_SHOWNOACTIVATE;
		SetWindowPlacement(g_toolsWindow, &placement);

		SetComboBoxMenuValue(g_toolsWindow, kToolModeCombo, g_toolMode);

		CreateToolsOffscreen();

		SwitchToolModes(g_toolMode);
		g_toolSelected = kSelectTool;
	}

	UpdateToolsCheckmark(true);
}

//--------------------------------------------------------------  CloseToolsWindow

void CloseToolsWindow (void)
{
	if (COMPILEDEMO)
		return;

	if (g_toolsWindow != NULL)
	{
		DestroyWindow(g_toolsWindow);
		g_toolsWindow = NULL;
	}
	KillToolsOffscreen();
	UpdateToolsCheckmark(false);
}

//--------------------------------------------------------------  ToggleToolsWindow

void ToggleToolsWindow (void)
{
	if (COMPILEDEMO)
		return;

	if (g_toolsWindow == NULL)
	{
		OpenToolsWindow();
		g_isToolsOpen = true;
	}
	else
	{
		CloseToolsWindow();
		g_isToolsOpen = false;
	}
}

//--------------------------------------------------------------  SwitchToolModes

void SwitchToolModes (SInt16 newMode)
{
	HWND hwndButton;
	SInt16 index;

	if (g_toolsWindow == NULL)
		return;

	SelectTool(kSelectTool);
	switch (newMode)
	{
	case kBlowerMode:
		g_firstTool = kFirstBlower;
		g_lastTool = kLastBlower;
		g_objectBase = kBlowerBase;
		break;

	case kFurnitureMode:
		g_firstTool = kFirstFurniture;
		g_lastTool = kLastFurniture;
		g_objectBase = kFurnitureBase;
		break;

	case kBonusMode:
		g_firstTool = kFirstBonus;
		g_lastTool = kLastBonus;
		g_objectBase = kBonusBase;
		break;

	case kTransportMode:
		g_firstTool = kFirstTransport;
		g_lastTool = kLastTransport;
		g_objectBase = kTransportBase;
		break;

	case kSwitchMode:
		g_firstTool = kFirstSwitch;
		g_lastTool = kLastSwitch;
		g_objectBase = kSwitchBase;
		break;

	case kLightMode:
		g_firstTool = kFirstLight;
		g_lastTool = kLastLight;
		g_objectBase = kLightBase;
		break;

	case kApplianceMode:
		g_firstTool = kFirstAppliance;
		g_lastTool = kLastAppliance;
		g_objectBase = kApplianceBase;
		break;

	case kEnemyMode:
		g_firstTool = kFirstEnemy;
		g_lastTool = kLastEnemy;
		g_objectBase = kEnemyBase;
		break;

	case kClutterMode:
		g_firstTool = kFirstClutter;
		g_lastTool = kLastClutter;
		g_objectBase = kClutterBase;
		break;
	}

	for (index = 1; index < kTotalTools; index++)
	{
		hwndButton = GetDlgItem(g_toolsWindow, kToolButtonBaseID + index);
		if (index <= g_lastTool)
		{
			ShowWindow(hwndButton, SW_SHOW);
		}
		else
		{
			ShowWindow(hwndButton, SW_HIDE);
		}
	}

	g_toolMode = newMode;
	UpdateToolTiles();
	UpdateToolTips(g_toolsWindow);
}

//--------------------------------------------------------------  NextToolMode

void NextToolMode (void)
{
	if (COMPILEDEMO)
		return;

	if (g_toolsWindow == NULL)
		return;

	if ((g_theMode == kEditMode) && (g_toolMode < kClutterMode))
	{
		g_toolMode++;
		SetComboBoxMenuValue(g_toolsWindow, kToolModeCombo, g_toolMode);
		SwitchToolModes(g_toolMode);
		g_toolSelected = kSelectTool;
	}
}

//--------------------------------------------------------------  PrevToolMode

void PrevToolMode (void)
{
	if (COMPILEDEMO)
		return;

	if (g_toolsWindow == NULL)
		return;

	if ((g_theMode == kEditMode) && (g_toolMode > kBlowerMode))
	{
		g_toolMode--;
		SetComboBoxMenuValue(g_toolsWindow, kToolModeCombo, g_toolMode);
		SwitchToolModes(g_toolMode);
		g_toolSelected = kSelectTool;
	}
}

//--------------------------------------------------------------  SetSpecificToolMode

void SetSpecificToolMode (SInt16 modeToSet)
{
	if (COMPILEDEMO)
		return;

	if ((g_toolsWindow == NULL) || (g_theMode != kEditMode))
		return;

	g_toolMode = modeToSet;
	SetComboBoxMenuValue(g_toolsWindow, kToolModeCombo, g_toolMode);
	SwitchToolModes(g_toolMode);
	g_toolSelected = kSelectTool;
}
