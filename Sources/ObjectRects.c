//============================================================================
//----------------------------------------------------------------------------
//                               ObjectRects.c
//----------------------------------------------------------------------------
//============================================================================

#include "ObjectRects.h"

#include "DynamicMaps.h"
#include "GliderDefines.h"
#include "House.h"
#include "HouseIO.h"
#include "MainWindow.h"
#include "Objects.h"
#include "RectUtils.h"
#include "ResourceIDs.h"
#include "ResourceLoader.h"
#include "Room.h"
#include "Sound.h"
#include "Utilities.h"

#define kFloorColumnWide        4
#define kCeilingColumnWide      24
#define kFanColumnThick         16
#define kFanColumnDown          20
#define kDeadlyFlameHeight      24
#define kStoolThick             25
#define kShredderActiveHigh     40

SInt16 AddActiveRect (const Rect *bounds, SInt16 action, SInt16 who, Boolean isOn,
	Boolean doScrutinize);

//==============================================================  Functions
//--------------------------------------------------------------  GetObjectRect

void GetObjectRect (objectPtr who, Rect *itsRect)
{
	HBITMAP thePict;
	SInt16 wide, tall;

	switch (who->what)
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
		*itsRect = g_srcRects[who->what];
		ZeroRectCorner(itsRect);
		QOffsetRect(itsRect, who->data.a.topLeft.h, who->data.a.topLeft.v);
		break;

		case kLiftArea:
		QSetRect(itsRect, 0, 0, who->data.a.distance, who->data.a.tall * 2);
		QOffsetRect(itsRect, who->data.a.topLeft.h, who->data.a.topLeft.v);
		break;

		case kTable:
		case kShelf:
		case kCabinet:
		case kFilingCabinet:
		case kWasteBasket:
		case kMilkCrate:
		case kCounter:
		case kDresser:
		case kStool:
		case kTrunk:
		case kDeckTable:
		case kInvisObstacle:
		case kManhole:
		case kBooks:
		case kInvisBounce:
		*itsRect = who->data.b.bounds;
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
		*itsRect = g_srcRects[who->what];
		ZeroRectCorner(itsRect);
		QOffsetRect(itsRect,
				who->data.c.topLeft.h,
				who->data.c.topLeft.v);
		break;

		case kSlider:
		*itsRect = g_srcRects[who->what];
		ZeroRectCorner(itsRect);
		QOffsetRect(itsRect,
				who->data.c.topLeft.h,
				who->data.c.topLeft.v);
		itsRect->right = itsRect->left + who->data.c.length;
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
		*itsRect = g_srcRects[who->what];
		ZeroRectCorner(itsRect);
		QOffsetRect(itsRect,
				who->data.d.topLeft.h,
				who->data.d.topLeft.v);
		break;

		case kInvisTrans:
		*itsRect = g_srcRects[who->what];
		ZeroRectCorner(itsRect);
		QOffsetRect(itsRect,
				who->data.d.topLeft.h,
				who->data.d.topLeft.v);
		itsRect->bottom = itsRect->top + who->data.d.tall;
		itsRect->right += (SInt16)who->data.d.wide;
		break;

		case kDeluxeTrans:
		wide = (who->data.d.tall & 0xFF00) >> 8;  // Get high byte
		tall = who->data.d.tall & 0x00FF;  // Get low byte
		QSetRect(itsRect, 0, 0, wide * 4, tall * 4);  // Scale by 4
		QOffsetRect(itsRect,
				who->data.d.topLeft.h,
				who->data.d.topLeft.v);
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
		*itsRect = g_srcRects[who->what];
		ZeroRectCorner(itsRect);
		QOffsetRect(itsRect,
				who->data.e.topLeft.h,
				who->data.e.topLeft.v);
		break;

		case kCeilingLight:
		case kLightBulb:
		case kTableLamp:
		case kHipLamp:
		case kDecoLamp:
		case kInvisLight:
		*itsRect = g_srcRects[who->what];
		ZeroRectCorner(itsRect);
		QOffsetRect(itsRect,
				who->data.f.topLeft.h,
				who->data.f.topLeft.v);
		break;

		case kFlourescent:
		case kTrackLight:
		*itsRect = g_srcRects[who->what];
		ZeroRectCorner(itsRect);
		itsRect->right = who->data.f.length;
		QOffsetRect(itsRect,
				who->data.f.topLeft.h,
				who->data.f.topLeft.v);
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
		*itsRect = g_srcRects[who->what];
		ZeroRectCorner(itsRect);
		QOffsetRect(itsRect,
				who->data.g.topLeft.h,
				who->data.g.topLeft.v);
		break;

		case kCustomPict:
		thePict = Gp_LoadImage(g_theHouseFile, who->data.g.height);
		if (thePict == NULL)
		{
			who->data.g.height = kCustomPictFallbackID;
			*itsRect = g_srcRects[who->what];
		}
		else
		{
			GetGraphicRect(thePict, itsRect);
			DeleteObject(thePict);
		}
		ZeroRectCorner(itsRect);
		QOffsetRect(itsRect,
				who->data.g.topLeft.h,
				who->data.g.topLeft.v);
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
		*itsRect = g_srcRects[who->what];
		ZeroRectCorner(itsRect);
		QOffsetRect(itsRect,
				who->data.h.topLeft.h,
				who->data.h.topLeft.v);
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
		*itsRect = who->data.i.bounds;
		break;

		case kObjectIsEmpty:
		default:
		QSetRect(itsRect, 0, 0, 0, 0);
		break;
	}
}

//--------------------------------------------------------------  AddActiveRect

SInt16 AddActiveRect (const Rect *bounds, SInt16 action, SInt16 who, Boolean isOn,
	Boolean doScrutinize)
{
	if (g_nHotSpots >= kMaxHotSpots)
		return (-1);

	g_hotSpots[g_nHotSpots].bounds = *bounds;  // the active rect
	g_hotSpots[g_nHotSpots].action = action;  // what it does
	g_hotSpots[g_nHotSpots].who = who;  // local obj. linked to
	g_hotSpots[g_nHotSpots].isOn = isOn;  // is it active?
	g_hotSpots[g_nHotSpots].stillOver = false;
	g_hotSpots[g_nHotSpots].doScrutinize = doScrutinize;
	g_nHotSpots++;

	return (g_nHotSpots - 1);
}

//--------------------------------------------------------------  CreateActiveRects

SInt16 CreateActiveRects (SInt16 who)
{
	objectType theObject;
	Rect bounds;
	SInt16 hotSpotNumber, wide, tall;
	Boolean isOn;

	// NOTE: CreateActiveRects is called in the process of adding an entry to the
	// `masterObjects` table, so checking `who >= g_numMasterObjects` is incorrect.
	// Just make sure that the index isn't completely out of bounds.
	if (who < 0 || who >= kMaxMasterObjects)
		return (-1);

	hotSpotNumber = -1;
	theObject = g_masterObjects[who].theObject;

	switch (theObject.what)
	{
		case kObjectIsEmpty:
		break;

		case kFloorVent:
		QSetRect(&bounds, 0, -theObject.data.a.distance, kFloorColumnWide, 0);
		QOffsetRect(&bounds,
				HalfRectWide(&g_srcRects[kFloorVent]) - kFloorColumnWide / 2, 0);
		QOffsetRect(&bounds, theObject.data.a.topLeft.h,
				theObject.data.a.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kLiftIt, who, theObject.data.a.state,
				false);
		break;

		case kCeilingVent:
		QSetRect(&bounds, 0, 0, kCeilingColumnWide, theObject.data.a.distance);
		QOffsetRect(&bounds,
				HalfRectWide(&g_srcRects[kCeilingVent]) - kCeilingColumnWide / 2,
				0);
		QOffsetRect(&bounds,
				theObject.data.a.topLeft.h,
				theObject.data.a.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kDropIt, who, theObject.data.a.state,
				false);
		break;

		case kFloorBlower:
		QSetRect(&bounds, 0, -theObject.data.a.distance, kFloorColumnWide, 0);
		QOffsetRect(&bounds,
				HalfRectWide(&g_srcRects[kFloorBlower]) - kFloorColumnWide / 2,
				0);
		QOffsetRect(&bounds,
				theObject.data.a.topLeft.h,
				theObject.data.a.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kLiftIt, who, theObject.data.a.state,
				false);
		break;

		case kCeilingBlower:
		QSetRect(&bounds, 0, 0, kCeilingColumnWide, theObject.data.a.distance);
		QOffsetRect(&bounds,
				HalfRectWide(&g_srcRects[kCeilingBlower]) - kCeilingColumnWide / 2,
				0);
		QOffsetRect(&bounds,
				theObject.data.a.topLeft.h,
				theObject.data.a.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kDropIt, who, theObject.data.a.state,
				false);
		break;

		case kSewerGrate:
		QSetRect(&bounds, 0, -theObject.data.a.distance, kFloorColumnWide, 0);
		QOffsetRect(&bounds,
				HalfRectWide(&g_srcRects[kSewerGrate]) - kFloorColumnWide / 2,
				0);
		QOffsetRect(&bounds,
				theObject.data.a.topLeft.h,
				theObject.data.a.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kLiftIt, who, theObject.data.a.state,
				false);
		break;

		case kLeftFan:
		QSetRect(&bounds, 0, 0, 13, 43);
		QOffsetRect(&bounds,
				theObject.data.a.topLeft.h + 16,
				theObject.data.a.topLeft.v + 12);
		hotSpotNumber = AddActiveRect(&bounds, kDissolveIt, who, true, true);
		QSetRect(&bounds, 0, 0, theObject.data.a.distance, kFanColumnThick);
		QOffsetRect(&bounds, -(theObject.data.a.distance), kFanColumnDown);
		QOffsetRect(&bounds,
				theObject.data.a.topLeft.h,
				theObject.data.a.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kPushItLeft, who,
				theObject.data.a.state, false);
		break;

		case kRightFan:
		QSetRect(&bounds, 0, 0, 13, 43);
		QOffsetRect(&bounds,
				theObject.data.a.topLeft.h + 6,
				theObject.data.a.topLeft.v + 12);
		hotSpotNumber = AddActiveRect(&bounds, kDissolveIt, who, true, true);
		QSetRect(&bounds, 0, 0, theObject.data.a.distance, kFanColumnThick);
		QOffsetRect(&bounds, RectWide(&g_srcRects[kRightFan]), kFanColumnDown);
		QOffsetRect(&bounds,
				theObject.data.a.topLeft.h,
				theObject.data.a.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kPushItRight, who,
				theObject.data.a.state, false);
		break;

		case kTaper:
		QSetRect(&bounds, 0, -theObject.data.a.distance, kFloorColumnWide, 0);
		QOffsetRect(&bounds,
				HalfRectWide(&g_srcRects[kTaper]) - kFloorColumnWide / 2,
				0);
		QOffsetRect(&bounds,
				theObject.data.a.topLeft.h,
				theObject.data.a.topLeft.v);
		if ((bounds.bottom - bounds.top) > kDeadlyFlameHeight)
		{
			bounds.bottom -= kDeadlyFlameHeight;
			hotSpotNumber = AddActiveRect(&bounds, kLiftIt, who, true, false);
			bounds.bottom += kDeadlyFlameHeight;
			bounds.top = bounds.bottom - kDeadlyFlameHeight + 2;
			hotSpotNumber = AddActiveRect(&bounds, kBurnIt, who, true, false);
		}
		else
			hotSpotNumber = AddActiveRect(&bounds, kBurnIt, who, true, false);
		QSetRect(&bounds, 0, 0, 7, 48);
		QOffsetRect(&bounds,
				theObject.data.a.topLeft.h + 6,
				theObject.data.a.topLeft.v + 11);
		hotSpotNumber = AddActiveRect(&bounds, kDissolveIt, who, true, true);
		break;

		case kCandle:
		QSetRect(&bounds, 0, -theObject.data.a.distance, kFloorColumnWide, 0);
		QOffsetRect(&bounds,
				HalfRectWide(&g_srcRects[kCandle]) - kFloorColumnWide / 2,
				0);
		QOffsetRect(&bounds,
				theObject.data.a.topLeft.h - 2,
				theObject.data.a.topLeft.v);
		if ((bounds.bottom - bounds.top) > kDeadlyFlameHeight)
		{
			bounds.bottom -= kDeadlyFlameHeight;
			hotSpotNumber = AddActiveRect(&bounds, kLiftIt, who, true, false);
			bounds.bottom += kDeadlyFlameHeight;
			bounds.top = bounds.bottom - kDeadlyFlameHeight + 2;
			hotSpotNumber = AddActiveRect(&bounds, kBurnIt, who, true, false);
		}
		else
			hotSpotNumber = AddActiveRect(&bounds, kBurnIt, who, true, false);
		QSetRect(&bounds, 0, 0, 8, 20);
		QOffsetRect(&bounds,
				theObject.data.a.topLeft.h + 9,
				theObject.data.a.topLeft.v + 11);
		hotSpotNumber = AddActiveRect(&bounds, kDissolveIt, who, true, true);
		break;

		case kStubby:
		QSetRect(&bounds, 0, -theObject.data.a.distance, kFloorColumnWide, 0);
		QOffsetRect(&bounds,
				(HalfRectWide(&g_srcRects[kStubby]) - kFloorColumnWide / 2) - 1,
				0);
		QOffsetRect(&bounds,
				theObject.data.a.topLeft.h,
				theObject.data.a.topLeft.v);
		if ((bounds.bottom - bounds.top) > kDeadlyFlameHeight)
		{
			bounds.bottom -= kDeadlyFlameHeight;
			hotSpotNumber = AddActiveRect(&bounds, kLiftIt, who, true, false);
			bounds.bottom += kDeadlyFlameHeight;
			bounds.top = bounds.bottom - kDeadlyFlameHeight + 2;
			hotSpotNumber = AddActiveRect(&bounds, kBurnIt, who, true, false);
		}
		else
			hotSpotNumber = AddActiveRect(&bounds, kBurnIt, who, true, false);
		QSetRect(&bounds, 0, 0, 15, 26);
		QOffsetRect(&bounds,
				theObject.data.a.topLeft.h + 1,
				theObject.data.a.topLeft.v + 11);
		hotSpotNumber = AddActiveRect(&bounds, kDissolveIt, who, true, true);
		break;

		case kTiki:
		QSetRect(&bounds, 0, -theObject.data.a.distance, kFloorColumnWide, 0);
		QOffsetRect(&bounds,
				HalfRectWide(&g_srcRects[kTiki]) - kFloorColumnWide / 2,
				0);
		QOffsetRect(&bounds,
				theObject.data.a.topLeft.h,
				theObject.data.a.topLeft.v);
		if ((bounds.bottom - bounds.top) > kDeadlyFlameHeight)
		{
			bounds.bottom -= kDeadlyFlameHeight;
			hotSpotNumber = AddActiveRect(&bounds, kLiftIt, who, true, false);
			bounds.bottom += kDeadlyFlameHeight;
			bounds.top = bounds.bottom - kDeadlyFlameHeight + 2;
			hotSpotNumber = AddActiveRect(&bounds, kBurnIt, who, true, false);
		}
		else
			hotSpotNumber = AddActiveRect(&bounds, kBurnIt, who, true, false);
		QSetRect(&bounds, 0, 0, 15, 14);
		QOffsetRect(&bounds,
				theObject.data.a.topLeft.h + 6,
				theObject.data.a.topLeft.v + 6);
		hotSpotNumber = AddActiveRect(&bounds, kDissolveIt, who, true, true);
		break;

		case kBBQ:
		QSetRect(&bounds, 0, -theObject.data.a.distance, kFloorColumnWide, 8);
		QOffsetRect(&bounds,
				HalfRectWide(&g_srcRects[kBBQ]) - kFloorColumnWide / 2, 0);
		QOffsetRect(&bounds, theObject.data.a.topLeft.h,
				theObject.data.a.topLeft.v);
		if ((bounds.bottom - bounds.top) > kDeadlyFlameHeight)
		{
			bounds.bottom -= kDeadlyFlameHeight;
			hotSpotNumber = AddActiveRect(&bounds, kLiftIt, who, true, false);
			bounds.bottom += kDeadlyFlameHeight;
			bounds.top = bounds.bottom - kDeadlyFlameHeight + 2;
			hotSpotNumber = AddActiveRect(&bounds, kBurnIt, who, true, false);
		}
		else
			hotSpotNumber = AddActiveRect(&bounds, kBurnIt, who, true, false);
		QSetRect(&bounds, 0, 0, 52, 17);
		QOffsetRect(&bounds,
				theObject.data.a.topLeft.h + 6,
				theObject.data.a.topLeft.v + 8);
		hotSpotNumber = AddActiveRect(&bounds, kDissolveIt, who, true, true);
		break;

		case kInvisBlower:
		switch (theObject.data.a.vector & 0x0F)
		{
			case 1:  // up
			QSetRect(&bounds, 0, -theObject.data.a.distance - 24,
					kFloorColumnWide, 0);
			QOffsetRect(&bounds, 12 - kFloorColumnWide / 2, 24);
			QOffsetRect(&bounds, theObject.data.a.topLeft.h,
					theObject.data.a.topLeft.v);
			hotSpotNumber = AddActiveRect(&bounds, kLiftIt, who, theObject.data.a.state,
					false);
			break;

			case 2:  // right
			QSetRect(&bounds, 0, 0, theObject.data.a.distance + 24,
					kFanColumnThick);
			QOffsetRect(&bounds, 0, 12 - kFanColumnThick / 2);
			QOffsetRect(&bounds, theObject.data.a.topLeft.h,
					theObject.data.a.topLeft.v);
			hotSpotNumber = AddActiveRect(&bounds, kPushItRight, who,
					theObject.data.a.state, false);
			break;

			case 4:  // down
			QSetRect(&bounds, 0, 0, kFloorColumnWide,
					theObject.data.a.distance + 24);
			QOffsetRect(&bounds, 12 - kFloorColumnWide / 2, 0);
			QOffsetRect(&bounds, theObject.data.a.topLeft.h,
					theObject.data.a.topLeft.v);
			hotSpotNumber = AddActiveRect(&bounds, kDropIt, who,
					theObject.data.a.state, false);
			break;

			case 8:  // left
			QSetRect(&bounds, 0, 0, theObject.data.a.distance + 24, kFanColumnThick);
			QOffsetRect(&bounds, -(theObject.data.a.distance), 12 - kFanColumnThick / 2);
			QOffsetRect(&bounds, theObject.data.a.topLeft.h,
					theObject.data.a.topLeft.v);
			hotSpotNumber = AddActiveRect(&bounds, kPushItLeft, who,
					theObject.data.a.state, false);
			break;
		}
		break;

		case kGrecoVent:
		QSetRect(&bounds, 0, -theObject.data.a.distance, kFloorColumnWide, 0);
		QOffsetRect(&bounds,
				HalfRectWide(&g_srcRects[kGrecoVent]) - kFloorColumnWide / 2,
				0);
		QOffsetRect(&bounds,
				theObject.data.a.topLeft.h,
				theObject.data.a.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kLiftIt, who,
				theObject.data.a.state, false);
		break;

		case kSewerBlower:
		QSetRect(&bounds, 0, -theObject.data.a.distance, kFloorColumnWide, 0);
		QOffsetRect(&bounds,
				HalfRectWide(&g_srcRects[kSewerBlower]) - kFloorColumnWide / 2,
				0);
		QOffsetRect(&bounds,
				theObject.data.a.topLeft.h,
				theObject.data.a.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kLiftIt, who,
				theObject.data.a.state, false);
		break;

		case kLiftArea:
		QSetRect(&bounds, 0, 0, theObject.data.a.distance, theObject.data.a.tall * 2);
		QOffsetRect(&bounds,
				theObject.data.a.topLeft.h,
				theObject.data.a.topLeft.v);
		switch (theObject.data.a.vector & 0x0F)
		{
			case 1:  // up
			hotSpotNumber = AddActiveRect(&bounds, kLiftIt, who, theObject.data.a.state,
					false);
			break;

			case 2:  // right
			hotSpotNumber = AddActiveRect(&bounds, kPushItRight, who,
					theObject.data.a.state, false);
			break;

			case 4:  // down
			hotSpotNumber = AddActiveRect(&bounds, kDropIt, who,
					theObject.data.a.state, false);
			break;

			case 8:  // left
			hotSpotNumber = AddActiveRect(&bounds, kPushItLeft, who,
					theObject.data.a.state, false);
			break;
		}
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
		case kTrunk:
		case kInvisObstacle:
		bounds = theObject.data.b.bounds;
		hotSpotNumber = AddActiveRect(&bounds, kDissolveIt, who, true, true);
		break;

		case kBooks:
		bounds = theObject.data.b.bounds;
		bounds.right -= 2;
		hotSpotNumber = AddActiveRect(&bounds, kDissolveIt, who, true, true);
		break;

		case kManhole:
		bounds = theObject.data.b.bounds;
		bounds.left += kGliderWide + 3;
		bounds.right -= kGliderWide + 3;
		bounds.top = kFloorLimit - 1;
		bounds.bottom = kTileHigh;
		hotSpotNumber = AddActiveRect(&bounds, kIgnoreGround, who, true, false);
		break;

		case kInvisBounce:
		bounds = theObject.data.b.bounds;
		hotSpotNumber = AddActiveRect(&bounds, kBounceIt, who, true, true);
		break;

		case kStool:
		bounds = theObject.data.b.bounds;
		QInsetRect(&bounds, 1, 1);
		bounds.bottom = bounds.top + kStoolThick;
		hotSpotNumber = AddActiveRect(&bounds, kDissolveIt, who, true, true);
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
		case kHelium:
		bounds = g_srcRects[theObject.what];
		ZeroRectCorner(&bounds);
		QOffsetRect(&bounds,
				theObject.data.c.topLeft.h,
				theObject.data.c.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kRewardIt, who,
				theObject.data.c.state, false);
		break;

		case kGreaseRt:
		if (theObject.data.c.state)
		{
			bounds = g_srcRects[theObject.what];
			ZeroRectCorner(&bounds);
			QOffsetRect(&bounds, theObject.data.c.topLeft.h,
					theObject.data.c.topLeft.v);
			hotSpotNumber = AddActiveRect(&bounds, kRewardIt, who, true, false);
		}
		else
		{
			QSetRect(&bounds, 0, -2, theObject.data.c.length - 5, 0);
			QOffsetRect(&bounds, 32 - 1, 27);
			QOffsetRect(&bounds, theObject.data.c.topLeft.h,
					theObject.data.c.topLeft.v);
			hotSpotNumber = AddActiveRect(&bounds, kSlideIt, who, true, false);
		}
		break;

		case kGreaseLf:
		if (theObject.data.c.state)
		{
			bounds = g_srcRects[theObject.what];
			ZeroRectCorner(&bounds);
			QOffsetRect(&bounds, theObject.data.c.topLeft.h,
					theObject.data.c.topLeft.v);
			hotSpotNumber = AddActiveRect(&bounds, kRewardIt, who, true, false);
		}
		else
		{
			QSetRect(&bounds, -theObject.data.c.length + 5, -2, 0, 0);
			QOffsetRect(&bounds, 1, 27);
			QOffsetRect(&bounds, theObject.data.c.topLeft.h,
					theObject.data.c.topLeft.v);
			hotSpotNumber = AddActiveRect(&bounds, kSlideIt, who, true, false);
		}
		break;

		case kSparkle:
		bounds = g_srcRects[theObject.what];
		ZeroRectCorner(&bounds);
		QOffsetRect(&bounds,
				theObject.data.c.topLeft.h,
				theObject.data.c.topLeft.v);
		break;

		case kSlider:
		QSetRect(&bounds, 0, 0, theObject.data.c.length, 16);
		QOffsetRect(&bounds, theObject.data.c.topLeft.h,
				theObject.data.c.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kSlideIt, who, true, false);
		break;

		case kUpStairs:
		QSetRect(&bounds, 0, 0, 112, 32);
		QOffsetRect(&bounds,
				theObject.data.d.topLeft.h,
				theObject.data.d.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kMoveItUp, who, true, false);
		break;

		case kDownStairs:
		QSetRect(&bounds, -80, -56, 0, 0);
		QOffsetRect(&bounds, g_srcRects[kDownStairs].right, 170);
		QOffsetRect(&bounds,
				theObject.data.d.topLeft.h,
				theObject.data.d.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kMoveItDown, who, true, false);
		break;

		case kMailboxLf:
		if (theObject.data.d.who != 255)
		{
			QSetRect(&bounds, -72, 0, 0, 40);
			QOffsetRect(&bounds, 30, 16);
			QOffsetRect(&bounds,
					theObject.data.d.topLeft.h,
					theObject.data.d.topLeft.v);
			hotSpotNumber = AddActiveRect(&bounds, kMailItLeft, who, true, false);
		}
		break;

		case kMailboxRt:
		if (theObject.data.d.who != 255)
		{
			QSetRect(&bounds, 0, 0, 72, 40);
			QOffsetRect(&bounds, 79, 16);
			QOffsetRect(&bounds,
					theObject.data.d.topLeft.h,
					theObject.data.d.topLeft.v);
			hotSpotNumber = AddActiveRect(&bounds, kMailItRight, who, true, false);
		}
		break;

		case kFloorTrans:
		if (theObject.data.d.who != 255)
		{
			QSetRect(&bounds, 0, -48, 76, 0);
			QOffsetRect(&bounds, -8, RectTall(&g_srcRects[kFloorTrans]));
			QOffsetRect(&bounds,
					theObject.data.d.topLeft.h,
					theObject.data.d.topLeft.v);
			hotSpotNumber = AddActiveRect(&bounds, kDuctItDown, who, true, false);
		}
		break;

		case kCeilingTrans:
		if (theObject.data.d.who != 255)
		{
			QSetRect(&bounds, 0, 0, 76, 48);
			QOffsetRect(&bounds, -8, 0);
			QOffsetRect(&bounds,
					theObject.data.d.topLeft.h,
					theObject.data.d.topLeft.v);
			hotSpotNumber = AddActiveRect(&bounds, kDuctItUp, who, true, false);
		}
		break;

		case kDoorInLf:
		QSetRect(&bounds, 0, 0, 16, 240);
		QOffsetRect(&bounds, 0, 52);
		QOffsetRect(&bounds,
				theObject.data.d.topLeft.h,
				theObject.data.d.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kIgnoreLeftWall, who, true, false);
		break;

		case kDoorInRt:
		QSetRect(&bounds, 0, 0, 16, 240);
		QOffsetRect(&bounds, 128, 52);
		QOffsetRect(&bounds,
				theObject.data.d.topLeft.h,
				theObject.data.d.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kIgnoreRightWall, who, true, false);
		break;

		case kDoorExRt:
		QSetRect(&bounds, 0, 0, 16, 240);
		QOffsetRect(&bounds, 0, 52);
		QOffsetRect(&bounds,
				theObject.data.d.topLeft.h,
				theObject.data.d.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kIgnoreRightWall, who, true, false);
		break;

		case kDoorExLf:
		QSetRect(&bounds, 0, 0, 16, 240);
		QOffsetRect(&bounds, 0, 52);
		QOffsetRect(&bounds,
				theObject.data.d.topLeft.h,
				theObject.data.d.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kIgnoreLeftWall, who, true, false);
		break;

		case kWindowInLf:
		QSetRect(&bounds, 0, 0, 16, 44);
		QOffsetRect(&bounds, 0, 96);
		QOffsetRect(&bounds,
				theObject.data.d.topLeft.h,
				theObject.data.d.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kIgnoreLeftWall, who, true, false);
		break;

		case kWindowInRt:
		QSetRect(&bounds, 0, 0, 16, 44);
		QOffsetRect(&bounds, 4, 96);
		QOffsetRect(&bounds,
				theObject.data.d.topLeft.h,
				theObject.data.d.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kIgnoreRightWall, who, true, false);
		break;

		case kWindowExRt:
		QSetRect(&bounds, 0, 0, 16, 44);
		QOffsetRect(&bounds, 0, 96);
		QOffsetRect(&bounds,
				theObject.data.d.topLeft.h,
				theObject.data.d.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kIgnoreRightWall, who, true, false);
		break;

		case kWindowExLf:
		QSetRect(&bounds, 0, 0, 16, 44);
		QOffsetRect(&bounds, 0, 96);
		QOffsetRect(&bounds,
				theObject.data.d.topLeft.h,
				theObject.data.d.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kIgnoreLeftWall, who, true, false);
		break;

		case kInvisTrans:
		if (theObject.data.d.who != 255)
		{
			QSetRect(&bounds, 0, 0, 64, 32);
			QOffsetRect(&bounds,
					theObject.data.d.topLeft.h,
					theObject.data.d.topLeft.v);
			bounds.bottom = bounds.top + theObject.data.d.tall;
			bounds.right += (SInt16)theObject.data.d.wide;
			hotSpotNumber = AddActiveRect(&bounds, kTransportIt, who, true, false);
		}
		break;

		case kDeluxeTrans:
		if (theObject.data.d.who != 255)
		{
			wide = (theObject.data.d.tall & 0xFF00) >> 8;  // Get high byte
			tall = theObject.data.d.tall & 0x00FF;  // Get low byte
			QSetRect(&bounds, 0, 0, wide * 4, tall * 4);  // Scale by 4
			QOffsetRect(&bounds,
					theObject.data.d.topLeft.h,
					theObject.data.d.topLeft.v);
			isOn = theObject.data.d.wide & 0x0F;
			hotSpotNumber = AddActiveRect(&bounds, kTransportIt, who, isOn, false);
		}
		break;

		case kLightSwitch:
		case kMachineSwitch:
		case kThermostat:
		case kPowerSwitch:
		case kKnifeSwitch:
		case kInvisSwitch:
		case kTrigger:
		case kLgTrigger:
		bounds = g_srcRects[theObject.what];
		ZeroRectCorner(&bounds);
		QOffsetRect(&bounds,
				theObject.data.e.topLeft.h,
				theObject.data.e.topLeft.v);
		if ((theObject.what == kTrigger) || (theObject.what == kLgTrigger))
		{
			if (theObject.data.e.where != -1)
				hotSpotNumber = AddActiveRect(&bounds, kTriggerIt, who, true, false);
		}
		else
		{
			if (theObject.data.e.where != -1)
				hotSpotNumber = AddActiveRect(&bounds, kSwitchIt, who, true, false);
		}
		break;

		case kSoundTrigger:
		QSetRect(&bounds, 0, 0, 48, 48);
		QOffsetRect(&bounds, theObject.data.e.topLeft.h, theObject.data.e.topLeft.v);
		if (LoadTriggerSound(theObject.data.e.where) == noErr)
			hotSpotNumber = AddActiveRect(&bounds, kSoundIt, who, true, false);
		break;

		case kCeilingLight:
		case kLightBulb:
		case kTableLamp:
		case kHipLamp:
		case kDecoLamp:
		case kFlourescent:
		case kTrackLight:
		case kInvisLight:
		break;

		case kShredder:
		bounds = g_srcRects[theObject.what];
		bounds.bottom = bounds.top + kShredderActiveHigh;
		bounds.right += 48;
		ZeroRectCorner(&bounds);
		QOffsetRect(&bounds, theObject.data.g.topLeft.h,
				theObject.data.g.topLeft.v);
		QOffsetRect(&bounds, -24, -36);
		hotSpotNumber = AddActiveRect(&bounds, kShredIt, who,
				theObject.data.g.state, true);
		break;

		case kGuitar:
		QSetRect(&bounds, 0, 0, 8, 96);
		QOffsetRect(&bounds, theObject.data.g.topLeft.h + 34,
				theObject.data.g.topLeft.v + 32);
		hotSpotNumber = AddActiveRect(&bounds, kStrumIt, who, true, false);
		break;

		case kOutlet:
		bounds = g_srcRects[theObject.what];
		ZeroRectCorner(&bounds);
		QOffsetRect(&bounds,
				theObject.data.g.topLeft.h,
				theObject.data.g.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kIgnoreIt, who,
				theObject.data.g.state, false);
		break;

		case kMicrowave:
		bounds = g_srcRects[theObject.what];
		ZeroRectCorner(&bounds);
		QOffsetRect(&bounds,
				theObject.data.g.topLeft.h,
				theObject.data.g.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kDissolveIt, who, true, true);
		bounds.bottom = bounds.top;
		bounds.top = 0;
		hotSpotNumber = AddActiveRect(&bounds, kMicrowaveIt, who, true, true);
		break;

		case kToaster:
		case kMacPlus:
		case kTV:
		case kCoffee:
		case kVCR:
		case kStereo:
		case kCinderBlock:
		case kFlowerBox:
		case kCDs:
		bounds = g_srcRects[theObject.what];
		ZeroRectCorner(&bounds);
		QOffsetRect(&bounds,
				theObject.data.g.topLeft.h,
				theObject.data.g.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kDissolveIt, who, true, true);
		break;

		case kCustomPict:
		break;

		case kBalloon:
		case kCopterLf:
		case kCopterRt:
		case kDartLf:
		case kDartRt:
		case kBall:
		case kDrip:
		bounds = g_srcRects[theObject.what];
		ZeroRectCorner(&bounds);
		QOffsetRect(&bounds,
				theObject.data.h.topLeft.h,
				theObject.data.h.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kIgnoreIt, who, true, false);
		break;

		case kFish:
		bounds = g_srcRects[theObject.what];
		ZeroRectCorner(&bounds);
		QOffsetRect(&bounds,
				theObject.data.h.topLeft.h,
				theObject.data.h.topLeft.v);
		hotSpotNumber = AddActiveRect(&bounds, kDissolveIt, who, true, true);
		break;

		case kCobweb:
		bounds = g_srcRects[theObject.what];
		ZeroRectCorner(&bounds);
		QOffsetRect(&bounds,
				theObject.data.h.topLeft.h,
				theObject.data.h.topLeft.v);
		QInsetRect(&bounds, -24, -10);
		hotSpotNumber = AddActiveRect(&bounds, kWebIt, who, true, true);
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
		break;

		case kChimes:
		g_numChimes++;
		bounds = g_srcRects[kChimes];
		ZeroRectCorner(&bounds);
		QOffsetRect(&bounds,
				theObject.data.i.bounds.left,
				theObject.data.i.bounds.top);
		hotSpotNumber = AddActiveRect(&bounds, kChimeIt, who, true, false);
		break;
	}

	return (hotSpotNumber);
}

//--------------------------------------------------------------  VerticalRoomOffset

SInt16 VerticalRoomOffset (SInt16 neighbor)
{
	SInt16 offset;

	offset = 0;

	switch (neighbor)
	{
		case kNorthRoom:
		case kNorthEastRoom:
		case kNorthWestRoom:
		offset -= kVertLocalOffset;
		break;

		case kSouthEastRoom:
		case kSouthRoom:
		case kSouthWestRoom:
		offset += kVertLocalOffset;
		break;
	}

	return (offset);
}

//--------------------------------------------------------------  OffsetRectRoomRelative

void OffsetRectRoomRelative (Rect *theRect, SInt16 neighbor)
{
	QOffsetRect(theRect, g_playOriginH, g_playOriginV);

	switch (neighbor)
	{
		case kNorthRoom:
		QOffsetRect(theRect, 0, -kVertLocalOffset);
		break;

		case kNorthEastRoom:
		QOffsetRect(theRect, kRoomWide, -kVertLocalOffset);
		break;

		case kEastRoom:
		QOffsetRect(theRect, kRoomWide, 0);
		break;

		case kSouthEastRoom:
		QOffsetRect(theRect, kRoomWide, kVertLocalOffset);
		break;

		case kSouthRoom:
		QOffsetRect(theRect, 0, kVertLocalOffset);
		break;

		case kSouthWestRoom:
		QOffsetRect(theRect, -kRoomWide, kVertLocalOffset);
		break;

		case kWestRoom:
		QOffsetRect(theRect, -kRoomWide, 0);
		break;

		case kNorthWestRoom:
		QOffsetRect(theRect, -kRoomWide, -kVertLocalOffset);
		break;
	}
}

//--------------------------------------------------------------  GetUpStairsRightEdge

SInt16 GetUpStairsRightEdge (void)
{
	objectType thisObject;
	SInt16 i, rightEdge;

	rightEdge = kRoomWide;

	if (g_thisRoomNumber < 0 || g_thisRoomNumber >= g_thisHouse.nRooms)
		return (rightEdge);

	for (i = 0; i < kMaxRoomObs; i++)
	{
		thisObject = g_thisHouse.rooms[g_thisRoomNumber].objects[i];
		if (thisObject.what == kDownStairs)
		{
			rightEdge = thisObject.data.d.topLeft.h + g_srcRects[kDownStairs].right - 1;
			break;
		}
	}

	return (rightEdge);
}

//--------------------------------------------------------------  GetDownStairsLeftEdge

SInt16 GetDownStairsLeftEdge (void)
{
	objectType thisObject;
	SInt16 i, leftEdge;

	leftEdge = 0;

	if (g_thisRoomNumber < 0 || g_thisRoomNumber >= g_thisHouse.nRooms)
		return (leftEdge);

	for (i = 0; i < kMaxRoomObs; i++)
	{
		thisObject = g_thisHouse.rooms[g_thisRoomNumber].objects[i];
		if (thisObject.what == kUpStairs)
		{
			leftEdge = thisObject.data.d.topLeft.h + 1;
			break;
		}
	}

	return (leftEdge);
}
