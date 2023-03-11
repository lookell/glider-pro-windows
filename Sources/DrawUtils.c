#include "DrawUtils.h"

void DrawInclusiveLine(HDC hdcDest, int startX, int startY, int endX, int endY)
{
	POINT pointList[3];
	int pointCount;

	pointList[0].x = startX;
	pointList[0].y = startY;
	pointList[1].x = endX;
	pointList[1].y = endY;
	pointList[2].x = startX;
	pointList[2].y = startY;
	pointCount = ARRAYSIZE(pointList);
	if (startX == endX && startY == endY)
	{
		// ensure that a single dot is drawn, instead of an otherwise empty line.
		pointList[1].x += 1;
		pointCount = 2;
	}
	Polyline(hdcDest, pointList, pointCount);
}

void DrawInclusiveLineDelta(HDC hdcDest, int startX, int startY, int offsetX, int offsetY)
{
	DrawInclusiveLine(hdcDest, startX, startY, startX + offsetX, startY + offsetY);
}
