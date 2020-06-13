#ifndef PLAY_H_
#define PLAY_H_

//============================================================================
//----------------------------------------------------------------------------
//                                   Play.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderStructs.h"
#include "MacTypes.h"
#include "WinAPI.h"

extern HDC		glidSrcMap;
extern HDC		glid2SrcMap;
extern HDC		glidMaskMap;

void NewGame (HWND ownerWindow, SInt16 mode);
void DoDemoGame (HWND ownerWindow);
void HideGlider (gliderPtr thisGlider);
void StrikeChime (void);
void RestoreEntireGameScreen (void);

#endif
