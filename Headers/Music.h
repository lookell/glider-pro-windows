#ifndef MUSIC_H_
#define MUSIC_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Music.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

OSErr StartMusic (void);
void StopTheMusic (void);
void ToggleMusicWhilePlaying (void);
void SetMusicalMode (SInt16 newMode);
void InitMusic (HWND ownerWindow);
void KillMusic (void);
SInt32 MusicBytesNeeded (void);
void TellHerNoMusic (HWND ownerWindow);

#endif
