#ifndef MACINTOSH_H_
#define MACINTOSH_H_

#include <stddef.h>
#include <stdint.h>

#ifndef __cplusplus
#define false 0
#define true 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#define nil NULL
#define pascal /* ... */

typedef uint8_t UInt8;
typedef int8_t SInt8;
typedef uint16_t UInt16;
typedef int16_t SInt16;
typedef uint32_t UInt32;
typedef int32_t SInt32;

typedef UInt8 Byte;
typedef UInt8 Boolean;
typedef SInt16 OSErr;

typedef UInt8 *StringPtr;
typedef UInt8 Str15[16];
typedef UInt8 Str27[28];
typedef UInt8 Str31[32];
typedef UInt8 Str32[33];
typedef UInt8 Str63[64];
typedef UInt8 Str255[256];

typedef void *Ptr;
typedef Ptr *Handle;

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

typedef struct AEEventHandlerUPP__ *AEEventHandlerUPP;
typedef struct AppleEvent__ *AppleEvent;
typedef struct CGrafPtr__ *CGrafPtr;
typedef struct ControlHandle__ *ControlHandle;
typedef struct CTabHandle__ *CTabHandle;
typedef struct CursHandle__ *CursHandle;
typedef struct Cursor__ *Cursor;
typedef struct DialogPtr__ *DialogPtr;
typedef struct EventRecord__ *EventRecord;
typedef struct GDHandle__ *GDHandle;
typedef CGrafPtr GWorldPtr;
typedef struct KeyMap__ *KeyMap;
typedef struct MenuHandle__ *MenuHandle;
typedef struct Movie__ *Movie;
typedef struct PixMapHandle__ *PixMapHandle;
typedef struct RgnHandle__ *RgnHandle;
typedef struct SndCommand__ *SndCommand;
typedef struct SndChannelPtr__ *SndChannelPtr;
typedef struct SndCallBackUPP__ *SndCallBackUPP;
typedef struct WindowPtr__ *WindowPtr;

#endif
