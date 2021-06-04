//============================================================================
//----------------------------------------------------------------------------
//                                  House.h
//----------------------------------------------------------------------------
//============================================================================

#ifndef HOUSE_H_
#define HOUSE_H_

#include "GliderDefines.h"
#include "GliderStructs.h"
#include "MacTypes.h"
#include "WinAPI.h"

extern houseType g_thisHouse;
extern linksPtr g_linksList;
extern Str32 g_thisHouseName;
extern SInt16 g_wasFloor;
extern SInt16 g_wasSuite;
extern retroLink g_retroLinkList[kMaxRoomObs];
extern Boolean g_houseUnlocked;

Boolean CreateNewHouse (HWND hwndOwner);
void InitializeEmptyHouse (void);
SInt16 RealRoomNumberCount (const houseType *house);
SInt16 GetFirstRoomNumber (void);
void WhereDoesGliderBegin (Rect *theRect, SInt16 mode);
SInt16 CountHouseLinks (void);
void GenerateLinksList (void);
void SortHouseObjects (void);
SInt16 CountRoomsVisited (const houseType *house);
void GenerateRetroLinks (void);
void DoGoToDialog (HWND ownerWindow);
void ConvertHouseVer1To2 (HWND ownerWindow);
void ShiftWholeHouse (SInt16 howFar, HWND ownerWindow);

#endif
