#ifndef SOUND_H_
#define SOUND_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Sound.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

extern Boolean dontLoadSounds;
extern Boolean isSoundOn;

void PlayPrioritySound (SInt16 which, SInt16 priority);
void FlushAnyTriggerPlaying (void);
OSErr LoadTriggerSound (SInt16 soundID);
void DumpTriggerSound (void);
#ifdef GP_USE_WINAPI_H
void InitSound (HWND ownerWindow);
#endif
void KillSound (void);

#endif
