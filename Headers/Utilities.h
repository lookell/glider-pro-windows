#ifndef UTILITIES_H_
#define UTILITIES_H_

//============================================================================
//----------------------------------------------------------------------------
//                                Utilities.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

/*
typedef	SInt16		SICN[16];
typedef	SICN		*SICNList;
typedef	SICNList	*SICNHand;
*/

extern GDHandle thisGDevice;

Point MyGetGlobalMouse (void);
void ToolBoxInit (void);
SInt16 RandomInt (SInt16 range);
SInt32 RandomLong (SInt32 range);
void InitRandomLongQUS (void);
UInt32 RandomLongQUS (void);
void RedAlert (SInt16 errorNumber);
void FindOurDevice (void);
//void CreateOffScreenBitMap (Rect *theRect, GrafPtr *offScreen);
//void CreateOffScreenPixMap (Rect *theRect, CGrafPtr *offScreen);
OSErr CreateOffScreenGWorld (HDC *theGWorld, Rect *bounds, SInt16 depth);
//void KillOffScreenPixMap (CGrafPtr offScreen);
//void KillOffScreenBitMap (GrafPtr offScreen);
void DisposeGWorld (HDC theGWorld);
HBITMAP GetPicture (SInt16 resID);
void LoadGraphic (HDC hdc, SInt16 resID);
void LoadScaledGraphic (HDC hdc, SInt16 resID, Rect *theRect);
//void PlotSICN (Rect *theRect, SICNHand theSICN, SInt32 theIndex);
void LargeIconPlot (Rect *theRect, SInt16 theID);
void DrawCIcon (HDC hdc, SInt16 theID, SInt16 h, SInt16 v);
SInt32 LongSquareRoot (SInt32 theNumber);
Boolean WaitForInputEvent (UInt16 seconds);
void WaitCommandQReleased (void);
char KeyMapOffsetFromRawKey (char rawKeyCode);
char GetKeyMapFromMessage (SInt32 message);
void GetKeyName (SInt32 message, StringPtr theName);
Boolean OptionKeyDown (void);
//SInt32 ExtractCTSeed (CGrafPtr porter);
//void ForceCTSeed (CGrafPtr porter, SInt32 newSeed);
void DelayTicks (SInt32 howLong);
void UnivGetSoundVolume (SInt16 *volume, Boolean hasSM3);
void UnivSetSoundVolume (SInt16 volume, Boolean hasSM3);
BOOL GetDataFolderPath (LPWSTR lpDataPath, DWORD cchDataPath);

#endif
