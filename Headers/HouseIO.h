#ifndef HOUSE_IO_H_
#define HOUSE_IO_H_

//============================================================================
//----------------------------------------------------------------------------
//                                 HouseIO.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

Boolean OpenHouse (HWND ownerWindow);
Boolean OpenSpecificHouse (FSSpec *specs);
Boolean SaveHouseAs (void);
Boolean ReadHouse (HWND ownerWindow);
Boolean WriteHouse (HWND ownerWindow, Boolean checkIt);
Boolean CloseHouse (HWND ownerWindow);
void OpenHouseResFork (HWND ownerWindow);
void CloseHouseResFork (void);
Boolean QuerySaveChanges (HWND ownerWindow);
void YellowAlert (HWND ownerWindow, SInt16 whichAlert, SInt16 identifier);

#endif
