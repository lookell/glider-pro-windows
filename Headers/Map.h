#ifndef MAP_H_
#define MAP_H_

//============================================================================
//----------------------------------------------------------------------------
//                                   Map.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

extern HWND g_mapWindow;
extern SInt16 g_isMapH;
extern SInt16 g_isMapV;
extern SInt16 g_mapRoomsWide;
extern SInt16 g_mapRoomsHigh;
extern SInt16 g_mapLeftRoom;
extern SInt16 g_mapTopRoom;
extern Boolean g_isMapOpen;
extern Boolean g_doPrettyMap;

Boolean ThisRoomVisibleOnMap (void);
void CenterMapOnRoom (SInt16 h, SInt16 v);
void FlagMapRoomsForUpdate (void);
void FindNewActiveRoomRect (void);
void DrawMapRoomImage (HDC hdc, SInt16 backgroundID, const Rect *theRect);
void UpdateMapWindow (void);
void OpenMapWindow (void);
void CloseMapWindow (void);
void ToggleMapWindow (void);
void MoveRoom (Point wherePt);

#endif
