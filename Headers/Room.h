//============================================================================
//----------------------------------------------------------------------------
//                                   Room.h
//----------------------------------------------------------------------------
//============================================================================

#ifndef ROOM_H_
#define ROOM_H_

#include "GliderStructs.h"
#include "MacTypes.h"
#include "WinAPI.h"

extern roomPtr g_thisRoom;
extern Rect g_backSrcRect;
extern HDC g_backSrcMap;
extern SInt16 g_thisRoomNumber;
extern SInt16 g_previousRoom;
extern SInt16 g_leftThresh;
extern SInt16 g_rightThresh;
extern SInt16 g_lastBackground;
extern Boolean g_autoRoomEdit;
extern Boolean g_newRoomNow;
extern Boolean g_noRoomAtAll;
extern Boolean g_leftOpen;
extern Boolean g_rightOpen;
extern Boolean g_topOpen;
extern Boolean g_bottomOpen;
extern Boolean g_doBitchDialogs;

void SetInitialTiles (SInt16 background, SInt16 *theTiles);
Boolean CreateNewRoom (HWND ownerWindow, SInt16 h, SInt16 v);
void ReadyBackground (SInt16 theID, const SInt16 *theTiles);
void ReflectCurrentRoom (Boolean forceMapRedraw);
void CopyRoomToThisRoom (SInt16 roomNumber);
void CopyThisRoomToRoom (void);
void ForceThisRoom (SInt16 roomNumber);
Boolean RoomExists (const houseType *house, SInt16 suite, SInt16 floor, SInt16 *roomNum);
Boolean RoomNumExists (SInt16 roomNum);
void DeleteRoom (HWND ownerWindow, Boolean doWarn);
SInt16 DoesNeighborRoomExist (SInt16 whichNeighbor);
void SelectNeighborRoom (SInt16 whichNeighbor);
SInt16 GetNeighborRoomNumber (const houseType *house, SInt16 roomNumber, SInt16 which);
Boolean GetRoomFloorSuite (const houseType *house, SInt16 room, SInt16 *floor, SInt16 *suite);
SInt16 GetRoomNumber (SInt16 floor, SInt16 suite);
Boolean IsRoomAStructure (SInt16 roomNum);
void DetermineRoomOpenings (void);
SInt16 GetNumberOfLights (SInt16 where);
Boolean IsShadowVisible (void);

#endif
