//============================================================================
//----------------------------------------------------------------------------
//                               RoomGraphics.h
//----------------------------------------------------------------------------
//============================================================================

#ifndef ROOM_GRAPHICS_H_
#define ROOM_GRAPHICS_H_

#include "GliderDefines.h"
#include "MacTypes.h"
#include "WinAPI.h"

extern Rect g_suppSrcRect;
extern HDC g_suppSrcMap;
extern Rect g_localRoomsDest[9];
extern Rect g_houseRect;
extern SInt16 g_numNeighbors;
extern SInt16 g_numLights;
extern SInt16 g_thisTiles[kNumTiles];
extern SInt16 g_localNumbers[9];
extern SInt16 g_thisBackground;
extern Boolean g_wardBitSet;

void DrawLocale (void);
void ReadyLevel (void);
void RedrawRoomLighting (void);

#endif
