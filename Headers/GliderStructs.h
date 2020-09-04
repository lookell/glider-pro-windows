#ifndef GLIDER_STRUCTS_H_
#define GLIDER_STRUCTS_H_

//============================================================================
//----------------------------------------------------------------------------
//                              GliderStructs.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderDefines.h"
#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

typedef struct blowerType
{
	Point topLeft;                      // 4
	SInt16 distance;                    // 2
	Boolean initial;                    // 1
	Boolean state;                      // 1                      F. lf. dn. rt. up
	Byte vector;                        // 1        | x | x | x | x | 8 | 4 | 2 | 1 |
	Byte tall;                          // 1
} blowerType;                           // total = 10

typedef struct furnitureType
{
	Rect bounds;                        // 8
	SInt16 pict;                        // 2
} furnitureType;                        // total = 10

typedef struct bonusType
{
	Point topLeft;                      // 4
	SInt16 length;                      // 2 grease spill
	SInt16 points;                      // 2 invis bonus
	Boolean state;                      // 1
	Boolean initial;                    // 1
} bonusType;                            // total = 10

typedef struct transportType
{
	Point topLeft;                      // 4
	SInt16 tall;                        // 2 invis transport
	SInt16 where;                       // 2
	Byte who;                           // 1
	Byte wide;                          // 1
} transportType;                        // total = 10

typedef struct switchType
{
	Point topLeft;                      // 4
	SInt16 delay;                       // 2
	SInt16 where;                       // 2
	Byte who;                           // 1
	Byte type;                          // 1
} switchType;                           // total = 10

typedef struct lightType
{
	Point topLeft;                      // 4
	SInt16 length;                      // 2
	Byte byte0;                         // 1
	Byte byte1;                         // 1
	Boolean initial;                    // 1
	Boolean state;                      // 1
} lightType;                            // total = 10

typedef struct applianceType
{
	Point topLeft;                      // 4
	SInt16 height;                      // 2 toaster, pict ID
	Byte byte0;                         // 1
	Byte delay;                         // 1
	Boolean initial;                    // 1
	Boolean state;                      // 1
} applianceType;                        // total = 10

typedef struct enemyType
{
	Point topLeft;                          // 4
	SInt16 length;                          // 2
	Byte delay;                             // 1
	Byte byte0;                             // 1
	Boolean initial;                        // 1
	Boolean state;                          // 1
} enemyType;                                // total = 10

typedef struct clutterType
{
	Rect bounds;                            // 8
	SInt16 pict;                            // 2
} clutterType;                              // total = 10

typedef union objectInfo
{
	blowerType a;                           // 10
	furnitureType b;                        // 10
	bonusType c;                            // 10
	transportType d;                        // 10
	switchType e;                           // 10
	lightType f;                            // 10
	applianceType g;                        // 10
	enemyType h;                            // 10
	clutterType i;                          // 10
} objectInfo;                               // size = 10

typedef struct objectType
{
	SInt16 what;                            // 2
	objectInfo data;                        // 10
} objectType, *objectPtr;                   // total = 12

typedef struct scoresType
{
	Str31 banner;                           // 32       = 32
	Str15 names[kMaxScores];                // 16 * 10  = 160
	SInt32 scores[kMaxScores];              // 4 * 10   = 40
	UInt32 timeStamps[kMaxScores];          // 4 * 10   = 40
	SInt16 levels[kMaxScores];              // 2 * 10   = 20
} scoresType;                               // total    = 292

typedef struct gameType
{
	SInt16 version;                         // 2
	SInt16 wasStarsLeft;                    // 2
	SInt32 timeStamp;                       // 4
	Point where;                            // 4
	SInt32 score;                           // 4
	SInt32 unusedLong;                      // 4
	SInt32 unusedLong2;                     // 4
	SInt16 energy;                          // 2
	SInt16 bands;                           // 2
	SInt16 roomNumber;                      // 2
	SInt16 gliderState;                     // 2
	SInt16 numGliders;                      // 2
	SInt16 foil;                            // 2
	SInt16 unusedShort;                     // 2
	Boolean facing;                         // 1
	Boolean showFoil;                       // 1
} gameType;                                 // total = 40

typedef struct savedRoom
{
	SInt16 unusedShort;                     // 2
	Byte unusedByte;                        // 1
	Boolean visited;                        // 1
	objectType objects[kMaxRoomObs];        // 24 * 12
} savedRoom;                                // total = 292

typedef struct game2Type
{
	FSSpec house;                           // 70
	SInt16 version;                         // 2
	SInt16 wasStarsLeft;                    // 2
	SInt32 timeStamp;                       // 4
	Point where;                            // 4
	SInt32 score;                           // 4
	SInt32 unusedLong;                      // 4
	SInt32 unusedLong2;                     // 4
	SInt16 energy;                          // 2
	SInt16 bands;                           // 2
	SInt16 roomNumber;                      // 2
	SInt16 gliderState;                     // 2
	SInt16 numGliders;                      // 2
	SInt16 foil;                            // 2
	SInt16 nRooms;                          // 2
	Boolean facing;                         // 1
	Boolean showFoil;                       // 1
	savedRoom *savedData;                   // 4
} game2Type;                                // total = 114

typedef struct roomType
{
	Str27 name;                             // 28
	SInt16 bounds;                          // 2
	Byte leftStart;                         // 1
	Byte rightStart;                        // 1
	Byte unusedByte;                        // 1
	Boolean visited;                        // 1
	SInt16 background;                      // 2
	SInt16 tiles[kNumTiles];                // 2 * 8
	SInt16 floor;                           // 2
	SInt16 suite;                           // 2
	SInt16 openings;                        // 2
	SInt16 numObjects;                      // 2
	objectType objects[kMaxRoomObs];        // 24 * 12
} roomType, *roomPtr;                       // total = 348

typedef struct houseType
{
	SInt16 version;                         // 2
	SInt16 unusedShort;                     // 2
	SInt32 timeStamp;                       // 4
	SInt32 flags;                           // 4 (bit 0 = wardBit)
	Point initial;                          // 4
	Str255 banner;                          // 256
	Str255 trailer;                         // 256
	scoresType highScores;                  // 292
	gameType savedGame;                     // 40
	Boolean hasGame;                        // 1
	Boolean unusedBoolean;                  // 1
	SInt16 firstRoom;                       // 2
	SInt16 nRooms;                          // 2
	roomPtr rooms;                          // 348 * nRooms
} houseType, *housePtr;                     // total = 866 +

typedef struct gliderType
{
	Rect src;
	Rect mask;
	Rect dest;
	Rect whole;
	Rect destShadow;
	Rect wholeShadow;
	Rect clip;
	Rect enteredRect;
	SInt32 leftKey;
	SInt32 rightKey;
	SInt32 battKey;
	SInt32 bandKey;
	SInt16 hVel;
	SInt16 vVel;
	SInt16 wasHVel;
	SInt16 wasVVel;
	SInt16 vDesiredVel;
	SInt16 hDesiredVel;
	SInt16 mode;
	SInt16 frame;
	SInt16 wasMode;
	Boolean facing;
	Boolean tipped;
	Boolean sliding;
	Boolean ignoreLeft;
	Boolean ignoreRight;
	Boolean fireHeld;
	Boolean which;
	Boolean heldLeft;
	Boolean heldRight;
	Boolean dontDraw;
	Boolean ignoreGround;
} gliderType, *gliderPtr;

typedef struct hotObject
{
	Rect bounds;
	SInt16 action;
	SInt16 who;
	Boolean isOn;
	Boolean stillOver;
	Boolean doScrutinize;
} hotObject, *hotPtr;

#ifdef GP_USE_WINAPI_H
typedef struct savedType
{
	Rect dest;
	HDC map;
	SInt16 where;
	SInt16 who;
} savedType;
#endif

typedef struct sparkleType
{
	Rect bounds;
	SInt16 mode;
} sparkleType;

typedef struct flyingPtType
{
	Rect dest;
	Rect whole;
	SInt16 start;
	SInt16 stop;
	SInt16 mode;
	SInt16 loops;
	SInt16 hVel;
	SInt16 vVel;
} flyingPtType;

typedef struct flameType
{
	Rect dest;
	Rect src;
	SInt16 mode;
	SInt16 who;
} flameType;

typedef struct pendulumType
{
	Rect dest;
	Rect src;
	SInt16 mode;
	SInt16 where;
	SInt16 who;
	SInt16 link;
	Boolean toOrFro;
	Boolean active;
} pendulumType;

typedef struct boundsType
{
	Boolean left;
	Boolean top;
	Boolean right;
	Boolean bottom;
} boundsType;

typedef struct bandType
{
	Rect dest;
	SInt16 mode;
	SInt16 count;
	SInt16 hVel;
	SInt16 vVel;
} bandType;

typedef struct linksType
{
	SInt16 srcRoom;
	SInt16 srcObj;
	SInt16 destRoom;
	SInt16 destObj;
} linksType, *linksPtr;

typedef struct greaseType
{
	Rect dest;
	SInt16 mapNum;
	SInt16 mode;
	SInt16 who;
	SInt16 where;
	SInt16 start;
	SInt16 stop;
	SInt16 frame;
	SInt16 hotNum;
	Boolean isRight;
} greaseType;

typedef struct starType
{
	Rect dest;
	Rect src;
	SInt16 mode;
	SInt16 who;
	SInt16 link;
	SInt16 where;
} starType;

typedef struct shredType
{
	Rect bounds;
	SInt16 frame;
} shredType;

typedef struct dynaType
{
	Rect dest;
	Rect whole;
	SInt16 hVel;
	SInt16 vVel;
	SInt16 type;
	SInt16 count;
	SInt16 frame;
	SInt16 timer;
	SInt16 position;
	SInt16 room;
	Byte byte0;
	Byte byte1;
	Boolean moving;
	Boolean active;
} dynaType;

typedef struct objDataType
{
	SInt16 roomNum;         // room # object in (real number)
	SInt16 objectNum;       // obj. # in house (real number)
	SInt16 roomLink;        // room # object linked to (if any)
	SInt16 objectLink;      // obj. # object linked to (if any)
	SInt16 localLink;       // index in master list if exists
	SInt16 hotNum;          // index into active rects (if any)
	SInt16 dynaNum;         // index into dinahs (if any)
	objectType theObject;   // actual object data
} objDataType;

typedef struct demoType
{
	SInt32 frame;
	Byte key;
	Byte padding;
} demoType;

#define demoTypeByteSize 6

typedef struct retroLink
{
	SInt16 room;
	SInt16 object;
} retroLink;

#ifdef GP_USE_WINAPI_H
typedef struct houseSpec
{
	wchar_t path[MAX_PATH];
	wchar_t houseName[33];
	Str32 name;
	HICON hIcon;
} houseSpec, *houseSpecPtr;
#endif

#endif
