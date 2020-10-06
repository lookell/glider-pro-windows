#define GP_USE_WINAPI_H

#include "Objects.h"

//============================================================================
//----------------------------------------------------------------------------
//                                 Objects.c
//----------------------------------------------------------------------------
//============================================================================


#include "AnimCursor.h"
#include "GliderDefines.h"
#include "GliderStructs.h"
#include "House.h"
#include "HouseIO.h"
#include "Link.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "Menu.h"
#include "Music.h"
#include "ObjectEdit.h"
#include "ObjectRects.h"
#include "Room.h"
#include "RoomGraphics.h"
#include "Sound.h"

#include <stdlib.h>


SInt16 GetRoomLinked (const objectType *who);
SInt16 GetObjectLinked (const objectType *who);
void ListOneRoomsObjects (SInt16 where);


// Blowers
Rect blowerSrcRect;
HDC blowerSrcMap;
HDC blowerMaskMap;
Rect flame[kNumCandleFlames];
Rect tikiFlame[kNumTikiFlames];
Rect coals[kNumBBQCoals];

// Furniture
Rect furnitureSrcRect;
HDC furnitureSrcMap;
HDC furnitureMaskMap;
Rect tableSrc;
Rect shelfSrc;
Rect hingeSrc;
Rect handleSrc;
Rect knobSrc;
Rect leftFootSrc;
Rect rightFootSrc;
Rect deckSrc;

// Bonuses
Rect bonusSrcRect;
HDC bonusSrcMap;
HDC bonusMaskMap;
Rect pointsSrcRect;
HDC pointsSrcMap;
HDC pointsMaskMap;
Rect starSrc[6];
Rect sparkleSrc[kNumSparkleModes];
Rect digits[11];
Rect pendulumSrc[3];
Rect greaseSrcRt[4];
Rect greaseSrcLf[4];

// Transport
Rect transSrcRect;
HDC transSrcMap;
HDC transMaskMap;

// Switches
Rect switchSrcRect;
HDC switchSrcMap;
Rect lightSwitchSrc[2];
Rect machineSwitchSrc[2];
Rect thermostatSrc[2];
Rect powerSrc[2];
Rect knifeSwitchSrc[2];

// Lights
Rect lightSrcRect;
HDC lightSrcMap;
HDC lightMaskMap;
Rect flourescentSrc1;
Rect flourescentSrc2;
Rect trackLightSrc[kNumTrackLights];

// Appliances
Rect applianceSrcRect;
HDC applianceSrcMap;
HDC applianceMaskMap;
Rect toastSrcRect;
HDC toastSrcMap;
HDC toastMaskMap;
Rect shredSrcRect;
HDC shredSrcMap;
HDC shredMaskMap;
Rect plusScreen1;
Rect plusScreen2;
Rect tvScreen1;
Rect tvScreen2;
Rect coffeeLight1;
Rect coffeeLight2;
Rect vcrTime1;
Rect vcrTime2;
Rect stereoLight1;
Rect stereoLight2;
Rect microOn;
Rect microOff;
Rect outletSrc[kNumOutletPicts];

// Enemies
Rect balloonSrcRect;
HDC balloonSrcMap;
HDC balloonMaskMap;
Rect balloonSrc[kNumBalloonFrames];
Rect copterSrcRect;
HDC copterSrcMap;
HDC copterMaskMap;
Rect copterSrc[kNumCopterFrames];
Rect dartSrcRect;
HDC dartSrcMap;
HDC dartMaskMap;
Rect dartSrc[kNumDartFrames];
Rect ballSrcRect;
HDC ballSrcMap;
HDC ballMaskMap;
Rect ballSrc[kNumBallFrames];
Rect dripSrcRect;
HDC dripSrcMap;
HDC dripMaskMap;
Rect dripSrc[kNumDripFrames];
Rect fishSrcRect;
HDC fishSrcMap;
HDC fishMaskMap;
Rect fishSrc[kNumFishFrames];
Rect enemySrcRect;
HDC enemySrcMap;
HDC enemyMaskMap;

// Clutter
HDC clutterSrcMap;
HDC clutterMaskMap;
Rect clutterSrcRect;
Rect flowerSrc[kNumFlowers];

Rect srcRects[kNumSrcRects];
Rect tempManholes[kMaxTempManholes];
savedType savedMaps[kMaxSavedMaps];
objDataType masterObjects[kMaxMasterObjects];
hotObject hotSpots[kMaxHotSpots];
SInt16 nHotSpots;
SInt16 numMasterObjects;
SInt16 numTempManholes;
SInt16 tvWithMovieNumber;
Boolean newState;


//==============================================================  Functions
//--------------------------------------------------------------  IsThisValid

Boolean IsThisValid (SInt16 where, SInt16 who)
{
	Boolean		itsGood;

	if (where < 0 || where >= thisHouse.nRooms)
		return (false);
	if (who < 0 || who >= kMaxRoomObs)
		return (false);

	itsGood = true;

	switch (thisHouse.rooms[where].objects[who].what)
	{
		case kObjectIsEmpty:
		itsGood = false;
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
		itsGood = thisHouse.rooms[where].objects[who].data.c.state;
		break;
	}

	return (itsGood);
}

//--------------------------------------------------------------  GetRoomLinked

SInt16 GetRoomLinked (const objectType *who)
{
	SInt16		compoundRoomNumber, whereLinked;
	SInt16		floor, suite;

	switch (who->what)
	{
		case kMailboxLf:
		case kMailboxRt:
		case kFloorTrans:
		case kCeilingTrans:
		case kInvisTrans:
		case kDeluxeTrans:
		compoundRoomNumber = who->data.d.where;
		if (compoundRoomNumber != -1)	// is object linked
		{
			ExtractFloorSuite(compoundRoomNumber, &floor, &suite);
			whereLinked = GetRoomNumber(floor, suite);
		}
		else
			whereLinked = -1;			// not linked
		break;

		case kLightSwitch:
		case kMachineSwitch:
		case kThermostat:
		case kPowerSwitch:
		case kKnifeSwitch:
		case kInvisSwitch:
		case kTrigger:
		case kLgTrigger:
		compoundRoomNumber = who->data.e.where;
		if (compoundRoomNumber != -1)	// is object linked
		{
			ExtractFloorSuite(compoundRoomNumber, &floor, &suite);
			whereLinked = GetRoomNumber(floor, suite);
		}
		else
			whereLinked = -1;			// not linked
		break;

		default:
		whereLinked = -1;
		break;
	}

	return (whereLinked);
}

//--------------------------------------------------------------  GetObjectLinked

SInt16 GetObjectLinked (const objectType *who)
{
	SInt16		whoLinked;

	switch (who->what)
	{
		case kMailboxLf:
		case kMailboxRt:
		case kFloorTrans:
		case kCeilingTrans:
		case kInvisTrans:
		case kDeluxeTrans:
		if (who->data.d.who != 255)		// is it linked?
			whoLinked = (SInt16)who->data.d.who;
		else
			whoLinked = -1;				// object not linked
		break;

		case kLightSwitch:
		case kMachineSwitch:
		case kThermostat:
		case kPowerSwitch:
		case kKnifeSwitch:
		case kInvisSwitch:
		case kTrigger:
		case kLgTrigger:
		if (who->data.e.who != 255)		// is it linked?
			whoLinked = (SInt16)who->data.e.who;
		else
			whoLinked = -1;				// object not linked
		break;

		default:
		whoLinked = -1;
		break;
	}

	return (whoLinked);
}

//--------------------------------------------------------------  ObjectIsLinkTransport

Boolean ObjectIsLinkTransport (const objectType *who)
{
	Boolean		itIs;

	itIs = false;
	if ((who->what == kMailboxLf) || (who->what == kMailboxRt) ||
			(who->what == kFloorTrans) || (who->what == kCeilingTrans) ||
			(who->what == kInvisTrans) || (who->what == kDeluxeTrans))
	{
		itIs = true;
	}

	return (itIs);
}

//--------------------------------------------------------------  ObjectIsLinkSwitch

Boolean ObjectIsLinkSwitch (const objectType *who)
{
	Boolean		itIs;

	itIs = false;
	if ((who->what == kLightSwitch) || (who->what == kMachineSwitch) ||
			(who->what == kThermostat) || (who->what == kPowerSwitch) ||
			(who->what == kKnifeSwitch) || (who->what == kInvisSwitch) ||
			(who->what == kTrigger) || (who->what == kLgTrigger))
	{
		itIs = true;
	}

	return (itIs);
}

//--------------------------------------------------------------  ListOneRoomsObjects

void ListOneRoomsObjects (SInt16 where)
{
	objectType	thisObject;
	SInt16		roomNum, n;

	if (where < 0 || where >= ARRAYSIZE(localNumbers))
		return;

	roomNum = localNumbers[where];
	if (roomNum < 0 || roomNum >= thisHouse.nRooms)
		return;

	for (n = 0; n < kMaxRoomObs; n++)
	{
		if (numMasterObjects < kMaxMasterObjects)
		{
			thisObject = thisHouse.rooms[roomNum].objects[n];

			masterObjects[numMasterObjects].roomNum = roomNum;
			masterObjects[numMasterObjects].objectNum = n;
			masterObjects[numMasterObjects].roomLink = GetRoomLinked(&thisObject);
			masterObjects[numMasterObjects].objectLink = GetObjectLinked(&thisObject);
			masterObjects[numMasterObjects].localLink = -1;
			masterObjects[numMasterObjects].theObject = thisHouse.rooms[roomNum].objects[n];

			if ((where == kCentralRoom) && (IsThisValid(roomNum, n)))
				masterObjects[numMasterObjects].hotNum = CreateActiveRects(n);
			else
				masterObjects[numMasterObjects].hotNum = -1;
			masterObjects[numMasterObjects].dynaNum = -1;

			numMasterObjects++;
		}
	}
}

//--------------------------------------------------------------  ListAllLocalObjects

void ListAllLocalObjects (void)
{
	SInt16		i, n;

	numMasterObjects = 0;
	nHotSpots = 0;

	ListOneRoomsObjects(kCentralRoom);

	if (numNeighbors > 1)
	{
		ListOneRoomsObjects(kEastRoom);
		ListOneRoomsObjects(kWestRoom);
	}

	if (numNeighbors > 3)
	{
		ListOneRoomsObjects(kNorthRoom);
		ListOneRoomsObjects(kNorthEastRoom);
		ListOneRoomsObjects(kSouthEastRoom);
		ListOneRoomsObjects(kSouthRoom);
		ListOneRoomsObjects(kSouthWestRoom);
		ListOneRoomsObjects(kNorthWestRoom);
	}

	for (i = 0; i < numMasterObjects; i++)				// correlate links with…
	{													// index into this list
		if ((masterObjects[i].roomLink != -1) && 		// if object has a link
				(masterObjects[i].objectLink != -1))
		{
			for (n = 0; n < numMasterObjects; n++)		// search for the object…
			{											// linked to in this list
				if ((masterObjects[i].roomLink == masterObjects[n].roomNum) &&
						(masterObjects[i].objectLink == masterObjects[n].objectNum))
				{
					masterObjects[i].localLink = n;		// log the index
				}
			}
		}
	}
}

//--------------------------------------------------------------  AddTempManholeRect

void AddTempManholeRect (const Rect *manHole)
{
	Rect		tempRect;

	if (numTempManholes < kMaxTempManholes)
	{
		tempRect = *manHole;
		tempRect.bottom = tempRect.top + kFloorSupportTall;
		tempManholes[numTempManholes] = tempRect;
		numTempManholes++;
	}
}

//--------------------------------------------------------------  SetObjectState

Boolean SetObjectState (SInt16 room, SInt16 object, SInt16 action, SInt16 local)
{
	Boolean		changed;

	changed = false;

	if (room < 0 || room >= thisHouse.nRooms)
		return (changed);
	if (object < 0 || object >= kMaxRoomObs)
		return (changed);

	switch (thisHouse.rooms[room].objects[object].what)
	{
		case kFloorVent:
		case kCeilingVent:
		case kFloorBlower:
		case kCeilingBlower:
		case kLeftFan:
		case kRightFan:
		case kSewerGrate:
		case kInvisBlower:
		case kGrecoVent:
		case kSewerBlower:
		case kLiftArea:
		switch (action)
		{
			case kToggle:
			newState = !thisHouse.rooms[room].objects[object].data.a.state;
			thisHouse.rooms[room].objects[object].data.a.state = newState;
			changed = true;
			break;

			case kForceOn:
			changed = (thisHouse.rooms[room].objects[object].data.a.state == false);
			newState = true;
			thisHouse.rooms[room].objects[object].data.a.state = newState;
			break;

			case kForceOff:
			changed = (thisHouse.rooms[room].objects[object].data.a.state == true);
			newState = false;
			thisHouse.rooms[room].objects[object].data.a.state = newState;
			break;
		}
		if ((changed) && (local != -1))
		{
			masterObjects[local].theObject.data.a.state = newState;
			if (room == thisRoomNumber)
				thisRoom->objects[object].data.a.state = newState;
			if (newState)
				PlayPrioritySound(kBlowerOn, kBlowerOnPriority);
			else
				PlayPrioritySound(kBlowerOff, kBlowerOffPriority);
			if (masterObjects[local].hotNum != -1)
				hotSpots[masterObjects[local].hotNum].isOn = newState;
		}
		break;

		case kTaper:
		case kCandle:
		case kStubby:
		case kTiki:
		case kBBQ:
		changed = false;	// Cannot switch on/off these
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
		changed = false;	// Cannot switch on/off these
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
		changed = (thisHouse.rooms[room].objects[object].data.c.state == true);
		newState = false;
		thisHouse.rooms[room].objects[object].data.c.state = newState;
		if ((changed) && (local != -1))
		{
			masterObjects[local].theObject.data.c.state = false;
			if (room == thisRoomNumber)
			{
				thisRoom->objects[object].data.c.state = false;
				if (masterObjects[local].hotNum != -1)
					hotSpots[masterObjects[local].hotNum].isOn = false;
			}
		}
		break;

		case kSlider:
		changed = false;
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
		changed = false;
		break;

		case kDeluxeTrans:
		switch (action)
		{
			case kToggle:
			newState = thisHouse.rooms[room].objects[object].data.d.wide & 0x0F;
			newState = !newState;
			thisHouse.rooms[room].objects[object].data.d.wide &= 0xF0;
			thisHouse.rooms[room].objects[object].data.d.wide += newState;
			changed = true;
			break;

			case kForceOn:
			changed = ((thisHouse.rooms[room].objects[object].data.d.wide & 0x0F) == 0x00);
			newState = true;
			thisHouse.rooms[room].objects[object].data.d.wide &= 0xF0;
			thisHouse.rooms[room].objects[object].data.d.wide += newState;
			break;

			case kForceOff:
			changed = ((thisHouse.rooms[room].objects[object].data.d.wide & 0x0F) != 0x00);
			newState = false;
			thisHouse.rooms[room].objects[object].data.d.wide &= 0xF0;
			thisHouse.rooms[room].objects[object].data.d.wide += newState;
			break;
		}
		if ((changed) && (local != -1))
		{
			masterObjects[local].theObject.data.d.wide =
					thisHouse.rooms[room].objects[object].data.d.wide;
			if (room == thisRoomNumber)
				thisRoom->objects[object].data.d.wide =
						thisHouse.rooms[room].objects[object].data.d.wide;
			if (masterObjects[local].hotNum != -1)
				hotSpots[masterObjects[local].hotNum].isOn = newState;
		}
		break;

		case kLightSwitch:
		case kMachineSwitch:
		case kThermostat:
		case kPowerSwitch:
		case kInvisSwitch:
		case kTrigger:
		case kLgTrigger:
		case kSoundTrigger:
		changed = false;
		break;

		case kCeilingLight:
		case kLightBulb:
		case kTableLamp:
		case kHipLamp:
		case kDecoLamp:
		case kFlourescent:
		case kTrackLight:
		case kInvisLight:
		switch (action)
		{
			case kToggle:
			newState = !thisHouse.rooms[room].objects[object].data.f.state;
			thisHouse.rooms[room].objects[object].data.f.state = newState;
			changed = true;
			break;

			case kForceOn:
			changed = (thisHouse.rooms[room].objects[object].data.f.state == false);
			newState = true;
			thisHouse.rooms[room].objects[object].data.f.state = newState;
			break;

			case kForceOff:
			changed = (thisHouse.rooms[room].objects[object].data.f.state == true);
			newState = false;
			thisHouse.rooms[room].objects[object].data.f.state = newState;
			break;
		}
		if ((changed) && (local != -1))
		{
			masterObjects[local].theObject.data.f.state = newState;
			if (room == thisRoomNumber)
				thisRoom->objects[object].data.f.state = newState;
		}
		break;

		case kGuitar:		// really no point to change this state
		changed = false;
		break;

		case kStereo:
		newState = !isPlayMusicGame;
		isPlayMusicGame = newState;
		changed = true;
		break;

		case kShredder:
		case kToaster:
		case kMacPlus:
		case kTV:
		case kCoffee:
		case kOutlet:
		case kVCR:
		case kMicrowave:
		switch (action)
		{
			case kToggle:
			newState = !thisHouse.rooms[room].objects[object].data.g.state;
			thisHouse.rooms[room].objects[object].data.g.state = newState;
			changed = true;
			break;

			case kForceOn:
			changed = (thisHouse.rooms[room].objects[object].data.g.state == false);
			newState = true;
			thisHouse.rooms[room].objects[object].data.g.state = newState;
			break;

			case kForceOff:
			changed = (thisHouse.rooms[room].objects[object].data.g.state == true);
			newState = false;
			thisHouse.rooms[room].objects[object].data.g.state = newState;
			break;
		}
		if ((changed) && (local != -1))
		{
			masterObjects[local].theObject.data.g.state = newState;
			if (room == thisRoomNumber)
			{
				thisRoom->objects[object].data.g.state = newState;
				if (thisHouse.rooms[room].objects[object].what == kShredder)
					hotSpots[masterObjects[local].hotNum].isOn = newState;
			}
		}
		break;

		case kCinderBlock:
		case kFlowerBox:
		case kCDs:
		case kCustomPict:
		changed = false;
		break;

		case kBalloon:
		case kCopterLf:
		case kCopterRt:
		case kDartLf:
		case kDartRt:
		case kBall:
		case kDrip:
		case kFish:
		switch (action)
		{
			case kToggle:
			newState = !thisHouse.rooms[room].objects[object].data.h.state;
			thisHouse.rooms[room].objects[object].data.h.state = newState;
			changed = true;
			break;

			case kForceOn:
			changed = (thisHouse.rooms[room].objects[object].data.h.state == false);
			newState = true;
			thisHouse.rooms[room].objects[object].data.h.state = newState;
			break;

			case kForceOff:
			changed = (thisHouse.rooms[room].objects[object].data.h.state == true);
			newState = false;
			thisHouse.rooms[room].objects[object].data.h.state = newState;
			break;
		}
		if ((changed) && (local != -1))
		{
			masterObjects[local].theObject.data.h.state = newState;
			if (room == thisRoomNumber)
				thisRoom->objects[object].data.h.state = newState;
		}
		break;

		case kCobweb:
		changed = false;
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
		changed = false;
		break;
	}

	return (changed);
}

//--------------------------------------------------------------  GetObjectState

Boolean GetObjectState (SInt16 room, SInt16 object)
{
	Boolean		theState;

	theState = true;

	if (room < 0 || room >= thisHouse.nRooms)
		return (theState);
	if (object < 0 || object >= kMaxRoomObs)
		return (theState);

	switch (thisHouse.rooms[room].objects[object].what)
	{
		case kFloorVent:
		case kCeilingVent:
		case kFloorBlower:
		case kCeilingBlower:
		case kLeftFan:
		case kRightFan:
		case kSewerGrate:
		case kInvisBlower:
		case kGrecoVent:
		case kSewerBlower:
		case kLiftArea:
		theState = thisHouse.rooms[room].objects[object].data.a.state;
		break;

		case kTaper:
		case kCandle:
		case kStubby:
		case kTiki:
		case kBBQ:
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
		theState = thisHouse.rooms[room].objects[object].data.c.state;
		break;

		case kSlider:
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
		break;

		case kDeluxeTrans:
		theState = thisHouse.rooms[room].objects[object].data.d.wide & 0x0F;
		break;

		case kLightSwitch:
		case kMachineSwitch:
		case kThermostat:
		case kPowerSwitch:
		case kInvisSwitch:
		case kTrigger:
		case kLgTrigger:
		case kSoundTrigger:
		break;

		case kCeilingLight:
		case kLightBulb:
		case kTableLamp:
		case kHipLamp:
		case kDecoLamp:
		case kFlourescent:
		case kTrackLight:
		case kInvisLight:
		theState = thisHouse.rooms[room].objects[object].data.f.state;
		break;

		case kStereo:
		theState = isPlayMusicGame;
		break;

		case kShredder:
		case kToaster:
		case kMacPlus:
		case kGuitar:
		case kTV:
		case kCoffee:
		case kOutlet:
		case kVCR:
		case kMicrowave:
		theState = thisHouse.rooms[room].objects[object].data.g.state;
		break;

		case kCinderBlock:
		case kFlowerBox:
		case kCDs:
		case kCustomPict:
		break;

		case kBalloon:
		case kCopterLf:
		case kCopterRt:
		case kDartLf:
		case kDartRt:
		case kBall:
		case kDrip:
		case kFish:
		theState = thisHouse.rooms[room].objects[object].data.h.state;
		break;

		case kCobweb:
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
		break;
	}

	return (theState);
}

//--------------------------------------------------------------  SendObjectToBack

#ifndef COMPILEDEMO
void BringSendFrontBack (HWND ownerWindow, Boolean bringFront)
{
	objectType	savedObject;
	SInt16		numLinks, i;
	SInt16		srcRoom, srcObj;
	SInt16		sorting[kMaxRoomObs];
	Byte		sorted[kMaxRoomObs];

	if (bringFront)							// No need to bring to front…
	{										// or send to back if the object…
		if (objActive == (kMaxRoomObs - 1))	// in question is already front-
			return;							// most or backmost.
	}
	else
	{
		if (objActive == 0)
			return;
	}

	CopyThisRoomToRoom();					// Any changes to room written…
											// back to the house handle.
	numLinks = CountHouseLinks();			// Determine space needed for all links.
	if (numLinks != 0)						// Create links list of ALL house links.
	{
		linksList = NULL;
		linksList = (linksPtr)calloc(numLinks, sizeof(*linksList));
		if (linksList == NULL)
		{
			YellowAlert(ownerWindow, kYellowCantOrderLinks, -1);
			return;
		}
		GenerateLinksList();				// Fill in links list with src/dest…
	}										// data on objects and room numbers.

	for (i = 0; i < kMaxRoomObs; i++)		// Set up an ordered array.
		sorting[i] = i;

	savedObject = thisHouse.rooms[thisRoomNumber].objects[objActive];

	if (bringFront)
	{
		for (i = objActive; i < kMaxRoomObs - 1; i++)
		{									// Pull all objects down to fill hole.
			thisHouse.rooms[thisRoomNumber].objects[i] =
					thisHouse.rooms[thisRoomNumber].objects[i + 1];
			sorting[i] = sorting[i + 1];
			SpinCursor(2);
		}
											// Insert object at end of array.
		thisHouse.rooms[thisRoomNumber].objects[kMaxRoomObs - 1] = savedObject;
		sorting[kMaxRoomObs - 1] = objActive;
	}
	else
	{
		for (i = objActive; i > 0; i--)
		{									// Move all objects up to fill hole.
			thisHouse.rooms[thisRoomNumber].objects[i] =
					thisHouse.rooms[thisRoomNumber].objects[i - 1];
			sorting[i] = sorting[i - 1];
			SpinCursor(2);
		}
											// Insert object at beginning of array.
		thisHouse.rooms[thisRoomNumber].objects[0] = savedObject;
		sorting[0] = objActive;
	}

	for (i = 0; i < kMaxRoomObs; i++)		// Set up retro-ordered array.
		sorted[sorting[i]] = (Byte)i;

	for (i = 0; i < numLinks; i++)			// Walk links list in order to assign…
	{										// corrected links to objects moved.
		if (linksList[i].destRoom == thisRoomNumber)
		{									// Does link point to room we re-ordered?
			srcRoom = linksList[i].srcRoom;	// Room where-which an object is linked from.
			if (srcRoom == thisRoomNumber)	// Handle special case for local links.
				srcObj = sorted[linksList[i].srcObj];
			else
				srcObj = linksList[i].srcObj;

			switch (thisHouse.rooms[srcRoom].objects[srcObj].what)
			{
				case kLightSwitch:
				case kMachineSwitch:
				case kThermostat:
				case kPowerSwitch:
				case kKnifeSwitch:
				case kInvisSwitch:
				case kTrigger:
				case kLgTrigger:
				thisHouse.rooms[srcRoom].objects[srcObj].data.e.who =
						sorted[linksList[i].destObj];
				break;

				default:
				thisHouse.rooms[srcRoom].objects[srcObj].data.d.who =
						sorted[linksList[i].destObj];
				break;
			}
		}
	}

	free(linksList);

	ForceThisRoom(thisRoomNumber);

	fileDirty = true;
	UpdateMenus(false);
	Mac_InvalWindowRect(mainWindow, &mainWindowRect);
	DeselectObject();
	GetThisRoomsObjRects();
	ReadyBackground(thisRoom->background, thisRoom->tiles);
	DrawThisRoomsObjects();
	GenerateRetroLinks();

	InitCursor();
}
#endif

