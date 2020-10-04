#include "Environ.h"

//============================================================================
//----------------------------------------------------------------------------
//                                 Environ.c
//----------------------------------------------------------------------------
//============================================================================


#include "Main.h"


Boolean DoWeHaveQuickTime (void);
SInt16 WhatsOurDepth (void);
void GetDeviceRect (Rect *theRect);


macEnviron thisMac;


//==============================================================  Functions
//--------------------------------------------------------------  DoWeHaveQuickTime

Boolean DoWeHaveQuickTime (void)
{
	// QuickTime movies are not currently supported in this port.
	return false;
}

//--------------------------------------------------------------  WhatsOurDepth
// Determines the pixel bit depth for current device (monitor).

SInt16 WhatsOurDepth (void)
{
	// Only 8-bit color mode is supported, for things that require an
	// underlying system palette.
	return 8;
}

//--------------------------------------------------------------  GetDeviceRect
// Returns the bounding rectangle for the current device (monitor).

void GetDeviceRect (Rect *theRect)
{
	theRect->left = 0;
	theRect->top = 0;
	theRect->right = (SInt16)isViewportWidth;
	theRect->bottom = (SInt16)isViewportHeight;
}

//--------------------------------------------------------------  CheckOurEnvirons
// Calls all the above functions in order to fill out a sort of "spec sheet"…
// for the current Mac.

void CheckOurEnvirons (void)
{
	GetDeviceRect(&thisMac.screen);
	thisMac.isDepth = WhatsOurDepth();
	thisMac.hasQT = DoWeHaveQuickTime();
}

