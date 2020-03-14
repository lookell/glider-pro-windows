//============================================================================
//----------------------------------------------------------------------------
//								   RectUtils.h
//----------------------------------------------------------------------------
//============================================================================


#pragma once


//#include <Quickdraw.h>
#include "Macintosh.h"


void FrameWHRect (SInt16, SInt16, SInt16, SInt16);
void NormalizeRect (Rect *);
void ZeroRectCorner (Rect *);
void CenterRectOnPoint (Rect *, Point);
SInt16 HalfRectWide (Rect *);
SInt16 HalfRectTall (Rect *);
SInt16 RectWide (Rect *);
SInt16 RectTall (Rect *);
void GlobalToLocalRect (Rect *);
void LocalToGlobalRect (Rect *);
void CenterRectInRect (Rect *, Rect *);
void HOffsetRect (Rect *, SInt16);
void VOffsetRect (Rect *, SInt16);
Boolean IsRectLeftOfRect (Rect *, Rect *);
void QOffsetRect (Rect *, SInt16, SInt16);
void QSetRect (Rect *, SInt16, SInt16, SInt16, SInt16);
Boolean ForceRectInRect (Rect *, Rect *);
void QUnionSimilarRect (Rect *, Rect *, Rect *);
void FrameRectSansCorners (Rect *);
void SetEraseRect (SInt16, SInt16, SInt16, SInt16);
