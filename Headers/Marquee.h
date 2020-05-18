//============================================================================
//----------------------------------------------------------------------------
//									Marquee.h
//----------------------------------------------------------------------------
//============================================================================


#pragma once


//#include <Quickdraw.h>
#include "Macintosh.h"
#include "WinAPI.h"


typedef struct
{
	HBRUSH		pats[kNumMarqueePats];
	Rect		bounds, handle;
	SInt16		index, direction, dist;
	Boolean		active, paused, handled;
} marquee;


extern marquee		theMarquee;

