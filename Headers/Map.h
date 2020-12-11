#ifndef MAP_H_
#define MAP_H_

//============================================================================
//----------------------------------------------------------------------------
//                                   Map.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

extern HWND mapWindow;
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
void DrawMapRoomImage (HDC hdc, SInt16 backgroundID, const Rect *theRect);
void UpdateMapWindow (void);
void OpenMapWindow (void);
void CloseMapWindow (void);
void ToggleMapWindow (void);
void MoveRoom (Point wherePt);

#endif
