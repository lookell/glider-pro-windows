#ifndef ENVIRON_H_
#define ENVIRON_H_

//============================================================================
//----------------------------------------------------------------------------
//                                 Environ.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

typedef struct macEnviron
{
	Rect screen;
	SInt16 isDepth;
	Boolean hasQT;
	Boolean hasDrag;
} macEnviron;

extern macEnviron thisMac;

void CheckOurEnvirons (void);
void CheckMemorySize (HWND ownerWindow);

#endif
