#define GP_USE_WINAPI_H

#include "Tools.h"

//============================================================================
//----------------------------------------------------------------------------
//                                  Tools.c
//----------------------------------------------------------------------------
//============================================================================


#include "DialogUtils.h"
#include "GliderDefines.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "Menu.h"
#include "ObjectInfo.h"
#include "RectUtils.h"
#include "ResourceIDs.h"
#include "StringUtils.h"
#include "Utilities.h"

#include <strsafe.h>


#define kToolModeCombo      1000
#define kToolButtonBaseID   2000
#define kToolButtonFirstID  2000
#define kToolButtonLastID   2015
#define kToolNameText       3000

#define kToolsHigh			4
#define kToolsWide			4
#define kTotalTools			16				// kToolsHigh * kToolsWide
#define kFirstBlower		1
#define kLastBlower			15
#define kBlowerBase			1
#define kFirstFurniture		1
#define kLastFurniture		15
#define kFurnitureBase		21
#define kFirstBonus			1
#define kLastBonus			15
#define kBonusBase			41
#define kFirstTransport		1
#define kLastTransport		12
#define kTransportBase		61
#define kFirstSwitch		1
#define kLastSwitch			9
#define kSwitchBase			81
#define kFirstLight			1
#define kLastLight			8
#define kLightBase			101
#define kFirstAppliance		1
#define kLastAppliance		14
#define kApplianceBase		121
#define kFirstEnemy			1
#define kLastEnemy			9
#define kEnemyBase			141
#define kFirstClutter		1
#define kLastClutter		15
#define kClutterBase		161


void CreateToolsOffscreen (void);
void KillToolsOffscreen (void);
void UpdateToolName (void);
void UpdateToolTiles (void);
INT_PTR CALLBACK ToolsWindowProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR Tools_OnInitDialog (HWND hwnd);
void Tools_OnDestroy (HWND hwnd);
void Tools_OnMove (HWND hwnd);
void Tools_OnToolSelChange (HWND hwnd);
void Tools_OnButtonClick (HWND hwnd, WORD buttonID);
void SwitchToolModes (SInt16 newMode);


HWND toolsWindow;
SInt16 isToolsH;
SInt16 isToolsV;
SInt16 toolSelected;
SInt16 toolMode;
Boolean isToolsOpen;

static HDC toolSrcMap = NULL;
static SInt16 firstTool;
static SInt16 lastTool;
static SInt16 objectBase;


//==============================================================  Functions
//--------------------------------------------------------------  CreateToolsOffscreen

#ifndef COMPILEDEMO
void CreateToolsOffscreen (void)
{
	Rect toolSrcRect;
	OSErr theErr;

	if (toolSrcMap == NULL)
	{
		QSetRect(&toolSrcRect, 0, 0, 360, 216);
		theErr = CreateOffScreenGWorld(&toolSrcMap, &toolSrcRect, kPreferredDepth);
		LoadGraphic(toolSrcMap, kToolsPictID);
	}
}
#endif

//--------------------------------------------------------------  KillToolsOffscreen

#ifndef COMPILEDEMO
void KillToolsOffscreen (void)
{
	if (toolSrcMap != NULL)
	{
		DisposeGWorld(toolSrcMap);
		toolSrcMap = NULL;
	}
}
#endif

//--------------------------------------------------------------  UpdateToolName

#ifndef COMPILEDEMO
void UpdateToolName (void)
{
	wchar_t theString[256];

	if (toolSelected == 0)
	{
		StringCchCopy(theString, ARRAYSIZE(theString), L"Selection Tool");
	}
	else
	{
		GetObjectName(theString, ARRAYSIZE(theString),
			toolSelected + ((toolMode - 1) * 0x0010));
	}

	SetDlgItemText(toolsWindow, kToolNameText, theString);
}
#endif

//--------------------------------------------------------------  UpdateToolTiles

#ifndef COMPILEDEMO
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
		DrawCIcon(hdc, kSelectionTool, -3, -3);
		SelectObject(hdc, prevBitmap);

		prevButtonBitmap = (HBITMAP)SendDlgItemMessage(toolsWindow,
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
		QOffsetRect(&srcRect, i * 24, (toolMode - 1) * 24);

		QSetRect(&destRect, 0, 0, 22, 22);

		buttonBitmap = CreateCompatibleBitmap(displayDC, 22, 22);
		if (buttonBitmap != NULL)
		{
			prevBitmap = (HBITMAP)SelectObject(hdc, buttonBitmap);
			Mac_CopyBits(toolSrcMap, hdc, &srcRect, &destRect, srcCopy, nil);
			SelectObject(hdc, prevBitmap);

			prevButtonBitmap = (HBITMAP)SendDlgItemMessage(toolsWindow,
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
#endif

//--------------------------------------------------------------  SelectTool

void SelectTool (SInt16 which)
{
#ifndef COMPILEDEMO
	SInt16 toolIcon;

	toolIcon = which;
	if ((toolMode == kBlowerMode) && (toolIcon >= 7))
	{
		toolIcon--;
	}
	else if ((toolMode == kTransportMode) && (toolIcon >= 7))
	{
		if (toolIcon >= 15)
			toolIcon -= 4;
		else
			toolIcon = ((toolIcon - 7) / 2) + 7;
	}

	CheckRadioButton(toolsWindow, kToolButtonFirstID, kToolButtonLastID,
		kToolButtonBaseID + toolIcon);

	toolSelected = which;
	UpdateToolName();
#endif
}

//--------------------------------------------------------------  ToolsWindowProc

#ifndef COMPILEDEMO
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
#endif

//--------------------------------------------------------------  Tools_OnInitDialog

#ifndef COMPILEDEMO
INT_PTR Tools_OnInitDialog (HWND hwnd)
{
	HMENU rootMenu;
	MENUITEMINFO menuItemInfo;

	rootMenu = LoadMenu(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDM_ROOT));
	if (rootMenu == NULL)
		RedAlert(kErrFailedResourceLoad);

	menuItemInfo.cbSize = sizeof(menuItemInfo);
	menuItemInfo.fMask = MIIM_SUBMENU;
	if (!GetMenuItemInfo(rootMenu, kToolsMenuID, FALSE, &menuItemInfo))
		RedAlert(kErrFailedResourceLoad);

	AddMenuToComboBox(hwnd, kToolModeCombo, menuItemInfo.hSubMenu);
	DestroyMenu(rootMenu);

	return FALSE; // don't change the focus
}
#endif

//--------------------------------------------------------------  Tools_OnDestroy

#ifndef COMPILEDEMO
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
}
#endif

//--------------------------------------------------------------  Tools_OnMove

#ifndef COMPILEDEMO
void Tools_OnMove (HWND hwnd)
{
	WINDOWPLACEMENT placement;

	placement.length = sizeof(placement);
	GetWindowPlacement(hwnd, &placement);
	isToolsH = (SInt16)placement.rcNormalPosition.left;
	isToolsV = (SInt16)placement.rcNormalPosition.top;
}
#endif

//--------------------------------------------------------------  Tools_OnToolSelChange

#ifndef COMPILEDEMO
void Tools_OnToolSelChange (HWND hwnd)
{
	SInt16 newMode;

	GetComboBoxMenuValue(hwnd, kToolModeCombo, &newMode);
	SwitchToolModes(newMode);
}
#endif

//--------------------------------------------------------------  Tools_OnButtonClick

#ifndef COMPiLEDEMO
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

	if ((toolMode == kBlowerMode) && (toolIcon >= 7))
	{
		toolIcon++;
	}
	if ((toolMode == kTransportMode) && (toolIcon >= 7))
	{
		if (toolIcon >= 11)
			toolIcon += 4;
		else
			toolIcon = ((toolIcon - 7) * 2) + 7;
	}
	SelectTool(toolIcon);
}
#endif

//--------------------------------------------------------------  OpenToolsWindow

void OpenToolsWindow (void)
{
#ifndef COMPILEDEMO
	WINDOWPLACEMENT placement;

	if (toolsWindow == NULL)
	{
		toolsWindow = CreateDialog(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(kToolsWindowID),
			mainWindow, ToolsWindowProc);
		if (toolsWindow == NULL)
			RedAlert(kErrNoMemory);

//		if (OptionKeyDown())
//		{
//			isToolsH = qd.screenBits.bounds.right - 120;
//			isToolsV = 35;
//		}
		placement.length = sizeof(placement);
		GetWindowPlacement(toolsWindow, &placement);
		OffsetRect(&placement.rcNormalPosition,
			-placement.rcNormalPosition.left,
			-placement.rcNormalPosition.top);
		OffsetRect(&placement.rcNormalPosition, isToolsH, isToolsV);
		placement.showCmd = SW_SHOWNOACTIVATE;
		SetWindowPlacement(toolsWindow, &placement);

		SetComboBoxMenuValue(toolsWindow, kToolModeCombo, toolMode);

		CreateToolsOffscreen();

		SwitchToolModes(toolMode);
		toolSelected = kSelectTool;
	}

	UpdateToolsCheckmark(true);
#endif
}

//--------------------------------------------------------------  CloseToolsWindow

void CloseToolsWindow (void)
{
#ifndef COMPILEDEMO
	if (toolsWindow != NULL)
	{
		DestroyWindow(toolsWindow);
		toolsWindow = NULL;
	}
	KillToolsOffscreen();
	UpdateToolsCheckmark(false);
#endif
}

//--------------------------------------------------------------  ToggleToolsWindow

void ToggleToolsWindow (void)
{
#ifndef COMPILEDEMO
	if (toolsWindow == NULL)
	{
		OpenToolsWindow();
		isToolsOpen = true;
	}
	else
	{
		CloseToolsWindow();
		isToolsOpen = false;
	}
#endif
}

//--------------------------------------------------------------  SwitchToolModes

#ifndef COMPILEDEMO
void SwitchToolModes (SInt16 newMode)
{
	HWND hwndButton;
	SInt16 index;

	if (toolsWindow == NULL)
		return;

	SelectTool(kSelectTool);
	switch (newMode)
	{
	case kBlowerMode:
		firstTool = kFirstBlower;
		lastTool = kLastBlower;
		objectBase = kBlowerBase;
		break;

	case kFurnitureMode:
		firstTool = kFirstFurniture;
		lastTool = kLastFurniture;
		objectBase = kFurnitureBase;
		break;

	case kBonusMode:
		firstTool = kFirstBonus;
		lastTool = kLastBonus;
		objectBase = kBonusBase;
		break;

	case kTransportMode:
		firstTool = kFirstTransport;
		lastTool = kLastTransport;
		objectBase = kTransportBase;
		break;

	case kSwitchMode:
		firstTool = kFirstSwitch;
		lastTool = kLastSwitch;
		objectBase = kSwitchBase;
		break;

	case kLightMode:
		firstTool = kFirstLight;
		lastTool = kLastLight;
		objectBase = kLightBase;
		break;

	case kApplianceMode:
		firstTool = kFirstAppliance;
		lastTool = kLastAppliance;
		objectBase = kApplianceBase;
		break;

	case kEnemyMode:
		firstTool = kFirstEnemy;
		lastTool = kLastEnemy;
		objectBase = kEnemyBase;
		break;

	case kClutterMode:
		firstTool = kFirstClutter;
		lastTool = kLastClutter;
		objectBase = kClutterBase;
		break;
	}

	for (index = 1; index < kTotalTools; index++)
	{
		hwndButton = GetDlgItem(toolsWindow, kToolButtonBaseID + index);
		if (index <= lastTool)
		{
			ShowWindow(hwndButton, SW_SHOW);
		}
		else
		{
			ShowWindow(hwndButton, SW_HIDE);
		}
	}

	toolMode = newMode;
	UpdateToolTiles();
}
#endif

//--------------------------------------------------------------  NextToolMode

void NextToolMode (void)
{
#ifndef COMPILEDEMO
	if (toolsWindow == NULL)
		return;

	if ((theMode == kEditMode) && (toolMode < kClutterMode))
	{
		toolMode++;
		SetComboBoxMenuValue(toolsWindow, kToolModeCombo, toolMode);
		SwitchToolModes(toolMode);
		toolSelected = kSelectTool;
	}
#endif
}

//--------------------------------------------------------------  PrevToolMode

void PrevToolMode (void)
{
#ifndef COMPILEDEMO
	if (toolsWindow == NULL)
		return;

	if ((theMode == kEditMode) && (toolMode > kBlowerMode))
	{
		toolMode--;
		SetComboBoxMenuValue(toolsWindow, kToolModeCombo, toolMode);
		SwitchToolModes(toolMode);
		toolSelected = kSelectTool;
	}
#endif
}

//--------------------------------------------------------------  SetSpecificToolMode

void SetSpecificToolMode (SInt16 modeToSet)
{
#ifndef COMPILEDEMO
	if ((toolsWindow == NULL) || (theMode != kEditMode))
		return;

	toolMode = modeToSet;
	SetComboBoxMenuValue(toolsWindow, kToolModeCombo, toolMode);
	SwitchToolModes(toolMode);
	toolSelected = kSelectTool;
#endif
}

