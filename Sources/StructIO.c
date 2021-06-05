#include "StructIO.h"

#include <stdlib.h>

#define RETURN_IF_FAILED(hr) \
	do { HRESULT hr__ = (hr); if (FAILED(hr__)) { return hr__; } } while (0)

static HRESULT ReadPoint(byteio *reader, Point *data)
{
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->v));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->h));
	return S_OK;
}

static HRESULT WritePoint(byteio *writer, const Point *data)
{
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->v));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->h));
	return S_OK;
}

static HRESULT ReadRect(byteio *reader, Rect *data)
{
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->top));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->left));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->bottom));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->right));
	return S_OK;
}

static HRESULT WriteRect(byteio *writer, const Rect *data)
{
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->top));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->left));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->bottom));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->right));
	return S_OK;
}

static void ClampPascalStringLength(StringPtr data, Byte maxLength)
{
	if (data[0] > maxLength)
	{
		data[0] = maxLength;
	}
}

static HRESULT ReadStr15(byteio *reader, Str15 *data)
{
	RETURN_IF_FAILED(byteio_read(reader, data, sizeof(*data)));
	ClampPascalStringLength(*data, sizeof(*data) - 1);
	return S_OK;
}

static HRESULT WriteStr15(byteio *writer, const Str15 *data)
{
	RETURN_IF_FAILED(byteio_write(writer, data, sizeof(*data)));
	return S_OK;
}

static HRESULT ReadStr27(byteio *reader, Str27 *data)
{
	RETURN_IF_FAILED(byteio_read(reader, data, sizeof(*data)));
	ClampPascalStringLength(*data, sizeof(*data) - 1);
	return S_OK;
}

static HRESULT WriteStr27(byteio *writer, const Str27 *data)
{
	RETURN_IF_FAILED(byteio_write(writer, data, sizeof(*data)));
	return S_OK;
}

static HRESULT ReadStr31(byteio *reader, Str31 *data)
{
	RETURN_IF_FAILED(byteio_read(reader, data, sizeof(*data)));
	ClampPascalStringLength(*data, sizeof(*data) - 1);
	return S_OK;
}

static HRESULT WriteStr31(byteio *writer, const Str31 *data)
{
	RETURN_IF_FAILED(byteio_write(writer, data, sizeof(*data)));
	return S_OK;
}

static HRESULT ReadStr32(byteio *reader, Str32 *data)
{
	RETURN_IF_FAILED(byteio_read(reader, data, sizeof(*data)));
	ClampPascalStringLength(*data, sizeof(*data) - 1);
	return S_OK;
}

static HRESULT WriteStr32(byteio *writer, const Str32 *data)
{
	RETURN_IF_FAILED(byteio_write(writer, data, sizeof(*data)));
	return S_OK;
}

static HRESULT ReadStr63(byteio *reader, Str63 *data)
{
	RETURN_IF_FAILED(byteio_read(reader, data, sizeof(*data)));
	ClampPascalStringLength(*data, sizeof(*data) - 1);
	return S_OK;
}

static HRESULT WriteStr63(byteio *writer, const Str63 *data)
{
	RETURN_IF_FAILED(byteio_write(writer, data, sizeof(*data)));
	return S_OK;
}

static HRESULT ReadStr255(byteio *reader, Str255 *data)
{
	RETURN_IF_FAILED(byteio_read(reader, data, sizeof(*data)));
	ClampPascalStringLength(*data, sizeof(*data) - 1);
	return S_OK;
}

static HRESULT WriteStr255(byteio *writer, const Str255 *data)
{
	RETURN_IF_FAILED(byteio_write(writer, data, sizeof(*data)));
	return S_OK;
}

static HRESULT ReadFSSpec(byteio *reader, FSSpec *data)
{
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->vRefNum));
	RETURN_IF_FAILED(byteio_read_be_i32(reader, &data->parID));
	RETURN_IF_FAILED(ReadStr63(reader, &data->name));
	return S_OK;
}

static HRESULT WriteFSSpec(byteio *writer, const FSSpec *data)
{
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->vRefNum));
	RETURN_IF_FAILED(byteio_write_be_i32(writer, data->parID));
	RETURN_IF_FAILED(WriteStr63(writer, &data->name));
	return S_OK;
}

HRESULT ReadPrefsInfo(byteio *reader, prefsInfo *data)
{
	RETURN_IF_FAILED(ReadStr32(reader, &data->wasDefaultName));
	RETURN_IF_FAILED(byteio_read_be_u16(reader, &data->wasViewportWidth));
	RETURN_IF_FAILED(byteio_read_be_u16(reader, &data->wasViewportHeight));
	RETURN_IF_FAILED(byteio_read(reader, &data->unusedBytes, sizeof(data->unusedBytes)));
	RETURN_IF_FAILED(ReadStr15(reader, &data->wasHighName));
	RETURN_IF_FAILED(ReadStr31(reader, &data->wasHighBanner));
	RETURN_IF_FAILED(byteio_read_be_i32(reader, &data->wasLeftKeyOne));
	RETURN_IF_FAILED(byteio_read_be_i32(reader, &data->wasRightKeyOne));
	RETURN_IF_FAILED(byteio_read_be_i32(reader, &data->wasBattKeyOne));
	RETURN_IF_FAILED(byteio_read_be_i32(reader, &data->wasBandKeyOne));
	RETURN_IF_FAILED(byteio_read_be_i32(reader, &data->wasLeftKeyTwo));
	RETURN_IF_FAILED(byteio_read_be_i32(reader, &data->wasRightKeyTwo));
	RETURN_IF_FAILED(byteio_read_be_i32(reader, &data->wasBattKeyTwo));
	RETURN_IF_FAILED(byteio_read_be_i32(reader, &data->wasBandKeyTwo));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->wasVolume));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->prefVersion));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->wasMaxFiles));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->wasEditH));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->wasEditV));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->wasMapH));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->wasMapV));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->wasMapWide));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->wasMapHigh));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->wasToolsH));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->wasToolsV));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->wasLinkH));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->wasLinkV));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->wasCoordH));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->wasCoordV));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->isMapLeft));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->isMapTop));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->wasNumNeighbors));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->wasToolGroup));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->wasFloor));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->wasSuite));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->wasMusicOn));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->wasAutoEdit));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->wasDoColorFade));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->wasMapOpen));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->wasToolsOpen));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->wasCoordOpen));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->wasQuickTrans));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->wasIdleMusic));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->wasGameMusic));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->wasEscPauseKey));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->wasDoAutoDemo));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->wasScreen2));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->wasHouseChecks));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->wasPrettyMap));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->wasBitchDialogs));
	return S_OK;
}

HRESULT WritePrefsInfo(byteio *writer, const prefsInfo *data)
{
	RETURN_IF_FAILED(WriteStr32(writer, &data->wasDefaultName));
	RETURN_IF_FAILED(byteio_write_be_u16(writer, data->wasViewportWidth));
	RETURN_IF_FAILED(byteio_write_be_u16(writer, data->wasViewportHeight));
	RETURN_IF_FAILED(byteio_write(writer, &data->unusedBytes, sizeof(data->unusedBytes)));
	RETURN_IF_FAILED(WriteStr15(writer, &data->wasHighName));
	RETURN_IF_FAILED(WriteStr31(writer, &data->wasHighBanner));
	RETURN_IF_FAILED(byteio_write_be_i32(writer, data->wasLeftKeyOne));
	RETURN_IF_FAILED(byteio_write_be_i32(writer, data->wasRightKeyOne));
	RETURN_IF_FAILED(byteio_write_be_i32(writer, data->wasBattKeyOne));
	RETURN_IF_FAILED(byteio_write_be_i32(writer, data->wasBandKeyOne));
	RETURN_IF_FAILED(byteio_write_be_i32(writer, data->wasLeftKeyTwo));
	RETURN_IF_FAILED(byteio_write_be_i32(writer, data->wasRightKeyTwo));
	RETURN_IF_FAILED(byteio_write_be_i32(writer, data->wasBattKeyTwo));
	RETURN_IF_FAILED(byteio_write_be_i32(writer, data->wasBandKeyTwo));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->wasVolume));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->prefVersion));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->wasMaxFiles));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->wasEditH));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->wasEditV));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->wasMapH));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->wasMapV));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->wasMapWide));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->wasMapHigh));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->wasToolsH));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->wasToolsV));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->wasLinkH));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->wasLinkV));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->wasCoordH));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->wasCoordV));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->isMapLeft));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->isMapTop));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->wasNumNeighbors));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->wasToolGroup));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->wasFloor));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->wasSuite));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->wasMusicOn));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->wasAutoEdit));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->wasDoColorFade));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->wasMapOpen));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->wasToolsOpen));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->wasCoordOpen));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->wasQuickTrans));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->wasIdleMusic));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->wasGameMusic));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->wasEscPauseKey));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->wasDoAutoDemo));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->wasScreen2));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->wasHouseChecks));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->wasPrettyMap));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->wasBitchDialogs));
	return S_OK;
}

static HRESULT ReadBlowerType(byteio *reader, blowerType *data)
{
	RETURN_IF_FAILED(ReadPoint(reader, &data->topLeft));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->distance));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->initial));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->state));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->vector));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->tall));
	return S_OK;
}

static HRESULT WriteBlowerType(byteio *writer, const blowerType *data)
{
	RETURN_IF_FAILED(WritePoint(writer, &data->topLeft));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->distance));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->initial));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->state));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->vector));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->tall));
	return S_OK;
}

static HRESULT ReadFurnitureType(byteio *reader, furnitureType *data)
{
	RETURN_IF_FAILED(ReadRect(reader, &data->bounds));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->pict));
	return S_OK;
}

static HRESULT WriteFurnitureType(byteio *writer, const furnitureType *data)
{
	RETURN_IF_FAILED(WriteRect(writer, &data->bounds));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->pict));
	return S_OK;
}

static HRESULT ReadBonusType(byteio *reader, bonusType *data)
{
	RETURN_IF_FAILED(ReadPoint(reader, &data->topLeft));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->length));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->points));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->state));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->initial));
	return S_OK;
}

static HRESULT WriteBonusType(byteio *writer, const bonusType *data)
{
	RETURN_IF_FAILED(WritePoint(writer, &data->topLeft));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->length));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->points));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->state));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->initial));
	return S_OK;
}

static HRESULT ReadTransportType(byteio *reader, transportType *data)
{
	RETURN_IF_FAILED(ReadPoint(reader, &data->topLeft));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->tall));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->where));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->who));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->wide));
	return S_OK;
}

static HRESULT WriteTransportType(byteio *writer, const transportType *data)
{
	RETURN_IF_FAILED(WritePoint(writer, &data->topLeft));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->tall));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->where));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->who));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->wide));
	return S_OK;
}

static HRESULT ReadSwitchType(byteio *reader, switchType *data)
{
	RETURN_IF_FAILED(ReadPoint(reader, &data->topLeft));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->delay));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->where));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->who));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->type));
	return S_OK;
}

static HRESULT WriteSwitchType(byteio *writer, const switchType *data)
{
	RETURN_IF_FAILED(WritePoint(writer, &data->topLeft));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->delay));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->where));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->who));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->type));
	return S_OK;
}

static HRESULT ReadLightType(byteio *reader, lightType *data)
{
	RETURN_IF_FAILED(ReadPoint(reader, &data->topLeft));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->length));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->byte0));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->byte1));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->initial));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->state));
	return S_OK;
}

static HRESULT WriteLightType(byteio *writer, const lightType *data)
{
	RETURN_IF_FAILED(WritePoint(writer, &data->topLeft));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->length));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->byte0));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->byte1));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->initial));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->state));
	return S_OK;
}

static HRESULT ReadApplianceType(byteio *reader, applianceType *data)
{
	RETURN_IF_FAILED(ReadPoint(reader, &data->topLeft));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->height));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->byte0));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->delay));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->initial));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->state));
	return S_OK;
}

static HRESULT WriteApplianceType(byteio *writer, const applianceType *data)
{
	RETURN_IF_FAILED(WritePoint(writer, &data->topLeft));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->height));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->byte0));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->delay));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->initial));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->state));
	return S_OK;
}

static HRESULT ReadEnemyType(byteio *reader, enemyType *data)
{
	RETURN_IF_FAILED(ReadPoint(reader, &data->topLeft));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->length));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->delay));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->byte0));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->initial));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->state));
	return S_OK;
}

static HRESULT WriteEnemyType(byteio *writer, const enemyType *data)
{
	RETURN_IF_FAILED(WritePoint(writer, &data->topLeft));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->length));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->delay));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->byte0));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->initial));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->state));
	return S_OK;
}

static HRESULT ReadClutterType(byteio *reader, clutterType *data)
{
	RETURN_IF_FAILED(ReadRect(reader, &data->bounds));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->pict));
	return S_OK;
}

static HRESULT WriteClutterType(byteio *writer, const clutterType *data)
{
	RETURN_IF_FAILED(WriteRect(writer, &data->bounds));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->pict));
	return S_OK;
}

static HRESULT ReadObjectType(byteio *reader, objectType *data)
{
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->what));
	switch (data->what)
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
		RETURN_IF_FAILED(ReadBlowerType(reader, &data->data.a));
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
		RETURN_IF_FAILED(ReadFurnitureType(reader, &data->data.b));
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
		RETURN_IF_FAILED(ReadBonusType(reader, &data->data.c));
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
		RETURN_IF_FAILED(ReadTransportType(reader, &data->data.d));
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
		RETURN_IF_FAILED(ReadSwitchType(reader, &data->data.e));
		break;

	case kCeilingLight:
	case kLightBulb:
	case kTableLamp:
	case kHipLamp:
	case kDecoLamp:
	case kFlourescent:
	case kTrackLight:
	case kInvisLight:
		RETURN_IF_FAILED(ReadLightType(reader, &data->data.f));
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
		RETURN_IF_FAILED(ReadApplianceType(reader, &data->data.g));
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
		RETURN_IF_FAILED(ReadEnemyType(reader, &data->data.h));
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
		RETURN_IF_FAILED(ReadClutterType(reader, &data->data.i));
		break;

	// unknown objects and empty object slots are interpreted as clutter
	case kObjectIsEmpty:
	default:
		RETURN_IF_FAILED(ReadClutterType(reader, &data->data.i));
		break;
	}
	return S_OK;
}

static HRESULT WriteObjectType(byteio *writer, const objectType *data)
{
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->what));
	switch (data->what)
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
		RETURN_IF_FAILED(WriteBlowerType(writer, &data->data.a));
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
		RETURN_IF_FAILED(WriteFurnitureType(writer, &data->data.b));
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
		RETURN_IF_FAILED(WriteBonusType(writer, &data->data.c));
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
		RETURN_IF_FAILED(WriteTransportType(writer, &data->data.d));
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
		RETURN_IF_FAILED(WriteSwitchType(writer, &data->data.e));
		break;

	case kCeilingLight:
	case kLightBulb:
	case kTableLamp:
	case kHipLamp:
	case kDecoLamp:
	case kFlourescent:
	case kTrackLight:
	case kInvisLight:
		RETURN_IF_FAILED(WriteLightType(writer, &data->data.f));
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
		RETURN_IF_FAILED(WriteApplianceType(writer, &data->data.g));
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
		RETURN_IF_FAILED(WriteEnemyType(writer, &data->data.h));
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
		RETURN_IF_FAILED(WriteClutterType(writer, &data->data.i));
		break;

	// unknown objects and empty object slots are interpreted as clutter
	case kObjectIsEmpty:
	default:
		RETURN_IF_FAILED(WriteClutterType(writer, &data->data.i));
		break;
	}
	return S_OK;
}

HRESULT ReadScoresType(byteio *reader, scoresType *data)
{
	size_t i;

	RETURN_IF_FAILED(ReadStr31(reader, &data->banner));
	for (i = 0; i < ARRAYSIZE(data->names); i++)
	{
		RETURN_IF_FAILED(ReadStr15(reader, &data->names[i]));
	}
	for (i = 0; i < ARRAYSIZE(data->scores); i++)
	{
		RETURN_IF_FAILED(byteio_read_be_i32(reader, &data->scores[i]));
	}
	for (i = 0; i < ARRAYSIZE(data->timeStamps); i++)
	{
		RETURN_IF_FAILED(byteio_read_be_u32(reader, &data->timeStamps[i]));
	}
	for (i = 0; i < ARRAYSIZE(data->levels); i++)
	{
		RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->levels[i]));
	}
	return S_OK;
}

HRESULT WriteScoresType(byteio *writer, const scoresType *data)
{
	size_t i;

	RETURN_IF_FAILED(WriteStr31(writer, &data->banner));
	for (i = 0; i < ARRAYSIZE(data->names); i++)
	{
		RETURN_IF_FAILED(WriteStr15(writer, &data->names[i]));
	}
	for (i = 0; i < ARRAYSIZE(data->scores); i++)
	{
		RETURN_IF_FAILED(byteio_write_be_i32(writer, data->scores[i]));
	}
	for (i = 0; i < ARRAYSIZE(data->timeStamps); i++)
	{
		RETURN_IF_FAILED(byteio_write_be_u32(writer, data->timeStamps[i]));
	}
	for (i = 0; i < ARRAYSIZE(data->levels); i++)
	{
		RETURN_IF_FAILED(byteio_write_be_i16(writer, data->levels[i]));
	}
	return S_OK;
}

static HRESULT ReadGameType(byteio *reader, gameType *data)
{
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->version));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->wasStarsLeft));
	RETURN_IF_FAILED(byteio_read_be_i32(reader, &data->timeStamp));
	RETURN_IF_FAILED(ReadPoint(reader, &data->where));
	RETURN_IF_FAILED(byteio_read_be_i32(reader, &data->score));
	RETURN_IF_FAILED(byteio_read_be_i32(reader, &data->unusedLong));
	RETURN_IF_FAILED(byteio_read_be_i32(reader, &data->unusedLong2));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->energy));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->bands));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->roomNumber));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->gliderState));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->numGliders));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->foil));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->unusedShort));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->facing));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->showFoil));
	return S_OK;
}

static HRESULT WriteGameType(byteio *writer, const gameType *data)
{
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->version));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->wasStarsLeft));
	RETURN_IF_FAILED(byteio_write_be_i32(writer, data->timeStamp));
	RETURN_IF_FAILED(WritePoint(writer, &data->where));
	RETURN_IF_FAILED(byteio_write_be_i32(writer, data->score));
	RETURN_IF_FAILED(byteio_write_be_i32(writer, data->unusedLong));
	RETURN_IF_FAILED(byteio_write_be_i32(writer, data->unusedLong2));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->energy));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->bands));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->roomNumber));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->gliderState));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->numGliders));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->foil));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->unusedShort));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->facing));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->showFoil));
	return S_OK;
}

static HRESULT ReadSavedRoom(byteio *reader, savedRoom *data)
{
	size_t i;

	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->unusedShort));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->unusedByte));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->visited));
	for (i = 0; i < ARRAYSIZE(data->objects); i++)
	{
		RETURN_IF_FAILED(ReadObjectType(reader, &data->objects[i]));
	}
	return S_OK;
}

static HRESULT WriteSavedRoom(byteio *writer, const savedRoom *data)
{
	size_t i;

	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->unusedShort));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->unusedByte));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->visited));
	for (i = 0; i < ARRAYSIZE(data->objects); i++)
	{
		RETURN_IF_FAILED(WriteObjectType(writer, &data->objects[i]));
	}
	return S_OK;
}

HRESULT ReadGame2Type(byteio *reader, game2Type *data)
{
	SInt16 i;
	HRESULT hr;

	data->nRooms = 0;
	data->savedData = NULL;
	RETURN_IF_FAILED(ReadFSSpec(reader, &data->house));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->version));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->wasStarsLeft));
	RETURN_IF_FAILED(byteio_read_be_i32(reader, &data->timeStamp));
	RETURN_IF_FAILED(ReadPoint(reader, &data->where));
	RETURN_IF_FAILED(byteio_read_be_i32(reader, &data->score));
	RETURN_IF_FAILED(byteio_read_be_i32(reader, &data->unusedLong));
	RETURN_IF_FAILED(byteio_read_be_i32(reader, &data->unusedLong2));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->energy));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->bands));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->roomNumber));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->gliderState));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->numGliders));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->foil));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->nRooms));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->facing));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->showFoil));
	if (data->nRooms <= 0)
	{
		data->nRooms = 0;
		data->savedData = NULL;
		return S_OK;
	}
	data->savedData = (savedRoom *)calloc((size_t)data->nRooms, sizeof(*data->savedData));
	if (data->savedData == NULL)
	{
		return E_OUTOFMEMORY;
	}
	for (i = 0; i < data->nRooms; i++)
	{
		hr = ReadSavedRoom(reader, &data->savedData[i]);
		if (FAILED(hr))
		{
			free(data->savedData);
			data->savedData = NULL;
			data->nRooms = 0;
			return hr;
		}
	}
	return S_OK;
}

HRESULT WriteGame2Type(byteio *writer, const game2Type *data)
{
	SInt16 i;

	RETURN_IF_FAILED(WriteFSSpec(writer, &data->house));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->version));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->wasStarsLeft));
	RETURN_IF_FAILED(byteio_write_be_i32(writer, data->timeStamp));
	RETURN_IF_FAILED(WritePoint(writer, &data->where));
	RETURN_IF_FAILED(byteio_write_be_i32(writer, data->score));
	RETURN_IF_FAILED(byteio_write_be_i32(writer, data->unusedLong));
	RETURN_IF_FAILED(byteio_write_be_i32(writer, data->unusedLong2));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->energy));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->bands));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->roomNumber));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->gliderState));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->numGliders));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->foil));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->nRooms));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->facing));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->showFoil));
	if (data->savedData != NULL)
	{
		for (i = 0; i < data->nRooms; i++)
		{
			RETURN_IF_FAILED(WriteSavedRoom(writer, &data->savedData[i]));
		}
	}
	return S_OK;
}

static HRESULT ReadRoomType(byteio *reader, roomType *data)
{
	size_t i;

	RETURN_IF_FAILED(ReadStr27(reader, &data->name));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->bounds));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->leftStart));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->rightStart));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->unusedByte));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->visited));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->background));
	for (i = 0; i < ARRAYSIZE(data->tiles); i++)
	{
		RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->tiles[i]));
	}
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->floor));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->suite));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->openings));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->numObjects));
	for (i = 0; i < ARRAYSIZE(data->objects); i++)
	{
		RETURN_IF_FAILED(ReadObjectType(reader, &data->objects[i]));
	}
	return S_OK;
}

static HRESULT WriteRoomType(byteio *writer, const roomType *data)
{
	size_t i;

	RETURN_IF_FAILED(WriteStr27(writer, &data->name));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->bounds));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->leftStart));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->rightStart));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->unusedByte));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->visited));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->background));
	for (i = 0; i < ARRAYSIZE(data->tiles); i++)
	{
		RETURN_IF_FAILED(byteio_write_be_i16(writer, data->tiles[i]));
	}
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->floor));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->suite));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->openings));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->numObjects));
	for (i = 0; i < ARRAYSIZE(data->objects); i++)
	{
		RETURN_IF_FAILED(WriteObjectType(writer, &data->objects[i]));
	}
	return S_OK;
}

HRESULT ReadHouseType(byteio *reader, houseType *data)
{
	SInt16 i;
	HRESULT hr;

	data->nRooms = 0;
	data->rooms = NULL;
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->version));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->unusedShort));
	RETURN_IF_FAILED(byteio_read_be_i32(reader, &data->timeStamp));
	RETURN_IF_FAILED(byteio_read_be_i32(reader, &data->flags));
	RETURN_IF_FAILED(ReadPoint(reader, &data->initial));
	RETURN_IF_FAILED(ReadStr255(reader, &data->banner));
	RETURN_IF_FAILED(ReadStr255(reader, &data->trailer));
	RETURN_IF_FAILED(ReadScoresType(reader, &data->highScores));
	RETURN_IF_FAILED(ReadGameType(reader, &data->savedGame));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->hasGame));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->unusedBoolean));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->firstRoom));
	RETURN_IF_FAILED(byteio_read_be_i16(reader, &data->nRooms));
	if (data->nRooms <= 0)
	{
		data->nRooms = 0;
		data->rooms = NULL;
		return S_OK;
	}
	data->rooms = (roomPtr)calloc((size_t)data->nRooms, sizeof(*data->rooms));
	if (data->rooms == NULL)
	{
		data->nRooms = 0;
		return E_OUTOFMEMORY;
	}
	for (i = 0; i < data->nRooms; i++)
	{
		hr = ReadRoomType(reader, &data->rooms[i]);
		if (FAILED(hr))
		{
			free(data->rooms);
			data->rooms = NULL;
			data->nRooms = 0;
			return hr;
		}
	}
	return S_OK;
}

HRESULT WriteHouseType(byteio *writer, const houseType *data)
{
	SInt16 i;

	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->version));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->unusedShort));
	RETURN_IF_FAILED(byteio_write_be_i32(writer, data->timeStamp));
	RETURN_IF_FAILED(byteio_write_be_i32(writer, data->flags));
	RETURN_IF_FAILED(WritePoint(writer, &data->initial));
	RETURN_IF_FAILED(WriteStr255(writer, &data->banner));
	RETURN_IF_FAILED(WriteStr255(writer, &data->trailer));
	RETURN_IF_FAILED(WriteScoresType(writer, &data->highScores));
	RETURN_IF_FAILED(WriteGameType(writer, &data->savedGame));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->hasGame));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->unusedBoolean));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->firstRoom));
	RETURN_IF_FAILED(byteio_write_be_i16(writer, data->nRooms));
	if (data->rooms != NULL)
	{
		for (i = 0; i < data->nRooms; i++)
		{
			RETURN_IF_FAILED(WriteRoomType(writer, &data->rooms[i]));
		}
	}
	return S_OK;
}

HRESULT ReadBoundsType(byteio *reader, boundsType *data)
{
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->left));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->top));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->right));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->bottom));
	return S_OK;
}

HRESULT WriteBoundsType(byteio *writer, const boundsType *data)
{
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->left));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->top));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->right));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->bottom));
	return S_OK;
}

HRESULT ReadDemoType(byteio *reader, demoType *data)
{
	RETURN_IF_FAILED(byteio_read_be_i32(reader, &data->frame));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->key));
	RETURN_IF_FAILED(byteio_read_be_u8(reader, &data->padding));
	return S_OK;
}

HRESULT WriteDemoType(byteio *writer, const demoType *data)
{
	RETURN_IF_FAILED(byteio_write_be_i32(writer, data->frame));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->key));
	RETURN_IF_FAILED(byteio_write_be_u8(writer, data->padding));
	return S_OK;
}
