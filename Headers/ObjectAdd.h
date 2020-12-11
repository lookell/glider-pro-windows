#ifndef OBJECT_ADD_H_
#define OBJECT_ADD_H_

//============================================================================
//----------------------------------------------------------------------------
//                                ObjectAdd.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

extern SInt16 wasFlower;

Boolean AddNewObject (HWND ownerWindow, Point where, SInt16 what, Boolean showItNow);
SInt16 FindObjectSlotInRoom (SInt16 roomNumber);
Boolean DoesRoomNumHaveObject (SInt16 room, SInt16 what);

#endif
