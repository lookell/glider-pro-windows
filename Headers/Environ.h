#ifndef ENVIRON_H_
#define ENVIRON_H_

//============================================================================
//----------------------------------------------------------------------------
//                                 Environ.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"

typedef struct macEnviron
{
	Rect screen;
	SInt16 isDepth;
	Boolean hasQT;
} macEnviron;

extern macEnviron thisMac;

void CheckOurEnvirons (void);

#endif
