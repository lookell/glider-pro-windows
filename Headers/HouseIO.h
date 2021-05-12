//============================================================================
//----------------------------------------------------------------------------
//                                 HouseIO.h
//----------------------------------------------------------------------------
//============================================================================

#ifndef HOUSE_IO_H_
#define HOUSE_IO_H_

#include "MacTypes.h"
#include "ResourceLoader.h"
#include "WinAPI.h"

extern Movie g_theMovie;
extern Rect g_movieRect;
extern SInt16 g_wasHouseVersion;
extern Boolean g_houseOpen;
extern Boolean g_fileDirty;
extern Boolean g_gameDirty;
extern Gp_HouseFile *g_theHouseFile;
extern Boolean g_changeLockStateOfHouse;
extern Boolean g_saveHouseLocked;
extern Boolean g_houseIsReadOnly;
extern Boolean g_hasMovie;
extern Boolean g_tvInRoom;

Boolean OpenHouse (HWND ownerWindow);
Boolean OpenSpecificHouse (PCWSTR filename, HWND ownerWindow);
Boolean SaveHouseAs (void);
Boolean ReadHouse (HWND ownerWindow, Boolean loadSplashScreen);
Boolean WriteHouse (HWND ownerWindow, Boolean checkIt);
Boolean CloseHouse (HWND ownerWindow);
Boolean QuerySaveChanges (HWND ownerWindow);
void YellowAlert (HWND ownerWindow, SInt16 whichAlert, SInt16 identifier);

#endif
