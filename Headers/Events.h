#ifndef EVENTS_H_
#define EVENTS_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Events.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

extern DWORD incrementModeTime;
extern Boolean doAutoDemo;
extern Boolean switchedOut;
extern Boolean ignoreDoubleClick;
extern HACCEL splashAccelTable;
extern HACCEL editAccelTable;

SInt16 BitchAboutColorDepth (HWND ownerWindow);
void HandleKeyEvent (HWND hwnd, BYTE vkey);
void HandleEvent (void);
void IgnoreThisClick (void);

#endif
