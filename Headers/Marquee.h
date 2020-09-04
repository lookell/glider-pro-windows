#ifndef MARQUEE_H_
#define MARQUEE_H_

//============================================================================
//----------------------------------------------------------------------------
//                                 Marquee.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderDefines.h"
#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

#ifdef GP_USE_WINAPI_H
typedef struct marquee
{
	HBITMAP pats[kNumMarqueePats];
	Rect bounds;
	Rect handle;
	SInt16 index;
	SInt16 direction;
	SInt16 dist;
	Boolean active;
	Boolean paused;
	Boolean handled;
} marquee;
#endif

#ifdef GP_USE_WINAPI_H
extern marquee theMarquee;
#endif

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
