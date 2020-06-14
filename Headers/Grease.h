#ifndef GREASE_H_
#define GREASE_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Grease.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderStructs.h"
#include "MacTypes.h"

extern greasePtr grease;
extern SInt16 numGrease;

void HandleGrease (void);
SInt16 ReBackUpGrease (SInt16 where, SInt16 who);
SInt16 AddGrease (SInt16 where, SInt16 who, SInt16 h, SInt16 v,
	SInt16 distance, Boolean isRight);
void SpillGrease (SInt16 who, SInt16 index);
void RedrawAllGrease (void);

#endif
