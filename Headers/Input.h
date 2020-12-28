#ifndef INPUT_H_
#define INPUT_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Input.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderStructs.h"
#include "MacTypes.h"

extern demoType g_demoData[2000];
extern SInt16 g_demoIndex;
extern Boolean g_isEscPauseKey;
extern Boolean g_paused;

void DumpDemoData (void);
void DoCommandKeyQuit (void);
void DoCommandKeySave (void);
void GetDemoInput (gliderPtr thisGlider);
void GetInput (gliderPtr thisGlider);

#endif
