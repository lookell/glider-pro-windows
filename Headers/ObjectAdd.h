#ifndef OBJECT_ADD_H_
#define OBJECT_ADD_H_

//============================================================================
//----------------------------------------------------------------------------
//                                ObjectAdd.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

extern SInt16 wasFlower;

#ifdef GP_USE_WINAPI_H
Boolean AddNewObject (HWND ownerWindow, Point where, SInt16 what, Boolean showItNow);
#endif
SInt16 FindObjectSlotInRoom (SInt16 roomNumber);
Boolean DoesRoomNumHaveObject (SInt16 room, SInt16 what);

#endif
