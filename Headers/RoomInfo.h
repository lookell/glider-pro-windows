#ifndef ROOM_INFO_H_
#define ROOM_INFO_H_

//============================================================================
//----------------------------------------------------------------------------
//                                 RoomInfo.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderDefines.h"
#include "MacTypes.h"

extern Rect tileSrcRect;
extern CGrafPtr tileSrcMap;
extern SInt16 tempTiles[kNumTiles];

void DoRoomInfo (void);
Boolean PictIDExists (SInt16 theID);

#endif
