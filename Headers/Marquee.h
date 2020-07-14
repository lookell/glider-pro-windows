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

extern marquee theMarquee;

void DoMarquee (void);
void StartMarquee (const Rect *theRect);
void StartMarqueeHandled (const Rect *theRect, SInt16 direction, SInt16 dist);
void StopMarquee (void);
void PauseMarquee (void);
void ResumeMarquee (void);
void DragOutMarqueeRect (Point start, Rect *theRect);
void DragMarqueeRect (Point start, Rect *theRect, Boolean lockH, Boolean lockV);
void DragMarqueeHandle (Point start, SInt16 *dragged);
void DragMarqueeCorner (Point start, SInt16 *hDragged, SInt16 *vDragged, Boolean isTop);
Boolean MarqueeHasHandles (SInt16 *direction, SInt16 *dist);
Boolean PtInMarqueeHandle (Point where);
void SetMarqueeGliderRect (SInt16 h, SInt16 v);
void InitMarquee (void);

#endif
