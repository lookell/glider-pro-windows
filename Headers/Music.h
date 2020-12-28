#ifndef MUSIC_H_
#define MUSIC_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Music.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

extern Boolean g_isMusicOn;
extern Boolean g_isPlayMusicIdle;
extern Boolean g_isPlayMusicGame;
extern Boolean g_failedMusic;
extern Boolean g_dontLoadMusic;

OSErr StartMusic (void);
void StopTheMusic (void);
void ToggleMusicWhilePlaying (void);
void SetMusicalMode (SInt16 newMode);
void InitMusic (HWND ownerWindow);
void KillMusic (void);

#endif
