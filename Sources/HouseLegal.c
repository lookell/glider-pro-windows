//============================================================================
//----------------------------------------------------------------------------
//                                HouseLegal.c
//----------------------------------------------------------------------------
//============================================================================

#include "HouseLegal.h"

#include "Banner.h"
#include "House.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "ObjectEdit.h"
#include "ObjectRects.h"
#include "Objects.h"
#include "RectUtils.h"
#include "Room.h"
#include "StringUtils.h"
#include "WindowUtils.h"

#include <strsafe.h>

#include <stdlib.h>

void WrapBannerAndTrailer (void);
void ValidateNumberOfRooms (void);
void CheckDuplicateFloorSuite (void);
void CompressHouse (void);
void LopOffExtraRooms (HWND mssgWindow);
void ValidateRoomNumbers (HWND mssgWindow);
void CountUntitledRooms (void);
void CheckRoomNameLength (void);
void MakeSureNumObjectsJives (void);
void KeepAllObjectsLegal (HWND mssgWindow);
void CheckForStaircasePairs (HWND mssgWindow);

Boolean g_isHouseChecks;

static SInt16 g_houseErrors;
static SInt16 g_wasRoom;

//==============================================================  Functions
//--------------------------------------------------------------  KeepObjectLegal
// Does a test of the current object active for any illegal bounds
// or values.  It corrects the erros and returns true if any changes
// were made.

Boolean KeepObjectLegal (void)
{
	objectType *theObject;
	Rect bounds, roomRect;
	SInt16 direction, dist;
	Boolean unchanged;

	unchanged = true;

	if (COMPILEDEMO)
		return (unchanged);

	if (g_objActive == kInitialGliderSelected)
	{
		if (g_thisHouse.initial.h < 0)
			g_thisHouse.initial.h = 0;
		if (g_thisHouse.initial.v < 0)
			g_thisHouse.initial.v = 0;
		if (g_thisHouse.initial.h > (kRoomWide - kGliderWide))
			g_thisHouse.initial.h = kRoomWide - kGliderWide;
		if (g_thisHouse.initial.v > (kTileHigh - kGliderHigh))
			g_thisHouse.initial.v = kTileHigh - kGliderHigh;
		return (true);
	}
	else if (g_objActive < 0 || g_objActive >= kMaxRoomObs)
	{
		return (true);
	}

	theObject = &g_thisRoom->objects[g_objActive];

	QSetRect(&roomRect, 0, 0, kRoomWide, kTileHigh);

	switch (theObject->what)
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
		case kLiftArea:
		GetObjectRect(theObject, &bounds);
		if (ForceRectInRect(&bounds, &roomRect))
		{
			theObject->data.a.topLeft.h = bounds.left;
			theObject->data.a.topLeft.v = bounds.top;
			unchanged = false;
			if (theObject->what == kLiftArea)
			{
				theObject->data.a.distance = RectWide(&bounds);
				theObject->data.a.tall = (Byte)(RectTall(&bounds) / 2);
			}
		}
		if ((theObject->what == kStubby) && (theObject->data.a.topLeft.h % 2 == 0))
		{
			theObject->data.a.topLeft.h--;
			unchanged = false;
		}
		if (((theObject->what == kTaper) || (theObject->what == kCandle) ||
				(theObject->what == kTiki) || (theObject->what == kBBQ)) &&
				(theObject->data.a.topLeft.h % 2 != 0))
		{
			theObject->data.a.topLeft.h--;
			unchanged = false;
		}
		if ((theObject->what == kFloorVent) && (theObject->data.a.topLeft.v != kFloorVentTop))
		{
			theObject->data.a.topLeft.v = kFloorVentTop;
			theObject->data.a.distance += 2;
		}
		if ((theObject->what == kFloorBlower) &&
				(theObject->data.a.topLeft.v != kFloorBlowerTop))
		{
			theObject->data.a.topLeft.v = kFloorBlowerTop;
			theObject->data.a.distance += 2;
		}
		if ((theObject->what == kSewerGrate) &&
				(theObject->data.a.topLeft.v != kSewerGrateTop))
		{
			theObject->data.a.topLeft.v = kSewerGrateTop;
			theObject->data.a.distance += 2;
		}
		if ((theObject->what == kFloorTrans) &&
				(theObject->data.a.topLeft.v != kFloorTransTop))
		{
			theObject->data.a.topLeft.v = kFloorTransTop;
			theObject->data.a.distance += 2;
		}
		if (ObjectHasHandle(&direction, &dist))
		{
			switch (direction)
			{
				case kAbove:
				dist = bounds.top - dist;
				if ((theObject->what == kFloorVent) ||
						(theObject->what == kFloorBlower) ||
						(theObject->what == kTaper) ||
						(theObject->what == kCandle) ||
						(theObject->what == kStubby))
				{
					if (dist < 36)
					{
						theObject->data.a.distance += dist - 36;
						unchanged = false;
					}
				}
				else
				{
					if (dist < 0)
					{
						theObject->data.a.distance += dist;
						unchanged = false;
					}
				}
				break;

				case kToRight:
				dist = bounds.right + dist;
				if (dist > kRoomWide)
				{
					theObject->data.a.distance += (kRoomWide - dist);
					unchanged = false;
				}
				break;

				case kBelow:
				dist = bounds.bottom + dist;
				if (dist > kTileHigh)
				{
					theObject->data.a.distance += (kTileHigh - dist);
					unchanged = false;
				}
				break;

				case kToLeft:
				dist = bounds.left - dist;
				if (dist < 0)
				{
					theObject->data.a.distance += dist;
					unchanged = false;
				}
				break;
			}
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
		case kStool:
		case kTrunk:
		case kInvisObstacle:
		case kManhole:
		case kBooks:
		case kInvisBounce:
		GetObjectRect(theObject, &bounds);
		if (ForceRectInRect(&bounds, &roomRect))
		{
			theObject->data.b.bounds = bounds;
			unchanged = false;
		}
		if ((theObject->what == kManhole) &&
				(((bounds.left - 3) % 64) != 0))
		{
			theObject->data.b.bounds.left =
					(((bounds.left + 29) / 64) * 64) + 3;
			theObject->data.b.bounds.right =
					theObject->data.b.bounds.left +
					RectWide(&g_srcRects[kManhole]);
			unchanged = false;
		}
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
		GetObjectRect(theObject, &bounds);
		if (ForceRectInRect(&bounds, &roomRect))
		{
			theObject->data.c.topLeft.h = bounds.left;
			theObject->data.c.topLeft.v = bounds.top;
			unchanged = false;
		}
		if ((theObject->what == kGreaseRt) &&
				(bounds.right + theObject->data.c.length > kRoomWide))
		{
			theObject->data.c.length = kRoomWide - bounds.right;
			unchanged = false;
		}
		else if ((theObject->what == kGreaseLf) &&
				(bounds.left - theObject->data.c.length < 0))
		{
			theObject->data.c.length = bounds.left;
			unchanged = false;
		}
		else if ((theObject->what == kSlider) &&
				(bounds.left + theObject->data.c.length > kRoomWide))
		{
			theObject->data.c.length = kRoomWide - bounds.left;
			unchanged = false;
		}
		if (theObject->data.c.topLeft.h % 2 != 0)
		{
			theObject->data.c.topLeft.h--;
			unchanged = false;
		}
		if ((theObject->what != kStar) &&
				(theObject->data.c.length % 2 != 0))
		{
			theObject->data.c.length--;
			unchanged = false;
		}
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
		case kInvisTrans:
		case kDeluxeTrans:
		GetObjectRect(theObject, &bounds);
		if (ForceRectInRect(&bounds, &roomRect))
		{
			theObject->data.d.topLeft.h = bounds.left;
			theObject->data.d.topLeft.v = bounds.top;
			unchanged = false;
			if (theObject->what == kDeluxeTrans)
			{
				theObject->data.d.tall = ((RectWide(&bounds) / 4) << 8) +
						(RectTall(&bounds) / 4);
			}
		}
		if ((theObject->what == kDoorInLf) ||
				(theObject->what == kDoorInRt))
		{
			if (theObject->data.d.topLeft.h +
					HalfRectWide(&g_srcRects[kDoorInLf]) > (kRoomWide / 2))
			{
				theObject->data.d.topLeft.h = kDoorInRtLeft;
				theObject->what = kDoorInRt;
			}
			else
			{
				theObject->data.d.topLeft.h = kDoorInLfLeft;
				theObject->what = kDoorInLf;
			}
		}
		if ((theObject->what == kDoorExRt) ||
				(theObject->what == kDoorExLf))
		{
			if (theObject->data.d.topLeft.h +
					HalfRectWide(&g_srcRects[kDoorExRt]) > (kRoomWide / 2))
			{
				theObject->data.d.topLeft.h = kDoorExRtLeft;
				theObject->what = kDoorExRt;
			}
			else
			{
				theObject->data.d.topLeft.h = kDoorExLfLeft;
				theObject->what = kDoorExLf;
			}
		}
		if ((theObject->what == kWindowInLf) ||
				(theObject->what == kWindowInRt))
		{
			if (theObject->data.d.topLeft.h +
					HalfRectWide(&g_srcRects[kWindowInLf]) > (kRoomWide / 2))
			{
				theObject->data.d.topLeft.h = kWindowInRtLeft;
				theObject->what = kWindowInRt;
			}
			else
			{
				theObject->data.d.topLeft.h = kWindowInLfLeft;
				theObject->what = kWindowInLf;
			}
		}
		if ((theObject->what == kWindowExRt) ||
				(theObject->what == kWindowExLf))
		{
			if (theObject->data.d.topLeft.h +
					HalfRectWide(&g_srcRects[kWindowExRt]) > (kRoomWide / 2))
			{
				theObject->data.d.topLeft.h = kWindowExRtLeft;
				theObject->what = kWindowExRt;
			}
			else
			{
				theObject->data.d.topLeft.h = kWindowExLfLeft;
				theObject->what = kWindowExLf;
			}
		}

		if ((theObject->what == kInvisTrans) &&
				((theObject->data.d.topLeft.v +
				theObject->data.d.tall) > kTileHigh))
		{
			theObject->data.d.tall = kTileHigh -
				theObject->data.d.topLeft.v;
			unchanged = false;
		}
		if ((theObject->what == kInvisTrans) &&
				(theObject->data.d.wide < 0))
		{
			theObject->data.d.wide = 0;
			unchanged = false;
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
		case kSoundTrigger:
		GetObjectRect(theObject, &bounds);
		if (ForceRectInRect(&bounds, &roomRect))
		{
			theObject->data.e.topLeft.h = bounds.left;
			theObject->data.e.topLeft.v = bounds.top;
			unchanged = false;
		}
		if (theObject->data.e.topLeft.h % 2 != 0)
		{
			theObject->data.e.topLeft.h--;
			unchanged = false;
		}
		break;

		case kCeilingLight:
		case kLightBulb:
		case kTableLamp:
		case kHipLamp:
		case kDecoLamp:
		case kFlourescent:
		case kTrackLight:
		case kInvisLight:
		GetObjectRect(theObject, &bounds);
		if (ForceRectInRect(&bounds, &roomRect))
		{
			if ((theObject->what == kFlourescent) || (theObject->what == kTrackLight))
			{
				if (theObject->data.f.topLeft.h < bounds.left)
					theObject->data.f.topLeft.h = bounds.left;

				if (theObject->data.f.topLeft.v < bounds.top)
					theObject->data.f.topLeft.v = bounds.top;

				if ((theObject->data.f.topLeft.h + theObject->data.f.length) > bounds.right)
					theObject->data.f.length = bounds.right - theObject->data.f.topLeft.h;
			}
			else
			{
				theObject->data.f.topLeft.h = bounds.left;
				theObject->data.f.topLeft.v = bounds.top;
			}
			unchanged = false;
		}
		if (((theObject->what == kFlourescent) ||
				(theObject->what == kTrackLight)) &&
				((bounds.right > kRoomWide) || (bounds.left < 0)))
		{
			if (theObject->data.f.topLeft.h < 0)
			{
				theObject->data.f.topLeft.h = 0;
				unchanged = false;
			}
			if (bounds.left < 0)
			{
				bounds.left = 0;
				unchanged = false;
			}
			if (theObject->data.f.topLeft.h > kRoomWide)
			{
				theObject->data.f.topLeft.h = kRoomWide;
				unchanged = false;
			}
			if (bounds.right > kRoomWide)
			{
				bounds.right = kRoomWide;
				unchanged = false;
			}
			theObject->data.f.length = kRoomWide - bounds.left;
		}
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
		GetObjectRect(theObject, &bounds);
		if (ForceRectInRect(&bounds, &roomRect))
		{
			theObject->data.g.topLeft.h = bounds.left;
			theObject->data.g.topLeft.v = bounds.top;
			unchanged = false;
		}
		if ((theObject->what == kToaster) &&
				(bounds.top - theObject->data.g.height < 0))
		{
			theObject->data.g.height = bounds.top;
			unchanged = false;
		}
		if ((theObject->what == kTV) &&
				(theObject->data.g.topLeft.h % 2 == 0))
		{
			theObject->data.g.topLeft.h--;
			unchanged = false;
		}
		if (((theObject->what == kToaster) ||
				(theObject->what == kMacPlus) ||
				(theObject->what == kCoffee) ||
				(theObject->what == kOutlet) ||
				(theObject->what == kVCR) ||
				(theObject->what == kStereo) ||
				(theObject->what == kMicrowave)) &&
				(theObject->data.g.topLeft.h % 2 != 0))
		{
			theObject->data.g.topLeft.h--;
			unchanged = false;
		}
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
		GetObjectRect(theObject, &bounds);
		if (ForceRectInRect(&bounds, &roomRect))
		{
			theObject->data.h.topLeft.h = bounds.left;
			theObject->data.h.topLeft.v = bounds.top;
			unchanged = false;
		}
		if (((theObject->what == kBall) ||
				(theObject->what == kFish)) &&
				(bounds.top - theObject->data.h.length < 0))
		{
			theObject->data.h.length = bounds.top;
			unchanged = false;
		}
		if ((theObject->what == kDrip) &&
				(bounds.bottom + theObject->data.h.length > kTileHigh))
		{
			theObject->data.h.length = kTileHigh - bounds.bottom;
			unchanged = false;
		}
		if (((theObject->what == kBalloon) ||
				(theObject->what == kCopterLf) ||
				(theObject->what == kCopterRt) ||
				(theObject->what == kBall) ||
				(theObject->what == kDrip) ||
				(theObject->what == kFish)) &&
				(theObject->data.h.topLeft.h % 2 != 0))
		{
			theObject->data.h.topLeft.h--;
			unchanged = false;
		}
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
		GetObjectRect(theObject, &bounds);
		if (ForceRectInRect(&bounds, &roomRect))
		{
			theObject->data.i.bounds = bounds;
			unchanged = false;
		}
		if (theObject->what == kMirror)
		{
			if (theObject->data.i.bounds.left % 2 != 0)
			{
				theObject->data.i.bounds.left--;
				unchanged = false;
			}
			if (theObject->data.i.bounds.right % 2 != 0)
			{
				theObject->data.i.bounds.right--;
				unchanged = false;
			}
		}
		break;

	}

	return (unchanged);
}

//--------------------------------------------------------------  WrapBannerAndTrailer
// Tries to wrap around the text of the banner and trailer messages.

void WrapBannerAndTrailer (void)
{
	WrapText(g_thisHouse.banner, 40);
	WrapText(g_thisHouse.trailer, 64);
}

//--------------------------------------------------------------  ValidateNumberOfRooms
// Makes sure the number of room count and actual number of rooms match.

void ValidateNumberOfRooms (void)
{
	// This function was useful when the house file reading routine was
	// essentially an `fread` call (i.e., `FSRead` for classic Mac OS).
	// The house reading routine in this source port, however, allocates
	// memory for rooms in a separate array, and makes sure that there
	// are enough bytes in the house file to fully construct every room.
	// As a result, the stated room count and actual number of rooms
	// will always match.
	return;
}

//--------------------------------------------------------------  CheckDuplicateFloorSuite
// Error check, looks for rooms with the same floor suite (stacked).

void CheckDuplicateFloorSuite (void)
{
	#define kRoomsTimesSuites  8192
	SInt16 i, numRooms, bitPlace;
	char pidgeonHoles[kRoomsTimesSuites];

	ZeroMemory(&pidgeonHoles, sizeof(pidgeonHoles));

	numRooms = g_thisHouse.nRooms;
	for (i = 0; i < numRooms; i++)
	{
		if (g_thisHouse.rooms[i].suite != kRoomIsEmpty)
		{
			bitPlace = ((g_thisHouse.rooms[i].floor + 7) * 128) +
					g_thisHouse.rooms[i].suite;
			if ((bitPlace < 0) || (bitPlace >= kRoomsTimesSuites))
			{
				//OutputDebugString(L"Blew array\n");
				continue;
			}
			if (pidgeonHoles[bitPlace] != 0)
			{
				g_houseErrors++;
				g_thisHouse.rooms[i].suite = kRoomIsEmpty;
			}
			else
			{
				pidgeonHoles[bitPlace]++;
			}
		}
	}
}

//--------------------------------------------------------------  CompressHouse
// Removes place-holder (deleted) rooms from the middle of the file.

void CompressHouse (void)
{
	SInt16 wasFirstRoom, roomNumber, probe;
	Boolean compressing, probing;

	if (g_thisHouse.nRooms <= 0)
	{
		return;
	}
	wasFirstRoom = g_thisHouse.firstRoom;
	compressing = true;
	roomNumber = g_thisHouse.nRooms - 1;  // start with last room
	do
	{
		// if not an empty room
		if (g_thisHouse.rooms[roomNumber].suite != kRoomIsEmpty)
		{
			// start looking for empty slot
			probe = 0;
			probing = true;
			do
			{
				// test room at probe to see if empty
				if (g_thisHouse.rooms[probe].suite == kRoomIsEmpty)
				{
					// if it is, copy room there
					g_thisHouse.rooms[probe] = g_thisHouse.rooms[roomNumber];
					g_thisHouse.rooms[roomNumber].suite = kRoomIsEmpty;
					if (roomNumber == wasFirstRoom)
						g_thisHouse.firstRoom = probe;
					if (roomNumber == g_wasRoom)
						g_wasRoom = probe;
					probing = false;
				}
				// bump probe up to next room
				probe++;
				// if we reached the current room
				if ((probing) && (probe >= roomNumber))
				{
					// we can look no further
					probing = false;
					// so we can compress no more
					compressing = false;
				}
			}
			while (probing);
		}
		// go on to room preceding
		roomNumber--;
		// stop if we reach the first room
		if (roomNumber <= 0)
			compressing = false;
	}
	while (compressing);
}

//--------------------------------------------------------------  LopOffExtraRooms
// Deletes all empty rooms hanging off the end of the house file.

void LopOffExtraRooms (HWND mssgWindow)
{
	size_t newSize;
	SInt16 r, count;
	wchar_t message[256];
	roomPtr newRoomsPtr;

	if (g_thisHouse.nRooms <= 0)
	{
		return;
	}

	count = 0;
	r = g_thisHouse.nRooms;  // begin at last room
	do
	{
		r--;
		// look for trailing empties
		if (g_thisHouse.rooms[r].suite == kRoomIsEmpty)
			count++;
		else
			r = 0;
	}
	while (r > 0);

	if (count > 0)  // if there were trailing empties
	{
		r = g_thisHouse.nRooms - count;
		// resize room array (shrink)
		if (r <= 0)
		{
			free(g_thisHouse.rooms);
			g_thisHouse.rooms = NULL;
		}
		else
		{
			newSize = sizeof(roomType) * (size_t)r;
			newRoomsPtr = (roomPtr)realloc(g_thisHouse.rooms, newSize);
			if (newRoomsPtr == NULL)  // problem?
			{
				SetMessageTextColor(mssgWindow, redColor);
				GetLocalizedString(16, message, ARRAYSIZE(message));
				SetMessageWindowMessage(mssgWindow, message);
			}
			else
			{
				g_thisHouse.rooms = newRoomsPtr;
			}
		}
		// reflect new room count
		g_thisHouse.nRooms -= count;
	}
}

//--------------------------------------------------------------  ValidateRoomNumbers
// Error check - ensures that the floor and suite numbers are within legal ranges.

void ValidateRoomNumbers (HWND mssgWindow)
{
	SInt16 i, numRooms;
	wchar_t message[256];

	numRooms = g_thisHouse.nRooms;
	if (numRooms < 0)
	{
		g_thisHouse.nRooms = 0;
		numRooms = 0;
	}
	for (i = 0; i < numRooms; i++)
	{
		if (g_thisHouse.rooms[i].suite != kRoomIsEmpty)
		{
			if ((g_thisHouse.rooms[i].floor > 56) ||
					(g_thisHouse.rooms[i].floor < -7))
			{
				g_thisHouse.rooms[i].suite = kRoomIsEmpty;
				SetMessageTextColor(mssgWindow, redColor);
				GetLocalizedString(17, message, ARRAYSIZE(message));
				SetMessageWindowMessage(mssgWindow, message);
				g_houseErrors++;
			}
			if ((g_thisHouse.rooms[i].suite >= 128) ||
					(g_thisHouse.rooms[i].suite < 0))
			{
				g_thisHouse.rooms[i].suite = kRoomIsEmpty;
				SetMessageTextColor(mssgWindow, redColor);
				GetLocalizedString(18, message, ARRAYSIZE(message));
				SetMessageWindowMessage(mssgWindow, message);
				g_houseErrors++;
			}
		}
	}
}

//--------------------------------------------------------------  CountUntitledRooms
// Returns the number of rooms left "Untitled".

void CountUntitledRooms (void)
{
	SInt16 i, numRooms;
	Str255 untitledRoomStr;

	PasStringCopyC("Untitled Room", untitledRoomStr);

	numRooms = g_thisHouse.nRooms;
	for (i = 0; i < numRooms; i++)
	{
		if ((g_thisHouse.rooms[i].suite != kRoomIsEmpty) &&
				(PasStringEqual(g_thisHouse.rooms[i].name, untitledRoomStr, false)))
			g_houseErrors++;
	}
}

//--------------------------------------------------------------  CheckRoomNameLength
// Error check - ensures the length of the room name is legal.

void CheckRoomNameLength (void)
{
	SInt16 i, numRooms;

	numRooms = g_thisHouse.nRooms;
	for (i = 0; i < numRooms; i++)
	{
		g_thisHouse.rooms[i].unusedByte = 0;

		if ((g_thisHouse.rooms[i].suite != kRoomIsEmpty) &&
				(g_thisHouse.rooms[i].name[0] > 27))
		{
			g_thisHouse.rooms[i].name[0] = 27;
			g_houseErrors++;
		}
	}
}

//--------------------------------------------------------------  MakeSureNumObjectsJives
// Error check - ensures the actual count of objects equals number of objects.

void MakeSureNumObjectsJives (void)
{
	SInt16 i, h, numRooms, count;

	numRooms = g_thisHouse.nRooms;
	for (i = 0; i < numRooms; i++)
	{
		if (g_thisHouse.rooms[i].suite != kRoomIsEmpty)
		{
			count = 0;
			for (h = 0; h < kMaxRoomObs; h++)
			{
				if (g_thisHouse.rooms[i].objects[h].what != kObjectIsEmpty)
					count++;
			}
			if (count != g_thisHouse.rooms[i].numObjects)
			{
				g_houseErrors++;
				g_thisHouse.rooms[i].numObjects = count;
			}
		}
	}
}

//--------------------------------------------------------------  KeepAllObjectsLegal
// Repeatedly calls KeepObjectLegal() on ALL objects in a house.  Wow!

void KeepAllObjectsLegal (HWND mssgWindow)
{
	SInt16 i, h, numRooms;
	wchar_t message[256];

	numRooms = g_thisHouse.nRooms;
	for (i = 0; i < numRooms; i++)
	{
		if (g_thisHouse.rooms[i].suite != kRoomIsEmpty)
		{
			ForceThisRoom(i);
			for (h = 0; h < kMaxRoomObs; h++)
			{
				g_objActive = h;
				if (g_thisRoom->objects[g_objActive].what != kObjectIsEmpty)
				{
					if (!KeepObjectLegal())
					{
						SetMessageTextColor(mssgWindow, redColor);
						GetLocalizedString(19, message, ARRAYSIZE(message));
						SetMessageWindowMessage(mssgWindow, message);
						g_houseErrors++;
						Sleep(1000);
					}
				}
			}
			CopyThisRoomToRoom();
		}
	}
}

//--------------------------------------------------------------  CheckForStaircasePairs
// Ensures that for every up-stair there is a down-stair.

void CheckForStaircasePairs (HWND mssgWindow)
{
	SInt16 i, h, g, numRooms, neighbor;
	Boolean hasStairs;
	wchar_t message[256];

	numRooms = g_thisHouse.nRooms;
	for (i = 0; i < numRooms; i++)
	{
		if (g_thisHouse.rooms[i].suite != kRoomIsEmpty)
		{
			for (h = 0; h < kMaxRoomObs; h++)
			{
				if (g_thisHouse.rooms[i].objects[h].what == kUpStairs)
				{
					// TODO: see if this assignment is necessary any more. It was
					// necessary in the past for the GetNeighborRoomNumber call,
					// because that function accessed g_thisRoomNumber.
					g_thisRoomNumber = i;
					neighbor = GetNeighborRoomNumber(&g_thisHouse, i, kNorthRoom);
					if (neighbor == kRoomIsEmpty)
					{
						SetMessageTextColor(mssgWindow, redColor);
						GetLocalizedString(20, message, ARRAYSIZE(message));
						SetMessageWindowMessage(mssgWindow, message);
						Sleep(1000);
					}
					else
					{
						hasStairs = false;
						for (g = 0; g < kMaxRoomObs; g++)
						{
							if (g_thisHouse.rooms[neighbor].objects[g].what == kDownStairs)
								hasStairs = true;
						}
						if (!hasStairs)
						{
							SetMessageTextColor(mssgWindow, redColor);
							GetLocalizedString(21, message, ARRAYSIZE(message));
							SetMessageWindowMessage(mssgWindow, message);
							Sleep(1000);
						}
					}
				}
				else if (g_thisHouse.rooms[i].objects[h].what == kDownStairs)
				{
					// TODO: see if this assignment is necessary any more. It was
					// necessary in the past for the GetNeighborRoomNumber call,
					// because that function accessed g_thisRoomNumber.
					g_thisRoomNumber = i;
					neighbor = GetNeighborRoomNumber(&g_thisHouse, i, kSouthRoom);
					if (neighbor == kRoomIsEmpty)
					{
						SetMessageTextColor(mssgWindow, redColor);
						GetLocalizedString(22, message, ARRAYSIZE(message));
						SetMessageWindowMessage(mssgWindow, message);
						Sleep(1000);
					}
					else
					{
						hasStairs = false;
						for (g = 0; g < kMaxRoomObs; g++)
						{
							if (g_thisHouse.rooms[neighbor].objects[g].what == kUpStairs)
								hasStairs = true;
						}
						if (!hasStairs)
						{
							SetMessageTextColor(mssgWindow, redColor);
							GetLocalizedString(23, message, ARRAYSIZE(message));
							SetMessageWindowMessage(mssgWindow, message);
							Sleep(1000);
						}
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------  CheckHouseForProblems
// Calls all the above functions and reports (and corrects) errors.

void CheckHouseForProblems (void)
{
	HWND mssgWindow;
	wchar_t message[256];
	wchar_t message2[256];
	SInt16 wasActive;

	if (COMPILEDEMO)
		return;

	g_houseErrors = 0;
	CopyThisRoomToRoom();
	g_wasRoom = g_thisRoomNumber;
	wasActive = g_objActive;
	GetLocalizedString(24, message, ARRAYSIZE(message));
	mssgWindow = OpenMessageWindow(message, g_mainWindow);

	SetMessageTextColor(mssgWindow, blackColor);
	GetLocalizedString(25, message, ARRAYSIZE(message));
	SetMessageWindowMessage(mssgWindow, message);
	WrapBannerAndTrailer();

	if (g_isHouseChecks)
	{
		SetMessageTextColor(mssgWindow, blackColor);
		GetLocalizedString(26, message, ARRAYSIZE(message));
		SetMessageWindowMessage(mssgWindow, message);
		ValidateNumberOfRooms();
		if (g_houseErrors != 0)
		{
			SetMessageTextColor(mssgWindow, blackColor);
			GetLocalizedString(27, message, ARRAYSIZE(message));
			SetMessageWindowMessage(mssgWindow, message);
			Sleep(1000);
			g_houseErrors = 0;
		}
	}

	if (g_isHouseChecks)
	{
		g_houseErrors = 0;
		CheckDuplicateFloorSuite();
		if (g_houseErrors != 0)
		{
			StringCchPrintf(message, ARRAYSIZE(message), L"%d", (int)g_houseErrors);
			GetLocalizedString(28, message2, ARRAYSIZE(message2));
			StringCchCat(message, ARRAYSIZE(message), message2);
			SetMessageTextColor(mssgWindow, redColor);
			SetMessageWindowMessage(mssgWindow, message);
			Sleep(750);
		}
	}

	CompressHouse();
	LopOffExtraRooms(mssgWindow);

	if (g_isHouseChecks)
	{
		ValidateRoomNumbers(mssgWindow);
		if (g_houseErrors != 0)
		{
			StringCchPrintf(message, ARRAYSIZE(message), L"%d", (int)g_houseErrors);
			GetLocalizedString(29, message2, ARRAYSIZE(message2));
			StringCchCat(message, ARRAYSIZE(message), message2);
			SetMessageTextColor(mssgWindow, redColor);
			SetMessageWindowMessage(mssgWindow, message);
			Sleep(1000);
		}
	}

	if (g_isHouseChecks)
	{
		g_houseErrors = 0;
		CountUntitledRooms();
		if (g_houseErrors != 0)
		{
			StringCchPrintf(message, ARRAYSIZE(message), L"%d", (int)g_houseErrors);
			GetLocalizedString(30, message2, ARRAYSIZE(message2));
			StringCchCat(message, ARRAYSIZE(message), message2);
			SetMessageTextColor(mssgWindow, blueColor);
			SetMessageWindowMessage(mssgWindow, message);
			Sleep(750);
		}
	}

	if (g_isHouseChecks)
	{
		g_houseErrors = 0;
		CheckRoomNameLength();
		if (g_houseErrors != 0)
		{
			StringCchPrintf(message, ARRAYSIZE(message), L"%d", (int)g_houseErrors);
			GetLocalizedString(31, message2, ARRAYSIZE(message2));
			StringCchCat(message, ARRAYSIZE(message), message2);
			SetMessageTextColor(mssgWindow, blueColor);
			SetMessageWindowMessage(mssgWindow, message);
			Sleep(750);
		}
	}

	if (g_isHouseChecks)
	{
		g_houseErrors = 0;
		MakeSureNumObjectsJives();
		if (g_houseErrors != 0)
		{
			StringCchPrintf(message, ARRAYSIZE(message), L"%d", (int)g_houseErrors);
			GetLocalizedString(32, message2, ARRAYSIZE(message2));
			StringCchCat(message, ARRAYSIZE(message), message2);
			SetMessageTextColor(mssgWindow, redColor);
			SetMessageWindowMessage(mssgWindow, message);
			Sleep(1000);
		}
	}

	if (g_isHouseChecks)
	{
		g_houseErrors = 0;
		SetMessageTextColor(mssgWindow, blackColor);
		GetLocalizedString(33, message, ARRAYSIZE(message));
		SetMessageWindowMessage(mssgWindow, message);
		KeepAllObjectsLegal(mssgWindow);
		if (g_houseErrors != 0)
		{
			StringCchPrintf(message, ARRAYSIZE(message), L"%d", (int)g_houseErrors);
			GetLocalizedString(34, message2, ARRAYSIZE(message2));
			StringCchCat(message, ARRAYSIZE(message), message2);
			SetMessageTextColor(mssgWindow, redColor);
			SetMessageWindowMessage(mssgWindow, message);
			Sleep(1000);
		}
	}

	if (g_isHouseChecks)
	{
		g_houseErrors = 0;
		CheckForStaircasePairs(mssgWindow);
	}

	if (g_isHouseChecks)
	{
		if (CountStarsInHouse(&g_thisHouse) < 1)
		{
			SetMessageTextColor(mssgWindow, redColor);
			GetLocalizedString(35, message, ARRAYSIZE(message));
			SetMessageWindowMessage(mssgWindow, message);
			Sleep(1000);
		}
	}

	CloseMessageWindow(mssgWindow);
	ForceThisRoom(g_wasRoom);
	g_objActive = wasActive;
}
