//============================================================================
//----------------------------------------------------------------------------
//                                 Environ.c
//----------------------------------------------------------------------------
//============================================================================

#include "Environ.h"

#include "Main.h"

Boolean DoWeHaveQuickTime (void);
SInt16 WhatsOurDepth (void);
void GetDeviceRect (Rect *theRect);

macEnviron g_thisMac;

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
	theRect->right = (SInt16)g_isViewportWidth;
	theRect->bottom = (SInt16)g_isViewportHeight;
}

//--------------------------------------------------------------  CheckOurEnvirons
// Calls all the above functions in order to fill out a sort of "spec sheet"
// for the current Mac.

void CheckOurEnvirons (void)
{
	GetDeviceRect(&g_thisMac.screen);
	g_thisMac.isDepth = WhatsOurDepth();
	g_thisMac.hasQT = DoWeHaveQuickTime();
}
