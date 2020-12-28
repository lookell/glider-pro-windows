#ifndef SAVED_GAMES_H_
#define SAVED_GAMES_H_

//============================================================================
//----------------------------------------------------------------------------
//                                SavedGames.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderStructs.h"
#include "MacTypes.h"
#include "WinAPI.h"

extern gameType g_smallGame;

void SaveGame2 (HWND ownerWindow);
Boolean OpenSavedGame (HWND ownerWindow);
void SaveGame (HWND ownerWindow, Boolean doSave);

#endif
