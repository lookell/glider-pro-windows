#ifndef EVENTS_H_
#define EVENTS_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Events.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

extern SInt32 incrementModeTime;
extern UInt32 doubleTime;
extern SInt16 idleMode;
extern Boolean doAutoDemo;
extern Boolean switchedOut;
extern HACCEL splashAccelTable;
extern HACCEL editAccelTable;

SInt16 BitchAboutColorDepth (HWND ownerWindow);
void HandleKeyEvent (HWND hwnd, BYTE vkey);
void HandleEvent (void);
void HiliteAllWindows (void);
void IgnoreThisClick (void);

#endif
