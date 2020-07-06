#include "Link.h"

//============================================================================
//----------------------------------------------------------------------------
//                                   Link.c
//----------------------------------------------------------------------------
//============================================================================


#include "Environ.h"
#include "House.h"
#include "HouseIO.h"
#include "MainWindow.h"
#include "Menu.h"
#include "ObjectEdit.h"
#include "RectUtils.h"
#include "ResourceIDs.h"
#include "Room.h"
#include "Utilities.h"


#define kLinkButton     1300
#define kUnlinkButton   1310


INT_PTR CALLBACK LinkWindowProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void DoLink (void);
void DoUnlink (void);


HWND			linkWindow;
SInt16			isLinkH, isLinkV, linkRoom, linkType;
Byte			linkObject;
Boolean			isLinkOpen, linkerIsSwitch;


//==============================================================  Functions
//--------------------------------------------------------------  MergeFloorSuite

SInt16 MergeFloorSuite (SInt16 floor, SInt16 suite)
{
	return ((suite * 100) + floor);
}

//--------------------------------------------------------------  ExtractFloorSuite

void ExtractFloorSuite (SInt16 combo, SInt16 *floor, SInt16 *suite)
{
	if (thisHouse.version < 0x0200)		// old floor/suite combo
	{
		*floor = (combo / 100) - kNumUndergroundFloors;
		*suite = combo % 100;
	}
	else
	{
		*suite = combo / 100;
		*floor = (combo % 100) - kNumUndergroundFloors;
	}
}

//--------------------------------------------------------------  LinkWindowProc

INT_PTR CALLBACK LinkWindowProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return FALSE; // don't change the focused window

	case WM_MOVE:
	{
		WINDOWPLACEMENT placement;
		placement.length = sizeof(placement);
		GetWindowPlacement(hDlg, &placement);
		isLinkH = (SInt16)placement.rcNormalPosition.left;
		isLinkV = (SInt16)placement.rcNormalPosition.top;
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

			if (thisRoomNumber == linkRoom)
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
#ifndef COMPILEDEMO
	HWND linkControl;

	if (linkWindow == NULL)
		return;

	linkControl = GetDlgItem(linkWindow, kLinkButton);

	switch (linkType)
	{
	case kSwitchLinkOnly:
		if (objActive == kNoObjectSelected)
		{
			EnableWindow(linkControl, FALSE);
		}
		else
		{
			switch (thisRoom->objects[objActive].what)
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
		}
		break;

	case kTriggerLinkOnly:
		if (objActive == kNoObjectSelected)
		{
			EnableWindow(linkControl, FALSE);
		}
		else
		{
			switch (thisRoom->objects[objActive].what)
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
				if (linkRoom == thisRoomNumber)
					EnableWindow(linkControl, TRUE);
				break;

			default:
				EnableWindow(linkControl, FALSE);
				break;
			}
		}
		break;

	case kTransportLinkOnly:
		if (objActive == kNoObjectSelected)
		{
			EnableWindow(linkControl, FALSE);
		}
		else
		{
			switch (thisRoom->objects[objActive].what)
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
		}
		break;
	}
#endif
}

//--------------------------------------------------------------  OpenLinkWindow

void OpenLinkWindow (void)
{
#ifndef COMPILEDEMO
	WINDOWPLACEMENT placement;

	if (linkWindow == NULL)
	{
		linkWindow = CreateDialog(HINST_THISCOMPONENT,
				MAKEINTRESOURCE(kLinkWindowID),
				mainWindow, LinkWindowProc);
		if (linkWindow == NULL)
			RedAlert(kErrFailedResourceLoad);

		placement.length = sizeof(placement);
		GetWindowPlacement(linkWindow, &placement);
		OffsetRect(&placement.rcNormalPosition,
				-placement.rcNormalPosition.left,
				-placement.rcNormalPosition.top);
		OffsetRect(&placement.rcNormalPosition, isLinkH, isLinkV);
		placement.showCmd = SW_SHOWNOACTIVATE;
		SetWindowPlacement(linkWindow, &placement);

		linkRoom = -1;
		linkObject = 255;
		UpdateLinkControl();

		isLinkOpen = true;
	}
#endif
}

//--------------------------------------------------------------  CloseLinkWindow

void CloseLinkWindow (void)
{
	if (linkWindow != NULL)
		DestroyWindow(linkWindow);

	linkWindow = NULL;
	isLinkOpen = false;
}

//--------------------------------------------------------------  DoLink

#ifndef COMPILEDEMO
void DoLink (void)
{
	SInt16 floor, suite;

	if (GetRoomFloorSuite(thisRoomNumber, &floor, &suite))
	{
		floor += kNumUndergroundFloors;
		if (thisRoomNumber == linkRoom)
		{
			if (linkerIsSwitch)
			{
				thisRoom->objects[linkObject].data.e.where =
						MergeFloorSuite(floor, suite);
				thisRoom->objects[linkObject].data.e.who =
						(Byte)objActive;
			}
			else
			{
				thisRoom->objects[linkObject].data.d.where =
						MergeFloorSuite(floor, suite);
				thisRoom->objects[linkObject].data.d.who =
						(Byte)objActive;
			}
		}
		else
		{
			if (linkerIsSwitch)
			{
				thisHouse.rooms[linkRoom].objects[linkObject].data.e.where =
						MergeFloorSuite(floor, suite);
				thisHouse.rooms[linkRoom].objects[linkObject].data.e.who =
						(Byte)objActive;
			}
			else	// linker is transport
			{
				thisHouse.rooms[linkRoom].objects[linkObject].data.d.where =
						MergeFloorSuite(floor, suite);
				thisHouse.rooms[linkRoom].objects[linkObject].data.d.who =
						(Byte)objActive;
			}
		}
		fileDirty = true;
		UpdateMenus(false);
		CloseLinkWindow();
	}
}
#endif

//--------------------------------------------------------------  DoUnlink

#ifndef COMPILEDEMO
void DoUnlink (void)
{
	if (thisRoomNumber == linkRoom)
	{
		if (linkerIsSwitch)
		{
			thisRoom->objects[linkObject].data.e.where = -1;
			thisRoom->objects[linkObject].data.e.who = 255;
		}
		else
		{
			thisRoom->objects[linkObject].data.d.where = -1;
			thisRoom->objects[linkObject].data.d.who = 255;
		}
	}
	else
	{
		if (linkerIsSwitch)
		{
			thisHouse.rooms[linkRoom].objects[linkObject].data.e.where = -1;
			thisHouse.rooms[linkRoom].objects[linkObject].data.e.who = 255;
		}
		else
		{
			thisHouse.rooms[linkRoom].objects[linkObject].data.d.where = -1;
			thisHouse.rooms[linkRoom].objects[linkObject].data.d.who = 255;
		}
	}
	fileDirty = true;
	UpdateMenus(false);
	CloseLinkWindow();
}
#endif

