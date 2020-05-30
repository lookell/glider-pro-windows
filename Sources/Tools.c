//============================================================================
//----------------------------------------------------------------------------
//								   Tools.c
//----------------------------------------------------------------------------
//============================================================================


//#include <TextUtils.h>
#include "Macintosh.h"
#include "Externs.h"
#include "Environ.h"
#include "RectUtils.h"
#include "ResourceIDs.h"
#include "Utilities.h"


#define kToolsHigh			4
#define kToolsWide			4
#define kTotalTools			16				// kToolsHigh * kToolsWide
#define kPopUpControl		129
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
void FrameSelectedTool (void);
void DrawToolName (void);
void DrawToolTiles (void);
void SwitchToolModes (SInt16);


Rect			toolsWindowRect, toolSrcRect, toolTextRect;
Rect			toolRects[kTotalTools];
ControlHandle	classPopUp;
HDC				toolSrcMap;
HWND			toolsWindow;
SInt16			isToolsH, isToolsV;
SInt16			toolSelected, toolMode;
SInt16			firstTool, lastTool, objectBase;
Boolean			isToolsOpen;


//==============================================================  Functions
//--------------------------------------------------------------  CreateToolsOffscreen

#ifndef COMPILEDEMO
void CreateToolsOffscreen (void)
{
	OSErr		theErr;

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

//--------------------------------------------------------------  FrameSelectedTool

#ifndef COMPILEDEMO
void FrameSelectedTool (void)
{
	return;
#if 0
	Rect		theRect;
	short		toolIcon;

	toolIcon = toolSelected;
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

	theRect = toolRects[toolIcon];
	PenSize(2, 2);
	ForeColor(redColor);
	FrameRect(&theRect);
	PenNormal();
	ForeColor(blackColor);
#endif
}
#endif

//--------------------------------------------------------------  DrawToolName

#ifndef COMPILEDEMO
void DrawToolName (void)
{
	return;
#if 0
	Str255		theString;

	if (toolSelected == 0)
		PasStringCopy("\pSelection Tool", theString);
	else
		GetIndString(theString, kObjectNameStrings,
				toolSelected + ((toolMode - 1) * 0x0010));

	EraseRect(&toolTextRect);
	MoveTo(toolTextRect.left + 3, toolTextRect.bottom - 6);
	TextFont(applFont);
	TextSize(9);
	TextFace(bold);
	ColorText(theString, 171L);
#endif
}
#endif

//--------------------------------------------------------------  DrawToolTiles

#ifndef COMPILEDEMO
void DrawToolTiles (void)
{
	return;
#if 0
	Rect		srcRect, destRect;
	short		i;

	// Selection Tool
	DrawCIcon(kSelectionTool, toolRects[0].left, toolRects[0].top);

	for (i = 0; i < 15; i++)								// Other tools
	{
		QSetRect(&srcRect, 0, 0, 24, 24);
		QSetRect(&destRect, 0, 0, 24, 24);

		QOffsetRect(&srcRect, i * 24, (toolMode - 1) * 24);
		QOffsetRect(&destRect, toolRects[i + 1].left + 2, toolRects[i + 1].top + 2);

		CopyBits((BitMap *)*GetGWorldPixMap(toolSrcMap),
				GetPortBitMapForCopyBits(GetWindowPort(toolsWindow)),
				&srcRect, &destRect, srcCopy, nil);
	}
#endif
}
#endif

//--------------------------------------------------------------  EraseSelectedTool

void EraseSelectedTool (void)
{
	return;
#if 0
#ifndef COMPILEDEMO
	Rect		theRect;
	short		toolIcon;

	if (toolsWindow == nil)
		return;

	SetPort((GrafPtr)toolsWindow);

	toolIcon = toolSelected;
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

	theRect = toolRects[toolIcon];
	PenSize(2, 2);
	ForeColor(whiteColor);
	FrameRect(&theRect);
#endif
#endif
}

//--------------------------------------------------------------  SelectTool

void SelectTool (SInt16 which)
{
	return;
#if 0
#ifndef COMPILEDEMO
	Rect		theRect;
	short		toolIcon;

	if (toolsWindow == nil)
		return;

	SetPort((GrafPtr)toolsWindow);

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

	theRect = toolRects[toolIcon];
	ForeColor(redColor);
	FrameRect(&theRect);
	PenNormal();
	ForeColor(blackColor);

	toolSelected = which;
	DrawToolName();
#endif
#endif
}

//--------------------------------------------------------------  UpdateToolsWindow

void UpdateToolsWindow (void)
{
	return;
#if 0
#ifndef COMPILEDEMO
	if (toolsWindow == nil)
		return;

	SetPortWindowPort(toolsWindow);
	DrawControls(toolsWindow);

	DkGrayForeColor();
	MoveTo(4, 25);
	Line(108, 0);
	ForeColor(blackColor);

	DrawToolTiles();
	FrameSelectedTool();
	DrawToolName();
#endif
#endif
}

//--------------------------------------------------------------  OpenToolsWindow

void OpenToolsWindow (void)
{
	return;
#if 0
#ifndef COMPILEDEMO
	Rect		src, dest;
	Point		globalMouse;
	short		h, v;

	if (toolsWindow == nil)
	{
		QSetRect(&toolsWindowRect, 0, 0, 116, 152);		// 143
		QSetRect(&toolTextRect, 0, 0, 116, 12);
		InsetRect(&toolTextRect, -1, -1);
		QOffsetRect(&toolTextRect, 0, 157 - 15);
		if (thisMac.hasColor)
			toolsWindow = NewCWindow(nil, &toolsWindowRect,
					"\pTools", false, kWindoidWDEF, kPutInFront, true, 0L);
		else
			toolsWindow = NewWindow(nil, &toolsWindowRect,
					"\pTools", false, kWindoidWDEF, kPutInFront, true, 0L);

		if (toolsWindow == nil)
			RedAlert(kErrNoMemory);

//		if (OptionKeyDown())
//		{
//			isToolsH = qd.screenBits.bounds.right - 120;
//			isToolsV = 35;
//		}
		MoveWindow(toolsWindow, isToolsH, isToolsV, true);
		globalMouse = MyGetGlobalMouse();
		QSetRect(&src, 0, 0, 1, 1);
		QOffsetRect(&src, globalMouse.h, globalMouse.v);
		GetWindowRect(toolsWindow, &dest);
		BringToFront(toolsWindow);
		ShowHide(toolsWindow, true);
//		FlagWindowFloating(toolsWindow);	TEMP - use flaoting windows
		HiliteAllWindows();

		classPopUp = GetNewControl(kPopUpControl, toolsWindow);
		if (classPopUp == nil)
			RedAlert(kErrFailedResourceLoad);

		SetControlValue(classPopUp, toolMode);

		for (v = 0; v < kToolsHigh; v++)
			for (h = 0; h < kToolsWide; h++)
			{
				QSetRect(&toolRects[(v * kToolsWide) + h], 2, 29, 30, 57);
				QOffsetRect(&toolRects[(v * kToolsWide) + h], h * 28, v * 28);
			}

		CreateToolsOffscreen();

		SwitchToolModes(toolMode);
		toolSelected = kSelectTool;
	}

	UpdateToolsCheckmark(true);
#endif
#endif
}

//--------------------------------------------------------------  CloseToolsWindow

void CloseToolsWindow (void)
{
	return;
#if 0
#ifndef COMPILEDEMO
	CloseThisWindow(&toolsWindow);
	KillToolsOffscreen();
	UpdateToolsCheckmark(false);
#endif
#endif
}

//--------------------------------------------------------------  ToggleToolsWindow

void ToggleToolsWindow (void)
{
	return;
#if 0
#ifndef COMPILEDEMO
	if (toolsWindow == nil)
	{
		OpenToolsWindow();
		isToolsOpen = true;
	}
	else
	{
		CloseToolsWindow();
		isToolsOpen = true;
	}
#endif
#endif
}

//--------------------------------------------------------------  SwitchToolModes

#ifndef COMPILEDEMO
void SwitchToolModes (SInt16 newMode)
{
	return;
#if 0
	if (toolsWindow == nil)
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

	toolMode = newMode;
	InvalWindowRect(toolsWindow, &toolsWindowRect);
#endif
}
#endif

//--------------------------------------------------------------  HandleToolsClick

void HandleToolsClick (Point wherePt)
{
	return;
#if 0
#ifndef COMPILEDEMO
	ControlHandle	theControl;
	short			i, part, newMode, toolIcon;

	if (toolsWindow == nil)
		return;

	SetPortWindowPort(toolsWindow);
	GlobalToLocal(&wherePt);

	part = FindControl(wherePt, toolsWindow, &theControl);
	if ((theControl != nil) && (part != 0))
	{
		part = TrackControl(theControl, wherePt, (ControlActionUPP)-1L);
		if (part != 0)
		{
			newMode = GetControlValue(theControl);
			if (newMode != toolMode)
			{
				EraseSelectedTool();
				SwitchToolModes(newMode);
			}
		}
	}
	else
	{
		for (i = 0; i < kTotalTools; i++)
			if ((PtInRect(wherePt, &toolRects[i])) && (i <= lastTool))
			{
				EraseSelectedTool();
				toolIcon = i;
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
				break;
			}
	}
#endif
#endif
}

//--------------------------------------------------------------  NextToolMode

void NextToolMode (void)
{
	return;
#if 0
#ifndef COMPILEDEMO
	if (toolsWindow == nil)
		return;

	if ((theMode == kEditMode) && (toolMode < kClutterMode))
	{
		EraseSelectedTool();
		toolMode++;
		SetControlValue(classPopUp, toolMode);
		SwitchToolModes(toolMode);
		toolSelected = kSelectTool;
	}
#endif
#endif
}

//--------------------------------------------------------------  PrevToolMode

void PrevToolMode (void)
{
	return;
#if 0
#ifndef COMPILEDEMO
	if (toolsWindow == nil)
		return;

	if ((theMode == kEditMode) && (toolMode > kBlowerMode))
	{
		EraseSelectedTool();
		toolMode--;
		SetControlValue(classPopUp, toolMode);
		SwitchToolModes(toolMode);
		toolSelected = kSelectTool;
	}
#endif
#endif
}

//--------------------------------------------------------------  SetSpecificToolMode

void SetSpecificToolMode (SInt16 modeToSet)
{
	return;
#if 0
#ifndef COMPILEDEMO
	if ((toolsWindow == nil) || (theMode != kEditMode))
		return;

	EraseSelectedTool();
	toolMode = modeToSet;
	SetControlValue(classPopUp, toolMode);
	SwitchToolModes(toolMode);
	toolSelected = kSelectTool;
#endif
#endif
}

