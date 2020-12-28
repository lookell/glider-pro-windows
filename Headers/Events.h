#ifndef EVENTS_H_
#define EVENTS_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Events.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

extern UInt32 g_incrementModeTime;
extern Boolean g_doAutoDemo;
extern Boolean g_switchedOut;
extern Boolean g_ignoreDoubleClick;
extern HACCEL g_splashAccelTable;
extern HACCEL g_editAccelTable;

void HandleEvent (void);
void IgnoreThisClick (void);

#endif
