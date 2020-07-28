#ifndef UTILITIES_H_
#define UTILITIES_H_

//============================================================================
//----------------------------------------------------------------------------
//                                Utilities.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

void ToolBoxInit (void);
SInt16 RandomInt (SInt16 range);
SInt32 RandomLong (SInt32 range);
void InitRandomLongQUS (void);
UInt32 RandomLongQUS (void);
__declspec(noreturn) void RedAlert (SInt16 errorNumber);
OSErr CreateOffScreenGWorld (HDC *theGWorld, const Rect *bounds, SInt16 depth);
void DisposeGWorld (HDC theGWorld);
HBITMAP GetPicture (SInt16 resID);
void LoadGraphic (HDC hdc, SInt16 resID);
void LoadScaledGraphic (HDC hdc, SInt16 resID, const Rect *theRect);
void DrawCIcon (HDC hdc, SInt16 theID, SInt16 h, SInt16 v);
SInt32 LongSquareRoot (SInt32 theNumber);
Boolean WaitForInputEvent (UInt16 seconds);
void WaitCommandQReleased (void);
Boolean OptionKeyDown (void);
void DelayTicks (SInt32 howLong);
void UnivGetSoundVolume (SInt16 *volume);
void UnivSetSoundVolume (SInt16 volume);
BOOL GetDataFolderPath (LPWSTR lpDataPath, DWORD cchDataPath);
HRESULT LoadModuleResource (HMODULE hModule, LPCWSTR pResName, LPCWSTR pResType,
	LPVOID *ppResData, DWORD *pResByteSize);

#endif
