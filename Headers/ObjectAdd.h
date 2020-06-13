#ifndef OBJECT_ADD_H_
#define OBJECT_ADD_H_

//============================================================================
//----------------------------------------------------------------------------
//                                ObjectAdd.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

Boolean AddNewObject (HWND ownerWindow, Point where, SInt16 what, Boolean showItNow);
SInt16 FindObjectSlotInRoom (SInt16 roomNumber);
Boolean DoesRoomNumHaveObject (SInt16 room, SInt16 what);
void ShoutNoMoreObjects (HWND ownerWindow);

#endif
