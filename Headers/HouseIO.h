#ifndef HOUSE_IO_H_
#define HOUSE_IO_H_

//============================================================================
//----------------------------------------------------------------------------
//                                 HouseIO.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "ResourceLoader.h"
#include "WinAPI.h"
#endif

extern Movie theMovie;
extern Rect movieRect;
extern SInt16 wasHouseVersion;
extern Boolean houseOpen;
extern Boolean fileDirty;
extern Boolean gameDirty;
#ifdef GP_USE_WINAPI_H
extern Gp_HouseFile *g_theHouseFile;
#endif
extern Boolean changeLockStateOfHouse;
extern Boolean saveHouseLocked;
extern Boolean houseIsReadOnly;
extern Boolean hasMovie;
extern Boolean tvInRoom;

#ifdef GP_USE_WINAPI_H
Boolean OpenHouse (HWND ownerWindow);
Boolean OpenSpecificHouse (PCWSTR filename, HWND ownerWindow);
#endif
Boolean SaveHouseAs (void);
#ifdef GP_USE_WINAPI_H
Boolean ReadHouse (HWND ownerWindow);
Boolean WriteHouse (HWND ownerWindow, Boolean checkIt);
Boolean CloseHouse (HWND ownerWindow);
Boolean QuerySaveChanges (HWND ownerWindow);
void YellowAlert (HWND ownerWindow, SInt16 whichAlert, SInt16 identifier);
#endif

#endif
