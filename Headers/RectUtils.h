//============================================================================
//----------------------------------------------------------------------------
//								   RectUtils.h
//----------------------------------------------------------------------------
//============================================================================


#pragma once


//#include <Quickdraw.h>
#include "Macintosh.h"


void FrameWHRect (short, short, short, short);
void NormalizeRect (Rect *);
void ZeroRectCorner (Rect *);
void CenterRectOnPoint (Rect *, Point);
SInt16 HalfRectWide (Rect *);
SInt16 HalfRectTall (Rect *);
short RectWide (Rect *);
short RectTall (Rect *);
void GlobalToLocalRect (Rect *);
void LocalToGlobalRect (Rect *);
void CenterRectInRect (Rect *, Rect *);
void HOffsetRect (Rect *, short);
void VOffsetRect (Rect *, short);
Boolean IsRectLeftOfRect (Rect *, Rect *);
void QOffsetRect (Rect *, short, short);
void QSetRect (Rect *, short, short, short, short);
Boolean ForceRectInRect (Rect *, Rect *);
void QUnionSimilarRect (Rect *, Rect *, Rect *);
void FrameRectSansCorners (Rect *);
void SetEraseRect (short, short, short, short);
