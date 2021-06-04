//============================================================================
//----------------------------------------------------------------------------
//                                  Input.h
//----------------------------------------------------------------------------
//============================================================================

#ifndef INPUT_H_
#define INPUT_H_

#include "GliderStructs.h"
#include "MacTypes.h"
#include "WinAPI.h"

extern demoType g_demoData[2000];
extern SInt16 g_demoIndex;
extern Boolean g_isEscPauseKey;
extern Boolean g_paused;

void DumpDemoData (void);
void DoCommandKeyQuit (HWND ownerWindow);
void DoCommandKeySave (HWND ownerWindow);
void GetDemoInput (gliderPtr thisGlider);
void GetInput (gliderPtr thisGlider);

#endif
