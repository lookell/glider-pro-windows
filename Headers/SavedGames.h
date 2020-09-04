#ifndef SAVED_GAMES_H_
#define SAVED_GAMES_H_

//============================================================================
//----------------------------------------------------------------------------
//                                SavedGames.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderStructs.h"
#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

extern gameType smallGame;

#ifdef GP_USE_WINAPI_H
void SaveGame2 (HWND ownerWindow);
Boolean OpenSavedGame (HWND ownerWindow);
void SaveGame (HWND ownerWindow, Boolean doSave);
#endif

#endif
