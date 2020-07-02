#ifndef HOUSE_H_
#define HOUSE_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  House.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderDefines.h"
#include "GliderStructs.h"
#include "MacTypes.h"
#include "WinAPI.h"

extern houseType thisHouse;
extern linksPtr linksList;
extern Str32 thisHouseName;
extern SInt16 srcLocations[kMaxRoomObs];
extern SInt16 destLocations[kMaxRoomObs];
extern SInt16 wasFloor;
extern SInt16 wasSuite;
extern retroLink retroLinkList[kMaxRoomObs];
extern Boolean houseUnlocked;

Boolean CreateNewHouse (HWND hwndOwner);
void InitializeEmptyHouse (void);
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
