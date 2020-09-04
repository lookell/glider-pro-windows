#ifndef ROOM_GRAPHICS_H_
#define ROOM_GRAPHICS_H_

//============================================================================
//----------------------------------------------------------------------------
//                               RoomGraphics.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderDefines.h"
#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

extern Rect suppSrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC suppSrcMap;
#endif
extern Rect localRoomsDest[9];
extern Rect houseRect;
extern SInt16 numNeighbors;
extern SInt16 numLights;
extern SInt16 thisTiles[kNumTiles];
extern SInt16 localNumbers[9];
extern SInt16 thisBackground;
extern Boolean wardBitSet;

void DrawLocale (void);
void ReadyLevel (void);
void RedrawRoomLighting (void);

#endif
