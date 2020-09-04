#ifndef MAP_H_
#define MAP_H_

//============================================================================
//----------------------------------------------------------------------------
//                                   Map.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

extern Rect nailSrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC nailSrcMap;
extern HWND mapWindow;
#endif
extern SInt16 isMapH;
extern SInt16 isMapV;
extern SInt16 mapRoomsWide;
extern SInt16 mapRoomsHigh;
extern SInt16 mapLeftRoom;
extern SInt16 mapTopRoom;
extern Boolean isMapOpen;
extern Boolean doPrettyMap;

Boolean ThisRoomVisibleOnMap (void);
void CenterMapOnRoom (SInt16 h, SInt16 v);
void FlagMapRoomsForUpdate (void);
void FindNewActiveRoomRect (void);
void UpdateMapWindow (void);
void OpenMapWindow (void);
void CloseMapWindow (void);
void ToggleMapWindow (void);
void HandleMapClick (SInt16 clickX, SInt16 clickY);
void MoveRoom (Point wherePt);

#endif
