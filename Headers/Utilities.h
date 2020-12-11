#ifndef UTILITIES_H_
#define UTILITIES_H_

//============================================================================
//----------------------------------------------------------------------------
//                                Utilities.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "ResourceLoader.h"
#include "WinAPI.h"
#endif

#define GP_BUILTIN_ASSETS  NULL

SInt16 RandomInt (SInt16 range);
void InitRandomLongQUS (void);
__declspec(noreturn) void RedAlert (SInt16 errorNumber);
#ifdef GP_USE_WINAPI_H
HDC CreateOffScreenGWorld (const Rect *bounds, SInt16 depth);
void DisposeGWorld (HDC theGWorld);
void LoadGraphic (HDC hdc, Gp_HouseFile *houseFile, SInt16 resID);
void LoadScaledGraphic (HDC hdc, Gp_HouseFile *houseFile, SInt16 resID, const Rect *theRect);
void DrawCIcon (HDC hdc, SInt16 theID, SInt16 h, SInt16 v);
HFONT CreateTahomaFont (LONG height, LONG weight);
#endif
void WaitForInputEvent (UInt16 seconds);
Boolean OptionKeyDown (void);
void DelayTicks (SInt32 howLong);
void UnivGetSoundVolume (SInt16 *volume);
void UnivSetSoundVolume (SInt16 volume);
#ifdef GP_USE_WINAPI_H
BOOL GetDataFolderPath (LPWSTR lpDataPath, DWORD cchDataPath);
#endif

#endif
