//============================================================================
//----------------------------------------------------------------------------
//                                ObjectEdit.c
//----------------------------------------------------------------------------
//============================================================================

#include "ObjectEdit.h"

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
#include "ResourceLoader.h"
#include "Room.h"
#include "RoomInfo.h"
#include "StringUtils.h"
#include "Tools.h"
#include "WindowUtils.h"

SInt16 FindObjectSelected (Point where);
void DragHandle (Point where);
void Gp_DragObject (Point where);
void AddObjectPairing (void);
Boolean ObjectIsUpBlower (const objectType *who);

Rect g_roomObjectRects[kMaxRoomObs];
Rect g_leftStartGliderSrc;
Rect g_rightStartGliderSrc;
SInt16 g_objActive;

static Rect g_initialGliderRect;
static Rect g_leftStartGliderDest;
static Rect g_rightStartGliderDest;
static Boolean g_isFirstRoom;

//==============================================================  Functions
//--------------------------------------------------------------  FindObjectSelected

SInt16 FindObjectSelected (Point where)
{
	SInt16 found, i;

	found = kNoObjectSelected;

	if (QPtInRect(where, &g_initialGliderRect))
		return (kInitialGliderSelected);
	else if (QPtInRect(where, &g_leftStartGliderDest))
		return (kLeftGliderSelected);
	else if (QPtInRect(where, &g_rightStartGliderDest))
		return (kRightGliderSelected);

	for (i = kMaxRoomObs - 1; i >= 0; i--)
	{
		if (QPtInRect(where, &g_roomObjectRects[i]))
		{
			found = i;
			break;
		}
	}
	return (found);
}

//--------------------------------------------------------------  DoSelectionClick

void DoSelectionClick (HWND hwnd, Point where, Boolean isDoubleClick)
{
	POINT dragPt;

	if (COMPILEDEMO)
		return;

	StopMarquee();

	dragPt.x = where.h;
	dragPt.y = where.v;
	ClientToScreen(hwnd, &dragPt);
	if ((PtInMarqueeHandle(where)) && (g_objActive != kNoObjectSelected))
	{
		if (DragDetect(hwnd, dragPt))
			DragHandle(where);
		StartMarqueeForActiveObject();
	}
	else
	{
		g_objActive = FindObjectSelected(where);
		if (g_objActive == kNoObjectSelected)
		{
			if (isDoubleClick)
				DoRoomInfo(hwnd);
		}
		else
		{
			if (isDoubleClick)
			{
				DoObjectInfo(hwnd);
				StartMarqueeForActiveObject();
			}
			else
			{

				if (DragDetect(hwnd, dragPt))
					Gp_DragObject(where);
				StartMarqueeForActiveObject();
			}
		}
	}
	UpdateMenus(false);
}

//--------------------------------------------------------------  DragHandle

void DragHandle (Point where)
{
	SInt16 hDelta, vDelta;
	Boolean whoCares;

	if (g_objActive < 0 || g_objActive >= kMaxRoomObs)
	{
		return;
	}

	switch (g_thisRoom->objects[g_objActive].what)
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
		vDelta = g_thisRoom->objects[g_objActive].data.a.distance;
		DragMarqueeHandle(where, &vDelta);
		g_thisRoom->objects[g_objActive].data.a.distance = vDelta;
		whoCares = KeepObjectLegal();
		break;

		case kLiftArea:
		hDelta = g_thisRoom->objects[g_objActive].data.a.distance;
		vDelta = g_thisRoom->objects[g_objActive].data.a.tall * 2;
		DragMarqueeCorner(where, &hDelta, &vDelta, false);
		g_thisRoom->objects[g_objActive].data.a.distance = hDelta;
		g_thisRoom->objects[g_objActive].data.a.tall = (Byte)(vDelta / 2);
		whoCares = KeepObjectLegal();
		InvalidateRect(g_mainWindow, NULL, TRUE);
		GetThisRoomsObjRects();
		ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
		DrawThisRoomsObjects();
		break;

		case kLeftFan:
		case kRightFan:
		hDelta = g_thisRoom->objects[g_objActive].data.a.distance;
		DragMarqueeHandle(where, &hDelta);
		g_thisRoom->objects[g_objActive].data.a.distance = hDelta;
		whoCares = KeepObjectLegal();
		break;

		case kInvisBlower:
		if (((g_thisRoom->objects[g_objActive].data.a.vector & 0x0F) == 1) ||
				((g_thisRoom->objects[g_objActive].data.a.vector & 0x0F) == 4))
		{
			vDelta = g_thisRoom->objects[g_objActive].data.a.distance;
			DragMarqueeHandle(where, &vDelta);
			g_thisRoom->objects[g_objActive].data.a.distance = vDelta;
		}
		else
		{
			hDelta = g_thisRoom->objects[g_objActive].data.a.distance;
			DragMarqueeHandle(where, &hDelta);
			g_thisRoom->objects[g_objActive].data.a.distance = hDelta;
		}
		whoCares = KeepObjectLegal();
		break;

		case kTable:
		case kShelf:
		case kDeckTable:
		hDelta = RectWide(&g_thisRoom->objects[g_objActive].data.b.bounds);
		DragMarqueeHandle(where, &hDelta);
		g_thisRoom->objects[g_objActive].data.b.bounds.right =
			g_thisRoom->objects[g_objActive].data.b.bounds.left + hDelta;
		whoCares = KeepObjectLegal();
		InvalidateRect(g_mainWindow, NULL, TRUE);
		GetThisRoomsObjRects();
		ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
		DrawThisRoomsObjects();
		break;

		case kCabinet:
		case kInvisObstacle:
		case kInvisBounce:
		hDelta = RectWide(&g_thisRoom->objects[g_objActive].data.b.bounds);
		vDelta = RectTall(&g_thisRoom->objects[g_objActive].data.b.bounds);
		DragMarqueeCorner(where, &hDelta, &vDelta, false);
		g_thisRoom->objects[g_objActive].data.b.bounds.right =
			g_thisRoom->objects[g_objActive].data.b.bounds.left + hDelta;
		g_thisRoom->objects[g_objActive].data.b.bounds.bottom =
			g_thisRoom->objects[g_objActive].data.b.bounds.top + vDelta;
		whoCares = KeepObjectLegal();
		InvalidateRect(g_mainWindow, NULL, TRUE);
		GetThisRoomsObjRects();
		ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
		DrawThisRoomsObjects();
		break;

		case kCounter:
		case kDresser:
		hDelta = RectWide(&g_thisRoom->objects[g_objActive].data.b.bounds);
		vDelta = RectTall(&g_thisRoom->objects[g_objActive].data.b.bounds);
		DragMarqueeCorner(where, &hDelta, &vDelta, true);
		g_thisRoom->objects[g_objActive].data.b.bounds.right =
			g_thisRoom->objects[g_objActive].data.b.bounds.left + hDelta;
		g_thisRoom->objects[g_objActive].data.b.bounds.top =
			g_thisRoom->objects[g_objActive].data.b.bounds.bottom - vDelta;
		whoCares = KeepObjectLegal();
		InvalidateRect(g_mainWindow, NULL, TRUE);
		GetThisRoomsObjRects();
		ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
		DrawThisRoomsObjects();
		break;

		case kGreaseRt:
		case kGreaseLf:
		case kSlider:
		hDelta = g_thisRoom->objects[g_objActive].data.c.length;
		DragMarqueeHandle(where, &hDelta);
		g_thisRoom->objects[g_objActive].data.c.length = hDelta;
		whoCares = KeepObjectLegal();
		InvalidateRect(g_mainWindow, NULL, TRUE);
		GetThisRoomsObjRects();
		ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
		DrawThisRoomsObjects();
		break;

		case kInvisTrans:
		hDelta = g_thisRoom->objects[g_objActive].data.d.wide;
		vDelta = g_thisRoom->objects[g_objActive].data.d.tall;
		DragMarqueeCorner(where, &hDelta, &vDelta, false);
		if (hDelta > 127)
			hDelta = 127;
		g_thisRoom->objects[g_objActive].data.d.wide = (Byte)hDelta;
		g_thisRoom->objects[g_objActive].data.d.tall = vDelta;
		whoCares = KeepObjectLegal();
		InvalidateRect(g_mainWindow, NULL, TRUE);
		GetThisRoomsObjRects();
		ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
		DrawThisRoomsObjects();
		break;

		case kDeluxeTrans:
		hDelta = ((g_thisRoom->objects[g_objActive].data.d.tall & 0xFF00) >> 8) * 4;
		vDelta = (g_thisRoom->objects[g_objActive].data.d.tall & 0x00FF) * 4;
		DragMarqueeCorner(where, &hDelta, &vDelta, false);
		if (hDelta < 64)
			hDelta = 64;
		if (vDelta < 32)
			vDelta = 32;
		g_thisRoom->objects[g_objActive].data.d.tall = ((hDelta / 4) << 8) + (vDelta / 4);
		whoCares = KeepObjectLegal();
		InvalidateRect(g_mainWindow, NULL, TRUE);
		GetThisRoomsObjRects();
		ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
		DrawThisRoomsObjects();
		break;

		case kFlourescent:
		case kTrackLight:
		hDelta = g_thisRoom->objects[g_objActive].data.f.length;
		DragMarqueeHandle(where, &hDelta);
		g_thisRoom->objects[g_objActive].data.f.length = hDelta;
		whoCares = KeepObjectLegal();
		InvalidateRect(g_mainWindow, NULL, TRUE);
		GetThisRoomsObjRects();
		ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
		DrawThisRoomsObjects();
		break;

		case kToaster:
		vDelta = g_thisRoom->objects[g_objActive].data.g.height;
		DragMarqueeHandle(where, &vDelta);
		g_thisRoom->objects[g_objActive].data.g.height = vDelta;
		whoCares = KeepObjectLegal();
		break;

		case kBall:
		case kDrip:
		case kFish:
		vDelta = g_thisRoom->objects[g_objActive].data.h.length;
		DragMarqueeHandle(where, &vDelta);
		g_thisRoom->objects[g_objActive].data.h.length = vDelta;
		whoCares = KeepObjectLegal();
		break;

		case kMirror:
		case kWallWindow:
		hDelta = RectWide(&g_thisRoom->objects[g_objActive].data.i.bounds);
		vDelta = RectTall(&g_thisRoom->objects[g_objActive].data.i.bounds);
		DragMarqueeCorner(where, &hDelta, &vDelta, false);
		g_thisRoom->objects[g_objActive].data.i.bounds.right =
			g_thisRoom->objects[g_objActive].data.i.bounds.left + hDelta;
		g_thisRoom->objects[g_objActive].data.i.bounds.bottom =
			g_thisRoom->objects[g_objActive].data.i.bounds.top + vDelta;
		whoCares = KeepObjectLegal();
		InvalidateRect(g_mainWindow, NULL, TRUE);
		GetThisRoomsObjRects();
		ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
		DrawThisRoomsObjects();
		break;
	}

	g_fileDirty = true;
	UpdateMenus(false);
}

//--------------------------------------------------------------  DragObject

void Gp_DragObject (Point where)
{
	Rect newRect, wasRect;
	SInt16 deltaH, deltaV, increment;
	Boolean invalAll;

	invalAll = false;

	if (g_objActive == kInitialGliderSelected)
	{
		wasRect = g_initialGliderRect;
		newRect = g_initialGliderRect;
		DragMarqueeRect(where, &newRect, false, false);
	}
	else if (g_objActive == kLeftGliderSelected)
	{
		wasRect = g_leftStartGliderDest;
		newRect = g_leftStartGliderDest;
		DragMarqueeRect(where, &newRect, false, true);
	}
	else if (g_objActive == kRightGliderSelected)
	{
		wasRect = g_rightStartGliderDest;
		newRect = g_rightStartGliderDest;
		DragMarqueeRect(where, &newRect, false, true);
	}
	else
	{
		wasRect = g_roomObjectRects[g_objActive];
		newRect = g_roomObjectRects[g_objActive];
		switch (g_thisRoom->objects[g_objActive].what)
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
		g_fileDirty = true;
		UpdateMenus(false);
	}

	if (g_objActive == kInitialGliderSelected)
	{
		g_thisHouse.initial.h += deltaH;
		g_thisHouse.initial.v += deltaV;
	}
	else if (g_objActive == kLeftGliderSelected)
	{
		increment = g_thisRoom->leftStart + deltaV;
		if (increment > 255)
			increment = 255;
		else if (increment < 0)
			increment = 0;
		g_thisRoom->leftStart = (Byte)increment;
		QSetRect(&g_leftStartGliderDest, 0, 0, 48, 16);
		QOffsetRect(&g_leftStartGliderDest, 0,
				kGliderStartsDown + (SInt16)g_thisRoom->leftStart);
	}
	else if (g_objActive == kRightGliderSelected)
	{
		increment = g_thisRoom->rightStart + deltaV;
		if (increment > 255)
			increment = 255;
		else if (increment < 0)
			increment = 0;
		g_thisRoom->rightStart = (Byte)increment;
		QSetRect(&g_rightStartGliderDest, 0, 0, 48, 16);
		QOffsetRect(&g_rightStartGliderDest, 0,
				kGliderStartsDown + (SInt16)g_thisRoom->rightStart);
	}
	else
	{
		switch (g_thisRoom->objects[g_objActive].what)
		{
			case kFloorVent:
			case kCeilingVent:
			case kFloorBlower:
			case kCeilingBlower:
			case kSewerGrate:
			case kGrecoVent:
			case kSewerBlower:
			g_thisRoom->objects[g_objActive].data.a.topLeft.h += deltaH;
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
			g_thisRoom->objects[g_objActive].data.a.topLeft.h += deltaH;
			g_thisRoom->objects[g_objActive].data.a.topLeft.v += deltaV;
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
			g_thisRoom->objects[g_objActive].data.b.bounds.left += deltaH;
			g_thisRoom->objects[g_objActive].data.b.bounds.right += deltaH;
			g_thisRoom->objects[g_objActive].data.b.bounds.top += deltaV;
			g_thisRoom->objects[g_objActive].data.b.bounds.bottom += deltaV;
			break;

			case kCounter:
			case kDresser:
			case kManhole:
			g_thisRoom->objects[g_objActive].data.b.bounds.left += deltaH;
			g_thisRoom->objects[g_objActive].data.b.bounds.right += deltaH;
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
			g_thisRoom->objects[g_objActive].data.c.topLeft.h += deltaH;
			g_thisRoom->objects[g_objActive].data.c.topLeft.v += deltaV;
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
			g_thisRoom->objects[g_objActive].data.d.topLeft.h += deltaH;
			break;

			case kMailboxLf:
			case kMailboxRt:
			case kInvisTrans:
			case kDeluxeTrans:
			g_thisRoom->objects[g_objActive].data.d.topLeft.h += deltaH;
			g_thisRoom->objects[g_objActive].data.d.topLeft.v += deltaV;
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
			g_thisRoom->objects[g_objActive].data.e.topLeft.h += deltaH;
			g_thisRoom->objects[g_objActive].data.e.topLeft.v += deltaV;
			break;

			case kCeilingLight:
			case kHipLamp:
			case kDecoLamp:
			case kFlourescent:
			case kTrackLight:
			g_thisRoom->objects[g_objActive].data.f.topLeft.h += deltaH;
			break;

			case kLightBulb:
			case kTableLamp:
			case kInvisLight:
			g_thisRoom->objects[g_objActive].data.f.topLeft.h += deltaH;
			g_thisRoom->objects[g_objActive].data.f.topLeft.v += deltaV;
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
			g_thisRoom->objects[g_objActive].data.g.topLeft.h += deltaH;
			g_thisRoom->objects[g_objActive].data.g.topLeft.v += deltaV;
			break;

			case kBalloon:
			case kCopterLf:
			case kCopterRt:
			g_thisRoom->objects[g_objActive].data.h.topLeft.h += deltaH;
			break;

			case kDartLf:
			case kDartRt:
			g_thisRoom->objects[g_objActive].data.h.topLeft.v += deltaV;
			break;

			case kBall:
			case kDrip:
			case kFish:
			case kCobweb:
			g_thisRoom->objects[g_objActive].data.h.topLeft.h += deltaH;
			g_thisRoom->objects[g_objActive].data.h.topLeft.v += deltaV;
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
			g_thisRoom->objects[g_objActive].data.i.bounds.left += deltaH;
			g_thisRoom->objects[g_objActive].data.i.bounds.right += deltaH;
			g_thisRoom->objects[g_objActive].data.i.bounds.top += deltaV;
			g_thisRoom->objects[g_objActive].data.i.bounds.bottom += deltaV;
			break;

			case kMousehole:
			case kFireplace:
			g_thisRoom->objects[g_objActive].data.i.bounds.left += deltaH;
			g_thisRoom->objects[g_objActive].data.i.bounds.right += deltaH;
			break;
		}
	}

	if (KeepObjectLegal())
	{
	}
	GetThisRoomsObjRects();
	if (invalAll)
		InvalidateRect(g_mainWindow, NULL, TRUE);
	else
	{
		Mac_InvalWindowRect(g_mainWindow, &wasRect);
		if (g_objActive == kInitialGliderSelected)
			Mac_InvalWindowRect(g_mainWindow, &g_initialGliderRect);
		else if (g_objActive == kLeftGliderSelected)
			Mac_InvalWindowRect(g_mainWindow, &g_leftStartGliderDest);
		else if (g_objActive == kRightGliderSelected)
			Mac_InvalWindowRect(g_mainWindow, &g_rightStartGliderDest);
		else
			Mac_InvalWindowRect(g_mainWindow, &g_roomObjectRects[g_objActive]);
	}

	ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
	DrawThisRoomsObjects();
}

//--------------------------------------------------------------  DoNewObjectClick

void DoNewObjectClick (HWND ownerWindow, Point where)
{
	SInt16 whatObject;

	if (COMPILEDEMO)
		return;

	StopMarquee();
	g_objActive = kNoObjectSelected;

	whatObject = g_toolSelected + ((g_toolMode - 1) * 0x0010);
	if (AddNewObject(ownerWindow, where, whatObject, true))
		IgnoreThisClick();
	UpdateMenus(false);

	AddObjectPairing();
}

//--------------------------------------------------------------  AddObjectPairing

void AddObjectPairing (void)
{
	roomType *testRoomPtr;
	SInt16 roomNum;
	SInt16 emptySlot;
	wchar_t message[256];
	HWND mssgWindow;

	if (COMPILEDEMO)
		return;

	if (g_thisRoom->objects[g_objActive].what == kDoorInRt)
	{
		roomNum = DoesNeighborRoomExist(kRoomToRight);
		if (roomNum != -1)
		{
			emptySlot = FindObjectSlotInRoom(roomNum);
			if ((emptySlot != -1) && (!DoesRoomNumHaveObject(roomNum, kDoorExLf)))
			{
				testRoomPtr = &(g_thisHouse.rooms[roomNum]);

				testRoomPtr->objects[emptySlot].what = kDoorExLf;
				testRoomPtr->objects[emptySlot].data.d.topLeft.h = kDoorExLfLeft;
				testRoomPtr->objects[emptySlot].data.d.topLeft.v = kDoorExTop;
				testRoomPtr->objects[emptySlot].data.d.tall = 0;
				testRoomPtr->objects[emptySlot].data.d.where = -1;
				testRoomPtr->objects[emptySlot].data.d.who = 255;
				testRoomPtr->objects[emptySlot].data.d.wide = 0;

				testRoomPtr->numObjects++;

				GetLocalizedString(45, message, ARRAYSIZE(message));
				mssgWindow = OpenMessageWindow(message, g_mainWindow);
				SetMessageTextColor(mssgWindow, blueColor);
				GetLocalizedString(46, message, ARRAYSIZE(message));
				SetMessageWindowMessage(mssgWindow, message);
				Sleep(1000);
				CloseMessageWindow(mssgWindow);
			}
		}
	}
	else if (g_thisRoom->objects[g_objActive].what == kDoorInLf)
	{
		roomNum = DoesNeighborRoomExist(kRoomToLeft);
		if (roomNum != -1)
		{
			emptySlot = FindObjectSlotInRoom(roomNum);
			if ((emptySlot != -1) && (!DoesRoomNumHaveObject(roomNum, kDoorExRt)))
			{
				testRoomPtr = &(g_thisHouse.rooms[roomNum]);

				testRoomPtr->objects[emptySlot].what = kDoorExRt;
				testRoomPtr->objects[emptySlot].data.d.topLeft.h = kDoorExRtLeft;
				testRoomPtr->objects[emptySlot].data.d.topLeft.v = kDoorExTop;
				testRoomPtr->objects[emptySlot].data.d.tall = 0;
				testRoomPtr->objects[emptySlot].data.d.where = -1;
				testRoomPtr->objects[emptySlot].data.d.who = 255;
				testRoomPtr->objects[emptySlot].data.d.wide = 0;

				testRoomPtr->numObjects++;

				GetLocalizedString(45, message, ARRAYSIZE(message));
				mssgWindow = OpenMessageWindow(message, g_mainWindow);
				SetMessageTextColor(mssgWindow, blueColor);
				GetLocalizedString(46, message, ARRAYSIZE(message));
				SetMessageWindowMessage(mssgWindow, message);
				Sleep(1000);
				CloseMessageWindow(mssgWindow);
			}
		}
	}
	else if (g_thisRoom->objects[g_objActive].what == kDoorExRt)
	{
		roomNum = DoesNeighborRoomExist(kRoomToRight);
		if (roomNum != -1)
		{
			emptySlot = FindObjectSlotInRoom(roomNum);
			if ((emptySlot != -1) && (!DoesRoomNumHaveObject(roomNum, kDoorInLf)))
			{
				testRoomPtr = &(g_thisHouse.rooms[roomNum]);

				testRoomPtr->objects[emptySlot].what = kDoorInLf;
				testRoomPtr->objects[emptySlot].data.d.topLeft.h = kDoorInLfLeft;
				testRoomPtr->objects[emptySlot].data.d.topLeft.v = kDoorInTop;
				testRoomPtr->objects[emptySlot].data.d.tall = 0;
				testRoomPtr->objects[emptySlot].data.d.where = -1;
				testRoomPtr->objects[emptySlot].data.d.who = 255;
				testRoomPtr->objects[emptySlot].data.d.wide = 0;

				testRoomPtr->numObjects++;

				GetLocalizedString(45, message, ARRAYSIZE(message));
				mssgWindow = OpenMessageWindow(message, g_mainWindow);
				SetMessageTextColor(mssgWindow, blueColor);
				GetLocalizedString(47, message, ARRAYSIZE(message));
				SetMessageWindowMessage(mssgWindow, message);
				Sleep(1000);
				CloseMessageWindow(mssgWindow);
			}
		}
	}
	else if (g_thisRoom->objects[g_objActive].what == kDoorExLf)
	{
		roomNum = DoesNeighborRoomExist(kRoomToLeft);
		if (roomNum != -1)
		{
			emptySlot = FindObjectSlotInRoom(roomNum);
			if ((emptySlot != -1) && (!DoesRoomNumHaveObject(roomNum, kDoorInRt)))
			{
				testRoomPtr = &(g_thisHouse.rooms[roomNum]);

				testRoomPtr->objects[emptySlot].what = kDoorInRt;
				testRoomPtr->objects[emptySlot].data.d.topLeft.h = kDoorInRtLeft;
				testRoomPtr->objects[emptySlot].data.d.topLeft.v = kDoorInTop;
				testRoomPtr->objects[emptySlot].data.d.tall = 0;
				testRoomPtr->objects[emptySlot].data.d.where = -1;
				testRoomPtr->objects[emptySlot].data.d.who = 255;
				testRoomPtr->objects[emptySlot].data.d.wide = 0;

				testRoomPtr->numObjects++;

				GetLocalizedString(45, message, ARRAYSIZE(message));
				mssgWindow = OpenMessageWindow(message, g_mainWindow);
				SetMessageTextColor(mssgWindow, blueColor);
				GetLocalizedString(47, message, ARRAYSIZE(message));
				SetMessageWindowMessage(mssgWindow, message);
				Sleep(1000);
				CloseMessageWindow(mssgWindow);
			}
		}
	}
	else if (g_thisRoom->objects[g_objActive].what == kWindowInLf)
	{
		roomNum = DoesNeighborRoomExist(kRoomToLeft);
		if (roomNum != -1)
		{
			emptySlot = FindObjectSlotInRoom(roomNum);
			if ((emptySlot != -1) && (!DoesRoomNumHaveObject(roomNum, kWindowExRt)))
			{
				testRoomPtr = &(g_thisHouse.rooms[roomNum]);

				testRoomPtr->objects[emptySlot].what = kWindowExRt;
				testRoomPtr->objects[emptySlot].data.d.topLeft.h = kWindowExRtLeft;
				testRoomPtr->objects[emptySlot].data.d.topLeft.v = kWindowExTop;
				testRoomPtr->objects[emptySlot].data.d.tall = 0;
				testRoomPtr->objects[emptySlot].data.d.where = -1;
				testRoomPtr->objects[emptySlot].data.d.who = 255;
				testRoomPtr->objects[emptySlot].data.d.wide = 0;

				testRoomPtr->numObjects++;

				GetLocalizedString(45, message, ARRAYSIZE(message));
				mssgWindow = OpenMessageWindow(message, g_mainWindow);
				SetMessageTextColor(mssgWindow, blueColor);
				GetLocalizedString(48, message, ARRAYSIZE(message));
				SetMessageWindowMessage(mssgWindow, message);
				Sleep(1000);
				CloseMessageWindow(mssgWindow);
			}
		}
	}
	else if (g_thisRoom->objects[g_objActive].what == kWindowInRt)
	{
		roomNum = DoesNeighborRoomExist(kRoomToRight);
		if (roomNum != -1)
		{
			emptySlot = FindObjectSlotInRoom(roomNum);
			if ((emptySlot != -1) && (!DoesRoomNumHaveObject(roomNum, kWindowExLf)))
			{
				testRoomPtr = &(g_thisHouse.rooms[roomNum]);

				testRoomPtr->objects[emptySlot].what = kWindowExLf;
				testRoomPtr->objects[emptySlot].data.d.topLeft.h = kWindowExLfLeft;
				testRoomPtr->objects[emptySlot].data.d.topLeft.v = kWindowExTop;
				testRoomPtr->objects[emptySlot].data.d.tall = 0;
				testRoomPtr->objects[emptySlot].data.d.where = -1;
				testRoomPtr->objects[emptySlot].data.d.who = 255;
				testRoomPtr->objects[emptySlot].data.d.wide = 0;

				testRoomPtr->numObjects++;

				GetLocalizedString(45, message, ARRAYSIZE(message));
				mssgWindow = OpenMessageWindow(message, g_mainWindow);
				SetMessageTextColor(mssgWindow, blueColor);
				GetLocalizedString(48, message, ARRAYSIZE(message));
				SetMessageWindowMessage(mssgWindow, message);
				Sleep(1000);
				CloseMessageWindow(mssgWindow);
			}
		}
	}
	else if (g_thisRoom->objects[g_objActive].what == kWindowExRt)
	{
		roomNum = DoesNeighborRoomExist(kRoomToRight);
		if (roomNum != -1)
		{
			emptySlot = FindObjectSlotInRoom(roomNum);
			if ((emptySlot != -1) && (!DoesRoomNumHaveObject(roomNum, kWindowInLf)))
			{
				testRoomPtr = &(g_thisHouse.rooms[roomNum]);

				testRoomPtr->objects[emptySlot].what = kWindowInLf;
				testRoomPtr->objects[emptySlot].data.d.topLeft.h = kWindowInLfLeft;
				testRoomPtr->objects[emptySlot].data.d.topLeft.v = kWindowInTop;
				testRoomPtr->objects[emptySlot].data.d.tall = 0;
				testRoomPtr->objects[emptySlot].data.d.where = -1;
				testRoomPtr->objects[emptySlot].data.d.who = 255;
				testRoomPtr->objects[emptySlot].data.d.wide = 0;

				testRoomPtr->numObjects++;

				GetLocalizedString(45, message, ARRAYSIZE(message));
				mssgWindow = OpenMessageWindow(message, g_mainWindow);
				SetMessageTextColor(mssgWindow, blueColor);
				GetLocalizedString(49, message, ARRAYSIZE(message));
				SetMessageWindowMessage(mssgWindow, message);
				Sleep(1000);
				CloseMessageWindow(mssgWindow);
			}
		}
	}
	else if (g_thisRoom->objects[g_objActive].what == kWindowExLf)
	{
		roomNum = DoesNeighborRoomExist(kRoomToLeft);
		if (roomNum != -1)
		{
			emptySlot = FindObjectSlotInRoom(roomNum);
			if ((emptySlot != -1) && (!DoesRoomNumHaveObject(roomNum, kWindowInRt)))
			{
				testRoomPtr = &(g_thisHouse.rooms[roomNum]);

				testRoomPtr->objects[emptySlot].what = kWindowInRt;
				testRoomPtr->objects[emptySlot].data.d.topLeft.h = kWindowInRtLeft;
				testRoomPtr->objects[emptySlot].data.d.topLeft.v = kWindowInTop;
				testRoomPtr->objects[emptySlot].data.d.tall = 0;
				testRoomPtr->objects[emptySlot].data.d.where = -1;
				testRoomPtr->objects[emptySlot].data.d.who = 255;
				testRoomPtr->objects[emptySlot].data.d.wide = 0;

				testRoomPtr->numObjects++;

				GetLocalizedString(45, message, ARRAYSIZE(message));
				mssgWindow = OpenMessageWindow(message, g_mainWindow);
				SetMessageTextColor(mssgWindow, blueColor);
				GetLocalizedString(49, message, ARRAYSIZE(message));
				SetMessageWindowMessage(mssgWindow, message);
				Sleep(1000);
				CloseMessageWindow(mssgWindow);
			}
		}
	}
	else if (g_thisRoom->objects[g_objActive].what == kUpStairs)
	{
		roomNum = DoesNeighborRoomExist(kRoomAbove);
		if (roomNum != -1)
		{
			emptySlot = FindObjectSlotInRoom(roomNum);
			if ((emptySlot != -1) && (!DoesRoomNumHaveObject(roomNum, kDownStairs)))
			{
				testRoomPtr = &(g_thisHouse.rooms[roomNum]);

				testRoomPtr->objects[emptySlot].what = kDownStairs;
				testRoomPtr->objects[emptySlot].data.d.topLeft.h =
						g_thisRoom->objects[g_objActive].data.d.topLeft.h;
				testRoomPtr->objects[emptySlot].data.d.topLeft.v = kStairsTop;
				testRoomPtr->objects[emptySlot].data.d.tall = 0;
				testRoomPtr->objects[emptySlot].data.d.where = -1;
				testRoomPtr->objects[emptySlot].data.d.who = 255;
				testRoomPtr->objects[emptySlot].data.d.wide = 0;

				testRoomPtr->numObjects++;

				GetLocalizedString(45, message, ARRAYSIZE(message));
				mssgWindow = OpenMessageWindow(message, g_mainWindow);
				SetMessageTextColor(mssgWindow, blueColor);
				GetLocalizedString(50, message, ARRAYSIZE(message));
				SetMessageWindowMessage(mssgWindow, message);
				Sleep(1000);
				CloseMessageWindow(mssgWindow);
			}
		}
	}
	else if (g_thisRoom->objects[g_objActive].what == kDownStairs)
	{
		roomNum = DoesNeighborRoomExist(kRoomBelow);
		if (roomNum != -1)
		{
			emptySlot = FindObjectSlotInRoom(roomNum);
			if ((emptySlot != -1) && (!DoesRoomNumHaveObject(roomNum, kUpStairs)))
			{
				testRoomPtr = &(g_thisHouse.rooms[roomNum]);

				testRoomPtr->objects[emptySlot].what = kUpStairs;
				testRoomPtr->objects[emptySlot].data.d.topLeft.h =
						g_thisRoom->objects[g_objActive].data.d.topLeft.h;
				testRoomPtr->objects[emptySlot].data.d.topLeft.v = kStairsTop;
				testRoomPtr->objects[emptySlot].data.d.tall = 0;
				testRoomPtr->objects[emptySlot].data.d.where = -1;
				testRoomPtr->objects[emptySlot].data.d.who = 255;
				testRoomPtr->objects[emptySlot].data.d.wide = 0;

				testRoomPtr->numObjects++;

				GetLocalizedString(45, message, ARRAYSIZE(message));
				mssgWindow = OpenMessageWindow(message, g_mainWindow);
				SetMessageTextColor(mssgWindow, blueColor);
				GetLocalizedString(51, message, ARRAYSIZE(message));
				SetMessageWindowMessage(mssgWindow, message);
				Sleep(1000);
				CloseMessageWindow(mssgWindow);
			}
		}
	}
}

//--------------------------------------------------------------  DeleteObject

void Gp_DeleteObject (void)
{
	SInt16 i;

	if (COMPILEDEMO)
		return;

	if ((g_theMode != kEditMode) || (g_objActive == kNoObjectSelected))
		return;

	if ((g_objActive == kInitialGliderSelected) ||
			(g_objActive == kLeftGliderSelected) ||
			(g_objActive == kRightGliderSelected))
	{
		MessageBeep(MB_ICONWARNING);
		return;
	}

	for (i = 0; i < kMaxRoomObs; i++)
	{
		if ((g_retroLinkList[i].room == g_thisRoomNumber) &&
				(g_retroLinkList[i].object == g_objActive))
			g_retroLinkList[i].room = -1;
	}

	g_thisRoom->objects[g_objActive].what = kObjectIsEmpty;
	g_thisRoom->numObjects--;
	g_fileDirty = true;
	UpdateMenus(false);
	InvalidateRect(g_mainWindow, NULL, TRUE);
	QSetRect(&g_roomObjectRects[g_objActive], -1, -1, 0, 0);
	DeselectObject();
	ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
	DrawThisRoomsObjects();
}

//--------------------------------------------------------------  DuplicateObject

void DuplicateObject (HWND ownerWindow)
{
	objectType tempObject;
	Point placePt;

	if (COMPILEDEMO)
		return;

	tempObject = g_thisRoom->objects[g_objActive];

	placePt.h = g_roomObjectRects[g_objActive].left +
			HalfRectWide(&g_roomObjectRects[g_objActive]) + 64;
	placePt.v = g_roomObjectRects[g_objActive].top +
			HalfRectTall(&g_roomObjectRects[g_objActive]);

	StopMarquee();

	if (AddNewObject(ownerWindow, placePt, tempObject.what, false))
	{
		switch (tempObject.what)
		{
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
			g_thisRoom->objects[g_objActive].data.a.distance = tempObject.data.a.distance;
			g_thisRoom->objects[g_objActive].data.a.initial = tempObject.data.a.initial;
			g_thisRoom->objects[g_objActive].data.a.state = tempObject.data.a.state;
			g_thisRoom->objects[g_objActive].data.a.vector = tempObject.data.a.vector;
			g_thisRoom->objects[g_objActive].data.a.tall = tempObject.data.a.tall;
			break;

		case kLiftArea:
			g_thisRoom->objects[g_objActive].data.a.topLeft.h = tempObject.data.a.topLeft.h + 64;
			g_thisRoom->objects[g_objActive].data.a.topLeft.v = tempObject.data.a.topLeft.v;
			g_thisRoom->objects[g_objActive].data.a.distance = tempObject.data.a.distance;
			g_thisRoom->objects[g_objActive].data.a.initial = tempObject.data.a.initial;
			g_thisRoom->objects[g_objActive].data.a.state = tempObject.data.a.state;
			g_thisRoom->objects[g_objActive].data.a.vector = tempObject.data.a.vector;
			g_thisRoom->objects[g_objActive].data.a.tall = tempObject.data.a.tall;
			break;

		case kFilingCabinet:
		case kWasteBasket:
		case kMilkCrate:
		case kStool:
		case kTrunk:
		case kManhole:
		case kBooks:
			g_thisRoom->objects[g_objActive].data.b.pict = tempObject.data.b.pict;
			break;

		case kTable:
		case kShelf:
		case kCabinet:
		case kCounter:
		case kDresser:
		case kDeckTable:
		case kInvisObstacle:
		case kInvisBounce:
			g_thisRoom->objects[g_objActive].data.b.bounds = tempObject.data.b.bounds;
			QOffsetRect(&g_thisRoom->objects[g_objActive].data.b.bounds, 64, 0);
			g_thisRoom->objects[g_objActive].data.b.pict = tempObject.data.b.pict;
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
			g_thisRoom->objects[g_objActive].data.c.length = tempObject.data.c.length;
			g_thisRoom->objects[g_objActive].data.c.points = tempObject.data.c.points;
			g_thisRoom->objects[g_objActive].data.c.state = tempObject.data.c.state;
			g_thisRoom->objects[g_objActive].data.c.initial = tempObject.data.c.initial;
			break;

		case kSlider:
			g_thisRoom->objects[g_objActive].data.c.topLeft.h = tempObject.data.c.topLeft.h + 64;
			g_thisRoom->objects[g_objActive].data.c.length = tempObject.data.c.length;
			g_thisRoom->objects[g_objActive].data.c.points = tempObject.data.c.points;
			g_thisRoom->objects[g_objActive].data.c.state = tempObject.data.c.state;
			g_thisRoom->objects[g_objActive].data.c.initial = tempObject.data.c.initial;
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
			g_thisRoom->objects[g_objActive].data.d.tall = tempObject.data.d.tall;
			g_thisRoom->objects[g_objActive].data.d.where = tempObject.data.d.where;
			g_thisRoom->objects[g_objActive].data.d.who = tempObject.data.d.who;
			g_thisRoom->objects[g_objActive].data.d.wide = tempObject.data.d.wide;
			break;

		case kInvisTrans:
		case kDeluxeTrans:
			g_thisRoom->objects[g_objActive].data.d.topLeft.h = tempObject.data.d.topLeft.h + 64;
			g_thisRoom->objects[g_objActive].data.d.topLeft.v = tempObject.data.d.topLeft.v;
			g_thisRoom->objects[g_objActive].data.d.tall = tempObject.data.d.tall;
			g_thisRoom->objects[g_objActive].data.d.where = tempObject.data.d.where;
			g_thisRoom->objects[g_objActive].data.d.who = tempObject.data.d.who;
			g_thisRoom->objects[g_objActive].data.d.wide = tempObject.data.d.wide;
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
			g_thisRoom->objects[g_objActive].data.e.delay = tempObject.data.e.delay;
			g_thisRoom->objects[g_objActive].data.e.where = tempObject.data.e.where;
			g_thisRoom->objects[g_objActive].data.e.who = tempObject.data.e.who;
			g_thisRoom->objects[g_objActive].data.e.type = tempObject.data.e.type;
			break;

		case kCeilingLight:
		case kLightBulb:
		case kTableLamp:
		case kHipLamp:
		case kDecoLamp:
		case kFlourescent:
		case kTrackLight:
		case kInvisLight:
			g_thisRoom->objects[g_objActive].data.f.length = tempObject.data.f.length;
			g_thisRoom->objects[g_objActive].data.f.byte0 = tempObject.data.f.byte0;
			g_thisRoom->objects[g_objActive].data.f.byte1 = tempObject.data.f.byte1;
			g_thisRoom->objects[g_objActive].data.f.initial = tempObject.data.f.initial;
			g_thisRoom->objects[g_objActive].data.f.state = tempObject.data.f.state;
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
			g_thisRoom->objects[g_objActive].data.g.height = tempObject.data.g.height;
			g_thisRoom->objects[g_objActive].data.g.byte0 = tempObject.data.g.byte0;
			g_thisRoom->objects[g_objActive].data.g.delay = tempObject.data.g.delay;
			g_thisRoom->objects[g_objActive].data.g.initial = tempObject.data.g.initial;
			g_thisRoom->objects[g_objActive].data.g.state = tempObject.data.g.state;
			break;

		case kCustomPict:
			g_thisRoom->objects[g_objActive].data.g.topLeft.h =
				tempObject.data.g.topLeft.h + 64;
			g_thisRoom->objects[g_objActive].data.g.topLeft.v = tempObject.data.g.topLeft.v;
			g_thisRoom->objects[g_objActive].data.g.height = tempObject.data.g.height;
			g_thisRoom->objects[g_objActive].data.g.byte0 = tempObject.data.g.byte0;
			g_thisRoom->objects[g_objActive].data.g.delay = tempObject.data.g.delay;
			g_thisRoom->objects[g_objActive].data.g.initial = tempObject.data.g.initial;
			g_thisRoom->objects[g_objActive].data.g.state = tempObject.data.g.state;
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
			g_thisRoom->objects[g_objActive].data.h.length = tempObject.data.h.length;
			g_thisRoom->objects[g_objActive].data.h.delay = tempObject.data.h.delay;
			g_thisRoom->objects[g_objActive].data.h.byte0 = tempObject.data.h.byte0;
			g_thisRoom->objects[g_objActive].data.h.initial = tempObject.data.h.initial;
			g_thisRoom->objects[g_objActive].data.h.state = tempObject.data.h.state;
			break;

		case kOzma:
		case kMousehole:
		case kFireplace:
		case kBear:
		case kCalendar:
		case kVase1:
		case kVase2:
		case kBulletin:
		case kCloud:
		case kFaucet:
		case kRug:
		case kChimes:
			g_thisRoom->objects[g_objActive].data.i.pict = tempObject.data.i.pict;
			break;

		case kMirror:
		case kFlower:
		case kWallWindow:
			g_thisRoom->objects[g_objActive].data.i.bounds = tempObject.data.i.bounds;
			QOffsetRect(&g_thisRoom->objects[g_objActive].data.i.bounds, 64, 0);
			g_thisRoom->objects[g_objActive].data.i.pict = tempObject.data.i.pict;
			break;
		}

		if (KeepObjectLegal())
		{
		}

		ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
		GetThisRoomsObjRects();
		DrawThisRoomsObjects();
		InvalidateRect(g_mainWindow, NULL, TRUE);
		StartMarqueeForActiveObject();
	}
}

//--------------------------------------------------------------  MoveObject

void MoveObject (SInt16 whichWay, Boolean shiftDown)
{
	Rect wasRect;
	SInt16 deltaH;
	SInt16 deltaV;
	SInt16 increment;

	if (COMPILEDEMO)
		return;

	if (g_theMode != kEditMode)
		return;

	StopMarquee();

	if (shiftDown)
		increment = 10;
	else
	{
		if (g_objActive == kInitialGliderSelected)
		{
			increment = 1;
		}
		else
		{
			if ((whichWay == kBumpRight) || (whichWay == kBumpLeft))
			{
				switch (g_thisRoom->objects[g_objActive].what)
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

	if (g_objActive == kInitialGliderSelected)
	{
		wasRect = g_initialGliderRect;
		g_thisHouse.initial.h += deltaH;
		g_thisHouse.initial.v += deltaV;
	}
	else if (g_objActive == kLeftGliderSelected)
	{
		wasRect = g_leftStartGliderDest;
		increment = g_thisRoom->leftStart + deltaV;
		if (increment > 255)
			increment = 255;
		else if (increment < 0)
			increment = 0;
		g_thisRoom->leftStart = (Byte)increment;
		QSetRect(&g_leftStartGliderDest, 0, 0, 48, 16);
		QOffsetRect(&g_leftStartGliderDest, 0,
				kGliderStartsDown + (SInt16)g_thisRoom->leftStart);
	}
	else if (g_objActive == kRightGliderSelected)
	{
		wasRect = g_rightStartGliderDest;
		increment = g_thisRoom->rightStart + deltaV;
		if (increment > 255)
			increment = 255;
		else if (increment < 0)
			increment = 0;
		g_thisRoom->rightStart = (Byte)increment;
		QSetRect(&g_rightStartGliderDest, 0, 0, 48, 16);
		QOffsetRect(&g_rightStartGliderDest, kRoomWide - 48,
				kGliderStartsDown + (SInt16)g_thisRoom->rightStart);
	}
	else
	{
		wasRect = g_roomObjectRects[g_objActive];
		switch (g_thisRoom->objects[g_objActive].what)
		{
			case kFloorVent:
			case kCeilingVent:
			case kFloorBlower:
			case kCeilingBlower:
			case kSewerGrate:
			case kGrecoVent:
			case kSewerBlower:
			g_thisRoom->objects[g_objActive].data.a.topLeft.h += deltaH;
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
			g_thisRoom->objects[g_objActive].data.a.topLeft.h += deltaH;
			g_thisRoom->objects[g_objActive].data.a.topLeft.v += deltaV;
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
			g_thisRoom->objects[g_objActive].data.b.bounds.left += deltaH;
			g_thisRoom->objects[g_objActive].data.b.bounds.right += deltaH;
			g_thisRoom->objects[g_objActive].data.b.bounds.top += deltaV;
			g_thisRoom->objects[g_objActive].data.b.bounds.bottom += deltaV;
			break;

			case kCounter:
			case kDresser:
			case kManhole:
			g_thisRoom->objects[g_objActive].data.b.bounds.left += deltaH;
			g_thisRoom->objects[g_objActive].data.b.bounds.right += deltaH;
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
			g_thisRoom->objects[g_objActive].data.c.topLeft.h += deltaH;
			g_thisRoom->objects[g_objActive].data.c.topLeft.v += deltaV;
			break;

			case kUpStairs:
			case kDownStairs:
			case kFloorTrans:
			case kCeilingTrans:
			g_thisRoom->objects[g_objActive].data.d.topLeft.h += deltaH;
			break;

			case kMailboxLf:
			case kMailboxRt:
			case kInvisTrans:
			case kDeluxeTrans:
			g_thisRoom->objects[g_objActive].data.d.topLeft.h += deltaH;
			g_thisRoom->objects[g_objActive].data.d.topLeft.v += deltaV;
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
			g_thisRoom->objects[g_objActive].data.e.topLeft.h += deltaH;
			g_thisRoom->objects[g_objActive].data.e.topLeft.v += deltaV;
			break;

			case kCeilingLight:
			case kHipLamp:
			case kDecoLamp:
			case kFlourescent:
			case kTrackLight:
			g_thisRoom->objects[g_objActive].data.f.topLeft.h += deltaH;
			break;

			case kLightBulb:
			case kTableLamp:
			case kInvisLight:
			g_thisRoom->objects[g_objActive].data.f.topLeft.h += deltaH;
			g_thisRoom->objects[g_objActive].data.f.topLeft.v += deltaV;
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
			g_thisRoom->objects[g_objActive].data.g.topLeft.h += deltaH;
			g_thisRoom->objects[g_objActive].data.g.topLeft.v += deltaV;
			break;

			case kBalloon:
			case kCopterLf:
			case kCopterRt:
			g_thisRoom->objects[g_objActive].data.h.topLeft.h += deltaH;
			break;

			case kDartLf:
			case kDartRt:
			g_thisRoom->objects[g_objActive].data.h.topLeft.v += deltaV;
			break;

			case kBall:
			case kDrip:
			case kFish:
			case kCobweb:
			g_thisRoom->objects[g_objActive].data.h.topLeft.h += deltaH;
			g_thisRoom->objects[g_objActive].data.h.topLeft.v += deltaV;
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
			g_thisRoom->objects[g_objActive].data.i.bounds.left += deltaH;
			g_thisRoom->objects[g_objActive].data.i.bounds.right += deltaH;
			g_thisRoom->objects[g_objActive].data.i.bounds.top += deltaV;
			g_thisRoom->objects[g_objActive].data.i.bounds.bottom += deltaV;
			break;

			case kMousehole:
			case kFireplace:
			g_thisRoom->objects[g_objActive].data.i.bounds.left += deltaH;
			g_thisRoom->objects[g_objActive].data.i.bounds.right += deltaH;
			break;

		}
	}

	if (KeepObjectLegal())
	{
	}
	g_fileDirty = true;
	UpdateMenus(false);
	GetThisRoomsObjRects();

	if (g_objActive == kInitialGliderSelected)
	{
		Mac_InvalWindowRect(g_mainWindow, &wasRect);
		Mac_InvalWindowRect(g_mainWindow, &g_initialGliderRect);
	}
	else if (g_objActive == kLeftGliderSelected)
	{
		Mac_InvalWindowRect(g_mainWindow, &wasRect);
		Mac_InvalWindowRect(g_mainWindow, &g_leftStartGliderDest);
	}
	else if (g_objActive == kRightGliderSelected)
	{
		Mac_InvalWindowRect(g_mainWindow, &wasRect);
		Mac_InvalWindowRect(g_mainWindow, &g_rightStartGliderDest);
	}
	else
	{
		switch (g_thisRoom->objects[g_objActive].what)
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
			InvalidateRect(g_mainWindow, NULL, TRUE);
			break;

			default:
			Mac_InvalWindowRect(g_mainWindow, &wasRect);
			Mac_InvalWindowRect(g_mainWindow, &g_roomObjectRects[g_objActive]);
			break;
		}
	}

	ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
	DrawThisRoomsObjects();
	StartMarqueeForActiveObject();
}

//--------------------------------------------------------------  DeselectObject

void DeselectObject (void)
{
	if (COMPILEDEMO)
		return;

	if ((g_theMode != kEditMode) || (g_objActive == kNoObjectSelected))
		return;

	g_objActive = kNoObjectSelected;
	StopMarquee();
	UpdateMenus(false);
}

//--------------------------------------------------------------  ObjectHasHandle

Boolean ObjectHasHandle (SInt16 *direction, SInt16 *dist)
{
	if ((g_objActive == kInitialGliderSelected) ||
		(g_objActive == kLeftGliderSelected) ||
		(g_objActive == kRightGliderSelected) ||
		(g_objActive == kNoObjectSelected))
	{
		return (false);
	}
	if (g_objActive < 0 || g_objActive >= kMaxRoomObs)
	{
		return (false);
	}

	switch (g_thisRoom->objects[g_objActive].what)
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
		*dist = g_thisRoom->objects[g_objActive].data.a.distance;
		return (true);
		break;

		case kCeilingVent:
		case kCeilingBlower:
		*direction = kBelow;
		*dist = g_thisRoom->objects[g_objActive].data.a.distance;
		return (true);
		break;

		case kLeftFan:
		*direction = kToLeft;
		*dist = g_thisRoom->objects[g_objActive].data.a.distance;
		return (true);
		break;

		case kRightFan:
		*direction = kToRight;
		*dist = g_thisRoom->objects[g_objActive].data.a.distance;
		return (true);
		break;

		case kInvisBlower:
		switch (g_thisRoom->objects[g_objActive].data.a.vector & 0x0F)
		{
			case 1:  // up
			*direction = kAbove;
			break;

			case 2:  // right
			*direction = kToRight;
			break;

			case 4:  // down
			*direction = kBelow;
			break;

			case 8:  // left
			*direction = kToLeft;
			break;

			default:  // invalid vector
			return (false);
			break;
		}
		*dist = g_thisRoom->objects[g_objActive].data.a.distance;
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
		*dist = g_thisRoom->objects[g_objActive].data.c.length;
		return (true);
		break;

		case kGreaseLf:
		*direction = kToLeft;
		*dist = g_thisRoom->objects[g_objActive].data.c.length;
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
		*dist = g_thisRoom->objects[g_objActive].data.g.height;
		return (true);
		break;

		case kBall:
		case kFish:
		*direction = kAbove;
		*dist = g_thisRoom->objects[g_objActive].data.h.length;
		return (true);
		break;

		case kDrip:
		*direction = kBelow;
		*dist = g_thisRoom->objects[g_objActive].data.h.length;
		return (true);
		break;

		default:
		return (false);
		break;
	}
}

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
	SInt16 direction, dist;

	if (COMPILEDEMO)
		return;

	if (ObjectIsUpBlower(&g_thisRoom->objects[g_objActive]))
	{
		if (ObjectHasHandle(&direction, &dist))
		{
			SetMarqueeGliderRect(((g_roomObjectRects[g_objActive].right +
					g_roomObjectRects[g_objActive].left) / 2),
					g_roomObjectRects[g_objActive].top - dist);
		}
	}
}

//--------------------------------------------------------------  SelectNextObject

void SelectNextObject (void)
{
	Boolean noneFound;

	if (COMPILEDEMO)
		return;

	if ((g_theMode != kEditMode) || (g_thisRoom->numObjects <= 0))
		return;

	noneFound = true;

	while (noneFound)
	{
		g_objActive++;
		if (g_objActive < 0 || g_objActive >= kMaxRoomObs)
			g_objActive = 0;
		if (g_thisRoom->objects[g_objActive].what != kObjectIsEmpty)
			noneFound = false;
	}

	UpdateMenus(false);
	StartMarqueeForActiveObject();
}

//--------------------------------------------------------------  SelectPrevObject

void SelectPrevObject (void)
{
	Boolean noneFound;

	if (COMPILEDEMO)
		return;

	if ((g_theMode != kEditMode) || (g_thisRoom->numObjects <= 0))
		return;

	noneFound = true;

	while (noneFound)
	{
		g_objActive--;
		if (g_objActive < 0 || g_objActive >= kMaxRoomObs)
			g_objActive = kMaxRoomObs - 1;
		if (g_thisRoom->objects[g_objActive].what != kObjectIsEmpty)
			noneFound = false;
	}

	UpdateMenus(false);
	StartMarqueeForActiveObject();
}

//--------------------------------------------------------------  GetThisRoomsObjRects

void GetThisRoomsObjRects (void)
{
	HBITMAP thePict;
	SInt16 i, wide, tall;
	BITMAP bmInfo;

	g_isFirstRoom = (GetFirstRoomNumber() == g_thisRoomNumber);

	if ((g_isFirstRoom) && (!g_noRoomAtAll) && (g_houseUnlocked))
		WhereDoesGliderBegin(&g_initialGliderRect, kNewGameMode);
	else
		QSetRect(&g_initialGliderRect, 0, 0, 0, 0);

	QSetRect(&g_leftStartGliderDest, 0, 0, 48, 16);
	QOffsetRect(&g_leftStartGliderDest, 0,
			kGliderStartsDown + (SInt16)g_thisRoom->leftStart);

	QSetRect(&g_rightStartGliderDest, 0, 0, 48, 16);
	QOffsetRect(&g_rightStartGliderDest, kRoomWide - 48,
			kGliderStartsDown + (SInt16)g_thisRoom->rightStart);

	if ((g_noRoomAtAll) || (!g_houseUnlocked))
	{
		return;
	}
	else
	{
		for (i = 0; i < kMaxRoomObs; i++)
		{
			switch (g_thisRoom->objects[i].what)
			{
				case kObjectIsEmpty:
				QSetRect(&g_roomObjectRects[i], -2, -2, -1, -1);
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
				g_roomObjectRects[i] = g_srcRects[g_thisRoom->objects[i].what];
				ZeroRectCorner(&g_roomObjectRects[i]);
				QOffsetRect(&g_roomObjectRects[i],
						g_thisRoom->objects[i].data.a.topLeft.h,
						g_thisRoom->objects[i].data.a.topLeft.v);
				break;

				case kLiftArea:
				QSetRect(&g_roomObjectRects[i], 0, 0,
						g_thisRoom->objects[i].data.a.distance,
						g_thisRoom->objects[i].data.a.tall * 2);
				QOffsetRect(&g_roomObjectRects[i],
						g_thisRoom->objects[i].data.a.topLeft.h,
						g_thisRoom->objects[i].data.a.topLeft.v);
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
				g_roomObjectRects[i] = g_thisRoom->objects[i].data.b.bounds;
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
				g_roomObjectRects[i] = g_srcRects[g_thisRoom->objects[i].what];
				ZeroRectCorner(&g_roomObjectRects[i]);
				QOffsetRect(&g_roomObjectRects[i],
						g_thisRoom->objects[i].data.c.topLeft.h,
						g_thisRoom->objects[i].data.c.topLeft.v);
				break;

				case kGreaseRt:
				g_roomObjectRects[i] = g_srcRects[g_thisRoom->objects[i].what];
				ZeroRectCorner(&g_roomObjectRects[i]);
				QOffsetRect(&g_roomObjectRects[i],
						g_thisRoom->objects[i].data.c.topLeft.h,
						g_thisRoom->objects[i].data.c.topLeft.v);
				if (!g_thisRoom->objects[i].data.c.initial)
					QOffsetRect(&g_roomObjectRects[i], 8, 0);
				break;

				case kGreaseLf:
				g_roomObjectRects[i] = g_srcRects[g_thisRoom->objects[i].what];
				ZeroRectCorner(&g_roomObjectRects[i]);
				QOffsetRect(&g_roomObjectRects[i],
						g_thisRoom->objects[i].data.c.topLeft.h,
						g_thisRoom->objects[i].data.c.topLeft.v);
				if (!g_thisRoom->objects[i].data.c.initial)
					QOffsetRect(&g_roomObjectRects[i], -8, 0);
				break;

				case kSlider:
				g_roomObjectRects[i] = g_srcRects[g_thisRoom->objects[i].what];
				ZeroRectCorner(&g_roomObjectRects[i]);
				QOffsetRect(&g_roomObjectRects[i],
						g_thisRoom->objects[i].data.c.topLeft.h,
						g_thisRoom->objects[i].data.c.topLeft.v);
				g_roomObjectRects[i].right = g_roomObjectRects[i].left +
						g_thisRoom->objects[i].data.c.length;
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
				g_roomObjectRects[i] = g_srcRects[g_thisRoom->objects[i].what];
				ZeroRectCorner(&g_roomObjectRects[i]);
				QOffsetRect(&g_roomObjectRects[i],
						g_thisRoom->objects[i].data.d.topLeft.h,
						g_thisRoom->objects[i].data.d.topLeft.v);
				break;

				case kInvisTrans:
				g_roomObjectRects[i] = g_srcRects[g_thisRoom->objects[i].what];
				ZeroRectCorner(&g_roomObjectRects[i]);
				QOffsetRect(&g_roomObjectRects[i],
						g_thisRoom->objects[i].data.d.topLeft.h,
						g_thisRoom->objects[i].data.d.topLeft.v);
				g_roomObjectRects[i].bottom = g_roomObjectRects[i].top +
						g_thisRoom->objects[i].data.d.tall;
				g_roomObjectRects[i].right += (SInt16)g_thisRoom->objects[i].data.d.wide;
				break;

				case kDeluxeTrans:  // Uses a kludge to get width & height (x4)
				wide = (g_thisRoom->objects[i].data.d.tall & 0xFF00) >> 8;
				tall = g_thisRoom->objects[i].data.d.tall & 0x00FF;
				QSetRect(&g_roomObjectRects[i], 0, 0, wide * 4, tall * 4);
				QOffsetRect(&g_roomObjectRects[i],
						g_thisRoom->objects[i].data.d.topLeft.h,
						g_thisRoom->objects[i].data.d.topLeft.v);
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
				g_roomObjectRects[i] = g_srcRects[g_thisRoom->objects[i].what];
				ZeroRectCorner(&g_roomObjectRects[i]);
				QOffsetRect(&g_roomObjectRects[i],
						g_thisRoom->objects[i].data.e.topLeft.h,
						g_thisRoom->objects[i].data.e.topLeft.v);
				break;

				case kCeilingLight:
				case kLightBulb:
				case kTableLamp:
				case kHipLamp:
				case kDecoLamp:
				case kInvisLight:
				g_roomObjectRects[i] = g_srcRects[g_thisRoom->objects[i].what];
				ZeroRectCorner(&g_roomObjectRects[i]);
				QOffsetRect(&g_roomObjectRects[i],
						g_thisRoom->objects[i].data.f.topLeft.h,
						g_thisRoom->objects[i].data.f.topLeft.v);
				break;

				case kFlourescent:
				case kTrackLight:
				g_roomObjectRects[i] = g_srcRects[g_thisRoom->objects[i].what];
				ZeroRectCorner(&g_roomObjectRects[i]);
				g_roomObjectRects[i].right = g_thisRoom->objects[i].data.f.length;
				QOffsetRect(&g_roomObjectRects[i],
						g_thisRoom->objects[i].data.f.topLeft.h,
						g_thisRoom->objects[i].data.f.topLeft.v);
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
				g_roomObjectRects[i] = g_srcRects[g_thisRoom->objects[i].what];
				ZeroRectCorner(&g_roomObjectRects[i]);
				QOffsetRect(&g_roomObjectRects[i],
						g_thisRoom->objects[i].data.g.topLeft.h,
						g_thisRoom->objects[i].data.g.topLeft.v);
				break;

				case kCustomPict:
				thePict = Gp_LoadImage(g_theHouseFile, g_thisRoom->objects[i].data.g.height);
				if (thePict == NULL)
				{
					g_thisRoom->objects[i].data.g.height = kCustomPictFallbackID;
					g_roomObjectRects[i] = g_srcRects[g_thisRoom->objects[i].what];
				}
				else
				{
					GetObject(thePict, sizeof(bmInfo), &bmInfo);
					QSetRect(&g_roomObjectRects[i], 0, 0,
							(SInt16)bmInfo.bmWidth, (SInt16)bmInfo.bmHeight);
					DeleteObject(thePict);
				}
				ZeroRectCorner(&g_roomObjectRects[i]);
				QOffsetRect(&g_roomObjectRects[i],
						g_thisRoom->objects[i].data.g.topLeft.h,
						g_thisRoom->objects[i].data.g.topLeft.v);
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
				g_roomObjectRects[i] = g_srcRects[g_thisRoom->objects[i].what];
				ZeroRectCorner(&g_roomObjectRects[i]);
				QOffsetRect(&g_roomObjectRects[i],
						g_thisRoom->objects[i].data.h.topLeft.h,
						g_thisRoom->objects[i].data.h.topLeft.v);
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
				g_roomObjectRects[i] = g_thisRoom->objects[i].data.i.bounds;
				break;

				default:
				QSetRect(&g_roomObjectRects[i], -2, -2, -1, -1);
				break;

			}
		}
	}
}

//--------------------------------------------------------------  DrawThisRoomsObjects

void DrawThisRoomsObjects (void)
{
	Rect tempRect;
	SInt16 i;

	if ((g_noRoomAtAll) || (!g_houseUnlocked))
		return;
	else
	{
		if (GetNumberOfLights(g_thisRoomNumber) <= 0)
		{
			DitherShadowRect(g_backSrcMap, &g_backSrcRect);
		}

		for (i = 0; i < kMaxRoomObs; i++)
		{
			switch (g_thisRoom->objects[i].what)
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
				DrawSimpleBlowers(g_thisRoom->objects[i].what, &g_roomObjectRects[i]);
				break;

				case kTiki:
				DrawTiki(&g_roomObjectRects[i], 0);
				break;

				case kInvisBlower:
				DrawInvisibleBlower(&g_roomObjectRects[i]);
				break;

				case kLiftArea:
				DrawLiftArea(&g_roomObjectRects[i]);
				break;

				case kTable:
				DrawTable(&g_roomObjectRects[i], 0);
				break;

				case kShelf:
				DrawShelf(&g_roomObjectRects[i]);
				break;

				case kCabinet:
				DrawCabinet(&g_roomObjectRects[i]);
				break;

				case kFilingCabinet:
				case kDownStairs:
				case kDoorExRt:
				case kDoorExLf:
				case kWindowExRt:
				case kWindowExLf:
				case kOzma:
				DrawPictObject(g_thisRoom->objects[i].what, &g_roomObjectRects[i]);
				break;

				case kWasteBasket:
				case kMilkCrate:
				DrawSimpleFurniture(g_thisRoom->objects[i].what, &g_roomObjectRects[i]);
				break;

				case kCounter:
				DrawCounter(&g_roomObjectRects[i]);
				break;

				case kDresser:
				DrawDresser(&g_roomObjectRects[i]);
				break;

				case kDeckTable:
				DrawDeckTable(&g_roomObjectRects[i], 0);
				break;

				case kStool:
				DrawStool(&g_roomObjectRects[i], 0);
				break;

				case kInvisObstacle:
				DrawInvisObstacle(&g_roomObjectRects[i]);
				break;

				case kInvisBounce:
				DrawInvisBounce(&g_roomObjectRects[i]);
				break;

				case kRedClock:
				DrawRedClock(&g_roomObjectRects[i]);
				break;

				case kBlueClock:
				DrawBlueClock(&g_roomObjectRects[i]);
				break;

				case kYellowClock:
				DrawYellowClock(&g_roomObjectRects[i]);
				break;

				case kCuckoo:
				DrawCuckoo(&g_roomObjectRects[i]);
				break;

				case kPaper:
				case kBattery:
				case kBands:
				case kStar:
				case kSparkle:
				case kHelium:
				DrawSimplePrizes(g_thisRoom->objects[i].what, &g_roomObjectRects[i]);
				break;

				case kGreaseRt:
				tempRect = g_roomObjectRects[i];
				if (!g_thisRoom->objects[i].data.c.initial)
					QOffsetRect(&tempRect, -8, 0);
				DrawGreaseRt(&tempRect, g_thisRoom->objects[i].data.c.length,
						g_thisRoom->objects[i].data.c.initial);
				break;

				case kGreaseLf:
				tempRect = g_roomObjectRects[i];
				if (!g_thisRoom->objects[i].data.c.initial)
					QOffsetRect(&tempRect, 8, 0);
				DrawGreaseLf(&tempRect, g_thisRoom->objects[i].data.c.length,
						g_thisRoom->objects[i].data.c.initial);
				break;

				case kFoil:
				DrawFoil(&g_roomObjectRects[i]);
				break;

				case kInvisBonus:
				DrawInvisBonus(&g_roomObjectRects[i]);
				break;

				case kSlider:
				DrawSlider(&g_roomObjectRects[i]);
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
				DrawPictSansWhiteObject(g_thisRoom->objects[i].what, &g_roomObjectRects[i]);
				break;

				case kCustomPict:
				DrawCustPictSansWhite(g_thisRoom->objects[i].data.g.height,
						&g_roomObjectRects[i]);
				break;

				case kMailboxLf:
				DrawMailboxLeft(&g_roomObjectRects[i], 0);
				break;

				case kMailboxRt:
				DrawMailboxRight(&g_roomObjectRects[i], 0);
				break;

				case kFloorTrans:
				case kCeilingTrans:
				DrawSimpleTransport(g_thisRoom->objects[i].what, &g_roomObjectRects[i]);
				break;

				case kInvisTrans:
				case kDeluxeTrans:
				DrawInvisTransport(&g_roomObjectRects[i]);
				break;

				case kLightSwitch:
				DrawLightSwitch(&g_roomObjectRects[i], true);
				break;

				case kMachineSwitch:
				DrawMachineSwitch(&g_roomObjectRects[i], true);
				break;

				case kThermostat:
				DrawThermostat(&g_roomObjectRects[i], true);
				break;

				case kPowerSwitch:
				DrawPowerSwitch(&g_roomObjectRects[i], true);
				break;

				case kKnifeSwitch:
				DrawKnifeSwitch(&g_roomObjectRects[i], true);
				break;

				case kInvisSwitch:
				DrawInvisibleSwitch(&g_roomObjectRects[i]);
				break;

				case kTrigger:
				case kLgTrigger:
				DrawTrigger(&g_roomObjectRects[i]);
				break;

				case kSoundTrigger:
				DrawSoundTrigger(&g_roomObjectRects[i]);
				break;

				case kCeilingLight:
				case kLightBulb:
				case kTableLamp:
				DrawSimpleLight(g_thisRoom->objects[i].what, &g_roomObjectRects[i]);
				break;

				case kFlourescent:
				DrawFlourescent(&g_roomObjectRects[i]);
				break;

				case kTrackLight:
				DrawTrackLight(&g_roomObjectRects[i]);
				break;

				case kInvisLight:
				DrawInvisLight(&g_roomObjectRects[i]);
				break;

				case kShredder:
				case kToaster:
				case kCDs:
				DrawSimpleAppliance(g_thisRoom->objects[i].what, &g_roomObjectRects[i]);
				break;

				case kMacPlus:
				DrawMacPlus(&g_roomObjectRects[i],
						g_thisRoom->objects[i].data.g.initial, true);
				break;

				case kTV:
				DrawTV(&g_roomObjectRects[i],
						g_thisRoom->objects[i].data.g.initial, true);
				break;

				case kCoffee:
				DrawCoffee(&g_roomObjectRects[i],
						g_thisRoom->objects[i].data.g.initial, true);
				break;

				case kOutlet:
				DrawOutlet(&g_roomObjectRects[i]);
				break;

				case kVCR:
				DrawVCR(&g_roomObjectRects[i],
						g_thisRoom->objects[i].data.g.initial, true);
				break;

				case kStereo:
				DrawStereo(&g_roomObjectRects[i], g_isPlayMusicGame, true);
				break;

				case kMicrowave:
				DrawMicrowave(&g_roomObjectRects[i],
						g_thisRoom->objects[i].data.g.initial, true);
				break;

				case kBalloon:
				DrawBalloon(&g_roomObjectRects[i]);
				break;

				case kCopterLf:
				case kCopterRt:
				DrawCopter(&g_roomObjectRects[i]);
				break;

				case kDartLf:
				case kDartRt:
				DrawDart(&g_roomObjectRects[i], g_thisRoom->objects[i].what);
				break;

				case kBall:
				DrawBall(g_thisRoom->objects[i].what, &g_roomObjectRects[i]);
				break;

				case kDrip:
				DrawDrip(&g_roomObjectRects[i]);
				break;

				case kFish:
				DrawFish(g_thisRoom->objects[i].what, &g_roomObjectRects[i]);
				break;

				case kCobweb:
				case kCloud:
				DrawPictWithMaskObject(g_thisRoom->objects[i].what, &g_roomObjectRects[i]);
				break;

				case kMirror:
				DrawMirror(&g_roomObjectRects[i]);
				break;

				case kMousehole:
				case kFaucet:
				DrawSimpleClutter(g_thisRoom->objects[i].what, &g_roomObjectRects[i]);
				break;

				case kFlower:
				DrawFlower(&g_roomObjectRects[i], g_thisRoom->objects[i].data.i.pict);
				break;

				case kWallWindow:
				DrawWallWindow(&g_roomObjectRects[i]);
				break;

				case kCalendar:
				DrawCalendar(&g_roomObjectRects[i]);
				break;

				case kBulletin:
				DrawBulletin(&g_roomObjectRects[i]);
				break;

				default:
				break;
			}
		}
	}

	if (g_isFirstRoom)
	{
		Mac_CopyMask(g_glidSrcMap, g_glidMaskMap, g_backSrcMap,
				&g_gliderSrc[0], &g_gliderSrc[0], &g_initialGliderRect);
	}

	Mac_CopyMask(g_blowerSrcMap, g_blowerMaskMap, g_backSrcMap,
			&g_leftStartGliderSrc, &g_leftStartGliderSrc, &g_leftStartGliderDest);

	Mac_CopyMask(g_blowerSrcMap, g_blowerMaskMap, g_backSrcMap,
			&g_rightStartGliderSrc, &g_rightStartGliderSrc, &g_rightStartGliderDest);

	Mac_CopyBits(g_backSrcMap, g_workSrcMap,
			&g_backSrcRect, &g_backSrcRect, srcCopy, nil);
}

//--------------------------------------------------------------  HiliteAllObjects

void HiliteAllObjects (void)
{
	HDC mainWindowDC;
	RECT focusRects[ARRAYSIZE(g_roomObjectRects)];
	MSG msg;
	SInt16 i;

	if (COMPILEDEMO)
		return;

	if (g_theMode != kEditMode)
		return;

	for (i = 0; i < kMaxRoomObs; i++)
	{
		focusRects[i].left = g_roomObjectRects[i].left;
		focusRects[i].top = g_roomObjectRects[i].top;
		focusRects[i].right = g_roomObjectRects[i].right;
		focusRects[i].bottom = g_roomObjectRects[i].bottom;
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

		if (g_mainWindow != NULL && GetActiveWindow() != g_mainWindow)
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
}

//--------------------------------------------------------------  GoToObjectInRoom

void GoToObjectInRoom (SInt16 object, SInt16 floor, SInt16 suite)
{
	SInt16 itsNumber;

	if (COMPILEDEMO)
		return;

	if (RoomExists(suite, floor, &itsNumber))
	{
		if (itsNumber != g_thisRoomNumber)
		{
			CopyRoomToThisRoom(itsNumber);
			DeselectObject();
			ReflectCurrentRoom(false);
		}
		else
			DeselectObject();

		if (g_thisRoom->objects[object].what != kObjectIsEmpty)
		{
			g_objActive = object;
			StartMarqueeForActiveObject();
			UpdateMenus(false);
		}
	}
}

//--------------------------------------------------------------  GoToObjectInRoomNum

void GoToObjectInRoomNum (SInt16 object, SInt16 roomNum)
{
	SInt16 floor, suite;

	if (GetRoomFloorSuite(roomNum, &floor, &suite))
		GoToObjectInRoom(object, floor, suite);
}

//--------------------------------------------------------------  StartMarqueeForActiveObject

void StartMarqueeForActiveObject (void)
{
	SInt16 direction;
	SInt16 dist;

	if (ObjectHasHandle(&direction, &dist))
	{
		StartMarqueeHandled(&g_roomObjectRects[g_objActive], direction, dist);
		HandleBlowerGlider();
	}
	else if (g_objActive == kInitialGliderSelected)
	{
		StartMarquee(&g_initialGliderRect);
	}
	else if (g_objActive == kLeftGliderSelected)
	{
		StartMarquee(&g_leftStartGliderDest);
	}
	else if (g_objActive == kRightGliderSelected)
	{
		StartMarquee(&g_rightStartGliderDest);
	}
	else if (g_objActive >= 0 && g_objActive < kMaxRoomObs)
	{
		StartMarquee(&g_roomObjectRects[g_objActive]);
	}
	else
	{
		StopMarquee();
	}
}
