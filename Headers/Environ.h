#ifndef ENVIRON_H_
#define ENVIRON_H_

//============================================================================
//----------------------------------------------------------------------------
//                                 Environ.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

typedef struct
{
	Rect		screen, gray;
	SInt32		dirID;
	SInt16		wasDepth, isDepth;
	SInt16		thisResFile;
	SInt16		numScreens;
	SInt16		vRefNum;
	Boolean		can1Bit;
	Boolean		can4Bit;
	Boolean		can8Bit;
	Boolean		wasColorOrGray;
	Boolean		hasWNE;
	Boolean		hasSystem7;
	Boolean		hasColor;
	Boolean		hasGestalt;
	Boolean		canSwitch;
	Boolean		canColor;
	Boolean		hasSM3;
	Boolean		hasQT;
	Boolean		hasDrag;
} macEnviron;

extern macEnviron	thisMac;

SInt16 WhatsOurDepth (void);
void SwitchToDepth (SInt16 newDepth, Boolean doColor);
void CheckOurEnvirons (void);
//void ReflectSecondMonitorEnvirons (Boolean use1Bit, Boolean use4Bit, Boolean use8Bit);
void HandleDepthSwitching (HWND ownerWindow);
void RestoreColorDepth (void);
void CheckMemorySize (HWND ownerWindow);
void SetAppMemorySize (SInt32 newSize);

#endif
