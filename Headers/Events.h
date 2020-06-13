#ifndef EVENTS_H_
#define EVENTS_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Events.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

SInt16 BitchAboutColorDepth (HWND ownerWindow);
void HandleKeyEvent (HWND hwnd, BYTE vkey);
void HandleEvent (void);
void HiliteAllWindows (void);
void IgnoreThisClick (void);

#endif
