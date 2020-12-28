#ifndef OBJECT_EDIT_H_
#define OBJECT_EDIT_H_

//============================================================================
//----------------------------------------------------------------------------
//                                ObjectEdit.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderDefines.h"
#include "MacTypes.h"
#include "WinAPI.h"

extern Rect g_roomObjectRects[kMaxRoomObs];
extern Rect g_leftStartGliderSrc;
extern Rect g_rightStartGliderSrc;
extern SInt16 g_objActive;

void DoSelectionClick (HWND hwnd, Point where, Boolean isDoubleClick);
void DoNewObjectClick (HWND ownerWindow, Point where);
void Gp_DeleteObject (void);
void DuplicateObject (HWND ownerWindow);
void MoveObject (SInt16 whichWay, Boolean shiftDown);
void DeselectObject (void);
Boolean ObjectHasHandle (SInt16 *direction, SInt16 *dist);
void HandleBlowerGlider (void);
void SelectNextObject (void);
void SelectPrevObject (void);
void GetThisRoomsObjRects (void);
void DrawThisRoomsObjects (void);
void HiliteAllObjects (void);
void GoToObjectInRoom (SInt16 object, SInt16 floor, SInt16 suite);
void GoToObjectInRoomNum (SInt16 object, SInt16 roomNum);
void StartMarqueeForActiveObject (void);

#endif
