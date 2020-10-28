#ifndef INPUT_H_
#define INPUT_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Input.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderStructs.h"
#include "MacTypes.h"

extern demoType demoData[2000];
extern SInt16 demoIndex;
extern Boolean isEscPauseKey;
extern Boolean paused;

void DumpDemoData (void);
void DoCommandKeyQuit (void);
void DoCommandKeySave (void);
void GetDemoInput (gliderPtr thisGlider);
void GetInput (gliderPtr thisGlider);

#endif
