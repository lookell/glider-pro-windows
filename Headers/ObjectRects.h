#ifndef OBJECT_RECTS_H_
#define OBJECT_RECTS_H_

//============================================================================
//----------------------------------------------------------------------------
//                               ObjectRects.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderStructs.h"
#include "MacTypes.h"

void GetObjectRect (objectPtr who, Rect *theRect);
SInt16 CreateActiveRects (SInt16 who);
SInt16 VerticalRoomOffset (SInt16 neighbor);
void OffsetRectRoomRelative (Rect *theRect, SInt16 neighbor);
SInt16 GetUpStairsRightEdge (void);
SInt16 GetDownStairsLeftEdge (void);

#endif
