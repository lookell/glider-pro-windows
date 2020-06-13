#ifndef HOUSE_H_
#define HOUSE_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  House.h
//----------------------------------------------------------------------------
//============================================================================


#include "MacTypes.h"
#include "WinAPI.h"


extern Str32		thisHouseName;
extern Boolean		houseUnlocked;

Boolean CreateNewHouse (void);
Boolean InitializeEmptyHouse (HWND ownerWindow);
SInt16 RealRoomNumberCount (void);
SInt16 GetFirstRoomNumber (void);
void WhereDoesGliderBegin (Rect *theRect, SInt16 mode);
Boolean HouseHasOriginalPicts (void);
SInt16 CountHouseLinks (void);
void GenerateLinksList (void);
void SortRoomsObjects (SInt16 which);
void SortHouseObjects (void);
SInt16 CountRoomsVisited (void);
void GenerateRetroLinks (void);
void DoGoToDialog (HWND ownerWindow);
void ConvertHouseVer1To2 (void);
void ShiftWholeHouse (SInt16 howFar);

#endif
