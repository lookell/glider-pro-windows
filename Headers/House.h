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
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

extern houseType thisHouse;
extern linksPtr linksList;
extern Str32 thisHouseName;
extern SInt16 wasFloor;
extern SInt16 wasSuite;
extern retroLink retroLinkList[kMaxRoomObs];
extern Boolean houseUnlocked;

#ifdef GP_USE_WINAPI_H
Boolean CreateNewHouse (HWND hwndOwner);
#endif
void InitializeEmptyHouse (void);
SInt16 RealRoomNumberCount (void);
SInt16 GetFirstRoomNumber (void);
void WhereDoesGliderBegin (Rect *theRect, SInt16 mode);
Boolean HouseHasOriginalPicts (void);
SInt16 CountHouseLinks (void);
void GenerateLinksList (void);
void SortHouseObjects (void);
SInt16 CountRoomsVisited (void);
void GenerateRetroLinks (void);
#ifdef GP_USE_WINAPI_H
void DoGoToDialog (HWND ownerWindow);
#endif
void ConvertHouseVer1To2 (void);
void ShiftWholeHouse (SInt16 howFar);

#endif
