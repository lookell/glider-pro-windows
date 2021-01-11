//============================================================================
//----------------------------------------------------------------------------
//                               Coordinates.h
//----------------------------------------------------------------------------
//============================================================================

#ifndef COORDINATES_H_
#define COORDINATES_H_

#include "MacTypes.h"
#include "WinAPI.h"

extern HWND g_coordWindow;
extern SInt16 g_isCoordH;
extern SInt16 g_isCoordV;
extern Boolean g_isCoordOpen;

void SetCoordinateHVD (SInt16 h, SInt16 v, SInt16 d);
void DeltaCoordinateD (SInt16 d);
void OpenCoordWindow (void);
void CloseCoordWindow (void);
void ToggleCoordinateWindow (void);

#endif
