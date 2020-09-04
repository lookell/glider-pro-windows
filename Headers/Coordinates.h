#ifndef COORDINATES_H_
#define COORDINATES_H_

//============================================================================
//----------------------------------------------------------------------------
//                               Coordinates.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

#ifdef GP_USE_WINAPI_H
extern HWND coordWindow;
#endif
extern SInt16 isCoordH;
extern SInt16 isCoordV;
extern Boolean isCoordOpen;

void SetCoordinateHVD (SInt16 h, SInt16 v, SInt16 d);
void DeltaCoordinateD (SInt16 d);
void UpdateCoordWindow (void);
void OpenCoordWindow (void);
void CloseCoordWindow (void);
void ToggleCoordinateWindow (void);

#endif
