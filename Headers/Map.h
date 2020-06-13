#ifndef MAP_H_
#define MAP_H_

//============================================================================
//----------------------------------------------------------------------------
//                                   Map.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

extern HDC		nailSrcMap;
extern HWND		mapWindow;

Boolean ThisRoomVisibleOnMap (void);
void CenterMapOnRoom (SInt16 h, SInt16 v);
void FlagMapRoomsForUpdate (void);
void FindNewActiveRoomRect (void);
void UpdateMapWindow (void);
void ResizeMapWindow (SInt16 newH, SInt16 newV);
void OpenMapWindow (void);
void CloseMapWindow (void);
void ToggleMapWindow (void);
void HandleMapClick (EventRecord *theEvent);
void MoveRoom (Point wherePt);

#endif
