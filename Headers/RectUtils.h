//============================================================================
//----------------------------------------------------------------------------
//                                RectUtils.h
//----------------------------------------------------------------------------
//============================================================================

#ifndef RECT_UTILS_H_
#define RECT_UTILS_H_

#include "MacTypes.h"

void NormalizeRect (Rect *theRect);
void ZeroRectCorner (Rect *theRect);
void CenterRectOnPoint (Rect *theRect, Point where);
SInt16 HalfRectWide (const Rect *theRect);
SInt16 HalfRectTall (const Rect *theRect);
SInt16 RectWide (const Rect *theRect);
SInt16 RectTall (const Rect *theRect);
void CenterRectInRect (Rect *rectA, const Rect *rectB);
void HOffsetRect (Rect *theRect, SInt16 h);
void VOffsetRect (Rect *theRect, SInt16 v);
Boolean IsRectLeftOfRect (const Rect *rect1, const Rect *rect2);
void QOffsetRect (Rect *theRect, SInt16 h, SInt16 v);
void QSetRect (Rect *theRect, SInt16 l, SInt16 t, SInt16 r, SInt16 b);
Boolean ForceRectInRect (Rect *small_, const Rect *large);
void QUnionSimilarRect (const Rect *rectA, const Rect *rectB, Rect *rectC);
void QInsetRect (Rect *r, SInt16 dh, SInt16 dv);
Boolean QPtInRect (Point pt, const Rect *r);
Boolean QSectRect (const Rect *src1, const Rect *src2, Rect *dstRect);

#endif
