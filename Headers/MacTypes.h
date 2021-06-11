#ifndef MAC_TYPES_H_
#define MAC_TYPES_H_

#include <stddef.h>
#include <stdint.h>

//
// Macintosh fixed-sized integer types
//

typedef uint8_t UInt8;
typedef int8_t SInt8;
typedef uint16_t UInt16;
typedef int16_t SInt16;
typedef uint32_t UInt32;
typedef int32_t SInt32;

//
// Macintosh error integer type
//

typedef SInt16 OSErr;
#define noErr 0

//
// Macintosh boolean type
//

typedef UInt8 Boolean;
#ifndef __cplusplus
# ifndef false
#  define false 0
# endif
# ifndef true
#  define true 1
# endif
#endif

//
// Macintosh Pascal-style string types
//

typedef UInt8 Byte;
typedef Byte *StringPtr;
typedef const Byte *ConstStringPtr;
typedef Byte Str15[16];
typedef Byte Str27[28];
typedef Byte Str31[32];
typedef Byte Str63[64];
typedef Byte Str255[256];

//
// Macintosh pointer and handle types
//

#define nil NULL

//
// Macintosh QuickDraw structures
//

typedef struct Point
{
	SInt16 v;
	SInt16 h;
} Point;

typedef struct Rect
{
	SInt16 top;
	SInt16 left;
	SInt16 bottom;
	SInt16 right;
} Rect;

//
// Macintosh file specification
//

typedef struct FSSpec
{
	SInt16 vRefNum;
	SInt32 parID;
	Str63 name;
} FSSpec;

//
// Unknown types (this list should be kept as short as possible)
//

typedef struct Movie__* Movie;

#endif
