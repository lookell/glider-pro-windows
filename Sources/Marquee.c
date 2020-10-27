#define GP_USE_WINAPI_H

#include "Marquee.h"

//============================================================================
//----------------------------------------------------------------------------
//                                 Marquee.c
//----------------------------------------------------------------------------
//============================================================================


#include "AnimCursor.h"
#include "Coordinates.h"
#include "GliderDefines.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "Objects.h"
#include "ObjectEdit.h"
#include "RectUtils.h"
#include "ResourceIDs.h"
#include "Utilities.h"

#include <commctrl.h>


#define kHandleSideLong			9


HBRUSH CreateMarqueeBrush (void);
HPEN CreateMarqueePen (void);
void PaintMarqueeRect (HDC hdc, const Rect *theRect);
void FrameMarqueeRect (HDC hdc, const Rect *theRect);
void DrawGliderMarquee (HDC hdc);
void DrawMarquee (HDC hdc);


marquee theMarquee;

static Rect marqueeGliderRect;
static Boolean gliderMarqueeUp;


//==============================================================  Functions
//--------------------------------------------------------------  CreateMarqueeBrush

HBRUSH CreateMarqueeBrush (void)
{
	return CreatePatternBrush(theMarquee.pats[theMarquee.index]);
}

//--------------------------------------------------------------  CreateMarqueePen

HPEN CreateMarqueePen (void)
{
	LOGBRUSH logBrush;

	logBrush.lbStyle = BS_PATTERN;
	logBrush.lbColor = 0;
	logBrush.lbHatch = (ULONG_PTR)theMarquee.pats[theMarquee.index];
	return ExtCreatePen(PS_GEOMETRIC | PS_SOLID, 1, &logBrush, 0, NULL);
}

//--------------------------------------------------------------  PaintMarqueeRect

void PaintMarqueeRect (HDC hdc, const Rect *theRect)
{
	HBRUSH marqueeBrush;
	int wasROP2;

	marqueeBrush = CreateMarqueeBrush();
	wasROP2 = SetROP2(hdc, R2_XORPEN);
	Mac_PaintRect(hdc, theRect, marqueeBrush);
	SetROP2(hdc, wasROP2);
	DeleteObject(marqueeBrush);
}

//--------------------------------------------------------------  FrameMarqueeRect

void FrameMarqueeRect (HDC hdc, const Rect *theRect)
{
	HBRUSH marqueeBrush;
	int wasROP2;

	marqueeBrush = CreateMarqueeBrush();
	wasROP2 = SetROP2(hdc, R2_XORPEN);
	Mac_FrameRect(hdc, theRect, marqueeBrush, 1, 1);
	SetROP2(hdc, wasROP2);
	DeleteObject(marqueeBrush);
}

//--------------------------------------------------------------  DoMarquee

void DoMarquee (void)
{
	HDC hdc;

	if ((!theMarquee.active) || (theMarquee.paused))
		return;

	hdc = GetMainWindowDC();

	DrawMarquee(hdc);

	theMarquee.index++;
	if (theMarquee.index >= kNumMarqueePats)
		theMarquee.index = 0;

	DrawMarquee(hdc);
	
	ReleaseMainWindowDC(hdc);
}

//--------------------------------------------------------------  StartMarquee

void StartMarquee (const Rect *theRect)
{
	HDC hdc;

	if (theMarquee.active)
		StopMarquee();

	if (objActive == kNoObjectSelected)
		return;

	theMarquee.bounds = *theRect;
	theMarquee.active = true;
	theMarquee.paused = false;
	theMarquee.handled = false;

	hdc = GetMainWindowDC();
	DrawMarquee(hdc);
	ReleaseMainWindowDC(hdc);

	SetCoordinateHVD(theMarquee.bounds.left, theMarquee.bounds.top, -1);
}

//--------------------------------------------------------------  StartMarqueeHandled

void StartMarqueeHandled (const Rect *theRect, SInt16 direction, SInt16 dist)
{
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

	hdc = GetMainWindowDC();
	DrawMarquee(hdc);
	ReleaseMainWindowDC(hdc);

	SetCoordinateHVD(theMarquee.bounds.left, theMarquee.bounds.top, dist);
}

//--------------------------------------------------------------  StopMarquee

void StopMarquee (void)
{
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

	hdc = GetMainWindowDC();
	DrawMarquee(hdc);
	ReleaseMainWindowDC(hdc);

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
	Point wasPt, newPt;
	HDC hdc;
	MSG msg;

	SetCapture(mainWindow);
	InitCursor();
	QSetRect(theRect, start.h, start.v, start.h, start.v);
	hdc = GetMainWindowDC();
	FrameMarqueeRect(hdc, theRect);
	ReleaseMainWindowDC(hdc);
	wasPt = start;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			newPt.h = GET_X_LPARAM(msg.lParam);
			newPt.v = GET_Y_LPARAM(msg.lParam);
			hdc = GetMainWindowDC();
			FrameMarqueeRect(hdc, theRect);
			QSetRect(theRect, start.h, start.v, newPt.h, newPt.v);
			NormalizeRect(theRect);
			FrameMarqueeRect(hdc, theRect);
			ReleaseMainWindowDC(hdc);
			wasPt = newPt;
			break;

		case WM_LBUTTONUP:
			if (GetCapture() == mainWindow)
				ReleaseCapture();
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			// ignore these events so that the current room and object don't change
			break;

		default:
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			break;
		}
		if (GetCapture() != mainWindow)
			break;
	}
	if (msg.message == WM_QUIT)
	{
		PostQuitMessage((int)msg.wParam);
		if (GetCapture() == mainWindow)
			ReleaseCapture();
	}
	hdc = GetMainWindowDC();
	FrameMarqueeRect(hdc, theRect);
	ReleaseMainWindowDC(hdc);
}

//--------------------------------------------------------------  DragMarqueeRect

void DragMarqueeRect (Point start, Rect *theRect, Boolean lockH, Boolean lockV)
{
	Point wasPt, newPt;
	SInt16 deltaH, deltaV;
	HDC hdc;
	MSG msg;

	SetCapture(mainWindow);
	SetCursor(handCursor);
	StopMarquee();
	hdc = GetMainWindowDC();
	theMarquee.bounds = *theRect;
	FrameMarqueeRect(hdc, &theMarquee.bounds);
	ReleaseMainWindowDC(hdc);

	wasPt = start;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			newPt.h = GET_X_LPARAM(msg.lParam);
			newPt.v = GET_Y_LPARAM(msg.lParam);
			if (lockV)
				deltaH = 0;
			else
				deltaH = newPt.h - wasPt.h;
			if (lockH)
				deltaV = 0;
			else
				deltaV = newPt.v - wasPt.v;
			hdc = GetMainWindowDC();
			FrameMarqueeRect(hdc, &theMarquee.bounds);
			QOffsetRect(&theMarquee.bounds, deltaH, deltaV);
			FrameMarqueeRect(hdc, &theMarquee.bounds);
			ReleaseMainWindowDC(hdc);
			wasPt = newPt;
			SetCoordinateHVD(theMarquee.bounds.left, theMarquee.bounds.top, -2);
			break;

		case WM_LBUTTONUP:
			if (GetCapture() == mainWindow)
				ReleaseCapture();
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			// ignore these events so that the current room and object don't change
			break;

		default:
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			break;
		}
		if (GetCapture() != mainWindow)
			break;
	}
	if (msg.message == WM_QUIT)
	{
		PostQuitMessage((int)msg.wParam);
		if (GetCapture() == mainWindow)
			ReleaseCapture();
	}

	hdc = GetMainWindowDC();
	FrameMarqueeRect(hdc, &theMarquee.bounds);
	ReleaseMainWindowDC(hdc);
	*theRect = theMarquee.bounds;
	InitCursor();
}

//--------------------------------------------------------------  DragMarqueeHandle

void DragMarqueeHandle (Point start, SInt16 *dragged)
{
	Point wasPt, newPt;
	SInt16 deltaH, deltaV;
	HDC hdc;
	MSG msg;

	SetCapture(mainWindow);
	if ((theMarquee.direction == kAbove) || (theMarquee.direction == kBelow))
		SetCursor(vertCursor);
	else
		SetCursor(horiCursor);
	StopMarquee();
	hdc = GetMainWindowDC();
	FrameMarqueeRect(hdc, &theMarquee.bounds);
	PaintMarqueeRect(hdc, &theMarquee.handle);
	ReleaseMainWindowDC(hdc);

	wasPt = start;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			newPt.h = GET_X_LPARAM(msg.lParam);
			newPt.v = GET_Y_LPARAM(msg.lParam);
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

			default:
				deltaH = 0;
				deltaV = 0;
				break;
			}

			hdc = GetMainWindowDC();
			PaintMarqueeRect(hdc, &theMarquee.handle);
			QOffsetRect(&theMarquee.handle, deltaH, deltaV);
			PaintMarqueeRect(hdc, &theMarquee.handle);
			ReleaseMainWindowDC(hdc);
			wasPt = newPt;
			break;

		case WM_LBUTTONUP:
			if (GetCapture() == mainWindow)
				ReleaseCapture();
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			// ignore these events so that the current room and object don't change
			break;

		default:
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			break;
		}
		if (GetCapture() != mainWindow)
			break;
	}
	if (msg.message == WM_QUIT)
	{
		PostQuitMessage((int)msg.wParam);
		if (GetCapture() == mainWindow)
			ReleaseCapture();
	}

	hdc = GetMainWindowDC();
	FrameMarqueeRect(hdc, &theMarquee.bounds);
	PaintMarqueeRect(hdc, &theMarquee.handle);
	ReleaseMainWindowDC(hdc);
	InitCursor();
}

//--------------------------------------------------------------  DragMarqueeCorner

void DragMarqueeCorner (Point start, SInt16 *hDragged, SInt16 *vDragged, Boolean isTop)
{
	Point wasPt, newPt;
	SInt16 deltaH, deltaV;
	HDC hdc;
	MSG msg;

	SetCapture(mainWindow);
	if (isTop)
		SetCursor(diagTopCursor);
	else
		SetCursor(diagBotCursor);
	StopMarquee();
	hdc = GetMainWindowDC();
	FrameMarqueeRect(hdc, &theMarquee.bounds);
	PaintMarqueeRect(hdc, &theMarquee.handle);
	ReleaseMainWindowDC(hdc);

	wasPt = start;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			newPt.h = GET_X_LPARAM(msg.lParam);
			newPt.v = GET_Y_LPARAM(msg.lParam);
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
			hdc = GetMainWindowDC();
			FrameMarqueeRect(hdc, &theMarquee.bounds);
			PaintMarqueeRect(hdc, &theMarquee.handle);
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
			FrameMarqueeRect(hdc, &theMarquee.bounds);
			PaintMarqueeRect(hdc, &theMarquee.handle);
			ReleaseMainWindowDC(hdc);
			wasPt = newPt;
			break;

		case WM_LBUTTONUP:
			if (GetCapture() == mainWindow)
				ReleaseCapture();
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			// ignore these events so that the current room and object don't change
			break;

		default:
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			break;
		}
		if (GetCapture() != mainWindow)
			break;
	}
	if (msg.message == WM_QUIT)
	{
		PostQuitMessage((int)msg.wParam);
		if (GetCapture() == mainWindow)
			ReleaseCapture();
	}

	hdc = GetMainWindowDC();
	FrameMarqueeRect(hdc, &theMarquee.bounds);
	PaintMarqueeRect(hdc, &theMarquee.handle);
	ReleaseMainWindowDC(hdc);
	InitCursor();
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
	return theMarquee.handled && QPtInRect(where, &theMarquee.handle);
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
	HDC hdc;

	marqueeGliderRect = leftStartGliderSrc;
	ZeroRectCorner(&marqueeGliderRect);
	QOffsetRect(&marqueeGliderRect, h - kHalfGliderWide, v - kGliderHigh);

	hdc = GetMainWindowDC();
	DrawGliderMarquee(hdc);
	ReleaseMainWindowDC(hdc);
	gliderMarqueeUp = true;
}

//--------------------------------------------------------------  DrawMarquee

void DrawMarquee (HDC hdc)
{
	HBRUSH marqueeBrush;
	HPEN marqueePen, wasPen;
	int wasROP2;

	marqueeBrush = CreateMarqueeBrush();
	wasROP2 = SetROP2(hdc, R2_XORPEN);

	Mac_FrameRect(hdc, &theMarquee.bounds, marqueeBrush, 1, 1); 
	if (theMarquee.handled)
	{
		Mac_PaintRect(hdc, &theMarquee.handle, marqueeBrush);
		marqueePen = CreateMarqueePen();
		wasPen = (HPEN)SelectObject(hdc, marqueePen);
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
		SelectObject(hdc, wasPen);
		DeleteObject(marqueePen);
	}

	SetROP2(hdc, wasROP2);
	DeleteObject(marqueeBrush);

	if (gliderMarqueeUp)
		DrawGliderMarquee(hdc);
}

//--------------------------------------------------------------  InitMarquee

void InitMarquee (void)
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
		hbmPrev = (HBITMAP)SelectObject(hdc, hBitmap);
		ImageList_Draw(himlMarquee, i, hdc, 0, 0, ILD_IMAGE);
		SelectObject(hdc, hbmPrev);
		theMarquee.pats[i] = hBitmap;
	}
	DeleteDC(hdc);
	ImageList_Destroy(himlMarquee);

	theMarquee.index = 0;
	theMarquee.active = false;
	theMarquee.paused = false;
	theMarquee.handled = false;
	gliderMarqueeUp = false;
}

