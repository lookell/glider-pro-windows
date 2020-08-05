#ifndef SOUND_H_
#define SOUND_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Sound.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

extern Boolean dontLoadSounds;
extern Boolean isSoundOn;

void PlayPrioritySound (SInt16 which, SInt16 priority);
void FlushAnyTriggerPlaying (void);
OSErr LoadTriggerSound (SInt16 soundID);
void DumpTriggerSound (void);
void InitSound (HWND ownerWindow);
void KillSound (void);
SInt32 SoundBytesNeeded (void);
void TellHerNoSounds (HWND ownerWindow);

#endif
