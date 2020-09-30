#ifndef OBJECT_EDIT_H_
#define OBJECT_EDIT_H_

//============================================================================
//----------------------------------------------------------------------------
//                                ObjectEdit.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderDefines.h"
#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

extern Rect roomObjectRects[kMaxRoomObs];
extern Rect leftStartGliderSrc;
extern Rect rightStartGliderSrc;
extern SInt16 objActive;

#ifdef GP_USE_WINAPI_H
void DoSelectionClick (HWND hwnd, Point where, Boolean isDoubleClick);
void DoNewObjectClick (HWND ownerWindow, Point where);
#endif
void Gp_DeleteObject (void);
#ifdef GP_USE_WINAPI_H
void DuplicateObject (HWND ownerWindow);
#endif
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
