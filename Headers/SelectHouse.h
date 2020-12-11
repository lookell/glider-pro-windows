#ifndef SELECT_HOUSE_H_
#define SELECT_HOUSE_H_

//============================================================================
//----------------------------------------------------------------------------
//                               SelectHouse.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderStructs.h"
#include "MacTypes.h"
#include "WinAPI.h"

typedef struct houseSpec
{
	WCHAR path[MAX_PATH];
	WCHAR houseName[33];
	Str32 name;
	int iconIndex;
	Boolean readOnly;
	Boolean hasMovie;
} houseSpec, *houseSpecPtr;

extern houseSpecPtr theHousesSpecs;
extern SInt16 housesFound;
extern SInt16 thisHouseIndex;
extern SInt16 maxFiles;
extern SInt16 willMaxFiles;
extern SInt16 demoHouseIndex;

void DoLoadHouse (HWND ownerWindow);
BOOL GetHouseFolderPath (LPWSTR buffer, DWORD cch);
void BuildHouseList (HWND ownerWindow);
void AddExtraHouse (const houseSpec *newHouse);

#endif
