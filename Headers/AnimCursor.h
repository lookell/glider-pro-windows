#ifndef ANIM_CURSOR_H_
#define ANIM_CURSOR_H_

//============================================================================
//----------------------------------------------------------------------------
//                                AnimCursor.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"

void LoadCursors (void);
void DisposCursors (void);
void InitCursor (void);
void IncrementCursor (void);
void DecrementCursor (void);
void SpinCursor (SInt16 incrementIndex);
void BackSpinCursor (SInt16 decrementIndex);

#endif
