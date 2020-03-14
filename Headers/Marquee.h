//============================================================================
//----------------------------------------------------------------------------
//									Marquee.h
//----------------------------------------------------------------------------
//============================================================================


#pragma once


//#include <Quickdraw.h>
#include "Macintosh.h"


typedef struct
{
	Pattern		pats[kNumMarqueePats];
	Rect		bounds, handle;
	SInt16		index, direction, dist;
	Boolean		active, paused, handled;
} marquee;


extern marquee		theMarquee;

