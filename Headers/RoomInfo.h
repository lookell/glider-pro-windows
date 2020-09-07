#ifndef ROOM_INFO_H_
#define ROOM_INFO_H_

//============================================================================
//----------------------------------------------------------------------------
//                                 RoomInfo.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

extern Rect tileSrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC tileSrcMap;
#endif

#ifdef GP_USE_WINAPI_H
void DoRoomInfo (HWND hwndOwner);
#endif

#endif
