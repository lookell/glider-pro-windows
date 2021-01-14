//============================================================================
//----------------------------------------------------------------------------
//                                 Objects.c
//----------------------------------------------------------------------------
//============================================================================

#include "Objects.h"

#include "House.h"
#include "HouseIO.h"
#include "Link.h"
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
Rect g_blowerSrcRect;
HDC g_blowerSrcMap;
HDC g_blowerMaskMap;
Rect g_flame[kNumCandleFlames];
Rect g_tikiFlame[kNumTikiFlames];
Rect g_coals[kNumBBQCoals];

// Furniture
Rect g_furnitureSrcRect;
HDC g_furnitureSrcMap;
HDC g_furnitureMaskMap;
Rect g_tableSrc;
Rect g_shelfSrc;
Rect g_hingeSrc;
Rect g_handleSrc;
Rect g_knobSrc;
Rect g_leftFootSrc;
Rect g_rightFootSrc;
Rect g_deckSrc;

// Bonuses
Rect g_bonusSrcRect;
HDC g_bonusSrcMap;
HDC g_bonusMaskMap;
Rect g_pointsSrcRect;
HDC g_pointsSrcMap;
HDC g_pointsMaskMap;
Rect g_starSrc[6];
Rect g_sparkleSrc[kNumSparkleModes];
Rect g_digits[11];
Rect g_pendulumSrc[3];
Rect g_greaseSrcRt[4];
Rect g_greaseSrcLf[4];

// Transport
Rect g_transSrcRect;
HDC g_transSrcMap;
HDC g_transMaskMap;

// Switches
Rect g_switchSrcRect;
HDC g_switchSrcMap;
Rect g_lightSwitchSrc[2];
Rect g_machineSwitchSrc[2];
Rect g_thermostatSrc[2];
Rect g_powerSrc[2];
Rect g_knifeSwitchSrc[2];

// Lights
Rect g_lightSrcRect;
HDC g_lightSrcMap;
HDC g_lightMaskMap;
Rect g_flourescentSrc1;
Rect g_flourescentSrc2;
Rect g_trackLightSrc[kNumTrackLights];

// Appliances
Rect g_applianceSrcRect;
HDC g_applianceSrcMap;
HDC g_applianceMaskMap;
Rect g_toastSrcRect;
HDC g_toastSrcMap;
HDC g_toastMaskMap;
Rect g_shredSrcRect;
HDC g_shredSrcMap;
HDC g_shredMaskMap;
Rect g_plusScreen1;
Rect g_plusScreen2;
Rect g_tvScreen1;
Rect g_tvScreen2;
Rect g_coffeeLight1;
Rect g_coffeeLight2;
Rect g_vcrTime1;
Rect g_vcrTime2;
Rect g_stereoLight1;
Rect g_stereoLight2;
Rect g_microOn;
Rect g_microOff;
Rect g_outletSrc[kNumOutletPicts];

// Enemies
Rect g_balloonSrcRect;
HDC g_balloonSrcMap;
HDC g_balloonMaskMap;
Rect g_balloonSrc[kNumBalloonFrames];
Rect g_copterSrcRect;
HDC g_copterSrcMap;
HDC g_copterMaskMap;
Rect g_copterSrc[kNumCopterFrames];
Rect g_dartSrcRect;
HDC g_dartSrcMap;
HDC g_dartMaskMap;
Rect g_dartSrc[kNumDartFrames];
Rect g_ballSrcRect;
HDC g_ballSrcMap;
HDC g_ballMaskMap;
Rect g_ballSrc[kNumBallFrames];
Rect g_dripSrcRect;
HDC g_dripSrcMap;
HDC g_dripMaskMap;
Rect g_dripSrc[kNumDripFrames];
Rect g_fishSrcRect;
HDC g_fishSrcMap;
HDC g_fishMaskMap;
Rect g_fishSrc[kNumFishFrames];
Rect g_enemySrcRect;
HDC g_enemySrcMap;
HDC g_enemyMaskMap;

// Clutter
HDC g_clutterSrcMap;
HDC g_clutterMaskMap;
Rect g_clutterSrcRect;
Rect g_flowerSrc[kNumFlowers];

Rect g_srcRects[kNumSrcRects];
Rect g_tempManholes[kMaxTempManholes];
savedType g_savedMaps[kMaxSavedMaps];
objDataType g_masterObjects[kMaxMasterObjects];
hotObject g_hotSpots[kMaxHotSpots];
SInt16 g_nHotSpots;
SInt16 g_numMasterObjects;
SInt16 g_numTempManholes;
SInt16 g_tvWithMovieNumber;
Boolean g_newState;

//==============================================================  Functions
//--------------------------------------------------------------  IsThisValid

Boolean IsThisValid (SInt16 where, SInt16 who)
{
	Boolean itsGood;

	if (where < 0 || where >= g_thisHouse.nRooms)
		return (false);
	if (who < 0 || who >= kMaxRoomObs)
		return (false);

	itsGood = true;

	switch (g_thisHouse.rooms[where].objects[who].what)
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
		itsGood = g_thisHouse.rooms[where].objects[who].data.c.state;
		break;
	}

	return (itsGood);
}

//--------------------------------------------------------------  GetRoomLinked

SInt16 GetRoomLinked (const objectType *who)
{
	SInt16 compoundRoomNumber, whereLinked;
	SInt16 floor, suite;

	switch (who->what)
	{
		case kMailboxLf:
		case kMailboxRt:
		case kFloorTrans:
		case kCeilingTrans:
		case kInvisTrans:
		case kDeluxeTrans:
		compoundRoomNumber = who->data.d.where;
		if (compoundRoomNumber != -1)  // is object linked
		{
			ExtractFloorSuite(compoundRoomNumber, &floor, &suite);
			whereLinked = GetRoomNumber(floor, suite);
		}
		else
			whereLinked = -1;  // not linked
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
		if (compoundRoomNumber != -1)  // is object linked
		{
			ExtractFloorSuite(compoundRoomNumber, &floor, &suite);
			whereLinked = GetRoomNumber(floor, suite);
		}
		else
			whereLinked = -1;  // not linked
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
	SInt16 whoLinked;

	switch (who->what)
	{
		case kMailboxLf:
		case kMailboxRt:
		case kFloorTrans:
		case kCeilingTrans:
		case kInvisTrans:
		case kDeluxeTrans:
		if (who->data.d.who != 255)  // is it linked?
			whoLinked = (SInt16)who->data.d.who;
		else
			whoLinked = -1;  // object not linked
		break;

		case kLightSwitch:
		case kMachineSwitch:
		case kThermostat:
		case kPowerSwitch:
		case kKnifeSwitch:
		case kInvisSwitch:
		case kTrigger:
		case kLgTrigger:
		if (who->data.e.who != 255)  // is it linked?
			whoLinked = (SInt16)who->data.e.who;
		else
			whoLinked = -1;  // object not linked
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
	Boolean itIs;

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
	Boolean itIs;

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
	objectType thisObject;
	SInt16 roomNum, n;

	if (where < 0 || where >= ARRAYSIZE(g_localNumbers))
		return;

	roomNum = g_localNumbers[where];
	if (roomNum < 0 || roomNum >= g_thisHouse.nRooms)
		return;

	for (n = 0; n < kMaxRoomObs; n++)
	{
		if (g_numMasterObjects < kMaxMasterObjects)
		{
			thisObject = g_thisHouse.rooms[roomNum].objects[n];

			g_masterObjects[g_numMasterObjects].roomNum = roomNum;
			g_masterObjects[g_numMasterObjects].objectNum = n;
			g_masterObjects[g_numMasterObjects].roomLink = GetRoomLinked(&thisObject);
			g_masterObjects[g_numMasterObjects].objectLink = GetObjectLinked(&thisObject);
			g_masterObjects[g_numMasterObjects].localLink = -1;
			g_masterObjects[g_numMasterObjects].theObject = g_thisHouse.rooms[roomNum].objects[n];

			if ((where == kCentralRoom) && (IsThisValid(roomNum, n)))
				g_masterObjects[g_numMasterObjects].hotNum = CreateActiveRects(n);
			else
				g_masterObjects[g_numMasterObjects].hotNum = -1;
			g_masterObjects[g_numMasterObjects].dynaNum = -1;

			g_numMasterObjects++;
		}
	}
}

//--------------------------------------------------------------  ListAllLocalObjects

void ListAllLocalObjects (void)
{
	SInt16 i, n;

	g_numMasterObjects = 0;
	g_nHotSpots = 0;

	ListOneRoomsObjects(kCentralRoom);

	if (g_numNeighbors > 1)
	{
		ListOneRoomsObjects(kEastRoom);
		ListOneRoomsObjects(kWestRoom);
	}

	if (g_numNeighbors > 3)
	{
		ListOneRoomsObjects(kNorthRoom);
		ListOneRoomsObjects(kNorthEastRoom);
		ListOneRoomsObjects(kSouthEastRoom);
		ListOneRoomsObjects(kSouthRoom);
		ListOneRoomsObjects(kSouthWestRoom);
		ListOneRoomsObjects(kNorthWestRoom);
	}

	// correlate link with index into this list
	for (i = 0; i < g_numMasterObjects; i++)
	{
		// if object has a link
		if ((g_masterObjects[i].roomLink != -1) &&
				(g_masterObjects[i].objectLink != -1))
		{
			// search for the object linked to in this list
			for (n = 0; n < g_numMasterObjects; n++)
			{
				if ((g_masterObjects[i].roomLink == g_masterObjects[n].roomNum) &&
						(g_masterObjects[i].objectLink == g_masterObjects[n].objectNum))
				{
					// log the index
					g_masterObjects[i].localLink = n;
				}
			}
		}
	}
}

//--------------------------------------------------------------  AddTempManholeRect

void AddTempManholeRect (const Rect *manHole)
{
	Rect tempRect;

	if (g_numTempManholes < kMaxTempManholes)
	{
		tempRect = *manHole;
		tempRect.bottom = tempRect.top + kFloorSupportTall;
		g_tempManholes[g_numTempManholes] = tempRect;
		g_numTempManholes++;
	}
}

//--------------------------------------------------------------  SetObjectState

Boolean SetObjectState (SInt16 room, SInt16 object, SInt16 action, SInt16 local)
{
	Boolean changed;

	changed = false;

	if (room < 0 || room >= g_thisHouse.nRooms)
		return (changed);
	if (object < 0 || object >= kMaxRoomObs)
		return (changed);

	switch (g_thisHouse.rooms[room].objects[object].what)
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
			g_newState = !g_thisHouse.rooms[room].objects[object].data.a.state;
			g_thisHouse.rooms[room].objects[object].data.a.state = g_newState;
			changed = true;
			break;

			case kForceOn:
			changed = (g_thisHouse.rooms[room].objects[object].data.a.state == false);
			g_newState = true;
			g_thisHouse.rooms[room].objects[object].data.a.state = g_newState;
			break;

			case kForceOff:
			changed = (g_thisHouse.rooms[room].objects[object].data.a.state != false);
			g_newState = false;
			g_thisHouse.rooms[room].objects[object].data.a.state = g_newState;
			break;
		}
		if ((changed) && (local != -1))
		{
			g_masterObjects[local].theObject.data.a.state = g_newState;
			if (room == g_thisRoomNumber)
				g_thisRoom->objects[object].data.a.state = g_newState;
			if (g_newState)
				PlayPrioritySound(kBlowerOn, kBlowerOnPriority);
			else
				PlayPrioritySound(kBlowerOff, kBlowerOffPriority);
			if (g_masterObjects[local].hotNum != -1)
				g_hotSpots[g_masterObjects[local].hotNum].isOn = g_newState;
		}
		break;

		case kTaper:
		case kCandle:
		case kStubby:
		case kTiki:
		case kBBQ:
		changed = false;  // Cannot switch on/off these
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
		changed = false;  // Cannot switch on/off these
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
		changed = (g_thisHouse.rooms[room].objects[object].data.c.state != false);
		g_newState = false;
		g_thisHouse.rooms[room].objects[object].data.c.state = g_newState;
		if ((changed) && (local != -1))
		{
			g_masterObjects[local].theObject.data.c.state = false;
			if (room == g_thisRoomNumber)
			{
				g_thisRoom->objects[object].data.c.state = false;
				if (g_masterObjects[local].hotNum != -1)
					g_hotSpots[g_masterObjects[local].hotNum].isOn = false;
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
			g_newState = g_thisHouse.rooms[room].objects[object].data.d.wide & 0x0F;
			g_newState = !g_newState;
			g_thisHouse.rooms[room].objects[object].data.d.wide &= 0xF0;
			g_thisHouse.rooms[room].objects[object].data.d.wide += g_newState;
			changed = true;
			break;

			case kForceOn:
			changed = ((g_thisHouse.rooms[room].objects[object].data.d.wide & 0x0F) == 0x00);
			g_newState = true;
			g_thisHouse.rooms[room].objects[object].data.d.wide &= 0xF0;
			g_thisHouse.rooms[room].objects[object].data.d.wide += g_newState;
			break;

			case kForceOff:
			changed = ((g_thisHouse.rooms[room].objects[object].data.d.wide & 0x0F) != 0x00);
			g_newState = false;
			g_thisHouse.rooms[room].objects[object].data.d.wide &= 0xF0;
			g_thisHouse.rooms[room].objects[object].data.d.wide += g_newState;
			break;
		}
		if ((changed) && (local != -1))
		{
			g_masterObjects[local].theObject.data.d.wide =
					g_thisHouse.rooms[room].objects[object].data.d.wide;
			if (room == g_thisRoomNumber)
				g_thisRoom->objects[object].data.d.wide =
						g_thisHouse.rooms[room].objects[object].data.d.wide;
			if (g_masterObjects[local].hotNum != -1)
				g_hotSpots[g_masterObjects[local].hotNum].isOn = g_newState;
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
			g_newState = !g_thisHouse.rooms[room].objects[object].data.f.state;
			g_thisHouse.rooms[room].objects[object].data.f.state = g_newState;
			changed = true;
			break;

			case kForceOn:
			changed = (g_thisHouse.rooms[room].objects[object].data.f.state == false);
			g_newState = true;
			g_thisHouse.rooms[room].objects[object].data.f.state = g_newState;
			break;

			case kForceOff:
			changed = (g_thisHouse.rooms[room].objects[object].data.f.state != false);
			g_newState = false;
			g_thisHouse.rooms[room].objects[object].data.f.state = g_newState;
			break;
		}
		if ((changed) && (local != -1))
		{
			g_masterObjects[local].theObject.data.f.state = g_newState;
			if (room == g_thisRoomNumber)
				g_thisRoom->objects[object].data.f.state = g_newState;
		}
		break;

		case kGuitar:  // really no point to change this state
		changed = false;
		break;

		case kStereo:
		g_newState = !g_isPlayMusicGame;
		g_isPlayMusicGame = g_newState;
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
			g_newState = !g_thisHouse.rooms[room].objects[object].data.g.state;
			g_thisHouse.rooms[room].objects[object].data.g.state = g_newState;
			changed = true;
			break;

			case kForceOn:
			changed = (g_thisHouse.rooms[room].objects[object].data.g.state == false);
			g_newState = true;
			g_thisHouse.rooms[room].objects[object].data.g.state = g_newState;
			break;

			case kForceOff:
			changed = (g_thisHouse.rooms[room].objects[object].data.g.state != false);
			g_newState = false;
			g_thisHouse.rooms[room].objects[object].data.g.state = g_newState;
			break;
		}
		if ((changed) && (local != -1))
		{
			g_masterObjects[local].theObject.data.g.state = g_newState;
			if (room == g_thisRoomNumber)
			{
				g_thisRoom->objects[object].data.g.state = g_newState;
				if (g_thisHouse.rooms[room].objects[object].what == kShredder)
					g_hotSpots[g_masterObjects[local].hotNum].isOn = g_newState;
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
			g_newState = !g_thisHouse.rooms[room].objects[object].data.h.state;
			g_thisHouse.rooms[room].objects[object].data.h.state = g_newState;
			changed = true;
			break;

			case kForceOn:
			changed = (g_thisHouse.rooms[room].objects[object].data.h.state == false);
			g_newState = true;
			g_thisHouse.rooms[room].objects[object].data.h.state = g_newState;
			break;

			case kForceOff:
			changed = (g_thisHouse.rooms[room].objects[object].data.h.state != false);
			g_newState = false;
			g_thisHouse.rooms[room].objects[object].data.h.state = g_newState;
			break;
		}
		if ((changed) && (local != -1))
		{
			g_masterObjects[local].theObject.data.h.state = g_newState;
			if (room == g_thisRoomNumber)
				g_thisRoom->objects[object].data.h.state = g_newState;
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
	Boolean theState;

	theState = true;

	if (room < 0 || room >= g_thisHouse.nRooms)
		return (theState);
	if (object < 0 || object >= kMaxRoomObs)
		return (theState);

	switch (g_thisHouse.rooms[room].objects[object].what)
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
		theState = g_thisHouse.rooms[room].objects[object].data.a.state;
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
		theState = g_thisHouse.rooms[room].objects[object].data.c.state;
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
		theState = g_thisHouse.rooms[room].objects[object].data.d.wide & 0x0F;
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
		theState = g_thisHouse.rooms[room].objects[object].data.f.state;
		break;

		case kStereo:
		theState = g_isPlayMusicGame;
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
		theState = g_thisHouse.rooms[room].objects[object].data.g.state;
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
		theState = g_thisHouse.rooms[room].objects[object].data.h.state;
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

void BringSendFrontBack (HWND ownerWindow, Boolean bringFront)
{
	objectType savedObject;
	SInt16 numLinks, i;
	SInt16 srcRoom, srcObj;
	SInt16 sorting[kMaxRoomObs];
	Byte sorted[kMaxRoomObs];

	// No need to bring to front or send to back if the object
	// in question is already frontmost or backmost.
	if (bringFront)
	{
		if (g_objActive == (kMaxRoomObs - 1))
			return;
	}
	else
	{
		if (g_objActive == 0)
			return;
	}

	// Any changes to room written back to the house handle.
	CopyThisRoomToRoom();
	// Determine space needed for all links.
	numLinks = CountHouseLinks();
	// Create links list of ALL house links.
	if (numLinks != 0)
	{
		g_linksList = NULL;
		g_linksList = (linksPtr)calloc(numLinks, sizeof(*g_linksList));
		if (g_linksList == NULL)
		{
			YellowAlert(ownerWindow, kYellowCantOrderLinks, -1);
			return;
		}
		// Fill in links list with src/dest data on objects and room numbers.
		GenerateLinksList();
	}

	// Set up an ordered array.
	for (i = 0; i < kMaxRoomObs; i++)
		sorting[i] = i;

	savedObject = g_thisHouse.rooms[g_thisRoomNumber].objects[g_objActive];

	if (bringFront)
	{
		// Pull all objects down to fill hole.
		for (i = g_objActive; i < kMaxRoomObs - 1; i++)
		{
			g_thisHouse.rooms[g_thisRoomNumber].objects[i] =
					g_thisHouse.rooms[g_thisRoomNumber].objects[i + 1];
			sorting[i] = sorting[i + 1];
		}
		// Insert object at end of array.
		g_thisHouse.rooms[g_thisRoomNumber].objects[kMaxRoomObs - 1] = savedObject;
		sorting[kMaxRoomObs - 1] = g_objActive;
	}
	else
	{
		// Move all objects up to fill hole.
		for (i = g_objActive; i > 0; i--)
		{
			g_thisHouse.rooms[g_thisRoomNumber].objects[i] =
					g_thisHouse.rooms[g_thisRoomNumber].objects[i - 1];
			sorting[i] = sorting[i - 1];
		}
		// Insert object at beginning of array.
		g_thisHouse.rooms[g_thisRoomNumber].objects[0] = savedObject;
		sorting[0] = g_objActive;
	}

	// Set up retro-ordered array.
	for (i = 0; i < kMaxRoomObs; i++)
		sorted[sorting[i]] = (Byte)i;

	// Walk links list in order to assign corrected links to objects moved.
	for (i = 0; i < numLinks; i++)
	{
		// Does link point to room we re-ordered?
		if (g_linksList[i].destRoom == g_thisRoomNumber)
		{
			// Room where-which object an object is linked from.
			srcRoom = g_linksList[i].srcRoom;
			// Handle special case for local links.
			if (srcRoom == g_thisRoomNumber)
				srcObj = sorted[g_linksList[i].srcObj];
			else
				srcObj = g_linksList[i].srcObj;

			switch (g_thisHouse.rooms[srcRoom].objects[srcObj].what)
			{
				case kLightSwitch:
				case kMachineSwitch:
				case kThermostat:
				case kPowerSwitch:
				case kKnifeSwitch:
				case kInvisSwitch:
				case kTrigger:
				case kLgTrigger:
				g_thisHouse.rooms[srcRoom].objects[srcObj].data.e.who =
						sorted[g_linksList[i].destObj];
				break;

				default:
				g_thisHouse.rooms[srcRoom].objects[srcObj].data.d.who =
						sorted[g_linksList[i].destObj];
				break;
			}
		}
	}

	free(g_linksList);

	ForceThisRoom(g_thisRoomNumber);

	g_fileDirty = true;
	UpdateMenus(false);
	InvalidateRect(g_mainWindow, NULL, TRUE);
	DeselectObject();
	GetThisRoomsObjRects();
	ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
	DrawThisRoomsObjects();
	GenerateRetroLinks();
}
