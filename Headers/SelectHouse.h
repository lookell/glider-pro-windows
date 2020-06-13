#ifndef SELECT_HOUSE_H_
#define SELECT_HOUSE_H_

//============================================================================
//----------------------------------------------------------------------------
//                               SelectHouse.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderStructs.h"
#include "WinAPI.h"

void DoLoadHouse (HWND ownerWindow);
void BuildHouseList (HWND ownerWindow);
void AddExtraHouse (houseSpec *newHouse);

#endif
