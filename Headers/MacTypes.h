#ifndef MAC_TYPES_H_
#define MAC_TYPES_H_

#include <stdint.h>

typedef uint8_t UInt8;
typedef int8_t SInt8;

typedef uint16_t UInt16;
typedef int16_t SInt16;

typedef uint32_t UInt32;
typedef int32_t SInt32;

typedef SInt16 OSErr;
#define noErr 0

typedef UInt8 Byte;
typedef UInt8 Boolean;
#ifndef __cplusplus
#define false 0
#define true 1
#endif

typedef Byte *StringPtr;
typedef const Byte *ConstStringPtr;
typedef Byte Str15[16];
typedef Byte Str27[28];
typedef Byte Str31[32];
typedef Byte Str32[33];
typedef Byte Str63[64];
typedef Byte Str255[256];

typedef void *Ptr;
typedef Ptr *Handle;
#define nil NULL

typedef struct Point {
	SInt16 v;
	SInt16 h;
} Point;

typedef struct Rect {
	SInt16 top;
	SInt16 left;
	SInt16 bottom;
	SInt16 right;
} Rect;

typedef struct Pattern {
	UInt8 pat[8];
} Pattern;

typedef struct RGBColor {
	UInt16 red;
	UInt16 green;
	UInt16 blue;
} RGBColor;

typedef struct ColorSpec {
	SInt16 value;
	RGBColor rgb;
} ColorSpec;

typedef struct FSSpec {
	SInt16 vRefNum;
	SInt32 parID;
	Str63 name;
} FSSpec, *FSSpecPtr;

// unknown types

typedef struct AEEventHandlerUPP__* AEEventHandlerUPP;
typedef struct AppleEvent__* AppleEvent;
typedef struct CGrafPtr__* CGrafPtr;
typedef struct ControlHandle__* ControlHandle;
typedef struct CTabHandle__* CTabHandle;
typedef struct DialogPtr__* DialogPtr;
typedef struct EventRecord__* EventRecord;
typedef struct GDHandle__* GDHandle;
typedef struct KeyMap__* KeyMap;
typedef struct Movie__* Movie;
typedef struct PixMapHandle__* PixMapHandle;
typedef struct RgnHandle__* RgnHandle;
typedef struct WindowPtr__* WindowPtr;

#endif
