#ifndef DRAW_UTILS_H_
#define DRAW_UTILS_H_

#include "WinAPI.h"

// Draws a line in GDI that includes the pixels at the start and at
// the end of the line, like Macintosh's QuickDraw does. If the start
// and end of the line are at the same place, a single dot is drawn.
// The coordinates are in logical space, and the current pen is used
// to draw the line.
void DrawInclusiveLine(HDC hdcDest, int startX, int startY, int endX, int endY);

// Similar to DrawInclusiveLine, but specifies the end point of the line
// as an offset to the start point.
void DrawInclusiveLineDelta(HDC hdcDest, int startX, int startY, int offsetX, int offsetY);

#endif
