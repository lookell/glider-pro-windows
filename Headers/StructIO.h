#ifndef STRUCT_IO_H_
#define STRUCT_IO_H_

#include "ByteIO.h"
#include "GliderStructs.h"

// "Externs.h" is only needed for prefsInfo.
// TODO: move prefsInfo to a separate header
#include "Externs.h"

// The following functions all return nonzero on success and zero on failure.

int ReadPrefsInfo (byteio *reader, prefsInfo *data);
int WritePrefsInfo (byteio *writer, const prefsInfo *data);
int ReadBlowerType (byteio *reader, blowerType *data);
int WriteBlowerType (byteio *writer, const blowerType *data);
int ReadFurnitureType (byteio *reader, furnitureType *data);
int WriteFurnitureType (byteio *writer, const furnitureType *data);
int ReadBonusType (byteio *reader, bonusType *data);
int WriteBonusType (byteio *writer, const bonusType *data);
int ReadTransportType (byteio *reader, transportType *data);
int WriteTransportType (byteio *writer, const transportType *data);
int ReadSwitchType (byteio *reader, switchType *data);
int WriteSwitchType (byteio *writer, const switchType *data);
int ReadLightType (byteio *reader, lightType *data);
int WriteLightType (byteio *writer, const lightType *data);
int ReadApplianceType (byteio *reader, applianceType *data);
int WriteApplianceType (byteio *writer, const applianceType *data);
int ReadEnemyType (byteio *reader, enemyType *data);
int WriteEnemyType (byteio *writer, const enemyType *data);
int ReadClutterType (byteio *reader, clutterType *data);
int WriteClutterType (byteio *writer, const clutterType *data);
int ReadObjectType (byteio *reader, objectType *data);
int WriteObjectType (byteio *writer, const objectType *data);
int ReadScoresType (byteio *reader, scoresType *data);
int WriteScoresType (byteio *writer, const scoresType *data);
int ReadGameType (byteio *reader, gameType *data);
int WriteGameType (byteio *writer, const gameType *data);
int ReadSavedRoom (byteio *reader, savedRoom *data);
int WriteSavedRoom (byteio *writer, const savedRoom *data);
int ReadGame2Type (byteio *reader, game2Type *data);
int WriteGame2Type (byteio *writer, const game2Type *data);
int ReadRoomType (byteio *reader, roomType *data);
int WriteRoomType (byteio *writer, const roomType *data);
int ReadHouseType (byteio *reader, houseType *data);
int WriteHouseType (byteio *writer, const houseType *data);
int ReadBoundsType (byteio *reader, boundsType *data);
int WriteBoundsType (byteio *writer, const boundsType *data);
int ReadDemoType (byteio *reader, demoType *data);
int WriteDemoType (byteio *writer, const demoType *data);

#endif
