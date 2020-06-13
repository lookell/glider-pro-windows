#ifndef COORDINATES_H_
#define COORDINATES_H_

//============================================================================
//----------------------------------------------------------------------------
//                               Coordinates.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"

void SetCoordinateHVD (SInt16 h, SInt16 v, SInt16 d);
void DeltaCoordinateD (SInt16 d);
void UpdateCoordWindow (void);
void OpenCoordWindow (void);
void CloseCoordWindow (void);
void ToggleCoordinateWindow (void);

#endif
