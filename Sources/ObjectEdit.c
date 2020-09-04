#define GP_USE_WINAPI_H

#include "ObjectEdit.h"

//============================================================================
//----------------------------------------------------------------------------
//                                ObjectEdit.c
//----------------------------------------------------------------------------
//============================================================================


#include "ColorUtils.h"
#include "Events.h"
#include "House.h"
#include "HouseIO.h"
#include "HouseLegal.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "Marquee.h"
#include "Menu.h"
#include "Music.h"
#include "ObjectAdd.h"
#include "ObjectDraw.h"
#include "ObjectInfo.h"
#include "Objects.h"
#include "Play.h"
#include "Player.h"
#include "RectUtils.h"
#include "ResourceIDs.h"
#include "Room.h"
#include "RoomInfo.h"
#include "StringUtils.h"
#include "Tools.h"
#include "Utilities.h"
#include "WindowUtils.h"


#define k8BlackColor            255


SInt16 FindObjectSelected (Point where);
void DragHandle (Point where);
void Gp_DragObject (Point where);
void AddObjectPairing (void);
Boolean ObjectIsUpBlower (const objectType *who);


Rect roomObjectRects[kMaxRoomObs];
Rect leftStartGliderSrc;
Rect rightStartGliderSrc;
SInt16 objActive;

static Rect initialGliderRect;
static Rect leftStartGliderDest;
static Rect rightStartGliderDest;
static Boolean isFirstRoom;


//==============================================================  Functions
//--------------------------------------------------------------  FindObjectSelected

#ifndef COMPILEDEMO
SInt16 FindObjectSelected (Point where)
{
	SInt16		found, i;

	found = kNoObjectSelected;

	if (QPtInRect(where, &initialGliderRect))
		return (kInitialGliderSelected);
	else if (QPtInRect(where, &leftStartGliderDest))
		return (kLeftGliderSelected);
	else if (QPtInRect(where, &rightStartGliderDest))
		return (kRightGliderSelected);

	for (i = kMaxRoomObs - 1; i >= 0; i--)
	{
		if (QPtInRect(where, &roomObjectRects[i]))
		{
			found = i;
			break;
		}
	}
	return (found);
}
#endif

//--------------------------------------------------------------  DoSelectionClick

void DoSelectionClick (HWND hwnd, Point where, Boolean isDoubleClick)
{
#ifndef COMPILEDEMO
	SInt16		direction, dist;
	POINT		dragPt;

	StopMarquee();

	dragPt.x = where.h;
	dragPt.y = where.v;
	ClientToScreen(hwnd, &dragPt);
	if ((PtInMarqueeHandle(where)) && (objActive != kNoObjectSelected))
	{
		if (DragDetect(hwnd, dragPt))
			DragHandle(where);
		if (ObjectHasHandle(&direction, &dist))
		{
			StartMarqueeHandled(&roomObjectRects[objActive], direction, dist);
			HandleBlowerGlider();
		}
		else
			StartMarquee(&roomObjectRects[objActive]);
	}
	else
	{
		objActive = FindObjectSelected(where);
		if (objActive == kNoObjectSelected)
		{
			if (isDoubleClick)
				DoRoomInfo(hwnd);
		}
		else
		{
			if (isDoubleClick)
			{
				DoObjectInfo(hwnd);
				if (ObjectHasHandle(&direction, &dist))
				{
					StartMarqueeHandled(&roomObjectRects[objActive], direction, dist);
					HandleBlowerGlider();
				}
				else
					StartMarquee(&roomObjectRects[objActive]);
			}
			else
			{

				if (DragDetect(hwnd, dragPt))
					Gp_DragObject(where);
				if (ObjectHasHandle(&direction, &dist))
				{
					StartMarqueeHandled(&roomObjectRects[objActive], direction, dist);
					HandleBlowerGlider();
				}
				else
				{
					if (objActive == kInitialGliderSelected)
						StartMarquee(&initialGliderRect);
					else if (objActive == kLeftGliderSelected)
						StartMarquee(&leftStartGliderDest);
					else if (objActive == kRightGliderSelected)
						StartMarquee(&rightStartGliderDest);
					else
						StartMarquee(&roomObjectRects[objActive]);
				}
			}
		}
	}
	UpdateMenus(false);
#endif
}

//--------------------------------------------------------------  DragHandle

#ifndef COMPILEDEMO
void DragHandle (Point where)
{
	SInt16		hDelta, vDelta;
	Boolean		whoCares;

	switch (thisRoom->objects[objActive].what)
	{
		case kFloorVent:
		case kCeilingVent:
		case kFloorBlower:
		case kCeilingBlower:
		case kSewerGrate:
		case kTaper:
		case kCandle:
		case kStubby:
		case kTiki:
		case kBBQ:
		case kGrecoVent:
		case kSewerBlower:
		vDelta = thisRoom->objects[objActive].data.a.distance;
		DragMarqueeHandle(where, &vDelta);
		thisRoom->objects[objActive].data.a.distance = vDelta;
		whoCares = KeepObjectLegal();
		break;

		case kLiftArea:
		hDelta = thisRoom->objects[objActive].data.a.distance;
		vDelta = thisRoom->objects[objActive].data.a.tall * 2;
		DragMarqueeCorner(where, &hDelta, &vDelta, false);
		thisRoom->objects[objActive].data.a.distance = hDelta;
		thisRoom->objects[objActive].data.a.tall = (Byte)(vDelta / 2);
		whoCares = KeepObjectLegal();
		Mac_InvalWindowRect(mainWindow, &mainWindowRect);
		GetThisRoomsObjRects();
		ReadyBackground(thisRoom->background, thisRoom->tiles);
		DrawThisRoomsObjects();
		break;

		case kLeftFan:
		case kRightFan:
		hDelta = thisRoom->objects[objActive].data.a.distance;
		DragMarqueeHandle(where, &hDelta);
		thisRoom->objects[objActive].data.a.distance = hDelta;
		whoCares = KeepObjectLegal();
		break;

		case kInvisBlower:
		if (((thisRoom->objects[objActive].data.a.vector & 0x0F) == 1) ||
				((thisRoom->objects[objActive].data.a.vector & 0x0F) == 4))
		{
			vDelta = thisRoom->objects[objActive].data.a.distance;
			DragMarqueeHandle(where, &vDelta);
			thisRoom->objects[objActive].data.a.distance = vDelta;
		}
		else
		{
			hDelta = thisRoom->objects[objActive].data.a.distance;
			DragMarqueeHandle(where, &hDelta);
			thisRoom->objects[objActive].data.a.distance = hDelta;
		}
		whoCares = KeepObjectLegal();
		break;

		case kTable:
		case kShelf:
		case kDeckTable:
		hDelta = RectWide(&thisRoom->objects[objActive].data.b.bounds);
		DragMarqueeHandle(where, &hDelta);
		thisRoom->objects[objActive].data.b.bounds.right =
			thisRoom->objects[objActive].data.b.bounds.left + hDelta;
		whoCares = KeepObjectLegal();
		Mac_InvalWindowRect(mainWindow, &mainWindowRect);
		GetThisRoomsObjRects();
		ReadyBackground(thisRoom->background, thisRoom->tiles);
		DrawThisRoomsObjects();
		break;

		case kCabinet:
		case kInvisObstacle:
		case kInvisBounce:
		hDelta = RectWide(&thisRoom->objects[objActive].data.b.bounds);
		vDelta = RectTall(&thisRoom->objects[objActive].data.b.bounds);
		DragMarqueeCorner(where, &hDelta, &vDelta, false);
		thisRoom->objects[objActive].data.b.bounds.right =
			thisRoom->objects[objActive].data.b.bounds.left + hDelta;
		thisRoom->objects[objActive].data.b.bounds.bottom =
			thisRoom->objects[objActive].data.b.bounds.top + vDelta;
		whoCares = KeepObjectLegal();
		Mac_InvalWindowRect(mainWindow, &mainWindowRect);
		GetThisRoomsObjRects();
		ReadyBackground(thisRoom->background, thisRoom->tiles);
		DrawThisRoomsObjects();
		break;

		case kCounter:
		case kDresser:
		hDelta = RectWide(&thisRoom->objects[objActive].data.b.bounds);
		vDelta = RectTall(&thisRoom->objects[objActive].data.b.bounds);
		DragMarqueeCorner(where, &hDelta, &vDelta, true);
		thisRoom->objects[objActive].data.b.bounds.right =
			thisRoom->objects[objActive].data.b.bounds.left + hDelta;
		thisRoom->objects[objActive].data.b.bounds.top =
			thisRoom->objects[objActive].data.b.bounds.bottom - vDelta;
		whoCares = KeepObjectLegal();
		Mac_InvalWindowRect(mainWindow, &mainWindowRect);
		GetThisRoomsObjRects();
		ReadyBackground(thisRoom->background, thisRoom->tiles);
		DrawThisRoomsObjects();
		break;

		case kGreaseRt:
		case kGreaseLf:
		case kSlider:
		hDelta = thisRoom->objects[objActive].data.c.length;
		DragMarqueeHandle(where, &hDelta);
		thisRoom->objects[objActive].data.c.length = hDelta;
		whoCares = KeepObjectLegal();
		Mac_InvalWindowRect(mainWindow, &mainWindowRect);
		GetThisRoomsObjRects();
		ReadyBackground(thisRoom->background, thisRoom->tiles);
		DrawThisRoomsObjects();
		break;

		case kInvisTrans:
		hDelta = thisRoom->objects[objActive].data.d.wide;
		vDelta = thisRoom->objects[objActive].data.d.tall;
		DragMarqueeCorner(where, &hDelta, &vDelta, false);
		if (hDelta > 127)
			hDelta = 127;
		thisRoom->objects[objActive].data.d.wide = (Byte)hDelta;
		thisRoom->objects[objActive].data.d.tall = vDelta;
		whoCares = KeepObjectLegal();
		Mac_InvalWindowRect(mainWindow, &mainWindowRect);
		GetThisRoomsObjRects();
		ReadyBackground(thisRoom->background, thisRoom->tiles);
		DrawThisRoomsObjects();
		break;

		case kDeluxeTrans:
		hDelta = ((thisRoom->objects[objActive].data.d.tall & 0xFF00) >> 8) * 4;
		vDelta = (thisRoom->objects[objActive].data.d.tall & 0x00FF) * 4;
		DragMarqueeCorner(where, &hDelta, &vDelta, false);
		if (hDelta < 64)
			hDelta = 64;
		if (vDelta < 32)
			vDelta = 32;
		thisRoom->objects[objActive].data.d.tall = ((hDelta / 4) << 8) + (vDelta / 4);
		whoCares = KeepObjectLegal();
		Mac_InvalWindowRect(mainWindow, &mainWindowRect);
		GetThisRoomsObjRects();
		ReadyBackground(thisRoom->background, thisRoom->tiles);
		DrawThisRoomsObjects();
		break;

		case kFlourescent:
		case kTrackLight:
		hDelta = thisRoom->objects[objActive].data.f.length;
		DragMarqueeHandle(where, &hDelta);
		thisRoom->objects[objActive].data.f.length = hDelta;
		whoCares = KeepObjectLegal();
		Mac_InvalWindowRect(mainWindow, &mainWindowRect);
		GetThisRoomsObjRects();
		ReadyBackground(thisRoom->background, thisRoom->tiles);
		DrawThisRoomsObjects();
		break;

		case kToaster:
		vDelta = thisRoom->objects[objActive].data.g.height;
		DragMarqueeHandle(where, &vDelta);
		thisRoom->objects[objActive].data.g.height = vDelta;
		whoCares = KeepObjectLegal();
		break;

		case kBall:
		case kDrip:
		case kFish:
		vDelta = thisRoom->objects[objActive].data.h.length;
		DragMarqueeHandle(where, &vDelta);
		thisRoom->objects[objActive].data.h.length = vDelta;
		whoCares = KeepObjectLegal();
		break;

		case kMirror:
		case kWallWindow:
		hDelta = RectWide(&thisRoom->objects[objActive].data.i.bounds);
		vDelta = RectTall(&thisRoom->objects[objActive].data.i.bounds);
		DragMarqueeCorner(where, &hDelta, &vDelta, false);
		thisRoom->objects[objActive].data.i.bounds.right =
			thisRoom->objects[objActive].data.i.bounds.left + hDelta;
		thisRoom->objects[objActive].data.i.bounds.bottom =
			thisRoom->objects[objActive].data.i.bounds.top + vDelta;
		whoCares = KeepObjectLegal();
		Mac_InvalWindowRect(mainWindow, &mainWindowRect);
		GetThisRoomsObjRects();
		ReadyBackground(thisRoom->background, thisRoom->tiles);
		DrawThisRoomsObjects();
		break;
	}

	fileDirty = true;
	UpdateMenus(false);
}
#endif

//--------------------------------------------------------------  DragObject

#ifndef COMPILEDEMO
void Gp_DragObject (Point where)
{
	Rect		newRect, wasRect;
	SInt16		deltaH, deltaV, increment;
	Boolean		invalAll;

	invalAll = false;

	if (objActive == kInitialGliderSelected)
	{
		wasRect = initialGliderRect;
		newRect = initialGliderRect;
		DragMarqueeRect(where, &newRect, false, false);
	}
	else if (objActive == kLeftGliderSelected)
	{
		wasRect = leftStartGliderDest;
		newRect = leftStartGliderDest;
		DragMarqueeRect(where, &newRect, false, true);
	}
	else if (objActive == kRightGliderSelected)
	{
		wasRect = rightStartGliderDest;
		newRect = rightStartGliderDest;
		DragMarqueeRect(where, &newRect, false, true);
	}
	else
	{
		wasRect = roomObjectRects[objActive];
		newRect = roomObjectRects[objActive];
		switch (thisRoom->objects[objActive].what)
		{
			case kFloorVent:
			case kCeilingVent:
			case kFloorBlower:
			case kCeilingBlower:
			case kSewerGrate:
			case kGrecoVent:
			case kSewerBlower:
			case kManhole:
			case kUpStairs:
			case kDownStairs:
			case kCeilingLight:
			case kHipLamp:
			case kDecoLamp:
			case kFlourescent:
			case kFloorTrans:
			case kCeilingTrans:
			case kDoorInLf:
			case kDoorInRt:
			case kDoorExRt:
			case kDoorExLf:
			case kWindowInLf:
			case kWindowInRt:
			case kWindowExRt:
			case kWindowExLf:
			case kBalloon:
			case kCopterLf:
			case kCopterRt:
			case kMousehole:
			case kFireplace:
			DragMarqueeRect(where, &newRect, true, false);
			invalAll = false;
			break;

			case kDartLf:
			case kDartRt:
			DragMarqueeRect(where, &newRect, false, true);
			invalAll = false;
			break;

			case kTiki:
			case kTable:
			case kShelf:
			case kCabinet:
			case kDeckTable:
			case kStool:
			case kInvisObstacle:
			case kInvisBounce:
			case kGreaseRt:
			case kGreaseLf:
			case kSlider:
			case kMailboxLf:
			case kMailboxRt:
			case kInvisTrans:
			case kDeluxeTrans:
			case kMirror:
			case kWallWindow:
			DragMarqueeRect(where, &newRect, false, false);
			invalAll = true;
			break;

			case kCounter:
			case kDresser:
			case kTrackLight:
			DragMarqueeRect(where, &newRect, true, false);
			invalAll = true;
			break;

			case kRedClock:
			case kBlueClock:
			case kYellowClock:
			case kCuckoo:
			case kPaper:
			case kBattery:
			case kBands:
			case kFoil:
			case kInvisBonus:
			case kStar:
			case kSparkle:
			case kHelium:
			case kLeftFan:
			case kRightFan:
			case kTaper:
			case kCandle:
			case kStubby:
			case kBBQ:
			case kInvisBlower:
			case kLiftArea:
			case kFilingCabinet:
			case kWasteBasket:
			case kMilkCrate:
			case kTrunk:
			case kBooks:
			case kLightSwitch:
			case kMachineSwitch:
			case kThermostat:
			case kPowerSwitch:
			case kKnifeSwitch:
			case kInvisSwitch:
			case kTrigger:
			case kLgTrigger:
			case kSoundTrigger:
			case kLightBulb:
			case kTableLamp:
			case kInvisLight:
			case kShredder:
			case kToaster:
			case kMacPlus:
			case kGuitar:
			case kTV:
			case kCoffee:
			case kOutlet:
			case kVCR:
			case kStereo:
			case kMicrowave:
			case kCinderBlock:
			case kFlowerBox:
			case kCDs:
			case kCustomPict:
			case kBall:
			case kDrip:
			case kFish:
			case kCobweb:
			case kOzma:
			case kFlower:
			case kBear:
			case kCalendar:
			case kVase1:
			case kVase2:
			case kBulletin:
			case kCloud:
			case kFaucet:
			case kRug:
			case kChimes:
			DragMarqueeRect(where, &newRect, false, false);
			invalAll = false;
			break;
		}
	}

	deltaH = newRect.left - wasRect.left;
	deltaV = newRect.top - wasRect.top;
	if ((deltaH != 0) || (deltaV != 0))
	{
		fileDirty = true;
		UpdateMenus(false);
	}

	if (objActive == kInitialGliderSelected)
	{
		thisHouse.initial.h += deltaH;
		thisHouse.initial.v += deltaV;
	}
	else if (objActive == kLeftGliderSelected)
	{
		increment = thisRoom->leftStart + deltaV;
		if (increment > 255)
			increment = 255;
		else if (increment < 0)
			increment = 0;
		thisRoom->leftStart = (Byte)increment;
		QSetRect(&leftStartGliderDest, 0, 0, 48, 16);
		QOffsetRect(&leftStartGliderDest, 0,
				kGliderStartsDown + (SInt16)thisRoom->leftStart);
	}
	else if (objActive == kRightGliderSelected)
	{
		increment = thisRoom->rightStart + deltaV;
		if (increment > 255)
			increment = 255;
		else if (increment < 0)
			increment = 0;
		thisRoom->rightStart = (Byte)increment;
		QSetRect(&rightStartGliderDest, 0, 0, 48, 16);
		QOffsetRect(&rightStartGliderDest, 0,
				kGliderStartsDown + (SInt16)thisRoom->rightStart);
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
			case kGrecoVent:
			case kSewerBlower:
			thisRoom->objects[objActive].data.a.topLeft.h += deltaH;
			break;

			case kLeftFan:
			case kRightFan:
			case kTaper:
			case kCandle:
			case kStubby:
			case kTiki:
			case kBBQ:
			case kInvisBlower:
			case kLiftArea:
			thisRoom->objects[objActive].data.a.topLeft.h += deltaH;
			thisRoom->objects[objActive].data.a.topLeft.v += deltaV;
			break;

			case kTable:
			case kShelf:
			case kCabinet:
			case kFilingCabinet:
			case kWasteBasket:
			case kMilkCrate:
			case kDeckTable:
			case kStool:
			case kTrunk:
			case kInvisObstacle:
			case kBooks:
			case kInvisBounce:
			thisRoom->objects[objActive].data.b.bounds.left += deltaH;
			thisRoom->objects[objActive].data.b.bounds.right += deltaH;
			thisRoom->objects[objActive].data.b.bounds.top += deltaV;
			thisRoom->objects[objActive].data.b.bounds.bottom += deltaV;
			break;

			case kCounter:
			case kDresser:
			case kManhole:
			thisRoom->objects[objActive].data.b.bounds.left += deltaH;
			thisRoom->objects[objActive].data.b.bounds.right += deltaH;
			break;

			case kRedClock:
			case kBlueClock:
			case kYellowClock:
			case kCuckoo:
			case kPaper:
			case kBattery:
			case kBands:
			case kGreaseRt:
			case kGreaseLf:
			case kFoil:
			case kInvisBonus:
			case kStar:
			case kSparkle:
			case kHelium:
			case kSlider:
			thisRoom->objects[objActive].data.c.topLeft.h += deltaH;
			thisRoom->objects[objActive].data.c.topLeft.v += deltaV;
			break;

			case kUpStairs:
			case kDownStairs:
			case kFloorTrans:
			case kCeilingTrans:
			case kDoorInLf:
			case kDoorInRt:
			case kDoorExRt:
			case kDoorExLf:
			case kWindowInLf:
			case kWindowInRt:
			case kWindowExRt:
			case kWindowExLf:
			thisRoom->objects[objActive].data.d.topLeft.h += deltaH;
			break;

			case kMailboxLf:
			case kMailboxRt:
			case kInvisTrans:
			case kDeluxeTrans:
			thisRoom->objects[objActive].data.d.topLeft.h += deltaH;
			thisRoom->objects[objActive].data.d.topLeft.v += deltaV;
			break;

			case kLightSwitch:
			case kMachineSwitch:
			case kThermostat:
			case kPowerSwitch:
			case kKnifeSwitch:
			case kInvisSwitch:
			case kTrigger:
			case kLgTrigger:
			case kSoundTrigger:
			thisRoom->objects[objActive].data.e.topLeft.h += deltaH;
			thisRoom->objects[objActive].data.e.topLeft.v += deltaV;
			break;

			case kCeilingLight:
			case kHipLamp:
			case kDecoLamp:
			case kFlourescent:
			case kTrackLight:
			thisRoom->objects[objActive].data.f.topLeft.h += deltaH;
			break;

			case kLightBulb:
			case kTableLamp:
			case kInvisLight:
			thisRoom->objects[objActive].data.f.topLeft.h += deltaH;
			thisRoom->objects[objActive].data.f.topLeft.v += deltaV;
			break;

			case kShredder:
			case kToaster:
			case kMacPlus:
			case kGuitar:
			case kTV:
			case kCoffee:
			case kOutlet:
			case kVCR:
			case kStereo:
			case kMicrowave:
			case kCinderBlock:
			case kFlowerBox:
			case kCDs:
			case kCustomPict:
			thisRoom->objects[objActive].data.g.topLeft.h += deltaH;
			thisRoom->objects[objActive].data.g.topLeft.v += deltaV;
			break;

			case kBalloon:
			case kCopterLf:
			case kCopterRt:
			thisRoom->objects[objActive].data.h.topLeft.h += deltaH;
			break;

			case kDartLf:
			case kDartRt:
			thisRoom->objects[objActive].data.h.topLeft.v += deltaV;
			break;

			case kBall:
			case kDrip:
			case kFish:
			case kCobweb:
			thisRoom->objects[objActive].data.h.topLeft.h += deltaH;
			thisRoom->objects[objActive].data.h.topLeft.v += deltaV;
			break;

			case kOzma:
			case kMirror:
			case kFlower:
			case kWallWindow:
			case kBear:
			case kCalendar:
			case kVase1:
			case kVase2:
			case kBulletin:
			case kCloud:
			case kFaucet:
			case kRug:
			case kChimes:
			thisRoom->objects[objActive].data.i.bounds.left += deltaH;
			thisRoom->objects[objActive].data.i.bounds.right += deltaH;
			thisRoom->objects[objActive].data.i.bounds.top += deltaV;
			thisRoom->objects[objActive].data.i.bounds.bottom += deltaV;
			break;

			case kMousehole:
			case kFireplace:
			thisRoom->objects[objActive].data.i.bounds.left += deltaH;
			thisRoom->objects[objActive].data.i.bounds.right += deltaH;
			break;
		}
	}

	if (KeepObjectLegal())
	{
	}
	GetThisRoomsObjRects();
	if (invalAll)
		Mac_InvalWindowRect(mainWindow, &mainWindowRect);
	else
	{
		Mac_InvalWindowRect(mainWindow, &wasRect);
		if (objActive == kInitialGliderSelected)
			Mac_InvalWindowRect(mainWindow, &initialGliderRect);
		else if (objActive == kLeftGliderSelected)
			Mac_InvalWindowRect(mainWindow, &leftStartGliderDest);
		else if (objActive == kRightGliderSelected)
			Mac_InvalWindowRect(mainWindow, &rightStartGliderDest);
		else
			Mac_InvalWindowRect(mainWindow, &roomObjectRects[objActive]);
	}

	ReadyBackground(thisRoom->background, thisRoom->tiles);
	DrawThisRoomsObjects();
}
#endif

//--------------------------------------------------------------  DoNewObjectClick

void DoNewObjectClick (HWND ownerWindow, Point where)
{
#ifndef COMPILEDEMO
	SInt16		whatObject;

	StopMarquee();
	objActive = kNoObjectSelected;

	whatObject = toolSelected + ((toolMode - 1) * 0x0010);
	if (AddNewObject(ownerWindow, where, whatObject, true))
		IgnoreThisClick();
	UpdateMenus(false);

	AddObjectPairing();
#endif
}

//--------------------------------------------------------------  AddObjectPairing

void AddObjectPairing (void)
{
#ifndef COMPILEDEMO
	roomType *testRoomPtr;
	SInt16 roomNum;
	SInt16 emptySlot;
	wchar_t message[256];

	if (thisRoom->objects[objActive].what == kDoorInRt)
	{
		roomNum = DoesNeighborRoomExist(kRoomToRight);
		if (roomNum != -1)
		{
			emptySlot = FindObjectSlotInRoom(roomNum);
			if ((emptySlot != -1) && (!DoesRoomNumHaveObject(roomNum, kDoorExLf)))
			{
				testRoomPtr = &(thisHouse.rooms[roomNum]);

				testRoomPtr->objects[emptySlot].what = kDoorExLf;
				testRoomPtr->objects[emptySlot].data.d.topLeft.h = kDoorExLfLeft;
				testRoomPtr->objects[emptySlot].data.d.topLeft.v = kDoorExTop;
				testRoomPtr->objects[emptySlot].data.d.tall = 0;
				testRoomPtr->objects[emptySlot].data.d.where = -1;
				testRoomPtr->objects[emptySlot].data.d.who = 255;
				testRoomPtr->objects[emptySlot].data.d.wide = 0;

				testRoomPtr->numObjects++;

				GetLocalizedString(45, message, ARRAYSIZE(message));
				OpenMessageWindow(message);
				SetMessageTextColor(blueColor);
				GetLocalizedString(46, message, ARRAYSIZE(message));
				SetMessageWindowMessage(message);
				DelayTicks(60);
				CloseMessageWindow();
			}
		}
	}
	else if (thisRoom->objects[objActive].what == kDoorInLf)
	{
		roomNum = DoesNeighborRoomExist(kRoomToLeft);
		if (roomNum != -1)
		{
			emptySlot = FindObjectSlotInRoom(roomNum);
			if ((emptySlot != -1) && (!DoesRoomNumHaveObject(roomNum, kDoorExRt)))
			{
				testRoomPtr = &(thisHouse.rooms[roomNum]);

				testRoomPtr->objects[emptySlot].what = kDoorExRt;
				testRoomPtr->objects[emptySlot].data.d.topLeft.h = kDoorExRtLeft;
				testRoomPtr->objects[emptySlot].data.d.topLeft.v = kDoorExTop;
				testRoomPtr->objects[emptySlot].data.d.tall = 0;
				testRoomPtr->objects[emptySlot].data.d.where = -1;
				testRoomPtr->objects[emptySlot].data.d.who = 255;
				testRoomPtr->objects[emptySlot].data.d.wide = 0;

				testRoomPtr->numObjects++;

				GetLocalizedString(45, message, ARRAYSIZE(message));
				OpenMessageWindow(message);
				SetMessageTextColor(blueColor);
				GetLocalizedString(46, message, ARRAYSIZE(message));
				SetMessageWindowMessage(message);
				DelayTicks(60);
				CloseMessageWindow();
			}
		}
	}
	else if (thisRoom->objects[objActive].what == kDoorExRt)
	{
		roomNum = DoesNeighborRoomExist(kRoomToRight);
		if (roomNum != -1)
		{
			emptySlot = FindObjectSlotInRoom(roomNum);
			if ((emptySlot != -1) && (!DoesRoomNumHaveObject(roomNum, kDoorInLf)))
			{
				testRoomPtr = &(thisHouse.rooms[roomNum]);

				testRoomPtr->objects[emptySlot].what = kDoorInLf;
				testRoomPtr->objects[emptySlot].data.d.topLeft.h = kDoorInLfLeft;
				testRoomPtr->objects[emptySlot].data.d.topLeft.v = kDoorInTop;
				testRoomPtr->objects[emptySlot].data.d.tall = 0;
				testRoomPtr->objects[emptySlot].data.d.where = -1;
				testRoomPtr->objects[emptySlot].data.d.who = 255;
				testRoomPtr->objects[emptySlot].data.d.wide = 0;

				testRoomPtr->numObjects++;

				GetLocalizedString(45, message, ARRAYSIZE(message));
				OpenMessageWindow(message);
				SetMessageTextColor(blueColor);
				GetLocalizedString(47, message, ARRAYSIZE(message));
				SetMessageWindowMessage(message);
				DelayTicks(60);
				CloseMessageWindow();
			}
		}
	}
	else if (thisRoom->objects[objActive].what == kDoorExLf)
	{
		roomNum = DoesNeighborRoomExist(kRoomToLeft);
		if (roomNum != -1)
		{
			emptySlot = FindObjectSlotInRoom(roomNum);
			if ((emptySlot != -1) && (!DoesRoomNumHaveObject(roomNum, kDoorInRt)))
			{
				testRoomPtr = &(thisHouse.rooms[roomNum]);

				testRoomPtr->objects[emptySlot].what = kDoorInRt;
				testRoomPtr->objects[emptySlot].data.d.topLeft.h = kDoorInRtLeft;
				testRoomPtr->objects[emptySlot].data.d.topLeft.v = kDoorInTop;
				testRoomPtr->objects[emptySlot].data.d.tall = 0;
				testRoomPtr->objects[emptySlot].data.d.where = -1;
				testRoomPtr->objects[emptySlot].data.d.who = 255;
				testRoomPtr->objects[emptySlot].data.d.wide = 0;

				testRoomPtr->numObjects++;

				GetLocalizedString(45, message, ARRAYSIZE(message));
				OpenMessageWindow(message);
				SetMessageTextColor(blueColor);
				GetLocalizedString(47, message, ARRAYSIZE(message));
				SetMessageWindowMessage(message);
				DelayTicks(60);
				CloseMessageWindow();
			}
		}
	}
	else if (thisRoom->objects[objActive].what == kWindowInLf)
	{
		roomNum = DoesNeighborRoomExist(kRoomToLeft);
		if (roomNum != -1)
		{
			emptySlot = FindObjectSlotInRoom(roomNum);
			if ((emptySlot != -1) && (!DoesRoomNumHaveObject(roomNum, kWindowExRt)))
			{
				testRoomPtr = &(thisHouse.rooms[roomNum]);

				testRoomPtr->objects[emptySlot].what = kWindowExRt;
				testRoomPtr->objects[emptySlot].data.d.topLeft.h = kWindowExRtLeft;
				testRoomPtr->objects[emptySlot].data.d.topLeft.v = kWindowExTop;
				testRoomPtr->objects[emptySlot].data.d.tall = 0;
				testRoomPtr->objects[emptySlot].data.d.where = -1;
				testRoomPtr->objects[emptySlot].data.d.who = 255;
				testRoomPtr->objects[emptySlot].data.d.wide = 0;

				testRoomPtr->numObjects++;

				GetLocalizedString(45, message, ARRAYSIZE(message));
				OpenMessageWindow(message);
				SetMessageTextColor(blueColor);
				GetLocalizedString(48, message, ARRAYSIZE(message));
				SetMessageWindowMessage(message);
				DelayTicks(60);
				CloseMessageWindow();
			}
		}
	}
	else if (thisRoom->objects[objActive].what == kWindowInRt)
	{
		roomNum = DoesNeighborRoomExist(kRoomToRight);
		if (roomNum != -1)
		{
			emptySlot = FindObjectSlotInRoom(roomNum);
			if ((emptySlot != -1) && (!DoesRoomNumHaveObject(roomNum, kWindowExLf)))
			{
				testRoomPtr = &(thisHouse.rooms[roomNum]);

				testRoomPtr->objects[emptySlot].what = kWindowExLf;
				testRoomPtr->objects[emptySlot].data.d.topLeft.h = kWindowExLfLeft;
				testRoomPtr->objects[emptySlot].data.d.topLeft.v = kWindowExTop;
				testRoomPtr->objects[emptySlot].data.d.tall = 0;
				testRoomPtr->objects[emptySlot].data.d.where = -1;
				testRoomPtr->objects[emptySlot].data.d.who = 255;
				testRoomPtr->objects[emptySlot].data.d.wide = 0;

				testRoomPtr->numObjects++;

				GetLocalizedString(45, message, ARRAYSIZE(message));
				OpenMessageWindow(message);
				SetMessageTextColor(blueColor);
				GetLocalizedString(48, message, ARRAYSIZE(message));
				SetMessageWindowMessage(message);
				DelayTicks(60);
				CloseMessageWindow();
			}
		}
	}
	else if (thisRoom->objects[objActive].what == kWindowExRt)
	{
		roomNum = DoesNeighborRoomExist(kRoomToRight);
		if (roomNum != -1)
		{
			emptySlot = FindObjectSlotInRoom(roomNum);
			if ((emptySlot != -1) && (!DoesRoomNumHaveObject(roomNum, kWindowInLf)))
			{
				testRoomPtr = &(thisHouse.rooms[roomNum]);

				testRoomPtr->objects[emptySlot].what = kWindowInLf;
				testRoomPtr->objects[emptySlot].data.d.topLeft.h = kWindowInLfLeft;
				testRoomPtr->objects[emptySlot].data.d.topLeft.v = kWindowInTop;
				testRoomPtr->objects[emptySlot].data.d.tall = 0;
				testRoomPtr->objects[emptySlot].data.d.where = -1;
				testRoomPtr->objects[emptySlot].data.d.who = 255;
				testRoomPtr->objects[emptySlot].data.d.wide = 0;

				testRoomPtr->numObjects++;

				GetLocalizedString(45, message, ARRAYSIZE(message));
				OpenMessageWindow(message);
				SetMessageTextColor(blueColor);
				GetLocalizedString(49, message, ARRAYSIZE(message));
				SetMessageWindowMessage(message);
				DelayTicks(60);
				CloseMessageWindow();
			}
		}
	}
	else if (thisRoom->objects[objActive].what == kWindowExLf)
	{
		roomNum = DoesNeighborRoomExist(kRoomToLeft);
		if (roomNum != -1)
		{
			emptySlot = FindObjectSlotInRoom(roomNum);
			if ((emptySlot != -1) && (!DoesRoomNumHaveObject(roomNum, kWindowInRt)))
			{
				testRoomPtr = &(thisHouse.rooms[roomNum]);

				testRoomPtr->objects[emptySlot].what = kWindowInRt;
				testRoomPtr->objects[emptySlot].data.d.topLeft.h = kWindowInRtLeft;
				testRoomPtr->objects[emptySlot].data.d.topLeft.v = kWindowInTop;
				testRoomPtr->objects[emptySlot].data.d.tall = 0;
				testRoomPtr->objects[emptySlot].data.d.where = -1;
				testRoomPtr->objects[emptySlot].data.d.who = 255;
				testRoomPtr->objects[emptySlot].data.d.wide = 0;

				testRoomPtr->numObjects++;

				GetLocalizedString(45, message, ARRAYSIZE(message));
				OpenMessageWindow(message);
				SetMessageTextColor(blueColor);
				GetLocalizedString(49, message, ARRAYSIZE(message));
				SetMessageWindowMessage(message);
				DelayTicks(60);
				CloseMessageWindow();
			}
		}
	}
	else if (thisRoom->objects[objActive].what == kUpStairs)
	{
		roomNum = DoesNeighborRoomExist(kRoomAbove);
		if (roomNum != -1)
		{
			emptySlot = FindObjectSlotInRoom(roomNum);
			if ((emptySlot != -1) && (!DoesRoomNumHaveObject(roomNum, kDownStairs)))
			{
				testRoomPtr = &(thisHouse.rooms[roomNum]);

				testRoomPtr->objects[emptySlot].what = kDownStairs;
				testRoomPtr->objects[emptySlot].data.d.topLeft.h =
						thisRoom->objects[objActive].data.d.topLeft.h;
				testRoomPtr->objects[emptySlot].data.d.topLeft.v = kStairsTop;
				testRoomPtr->objects[emptySlot].data.d.tall = 0;
				testRoomPtr->objects[emptySlot].data.d.where = -1;
				testRoomPtr->objects[emptySlot].data.d.who = 255;
				testRoomPtr->objects[emptySlot].data.d.wide = 0;

				testRoomPtr->numObjects++;

				GetLocalizedString(45, message, ARRAYSIZE(message));
				OpenMessageWindow(message);
				SetMessageTextColor(blueColor);
				GetLocalizedString(50, message, ARRAYSIZE(message));
				SetMessageWindowMessage(message);
				DelayTicks(60);
				CloseMessageWindow();
			}
		}
	}
	else if (thisRoom->objects[objActive].what == kDownStairs)
	{
		roomNum = DoesNeighborRoomExist(kRoomBelow);
		if (roomNum != -1)
		{
			emptySlot = FindObjectSlotInRoom(roomNum);
			if ((emptySlot != -1) && (!DoesRoomNumHaveObject(roomNum, kUpStairs)))
			{
				testRoomPtr = &(thisHouse.rooms[roomNum]);

				testRoomPtr->objects[emptySlot].what = kUpStairs;
				testRoomPtr->objects[emptySlot].data.d.topLeft.h =
						thisRoom->objects[objActive].data.d.topLeft.h;
				testRoomPtr->objects[emptySlot].data.d.topLeft.v = kStairsTop;
				testRoomPtr->objects[emptySlot].data.d.tall = 0;
				testRoomPtr->objects[emptySlot].data.d.where = -1;
				testRoomPtr->objects[emptySlot].data.d.who = 255;
				testRoomPtr->objects[emptySlot].data.d.wide = 0;

				testRoomPtr->numObjects++;

				GetLocalizedString(45, message, ARRAYSIZE(message));
				OpenMessageWindow(message);
				SetMessageTextColor(blueColor);
				GetLocalizedString(51, message, ARRAYSIZE(message));
				SetMessageWindowMessage(message);
				DelayTicks(60);
				CloseMessageWindow();
			}
		}
	}
#endif
}

//--------------------------------------------------------------  DeleteObject

void Gp_DeleteObject (void)
{
#ifndef COMPILEDEMO
	SInt16		i;

	if ((theMode != kEditMode) || (objActive == kNoObjectSelected))
		return;

	if ((objActive == kInitialGliderSelected) ||
			(objActive == kLeftGliderSelected) ||
			(objActive == kRightGliderSelected))
	{
		MessageBeep(MB_ICONWARNING);
		return;
	}

	for (i = 0; i < kMaxRoomObs; i++)
	{
		if ((retroLinkList[i].room == thisRoomNumber) &&
				(retroLinkList[i].object == objActive))
			retroLinkList[i].room = -1;
	}

	thisRoom->objects[objActive].what = kObjectIsEmpty;
	thisRoom->numObjects--;
	fileDirty = true;
	UpdateMenus(false);
	Mac_InvalWindowRect(mainWindow, &mainWindowRect);
	QSetRect(&roomObjectRects[objActive], -1, -1, 0, 0);
	DeselectObject();
	ReadyBackground(thisRoom->background, thisRoom->tiles);
	DrawThisRoomsObjects();
#endif
}

//--------------------------------------------------------------  DuplicateObject

void DuplicateObject (HWND ownerWindow)
{
#ifndef COMPILEDEMO
	objectType	tempObject;
	Point		placePt;
	SInt16		direction, dist;
	Boolean		handled;

	tempObject = thisRoom->objects[objActive];

	placePt.h = roomObjectRects[objActive].left +
			HalfRectWide(&roomObjectRects[objActive]) + 64;
	placePt.v = roomObjectRects[objActive].top +
			HalfRectTall(&roomObjectRects[objActive]);

	StopMarquee();

	if (AddNewObject(ownerWindow, placePt, tempObject.what, false))
	{
		switch (tempObject.what)
		{
			case kFloorVent:		case kCeilingVent:		case kFloorBlower:
			case kCeilingBlower:	case kSewerGrate:		case kLeftFan:
			case kRightFan:			case kTaper:			case kCandle:
			case kStubby:			case kTiki:				case kBBQ:
			case kInvisBlower:		case kGrecoVent:		case kSewerBlower:
			thisRoom->objects[objActive].data.a.distance = tempObject.data.a.distance;
			thisRoom->objects[objActive].data.a.initial = tempObject.data.a.initial;
			thisRoom->objects[objActive].data.a.state = tempObject.data.a.state;
			thisRoom->objects[objActive].data.a.vector = tempObject.data.a.vector;
			thisRoom->objects[objActive].data.a.tall = tempObject.data.a.tall;
			break;

			case kLiftArea:
			thisRoom->objects[objActive].data.a.topLeft.h = tempObject.data.a.topLeft.h + 64;
			thisRoom->objects[objActive].data.a.topLeft.v = tempObject.data.a.topLeft.v;
			thisRoom->objects[objActive].data.a.distance = tempObject.data.a.distance;
			thisRoom->objects[objActive].data.a.initial = tempObject.data.a.initial;
			thisRoom->objects[objActive].data.a.state = tempObject.data.a.state;
			thisRoom->objects[objActive].data.a.vector = tempObject.data.a.vector;
			thisRoom->objects[objActive].data.a.tall = tempObject.data.a.tall;
			break;

			case kFilingCabinet:	case kWasteBasket:		case kMilkCrate:
			case kStool:			case kTrunk:			case kManhole:
			case kBooks:
			thisRoom->objects[objActive].data.b.pict = tempObject.data.b.pict;
			break;

			case kTable:			case kShelf:			case kCabinet:
			case kCounter:			case kDresser:			case kDeckTable:
			case kInvisObstacle:	case kInvisBounce:
			thisRoom->objects[objActive].data.b.bounds = tempObject.data.b.bounds;
			QOffsetRect(&thisRoom->objects[objActive].data.b.bounds, 64, 0);
			thisRoom->objects[objActive].data.b.pict = tempObject.data.b.pict;
			break;

			case kRedClock:			case kBlueClock:		case kYellowClock:
			case kCuckoo:			case kPaper:			case kBattery:
			case kBands:			case kGreaseRt:			case kGreaseLf:
			case kFoil:				case kInvisBonus:		case kStar:
			case kSparkle:			case kHelium:
			thisRoom->objects[objActive].data.c.length = tempObject.data.c.length;
			thisRoom->objects[objActive].data.c.points = tempObject.data.c.points;
			thisRoom->objects[objActive].data.c.state = tempObject.data.c.state;
			thisRoom->objects[objActive].data.c.initial = tempObject.data.c.initial;
			break;

			case kSlider:
			thisRoom->objects[objActive].data.c.topLeft.h = tempObject.data.c.topLeft.h + 64;
			thisRoom->objects[objActive].data.c.length = tempObject.data.c.length;
			thisRoom->objects[objActive].data.c.points = tempObject.data.c.points;
			thisRoom->objects[objActive].data.c.state = tempObject.data.c.state;
			thisRoom->objects[objActive].data.c.initial = tempObject.data.c.initial;
			break;

			case kUpStairs:			case kDownStairs:		case kMailboxLf:
			case kMailboxRt:		case kFloorTrans:		case kCeilingTrans:
			case kDoorInLf:			case kDoorInRt:			case kDoorExRt:
			case kDoorExLf:			case kWindowInLf:		case kWindowInRt:
			case kWindowExRt:		case kWindowExLf:
			thisRoom->objects[objActive].data.d.tall = tempObject.data.d.tall;
			thisRoom->objects[objActive].data.d.where = tempObject.data.d.where;
			thisRoom->objects[objActive].data.d.who = tempObject.data.d.who;
			thisRoom->objects[objActive].data.d.wide = tempObject.data.d.wide;
			break;

			case kInvisTrans:		case kDeluxeTrans:
			thisRoom->objects[objActive].data.d.topLeft.h = tempObject.data.d.topLeft.h + 64;
			thisRoom->objects[objActive].data.d.topLeft.v = tempObject.data.d.topLeft.v;
			thisRoom->objects[objActive].data.d.tall = tempObject.data.d.tall;
			thisRoom->objects[objActive].data.d.where = tempObject.data.d.where;
			thisRoom->objects[objActive].data.d.who = tempObject.data.d.who;
			thisRoom->objects[objActive].data.d.wide = tempObject.data.d.wide;
			break;

			case kLightSwitch:		case kMachineSwitch:	case kThermostat:
			case kPowerSwitch:		case kKnifeSwitch:		case kInvisSwitch:
			case kTrigger:			case kLgTrigger:		case kSoundTrigger:
			thisRoom->objects[objActive].data.e.delay = tempObject.data.e.delay;
			thisRoom->objects[objActive].data.e.where = tempObject.data.e.where;
			thisRoom->objects[objActive].data.e.who = tempObject.data.e.who;
			thisRoom->objects[objActive].data.e.type = tempObject.data.e.type;
			break;

			case kCeilingLight:		case kLightBulb:		case kTableLamp:
			case kHipLamp:			case kDecoLamp:			case kFlourescent:
			case kTrackLight:		case kInvisLight:
			thisRoom->objects[objActive].data.f.length = tempObject.data.f.length;
			thisRoom->objects[objActive].data.f.byte0 = tempObject.data.f.byte0;
			thisRoom->objects[objActive].data.f.byte1 = tempObject.data.f.byte1;
			thisRoom->objects[objActive].data.f.initial = tempObject.data.f.initial;
			thisRoom->objects[objActive].data.f.state = tempObject.data.f.state;
			break;

			case kShredder:			case kToaster:			case kMacPlus:
			case kGuitar:			case kTV:				case kCoffee:
			case kOutlet:			case kVCR:				case kStereo:
			case kMicrowave:		case kCinderBlock:		case kFlowerBox:
			case kCDs:
			thisRoom->objects[objActive].data.g.height = tempObject.data.g.height;
			thisRoom->objects[objActive].data.g.byte0 = tempObject.data.g.byte0;
			thisRoom->objects[objActive].data.g.delay = tempObject.data.g.delay;
			thisRoom->objects[objActive].data.g.initial = tempObject.data.g.initial;
			thisRoom->objects[objActive].data.g.state = tempObject.data.g.state;
			break;

			case kCustomPict:
			thisRoom->objects[objActive].data.g.topLeft.h =
					tempObject.data.g.topLeft.h + 64;
			thisRoom->objects[objActive].data.g.topLeft.v = tempObject.data.g.topLeft.v;
			thisRoom->objects[objActive].data.g.height = tempObject.data.g.height;
			thisRoom->objects[objActive].data.g.byte0 = tempObject.data.g.byte0;
			thisRoom->objects[objActive].data.g.delay = tempObject.data.g.delay;
			thisRoom->objects[objActive].data.g.initial = tempObject.data.g.initial;
			thisRoom->objects[objActive].data.g.state = tempObject.data.g.state;
			break;

			case kBalloon:			case kCopterLf:			case kCopterRt:
			case kDartLf:			case kDartRt:			case kBall:
			case kDrip:				case kFish:				case kCobweb:
			thisRoom->objects[objActive].data.h.length = tempObject.data.h.length;
			thisRoom->objects[objActive].data.h.delay = tempObject.data.h.delay;
			thisRoom->objects[objActive].data.h.byte0 = tempObject.data.h.byte0;
			thisRoom->objects[objActive].data.h.initial = tempObject.data.h.initial;
			thisRoom->objects[objActive].data.h.state = tempObject.data.h.state;
			break;

			case kOzma:				case kMousehole:		case kFireplace:
			case kBear:				case kCalendar:			case kVase1:
			case kVase2:			case kBulletin:			case kCloud:
			case kFaucet:			case kRug:				case kChimes:
			thisRoom->objects[objActive].data.i.pict = tempObject.data.i.pict;
			break;

			case kMirror:			case kFlower:			case kWallWindow:
			thisRoom->objects[objActive].data.i.bounds = tempObject.data.i.bounds;
			QOffsetRect(&thisRoom->objects[objActive].data.i.bounds, 64, 0);
			thisRoom->objects[objActive].data.i.pict = tempObject.data.i.pict;
			break;
		}

		if (KeepObjectLegal())
		{
		}
		handled = ObjectHasHandle(&direction, &dist);

		ReadyBackground(thisRoom->background, thisRoom->tiles);
		GetThisRoomsObjRects();
		DrawThisRoomsObjects();
		Mac_InvalWindowRect(mainWindow, &mainWindowRect);

		if (handled)
		{
			StartMarqueeHandled(&roomObjectRects[objActive], direction, dist);
			HandleBlowerGlider();
		}
		else
			StartMarquee(&roomObjectRects[objActive]);
	}
#endif
}

//--------------------------------------------------------------  MoveObject

void MoveObject (SInt16 whichWay, Boolean shiftDown)
{
#ifndef COMPILEDEMO
	Rect		wasRect;
	SInt16		deltaH, deltaV, increment;
	SInt16		dist, direction;

	if (theMode != kEditMode)
		return;

	StopMarquee();

	if (shiftDown)
		increment = 10;
	else
	{
		if (objActive == kInitialGliderSelected)
		{
			increment = 1;
		}
		else
		{
			if ((whichWay == kBumpRight) || (whichWay == kBumpLeft))
			{
				switch (thisRoom->objects[objActive].what)
				{
					case kTaper:
					case kCandle:
					case kStubby:
					case kTiki:
					case kBBQ:
					case kRedClock:
					case kBlueClock:
					case kYellowClock:
					case kCuckoo:
					case kPaper:
					case kBattery:
					case kBands:
					case kGreaseRt:
					case kGreaseLf:
					case kFoil:
					case kInvisBonus:
					case kStar:
					case kSparkle:
					case kHelium:
					case kSlider:
					case kLightSwitch:
					case kMachineSwitch:
					case kThermostat:
					case kPowerSwitch:
					case kKnifeSwitch:
					case kInvisSwitch:
					case kTrigger:
					case kLgTrigger:
					case kSoundTrigger:
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
					case kBall:
					case kDrip:
					case kFish:
					case kMirror:
					increment = 2;
					break;

					case kManhole:
					increment = 64;
					break;

					default:
					increment = 1;
					break;
				}
			}
			else
				increment = 1;
		}
	}

	switch (whichWay)
	{
		case kBumpUp:
		deltaH = 0;
		deltaV = -increment;
		break;

		case kBumpDown:
		deltaH = 0;
		deltaV = increment;
		break;

		case kBumpRight:
		deltaH = increment;
		deltaV = 0;
		break;

		case kBumpLeft:
		deltaH = -increment;
		deltaV = 0;
		break;

		default:
		deltaH = 0;
		deltaV = 0;
		break;
	}

	if (objActive == kInitialGliderSelected)
	{
		wasRect = initialGliderRect;
		thisHouse.initial.h += deltaH;
		thisHouse.initial.v += deltaV;
	}
	else if (objActive == kLeftGliderSelected)
	{
		wasRect = leftStartGliderDest;
		increment = thisRoom->leftStart + deltaV;
		if (increment > 255)
			increment = 255;
		else if (increment < 0)
			increment = 0;
		thisRoom->leftStart = (Byte)increment;
		QSetRect(&leftStartGliderDest, 0, 0, 48, 16);
		QOffsetRect(&leftStartGliderDest, 0,
				kGliderStartsDown + (SInt16)thisRoom->leftStart);
	}
	else if (objActive == kRightGliderSelected)
	{
		wasRect = rightStartGliderDest;
		increment = thisRoom->rightStart + deltaV;
		if (increment > 255)
			increment = 255;
		else if (increment < 0)
			increment = 0;
		thisRoom->rightStart = (Byte)increment;
		QSetRect(&rightStartGliderDest, 0, 0, 48, 16);
		QOffsetRect(&rightStartGliderDest, kRoomWide - 48,
				kGliderStartsDown + (SInt16)thisRoom->rightStart);
	}
	else
	{
		wasRect = roomObjectRects[objActive];
		switch (thisRoom->objects[objActive].what)
		{
			case kFloorVent:
			case kCeilingVent:
			case kFloorBlower:
			case kCeilingBlower:
			case kSewerGrate:
			case kGrecoVent:
			case kSewerBlower:
			thisRoom->objects[objActive].data.a.topLeft.h += deltaH;
			break;

			case kLeftFan:
			case kRightFan:
			case kTaper:
			case kCandle:
			case kStubby:
			case kTiki:
			case kBBQ:
			case kInvisBlower:
			case kLiftArea:
			thisRoom->objects[objActive].data.a.topLeft.h += deltaH;
			thisRoom->objects[objActive].data.a.topLeft.v += deltaV;
			break;

			case kTable:
			case kShelf:
			case kCabinet:
			case kFilingCabinet:
			case kWasteBasket:
			case kMilkCrate:
			case kDeckTable:
			case kStool:
			case kTrunk:
			case kInvisObstacle:
			case kBooks:
			case kInvisBounce:
			thisRoom->objects[objActive].data.b.bounds.left += deltaH;
			thisRoom->objects[objActive].data.b.bounds.right += deltaH;
			thisRoom->objects[objActive].data.b.bounds.top += deltaV;
			thisRoom->objects[objActive].data.b.bounds.bottom += deltaV;
			break;

			case kCounter:
			case kDresser:
			case kManhole:
			thisRoom->objects[objActive].data.b.bounds.left += deltaH;
			thisRoom->objects[objActive].data.b.bounds.right += deltaH;
			break;

			case kRedClock:
			case kBlueClock:
			case kYellowClock:
			case kCuckoo:
			case kPaper:
			case kBattery:
			case kBands:
			case kGreaseRt:
			case kGreaseLf:
			case kFoil:
			case kInvisBonus:
			case kStar:
			case kSparkle:
			case kHelium:
			case kSlider:
			thisRoom->objects[objActive].data.c.topLeft.h += deltaH;
			thisRoom->objects[objActive].data.c.topLeft.v += deltaV;
			break;

			case kUpStairs:
			case kDownStairs:
			case kFloorTrans:
			case kCeilingTrans:
			thisRoom->objects[objActive].data.d.topLeft.h += deltaH;
			break;

			case kMailboxLf:
			case kMailboxRt:
			case kInvisTrans:
			case kDeluxeTrans:
			thisRoom->objects[objActive].data.d.topLeft.h += deltaH;
			thisRoom->objects[objActive].data.d.topLeft.v += deltaV;
			break;

			case kLightSwitch:
			case kMachineSwitch:
			case kThermostat:
			case kPowerSwitch:
			case kKnifeSwitch:
			case kInvisSwitch:
			case kTrigger:
			case kLgTrigger:
			case kSoundTrigger:
			thisRoom->objects[objActive].data.e.topLeft.h += deltaH;
			thisRoom->objects[objActive].data.e.topLeft.v += deltaV;
			break;

			case kCeilingLight:
			case kHipLamp:
			case kDecoLamp:
			case kFlourescent:
			case kTrackLight:
			thisRoom->objects[objActive].data.f.topLeft.h += deltaH;
			break;

			case kLightBulb:
			case kTableLamp:
			case kInvisLight:
			thisRoom->objects[objActive].data.f.topLeft.h += deltaH;
			thisRoom->objects[objActive].data.f.topLeft.v += deltaV;
			break;

			case kShredder:
			case kToaster:
			case kMacPlus:
			case kGuitar:
			case kTV:
			case kCoffee:
			case kOutlet:
			case kVCR:
			case kStereo:
			case kMicrowave:
			case kCinderBlock:
			case kFlowerBox:
			case kCDs:
			case kCustomPict:
			thisRoom->objects[objActive].data.g.topLeft.h += deltaH;
			thisRoom->objects[objActive].data.g.topLeft.v += deltaV;
			break;

			case kBalloon:
			case kCopterLf:
			case kCopterRt:
			thisRoom->objects[objActive].data.h.topLeft.h += deltaH;
			break;

			case kDartLf:
			case kDartRt:
			thisRoom->objects[objActive].data.h.topLeft.v += deltaV;
			break;

			case kBall:
			case kDrip:
			case kFish:
			case kCobweb:
			thisRoom->objects[objActive].data.h.topLeft.h += deltaH;
			thisRoom->objects[objActive].data.h.topLeft.v += deltaV;
			break;

			case kOzma:
			case kMirror:
			case kFlower:
			case kWallWindow:
			case kBear:
			case kCalendar:
			case kVase1:
			case kVase2:
			case kBulletin:
			case kCloud:
			case kFaucet:
			case kRug:
			case kChimes:
			thisRoom->objects[objActive].data.i.bounds.left += deltaH;
			thisRoom->objects[objActive].data.i.bounds.right += deltaH;
			thisRoom->objects[objActive].data.i.bounds.top += deltaV;
			thisRoom->objects[objActive].data.i.bounds.bottom += deltaV;
			break;

			case kMousehole:
			case kFireplace:
			thisRoom->objects[objActive].data.i.bounds.left += deltaH;
			thisRoom->objects[objActive].data.i.bounds.right += deltaH;
			break;

		}
	}

	if (KeepObjectLegal())
	{
	}
	fileDirty = true;
	UpdateMenus(false);
	GetThisRoomsObjRects();

	if (objActive == kInitialGliderSelected)
	{
		Mac_InvalWindowRect(mainWindow, &wasRect);
		Mac_InvalWindowRect(mainWindow, &initialGliderRect);
	}
	else if (objActive == kLeftGliderSelected)
	{
		Mac_InvalWindowRect(mainWindow, &wasRect);
		Mac_InvalWindowRect(mainWindow, &leftStartGliderDest);
	}
	else if (objActive == kRightGliderSelected)
	{
		Mac_InvalWindowRect(mainWindow, &wasRect);
		Mac_InvalWindowRect(mainWindow, &rightStartGliderDest);
	}
	else
	{
		switch (thisRoom->objects[objActive].what)
		{
			case kTiki:
			case kTable:
			case kShelf:
			case kCabinet:
			case kDeckTable:
			case kStool:
			case kCounter:
			case kDresser:
			case kGreaseRt:
			case kGreaseLf:
			case kSlider:
			case kMailboxLf:
			case kMailboxRt:
			case kTrackLight:
			case kMirror:
			case kWallWindow:
			Mac_InvalWindowRect(mainWindow, &mainWindowRect);
			break;

			default:
			Mac_InvalWindowRect(mainWindow, &wasRect);
			Mac_InvalWindowRect(mainWindow, &roomObjectRects[objActive]);
			break;
		}
	}

	ReadyBackground(thisRoom->background, thisRoom->tiles);
	DrawThisRoomsObjects();

	if (ObjectHasHandle(&direction, &dist))
	{
		StartMarqueeHandled(&roomObjectRects[objActive], direction, dist);
		HandleBlowerGlider();
	}
	else
	{
		if (objActive == kInitialGliderSelected)
			StartMarquee(&initialGliderRect);
		else if (objActive == kLeftGliderSelected)
			StartMarquee(&leftStartGliderDest);
		else if (objActive == kRightGliderSelected)
			StartMarquee(&rightStartGliderDest);
		else
			StartMarquee(&roomObjectRects[objActive]);
	}
#endif
}

//--------------------------------------------------------------  DeselectObject

void DeselectObject (void)
{
#ifndef COMPILEDEMO
	if ((theMode != kEditMode) || (objActive == kNoObjectSelected))
		return;

	objActive = kNoObjectSelected;
	StopMarquee();
	UpdateMenus(false);
#endif
}

//--------------------------------------------------------------  ObjectHasHandle

#ifndef COMPILEDEMO
Boolean ObjectHasHandle (SInt16 *direction, SInt16 *dist)
{
	if ((objActive == kInitialGliderSelected) ||
			(objActive == kNoObjectSelected))
		return (false);

	switch (thisRoom->objects[objActive].what)
	{
		case kFloorVent:
		case kFloorBlower:
		case kSewerGrate:
		case kTaper:
		case kCandle:
		case kStubby:
		case kTiki:
		case kBBQ:
		case kGrecoVent:
		case kSewerBlower:
		*direction = kAbove;
		*dist = thisRoom->objects[objActive].data.a.distance;
		return (true);
		break;

		case kCeilingVent:
		case kCeilingBlower:
		*direction = kBelow;
		*dist = thisRoom->objects[objActive].data.a.distance;
		return (true);
		break;

		case kLeftFan:
		*direction = kToLeft;
		*dist = thisRoom->objects[objActive].data.a.distance;
		return (true);
		break;

		case kRightFan:
		*direction = kToRight;
		*dist = thisRoom->objects[objActive].data.a.distance;
		return (true);
		break;

		case kInvisBlower:
		switch (thisRoom->objects[objActive].data.a.vector & 0x0F)
		{
			case 1: 	// up
			*direction = kAbove;
			break;

			case 2: 	// right
			*direction = kToRight;
			break;

			case 4: 	// down
			*direction = kBelow;
			break;

			case 8: 	// left
			*direction = kToLeft;
			break;
		}
		*dist = thisRoom->objects[objActive].data.a.distance;
		return (true);
		break;

		case kTable:
		case kShelf:
		case kDeckTable:
		*direction = kToRight;
		*dist = 0;
		return (true);
		break;

		case kLiftArea:
		case kCabinet:
		case kInvisObstacle:
		case kInvisBounce:
		case kMirror:
		case kWallWindow:
		*direction = kBottomCorner;
		*dist = 0;
		return (true);
		break;

		case kCounter:
		case kDresser:
		*direction = kTopCorner;
		*dist = 0;
		return (true);
		break;

		case kGreaseRt:
		*direction = kToRight;
		*dist = thisRoom->objects[objActive].data.c.length;
		return (true);
		break;

		case kGreaseLf:
		*direction = kToLeft;
		*dist = thisRoom->objects[objActive].data.c.length;
		return (true);
		break;

		case kSlider:
		*direction = kToRight;
		*dist = 0;
		return (true);
		break;

		case kInvisTrans:
		case kDeluxeTrans:
		*direction = kBottomCorner;
		*dist = 0;
		return (true);
		break;

		case kFlourescent:
		case kTrackLight:
		*direction = kToRight;
		*dist = 0;
		return (true);
		break;

		case kToaster:
		*direction = kAbove;
		*dist = thisRoom->objects[objActive].data.g.height;
		return (true);
		break;

		case kBall:
		case kFish:
		*direction = kAbove;
		*dist = thisRoom->objects[objActive].data.h.length;
		return (true);
		break;

		case kDrip:
		*direction = kBelow;
		*dist = thisRoom->objects[objActive].data.h.length;
		return (true);
		break;

		default:
		return (false);
		break;
	}
}
#endif

//--------------------------------------------------------------  ObjectIsUpBlower

Boolean ObjectIsUpBlower (const objectType *who)
{
	if ((who->what == kFloorVent) || (who->what == kFloorBlower) ||
			(who->what == kSewerGrate) || (who->what == kTaper) ||
			(who->what == kCandle) || (who->what == kStubby) ||
			(who->what == kTiki) || (who->what == kBBQ) ||
			(who->what == kGrecoVent) || (who->what == kSewerBlower))
		return (true);
	else if ((who->what == kInvisBlower) || (who->what == kLiftArea))
	{
		if ((who->data.a.vector & 0x01) == 0x01)
			return (true);
		else
			return (false);
	}
	else
		return (false);
}

//--------------------------------------------------------------  HandleBlowerGlider

void HandleBlowerGlider (void)
{
#ifndef COMPILEDEMO
	SInt16		direction, dist;

	if (ObjectIsUpBlower(&thisRoom->objects[objActive]))
	{
		if (ObjectHasHandle(&direction, &dist))
		{
			SetMarqueeGliderRect(((roomObjectRects[objActive].right +
					roomObjectRects[objActive].left) / 2),
					roomObjectRects[objActive].top - dist);
		}
	}
#endif
}

//--------------------------------------------------------------  SelectNextObject

void SelectNextObject (void)
{
#ifndef COMPILEDEMO
	SInt16		direction, dist;
	Boolean		noneFound;

	if ((theMode != kEditMode) || (thisRoom->numObjects <= 0))
		return;

	noneFound = true;

	while (noneFound)
	{
		objActive++;
		if (objActive >= kMaxRoomObs)
			objActive = 0;
		if (thisRoom->objects[objActive].what != kObjectIsEmpty)
			noneFound = false;
	}

	UpdateMenus(false);
	if (ObjectHasHandle(&direction, &dist))
	{
		StartMarqueeHandled(&roomObjectRects[objActive], direction, dist);
		HandleBlowerGlider();
	}
	else
		StartMarquee(&roomObjectRects[objActive]);
#endif
}

//--------------------------------------------------------------  SelectPrevObject

void SelectPrevObject (void)
{
#ifndef COMPILEDEMO
	SInt16		direction, dist;
	Boolean		noneFound;

	if ((theMode != kEditMode) || (thisRoom->numObjects <= 0))
		return;

	noneFound = true;

	while (noneFound)
	{
		objActive--;
		if (objActive < 0)
			objActive = kMaxRoomObs - 1;
		if (thisRoom->objects[objActive].what != kObjectIsEmpty)
			noneFound = false;
	}

	UpdateMenus(false);
	if (ObjectHasHandle(&direction, &dist))
	{
		StartMarqueeHandled(&roomObjectRects[objActive], direction, dist);
		HandleBlowerGlider();
	}
	else
		StartMarquee(&roomObjectRects[objActive]);
#endif
}

//--------------------------------------------------------------  GetThisRoomsObjRects

#ifndef COMPILEDEMO
void GetThisRoomsObjRects (void)
{
	HBITMAP		thePict;
	SInt16		i, wide, tall;
	BITMAP		bmInfo;

	isFirstRoom = (GetFirstRoomNumber() == thisRoomNumber);

	if ((isFirstRoom) && (!noRoomAtAll) && (houseUnlocked))
		WhereDoesGliderBegin(&initialGliderRect, kNewGameMode);
	else
		QSetRect(&initialGliderRect, 0, 0, 0, 0);

	QSetRect(&leftStartGliderDest, 0, 0, 48, 16);
	QOffsetRect(&leftStartGliderDest, 0,
			kGliderStartsDown + (SInt16)thisRoom->leftStart);

	QSetRect(&rightStartGliderDest, 0, 0, 48, 16);
	QOffsetRect(&rightStartGliderDest, kRoomWide - 48,
			kGliderStartsDown + (SInt16)thisRoom->rightStart);

	if ((noRoomAtAll) || (!houseUnlocked))
	{
		return;
	}
	else
	{
		for (i = 0; i < kMaxRoomObs; i++)
		{
			switch (thisRoom->objects[i].what)
			{
				case kObjectIsEmpty:
				QSetRect(&roomObjectRects[i], -2, -2, -1, -1);
				break;

				case kFloorVent:
				case kCeilingVent:
				case kFloorBlower:
				case kCeilingBlower:
				case kSewerGrate:
				case kLeftFan:
				case kRightFan:
				case kTaper:
				case kCandle:
				case kStubby:
				case kTiki:
				case kBBQ:
				case kInvisBlower:
				case kGrecoVent:
				case kSewerBlower:
				roomObjectRects[i] = srcRects[thisRoom->objects[i].what];
				ZeroRectCorner(&roomObjectRects[i]);
				QOffsetRect(&roomObjectRects[i],
						thisRoom->objects[i].data.a.topLeft.h,
						thisRoom->objects[i].data.a.topLeft.v);
				break;

				case kLiftArea:
				QSetRect(&roomObjectRects[i], 0, 0,
						thisRoom->objects[i].data.a.distance,
						thisRoom->objects[i].data.a.tall * 2);
				QOffsetRect(&roomObjectRects[i],
						thisRoom->objects[i].data.a.topLeft.h,
						thisRoom->objects[i].data.a.topLeft.v);
				break;

				case kTable:
				case kShelf:
				case kCabinet:
				case kFilingCabinet:
				case kWasteBasket:
				case kMilkCrate:
				case kCounter:
				case kDresser:
				case kDeckTable:
				case kStool:
				case kTrunk:
				case kInvisObstacle:
				case kManhole:
				case kBooks:
				case kInvisBounce:
				roomObjectRects[i] = thisRoom->objects[i].data.b.bounds;
				break;

				case kRedClock:
				case kBlueClock:
				case kYellowClock:
				case kCuckoo:
				case kPaper:
				case kBattery:
				case kBands:
				case kFoil:
				case kInvisBonus:
				case kStar:
				case kSparkle:
				case kHelium:
				roomObjectRects[i] = srcRects[thisRoom->objects[i].what];
				ZeroRectCorner(&roomObjectRects[i]);
				QOffsetRect(&roomObjectRects[i],
						thisRoom->objects[i].data.c.topLeft.h,
						thisRoom->objects[i].data.c.topLeft.v);
				break;

				case kGreaseRt:
				roomObjectRects[i] = srcRects[thisRoom->objects[i].what];
				ZeroRectCorner(&roomObjectRects[i]);
				QOffsetRect(&roomObjectRects[i],
						thisRoom->objects[i].data.c.topLeft.h,
						thisRoom->objects[i].data.c.topLeft.v);
				if (!thisRoom->objects[i].data.c.initial)
					QOffsetRect(&roomObjectRects[i], 8, 0);
				break;

				case kGreaseLf:
				roomObjectRects[i] = srcRects[thisRoom->objects[i].what];
				ZeroRectCorner(&roomObjectRects[i]);
				QOffsetRect(&roomObjectRects[i],
						thisRoom->objects[i].data.c.topLeft.h,
						thisRoom->objects[i].data.c.topLeft.v);
				if (!thisRoom->objects[i].data.c.initial)
					QOffsetRect(&roomObjectRects[i], -8, 0);
				break;

				case kSlider:
				roomObjectRects[i] = srcRects[thisRoom->objects[i].what];
				ZeroRectCorner(&roomObjectRects[i]);
				QOffsetRect(&roomObjectRects[i],
						thisRoom->objects[i].data.c.topLeft.h,
						thisRoom->objects[i].data.c.topLeft.v);
				roomObjectRects[i].right = roomObjectRects[i].left +
						thisRoom->objects[i].data.c.length;
				break;

				case kUpStairs:
				case kDownStairs:
				case kMailboxLf:
				case kMailboxRt:
				case kFloorTrans:
				case kCeilingTrans:
				case kDoorInLf:
				case kDoorInRt:
				case kDoorExRt:
				case kDoorExLf:
				case kWindowInLf:
				case kWindowInRt:
				case kWindowExRt:
				case kWindowExLf:
				roomObjectRects[i] = srcRects[thisRoom->objects[i].what];
				ZeroRectCorner(&roomObjectRects[i]);
				QOffsetRect(&roomObjectRects[i],
						thisRoom->objects[i].data.d.topLeft.h,
						thisRoom->objects[i].data.d.topLeft.v);
				break;

				case kInvisTrans:
				roomObjectRects[i] = srcRects[thisRoom->objects[i].what];
				ZeroRectCorner(&roomObjectRects[i]);
				QOffsetRect(&roomObjectRects[i],
						thisRoom->objects[i].data.d.topLeft.h,
						thisRoom->objects[i].data.d.topLeft.v);
				roomObjectRects[i].bottom = roomObjectRects[i].top +
						thisRoom->objects[i].data.d.tall;
				roomObjectRects[i].right += (short)thisRoom->objects[i].data.d.wide;
				break;

				case kDeluxeTrans:		// Uses a kludge to get width & height (x4)
				wide = (thisRoom->objects[i].data.d.tall & 0xFF00) >> 8;
				tall = thisRoom->objects[i].data.d.tall & 0x00FF;
				QSetRect(&roomObjectRects[i], 0, 0, wide * 4, tall * 4);
				QOffsetRect(&roomObjectRects[i],
						thisRoom->objects[i].data.d.topLeft.h,
						thisRoom->objects[i].data.d.topLeft.v);
				break;

				case kLightSwitch:
				case kMachineSwitch:
				case kThermostat:
				case kPowerSwitch:
				case kKnifeSwitch:
				case kInvisSwitch:
				case kTrigger:
				case kLgTrigger:
				case kSoundTrigger:
				roomObjectRects[i] = srcRects[thisRoom->objects[i].what];
				ZeroRectCorner(&roomObjectRects[i]);
				QOffsetRect(&roomObjectRects[i],
						thisRoom->objects[i].data.e.topLeft.h,
						thisRoom->objects[i].data.e.topLeft.v);
				break;

				case kCeilingLight:
				case kLightBulb:
				case kTableLamp:
				case kHipLamp:
				case kDecoLamp:
				case kInvisLight:
				roomObjectRects[i] = srcRects[thisRoom->objects[i].what];
				ZeroRectCorner(&roomObjectRects[i]);
				QOffsetRect(&roomObjectRects[i],
						thisRoom->objects[i].data.f.topLeft.h,
						thisRoom->objects[i].data.f.topLeft.v);
				break;

				case kFlourescent:
				case kTrackLight:
				roomObjectRects[i] = srcRects[thisRoom->objects[i].what];
				ZeroRectCorner(&roomObjectRects[i]);
				roomObjectRects[i].right = thisRoom->objects[i].data.f.length;
				QOffsetRect(&roomObjectRects[i],
						thisRoom->objects[i].data.f.topLeft.h,
						thisRoom->objects[i].data.f.topLeft.v);
				break;

				case kShredder:
				case kToaster:
				case kMacPlus:
				case kGuitar:
				case kTV:
				case kCoffee:
				case kOutlet:
				case kVCR:
				case kStereo:
				case kMicrowave:
				case kCinderBlock:
				case kFlowerBox:
				case kCDs:
				roomObjectRects[i] = srcRects[thisRoom->objects[i].what];
				ZeroRectCorner(&roomObjectRects[i]);
				QOffsetRect(&roomObjectRects[i],
						thisRoom->objects[i].data.g.topLeft.h,
						thisRoom->objects[i].data.g.topLeft.v);
				break;

				case kCustomPict:
				thePict = GetPicture(thisRoom->objects[i].data.g.height);
				if (thePict == NULL)
				{
					thisRoom->objects[i].data.g.height = kCustomPictFallbackID;
					roomObjectRects[i] = srcRects[thisRoom->objects[i].what];
				}
				else
				{
					GetObject(thePict, sizeof(bmInfo), &bmInfo);
					QSetRect(&roomObjectRects[i], 0, 0,
							(SInt16)bmInfo.bmWidth, (SInt16)bmInfo.bmHeight);
					DeleteObject(thePict);
				}
				ZeroRectCorner(&roomObjectRects[i]);
				QOffsetRect(&roomObjectRects[i],
						thisRoom->objects[i].data.g.topLeft.h,
						thisRoom->objects[i].data.g.topLeft.v);
				break;

				case kBalloon:
				case kCopterLf:
				case kCopterRt:
				case kDartLf:
				case kDartRt:
				case kBall:
				case kDrip:
				case kFish:
				case kCobweb:
				roomObjectRects[i] = srcRects[thisRoom->objects[i].what];
				ZeroRectCorner(&roomObjectRects[i]);
				QOffsetRect(&roomObjectRects[i],
						thisRoom->objects[i].data.h.topLeft.h,
						thisRoom->objects[i].data.h.topLeft.v);
				break;

				case kOzma:
				case kMirror:
				case kMousehole:
				case kFireplace:
				case kFlower:
				case kWallWindow:
				case kBear:
				case kCalendar:
				case kVase1:
				case kVase2:
				case kBulletin:
				case kCloud:
				case kFaucet:
				case kRug:
				case kChimes:
				roomObjectRects[i] = thisRoom->objects[i].data.i.bounds;
				break;

				default:
				QSetRect(&roomObjectRects[i], -2, -2, -1, -1);
				break;

			}
		}
	}
}
#endif

//--------------------------------------------------------------  DrawThisRoomsObjects

#ifndef COMPILEDEMO
void DrawThisRoomsObjects (void)
{
	Rect		tempRect;
	SInt16		i;

	if ((noRoomAtAll) || (!houseUnlocked))
		return;
	else
	{
		if (GetNumberOfLights(thisRoomNumber) <= 0)
		{
			ColorShadowRect(backSrcMap, &backSrcRect, k8BlackColor);
		}

		for (i = 0; i < kMaxRoomObs; i++)
		{
			switch (thisRoom->objects[i].what)
			{
				case kObjectIsEmpty:
				break;

				case kFloorVent:
				case kCeilingVent:
				case kFloorBlower:
				case kCeilingBlower:
				case kSewerGrate:
				case kLeftFan:
				case kRightFan:
				case kTaper:
				case kCandle:
				case kStubby:
				case kGrecoVent:
				case kSewerBlower:
				DrawSimpleBlowers(thisRoom->objects[i].what, &roomObjectRects[i]);
				break;

				case kTiki:
				DrawTiki(&roomObjectRects[i], 0);
				break;

				case kInvisBlower:
				DrawInvisibleBlower(&roomObjectRects[i]);
				break;

				case kLiftArea:
				DrawLiftArea(&roomObjectRects[i]);
				break;

				case kTable:
				DrawTable(&roomObjectRects[i], 0);
				break;

				case kShelf:
				DrawShelf(&roomObjectRects[i]);
				break;

				case kCabinet:
				DrawCabinet(&roomObjectRects[i]);
				break;

				case kFilingCabinet:
				case kDownStairs:
				case kDoorExRt:
				case kDoorExLf:
				case kWindowExRt:
				case kWindowExLf:
				case kOzma:
				DrawPictObject(thisRoom->objects[i].what, &roomObjectRects[i]);
				break;

				case kWasteBasket:
				case kMilkCrate:
				DrawSimpleFurniture(thisRoom->objects[i].what, &roomObjectRects[i]);
				break;

				case kCounter:
				DrawCounter(&roomObjectRects[i]);
				break;

				case kDresser:
				DrawDresser(&roomObjectRects[i]);
				break;

				case kDeckTable:
				DrawDeckTable(&roomObjectRects[i], 0);
				break;

				case kStool:
				DrawStool(&roomObjectRects[i], 0);
				break;

				case kInvisObstacle:
				DrawInvisObstacle(&roomObjectRects[i]);
				break;

				case kInvisBounce:
				DrawInvisBounce(&roomObjectRects[i]);
				break;

				case kRedClock:
				DrawRedClock(&roomObjectRects[i]);
				break;

				case kBlueClock:
				DrawBlueClock(&roomObjectRects[i]);
				break;

				case kYellowClock:
				DrawYellowClock(&roomObjectRects[i]);
				break;

				case kCuckoo:
				DrawCuckoo(&roomObjectRects[i]);
				break;

				case kPaper:
				case kBattery:
				case kBands:
				case kStar:
				case kSparkle:
				case kHelium:
				DrawSimplePrizes(thisRoom->objects[i].what, &roomObjectRects[i]);
				break;

				case kGreaseRt:
				tempRect = roomObjectRects[i];
				if (!thisRoom->objects[i].data.c.initial)
					QOffsetRect(&tempRect, -8, 0);
				DrawGreaseRt(&tempRect, thisRoom->objects[i].data.c.length,
						thisRoom->objects[i].data.c.initial);
				break;

				case kGreaseLf:
				tempRect = roomObjectRects[i];
				if (!thisRoom->objects[i].data.c.initial)
					QOffsetRect(&tempRect, 8, 0);
				DrawGreaseLf(&tempRect, thisRoom->objects[i].data.c.length,
						thisRoom->objects[i].data.c.initial);
				break;

				case kFoil:
				DrawFoil(&roomObjectRects[i]);
				break;

				case kInvisBonus:
				DrawInvisBonus(&roomObjectRects[i]);
				break;

				case kSlider:
				DrawSlider(&roomObjectRects[i]);
				break;

				case kBBQ:
				case kTrunk:
				case kManhole:
				case kBooks:
				case kUpStairs:
				case kDoorInLf:
				case kDoorInRt:
				case kWindowInLf:
				case kWindowInRt:
				case kHipLamp:
				case kDecoLamp:
				case kGuitar:
				case kCinderBlock:
				case kFlowerBox:
				case kFireplace:
				case kBear:
				case kVase1:
				case kVase2:
				case kRug:
				case kChimes:
				DrawPictSansWhiteObject(thisRoom->objects[i].what, &roomObjectRects[i]);
				break;

				case kCustomPict:
				DrawCustPictSansWhite(thisRoom->objects[i].data.g.height,
						&roomObjectRects[i]);
				break;

				case kMailboxLf:
				DrawMailboxLeft(&roomObjectRects[i], 0);
				break;

				case kMailboxRt:
				DrawMailboxRight(&roomObjectRects[i], 0);
				break;

				case kFloorTrans:
				case kCeilingTrans:
				DrawSimpleTransport(thisRoom->objects[i].what, &roomObjectRects[i]);
				break;

				case kInvisTrans:
				case kDeluxeTrans:
				DrawInvisTransport(&roomObjectRects[i]);
				break;

				case kLightSwitch:
				DrawLightSwitch(&roomObjectRects[i], true);
				break;

				case kMachineSwitch:
				DrawMachineSwitch(&roomObjectRects[i], true);
				break;

				case kThermostat:
				DrawThermostat(&roomObjectRects[i], true);
				break;

				case kPowerSwitch:
				DrawPowerSwitch(&roomObjectRects[i], true);
				break;

				case kKnifeSwitch:
				DrawKnifeSwitch(&roomObjectRects[i], true);
				break;

				case kInvisSwitch:
				DrawInvisibleSwitch(&roomObjectRects[i]);
				break;

				case kTrigger:
				case kLgTrigger:
				DrawTrigger(&roomObjectRects[i]);
				break;

				case kSoundTrigger:
				DrawSoundTrigger(&roomObjectRects[i]);
				break;

				case kCeilingLight:
				case kLightBulb:
				case kTableLamp:
				DrawSimpleLight(thisRoom->objects[i].what, &roomObjectRects[i]);
				break;

				case kFlourescent:
				DrawFlourescent(&roomObjectRects[i]);
				break;

				case kTrackLight:
				DrawTrackLight(&roomObjectRects[i]);
				break;

				case kInvisLight:
				DrawInvisLight(&roomObjectRects[i]);
				break;

				case kShredder:
				case kToaster:
				case kCDs:
				DrawSimpleAppliance(thisRoom->objects[i].what, &roomObjectRects[i]);
				break;

				case kMacPlus:
				DrawMacPlus(&roomObjectRects[i],
						thisRoom->objects[i].data.g.initial, true);
				break;

				case kTV:
				DrawTV(&roomObjectRects[i],
						thisRoom->objects[i].data.g.initial, true);
				break;

				case kCoffee:
				DrawCoffee(&roomObjectRects[i],
						thisRoom->objects[i].data.g.initial, true);
				break;

				case kOutlet:
				DrawOutlet(&roomObjectRects[i]);
				break;

				case kVCR:
				DrawVCR(&roomObjectRects[i],
						thisRoom->objects[i].data.g.initial, true);
				break;

				case kStereo:
				DrawStereo(&roomObjectRects[i], isPlayMusicGame, true);
				break;

				case kMicrowave:
				DrawMicrowave(&roomObjectRects[i],
						thisRoom->objects[i].data.g.initial, true);
				break;

				case kBalloon:
				DrawBalloon(&roomObjectRects[i]);
				break;

				case kCopterLf:
				case kCopterRt:
				DrawCopter(&roomObjectRects[i]);
				break;

				case kDartLf:
				case kDartRt:
				DrawDart(&roomObjectRects[i], thisRoom->objects[i].what);
				break;

				case kBall:
				DrawBall(thisRoom->objects[i].what, &roomObjectRects[i]);
				break;

				case kDrip:
				DrawDrip(&roomObjectRects[i]);
				break;

				case kFish:
				DrawFish(thisRoom->objects[i].what, &roomObjectRects[i]);
				break;

				case kCobweb:
				case kCloud:
				DrawPictWithMaskObject(thisRoom->objects[i].what, &roomObjectRects[i]);
				break;

				case kMirror:
				DrawMirror(&roomObjectRects[i]);
				break;

				case kMousehole:
				case kFaucet:
				DrawSimpleClutter(thisRoom->objects[i].what, &roomObjectRects[i]);
				break;

				case kFlower:
				DrawFlower(&roomObjectRects[i], thisRoom->objects[i].data.i.pict);
				break;

				case kWallWindow:
				DrawWallWindow(&roomObjectRects[i]);
				break;

				case kCalendar:
				DrawCalendar(&roomObjectRects[i]);
				break;

				case kBulletin:
				DrawBulletin(&roomObjectRects[i]);
				break;

				default:
				break;
			}
		}
	}

	if (isFirstRoom)
	{
		Mac_CopyMask(glidSrcMap, glidMaskMap, backSrcMap,
				&gliderSrc[0], &gliderSrc[0], &initialGliderRect);
	}

	Mac_CopyMask(blowerSrcMap, blowerMaskMap, backSrcMap,
			&leftStartGliderSrc, &leftStartGliderSrc, &leftStartGliderDest);

	Mac_CopyMask(blowerSrcMap, blowerMaskMap, backSrcMap,
			&rightStartGliderSrc, &rightStartGliderSrc, &rightStartGliderDest);

	Mac_CopyBits(backSrcMap, workSrcMap,
			&backSrcRect, &backSrcRect, srcCopy, nil);
}
#endif

//--------------------------------------------------------------  HiliteAllObjects

void HiliteAllObjects (void)
{
#ifndef COMPILEDEMO
	HDC			mainWindowDC;
	RECT		focusRects[ARRAYSIZE(roomObjectRects)];
	MSG			msg;
	SInt16		i;

	if (theMode != kEditMode)
		return;

	for (i = 0; i < kMaxRoomObs; i++)
	{
		focusRects[i].left = roomObjectRects[i].left;
		focusRects[i].top = roomObjectRects[i].top;
		focusRects[i].right = roomObjectRects[i].right;
		focusRects[i].bottom = roomObjectRects[i].bottom;
	}

	PauseMarquee();

	mainWindowDC = GetMainWindowDC();
	for (i = 0; i < kMaxRoomObs; i++)
		DrawFocusRect(mainWindowDC, &focusRects[i]);
	ReleaseMainWindowDC(mainWindowDC);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (mainWindow != NULL && GetActiveWindow() != mainWindow)
			break;
		// TODO: consider changing the Option key substitute to the Shift key here
		// (Also see function HandleEvent).
		// if Command (CTRL) or Option (ALT) key go up
		if ((GetKeyState(VK_CONTROL) >= 0) || (GetKeyState(VK_MENU) >= 0))
			break;
	}
	if (msg.message == WM_QUIT)
		PostQuitMessage((int)msg.wParam);

	mainWindowDC = GetMainWindowDC();
	for (i = 0; i < kMaxRoomObs; i++)
		DrawFocusRect(mainWindowDC, &focusRects[i]);
	ReleaseMainWindowDC(mainWindowDC);

	ResumeMarquee();
#endif
}

//--------------------------------------------------------------  GoToObjectInRoom

void GoToObjectInRoom (SInt16 object, SInt16 floor, SInt16 suite)
{
	#ifndef COMPILEDEMO
	SInt16		itsNumber, direction, dist;

	if (RoomExists(suite, floor, &itsNumber))
	{
		if (itsNumber != thisRoomNumber)
		{
			CopyRoomToThisRoom(itsNumber);
			DeselectObject();
			ReflectCurrentRoom(false);
		}
		else
			DeselectObject();

		if (thisRoom->objects[object].what != kObjectIsEmpty)
		{
			objActive = object;
			if (ObjectHasHandle(&direction, &dist))
			{
				StartMarqueeHandled(&roomObjectRects[objActive], direction, dist);
				HandleBlowerGlider();
			}
			else
				StartMarquee(&roomObjectRects[objActive]);
			UpdateMenus(false);
		}
	}
	#endif
}

//--------------------------------------------------------------  GoToObjectInRoomNum

void GoToObjectInRoomNum (SInt16 object, SInt16 roomNum)
{
	SInt16		floor, suite;

	if (GetRoomFloorSuite(roomNum, &floor, &suite))
		GoToObjectInRoom(object, floor, suite);
}

