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
	short		index, direction, dist;
	Boolean		active, paused, handled;
} marquee;


extern marquee		theMarquee;

