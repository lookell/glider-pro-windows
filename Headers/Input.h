#ifndef INPUT_H_
#define INPUT_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Input.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderStructs.h"
#include "MacTypes.h"

extern demoPtr demoData;
extern SInt16 demoIndex;
extern Boolean isEscPauseKey;
extern Boolean paused;

void GetDemoInput (gliderPtr thisGlider);
void GetInput (gliderPtr thisGlider);

#endif
