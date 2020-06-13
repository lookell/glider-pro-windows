#ifndef RECT_UTILS_H_
#define RECT_UTILS_H_

//============================================================================
//----------------------------------------------------------------------------
//                                RectUtils.h
//----------------------------------------------------------------------------
//============================================================================

#pragma once

#include "MacTypes.h"
#include "WinAPI.h"

#undef small // Remove <windows.h> definition, since it's not wanted here

void FrameWHRect (HDC hdc, SInt16 left, SInt16 top, SInt16 wide, SInt16 high);
void NormalizeRect (Rect *theRect);
void ZeroRectCorner (Rect *theRect);
void CenterRectOnPoint (Rect *theRect, Point where);
SInt16 HalfRectWide (Rect *theRect);
SInt16 HalfRectTall (Rect *theRect);
SInt16 RectWide (Rect *theRect);
SInt16 RectTall (Rect *theRect);
void GlobalToLocalRect (HWND hwnd, Rect *theRect);
void LocalToGlobalRect (HWND hwnd, Rect *theRect);
void CenterRectInRect (Rect *rectA, Rect *rectB);
void HOffsetRect (Rect *theRect, SInt16 h);
void VOffsetRect (Rect *theRect, SInt16 v);
Boolean IsRectLeftOfRect (Rect *rect1, Rect *rect2);
void QOffsetRect (Rect *theRect, SInt16 h, SInt16 v);
void QSetRect (Rect *theRect, SInt16 l, SInt16 t, SInt16 r, SInt16 b);
Boolean ForceRectInRect (Rect *small, Rect *large);
void QUnionSimilarRect (Rect *rectA, Rect *rectB, Rect *rectC);
void FrameRectSansCorners (HDC hdc, Rect *theRect);

#endif
