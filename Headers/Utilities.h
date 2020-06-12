#ifndef UTILITIES_H_
#define UTILITIES_H_

//============================================================================
//----------------------------------------------------------------------------
//                                Utilities.h
//----------------------------------------------------------------------------
//============================================================================


#include "MacTypes.h"
#include "WinAPI.h"


OSErr CreateOffScreenGWorld (HDC *theGWorld, Rect *bounds, SInt16 depth);
void DisposeGWorld (HDC theGWorld);

#endif
