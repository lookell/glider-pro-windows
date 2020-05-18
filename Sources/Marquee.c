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


HPEN CreateMarqueePen(SInt16);
void DrawGliderMarquee (HDC);
void DrawMarquee (HDC);


marquee		theMarquee;
Rect		marqueeGliderRect;
Boolean		gliderMarqueeUp;


extern	HCURSOR		handCursor, vertCursor, horiCursor, diagCursor;
extern	Rect		leftStartGliderSrc;


//==============================================================  Functions
//--------------------------------------------------------------  CreateMarqueePen

HPEN CreateMarqueePen(SInt16 index)
{
	LOGBRUSH logBrush;

	GetObject(theMarquee.pats[index], sizeof(logBrush), &logBrush);
	return ExtCreatePen(PS_GEOMETRIC | PS_SOLID, 1, &logBrush, 0, NULL);
}

//--------------------------------------------------------------  DoMarquee

void DoMarquee (void)
{
	HPEN marqueePen, wasPen;
	HDC hdc;

	if ((!theMarquee.active) || (theMarquee.paused))
		return;

	hdc = GetMainWindowDC();
	SaveDC(hdc);
	SetROP2(hdc, R2_XORPEN);

	SelectObject(hdc, theMarquee.pats[theMarquee.index]);
	marqueePen = CreateMarqueePen(theMarquee.index);
	wasPen = SelectObject(hdc, marqueePen);
	DrawMarquee(hdc);
	SelectObject(hdc, wasPen);
	DeleteObject(marqueePen);

	theMarquee.index++;
	if (theMarquee.index >= kNumMarqueePats)
		theMarquee.index = 0;

	SelectObject(hdc, theMarquee.pats[theMarquee.index]);
	marqueePen = CreateMarqueePen(theMarquee.index);
	wasPen = SelectObject(hdc, marqueePen);
	DrawMarquee(hdc);
	SelectObject(hdc, wasPen);
	DeleteObject(marqueePen);

	RestoreDC(hdc, -1);
	ReleaseMainWindowDC(hdc);
}

//--------------------------------------------------------------  StartMarquee

void StartMarquee (Rect *theRect)
{
	HPEN marqueePen;
	HDC hdc;

	if (theMarquee.active)
		StopMarquee();

	if (objActive == kNoObjectSelected)
		return;

	theMarquee.bounds = *theRect;
	theMarquee.active = true;
	theMarquee.paused = false;
	theMarquee.handled = false;

	marqueePen = CreateMarqueePen(theMarquee.index);
	hdc = GetMainWindowDC();
	SaveDC(hdc);
	SetROP2(hdc, R2_XORPEN);
	SelectObject(hdc, theMarquee.pats[theMarquee.index]);
	SelectObject(hdc, marqueePen);
	DrawMarquee(hdc);
	RestoreDC(hdc, -1);
	ReleaseMainWindowDC(hdc);
	DeleteObject(marqueePen);

	SetCoordinateHVD(theMarquee.bounds.left, theMarquee.bounds.top, -1);
}

//--------------------------------------------------------------  StartMarqueeHandled

void StartMarqueeHandled (Rect *theRect, SInt16 direction, SInt16 dist)
{
	HPEN marqueePen;
	HDC hdc;

	if (theMarquee.active)
		StopMarquee();

	if (objActive == kNoObjectSelected)
		return;

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

	marqueePen = CreateMarqueePen(theMarquee.index);
	hdc = GetMainWindowDC();
	SaveDC(hdc);
	SetROP2(hdc, R2_XORPEN);
	SelectObject(hdc, theMarquee.pats[theMarquee.index]);
	SelectObject(hdc, marqueePen);
	DrawMarquee(hdc);
	RestoreDC(hdc, -1);
	ReleaseMainWindowDC(hdc);
	DeleteObject(marqueePen);

	SetCoordinateHVD(theMarquee.bounds.left, theMarquee.bounds.top, dist);
}

//--------------------------------------------------------------  StopMarquee

void StopMarquee (void)
{
	HPEN marqueePen;
	HDC hdc;

	if (gliderMarqueeUp)
	{
		hdc = GetMainWindowDC();
		DrawGliderMarquee(hdc);
		ReleaseMainWindowDC(hdc);
		gliderMarqueeUp = false;
	}

	if (!theMarquee.active)
		return;

	marqueePen = CreateMarqueePen(theMarquee.index);
	hdc = GetMainWindowDC();
	SaveDC(hdc);
	SetROP2(hdc, R2_XORPEN);
	SelectObject(hdc, theMarquee.pats[theMarquee.index]);
	SelectObject(hdc, marqueePen);
	DrawMarquee(hdc);
	RestoreDC(hdc, -1);
	ReleaseMainWindowDC(hdc);
	DeleteObject(marqueePen);

	theMarquee.active = false;
	SetCoordinateHVD(-1, -1, -1);
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
#if 0
	Point		wasPt, newPt;
	SInt16		deltaH, deltaV;

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
#if 0
	Point		wasPt, newPt;
	SInt16		deltaH, deltaV;

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
#if 0
	Point		wasPt, newPt;
	SInt16		deltaH, deltaV;

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

void DrawGliderMarquee (HDC hdc)
{
	Mac_CopyBits(blowerMaskMap, hdc,
			&leftStartGliderSrc, &marqueeGliderRect,
			srcXor, nil);
}

//--------------------------------------------------------------  SetMarqueeGliderCenter

void SetMarqueeGliderRect (SInt16 h, SInt16 v)
{
	HDC mainWindowDC;

	marqueeGliderRect = leftStartGliderSrc;
	ZeroRectCorner(&marqueeGliderRect);
	QOffsetRect(&marqueeGliderRect, h - kHalfGliderWide, v - kGliderHigh);

	mainWindowDC = GetMainWindowDC();
	DrawGliderMarquee(mainWindowDC);
	ReleaseMainWindowDC(mainWindowDC);
	gliderMarqueeUp = true;
}

//--------------------------------------------------------------  DrawMarquee

void DrawMarquee (HDC hdc)
{
	Mac_FrameRect(hdc, &theMarquee.bounds, GetCurrentObject(hdc, OBJ_BRUSH), 1, 1); 
	if (theMarquee.handled)
	{
		Mac_PaintRect(hdc, &theMarquee.handle, GetCurrentObject(hdc, OBJ_BRUSH));
		switch (theMarquee.direction)
		{
			case kAbove:
			MoveToEx(hdc, theMarquee.handle.left + (kHandleSideLong / 2),
					theMarquee.handle.bottom, NULL);
			Mac_LineTo(hdc, theMarquee.handle.left + (kHandleSideLong / 2),
					theMarquee.bounds.top - 1);
			break;

			case kToRight:
			MoveToEx(hdc, theMarquee.handle.left,
					theMarquee.handle.top + (kHandleSideLong / 2), NULL);
			Mac_LineTo(hdc, theMarquee.bounds.right,
					theMarquee.handle.top + (kHandleSideLong / 2));
			break;

			case kBelow:
			MoveToEx(hdc, theMarquee.handle.left + (kHandleSideLong / 2),
					theMarquee.handle.top - 1, NULL);
			Mac_LineTo(hdc, theMarquee.handle.left + (kHandleSideLong / 2),
					theMarquee.bounds.bottom);
			break;

			case kToLeft:
			MoveToEx(hdc, theMarquee.handle.right,
					theMarquee.handle.top + (kHandleSideLong / 2), NULL);
			Mac_LineTo(hdc, theMarquee.bounds.left,
					theMarquee.handle.top + (kHandleSideLong / 2));
			break;
		}
	}

	if (gliderMarqueeUp)
		DrawGliderMarquee(hdc);
}

//--------------------------------------------------------------  InitMarquee

void InitMarquee(void)
{
	HIMAGELIST	himlMarquee;
	HDC			hdc;
	HBITMAP		hBitmap, hbmPrev;
	INT			i, numMarqueePats;

	himlMarquee = ImageList_LoadImage(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(kMarqueePatListID), 8, 0,
			CLR_NONE, IMAGE_BITMAP, LR_MONOCHROME);
	if (himlMarquee == NULL)
		RedAlert(kErrFailedResourceLoad);

	numMarqueePats = ImageList_GetImageCount(himlMarquee);
	if (numMarqueePats != kNumMarqueePats)
		RedAlert(kErrUnnaccounted);

	hdc = CreateCompatibleDC(NULL);
	for (i = 0; i < kNumMarqueePats; i++)
	{
		hBitmap = CreateBitmap(8, 8, 1, 1, NULL);
		hbmPrev = SelectObject(hdc, hBitmap);
		ImageList_Draw(himlMarquee, i, hdc, 0, 0, ILD_IMAGE);
		SelectObject(hdc, hbmPrev);
		theMarquee.pats[i] = CreatePatternBrush(hBitmap);
	}
	DeleteDC(hdc);
	ImageList_Destroy(himlMarquee);

	theMarquee.index = 0;
	theMarquee.active = false;
	theMarquee.paused = false;
	theMarquee.handled = false;
	gliderMarqueeUp = false;
}

