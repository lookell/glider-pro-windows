#ifndef ENVIRON_H_
#define ENVIRON_H_

//============================================================================
//----------------------------------------------------------------------------
//                                 Environ.h
//----------------------------------------------------------------------------
//============================================================================


#include "MacTypes.h"


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


#endif
