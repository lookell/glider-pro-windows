//============================================================================
//----------------------------------------------------------------------------
//                                   Link.c
//----------------------------------------------------------------------------
//============================================================================

#include "Link.h"

#include "House.h"
#include "HouseIO.h"
#include "MainWindow.h"
#include "Menu.h"
#include "ObjectEdit.h"
#include "ResourceIDs.h"
#include "Room.h"
#include "Utilities.h"

#define kLinkButton     1300
#define kUnlinkButton   1310

INT_PTR CALLBACK LinkWindowProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void DoLink (void);
void DoUnlink (void);

HWND g_linkWindow;
SInt16 g_isLinkH;
SInt16 g_isLinkV;
SInt16 g_linkRoom;
SInt16 g_linkType;
Byte g_linkObject;
Boolean g_linkerIsSwitch;

//==============================================================  Functions
//--------------------------------------------------------------  MergeFloorSuite

SInt16 MergeFloorSuite (SInt16 floor, SInt16 suite)
{
	if (g_thisHouse.version < 0x0200) // old floor/suite combo
	{
		return MergeFloorSuiteVer1(floor, suite);
	}
	else
	{
		return MergeFloorSuiteVer2(floor, suite);
	}
}

//--------------------------------------------------------------  MergeFloorSuiteVer1

SInt16 MergeFloorSuiteVer1 (SInt16 floor, SInt16 suite)
{
	floor += kNumUndergroundFloors;
	return ((floor * 100) + suite);
}

//--------------------------------------------------------------  MergeFloorSuiteVer2

SInt16 MergeFloorSuiteVer2 (SInt16 floor, SInt16 suite)
{
	floor += kNumUndergroundFloors;
	return ((suite * 100) + floor);
}

//--------------------------------------------------------------  ExtractFloorSuite

void ExtractFloorSuite (SInt16 combo, SInt16 *floor, SInt16 *suite)
{
	if (g_thisHouse.version < 0x0200) // old floor/suite combo
	{
		ExtractFloorSuiteVer1(combo, floor, suite);
	}
	else
	{
		ExtractFloorSuiteVer2(combo, floor, suite);
	}
}

//--------------------------------------------------------------  ExtractFloorSuiteVer1

void ExtractFloorSuiteVer1 (SInt16 combo, SInt16 *floor, SInt16 *suite)
{
	*floor = (combo / 100) - kNumUndergroundFloors;
	*suite = combo % 100;
}

//--------------------------------------------------------------  ExtractFloorSuiteVer2

void ExtractFloorSuiteVer2 (SInt16 combo, SInt16 *floor, SInt16 *suite)
{
	*suite = combo / 100;
	*floor = (combo % 100) - kNumUndergroundFloors;
}

//--------------------------------------------------------------  LinkWindowProc

INT_PTR CALLBACK LinkWindowProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	(void)lParam;

	switch (message)
	{
	case WM_INITDIALOG:
		return FALSE; // don't change the focused window

	case WM_MOVE:
	{
		WINDOWPLACEMENT placement;
		placement.length = sizeof(placement);
		GetWindowPlacement(hDlg, &placement);
		g_isLinkH = (SInt16)placement.rcNormalPosition.left;
		g_isLinkV = (SInt16)placement.rcNormalPosition.top;
		return TRUE;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			CloseLinkWindow();
			break;

		case kLinkButton:
		case kUnlinkButton:
			if (LOWORD(wParam) == kLinkButton)
				DoLink();
			else if (LOWORD(wParam) == kUnlinkButton)
				DoUnlink();

			if (g_thisRoomNumber == g_linkRoom)
				CopyThisRoomToRoom();
			GenerateRetroLinks();
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  UpdateLinkControl

void UpdateLinkControl (void)
{
	HWND linkControl;

	if (COMPILEDEMO)
		return;

	if (g_linkWindow == NULL)
		return;

	linkControl = GetDlgItem(g_linkWindow, kLinkButton);

	if ((g_objActive == kNoObjectSelected) ||
		(g_objActive == kInitialGliderSelected) ||
		(g_objActive == kLeftGliderSelected) ||
		(g_objActive == kRightGliderSelected))
	{
		EnableWindow(linkControl, FALSE);
		return;
	}
	if (g_objActive < 0 || g_objActive >= kMaxRoomObs)
	{
		EnableWindow(linkControl, FALSE);
		return;
	}

	switch (g_linkType)
	{
	case kSwitchLinkOnly:
		switch (g_thisRoom->objects[g_objActive].what)
		{
		case kFloorVent:
		case kCeilingVent:
		case kFloorBlower:
		case kCeilingBlower:
		case kSewerGrate:
		case kLeftFan:
		case kRightFan:
		case kInvisBlower:
		case kGrecoVent:
		case kSewerBlower:
		case kLiftArea:
		case kRedClock:
		case kBlueClock:
		case kYellowClock:
		case kCuckoo:
		case kPaper:
		case kBattery:
		case kBands:
		case kFoil:
		case kInvisBonus:
		case kHelium:
		case kDeluxeTrans:
		case kCeilingLight:
		case kLightBulb:
		case kTableLamp:
		case kHipLamp:
		case kDecoLamp:
		case kFlourescent:
		case kTrackLight:
		case kInvisLight:
		case kShredder:
		case kToaster:
		case kMacPlus:
		case kTV:
		case kCoffee:
		case kOutlet:
		case kVCR:
		case kStereo:
		case kMicrowave:
		case kBalloon:
		case kCopterLf:
		case kCopterRt:
		case kDartLf:
		case kDartRt:
		case kBall:
		case kDrip:
		case kFish:
			EnableWindow(linkControl, TRUE);
			break;

		default:
			EnableWindow(linkControl, FALSE);
			break;
		}
		break;

	case kTriggerLinkOnly:
		switch (g_thisRoom->objects[g_objActive].what)
		{
		case kGreaseRt:
		case kGreaseLf:
		case kToaster:
		case kGuitar:
		case kCoffee:
		case kOutlet:
		case kBalloon:
		case kCopterLf:
		case kCopterRt:
		case kDartLf:
		case kDartRt:
		case kDrip:
		case kFish:
			EnableWindow(linkControl, TRUE);
			break;

		case kLightSwitch:
		case kMachineSwitch:
		case kThermostat:
		case kPowerSwitch:
		case kKnifeSwitch:
		case kInvisSwitch:
			EnableWindow(linkControl, (g_linkRoom == g_thisRoomNumber));
			break;

		default:
			EnableWindow(linkControl, FALSE);
			break;
		}
		break;

	case kTransportLinkOnly:
		switch (g_thisRoom->objects[g_objActive].what)
		{
		case kMailboxLf:
		case kMailboxRt:
		case kCeilingTrans:
		case kInvisTrans:
		case kDeluxeTrans:
		case kInvisLight:
		case kOzma:
		case kMirror:
		case kFireplace:
		case kWallWindow:
		case kCalendar:
		case kBulletin:
		case kCloud:
			EnableWindow(linkControl, TRUE);
			break;

		default:
			EnableWindow(linkControl, FALSE);
			break;
		}
		break;
	}
}

//--------------------------------------------------------------  OpenLinkWindow

void OpenLinkWindow (void)
{
	WINDOWPLACEMENT placement;

	if (COMPILEDEMO)
		return;

	if (g_linkWindow == NULL)
	{
		g_linkWindow = CreateDialog(HINST_THISCOMPONENT,
				MAKEINTRESOURCE(kLinkWindowID),
				g_mainWindow, LinkWindowProc);
		if (g_linkWindow == NULL)
			RedAlert(kErrFailedResourceLoad);

		placement.length = sizeof(placement);
		GetWindowPlacement(g_linkWindow, &placement);
		OffsetRect(&placement.rcNormalPosition,
				-placement.rcNormalPosition.left,
				-placement.rcNormalPosition.top);
		OffsetRect(&placement.rcNormalPosition, g_isLinkH, g_isLinkV);
		placement.showCmd = SW_SHOWNOACTIVATE;
		SetWindowPlacement(g_linkWindow, &placement);

		g_linkRoom = -1;
		g_linkObject = 255;
		UpdateLinkControl();
	}
}

//--------------------------------------------------------------  CloseLinkWindow

void CloseLinkWindow (void)
{
	if (g_linkWindow != NULL)
		DestroyWindow(g_linkWindow);

	g_linkWindow = NULL;
}

//--------------------------------------------------------------  DoLink

void DoLink (void)
{
	SInt16 floor, suite;

	if (GetRoomFloorSuite(g_thisRoomNumber, &floor, &suite))
	{
		if (g_thisRoomNumber == g_linkRoom)
		{
			if (g_linkerIsSwitch)
			{
				g_thisRoom->objects[g_linkObject].data.e.where =
						MergeFloorSuite(floor, suite);
				g_thisRoom->objects[g_linkObject].data.e.who =
						(Byte)g_objActive;
			}
			else
			{
				g_thisRoom->objects[g_linkObject].data.d.where =
						MergeFloorSuite(floor, suite);
				g_thisRoom->objects[g_linkObject].data.d.who =
						(Byte)g_objActive;
			}
		}
		else
		{
			if (g_linkerIsSwitch)
			{
				g_thisHouse.rooms[g_linkRoom].objects[g_linkObject].data.e.where =
						MergeFloorSuite(floor, suite);
				g_thisHouse.rooms[g_linkRoom].objects[g_linkObject].data.e.who =
						(Byte)g_objActive;
			}
			else  // linker is transport
			{
				g_thisHouse.rooms[g_linkRoom].objects[g_linkObject].data.d.where =
						MergeFloorSuite(floor, suite);
				g_thisHouse.rooms[g_linkRoom].objects[g_linkObject].data.d.who =
						(Byte)g_objActive;
			}
		}
		g_fileDirty = true;
		UpdateMenus(false);
		CloseLinkWindow();
	}
}

//--------------------------------------------------------------  DoUnlink

void DoUnlink (void)
{
	if (g_thisRoomNumber == g_linkRoom)
	{
		if (g_linkerIsSwitch)
		{
			g_thisRoom->objects[g_linkObject].data.e.where = -1;
			g_thisRoom->objects[g_linkObject].data.e.who = 255;
		}
		else
		{
			g_thisRoom->objects[g_linkObject].data.d.where = -1;
			g_thisRoom->objects[g_linkObject].data.d.who = 255;
		}
	}
	else
	{
		if (g_linkerIsSwitch)
		{
			g_thisHouse.rooms[g_linkRoom].objects[g_linkObject].data.e.where = -1;
			g_thisHouse.rooms[g_linkRoom].objects[g_linkObject].data.e.who = 255;
		}
		else
		{
			g_thisHouse.rooms[g_linkRoom].objects[g_linkObject].data.d.where = -1;
			g_thisHouse.rooms[g_linkRoom].objects[g_linkObject].data.d.who = 255;
		}
	}
	g_fileDirty = true;
	UpdateMenus(false);
	CloseLinkWindow();
}
