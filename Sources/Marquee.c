//============================================================================
//----------------------------------------------------------------------------
//									Marquee.c
//----------------------------------------------------------------------------
//============================================================================


#include "Externs.h"
#include "Marquee.h"
#include "Objects.h"
#include "ObjectEdit.h"
#include "RectUtils.h"


#define	kMarqueePatListID		128
#define kHandleSideLong			9


void DrawGliderMarquee (void);
void DrawMarquee (void);


marquee		theMarquee;
Rect		marqueeGliderRect;
Boolean		gliderMarqueeUp;


extern	HCURSOR		handCursor, vertCursor, horiCursor, diagCursor;
extern	Rect		leftStartGliderSrc;


//==============================================================  Functions
//--------------------------------------------------------------  DoMarquee

void DoMarquee (void)
{
	return;
#if 0
	if ((!theMarquee.active) || (theMarquee.paused))
		return;

	SetPortWindowPort(mainWindow);
	PenMode(patXor);
	PenPat(&theMarquee.pats[theMarquee.index]);
	DrawMarquee();
	theMarquee.index++;
	if (theMarquee.index >= kNumMarqueePats)
		theMarquee.index = 0;
	PenPat(&theMarquee.pats[theMarquee.index]);
	DrawMarquee();
	PenNormal();
#endif
}

//--------------------------------------------------------------  StartMarquee

void StartMarquee (Rect *theRect)
{
	return;
#if 0
	if (theMarquee.active)
		StopMarquee();

	if (objActive == kNoObjectSelected)
		return;

	SetPortWindowPort(mainWindow);
	theMarquee.bounds = *theRect;
	theMarquee.active = true;
	theMarquee.paused = false;
	theMarquee.handled = false;
	PenMode(patXor);
	PenPat(&theMarquee.pats[theMarquee.index]);
	DrawMarquee();
	PenNormal();
	SetCoordinateHVD(theMarquee.bounds.left, theMarquee.bounds.top, -1);
#endif
}

//--------------------------------------------------------------  StartMarqueeHandled

void StartMarqueeHandled (Rect *theRect, SInt16 direction, SInt16 dist)
{
	return;
#if 0
	if (theMarquee.active)
		StopMarquee();

	if (objActive == kNoObjectSelected)
		return;

	SetPortWindowPort(mainWindow);
	theMarquee.bounds = *theRect;
	theMarquee.active = true;
	theMarquee.paused = false;
	theMarquee.handled = true;
	QSetRect(&theMarquee.handle, 0, 0, kHandleSideLong, kHandleSideLong);
	QOffsetRect(&theMarquee.handle, kHandleSideLong / -2, kHandleSideLong / -2);
	switch (direction)
	{
		case kAbove:
		QOffsetRect(&theMarquee.handle, theMarquee.bounds.left,
				theMarquee.bounds.top);
		QOffsetRect(&theMarquee.handle, HalfRectWide(&theMarquee.bounds), -dist);
		break;

		case kToRight:
		QOffsetRect(&theMarquee.handle, theMarquee.bounds.right,
				theMarquee.bounds.top);
		QOffsetRect(&theMarquee.handle, dist, HalfRectTall(&theMarquee.bounds));
		break;

		case kBelow:
		QOffsetRect(&theMarquee.handle, theMarquee.bounds.left,
				theMarquee.bounds.bottom);
		QOffsetRect(&theMarquee.handle, HalfRectWide(&theMarquee.bounds), dist);
		break;

		case kToLeft:
		QOffsetRect(&theMarquee.handle, theMarquee.bounds.left,
				theMarquee.bounds.top);
		QOffsetRect(&theMarquee.handle, -dist, HalfRectTall(&theMarquee.bounds));
		break;

		case kBottomCorner:
		QOffsetRect(&theMarquee.handle, theMarquee.bounds.right,
				theMarquee.bounds.bottom);
		break;

		case kTopCorner:
		QOffsetRect(&theMarquee.handle, theMarquee.bounds.right,
				theMarquee.bounds.top);
		break;
	}
	theMarquee.direction = direction;
	theMarquee.dist = dist;

	PenMode(patXor);
	PenPat(&theMarquee.pats[theMarquee.index]);
	DrawMarquee();
	PenNormal();
	SetCoordinateHVD(theMarquee.bounds.left, theMarquee.bounds.top, dist);
#endif
}

//--------------------------------------------------------------  StopMarquee

void StopMarquee (void)
{
	return;
#if 0
	if (gliderMarqueeUp)
	{
		DrawGliderMarquee();
		gliderMarqueeUp = false;
	}

	if (!theMarquee.active)
		return;

	SetPortWindowPort(mainWindow);
	PenMode(patXor);
	PenPat(&theMarquee.pats[theMarquee.index]);
	DrawMarquee();
	PenNormal();
	theMarquee.active = false;
	SetCoordinateHVD(-1, -1, -1);
#endif
}

//--------------------------------------------------------------  PauseMarquee

void PauseMarquee (void)
{
	if (!theMarquee.active)
		return;

	theMarquee.paused = true;
	StopMarquee();
}

//--------------------------------------------------------------  ResumeMarquee

void ResumeMarquee (void)
{
	if (!theMarquee.paused)
		return;

	if (theMarquee.handled)
	{
		StartMarqueeHandled(&theMarquee.bounds, theMarquee.direction, theMarquee.dist);
		HandleBlowerGlider();
	}
	else
		StartMarquee(&theMarquee.bounds);
}

//--------------------------------------------------------------  DragOutMarqueeRect

void DragOutMarqueeRect (Point start, Rect *theRect)
{
	return;
#if 0
	Point		wasPt, newPt;

	SetPortWindowPort(mainWindow);
	InitCursor();
	QSetRect(theRect, start.h, start.v, start.h, start.v);
	PenMode(patXor);
	PenPat(&theMarquee.pats[theMarquee.index]);
	FrameRect(theRect);
	wasPt = start;

	while (WaitMouseUp())
	{
		GetMouse(&newPt);
		if (DeltaPoint(wasPt, newPt))
		{
			FrameRect(theRect);
			QSetRect(theRect, start.h, start.v, newPt.h, newPt.v);
			NormalizeRect(theRect);
			FrameRect(theRect);
			wasPt = newPt;
		}
	}
	FrameRect(theRect);
	PenNormal();
#endif
}

//--------------------------------------------------------------  DragMarqueeRect

void DragMarqueeRect (Point start, Rect *theRect, Boolean lockH, Boolean lockV)
{
	return;
#if 0
	Point		wasPt, newPt;
	short		deltaH, deltaV;

	SetCursor(&handCursor);
	StopMarquee();
	PenMode(patXor);
	PenPat(&theMarquee.pats[theMarquee.index]);
	theMarquee.bounds = *theRect;
	FrameRect(&theMarquee.bounds);

	wasPt = start;
	while (WaitMouseUp())
	{
		GetMouse(&newPt);
		if (DeltaPoint(wasPt, newPt))
		{
			if (lockV)
				deltaH = 0;
			else
				deltaH = newPt.h - wasPt.h;
			if (lockH)
				deltaV = 0;
			else
				deltaV = newPt.v - wasPt.v;
			FrameRect(&theMarquee.bounds);
			QOffsetRect(&theMarquee.bounds, deltaH, deltaV);
			FrameRect(&theMarquee.bounds);
			wasPt = newPt;
			SetCoordinateHVD(theMarquee.bounds.left, theMarquee.bounds.top, -2);
		}
	}
	FrameRect(&theMarquee.bounds);
	*theRect = theMarquee.bounds;
	PenNormal();
	InitCursor();
#endif
}

//--------------------------------------------------------------  DragMarqueeHandle

void DragMarqueeHandle (Point start, SInt16 *dragged)
{
	return;
#if 0
	Point		wasPt, newPt;
	short		deltaH, deltaV;

	if ((theMarquee.direction == kAbove) || (theMarquee.direction == kBelow))
		SetCursor(&vertCursor);
	else
		SetCursor(&horiCursor);
	StopMarquee();
	PenMode(patXor);
	PenPat(&theMarquee.pats[theMarquee.index]);
	FrameRect(&theMarquee.bounds);
	PaintRect(&theMarquee.handle);

	wasPt = start;
	while (WaitMouseUp())
	{
		GetMouse(&newPt);
		if (DeltaPoint(wasPt, newPt))
		{
			switch (theMarquee.direction)
			{
				case kAbove:
				deltaH = 0;
				deltaV = newPt.v - wasPt.v;
				*dragged -= deltaV;
				if (*dragged <= 0)
				{
					deltaV += *dragged;
					*dragged = 0;
				}
				DeltaCoordinateD(*dragged);
				break;

				case kToRight:
				deltaH = newPt.h - wasPt.h;
				deltaV = 0;
				*dragged += deltaH;
				if (*dragged <= 0)
				{
					deltaH -= *dragged;
					*dragged = 0;
				}
				DeltaCoordinateD(*dragged);
				break;

				case kBelow:
				deltaH = 0;
				deltaV = newPt.v - wasPt.v;
				*dragged += deltaV;
				if (*dragged <= 0)
				{
					deltaV -= *dragged;
					*dragged = 0;
				}
				DeltaCoordinateD(*dragged);
				break;

				case kToLeft:
				deltaH = newPt.h - wasPt.h;
				deltaV = 0;
				*dragged -= deltaH;
				if (*dragged <= 0)
				{
					deltaH += *dragged;
					*dragged = 0;
				}
				DeltaCoordinateD(*dragged);
				break;
			}

			PaintRect(&theMarquee.handle);
			QOffsetRect(&theMarquee.handle, deltaH, deltaV);
			PaintRect(&theMarquee.handle);
			wasPt = newPt;
		}
	}
	FrameRect(&theMarquee.bounds);
	PaintRect(&theMarquee.handle);
	PenNormal();
	InitCursor();
#endif
}

//--------------------------------------------------------------  DragMarqueeCorner

void DragMarqueeCorner (Point start, SInt16 *hDragged, SInt16 *vDragged, Boolean isTop)
{
	return;
#if 0
	Point		wasPt, newPt;
	short		deltaH, deltaV;

	SetCursor(&diagCursor);
	StopMarquee();
	PenMode(patXor);
	PenPat(&theMarquee.pats[theMarquee.index]);
	FrameRect(&theMarquee.bounds);
	PaintRect(&theMarquee.handle);

	wasPt = start;
	while (WaitMouseUp())
	{
		GetMouse(&newPt);
		if (DeltaPoint(wasPt, newPt))
		{
			deltaH = newPt.h - wasPt.h;
			if (isTop)
				deltaV = wasPt.v - newPt.v;
			else
				deltaV = newPt.v - wasPt.v;
			*hDragged += deltaH;
			if (*hDragged <= 0)
			{
				deltaH -= *hDragged;
				*hDragged = 0;
			}
			*vDragged += deltaV;
			if (*vDragged <= 0)
			{
				deltaV -= *vDragged;
				*vDragged = 0;
			}
			FrameRect(&theMarquee.bounds);
			PaintRect(&theMarquee.handle);
			if (isTop)
			{
				QOffsetRect(&theMarquee.handle, deltaH, -deltaV);
				theMarquee.bounds.right += deltaH;
				theMarquee.bounds.top -= deltaV;
			}
			else
			{
				QOffsetRect(&theMarquee.handle, deltaH, deltaV);
				theMarquee.bounds.right += deltaH;
				theMarquee.bounds.bottom += deltaV;
			}
			FrameRect(&theMarquee.bounds);
			PaintRect(&theMarquee.handle);
			wasPt = newPt;
		}
	}
	FrameRect(&theMarquee.bounds);
	PaintRect(&theMarquee.handle);
	PenNormal();
	InitCursor();
#endif
}

//--------------------------------------------------------------  MarqueeHasHandles

Boolean MarqueeHasHandles (SInt16 *direction, SInt16 *dist)
{
	if (theMarquee.handled)
	{
		*direction = theMarquee.direction;
		*dist = theMarquee.dist;
		return (true);
	}
	else
	{
		*direction = 0;
		*dist = 0;
		return (false);
	}
}

//--------------------------------------------------------------  PtInMarqueeHandle

Boolean PtInMarqueeHandle (Point where)
{
	return (Mac_PtInRect(where, &theMarquee.handle));
}

//--------------------------------------------------------------  DrawGliderMarquee

void DrawGliderMarquee (void)
{
	return;
#if 0
	CopyBits((BitMap *)*GetGWorldPixMap(blowerMaskMap),
			GetPortBitMapForCopyBits(GetWindowPort(mainWindow)),
			&leftStartGliderSrc,
			&marqueeGliderRect,
			srcXor, nil);
#endif
}

//--------------------------------------------------------------  SetMarqueeGliderCenter

void SetMarqueeGliderRect (SInt16 h, SInt16 v)
{
	marqueeGliderRect = leftStartGliderSrc;
	ZeroRectCorner(&marqueeGliderRect);
	QOffsetRect(&marqueeGliderRect, h - kHalfGliderWide, v - kGliderHigh);

	DrawGliderMarquee();
	gliderMarqueeUp = true;
}

//--------------------------------------------------------------  DrawMarquee

void DrawMarquee (void)
{
	return;
#if 0
	FrameRect(&theMarquee.bounds);
	if (theMarquee.handled)
	{
		PaintRect(&theMarquee.handle);
		switch (theMarquee.direction)
		{
			case kAbove:
			MoveTo(theMarquee.handle.left + (kHandleSideLong / 2),
					theMarquee.handle.bottom);
			LineTo(theMarquee.handle.left + (kHandleSideLong / 2),
					theMarquee.bounds.top - 1);
			break;

			case kToRight:
			MoveTo(theMarquee.handle.left,
					theMarquee.handle.top + (kHandleSideLong / 2));
			LineTo(theMarquee.bounds.right,
					theMarquee.handle.top + (kHandleSideLong / 2));
			break;

			case kBelow:
			MoveTo(theMarquee.handle.left + (kHandleSideLong / 2),
					theMarquee.handle.top - 1);
			LineTo(theMarquee.handle.left + (kHandleSideLong / 2),
					theMarquee.bounds.bottom);
			break;

			case kToLeft:
			MoveTo(theMarquee.handle.right,
					theMarquee.handle.top + (kHandleSideLong / 2));
			LineTo(theMarquee.bounds.left,
					theMarquee.handle.top + (kHandleSideLong / 2));
			break;
		}
	}

	if (gliderMarqueeUp)
		DrawGliderMarquee();
#endif
}

//--------------------------------------------------------------  InitMarquee

void InitMarquee (void)
{
	return;
#if 0
	short		i;

	for (i = 0; i < kNumMarqueePats; i++)
		GetIndPattern(&theMarquee.pats[i], kMarqueePatListID, i + 1);
	theMarquee.index = 0;
	theMarquee.active = false;
	theMarquee.paused = false;
	theMarquee.handled = false;
	gliderMarqueeUp = false;
#endif
}

