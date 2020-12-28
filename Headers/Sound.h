#ifndef SOUND_H_
#define SOUND_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Sound.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

extern Boolean g_dontLoadSounds;
extern Boolean g_isSoundOn;

void PlayPrioritySound (SInt16 which, SInt16 priority);
void FlushAnyTriggerPlaying (void);
OSErr LoadTriggerSound (SInt16 soundID);
void DumpTriggerSound (void);
void InitSound (HWND ownerWindow);
void KillSound (void);

#endif
