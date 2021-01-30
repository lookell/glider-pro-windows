//============================================================================
//----------------------------------------------------------------------------
//                                ObjectAdd.c
//----------------------------------------------------------------------------
//============================================================================

#include "ObjectAdd.h"

#include "DialogUtils.h"
#include "GliderDefines.h"
#include "House.h"
#include "HouseIO.h"
#include "HouseLegal.h"
#include "MainWindow.h"
#include "Menu.h"
#include "ObjectEdit.h"
#include "Objects.h"
#include "RectUtils.h"
#include "ResourceIDs.h"
#include "Room.h"
#include "Utilities.h"

#define kMaxSoundTriggers       1
#define kMaxStairs              1
#define kMouseholeBottom        295
#define kFireplaceBottom        297
#define kManholeSits            322
#define kGrecoVentTop           303
#define kSewerBlowerTop         292

SInt16 FindEmptyObjectSlot (const roomType *room);
void ShoutNoMoreObjects (HWND ownerWindow);
SInt16 HowManyCandleObjects (const roomType *room);
SInt16 HowManyTikiObjects (const roomType *room);
SInt16 HowManyBBQObjects (const roomType *room);
SInt16 HowManyCuckooObjects (const roomType *room);
SInt16 HowManyBandsObjects (void);
SInt16 HowManyGreaseObjects (void);
SInt16 HowManyStarsObjects (void);
SInt16 HowManySoundObjects (void);
SInt16 HowManyUpStairsObjects (void);
SInt16 HowManyDownStairsObjects (void);
SInt16 HowManyShredderObjects (void);
SInt16 HowManyDynamicObjects (void);
void ShoutNoMoreSpecialObjects (HWND ownerWindow);

SInt16 g_wasFlower;

//==============================================================  Functions
//--------------------------------------------------------------  AddNewObject

Boolean AddNewObject (HWND ownerWindow, Point where, SInt16 what, Boolean showItNow)
{
	objectType *theObject;
	Rect srcRect;
	Rect newRect;
	Boolean drawWholeRoom;

	if (COMPILEDEMO)
		return (false);

	g_objActive = FindEmptyObjectSlot(g_thisRoom);
	if (g_objActive == -1)
	{
		ShoutNoMoreObjects(ownerWindow);
		return (false);
	}
	theObject = &g_thisRoom->objects[g_objActive];

	drawWholeRoom = false;

	switch (what)
	{
		case kFloorVent:
		case kFloorBlower:
		case kSewerGrate:
		case kTaper:
		case kCandle:
		case kStubby:
		case kTiki:
		case kBBQ:
		case kInvisBlower:
		case kGrecoVent:
		case kSewerBlower:
		case kLiftArea:
		if (((what == kTaper) || (what == kCandle) || (what == kStubby)) &&
				(HowManyCandleObjects(g_thisRoom) >= kMaxCandles))
		{
			ShoutNoMoreSpecialObjects(ownerWindow);
			return (false);
		}
		else if ((what == kTiki) && (HowManyTikiObjects(g_thisRoom) >= kMaxTikis))
		{
			ShoutNoMoreSpecialObjects(ownerWindow);
			return (false);
		}
		else if ((what == kBBQ) && (HowManyBBQObjects(g_thisRoom) >= kMaxCoals))
		{
			ShoutNoMoreSpecialObjects(ownerWindow);
			return (false);
		}
		srcRect = g_srcRects[what];
		theObject->data.a.topLeft.h = where.h - HalfRectWide(&srcRect);
		QSetRect(&newRect, 0, 0, RectWide(&srcRect), RectTall(&srcRect));
		if (what == kFloorVent)
			theObject->data.a.topLeft.v = kFloorVentTop;
		else if (what == kFloorBlower)
			theObject->data.a.topLeft.v = kFloorBlowerTop;
		else if ((what == kTaper) || (what == kCandle) || (what == kStubby) ||
				(what == kTiki) || (what == kBBQ) || (what == kInvisBlower) ||
				(what == kLiftArea))
			theObject->data.a.topLeft.v = where.v - HalfRectTall(&srcRect);
		else if (what == kGrecoVent)
			theObject->data.a.topLeft.v = kGrecoVentTop;
		else if (what == kSewerBlower)
			theObject->data.a.topLeft.v = kSewerBlowerTop;
		QOffsetRect(&newRect, theObject->data.a.topLeft.h,
				theObject->data.a.topLeft.v);
		theObject->data.a.distance = 64;
		theObject->data.a.initial = true;
		theObject->data.a.state = true;
		theObject->data.a.vector = 0x01;
		if (what == kLiftArea)
			theObject->data.a.tall = 0x10;
		else
			theObject->data.a.tall = 0x00;
		break;

		case kCeilingVent:
		case kCeilingBlower:
		srcRect = g_srcRects[what];
		theObject->data.a.topLeft.h = where.h - HalfRectWide(&srcRect);
		QSetRect(&newRect, 0, 0, RectWide(&srcRect), RectTall(&srcRect));
		if (what == kCeilingVent)
			theObject->data.a.topLeft.v = kCeilingVentTop;
		else if (what == kCeilingBlower)
			theObject->data.a.topLeft.v = kCeilingBlowerTop;
		QOffsetRect(&newRect, theObject->data.a.topLeft.h,
				theObject->data.a.topLeft.v);
		theObject->data.a.distance = 32;
		theObject->data.a.initial = true;
		theObject->data.a.state = true;
		theObject->data.a.vector = 0x04;
		break;

		case kLeftFan:
		theObject->data.a.topLeft.h =
				where.h - HalfRectWide(&g_srcRects[kLeftFan]);
		theObject->data.a.topLeft.v =
				where.v - HalfRectTall(&g_srcRects[kLeftFan]);
		QSetRect(&newRect, 0, 0, RectWide(&g_srcRects[kLeftFan]),
				RectTall(&g_srcRects[kLeftFan]));
		QOffsetRect(&newRect, theObject->data.a.topLeft.h,
				theObject->data.a.topLeft.v);
		theObject->data.a.distance = 32;
		theObject->data.a.initial = true;
		theObject->data.a.state = true;
		theObject->data.a.vector = 0x08;
		break;

		case kRightFan:
		theObject->data.a.topLeft.h =
				where.h - HalfRectWide(&g_srcRects[kRightFan]);
		theObject->data.a.topLeft.v =
				where.v - HalfRectTall(&g_srcRects[kRightFan]);
		QSetRect(&newRect, 0, 0, RectWide(&g_srcRects[kRightFan]), RectTall(&g_srcRects[kRightFan]));
		QOffsetRect(&newRect, theObject->data.a.topLeft.h,
				theObject->data.a.topLeft.v);
		theObject->data.a.distance = 32;
		theObject->data.a.initial = true;
		theObject->data.a.state = true;
		theObject->data.a.vector = 0x02;
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
		case kBooks:
		case kInvisBounce:
		newRect = g_srcRects[what];
		CenterRectOnPoint(&newRect, where);
		if (what == kCounter)
			newRect.bottom = kCounterBottom;
		else if (what == kDresser)
			newRect.bottom = kDresserBottom;
		theObject->data.b.bounds = newRect;
		theObject->data.b.pict = 0;
		break;

		case kManhole:
		newRect = g_srcRects[kManhole];
		CenterRectOnPoint(&newRect, where);
		newRect.left = (((newRect.left - 3) / 64) * 64) + 3;
		newRect.right = newRect.left + RectWide(&g_srcRects[kManhole]);
		newRect.bottom = kManholeSits;
		newRect.top = newRect.bottom - RectTall(&g_srcRects[kManhole]);
		theObject->data.b.bounds = newRect;
		theObject->data.b.pict = 0;
		break;

		case kRedClock:
		case kBlueClock:
		case kYellowClock:
		case kCuckoo:
		case kPaper:
		case kBattery:
		case kBands:
		case kFoil:
		case kStar:
		case kSparkle:
		case kHelium:
		if ((what == kCuckoo) && (HowManyCuckooObjects(g_thisRoom) >= kMaxPendulums))
		{
			ShoutNoMoreSpecialObjects(ownerWindow);
			return (false);
		}
		else if ((what == kBands) && (HowManyBandsObjects() >= kMaxRubberBands))
		{
			ShoutNoMoreSpecialObjects(ownerWindow);
			return (false);
		}
		else if ((what == kStar) && (HowManyStarsObjects() >= kMaxStars))
		{
			ShoutNoMoreSpecialObjects(ownerWindow);
			return (false);
		}
		else if ((what == kSparkle) && (HowManyDynamicObjects() >= kMaxDynamicObs))
		{
			ShoutNoMoreSpecialObjects(ownerWindow);
			return (false);
		}
		theObject->data.c.topLeft.h =
				where.h - HalfRectWide(&g_srcRects[what]);
		theObject->data.c.topLeft.v =
				where.v - HalfRectTall(&g_srcRects[what]);
		QSetRect(&newRect, 0, 0, RectWide(&g_srcRects[what]),
				RectTall(&g_srcRects[what]));
		QOffsetRect(&newRect, theObject->data.c.topLeft.h,
				theObject->data.c.topLeft.v);
		theObject->data.c.length = 0;
		theObject->data.c.points = 0;
		theObject->data.c.state = true;
		theObject->data.c.initial = true;
		break;

		case kGreaseRt:
		case kGreaseLf:
		if (HowManyGreaseObjects() >= kMaxGrease)
		{
			ShoutNoMoreSpecialObjects(ownerWindow);
			return (false);
		}
		theObject->data.c.topLeft.h =
				where.h - HalfRectWide(&g_srcRects[what]);
		theObject->data.c.topLeft.v =
				where.v - HalfRectTall(&g_srcRects[what]);
		QSetRect(&newRect, 0, 0, RectWide(&g_srcRects[what]), RectTall(&g_srcRects[what]));
		QOffsetRect(&newRect, theObject->data.c.topLeft.h,
				theObject->data.c.topLeft.v);
		theObject->data.c.length = 64;
		theObject->data.c.points = 0;
		theObject->data.c.state = true;
		theObject->data.c.initial = true;
		break;

		case kInvisBonus:
		theObject->data.c.topLeft.h =
				where.h - HalfRectWide(&g_srcRects[what]);
		theObject->data.c.topLeft.v =
				where.v - HalfRectTall(&g_srcRects[what]);
		QSetRect(&newRect, 0, 0, RectWide(&g_srcRects[what]), RectTall(&g_srcRects[what]));
		QOffsetRect(&newRect, theObject->data.c.topLeft.h,
				theObject->data.c.topLeft.v);
		theObject->data.c.length = 0;
		theObject->data.c.points = 100;
		theObject->data.c.state = true;
		theObject->data.c.initial = true;
		break;

		case kSlider:
		theObject->data.c.topLeft.h =
				where.h - HalfRectWide(&g_srcRects[what]);
		theObject->data.c.topLeft.v =
				where.v - HalfRectTall(&g_srcRects[what]);
		QSetRect(&newRect, 0, 0, RectWide(&g_srcRects[what]), RectTall(&g_srcRects[what]));
		QOffsetRect(&newRect, theObject->data.c.topLeft.h,
				theObject->data.c.topLeft.v);
		theObject->data.c.length = 64;
		theObject->data.c.points = 0;
		theObject->data.c.state = true;
		theObject->data.c.initial = true;
		break;

		case kUpStairs:
		case kDownStairs:
		if ((what == kUpStairs) && (HowManyUpStairsObjects() >= kMaxStairs))
		{
			ShoutNoMoreSpecialObjects(ownerWindow);
			return (false);
		}
		else if ((what == kDownStairs) && (HowManyDownStairsObjects() >= kMaxStairs))
		{
			ShoutNoMoreSpecialObjects(ownerWindow);
			return (false);
		}
		theObject->data.d.topLeft.h =
				where.h - HalfRectWide(&g_srcRects[what]);
		theObject->data.d.topLeft.v = kStairsTop;
		QSetRect(&newRect, 0, 0, RectWide(&g_srcRects[what]),
				RectTall(&g_srcRects[what]));
		QOffsetRect(&newRect, theObject->data.d.topLeft.h,
				theObject->data.d.topLeft.v);
		theObject->data.d.tall = 0;
		theObject->data.d.where = -1;
		theObject->data.d.who = 255;
		theObject->data.d.wide = 0;
		break;

		case kMailboxLf:
		case kMailboxRt:
		theObject->data.d.topLeft.h =
				where.h - HalfRectWide(&g_srcRects[what]);
		theObject->data.d.topLeft.v =
				where.v - HalfRectTall(&g_srcRects[what]);
		QSetRect(&newRect, 0, 0, RectWide(&g_srcRects[what]),
				RectTall(&g_srcRects[what]));
		QOffsetRect(&newRect, theObject->data.d.topLeft.h,
				theObject->data.d.topLeft.v);
		theObject->data.d.tall = 0;
		theObject->data.d.where = -1;
		theObject->data.d.who = 255;
		theObject->data.d.wide = 0;
		break;

		case kFloorTrans:
		theObject->data.d.topLeft.h =
				where.h - HalfRectWide(&g_srcRects[what]);
		theObject->data.d.topLeft.v = kFloorTransTop;
		QSetRect(&newRect, 0, 0, RectWide(&g_srcRects[what]),
				RectTall(&g_srcRects[what]));
		QOffsetRect(&newRect, theObject->data.d.topLeft.h,
				theObject->data.d.topLeft.v);
		theObject->data.d.tall = 0;
		theObject->data.d.where = -1;
		theObject->data.d.who = 255;
		theObject->data.d.wide = 0;
		break;

		case kCeilingTrans:
		theObject->data.d.topLeft.h =
				where.h - HalfRectWide(&g_srcRects[what]);
		theObject->data.d.topLeft.v = kCeilingTransTop;
		QSetRect(&newRect, 0, 0, RectWide(&g_srcRects[what]),
				RectTall(&g_srcRects[what]));
		QOffsetRect(&newRect, theObject->data.d.topLeft.h,
				theObject->data.d.topLeft.v);
		theObject->data.d.tall = 0;
		theObject->data.d.where = -1;
		theObject->data.d.who = 255;
		theObject->data.d.wide = 0;
		break;

		case kDoorInLf:
		case kDoorInRt:
		case kDoorExRt:
		case kDoorExLf:
		case kWindowInLf:
		case kWindowInRt:
		case kWindowExRt:
		case kWindowExLf:
		if ((what == kDoorInLf) || (what == kDoorInRt))
		{
			if (where.h > (kRoomWide / 2))
			{
				what = kDoorInRt;
				theObject->what = kDoorInRt;
				theObject->data.d.topLeft.h = kDoorInRtLeft;
				theObject->data.d.topLeft.v = kDoorInTop;
			}
			else
			{
				what = kDoorInLf;
				theObject->what = kDoorInLf;
				theObject->data.d.topLeft.h = kDoorInLfLeft;
				theObject->data.d.topLeft.v = kDoorInTop;
			}
		}
		else if ((what == kDoorExRt) || (what == kDoorExLf))
		{
			if (where.h > (kRoomWide / 2))
			{
				what = kDoorExRt;
				theObject->what = kDoorExRt;
				theObject->data.d.topLeft.h = kDoorExRtLeft;
				theObject->data.d.topLeft.v = kDoorExTop;
			}
			else
			{
				what = kDoorExLf;
				theObject->what = kDoorExLf;
				theObject->data.d.topLeft.h = kDoorExLfLeft;
				theObject->data.d.topLeft.v = kDoorExTop;
			}
		}
		else if ((what == kWindowInLf) || (what == kWindowInRt))
		{
			if (where.h > (kRoomWide / 2))
			{
				what = kWindowInRt;
				theObject->what = kWindowInRt;
				theObject->data.d.topLeft.h = kWindowInRtLeft;
				theObject->data.d.topLeft.v = kWindowInTop;
			}
			else
			{
				what = kWindowInLf;
				theObject->what = kWindowInLf;
				theObject->data.d.topLeft.h = kWindowInLfLeft;
				theObject->data.d.topLeft.v = kWindowInTop;
			}
		}
		else if ((what == kWindowExRt) || (what == kWindowExLf))
		{
			if (where.h > (kRoomWide / 2))
			{
				what = kWindowExRt;
				theObject->what = kWindowExRt;
				theObject->data.d.topLeft.h = kWindowExRtLeft;
				theObject->data.d.topLeft.v = kWindowExTop;
			}
			else
			{
				what = kWindowExLf;
				theObject->what = kWindowExLf;
				theObject->data.d.topLeft.h = kWindowExLfLeft;
				theObject->data.d.topLeft.v = kWindowExTop;
			}
		}
		QSetRect(&newRect, 0, 0, RectWide(&g_srcRects[what]),
				RectTall(&g_srcRects[what]));
		QOffsetRect(&newRect, theObject->data.d.topLeft.h,
				theObject->data.d.topLeft.v);
		theObject->data.d.tall = 0;
		theObject->data.d.where = -1;
		theObject->data.d.who = 255;
		theObject->data.d.wide = 0;
		drawWholeRoom = true;
		break;

		case kInvisTrans:
		newRect = g_srcRects[what];
		CenterRectOnPoint(&newRect, where);
		theObject->data.d.topLeft.h = newRect.left;
		theObject->data.d.topLeft.v = newRect.top;
		theObject->data.d.tall = newRect.bottom - newRect.top;
		theObject->data.d.where = -1;
		theObject->data.d.who = 255;
		theObject->data.d.wide = 0;
		break;

		case kDeluxeTrans:
		newRect = g_srcRects[what];
		CenterRectOnPoint(&newRect, where);
		theObject->data.d.topLeft.h = newRect.left;
		theObject->data.d.topLeft.v = newRect.top;
		theObject->data.d.tall = 0x1010;  // 64 x 64
		theObject->data.d.where = -1;
		theObject->data.d.who = 255;
		theObject->data.d.wide = 0x10;  // Initially on
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
		if ((what == kSoundTrigger) && (HowManySoundObjects() >= kMaxSoundTriggers))
		{
			ShoutNoMoreSpecialObjects(ownerWindow);
			return (false);
		}
		theObject->data.e.topLeft.h =
				where.h - HalfRectWide(&g_srcRects[what]);
		theObject->data.e.topLeft.v =
				where.v - HalfRectTall(&g_srcRects[what]);
		QSetRect(&newRect, 0, 0, RectWide(&g_srcRects[what]),
				RectTall(&g_srcRects[what]));
		QOffsetRect(&newRect, theObject->data.e.topLeft.h,
				theObject->data.e.topLeft.v);
		theObject->data.e.delay = 0;
		if (what == kSoundTrigger)
			theObject->data.e.where = 3000;
		else
			theObject->data.e.where = -1;
		theObject->data.e.who = 255;
		if ((what == kTrigger) || (what == kLgTrigger))
			theObject->data.e.type = kOneShot;
		else
			theObject->data.e.type = kToggle;
		break;

		case kCeilingLight:
		case kLightBulb:
		case kTableLamp:
		case kHipLamp:
		case kDecoLamp:
		case kFlourescent:
		case kTrackLight:
		case kInvisLight:
		if (what == kCeilingLight)
		{
			theObject->data.f.topLeft.h =
					where.h - HalfRectWide(&g_srcRects[what]);
			theObject->data.f.topLeft.v = kCeilingLightTop;
			QSetRect(&newRect, 0, 0, RectWide(&g_srcRects[what]),
					RectTall(&g_srcRects[what]));
			QOffsetRect(&newRect, theObject->data.f.topLeft.h,
					theObject->data.f.topLeft.v);
			theObject->data.f.length = 64;
		}
		else if (what == kHipLamp)
		{
			theObject->data.f.topLeft.h =
					where.h - HalfRectWide(&g_srcRects[what]);
			theObject->data.f.topLeft.v = kHipLampTop;
			QSetRect(&newRect, 0, 0, RectWide(&g_srcRects[what]),
					RectTall(&g_srcRects[what]));
			QOffsetRect(&newRect, theObject->data.f.topLeft.h,
					theObject->data.f.topLeft.v);
			theObject->data.f.length = 0;
		}
		else if (what == kDecoLamp)
		{
			theObject->data.f.topLeft.h =
					where.h - HalfRectWide(&g_srcRects[what]);
			theObject->data.f.topLeft.v = kDecoLampTop;
			QSetRect(&newRect, 0, 0, RectWide(&g_srcRects[what]),
					RectTall(&g_srcRects[what]));
			QOffsetRect(&newRect, theObject->data.f.topLeft.h,
					theObject->data.f.topLeft.v);
			theObject->data.f.length = 0;
		}
		else if (what == kFlourescent)
		{
			theObject->data.f.topLeft.h =
					where.h - HalfRectWide(&g_srcRects[what]);
			theObject->data.f.topLeft.v = kFlourescentTop;
			newRect = g_srcRects[what];
			QOffsetRect(&newRect, theObject->data.f.topLeft.h,
					theObject->data.f.topLeft.v);
			theObject->data.f.length = 64;
		}
		else if (what == kTrackLight)
		{
			theObject->data.f.topLeft.h =
					where.h - HalfRectWide(&g_srcRects[what]);
			theObject->data.f.topLeft.v = kTrackLightTop;
			newRect = g_srcRects[what];
			QOffsetRect(&newRect, theObject->data.f.topLeft.h,
					theObject->data.f.topLeft.v);
			theObject->data.f.length = 64;
		}
		else
		{
			theObject->data.f.topLeft.h =
					where.h - HalfRectWide(&g_srcRects[what]);
			theObject->data.f.topLeft.v =
					where.v - HalfRectTall(&g_srcRects[what]);
			QSetRect(&newRect, 0, 0, RectWide(&g_srcRects[what]),
					RectTall(&g_srcRects[what]));
			QOffsetRect(&newRect, theObject->data.f.topLeft.h,
					theObject->data.f.topLeft.v);
			theObject->data.f.length = 0;
		}
		theObject->data.f.initial = true;
		theObject->data.f.state = true;
		theObject->data.f.byte0 = 0;
		theObject->data.f.byte1 = 0;
		drawWholeRoom = true;
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
		if ((what != kGuitar) && (what != kCinderBlock) && (what != kFlowerBox) &&
				(what != kCDs) && (what != kCustomPict) &&
				(HowManyShredderObjects() >= kMaxShredded))
		{
			ShoutNoMoreSpecialObjects(ownerWindow);
			return (false);
		}
		theObject->data.g.topLeft.h =
				where.h - HalfRectWide(&g_srcRects[what]);
		theObject->data.g.topLeft.v =
				where.v - HalfRectTall(&g_srcRects[what]);
		QSetRect(&newRect, 0, 0, RectWide(&g_srcRects[what]),
				RectTall(&g_srcRects[what]));
		QOffsetRect(&newRect, theObject->data.g.topLeft.h,
				theObject->data.g.topLeft.v);
		if (what == kToaster)
		{
			theObject->data.g.height = 64;
			theObject->data.g.delay = 10 + (Byte)RandomInt(10);
		}
		else if (what == kOutlet)
		{
			theObject->data.g.height = 0;
			theObject->data.g.delay = 10 + (Byte)RandomInt(10);
		}
		else if (what == kCustomPict)
		{
			theObject->data.g.height = kCustomPictFallbackID;
			theObject->data.g.delay = 0;
		}
		else
		{
			theObject->data.g.height = 0;
			theObject->data.g.delay = 0;
		}
		if (what == kMicrowave)
			theObject->data.g.byte0 = 7;
		else
			theObject->data.g.byte0 = 0;
		theObject->data.g.initial = true;
		theObject->data.g.state = true;
		break;

		case kBalloon:
		case kCopterLf:
		case kCopterRt:
		case kDartLf:
		case kDartRt:
		case kCobweb:
		if ((what != kCobweb) && (HowManyDynamicObjects() >= kMaxDynamicObs))
		{
			ShoutNoMoreSpecialObjects(ownerWindow);
			return (false);
		}
		if (what == kDartLf)
		{
			theObject->data.h.topLeft.h =
					kRoomWide - RectWide(&g_srcRects[what]);
		}
		else if (what == kDartRt)
		{
			theObject->data.h.topLeft.h = 0;
		}
		else
		{
			theObject->data.h.topLeft.h =
					where.h - HalfRectWide(&g_srcRects[what]);
		}
		if ((what == kDartLf) || (what == kDartRt) || (what == kCobweb))
		{
			theObject->data.h.topLeft.v =
					where.v - HalfRectTall(&g_srcRects[what]);
		}
		else
		{
			theObject->data.h.topLeft.v =
					(kTileHigh / 2) - HalfRectTall(&g_srcRects[what]);
		}
		QSetRect(&newRect, 0, 0, RectWide(&g_srcRects[what]),
				RectTall(&g_srcRects[what]));
		QOffsetRect(&newRect, theObject->data.h.topLeft.h,
				theObject->data.h.topLeft.v);
		theObject->data.h.length = 0;
		if (what == kCobweb)
			theObject->data.h.delay = 0;
		else
			theObject->data.h.delay = 10 + (Byte)RandomInt(10);
		theObject->data.h.byte0 = 0;
		theObject->data.h.initial = true;
		theObject->data.h.state = true;
		break;

		case kBall:
		case kDrip:
		case kFish:
		if (HowManyDynamicObjects() >= kMaxDynamicObs)
		{
			ShoutNoMoreSpecialObjects(ownerWindow);
			return (false);
		}
		theObject->data.h.topLeft.h =
				where.h - HalfRectWide(&g_srcRects[what]);
		theObject->data.h.topLeft.v =
				where.v - HalfRectTall(&g_srcRects[what]);
		QSetRect(&newRect, 0, 0, RectWide(&g_srcRects[what]),
				RectTall(&g_srcRects[what]));
		QOffsetRect(&newRect, theObject->data.h.topLeft.h,
				theObject->data.h.topLeft.v);
		theObject->data.h.length = 64;
		if (what == kBall)
			theObject->data.h.delay = 0;
		else
			theObject->data.h.delay = 10 + (Byte)RandomInt(10);
		theObject->data.h.byte0 = 0;
		theObject->data.h.initial = true;
		theObject->data.h.state = true;
		break;

		case kMousehole:
		newRect = g_srcRects[what];
		CenterRectOnPoint(&newRect, where);
		newRect.bottom = kMouseholeBottom;
		newRect.top = newRect.bottom - RectTall(&g_srcRects[what]);
		theObject->data.i.bounds = newRect;
		theObject->data.i.pict = 0;
		break;

		case kFireplace:
		newRect = g_srcRects[what];
		CenterRectOnPoint(&newRect, where);
		newRect.bottom = kFireplaceBottom;
		newRect.top = newRect.bottom - RectTall(&g_srcRects[what]);
		theObject->data.i.bounds = newRect;
		theObject->data.i.pict = 0;
		break;

		case kFlower:
		if (g_wasFlower < 0 || g_wasFlower >= kNumFlowers)  // sanity check
			g_wasFlower = 0;
		if (GetKeyState(VK_SHIFT) >= 0)  // shift key up?
			g_wasFlower = RandomInt(kNumFlowers);
		newRect = g_flowerSrc[g_wasFlower];
		CenterRectOnPoint(&newRect, where);
		theObject->data.i.bounds = newRect;
		theObject->data.i.pict = g_wasFlower;
		break;

		case kOzma:
		case kMirror:
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
		newRect = g_srcRects[what];
		CenterRectOnPoint(&newRect, where);
		theObject->data.i.bounds = newRect;
		theObject->data.i.pict = 0;
		break;

		default:
		return (false);
		break;
	}

	theObject->what = what;
	g_thisRoom->numObjects++;
	if (KeepObjectLegal())
	{
	}
	g_fileDirty = true;
	UpdateMenus(false);

	if (showItNow)
	{
		if (drawWholeRoom)
			ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
		GetThisRoomsObjRects();
		DrawThisRoomsObjects();
		InvalidateRect(g_mainWindow, NULL, TRUE);
		StartMarqueeForActiveObject();
	}

	return (true);
}

//--------------------------------------------------------------  FindEmptyObjectSlot

SInt16 FindEmptyObjectSlot (const roomType *room)
{
	SInt16 i, emptySlot;

	emptySlot = -1;
	for (i = 0; i < kMaxRoomObs; i++)
	{
		if (room->objects[i].what == kObjectIsEmpty)
		{
			emptySlot = i;
			break;
		}
	}

	return (emptySlot);
}

//--------------------------------------------------------------  FindObjectSlotInRoom

SInt16 FindObjectSlotInRoom (const houseType *house, SInt16 roomNumber)
{
	roomPtr testRoomPtr;
	SInt16 i, emptySlot;

	if (roomNumber < 0 || roomNumber >= house->nRooms)
	{
		return -1;
	}

	emptySlot = -1;
	testRoomPtr = &(house->rooms[roomNumber]);

	for (i = 0; i < kMaxRoomObs; i++)
	{
		if (testRoomPtr->objects[i].what == kObjectIsEmpty)
		{
			emptySlot = i;
			break;
		}
	}

	return (emptySlot);
}

//--------------------------------------------------------------  DoesRoomNumHaveObject

Boolean DoesRoomNumHaveObject (const houseType *house, SInt16 room, SInt16 what)
{
	roomPtr testRoomPtr;
	SInt16 i;
	Boolean hasIt;

	if (room < 0 || room >= house->nRooms)
	{
		return false;
	}

	testRoomPtr = &(house->rooms[room]);
	hasIt = false;

	for (i = 0; i < kMaxRoomObs; i++)
	{
		if (testRoomPtr->objects[i].what == what)
		{
			hasIt = true;
			break;
		}
	}

	return (hasIt);
}

//--------------------------------------------------------------  ShoutNoMoreObjects

void ShoutNoMoreObjects (HWND ownerWindow)
{
	Alert(kNoMoreObjectsAlert, ownerWindow, NULL);
}

//--------------------------------------------------------------  HowManyCandleObjects

SInt16 HowManyCandleObjects (const roomType *room)
{
	SInt16 i, aCandle;

	aCandle = 0;
	for (i = 0; i < kMaxRoomObs; i++)
	{
		if ((room->objects[i].what == kTaper) ||
			(room->objects[i].what == kCandle) ||
			(room->objects[i].what == kStubby))
		{
			aCandle++;
		}
	}

	return (aCandle);
}

//--------------------------------------------------------------  HowManyTikiObjects

SInt16 HowManyTikiObjects (const roomType *room)
{
	SInt16 i, aTiki;

	aTiki = 0;
	for (i = 0; i < kMaxRoomObs; i++)
	{
		if (room->objects[i].what == kTiki)
		{
			aTiki++;
		}
	}

	return (aTiki);
}

//--------------------------------------------------------------  HowManyBBQObjects

SInt16 HowManyBBQObjects (const roomType *room)
{
	SInt16 i, aBBQ;

	aBBQ = 0;
	for (i = 0; i < kMaxRoomObs; i++)
	{
		if (room->objects[i].what == kBBQ)
		{
			aBBQ++;
		}
	}

	return (aBBQ);
}

//--------------------------------------------------------------  HowManyCuckooObjects

SInt16 HowManyCuckooObjects (const roomType *room)
{
	SInt16 i, aCuckoo;

	aCuckoo = 0;
	for (i = 0; i < kMaxRoomObs; i++)
	{
		if (room->objects[i].what == kCuckoo)
		{
			aCuckoo++;
		}
	}

	return (aCuckoo);
}

//--------------------------------------------------------------  HowManyBandsObjects

SInt16 HowManyBandsObjects (void)
{
	SInt16 i, aBands;

	aBands = 0;
	for (i = 0; i < kMaxRoomObs; i++)
		if (g_thisRoom->objects[i].what == kBands)
			aBands++;

	return (aBands);
}

//--------------------------------------------------------------  HowManyGreaseObjects

SInt16 HowManyGreaseObjects (void)
{
	SInt16 i, aGrease;

	aGrease = 0;
	for (i = 0; i < kMaxRoomObs; i++)
		if ((g_thisRoom->objects[i].what == kGreaseRt) ||
				(g_thisRoom->objects[i].what == kGreaseLf))
			aGrease++;

	return (aGrease);
}

//--------------------------------------------------------------  HowManyStarsObjects

SInt16 HowManyStarsObjects (void)
{
	SInt16 i, aStar;

	aStar = 0;
	for (i = 0; i < kMaxRoomObs; i++)
		if (g_thisRoom->objects[i].what == kStar)
			aStar++;

	return (aStar);
}

//--------------------------------------------------------------  HowManySoundObjects

SInt16 HowManySoundObjects (void)
{
	SInt16 i, aSound;

	aSound = 0;
	for (i = 0; i < kMaxRoomObs; i++)
		if (g_thisRoom->objects[i].what == kSoundTrigger)
			aSound++;

	return (aSound);
}

//--------------------------------------------------------------  HowManyUpStairsObjects

SInt16 HowManyUpStairsObjects (void)
{
	SInt16 i, aStair;

	aStair = 0;
	for (i = 0; i < kMaxRoomObs; i++)
		if (g_thisRoom->objects[i].what == kUpStairs)
			aStair++;

	return (aStair);
}

//--------------------------------------------------------------  HowManyDownStairsObjects

SInt16 HowManyDownStairsObjects (void)
{
	SInt16 i, aStair;

	aStair = 0;
	for (i = 0; i < kMaxRoomObs; i++)
		if (g_thisRoom->objects[i].what == kDownStairs)
			aStair++;

	return (aStair);
}

//--------------------------------------------------------------  HowManyShredderObjects

SInt16 HowManyShredderObjects (void)
{
	SInt16 i, aShredder;

	aShredder = 0;
	for (i = 0; i < kMaxRoomObs; i++)
		if (g_thisRoom->objects[i].what == kShredder)
			aShredder++;

	return (aShredder);
}

//--------------------------------------------------------------  HowManyDynamicObjects

SInt16 HowManyDynamicObjects (void)
{
	SInt16 i, aDinah;

	aDinah = 0;
	for (i = 0; i < kMaxRoomObs; i++)
		if ((g_thisRoom->objects[i].what == kSparkle) ||
				(g_thisRoom->objects[i].what == kToaster) ||
				(g_thisRoom->objects[i].what == kMacPlus) ||
				(g_thisRoom->objects[i].what == kTV) ||
				(g_thisRoom->objects[i].what == kCoffee) ||
				(g_thisRoom->objects[i].what == kOutlet) ||
				(g_thisRoom->objects[i].what == kVCR) ||
				(g_thisRoom->objects[i].what == kStereo) ||
				(g_thisRoom->objects[i].what == kMicrowave) ||
				(g_thisRoom->objects[i].what == kBalloon) ||
				(g_thisRoom->objects[i].what == kCopterLf) ||
				(g_thisRoom->objects[i].what == kCopterRt) ||
				(g_thisRoom->objects[i].what == kDartLf) ||
				(g_thisRoom->objects[i].what == kDartRt) ||
				(g_thisRoom->objects[i].what == kBall) ||
				(g_thisRoom->objects[i].what == kDrip) ||
				(g_thisRoom->objects[i].what == kFish))
			aDinah++;

	return (aDinah);
}

//--------------------------------------------------------------  ShoutNoMoreSpecialObjects

void ShoutNoMoreSpecialObjects (HWND ownerWindow)
{
	Alert(kNoMoreSpecialAlert, ownerWindow, NULL);
}
