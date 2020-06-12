#ifndef MARQUEE_H_
#define MARQUEE_H_

//============================================================================
//----------------------------------------------------------------------------
//                                 Marquee.h
//----------------------------------------------------------------------------
//============================================================================


#pragma once


#include "GliderDefines.h"
#include "MacTypes.h"
#include "WinAPI.h"


typedef struct
{
	HBITMAP		pats[kNumMarqueePats];
	Rect		bounds, handle;
	SInt16		index, direction, dist;
	Boolean		active, paused, handled;
} marquee;


extern marquee		theMarquee;

#endif
