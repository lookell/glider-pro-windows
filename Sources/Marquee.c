#include "Marquee.h"

//============================================================================
//----------------------------------------------------------------------------
//                                 Marquee.c
//----------------------------------------------------------------------------
//============================================================================

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

marquee g_theMarquee;

static Rect g_marqueeGliderRect;
static Boolean g_gliderMarqueeUp;

//==============================================================  Functions
//--------------------------------------------------------------  CreateMarqueeBrush

HBRUSH CreateMarqueeBrush (void)
{
	return CreatePatternBrush(g_theMarquee.pats[g_theMarquee.index]);
}

//--------------------------------------------------------------  CreateMarqueePen

HPEN CreateMarqueePen (void)
{
	LOGBRUSH logBrush;

	logBrush.lbStyle = BS_PATTERN;
	logBrush.lbColor = 0;
	logBrush.lbHatch = (ULONG_PTR)g_theMarquee.pats[g_theMarquee.index];
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

	if ((!g_theMarquee.active) || (g_theMarquee.paused))
		return;

	hdc = GetMainWindowDC();

	DrawMarquee(hdc);

	g_theMarquee.index++;
	if (g_theMarquee.index >= kNumMarqueePats)
		g_theMarquee.index = 0;

	DrawMarquee(hdc);
	
	ReleaseMainWindowDC(hdc);
}

//--------------------------------------------------------------  StartMarquee

void StartMarquee (const Rect *theRect)
{
	HDC hdc;

	if (g_theMarquee.active)
		StopMarquee();

	if (g_objActive == kNoObjectSelected)
		return;

	g_theMarquee.bounds = *theRect;
	g_theMarquee.active = true;
	g_theMarquee.paused = false;
	g_theMarquee.handled = false;

	hdc = GetMainWindowDC();
	DrawMarquee(hdc);
	ReleaseMainWindowDC(hdc);

	SetCoordinateHVD(g_theMarquee.bounds.left, g_theMarquee.bounds.top, -1);
}

//--------------------------------------------------------------  StartMarqueeHandled

void StartMarqueeHandled (const Rect *theRect, SInt16 direction, SInt16 dist)
{
	HDC hdc;

	if (g_theMarquee.active)
		StopMarquee();

	if (g_objActive == kNoObjectSelected)
		return;

	g_theMarquee.bounds = *theRect;
	g_theMarquee.active = true;
	g_theMarquee.paused = false;
	g_theMarquee.handled = true;
	QSetRect(&g_theMarquee.handle, 0, 0, kHandleSideLong, kHandleSideLong);
	QOffsetRect(&g_theMarquee.handle, kHandleSideLong / -2, kHandleSideLong / -2);
	switch (direction)
	{
		case kAbove:
		QOffsetRect(&g_theMarquee.handle, g_theMarquee.bounds.left,
				g_theMarquee.bounds.top);
		QOffsetRect(&g_theMarquee.handle, HalfRectWide(&g_theMarquee.bounds), -dist);
		break;

		case kToRight:
		QOffsetRect(&g_theMarquee.handle, g_theMarquee.bounds.right,
				g_theMarquee.bounds.top);
		QOffsetRect(&g_theMarquee.handle, dist, HalfRectTall(&g_theMarquee.bounds));
		break;

		case kBelow:
		QOffsetRect(&g_theMarquee.handle, g_theMarquee.bounds.left,
				g_theMarquee.bounds.bottom);
		QOffsetRect(&g_theMarquee.handle, HalfRectWide(&g_theMarquee.bounds), dist);
		break;

		case kToLeft:
		QOffsetRect(&g_theMarquee.handle, g_theMarquee.bounds.left,
				g_theMarquee.bounds.top);
		QOffsetRect(&g_theMarquee.handle, -dist, HalfRectTall(&g_theMarquee.bounds));
		break;

		case kBottomCorner:
		QOffsetRect(&g_theMarquee.handle, g_theMarquee.bounds.right,
				g_theMarquee.bounds.bottom);
		break;

		case kTopCorner:
		QOffsetRect(&g_theMarquee.handle, g_theMarquee.bounds.right,
				g_theMarquee.bounds.top);
		break;
	}
	g_theMarquee.direction = direction;
	g_theMarquee.dist = dist;

	hdc = GetMainWindowDC();
	DrawMarquee(hdc);
	ReleaseMainWindowDC(hdc);

	SetCoordinateHVD(g_theMarquee.bounds.left, g_theMarquee.bounds.top, dist);
}

//--------------------------------------------------------------  StopMarquee

void StopMarquee (void)
{
	HDC hdc;

	if (g_gliderMarqueeUp)
	{
		hdc = GetMainWindowDC();
		DrawGliderMarquee(hdc);
		ReleaseMainWindowDC(hdc);
		g_gliderMarqueeUp = false;
	}

	if (!g_theMarquee.active)
		return;

	hdc = GetMainWindowDC();
	DrawMarquee(hdc);
	ReleaseMainWindowDC(hdc);

	g_theMarquee.active = false;
	SetCoordinateHVD(-1, -1, -1);
}

//--------------------------------------------------------------  PauseMarquee

void PauseMarquee (void)
{
	if (!g_theMarquee.active)
		return;

	g_theMarquee.paused = true;
	StopMarquee();
}

//--------------------------------------------------------------  ResumeMarquee

void ResumeMarquee (void)
{
	if (!g_theMarquee.paused)
		return;

	if (g_theMarquee.handled)
	{
		StartMarqueeHandled(&g_theMarquee.bounds, g_theMarquee.direction, g_theMarquee.dist);
		HandleBlowerGlider();
	}
	else
		StartMarquee(&g_theMarquee.bounds);
}

//--------------------------------------------------------------  DragOutMarqueeRect

void DragOutMarqueeRect (Point start, Rect *theRect)
{
	Point wasPt, newPt;
	HDC hdc;
	MSG msg;

	SetCapture(g_mainWindow);
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
			if (GetCapture() == g_mainWindow)
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
		if (GetCapture() != g_mainWindow)
			break;
	}
	if (msg.message == WM_QUIT)
	{
		PostQuitMessage((int)msg.wParam);
		if (GetCapture() == g_mainWindow)
			ReleaseCapture();
	}
	hdc = GetMainWindowDC();
	FrameMarqueeRect(hdc, theRect);
	ReleaseMainWindowDC(hdc);
}

//--------------------------------------------------------------  DragMarqueeRect

void DragMarqueeRect (Point start, Rect *theRect, Boolean lockH, Boolean lockV)
{
	HCURSOR dragCursor = NULL;
	HCURSOR oldCursor = NULL;
	Point wasPt, newPt;
	SInt16 deltaH, deltaV;
	HDC hdc;
	MSG msg;

	SetCapture(g_mainWindow);
	StopMarquee();
	hdc = GetMainWindowDC();
	g_theMarquee.bounds = *theRect;
	FrameMarqueeRect(hdc, &g_theMarquee.bounds);
	ReleaseMainWindowDC(hdc);

	dragCursor = LoadCursor(NULL, IDC_SIZEALL);
	if (dragCursor != NULL)
	{
		oldCursor = SetCursor(dragCursor);
	}

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
			FrameMarqueeRect(hdc, &g_theMarquee.bounds);
			QOffsetRect(&g_theMarquee.bounds, deltaH, deltaV);
			FrameMarqueeRect(hdc, &g_theMarquee.bounds);
			ReleaseMainWindowDC(hdc);
			wasPt = newPt;
			SetCoordinateHVD(g_theMarquee.bounds.left, g_theMarquee.bounds.top, -2);
			break;

		case WM_LBUTTONUP:
			if (GetCapture() == g_mainWindow)
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
		if (GetCapture() != g_mainWindow)
			break;
	}
	if (msg.message == WM_QUIT)
	{
		PostQuitMessage((int)msg.wParam);
		if (GetCapture() == g_mainWindow)
			ReleaseCapture();
	}

	hdc = GetMainWindowDC();
	FrameMarqueeRect(hdc, &g_theMarquee.bounds);
	ReleaseMainWindowDC(hdc);
	*theRect = g_theMarquee.bounds;

	if (dragCursor != NULL)
	{
		SetCursor(oldCursor);
	}
}

//--------------------------------------------------------------  DragMarqueeHandle

void DragMarqueeHandle (Point start, SInt16 *dragged)
{
	HCURSOR dragCursor = NULL;
	HCURSOR oldCursor = NULL;
	Point wasPt, newPt;
	SInt16 deltaH, deltaV;
	HDC hdc;
	MSG msg;

	SetCapture(g_mainWindow);
	StopMarquee();
	hdc = GetMainWindowDC();
	FrameMarqueeRect(hdc, &g_theMarquee.bounds);
	PaintMarqueeRect(hdc, &g_theMarquee.handle);
	ReleaseMainWindowDC(hdc);

	if ((g_theMarquee.direction == kAbove) || (g_theMarquee.direction == kBelow))
	{
		dragCursor = LoadCursor(NULL, IDC_SIZENS);
	}
	else
	{
		dragCursor = LoadCursor(NULL, IDC_SIZEWE);
	}
	if (dragCursor != NULL)
	{
		oldCursor = SetCursor(dragCursor);
	}

	wasPt = start;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			newPt.h = GET_X_LPARAM(msg.lParam);
			newPt.v = GET_Y_LPARAM(msg.lParam);
			switch (g_theMarquee.direction)
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
			PaintMarqueeRect(hdc, &g_theMarquee.handle);
			QOffsetRect(&g_theMarquee.handle, deltaH, deltaV);
			PaintMarqueeRect(hdc, &g_theMarquee.handle);
			ReleaseMainWindowDC(hdc);
			wasPt = newPt;
			break;

		case WM_LBUTTONUP:
			if (GetCapture() == g_mainWindow)
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
		if (GetCapture() != g_mainWindow)
			break;
	}
	if (msg.message == WM_QUIT)
	{
		PostQuitMessage((int)msg.wParam);
		if (GetCapture() == g_mainWindow)
			ReleaseCapture();
	}

	hdc = GetMainWindowDC();
	FrameMarqueeRect(hdc, &g_theMarquee.bounds);
	PaintMarqueeRect(hdc, &g_theMarquee.handle);
	ReleaseMainWindowDC(hdc);

	if (dragCursor != NULL)
	{
		SetCursor(oldCursor);
	}
}

//--------------------------------------------------------------  DragMarqueeCorner

void DragMarqueeCorner (Point start, SInt16 *hDragged, SInt16 *vDragged, Boolean isTop)
{
	HCURSOR dragCursor = NULL;
	HCURSOR oldCursor = NULL;
	Point wasPt, newPt;
	SInt16 deltaH, deltaV;
	HDC hdc;
	MSG msg;

	SetCapture(g_mainWindow);
	StopMarquee();
	hdc = GetMainWindowDC();
	FrameMarqueeRect(hdc, &g_theMarquee.bounds);
	PaintMarqueeRect(hdc, &g_theMarquee.handle);
	ReleaseMainWindowDC(hdc);

	if (isTop)
	{
		dragCursor = LoadCursor(NULL, IDC_SIZENESW);
	}
	else
	{
		dragCursor = LoadCursor(NULL, IDC_SIZENWSE);
	}
	if (dragCursor != NULL)
	{
		oldCursor = SetCursor(dragCursor);
	}

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
			FrameMarqueeRect(hdc, &g_theMarquee.bounds);
			PaintMarqueeRect(hdc, &g_theMarquee.handle);
			if (isTop)
			{
				QOffsetRect(&g_theMarquee.handle, deltaH, -deltaV);
				g_theMarquee.bounds.right += deltaH;
				g_theMarquee.bounds.top -= deltaV;
			}
			else
			{
				QOffsetRect(&g_theMarquee.handle, deltaH, deltaV);
				g_theMarquee.bounds.right += deltaH;
				g_theMarquee.bounds.bottom += deltaV;
			}
			FrameMarqueeRect(hdc, &g_theMarquee.bounds);
			PaintMarqueeRect(hdc, &g_theMarquee.handle);
			ReleaseMainWindowDC(hdc);
			wasPt = newPt;
			break;

		case WM_LBUTTONUP:
			if (GetCapture() == g_mainWindow)
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
		if (GetCapture() != g_mainWindow)
			break;
	}
	if (msg.message == WM_QUIT)
	{
		PostQuitMessage((int)msg.wParam);
		if (GetCapture() == g_mainWindow)
			ReleaseCapture();
	}

	hdc = GetMainWindowDC();
	FrameMarqueeRect(hdc, &g_theMarquee.bounds);
	PaintMarqueeRect(hdc, &g_theMarquee.handle);
	ReleaseMainWindowDC(hdc);

	if (dragCursor != NULL)
	{
		SetCursor(oldCursor);
	}
}

//--------------------------------------------------------------  MarqueeHasHandles

Boolean MarqueeHasHandles (SInt16 *direction, SInt16 *dist)
{
	if (g_theMarquee.handled)
	{
		*direction = g_theMarquee.direction;
		*dist = g_theMarquee.dist;
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
	return g_theMarquee.handled && QPtInRect(where, &g_theMarquee.handle);
}

//--------------------------------------------------------------  DrawGliderMarquee

void DrawGliderMarquee (HDC hdc)
{
	Mac_CopyBits(g_blowerMaskMap, hdc,
			&g_leftStartGliderSrc, &g_marqueeGliderRect,
			srcXor, nil);
}

//--------------------------------------------------------------  SetMarqueeGliderCenter

void SetMarqueeGliderRect (SInt16 h, SInt16 v)
{
	HDC hdc;

	g_marqueeGliderRect = g_leftStartGliderSrc;
	ZeroRectCorner(&g_marqueeGliderRect);
	QOffsetRect(&g_marqueeGliderRect, h - kHalfGliderWide, v - kGliderHigh);

	hdc = GetMainWindowDC();
	DrawGliderMarquee(hdc);
	ReleaseMainWindowDC(hdc);
	g_gliderMarqueeUp = true;
}

//--------------------------------------------------------------  DrawMarquee

void DrawMarquee (HDC hdc)
{
	HBRUSH marqueeBrush;
	HPEN marqueePen, wasPen;
	int wasROP2;

	marqueeBrush = CreateMarqueeBrush();
	wasROP2 = SetROP2(hdc, R2_XORPEN);

	Mac_FrameRect(hdc, &g_theMarquee.bounds, marqueeBrush, 1, 1); 
	if (g_theMarquee.handled)
	{
		Mac_PaintRect(hdc, &g_theMarquee.handle, marqueeBrush);
		marqueePen = CreateMarqueePen();
		wasPen = (HPEN)SelectObject(hdc, marqueePen);
		switch (g_theMarquee.direction)
		{
			case kAbove:
			MoveToEx(hdc, g_theMarquee.handle.left + (kHandleSideLong / 2),
					g_theMarquee.handle.bottom, NULL);
			Mac_LineTo(hdc, g_theMarquee.handle.left + (kHandleSideLong / 2),
					g_theMarquee.bounds.top - 1);
			break;

			case kToRight:
			MoveToEx(hdc, g_theMarquee.handle.left,
					g_theMarquee.handle.top + (kHandleSideLong / 2), NULL);
			Mac_LineTo(hdc, g_theMarquee.bounds.right,
					g_theMarquee.handle.top + (kHandleSideLong / 2));
			break;

			case kBelow:
			MoveToEx(hdc, g_theMarquee.handle.left + (kHandleSideLong / 2),
					g_theMarquee.handle.top - 1, NULL);
			Mac_LineTo(hdc, g_theMarquee.handle.left + (kHandleSideLong / 2),
					g_theMarquee.bounds.bottom);
			break;

			case kToLeft:
			MoveToEx(hdc, g_theMarquee.handle.right,
					g_theMarquee.handle.top + (kHandleSideLong / 2), NULL);
			Mac_LineTo(hdc, g_theMarquee.bounds.left,
					g_theMarquee.handle.top + (kHandleSideLong / 2));
			break;
		}
		SelectObject(hdc, wasPen);
		DeleteObject(marqueePen);
	}

	SetROP2(hdc, wasROP2);
	DeleteObject(marqueeBrush);

	if (g_gliderMarqueeUp)
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
		g_theMarquee.pats[i] = hBitmap;
	}
	DeleteDC(hdc);
	ImageList_Destroy(himlMarquee);

	g_theMarquee.index = 0;
	g_theMarquee.active = false;
	g_theMarquee.paused = false;
	g_theMarquee.handled = false;
	g_gliderMarqueeUp = false;
}
