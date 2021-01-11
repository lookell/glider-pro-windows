//============================================================================
//----------------------------------------------------------------------------
//                                  Grease.h
//----------------------------------------------------------------------------
//============================================================================

#ifndef GREASE_H_
#define GREASE_H_

#include "MacTypes.h"

void HandleGrease (void);
SInt16 ReBackUpGrease (SInt16 where, SInt16 who);
SInt16 AddGrease (SInt16 where, SInt16 who, SInt16 h, SInt16 v,
	SInt16 distance, Boolean isRight);
void SpillGrease (SInt16 who, SInt16 index);
void RedrawAllGrease (void);
void ZeroGrease (void);

#endif
