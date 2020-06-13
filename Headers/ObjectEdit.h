#ifndef OBJECT_EDIT_H_
#define OBJECT_EDIT_H_

//============================================================================
//----------------------------------------------------------------------------
//                                ObjectEdit.h
//----------------------------------------------------------------------------
//============================================================================

#pragma once

#include "MacTypes.h"
#include "WinAPI.h"

extern Rect			roomObjectRects[];
extern SInt16		objActive;

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

#endif
