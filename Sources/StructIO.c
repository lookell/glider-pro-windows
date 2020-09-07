#include "StructIO.h"

#include <stdlib.h>

#ifndef ARRAYSIZE
#define ARRAYSIZE(A) (sizeof(A)/sizeof((A)[0]))
#endif
#define FORWARD_FALSE(x) do { if ((x) == 0) return 0; } while (0)

static int ReadPoint(byteio *reader, Point *data)
{
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->v));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->h));
	return 1;
}

static int WritePoint(byteio *writer, const Point *data)
{
	FORWARD_FALSE(byteio_write_be_i16(writer, data->v));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->h));
	return 1;
}

static int ReadRect(byteio *reader, Rect *data)
{
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->top));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->left));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->bottom));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->right));
	return 1;
}

static int WriteRect(byteio *writer, const Rect *data)
{
	FORWARD_FALSE(byteio_write_be_i16(writer, data->top));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->left));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->bottom));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->right));
	return 1;
}

static int ReadStr15(byteio *reader, Str15 *data)
{
	FORWARD_FALSE(byteio_read(reader, data, sizeof(*data)));
	return ((*data)[0] < sizeof(*data));
}

static int WriteStr15(byteio *writer, const Str15 *data)
{
	FORWARD_FALSE(byteio_write(writer, data, sizeof(*data)));
	return 1;
}

static int ReadStr27(byteio *reader, Str27 *data)
{
	FORWARD_FALSE(byteio_read(reader, data, sizeof(*data)));
	return ((*data)[0] < sizeof(*data));
}

static int WriteStr27(byteio *writer, const Str27 *data)
{
	FORWARD_FALSE(byteio_write(writer, data, sizeof(*data)));
	return 1;
}

static int ReadStr31(byteio *reader, Str31 *data)
{
	FORWARD_FALSE(byteio_read(reader, data, sizeof(*data)));
	return ((*data)[0] < sizeof(*data));
}

static int WriteStr31(byteio *writer, const Str31 *data)
{
	FORWARD_FALSE(byteio_write(writer, data, sizeof(*data)));
	return 1;
}

static int ReadStr32(byteio *reader, Str32 *data)
{
	FORWARD_FALSE(byteio_read(reader, data, sizeof(*data)));
	return ((*data)[0] < sizeof(*data));
}

static int WriteStr32(byteio *writer, const Str32 *data)
{
	FORWARD_FALSE(byteio_write(writer, data, sizeof(*data)));
	return 1;
}

static int ReadStr63(byteio *reader, Str63 *data)
{
	FORWARD_FALSE(byteio_read(reader, data, sizeof(*data)));
	return ((*data)[0] < sizeof(*data));
}

static int WriteStr63(byteio *writer, const Str63 *data)
{
	FORWARD_FALSE(byteio_write(writer, data, sizeof(*data)));
	return 1;
}

static int ReadStr255(byteio *reader, Str255 *data)
{
	FORWARD_FALSE(byteio_read(reader, data, sizeof(*data)));
	return ((*data)[0] < sizeof(*data));
}

static int WriteStr255(byteio *writer, const Str255 *data)
{
	FORWARD_FALSE(byteio_write(writer, data, sizeof(*data)));
	return 1;
}

static int ReadFSSpec(byteio *reader, FSSpec *data)
{
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->vRefNum));
	FORWARD_FALSE(byteio_read_be_i32(reader, &data->parID));
	FORWARD_FALSE(ReadStr63(reader, &data->name));
	return 1;
}

static int WriteFSSpec(byteio *writer, const FSSpec *data)
{
	FORWARD_FALSE(byteio_write_be_i16(writer, data->vRefNum));
	FORWARD_FALSE(byteio_write_be_i32(writer, data->parID));
	FORWARD_FALSE(WriteStr63(writer, &data->name));
	return 1;
}

int ReadPrefsInfo(byteio *reader, prefsInfo *data)
{
	FORWARD_FALSE(ReadStr32(reader, &data->wasDefaultName));
	FORWARD_FALSE(byteio_read_be_u16(reader, &data->wasViewportWidth));
	FORWARD_FALSE(byteio_read_be_u16(reader, &data->wasViewportHeight));
	FORWARD_FALSE(byteio_read(reader, &data->unusedBytes, sizeof(data->unusedBytes)));
	FORWARD_FALSE(ReadStr15(reader, &data->wasHighName));
	FORWARD_FALSE(ReadStr31(reader, &data->wasHighBanner));
	FORWARD_FALSE(byteio_read_be_i32(reader, &data->wasLeftKeyOne));
	FORWARD_FALSE(byteio_read_be_i32(reader, &data->wasRightKeyOne));
	FORWARD_FALSE(byteio_read_be_i32(reader, &data->wasBattKeyOne));
	FORWARD_FALSE(byteio_read_be_i32(reader, &data->wasBandKeyOne));
	FORWARD_FALSE(byteio_read_be_i32(reader, &data->wasLeftKeyTwo));
	FORWARD_FALSE(byteio_read_be_i32(reader, &data->wasRightKeyTwo));
	FORWARD_FALSE(byteio_read_be_i32(reader, &data->wasBattKeyTwo));
	FORWARD_FALSE(byteio_read_be_i32(reader, &data->wasBandKeyTwo));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->wasVolume));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->prefVersion));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->wasMaxFiles));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->wasEditH));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->wasEditV));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->wasMapH));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->wasMapV));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->wasMapWide));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->wasMapHigh));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->wasToolsH));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->wasToolsV));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->wasLinkH));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->wasLinkV));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->wasCoordH));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->wasCoordV));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->isMapLeft));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->isMapTop));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->wasNumNeighbors));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->wasToolGroup));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->wasFloor));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->wasSuite));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->wasMusicOn));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->wasAutoEdit));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->wasDoColorFade));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->wasMapOpen));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->wasToolsOpen));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->wasCoordOpen));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->wasQuickTrans));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->wasIdleMusic));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->wasGameMusic));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->wasEscPauseKey));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->wasDoAutoDemo));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->wasScreen2));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->wasHouseChecks));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->wasPrettyMap));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->wasBitchDialogs));
	return 1;
}

int WritePrefsInfo(byteio *writer, const prefsInfo *data)
{
	FORWARD_FALSE(WriteStr32(writer, &data->wasDefaultName));
	FORWARD_FALSE(byteio_write_be_u16(writer, data->wasViewportWidth));
	FORWARD_FALSE(byteio_write_be_u16(writer, data->wasViewportHeight));
	FORWARD_FALSE(byteio_write(writer, &data->unusedBytes, sizeof(data->unusedBytes)));
	FORWARD_FALSE(WriteStr15(writer, &data->wasHighName));
	FORWARD_FALSE(WriteStr31(writer, &data->wasHighBanner));
	FORWARD_FALSE(byteio_write_be_i32(writer, data->wasLeftKeyOne));
	FORWARD_FALSE(byteio_write_be_i32(writer, data->wasRightKeyOne));
	FORWARD_FALSE(byteio_write_be_i32(writer, data->wasBattKeyOne));
	FORWARD_FALSE(byteio_write_be_i32(writer, data->wasBandKeyOne));
	FORWARD_FALSE(byteio_write_be_i32(writer, data->wasLeftKeyTwo));
	FORWARD_FALSE(byteio_write_be_i32(writer, data->wasRightKeyTwo));
	FORWARD_FALSE(byteio_write_be_i32(writer, data->wasBattKeyTwo));
	FORWARD_FALSE(byteio_write_be_i32(writer, data->wasBandKeyTwo));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->wasVolume));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->prefVersion));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->wasMaxFiles));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->wasEditH));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->wasEditV));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->wasMapH));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->wasMapV));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->wasMapWide));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->wasMapHigh));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->wasToolsH));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->wasToolsV));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->wasLinkH));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->wasLinkV));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->wasCoordH));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->wasCoordV));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->isMapLeft));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->isMapTop));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->wasNumNeighbors));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->wasToolGroup));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->wasFloor));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->wasSuite));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->wasMusicOn));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->wasAutoEdit));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->wasDoColorFade));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->wasMapOpen));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->wasToolsOpen));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->wasCoordOpen));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->wasQuickTrans));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->wasIdleMusic));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->wasGameMusic));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->wasEscPauseKey));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->wasDoAutoDemo));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->wasScreen2));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->wasHouseChecks));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->wasPrettyMap));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->wasBitchDialogs));
	return 1;
}

static int ReadBlowerType(byteio *reader, blowerType *data)
{
	FORWARD_FALSE(ReadPoint(reader, &data->topLeft));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->distance));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->initial));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->state));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->vector));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->tall));
	return 1;
}

static int WriteBlowerType(byteio *writer, const blowerType *data)
{
	FORWARD_FALSE(WritePoint(writer, &data->topLeft));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->distance));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->initial));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->state));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->vector));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->tall));
	return 1;
}

static int ReadFurnitureType(byteio *reader, furnitureType *data)
{
	FORWARD_FALSE(ReadRect(reader, &data->bounds));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->pict));
	return 1;
}

static int WriteFurnitureType(byteio *writer, const furnitureType *data)
{
	FORWARD_FALSE(WriteRect(writer, &data->bounds));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->pict));
	return 1;
}

static int ReadBonusType(byteio *reader, bonusType *data)
{
	FORWARD_FALSE(ReadPoint(reader, &data->topLeft));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->length));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->points));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->state));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->initial));
	return 1;
}

static int WriteBonusType(byteio *writer, const bonusType *data)
{
	FORWARD_FALSE(WritePoint(writer, &data->topLeft));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->length));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->points));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->state));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->initial));
	return 1;
}

static int ReadTransportType(byteio *reader, transportType *data)
{
	FORWARD_FALSE(ReadPoint(reader, &data->topLeft));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->tall));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->where));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->who));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->wide));
	return 1;
}

static int WriteTransportType(byteio *writer, const transportType *data)
{
	FORWARD_FALSE(WritePoint(writer, &data->topLeft));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->tall));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->where));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->who));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->wide));
	return 1;
}

static int ReadSwitchType(byteio *reader, switchType *data)
{
	FORWARD_FALSE(ReadPoint(reader, &data->topLeft));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->delay));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->where));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->who));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->type));
	return 1;
}

static int WriteSwitchType(byteio *writer, const switchType *data)
{
	FORWARD_FALSE(WritePoint(writer, &data->topLeft));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->delay));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->where));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->who));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->type));
	return 1;
}

static int ReadLightType(byteio *reader, lightType *data)
{
	FORWARD_FALSE(ReadPoint(reader, &data->topLeft));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->length));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->byte0));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->byte1));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->initial));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->state));
	return 1;
}

static int WriteLightType(byteio *writer, const lightType *data)
{
	FORWARD_FALSE(WritePoint(writer, &data->topLeft));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->length));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->byte0));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->byte1));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->initial));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->state));
	return 1;
}

static int ReadApplianceType(byteio *reader, applianceType *data)
{
	FORWARD_FALSE(ReadPoint(reader, &data->topLeft));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->height));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->byte0));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->delay));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->initial));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->state));
	return 1;
}

static int WriteApplianceType(byteio *writer, const applianceType *data)
{
	FORWARD_FALSE(WritePoint(writer, &data->topLeft));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->height));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->byte0));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->delay));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->initial));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->state));
	return 1;
}

static int ReadEnemyType(byteio *reader, enemyType *data)
{
	FORWARD_FALSE(ReadPoint(reader, &data->topLeft));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->length));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->delay));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->byte0));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->initial));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->state));
	return 1;
}

static int WriteEnemyType(byteio *writer, const enemyType *data)
{
	FORWARD_FALSE(WritePoint(writer, &data->topLeft));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->length));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->delay));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->byte0));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->initial));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->state));
	return 1;
}

static int ReadClutterType(byteio *reader, clutterType *data)
{
	FORWARD_FALSE(ReadRect(reader, &data->bounds));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->pict));
	return 1;
}

static int WriteClutterType(byteio *writer, const clutterType *data)
{
	FORWARD_FALSE(WriteRect(writer, &data->bounds));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->pict));
	return 1;
}

static int ReadObjectType(byteio *reader, objectType *data)
{
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->what));
	switch (data->what)
	{
		case kFloorVent:		case kCeilingVent:		case kFloorBlower:
		case kCeilingBlower:	case kSewerGrate:		case kLeftFan:
		case kRightFan:			case kTaper:			case kCandle:
		case kStubby:			case kTiki:				case kBBQ:
		case kInvisBlower:		case kGrecoVent:		case kSewerBlower:
		case kLiftArea:
		FORWARD_FALSE(ReadBlowerType(reader, &data->data.a));
		break;

		case kTable:			case kShelf:			case kCabinet:
		case kFilingCabinet:	case kWasteBasket:		case kMilkCrate:
		case kCounter:			case kDresser:			case kDeckTable:
		case kStool:			case kTrunk:			case kInvisObstacle:
		case kManhole:			case kBooks:			case kInvisBounce:
		FORWARD_FALSE(ReadFurnitureType(reader, &data->data.b));
		break;

		case kRedClock:			case kBlueClock:		case kYellowClock:
		case kCuckoo:			case kPaper:			case kBattery:
		case kBands:			case kGreaseRt:			case kGreaseLf:
		case kFoil:				case kInvisBonus:		case kStar:
		case kSparkle:			case kHelium:			case kSlider:
		FORWARD_FALSE(ReadBonusType(reader, &data->data.c));
		break;

		case kUpStairs:			case kDownStairs:		case kMailboxLf:
		case kMailboxRt:		case kFloorTrans:		case kCeilingTrans:
		case kDoorInLf:			case kDoorInRt:			case kDoorExRt:
		case kDoorExLf:			case kWindowInLf:		case kWindowInRt:
		case kWindowExRt:		case kWindowExLf:		case kInvisTrans:
		case kDeluxeTrans:
		FORWARD_FALSE(ReadTransportType(reader, &data->data.d));
		break;

		case kLightSwitch:		case kMachineSwitch:	case kThermostat:
		case kPowerSwitch:		case kKnifeSwitch:		case kInvisSwitch:
		case kTrigger:			case kLgTrigger:		case kSoundTrigger:
		FORWARD_FALSE(ReadSwitchType(reader, &data->data.e));
		break;

		case kCeilingLight:		case kLightBulb:		case kTableLamp:
		case kHipLamp:			case kDecoLamp:			case kFlourescent:
		case kTrackLight:		case kInvisLight:
		FORWARD_FALSE(ReadLightType(reader, &data->data.f));
		break;

		case kShredder:			case kToaster:			case kMacPlus:
		case kGuitar:			case kTV:				case kCoffee:
		case kOutlet:			case kVCR:				case kStereo:
		case kMicrowave:		case kCinderBlock:		case kFlowerBox:
		case kCDs:				case kCustomPict:
		FORWARD_FALSE(ReadApplianceType(reader, &data->data.g));
		break;

		case kBalloon:			case kCopterLf:			case kCopterRt:
		case kDartLf:			case kDartRt:			case kBall:
		case kDrip:				case kFish:				case kCobweb:
		FORWARD_FALSE(ReadEnemyType(reader, &data->data.h));
		break;

		case kOzma:				case kMirror:			case kMousehole:
		case kFireplace:		case kFlower:			case kWallWindow:
		case kBear:				case kCalendar:			case kVase1:
		case kVase2:			case kBulletin:			case kCloud:
		case kFaucet:			case kRug:				case kChimes:
		FORWARD_FALSE(ReadClutterType(reader, &data->data.i));
		break;

		// unknown objects and empty object slots are interpreted as clutter
		case kObjectIsEmpty:	default:
		FORWARD_FALSE(ReadClutterType(reader, &data->data.i));
		break;
	}
	return 1;
}

static int WriteObjectType(byteio *writer, const objectType *data)
{
	FORWARD_FALSE(byteio_write_be_i16(writer, data->what));
	switch (data->what)
	{
		case kFloorVent:		case kCeilingVent:		case kFloorBlower:
		case kCeilingBlower:	case kSewerGrate:		case kLeftFan:
		case kRightFan:			case kTaper:			case kCandle:
		case kStubby:			case kTiki:				case kBBQ:
		case kInvisBlower:		case kGrecoVent:		case kSewerBlower:
		case kLiftArea:
		FORWARD_FALSE(WriteBlowerType(writer, &data->data.a));
		break;

		case kTable:			case kShelf:			case kCabinet:
		case kFilingCabinet:	case kWasteBasket:		case kMilkCrate:
		case kCounter:			case kDresser:			case kDeckTable:
		case kStool:			case kTrunk:			case kInvisObstacle:
		case kManhole:			case kBooks:			case kInvisBounce:
		FORWARD_FALSE(WriteFurnitureType(writer, &data->data.b));
		break;

		case kRedClock:			case kBlueClock:		case kYellowClock:
		case kCuckoo:			case kPaper:			case kBattery:
		case kBands:			case kGreaseRt:			case kGreaseLf:
		case kFoil:				case kInvisBonus:		case kStar:
		case kSparkle:			case kHelium:			case kSlider:
		FORWARD_FALSE(WriteBonusType(writer, &data->data.c));
		break;

		case kUpStairs:			case kDownStairs:		case kMailboxLf:
		case kMailboxRt:		case kFloorTrans:		case kCeilingTrans:
		case kDoorInLf:			case kDoorInRt:			case kDoorExRt:
		case kDoorExLf:			case kWindowInLf:		case kWindowInRt:
		case kWindowExRt:		case kWindowExLf:		case kInvisTrans:
		case kDeluxeTrans:
		FORWARD_FALSE(WriteTransportType(writer, &data->data.d));
		break;

		case kLightSwitch:		case kMachineSwitch:	case kThermostat:
		case kPowerSwitch:		case kKnifeSwitch:		case kInvisSwitch:
		case kTrigger:			case kLgTrigger:		case kSoundTrigger:
		FORWARD_FALSE(WriteSwitchType(writer, &data->data.e));
		break;

		case kCeilingLight:		case kLightBulb:		case kTableLamp:
		case kHipLamp:			case kDecoLamp:			case kFlourescent:
		case kTrackLight:		case kInvisLight:
		FORWARD_FALSE(WriteLightType(writer, &data->data.f));
		break;

		case kShredder:			case kToaster:			case kMacPlus:
		case kGuitar:			case kTV:				case kCoffee:
		case kOutlet:			case kVCR:				case kStereo:
		case kMicrowave:		case kCinderBlock:		case kFlowerBox:
		case kCDs:				case kCustomPict:
		FORWARD_FALSE(WriteApplianceType(writer, &data->data.g));
		break;

		case kBalloon:			case kCopterLf:			case kCopterRt:
		case kDartLf:			case kDartRt:			case kBall:
		case kDrip:				case kFish:				case kCobweb:
		FORWARD_FALSE(WriteEnemyType(writer, &data->data.h));
		break;

		case kOzma:				case kMirror:			case kMousehole:
		case kFireplace:		case kFlower:			case kWallWindow:
		case kBear:				case kCalendar:			case kVase1:
		case kVase2:			case kBulletin:			case kCloud:
		case kFaucet:			case kRug:				case kChimes:
		FORWARD_FALSE(WriteClutterType(writer, &data->data.i));
		break;

		// unknown objects and empty object slots are interpreted as clutter
		case kObjectIsEmpty:	default:
		FORWARD_FALSE(WriteClutterType(writer, &data->data.i));
		break;
	}
	return 1;
}

int ReadScoresType(byteio *reader, scoresType *data)
{
	size_t i;
	FORWARD_FALSE(ReadStr31(reader, &data->banner));
	for (i = 0; i < ARRAYSIZE(data->names); i++)
		FORWARD_FALSE(ReadStr15(reader, &data->names[i]));
	for (i = 0; i < ARRAYSIZE(data->scores); i++)
		FORWARD_FALSE(byteio_read_be_i32(reader, &data->scores[i]));
	for (i = 0; i < ARRAYSIZE(data->timeStamps); i++)
		FORWARD_FALSE(byteio_read_be_u32(reader, &data->timeStamps[i]));
	for (i = 0; i < ARRAYSIZE(data->levels); i++)
		FORWARD_FALSE(byteio_read_be_i16(reader, &data->levels[i]));
	return 1;
}

int WriteScoresType(byteio *writer, const scoresType *data)
{
	size_t i;
	FORWARD_FALSE(WriteStr31(writer, &data->banner));
	for (i = 0; i < ARRAYSIZE(data->names); i++)
		FORWARD_FALSE(WriteStr15(writer, &data->names[i]));
	for (i = 0; i < ARRAYSIZE(data->scores); i++)
		FORWARD_FALSE(byteio_write_be_i32(writer, data->scores[i]));
	for (i = 0; i < ARRAYSIZE(data->timeStamps); i++)
		FORWARD_FALSE(byteio_write_be_u32(writer, data->timeStamps[i]));
	for (i = 0; i < ARRAYSIZE(data->levels); i++)
		FORWARD_FALSE(byteio_write_be_i16(writer, data->levels[i]));
	return 1;
}

static int ReadGameType(byteio *reader, gameType *data)
{
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->version));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->wasStarsLeft));
	FORWARD_FALSE(byteio_read_be_i32(reader, &data->timeStamp));
	FORWARD_FALSE(ReadPoint(reader, &data->where));
	FORWARD_FALSE(byteio_read_be_i32(reader, &data->score));
	FORWARD_FALSE(byteio_read_be_i32(reader, &data->unusedLong));
	FORWARD_FALSE(byteio_read_be_i32(reader, &data->unusedLong2));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->energy));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->bands));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->roomNumber));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->gliderState));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->numGliders));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->foil));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->unusedShort));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->facing));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->showFoil));
	return 1;
}

static int WriteGameType(byteio *writer, const gameType *data)
{
	FORWARD_FALSE(byteio_write_be_i16(writer, data->version));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->wasStarsLeft));
	FORWARD_FALSE(byteio_write_be_i32(writer, data->timeStamp));
	FORWARD_FALSE(WritePoint(writer, &data->where));
	FORWARD_FALSE(byteio_write_be_i32(writer, data->score));
	FORWARD_FALSE(byteio_write_be_i32(writer, data->unusedLong));
	FORWARD_FALSE(byteio_write_be_i32(writer, data->unusedLong2));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->energy));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->bands));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->roomNumber));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->gliderState));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->numGliders));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->foil));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->unusedShort));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->facing));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->showFoil));
	return 1;
}

static int ReadSavedRoom(byteio *reader, savedRoom *data)
{
	size_t i;
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->unusedShort));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->unusedByte));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->visited));
	for (i = 0; i < ARRAYSIZE(data->objects); i++)
		FORWARD_FALSE(ReadObjectType(reader, &data->objects[i]));
	return 1;
}

static int WriteSavedRoom(byteio *writer, const savedRoom *data)
{
	size_t i;
	FORWARD_FALSE(byteio_write_be_i16(writer, data->unusedShort));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->unusedByte));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->visited));
	for (i = 0; i < ARRAYSIZE(data->objects); i++)
		FORWARD_FALSE(WriteObjectType(writer, &data->objects[i]));
	return 1;
}

int ReadGame2Type(byteio *reader, game2Type *data)
{
	SInt16 i;
	FORWARD_FALSE(ReadFSSpec(reader, &data->house));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->version));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->wasStarsLeft));
	FORWARD_FALSE(byteio_read_be_i32(reader, &data->timeStamp));
	FORWARD_FALSE(ReadPoint(reader, &data->where));
	FORWARD_FALSE(byteio_read_be_i32(reader, &data->score));
	FORWARD_FALSE(byteio_read_be_i32(reader, &data->unusedLong));
	FORWARD_FALSE(byteio_read_be_i32(reader, &data->unusedLong2));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->energy));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->bands));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->roomNumber));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->gliderState));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->numGliders));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->foil));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->nRooms));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->facing));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->showFoil));
	if (data->nRooms < 0)
		return 0;
	data->savedData = calloc(data->nRooms, sizeof(*data->savedData));
	if ((data->savedData == NULL) && (data->nRooms != 0))
		return 0;
	for (i = 0; i < data->nRooms; i++)
	{
		if (ReadSavedRoom(reader, &data->savedData[i]) == 0)
		{
			free(data->savedData);
			data->savedData = NULL;
			data->nRooms = 0;
			return 0;
		}
	}
	return 1;
}

int WriteGame2Type(byteio *writer, const game2Type *data)
{
	SInt16 i;
	FORWARD_FALSE(WriteFSSpec(writer, &data->house));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->version));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->wasStarsLeft));
	FORWARD_FALSE(byteio_write_be_i32(writer, data->timeStamp));
	FORWARD_FALSE(WritePoint(writer, &data->where));
	FORWARD_FALSE(byteio_write_be_i32(writer, data->score));
	FORWARD_FALSE(byteio_write_be_i32(writer, data->unusedLong));
	FORWARD_FALSE(byteio_write_be_i32(writer, data->unusedLong2));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->energy));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->bands));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->roomNumber));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->gliderState));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->numGliders));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->foil));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->nRooms));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->facing));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->showFoil));
	if (data->savedData != NULL)
		for (i = 0; i < data->nRooms; i++)
			FORWARD_FALSE(WriteSavedRoom(writer, &data->savedData[i]));
	return 1;
}

static int ReadRoomType(byteio *reader, roomType *data)
{
	size_t i;
	FORWARD_FALSE(ReadStr27(reader, &data->name));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->bounds));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->leftStart));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->rightStart));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->unusedByte));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->visited));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->background));
	for (i = 0; i < ARRAYSIZE(data->tiles); i++)
		FORWARD_FALSE(byteio_read_be_i16(reader, &data->tiles[i]));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->floor));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->suite));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->openings));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->numObjects));
	for (i = 0; i < ARRAYSIZE(data->objects); i++)
		FORWARD_FALSE(ReadObjectType(reader, &data->objects[i]));
	return 1;
}

static int WriteRoomType(byteio *writer, const roomType *data)
{
	size_t i;
	FORWARD_FALSE(WriteStr27(writer, &data->name));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->bounds));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->leftStart));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->rightStart));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->unusedByte));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->visited));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->background));
	for (i = 0; i < ARRAYSIZE(data->tiles); i++)
		FORWARD_FALSE(byteio_write_be_i16(writer, data->tiles[i]));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->floor));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->suite));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->openings));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->numObjects));
	for (i = 0; i < ARRAYSIZE(data->objects); i++)
		FORWARD_FALSE(WriteObjectType(writer, &data->objects[i]));
	return 1;
}

int ReadHouseType(byteio *reader, houseType *data)
{
	SInt16 i;
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->version));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->unusedShort));
	FORWARD_FALSE(byteio_read_be_i32(reader, &data->timeStamp));
	FORWARD_FALSE(byteio_read_be_i32(reader, &data->flags));
	FORWARD_FALSE(ReadPoint(reader, &data->initial));
	FORWARD_FALSE(ReadStr255(reader, &data->banner));
	FORWARD_FALSE(ReadStr255(reader, &data->trailer));
	FORWARD_FALSE(ReadScoresType(reader, &data->highScores));
	FORWARD_FALSE(ReadGameType(reader, &data->savedGame));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->hasGame));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->unusedBoolean));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->firstRoom));
	FORWARD_FALSE(byteio_read_be_i16(reader, &data->nRooms));
	if (data->nRooms < 0)
		return 0;
	data->rooms = calloc(data->nRooms, sizeof(*data->rooms));
	if ((data->rooms == NULL) && (data->nRooms != 0))
		return 0;
	for (i = 0; i < data->nRooms; i++)
	{
		if (ReadRoomType(reader, &data->rooms[i]) == 0)
		{
			free(data->rooms);
			data->rooms = NULL;
			data->nRooms = 0;
			return 0;
		}
	}
	return 1;
}

int WriteHouseType(byteio *writer, const houseType *data)
{
	SInt16 i;
	FORWARD_FALSE(byteio_write_be_i16(writer, data->version));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->unusedShort));
	FORWARD_FALSE(byteio_write_be_i32(writer, data->timeStamp));
	FORWARD_FALSE(byteio_write_be_i32(writer, data->flags));
	FORWARD_FALSE(WritePoint(writer, &data->initial));
	FORWARD_FALSE(WriteStr255(writer, &data->banner));
	FORWARD_FALSE(WriteStr255(writer, &data->trailer));
	FORWARD_FALSE(WriteScoresType(writer, &data->highScores));
	FORWARD_FALSE(WriteGameType(writer, &data->savedGame));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->hasGame));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->unusedBoolean));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->firstRoom));
	FORWARD_FALSE(byteio_write_be_i16(writer, data->nRooms));
	if (data->rooms != NULL)
		for (i = 0; i < data->nRooms; i++)
			FORWARD_FALSE(WriteRoomType(writer, &data->rooms[i]));
	return 1;
}

int ReadBoundsType(byteio *reader, boundsType *data)
{
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->left));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->top));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->right));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->bottom));
	return 1;
}

int WriteBoundsType(byteio *writer, const boundsType *data)
{
	FORWARD_FALSE(byteio_write_be_u8(writer, data->left));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->top));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->right));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->bottom));
	return 1;
}

int ReadDemoType(byteio *reader, demoType *data)
{
	FORWARD_FALSE(byteio_read_be_i32(reader, &data->frame));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->key));
	FORWARD_FALSE(byteio_read_be_u8(reader, &data->padding));
	return 1;
}

int WriteDemoType(byteio *writer, const demoType *data)
{
	FORWARD_FALSE(byteio_write_be_i32(writer, data->frame));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->key));
	FORWARD_FALSE(byteio_write_be_u8(writer, data->padding));
	return 1;
}
