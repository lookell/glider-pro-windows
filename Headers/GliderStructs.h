//============================================================================
//----------------------------------------------------------------------------
//								GliderStructs.h
//----------------------------------------------------------------------------
//============================================================================


//#include <QDOffscreen.h>
#include "Macintosh.h"


typedef struct
{
	Point		topLeft;				// 4
	SInt16		distance;				// 2
	Boolean		initial;				// 1
	Boolean		state;					// 1		              F. lf. dn. rt. up
	Byte		vector;					// 1		| x | x | x | x | 8 | 4 | 2 | 1 |
	Byte		tall;					// 1
} blowerType;							// total = 10

typedef struct
{
	Rect		bounds;					// 8
	SInt16		pict;					// 2
} furnitureType;						// total = 10

typedef struct
{
	Point		topLeft;				// 4
	SInt16		length;					// 2 grease spill
	SInt16		points;					// 2 invis bonus
	Boolean		state;					// 1
	Boolean		initial;				// 1
} bonusType;							// total = 10

typedef struct
{
	Point		topLeft;				// 4
	SInt16		tall;					// 2 invis transport
	SInt16		where;					// 2
	Byte		who;					// 1
	Byte		wide;					// 1
} transportType;						// total = 10

typedef struct
{
	Point		topLeft;				// 4
	SInt16		delay;					// 2
	SInt16		where;					// 2
	Byte		who;					// 1
	Byte		type;					// 1
} switchType;							// total = 10

typedef struct
{
	Point		topLeft;				// 4
	SInt16		length;					// 2
	Byte		byte0;					// 1
	Byte		byte1;					// 1
	Boolean		initial;				// 1
	Boolean		state;					// 1
} lightType;							// total = 10

typedef struct
{
	Point		topLeft;				// 4
	SInt16		height;					// 2 toaster, pict ID
	Byte		byte0;					// 1
	Byte		delay;					// 1
	Boolean		initial;				// 1
	Boolean		state;					// 1
} applianceType;						// total = 10

typedef struct
{
	Point		topLeft;					// 4
	SInt16		length;						// 2
	Byte		delay;						// 1
	Byte		byte0;						// 1
	Boolean		initial;					// 1
	Boolean		state;						// 1
} enemyType;								// total = 10

typedef struct
{
	Rect		bounds;						// 8
	SInt16		pict;						// 2
} clutterType;								// total = 10

typedef struct
{
	SInt16		what;						// 2
	union
	{
		blowerType		a;
		furnitureType	b;
		bonusType		c;
		transportType	d;
		switchType		e;
		lightType		f;
		applianceType	g;
		enemyType		h;
		clutterType		i;
	} data;									// 10
} objectType, *objectPtr;					// total = 12

typedef struct
{
	Str31			banner;					// 32		= 32
	Str15			names[kMaxScores];		// 16 * 10	= 160
	SInt32			scores[kMaxScores];		// 4 * 10	= 40
	UInt32			timeStamps[kMaxScores];	// 4 * 10	= 40
	SInt16			levels[kMaxScores];		// 2 * 10	= 20
} scoresType;								// total 	= 292

typedef struct
{
	SInt16		version;					// 2
	SInt16		wasStarsLeft;				// 2
	SInt32		timeStamp;					// 4
	Point		where;						// 4
	SInt32		score;						// 4
	SInt32		unusedLong;					// 4
	SInt32		unusedLong2;				// 4
	SInt16		energy;						// 2
	SInt16		bands;						// 2
	SInt16		roomNumber;					// 2
	SInt16		gliderState;				// 2
	SInt16		numGliders;					// 2
	SInt16		foil;						// 2
	SInt16		unusedShort;				// 2
	Boolean		facing;						// 1
	Boolean		showFoil;					// 1
} gameType;									// total = 40

typedef struct
{
	SInt16		unusedShort;				// 2
	Byte		unusedByte;					// 1
	Boolean		visited;					// 1
	objectType	objects[kMaxRoomObs];		// 24 * 12
} savedRoom, *savedRoomPtr;					// total = 292

typedef struct
{
	FSSpec			house;					// 70
	SInt16			version;				// 2
	SInt16			wasStarsLeft;			// 2
	SInt32			timeStamp;				// 4
	Point			where;					// 4
	SInt32			score;					// 4
	SInt32			unusedLong;				// 4
	SInt32			unusedLong2;			// 4
	SInt16			energy;					// 2
	SInt16			bands;					// 2
	SInt16			roomNumber;				// 2
	SInt16			gliderState;			// 2
	SInt16			numGliders;				// 2
	SInt16			foil;					// 2
	SInt16			nRooms;					// 2
	Boolean			facing;					// 1
	Boolean			showFoil;				// 1
	savedRoomPtr	savedData;				// 4
} game2Type, *gamePtr;						// total = 114

typedef struct
{
	Str27		name;						// 28
	SInt16		bounds;						// 2
	Byte		leftStart;					// 1
	Byte		rightStart;					// 1
	Byte		unusedByte;					// 1
	Boolean		visited;					// 1
	SInt16		background;					// 2
	SInt16		tiles[kNumTiles];			// 2 * 8
	SInt16		floor, suite;				// 2 + 2
	SInt16		openings;					// 2
	SInt16		numObjects;					// 2
	objectType	objects[kMaxRoomObs];		// 24 * 12
} roomType, *roomPtr;						// total = 348

typedef struct
{
	SInt16		version;					// 2
	SInt16		unusedShort;				// 2
	SInt32		timeStamp;					// 4
	SInt32		flags;						// 4 (bit 0 = wardBit)
	Point		initial;					// 4
	Str255		banner;						// 256
	Str255		trailer;					// 256
	scoresType	highScores;					// 292
	gameType	savedGame;					// 40
	Boolean		hasGame;					// 1
	Boolean		unusedBoolean;				// 1
	SInt16		firstRoom;					// 2
	SInt16		nRooms;						// 2
	roomPtr		rooms;						// 348 * nRooms
} houseType, *housePtr, **houseHand;		// total = 866 +

typedef struct
{
	Rect		src, mask, dest, whole;
	Rect		destShadow, wholeShadow;
	Rect		clip, enteredRect;
	SInt32		leftKey, rightKey;
	SInt32		battKey, bandKey;
	SInt16		hVel, vVel;
	SInt16		wasHVel, wasVVel;
	SInt16		vDesiredVel, hDesiredVel;
	SInt16		mode, frame, wasMode;
	Boolean		facing, tipped;
	Boolean		sliding, ignoreLeft, ignoreRight;
	Boolean		fireHeld, which;
	Boolean		heldLeft, heldRight;
	Boolean		dontDraw, ignoreGround;
} gliderType, *gliderPtr;

typedef struct
{
	Rect		bounds;
	SInt16		action;
	SInt16		who;
	Boolean		isOn, stillOver;
	Boolean		doScrutinize;
} hotObject, *hotPtr;

typedef struct
{
	Rect		dest;
	GWorldPtr	map;
	SInt16		where;
	SInt16		who;
} savedType, *savedPtr;

typedef struct
{
	Rect		bounds;
	SInt16		mode;
} sparkleType, *sparklePtr;

typedef struct
{
	Rect		dest, whole;
	SInt16		start;
	SInt16		stop;
	SInt16		mode;
	SInt16		loops;
	SInt16		hVel, vVel;
} flyingPtType, *flyingPtPtr;

typedef struct
{
	Rect		dest, src;
	SInt16		mode;
	SInt16		who;
} flameType, *flamePtr;

typedef struct
{
	Rect		dest, src;
	SInt16		mode, where;
	SInt16		who, link;
	Boolean		toOrFro, active;
} pendulumType, *pendulumPtr;

typedef struct
{
	Boolean		left;
	Boolean		top;
	Boolean		right;
	Boolean		bottom;
} boundsType, *boundsPtr, **boundsHand;

typedef struct
{
	Rect		dest;
	SInt16		mode, count;
	SInt16		hVel, vVel;
} bandType, *bandPtr;

typedef struct
{
	SInt16		srcRoom, srcObj;
	SInt16		destRoom, destObj;
} linksType, *linksPtr;

typedef struct
{
	Rect		dest;
	SInt16		mapNum, mode;
	SInt16		who, where;
	SInt16		start, stop;
	SInt16		frame, hotNum;
	Boolean		isRight;
} greaseType, *greasePtr;

typedef struct
{
	Rect		dest, src;
	SInt16		mode, who;
	SInt16		link, where;
} starType, *starPtr;

typedef struct
{
	Rect		bounds;
	SInt16		frame;
} shredType, *shredPtr;

typedef struct
{
	Rect		dest;
	Rect		whole;
	SInt16		hVel, vVel;
	SInt16		type, count;
	SInt16		frame, timer;
	SInt16		position, room;
	Byte		byte0, byte1;
	Boolean		moving, active;
} dynaType, *dynaPtr;

typedef struct
{
	SInt16		roomNum;	// room # object in (real number)
	SInt16		objectNum;	// obj. # in house (real number)
	SInt16		roomLink;	// room # object linked to (if any)
	SInt16		objectLink;	// obj. # object linked to (if any)
	SInt16		localLink;	// index in master list if exists
	SInt16		hotNum;		// index into active rects (if any)
	SInt16		dynaNum;	// index into dinahs (if any)
	objectType	theObject;	// actual object data
} objDataType, *objDataPtr;

typedef struct
{
	SInt32		frame;
	Byte		key;
	Byte		padding;
} demoType, *demoPtr;

#define demoTypeByteSize 6

typedef struct
{
	SInt16		room;
	SInt16		object;
} retroLink, *retroLinkPtr;


