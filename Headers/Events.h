#ifndef EVENTS_H_
#define EVENTS_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Events.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

extern UInt32 incrementModeTime;
extern Boolean doAutoDemo;
extern Boolean switchedOut;
extern Boolean ignoreDoubleClick;
#ifdef GP_USE_WINAPI_H
extern HACCEL splashAccelTable;
extern HACCEL editAccelTable;
#endif

#ifdef GP_USE_WINAPI_H
void HandleKeyEvent (HWND hwnd, BYTE vkey);
#endif
void HandleEvent (void);
void IgnoreThisClick (void);

#endif
