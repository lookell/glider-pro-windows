#ifndef MUSIC_H_
#define MUSIC_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Music.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

extern Boolean isMusicOn;
extern Boolean isPlayMusicIdle;
extern Boolean isPlayMusicGame;
extern Boolean failedMusic;
extern Boolean dontLoadMusic;

OSErr StartMusic (void);
void StopTheMusic (void);
void ToggleMusicWhilePlaying (void);
void SetMusicalMode (SInt16 newMode);
#ifdef GP_USE_WINAPI_H
void InitMusic (HWND ownerWindow);
#endif
void KillMusic (void);

#endif
