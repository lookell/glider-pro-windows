#ifndef MACTYPES_H_
#define MACTYPES_H_

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
typedef UInt8 Str255[256];

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

// unknown types

typedef const void *AEEventHandlerUPP;
typedef const void *AppleEvent;
typedef const void *CGrafPtr;
typedef const void *ColorSpec;
typedef const void *ControlHandle;
typedef const void *CTabHandle;
typedef const void *CursHandle;
typedef const void *Cursor;
typedef const void *DialogPtr;
typedef const void *EventRecord;
typedef const void *FSSpec;
typedef const void *FSSpecPtr;
typedef const void *GDHandle;
typedef const void *GWorldPtr;
typedef const void *Handle;
typedef const void *KeyMap;
typedef const void *MenuHandle;
typedef const void *Movie;
typedef const void *PixMapHandle;
typedef const void *Ptr;
typedef const void *RgnHandle;
typedef const void *SndCommand;
typedef const void *SndChannelPtr;
typedef const void *SndCallBackUPP;
typedef const void *WindowPtr;

#endif
