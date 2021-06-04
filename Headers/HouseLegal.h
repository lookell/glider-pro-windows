//============================================================================
//----------------------------------------------------------------------------
//                                HouseLegal.h
//----------------------------------------------------------------------------
//============================================================================

#ifndef HOUSE_LEGAL_H_
#define HOUSE_LEGAL_H_

#include "MacTypes.h"
#include "WinAPI.h"

extern Boolean g_isHouseChecks;

Boolean KeepObjectLegal (void);
void CheckHouseForProblems (HWND ownerWindow);

#endif
