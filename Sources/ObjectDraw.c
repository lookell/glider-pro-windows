//============================================================================
//----------------------------------------------------------------------------
//                                ObjectDraw.c
//----------------------------------------------------------------------------
//============================================================================

#include "ObjectDraw.h"

#include "ColorUtils.h"
#include "HouseIO.h"
#include "Macintosh.h"
#include "Objects.h"
#include "RectUtils.h"
#include "ResourceIDs.h"
#include "ResourceLoader.h"
#include "Utilities.h"

#define k8WhiteColor            0
#define kYellowColor            5
#define kIntenseYellowColor     5
#define kGoldColor              11
#define k8RedColor              35
#define kPaleVioletColor        42
#define k8LtTanColor            52
#define k8BambooColor           53
#define kDarkFleshColor         58
#define k8TanColor              94
#define k8PissYellowColor       95
#define k8OrangeColor           59
#define k8BrownColor            137
#define k8Red4Color             143
#define k8SkyColor              150
#define k8EarthBlueColor        170
#define k8DkRedColor            222
#define k8DkRed2Color           223
#define kIntenseGreenColor      225
#define kIntenseBlueColor       235
#define k8PumpkinColor          101
#define k8LtstGrayColor         245
#define k8LtstGray2Color        246
#define k8LtstGray3Color        43
#define k8LtstGray4Color        247
#define k8LtstGray5Color        248
#define k8LtGrayColor           249
#define k8GrayColor             250
#define k8Gray2Color            251
#define k8DkGrayColor           252
#define k8DkGray2Color          253
#define k8DkGray3Color          172
#define k8DkstGrayColor         254
#define k8BlackColor            255

#define kTikiPoleBase           300
#define kMailboxBase            296

void DrawClockDigit (HDC hdcDest, SInt16 number, const Rect *dest);
void DrawClockHands (HDC hdcDest, Point where, SInt16 bigHand, SInt16 littleHand);
void DrawLargeClockHands (HDC hdcDest, Point where, SInt16 bigHand, SInt16 littleHand);
void CopyBitsSansWhite (HDC srcBits, HDC dstBits, const Rect *srcRect, const Rect *dstRect);

//==============================================================  Functions
//--------------------------------------------------------------  DrawSimpleBlowers

void DrawSimpleBlowers (HDC hdcDest, SInt16 what, const Rect *theRect)
{
	if (what < 0 || what >= ARRAYSIZE(g_srcRects))
		return;

	Mac_CopyMask(g_blowerSrcMap, g_blowerMaskMap, hdcDest,
			&g_srcRects[what], &g_srcRects[what], theRect);
}

//--------------------------------------------------------------  DrawTiki

void DrawTiki (HDC hdcDest, const Rect *theRect, SInt16 down)
{
	SInt32 darkGrayC, lightWoodC, darkWoodC;

	darkGrayC = k8DkstGrayColor;
	lightWoodC = k8BambooColor;
	darkWoodC = k8PissYellowColor;

	if (theRect->bottom < kTikiPoleBase + down)
	{
		ColorLine(hdcDest, theRect->left + 11, theRect->bottom - 1,
				theRect->left + 11, kTikiPoleBase + down - 1, darkGrayC);
		ColorLine(hdcDest, theRect->left + 12, theRect->bottom - 1,
				theRect->left + 12, kTikiPoleBase + down, lightWoodC);
		ColorLine(hdcDest, theRect->left + 13, theRect->bottom - 1,
				theRect->left + 13, kTikiPoleBase + down, darkWoodC);
		ColorLine(hdcDest, theRect->left + 14, theRect->bottom - 1,
				theRect->left + 14, kTikiPoleBase + down, darkWoodC);
		ColorLine(hdcDest, theRect->left + 15, theRect->bottom - 1,
				theRect->left + 15, kTikiPoleBase + down - 1, darkGrayC);
	}

	Mac_CopyMask(g_blowerSrcMap, g_blowerMaskMap, hdcDest,
			&g_srcRects[kTiki], &g_srcRects[kTiki], theRect);
}

//--------------------------------------------------------------  DrawInvisibleBlower

void DrawInvisibleBlower (HDC hdcDest, const Rect *theRect)
{
	Rect tempRect;

	QSetRect(&tempRect, 0, 0, 24, 24);
	QOffsetRect(&tempRect, theRect->left, theRect->top);

	ColorFrameRect(hdcDest, &tempRect, 192);
}

//--------------------------------------------------------------  DrawLiftArea

void DrawLiftArea (HDC hdcDest, const Rect *theRect)
{
	ColorFrameRect(hdcDest, theRect, 192);
}

//--------------------------------------------------------------  DrawTable

void DrawTable (HDC hdcDest, const Rect *tableTop, SInt16 down)
{
	#define kTableBaseTop  296
	#define kTableShadowTop  312
	#define kTableShadowOffset  12
	Rect tempRect;
	SInt32 brownC, tanC, dkRedC, blackC;
	SInt16 hCenter, vShadow;

	brownC = k8BrownColor;
	tanC = k8TanColor;
	dkRedC = k8DkRed2Color;
	blackC = k8BlackColor;

	QSetRect(&tempRect, tableTop->left, 0, tableTop->right,
			RectWide(tableTop) / 10);
	QOffsetRect(&tempRect, 0,
			-HalfRectTall(&tempRect) + kTableShadowTop + down);
	QOffsetRect(&tempRect, kTableShadowOffset, -kTableShadowOffset);
	DitherShadowOval(hdcDest, &tempRect);

	tempRect = *tableTop;
	QInsetRect(&tempRect, 0, 1);
	ColorRect(hdcDest, &tempRect, brownC);

	ColorLine(hdcDest, tableTop->left, tableTop->top + 1,
			tableTop->left, tableTop->top + 1, k8WhiteColor);
	ColorLine(hdcDest, tableTop->left + 1, tableTop->top,
			tableTop->right - 2, tableTop->top, k8WhiteColor);
	ColorLine(hdcDest, tableTop->right - 1, tableTop->top + 1,
			tableTop->right - 1, tableTop->top + 1, k8WhiteColor);

	ColorLine(hdcDest, tableTop->left + 1, tableTop->top + 1,
			tableTop->right - 2, tableTop->top + 1, tanC);
	ColorLine(hdcDest, tableTop->left, tableTop->top + 2,
			tableTop->left, tableTop->bottom - 2, tanC);

	ColorLine(hdcDest, tableTop->left + 1, tableTop->bottom - 1,
			tableTop->right - 2, tableTop->bottom - 1, blackC);
	ColorLine(hdcDest, tableTop->right - 1, tableTop->top + 2,
			tableTop->right - 1, tableTop->bottom - 2, blackC);

	ColorLine(hdcDest, tableTop->left + 1, tableTop->bottom - 2,
			tableTop->right - 2, tableTop->bottom - 2, dkRedC);

	if (tableTop->bottom < kTableBaseTop + down)
	{
		hCenter = (tableTop->left + tableTop->right) / 2;

		ColorLine(hdcDest, hCenter - 3, tableTop->bottom,
				hCenter - 3, kTableBaseTop + down, blackC);
		ColorLine(hdcDest, hCenter - 2, tableTop->bottom,
				hCenter - 2, kTableBaseTop + down, k8LtGrayColor);
		ColorLine(hdcDest, hCenter - 1, tableTop->bottom,
				hCenter - 1, kTableBaseTop + down, k8GrayColor);
		ColorLine(hdcDest, hCenter, tableTop->bottom,
				hCenter, kTableBaseTop + down, k8DkGrayColor);
		ColorLine(hdcDest, hCenter + 1, tableTop->bottom,
				hCenter + 1, kTableBaseTop + down, blackC);

		vShadow = tableTop->bottom + RectWide(tableTop) / 4 - 2;
		if (vShadow > kTableBaseTop + down)
		{
			ColorLine(hdcDest, hCenter - 2, tableTop->bottom,
					hCenter - 2, kTableBaseTop + down, k8DkGrayColor);
			ColorLine(hdcDest, hCenter - 1, tableTop->bottom,
					hCenter - 1, kTableBaseTop + down, k8DkGrayColor);
			ColorLine(hdcDest, hCenter, tableTop->bottom,
					hCenter, kTableBaseTop + down, blackC);
		}
		else
		{
			ColorLine(hdcDest, hCenter - 2, tableTop->bottom,
					hCenter - 2, vShadow, k8DkGrayColor);
			ColorLine(hdcDest, hCenter - 1, tableTop->bottom,
					hCenter - 1, vShadow + 1, k8DkGrayColor);
			ColorLine(hdcDest, hCenter, tableTop->bottom,
					hCenter, vShadow + 2, blackC);
		}
	}

	tempRect = g_tableSrc;
	QOffsetRect(&tempRect, -HalfRectWide(&g_tableSrc) + tableTop->left +
			HalfRectWide(tableTop), kTableBaseTop + down);
	Mac_CopyMask(g_furnitureSrcMap, g_furnitureMaskMap, hdcDest,
			&g_tableSrc, &g_tableSrc, &tempRect);
}

//--------------------------------------------------------------  DrawShelf

void DrawShelf (HDC hdcDest, const Rect *shelfTop)
{
	#define kBracketInset  18
	#define kShelfDeep  4
	#define kBracketThick  5
	#define kShelfShadowOff  12
	Rect tempRect;
	SInt32 brownC, ltTanC, tanC, dkRedC, blackC;

	brownC = k8BrownColor;
	ltTanC = k8LtTanColor;
	tanC = k8TanColor;
	dkRedC = k8DkRed2Color;
	blackC = k8BlackColor;

	BeginPath(hdcDest);
	MoveToEx(hdcDest, shelfTop->left, shelfTop->bottom, NULL);
	Mac_Line(hdcDest, kShelfShadowOff, kShelfShadowOff);
	Mac_Line(hdcDest, RectWide(shelfTop) - kShelfDeep, 0);
	Mac_Line(hdcDest, 0, -kShelfThick + 1);
	Mac_Line(hdcDest, -kShelfShadowOff, -kShelfShadowOff);
	Mac_LineTo(hdcDest, shelfTop->left, shelfTop->bottom);
	EndPath(hdcDest);
	DitherShadowPath(hdcDest);

	tempRect = *shelfTop;
	QInsetRect(&tempRect, 0, 1);
	ColorRect(hdcDest, &tempRect, brownC);

	ColorLine(hdcDest, shelfTop->left + 1, shelfTop->top,
			shelfTop->left + 1 + kShelfDeep, shelfTop->top, ltTanC);
	ColorLine(hdcDest, shelfTop->left, shelfTop->top + 1,
			shelfTop->left + kShelfDeep, shelfTop->top + 1, tanC);
	ColorLine(hdcDest, shelfTop->left, shelfTop->top + 2,
			shelfTop->left + kShelfDeep, shelfTop->top + 2, tanC);
	ColorLine(hdcDest, shelfTop->left, shelfTop->top + 3,
			shelfTop->left + kShelfDeep, shelfTop->top + 3, tanC);
	ColorLine(hdcDest, shelfTop->left + 1, shelfTop->bottom - 1,
			shelfTop->left + 1 + kShelfDeep, shelfTop->bottom - 1, dkRedC);
	ColorLine(hdcDest, shelfTop->left + 2 + kShelfDeep, shelfTop->bottom - 1,
			shelfTop->right - 2, shelfTop->bottom - 1, blackC);
	ColorLine(hdcDest, shelfTop->left + 2 + kShelfDeep, shelfTop->top,
			shelfTop->right - 2, shelfTop->top, tanC);
	ColorLine(hdcDest, shelfTop->right - 1, shelfTop->top + 1,
			shelfTop->right - 1, shelfTop->bottom - 2, blackC);

	tempRect = g_shelfSrc;
	ZeroRectCorner(&tempRect);
	QOffsetRect(&tempRect, shelfTop->left + kBracketInset, shelfTop->bottom);
	Mac_CopyMask(g_furnitureSrcMap, g_furnitureMaskMap, hdcDest,
			&g_shelfSrc, &g_shelfSrc, &tempRect);

	ZeroRectCorner(&tempRect);
	QOffsetRect(&tempRect, shelfTop->right - kBracketInset - kShelfDeep -
			kBracketThick, shelfTop->bottom);
	Mac_CopyMask(g_furnitureSrcMap, g_furnitureMaskMap, hdcDest,
			&g_shelfSrc, &g_shelfSrc, &tempRect);
}

//--------------------------------------------------------------  DrawCabinet

void DrawCabinet (HDC hdcDest, const Rect *cabinet)
{
	#define kCabinetDeep  4
	#define kCabinetShadowOff  6
	Rect tempRect;
	SInt32 brownC, dkGrayC, ltTanC, tanC, dkRedC;

	brownC = k8BrownColor;
	dkGrayC = k8DkstGrayColor;
	ltTanC = k8LtTanColor;
	tanC = k8TanColor;
	dkRedC = k8DkRed2Color;

	BeginPath(hdcDest);
	MoveToEx(hdcDest, cabinet->left, cabinet->bottom, NULL);
	Mac_Line(hdcDest, kCabinetShadowOff, kCabinetShadowOff);
	Mac_Line(hdcDest, RectWide(cabinet), 0);
	Mac_Line(hdcDest, 0, -RectTall(cabinet) + kCabinetDeep);
	Mac_Line(hdcDest, -kCabinetShadowOff, -kCabinetShadowOff);
	Mac_LineTo(hdcDest, cabinet->left, cabinet->bottom);
	EndPath(hdcDest);
	DitherShadowPath(hdcDest);

	// fill bulk of cabinet brown
	tempRect = *cabinet;
	QInsetRect(&tempRect, 1, 1);
	ColorRect(hdcDest, &tempRect, brownC);

	// add lighter left side
	tempRect = *cabinet;
	tempRect.right = tempRect.left + kCabinetDeep;
	ColorRect(hdcDest, &tempRect, tanC);

	// hilight top edge
	ColorLine(hdcDest, cabinet->left + 1, cabinet->top + 1,
			cabinet->left + kCabinetDeep, cabinet->top + 1, ltTanC);
	ColorLine(hdcDest, cabinet->left + kCabinetDeep, cabinet->top + 1,
			cabinet->right - 3, cabinet->top + 1, tanC);

	// shadow bottom edge
	ColorLine(hdcDest, cabinet->left + kCabinetDeep + 3, cabinet->top + 5,
			cabinet->left + kCabinetDeep + 3, cabinet->bottom - 6, tanC);
	ColorLine(hdcDest, cabinet->left + kCabinetDeep + 4, cabinet->top + 5,
			cabinet->left + kCabinetDeep + 4, cabinet->bottom - 6, tanC);
	ColorLine(hdcDest, cabinet->left + kCabinetDeep + 9, cabinet->top + 10,
			cabinet->left + kCabinetDeep + 9, cabinet->bottom - 11, dkGrayC);

	ColorLine(hdcDest, cabinet->right - 4, cabinet->top + 6,
			cabinet->right - 4, cabinet->bottom - 5, dkRedC);
	ColorLine(hdcDest, cabinet->right - 5, cabinet->top + 5,
			cabinet->right - 5, cabinet->bottom - 6, dkGrayC);
	ColorLine(hdcDest, cabinet->right - 10, cabinet->top + 10,
			cabinet->right - 10, cabinet->bottom - 11, tanC);

	ColorLine(hdcDest, cabinet->left + kCabinetDeep + 4, cabinet->top + 4,
			cabinet->left + kCabinetDeep + 4, cabinet->top + 4, ltTanC);
	ColorLine(hdcDest, cabinet->left + kCabinetDeep + 5, cabinet->top + 4,
			cabinet->right - 6, cabinet->top + 4, tanC);
	ColorLine(hdcDest, cabinet->left + kCabinetDeep + 10, cabinet->top + 9,
			cabinet->right - 11, cabinet->top + 9, dkGrayC);

	ColorLine(hdcDest, cabinet->right - 5, cabinet->bottom - 5,
			cabinet->right - 5, cabinet->bottom - 5, dkRedC);
	ColorLine(hdcDest, cabinet->left + kCabinetDeep + 6, cabinet->bottom - 4,
			cabinet->right - 5, cabinet->bottom - 4, dkRedC);
	ColorLine(hdcDest, cabinet->left + kCabinetDeep + 5, cabinet->bottom - 5,
			cabinet->right - 6, cabinet->bottom - 5, dkGrayC);

	ColorLine(hdcDest, cabinet->left + kCabinetDeep + 10, cabinet->bottom - 10,
			cabinet->right - 11, cabinet->bottom - 10, tanC);

	tempRect = g_hingeSrc;
	ZeroRectCorner(&tempRect);
	QOffsetRect(&tempRect, cabinet->left + kCabinetDeep + 2, cabinet->top + 10);
	Mac_CopyMask(g_furnitureSrcMap, g_furnitureMaskMap, hdcDest,
			&g_hingeSrc, &g_hingeSrc, &tempRect);

	tempRect = g_hingeSrc;
	ZeroRectCorner(&tempRect);
	QOffsetRect(&tempRect, cabinet->left + kCabinetDeep + 2, cabinet->bottom - 26);
	Mac_CopyMask(g_furnitureSrcMap, g_furnitureMaskMap, hdcDest,
			&g_hingeSrc, &g_hingeSrc, &tempRect);

	tempRect = g_handleSrc;
	ZeroRectCorner(&tempRect);
	QOffsetRect(&tempRect, cabinet->right - 8, cabinet->top +
			HalfRectTall(cabinet) - HalfRectTall(&g_handleSrc));
	Mac_CopyMask(g_furnitureSrcMap, g_furnitureMaskMap, hdcDest,
			&g_handleSrc, &g_handleSrc, &tempRect);

	Mac_FrameRect(hdcDest, cabinet, GetStockBrush(BLACK_BRUSH), 1, 1);
}

//--------------------------------------------------------------  DrawSimpleFurniture

void DrawSimpleFurniture (HDC hdcDest, SInt16 what, const Rect *theRect)
{
	if (what < 0 || what >= ARRAYSIZE(g_srcRects))
		return;

	Mac_CopyMask(g_furnitureSrcMap, g_furnitureMaskMap, hdcDest,
			&g_srcRects[what], &g_srcRects[what], theRect);
}

//--------------------------------------------------------------  DrawCounter

void DrawCounter (HDC hdcDest, const Rect *counter)
{
	#define kCounterFooterHigh  12
	#define kCounterStripWide  6
	#define kCounterStripTall  29
	#define kCounterPanelDrop  12
	Rect tempRect;
	SInt32 brownC, dkGrayC, tanC, blackC, dkstRedC;
	SInt16 nRects, width, i;

	brownC = k8BrownColor;
	dkGrayC = k8DkstGrayColor;
	tanC = k8TanColor;
	blackC = k8BlackColor;
	dkstRedC = k8DkRed2Color;

	BeginPath(hdcDest);
	MoveToEx(hdcDest, counter->right - 2, counter->bottom, NULL);
	Mac_Line(hdcDest, 10, -10);
	Mac_Line(hdcDest, 0, -RectTall(counter) + 29);
	Mac_Line(hdcDest, 2, 0);
	Mac_Line(hdcDest, 0, -7);
	Mac_Line(hdcDest, -12, -12);
	Mac_LineTo(hdcDest, counter->right - 2, counter->bottom);
	EndPath(hdcDest);
	DitherShadowPath(hdcDest);

	tempRect = *counter;
	QInsetRect(&tempRect, 2, 2);
	ColorRect(hdcDest, &tempRect, brownC);

	tempRect = *counter;
	tempRect.top = tempRect.bottom - kCounterFooterHigh;
	tempRect.left += 2;
	tempRect.right -= 2;
	ColorRect(hdcDest, &tempRect, dkGrayC);
	ColorLine(hdcDest, counter->left + 2, counter->bottom - kCounterFooterHigh,
			counter->right - 3, counter->bottom - kCounterFooterHigh, blackC);
	ColorLine(hdcDest, counter->left + 2, counter->bottom - kCounterFooterHigh + 1,
			counter->right - 3, counter->bottom - kCounterFooterHigh + 1, blackC);
	ColorLine(hdcDest, counter->right - 3, counter->bottom - kCounterFooterHigh,
			counter->right - 3, counter->bottom - 1, blackC);
	ColorLine(hdcDest, counter->left + 2, counter->bottom - kCounterFooterHigh,
			counter->left + 2, counter->bottom - 1, k8DkGrayColor);

	ColorLine(hdcDest, counter->right - 2, counter->top,
			counter->right - 2, counter->bottom - kCounterFooterHigh - 1, dkstRedC);
	ColorLine(hdcDest, counter->left + 1, counter->top + 8,
			counter->left + 1, counter->bottom - kCounterFooterHigh - 1, tanC);

	ColorLine(hdcDest, counter->left - 1, counter->top,
			counter->right, counter->top, k8LtstGrayColor);
	ColorLine(hdcDest, counter->left - 1, counter->top + 1,
			counter->right, counter->top + 1, k8LtstGray2Color);
	ColorLine(hdcDest, counter->left - 1, counter->top + 2,
			counter->right, counter->top + 2, k8LtstGray3Color);
	ColorLine(hdcDest, counter->left - 1, counter->top + 3,
			counter->right, counter->top + 3, k8LtstGray4Color);
	ColorLine(hdcDest, counter->left - 1, counter->top + 4,
			counter->right, counter->top + 4, k8LtstGray5Color);
	ColorLine(hdcDest, counter->left - 1, counter->top + 5,
			counter->right, counter->top + 5, k8LtstGray5Color);
	ColorLine(hdcDest, counter->left - 1, counter->top + 6,
			counter->right, counter->top + 6, k8LtstGray5Color);
	ColorLine(hdcDest, counter->left - 1, counter->top,
			counter->left - 1, counter->top + 6, k8LtstGrayColor);

	ColorLine(hdcDest, counter->right, counter->top,
			counter->right, counter->top + 6, k8LtGrayColor);
	ColorLine(hdcDest, counter->left + 1, counter->top + 7,
			counter->right - 2, counter->top + 7, dkstRedC);
	ColorLine(hdcDest, counter->left + 1, counter->top + 8,
			counter->right - 2, counter->top + 8, dkstRedC);

	nRects = RectWide(counter) / 40;
	if (nRects == 0)
		nRects = 1;
	width = ((RectWide(counter) - kCounterStripWide) / nRects) - kCounterStripWide;
	QSetRect(&tempRect, 0, 0, width, RectTall(counter) - kCounterStripTall);
	QOffsetRect(&tempRect, counter->left + kCounterStripWide,
			counter->top + kCounterPanelDrop);
	for (i = 0; i < nRects; i++)
	{
		HiliteRect(hdcDest, &tempRect, tanC, dkstRedC);
		QInsetRect(&tempRect, 4, 4);
		HiliteRect(hdcDest, &tempRect, dkstRedC, tanC);
		QInsetRect(&tempRect, -4, -4);
		QOffsetRect(&tempRect, kCounterStripWide + width, 0);
	}
}

//--------------------------------------------------------------  DrawDresser

void DrawDresser (HDC hdcDest, const Rect *dresser)
{
	#define kDresserTopThick  4
	#define kDresserCrease  9
	#define kDresserDrawerDrop  12
	#define kDresserSideSpare  14
	Rect tempRect, dest;
	SInt32 yellowC, brownC, ltTanC, dkstRedC;
	SInt16 nRects, height, i;

	yellowC = k8PissYellowColor;
	brownC = k8BrownColor;
	ltTanC = k8LtTanColor;
	dkstRedC = k8DkRed2Color;

	BeginPath(hdcDest);
	MoveToEx(hdcDest, dresser->left + 10, dresser->bottom + 9, NULL);
	Mac_Line(hdcDest, RectWide(dresser) - 11, 0);
	Mac_Line(hdcDest, 9, -9);
	Mac_Line(hdcDest, 0, -RectTall(dresser) + 12);
	Mac_Line(hdcDest, -9, -9);
	Mac_Line(hdcDest, -RectWide(dresser) + 11, 0);
	Mac_LineTo(hdcDest, dresser->left + 10, dresser->bottom + 9);
	EndPath(hdcDest);
	DitherShadowPath(hdcDest);

	tempRect = *dresser;
	QInsetRect(&tempRect, 2, 2);
	ColorRect(hdcDest, &tempRect, k8PumpkinColor);
	HiliteRect(hdcDest, &tempRect, k8OrangeColor, dkstRedC);

	tempRect = *dresser;
	tempRect.bottom = tempRect.top + kDresserTopThick;
	ColorRect(hdcDest, &tempRect, k8PissYellowColor);
	HiliteRect(hdcDest, &tempRect, ltTanC, dkstRedC);
	ColorLine(hdcDest, dresser->left + 2, dresser->top + kDresserTopThick,
			dresser->right - 3, dresser->top + kDresserTopThick, k8Red4Color);

	ColorLine(hdcDest, dresser->left + kDresserCrease, dresser->top + kDresserTopThick + 1,
			dresser->left + kDresserCrease, dresser->bottom - 4, k8Red4Color);
	ColorLine(hdcDest, dresser->right - kDresserCrease, dresser->top + kDresserTopThick + 1,
			dresser->right - kDresserCrease, dresser->bottom - 4, k8OrangeColor);

	nRects = RectTall(dresser) / 30;
	if (nRects == 0)
		nRects = 1;
	height = (RectTall(dresser) - 14) / nRects - 4;
	QSetRect(&tempRect, 0, 0, RectWide(dresser) - kDresserSideSpare, height);
	QOffsetRect(&tempRect, dresser->left + 7, dresser->top + 10);
	for (i = 0; i < nRects; i++)
	{
		ColorLine(hdcDest, tempRect.left + 1, tempRect.bottom,
				tempRect.right, tempRect.bottom, dkstRedC);
		ColorLine(hdcDest, tempRect.right, tempRect.top + 1,
				tempRect.right, tempRect.bottom, dkstRedC);
		ColorRect(hdcDest, &tempRect, yellowC);
		HiliteRect(hdcDest, &tempRect, ltTanC, brownC);
		QInsetRect(&tempRect, 1, 1);
		HiliteRect(hdcDest, &tempRect, ltTanC, brownC);
		QInsetRect(&tempRect, -1, -1);

		QSetRect(&dest, -4, -4, 4, 4);
		QOffsetRect(&dest, HalfRectTall(&tempRect), HalfRectTall(&tempRect));
		QOffsetRect(&dest, tempRect.left, tempRect.top);
		Mac_CopyBits(g_furnitureSrcMap, hdcDest,
				&g_knobSrc, &dest, srcCopy, nil);

		QSetRect(&dest, -4, -4, 4, 4);
		QOffsetRect(&dest, -HalfRectTall(&tempRect), HalfRectTall(&tempRect));
		QOffsetRect(&dest, tempRect.right, tempRect.top);
		Mac_CopyBits(g_furnitureSrcMap, hdcDest,
				&g_knobSrc, &dest, srcCopy, nil);

		QOffsetRect(&tempRect, 0, kDresserTopThick + height);
	}

	dest = g_leftFootSrc;
	ZeroRectCorner(&dest);
	QOffsetRect(&dest, dresser->left + 6, dresser->bottom - 2);

	Mac_CopyMask(g_furnitureSrcMap, g_furnitureMaskMap, hdcDest,
			&g_leftFootSrc, &g_leftFootSrc, &dest);

	dest = g_rightFootSrc;
	ZeroRectCorner(&dest);
	QOffsetRect(&dest, dresser->right - 19, dresser->bottom - 2);

	Mac_CopyMask(g_furnitureSrcMap, g_furnitureMaskMap, hdcDest,
			&g_rightFootSrc, &g_rightFootSrc, &dest);
}

//--------------------------------------------------------------  DrawDeckTable

void DrawDeckTable (HDC hdcDest, const Rect *tableTop, SInt16 down)
{
	#define kTableBaseTop  296
	#define kTableShadowTop  312
	#define kTableShadowOffset  12
	Rect tempRect;
	SInt32 bambooC, brownC, dkGrayC;
	SInt16 hCenter, vShadow;

	bambooC = k8BambooColor;
	brownC = k8BrownColor;
	dkGrayC = k8DkstGrayColor;

	QSetRect(&tempRect, tableTop->left, 0, tableTop->right,
			RectWide(tableTop) / 10);
	QOffsetRect(&tempRect, 0,
			-HalfRectTall(&tempRect) + kTableShadowTop + down);
	QOffsetRect(&tempRect, kTableShadowOffset, -kTableShadowOffset);
	DitherShadowOval(hdcDest, &tempRect);

	tempRect = *tableTop;
	QInsetRect(&tempRect, 0, 1);
	ColorRect(hdcDest, &tempRect, kGoldColor);

	ColorLine(hdcDest, tableTop->left, tableTop->top + 1,
			tableTop->left, tableTop->top + 1, k8WhiteColor);
	ColorLine(hdcDest, tableTop->left + 1, tableTop->top,
			tableTop->right - 2, tableTop->top, k8WhiteColor);
	ColorLine(hdcDest, tableTop->right - 1, tableTop->top + 1,
			tableTop->right - 1, tableTop->top + 1, k8WhiteColor);

	ColorLine(hdcDest, tableTop->left + 1, tableTop->top + 1,
			tableTop->right - 2, tableTop->top + 1, kYellowColor);
	ColorLine(hdcDest, tableTop->left, tableTop->top + 2,
			tableTop->left, tableTop->bottom - 2, kYellowColor);

	ColorLine(hdcDest, tableTop->left + 1, tableTop->bottom - 1,
			tableTop->right - 2, tableTop->bottom - 1, brownC);
	ColorLine(hdcDest, tableTop->right - 1, tableTop->top + 2,
			tableTop->right - 1, tableTop->bottom - 2, brownC);

	ColorLine(hdcDest, tableTop->left + 1, tableTop->bottom - 2,
			tableTop->right - 2, tableTop->bottom - 2, bambooC);

	if (tableTop->bottom < kTableBaseTop + down)
	{
		hCenter = (tableTop->left + tableTop->right) / 2;

		ColorLine(hdcDest, hCenter - 3, tableTop->bottom,
				hCenter - 3, kTableBaseTop + down, dkGrayC);
		ColorLine(hdcDest, hCenter - 2, tableTop->bottom,
				hCenter - 2, kTableBaseTop + down, k8WhiteColor);
		ColorLine(hdcDest, hCenter - 1, tableTop->bottom,
				hCenter - 1, kTableBaseTop + down, k8WhiteColor);
		ColorLine(hdcDest, hCenter, tableTop->bottom,
				hCenter, kTableBaseTop + down, k8LtGrayColor);
		ColorLine(hdcDest, hCenter + 1, tableTop->bottom,
				hCenter + 1, kTableBaseTop + down, dkGrayC);

		vShadow = tableTop->bottom + RectWide(tableTop) / 4 - 2;
		if (vShadow > kTableBaseTop + down)
		{
			ColorLine(hdcDest, hCenter - 2, tableTop->bottom,
					hCenter - 2, kTableBaseTop + down, k8LtGrayColor);
			ColorLine(hdcDest, hCenter - 1, tableTop->bottom,
					hCenter - 1, kTableBaseTop + down, k8LtGrayColor);
			ColorLine(hdcDest, hCenter, tableTop->bottom,
					hCenter, kTableBaseTop + down, dkGrayC);
		}
		else
		{
			ColorLine(hdcDest, hCenter - 2, tableTop->bottom,
					hCenter - 2, vShadow, k8LtGrayColor);
			ColorLine(hdcDest, hCenter - 1, tableTop->bottom,
					hCenter - 1, vShadow + 1, k8LtGrayColor);
			ColorLine(hdcDest, hCenter, tableTop->bottom,
					hCenter, vShadow + 2, dkGrayC);
		}
	}

	tempRect = g_deckSrc;
	ZeroRectCorner(&tempRect);
	QOffsetRect(&tempRect, -HalfRectWide(&g_deckSrc) + tableTop->left +
			HalfRectWide(tableTop), kTableBaseTop + down);
	Mac_CopyMask(g_furnitureSrcMap, g_furnitureMaskMap, hdcDest,
			&g_deckSrc, &g_deckSrc, &tempRect);
}

//--------------------------------------------------------------  DrawStool

void DrawStool (HDC hdcDest, const Rect *theRect, SInt16 down)
{
	#define kStoolBase  304
	SInt32 grayC, dkGrayC;

	grayC = k8DkGray2Color;
	dkGrayC = k8DkstGrayColor;

	if (theRect->bottom < kStoolBase + down)
	{
		ColorLine(hdcDest, theRect->left + 21, theRect->bottom - 1,
				theRect->left + 21, kStoolBase + down - 1, k8DkGrayColor);
		ColorLine(hdcDest, theRect->left + 22, theRect->bottom - 1,
				theRect->left + 22, kStoolBase + down, k8Gray2Color);
		ColorLine(hdcDest, theRect->left + 23, theRect->bottom - 1,
				theRect->left + 23, kStoolBase + down, k8DkGrayColor);
		ColorLine(hdcDest, theRect->left + 24, theRect->bottom - 1,
				theRect->left + 24, kStoolBase + down, k8DkGray3Color);
		ColorLine(hdcDest, theRect->left + 25, theRect->bottom - 1,
				theRect->left + 25, kStoolBase + down, grayC);
		ColorLine(hdcDest, theRect->left + 26, theRect->bottom - 1,
				theRect->left + 26, kStoolBase + down - 1, dkGrayC);
	}

	Mac_CopyMask(g_furnitureSrcMap, g_furnitureMaskMap, hdcDest,
			&g_srcRects[kStool], &g_srcRects[kStool], theRect);
}

//--------------------------------------------------------------  DrawInvisObstacle

void DrawInvisObstacle (HDC hdcDest, const Rect *theRect)
{
	ColorFrameRect(hdcDest, theRect, k8BrownColor);
}

//--------------------------------------------------------------  DrawInvisBounce

void DrawInvisBounce (HDC hdcDest, const Rect *theRect)
{
	ColorFrameRect(hdcDest, theRect, k8RedColor);
}

//--------------------------------------------------------------  DrawRedClock

void DrawRedClock (HDC hdcDest, const Rect *theRect)
{
	SYSTEMTIME localTime;
	Rect dest;
	SInt16 hour, minutes;

	Mac_CopyMask(g_bonusSrcMap, g_bonusMaskMap, hdcDest,
			&g_srcRects[kRedClock], &g_srcRects[kRedClock], theRect);

	GetLocalTime(&localTime);
	hour = localTime.wHour % 12;
	if (hour == 0)
		hour = 12;
	minutes = localTime.wMinute;

	QSetRect(&dest, 0, 0, 4, 6);
	QOffsetRect(&dest, theRect->left + 5, theRect->top + 7);
	if (hour > 9)
		DrawClockDigit(hdcDest, hour / 10, &dest);
	QOffsetRect(&dest, 4, 0);
	DrawClockDigit(hdcDest, hour % 10, &dest);
	QOffsetRect(&dest, 6, 0);
	DrawClockDigit(hdcDest, minutes / 10, &dest);
	QOffsetRect(&dest, 4, 0);
	DrawClockDigit(hdcDest, minutes % 10, &dest);
}

//--------------------------------------------------------------  DrawClockDigit

void DrawClockDigit (HDC hdcDest, SInt16 number, const Rect *dest)
{
	if (number < 0 || number >= ARRAYSIZE(g_digits))
		return;

	Mac_CopyBits(g_bonusSrcMap, hdcDest,
			&g_digits[number], dest, srcCopy, nil);
}

//--------------------------------------------------------------  DrawBlueClock

void DrawBlueClock (HDC hdcDest, const Rect *theRect)
{
	SYSTEMTIME localTime;
	Point dest;
	SInt16 hour, minutes;

	Mac_CopyMask(g_bonusSrcMap, g_bonusMaskMap, hdcDest,
			&g_srcRects[kBlueClock], &g_srcRects[kBlueClock], theRect);

	dest.h = theRect->left + 13;
	dest.v = theRect->top + 13;
	GetLocalTime(&localTime);
	hour = localTime.wHour % 12;
	minutes = ((localTime.wMinute + 2) / 5) % 12;
	DrawClockHands(hdcDest, dest, minutes, hour);
}

//--------------------------------------------------------------  DrawYellowClock

void DrawYellowClock (HDC hdcDest, const Rect *theRect)
{
	SYSTEMTIME localTime;
	Point dest;
	SInt16 hour, minutes;

	Mac_CopyMask(g_bonusSrcMap, g_bonusMaskMap, hdcDest,
			&g_srcRects[kYellowClock], &g_srcRects[kYellowClock], theRect);

	dest.h = theRect->left + 13;
	dest.v = theRect->top + 15;
	GetLocalTime(&localTime);
	hour = localTime.wHour % 12;
	minutes = ((localTime.wMinute + 2) / 5) % 12;
	DrawClockHands(hdcDest, dest, minutes, hour);
}

//--------------------------------------------------------------  DrawCuckoo

void DrawCuckoo (HDC hdcDest, const Rect *theRect)
{
	SYSTEMTIME localTime;
	Point dest;
	SInt16 hour, minutes;

	Mac_CopyMask(g_bonusSrcMap, g_bonusMaskMap, hdcDest,
			&g_srcRects[kCuckoo], &g_srcRects[kCuckoo], theRect);

	dest.h = theRect->left + 19;
	dest.v = theRect->top + 31;
	GetLocalTime(&localTime);
	hour = localTime.wHour % 12;
	minutes = ((localTime.wMinute + 2) / 5) % 12;
	DrawLargeClockHands(hdcDest, dest, minutes, hour);
}

//--------------------------------------------------------------  DrawClockHands

void DrawClockHands (HDC hdcDest, Point where, SInt16 bigHand, SInt16 littleHand)
{
	HPEN wasPen;

	wasPen = SelectPen(hdcDest, GetStockPen(BLACK_PEN));

	MoveToEx(hdcDest, where.h, where.v, NULL);
	switch (bigHand)
	{
		case 0:
		Mac_Line(hdcDest, 0, -6);
		break;

		case 1:
		Mac_Line(hdcDest, 3, -5);
		break;

		case 2:
		Mac_Line(hdcDest, 5, -3);
		break;

		case 3:
		Mac_Line(hdcDest, 6, 0);
		break;

		case 4:
		Mac_Line(hdcDest, 5, 3);
		break;

		case 5:
		Mac_Line(hdcDest, 3, 5);
		break;

		case 6:
		Mac_Line(hdcDest, 0, 6);
		break;

		case 7:
		Mac_Line(hdcDest, -3, 5);
		break;

		case 8:
		Mac_Line(hdcDest, -5, 3);
		break;

		case 9:
		Mac_Line(hdcDest, -6, 0);
		break;

		case 10:
		Mac_Line(hdcDest, -5, -3);
		break;

		case 11:
		Mac_Line(hdcDest, -3, -5);
		break;
	}

	MoveToEx(hdcDest, where.h, where.v, NULL);
	switch (littleHand)
	{
		case 0:
		Mac_Line(hdcDest, 0, -4);
		break;

		case 1:
		Mac_Line(hdcDest, 2, -3);
		break;

		case 2:
		Mac_Line(hdcDest, 3, -2);
		break;

		case 3:
		Mac_Line(hdcDest, 4, 0);
		break;

		case 4:
		Mac_Line(hdcDest, 3, 2);
		break;

		case 5:
		Mac_Line(hdcDest, 2, 3);
		break;

		case 6:
		Mac_Line(hdcDest, 0, 4);
		break;

		case 7:
		Mac_Line(hdcDest, -2, 3);
		break;

		case 8:
		Mac_Line(hdcDest, -3, 2);
		break;

		case 9:
		Mac_Line(hdcDest, -4, 0);
		break;

		case 10:
		Mac_Line(hdcDest, -3, -2);
		break;

		case 11:
		Mac_Line(hdcDest, -2, -3);
		break;
	}

	SelectPen(hdcDest, wasPen);
}

//--------------------------------------------------------------  DrawLargeClockHands

void DrawLargeClockHands (HDC hdcDest, Point where, SInt16 bigHand, SInt16 littleHand)
{
	HPEN wasPen;

	wasPen = SelectPen(hdcDest, GetStockPen(WHITE_PEN));

	MoveToEx(hdcDest, where.h, where.v, NULL);
	switch (bigHand)
	{
		case 0:
		Mac_Line(hdcDest, 0, -10);
		break;

		case 1:
		Mac_Line(hdcDest, 5, -9);
		break;

		case 2:
		Mac_Line(hdcDest, 9, -5);
		break;

		case 3:
		Mac_Line(hdcDest, 10, 0);
		break;

		case 4:
		Mac_Line(hdcDest, 9, 5);
		break;

		case 5:
		Mac_Line(hdcDest, 5, 9);
		break;

		case 6:
		Mac_Line(hdcDest, 0, 10);
		break;

		case 7:
		Mac_Line(hdcDest, -5, 9);
		break;

		case 8:
		Mac_Line(hdcDest, -9, 5);
		break;

		case 9:
		Mac_Line(hdcDest, -10, 0);
		break;

		case 10:
		Mac_Line(hdcDest, -9, -5);
		break;

		case 11:
		Mac_Line(hdcDest, -5, -9);
		break;
	}

	MoveToEx(hdcDest, where.h, where.v, NULL);
	switch (littleHand)
	{
		case 0:
		Mac_Line(hdcDest, 0, -6);
		break;

		case 1:
		Mac_Line(hdcDest, 3, -5);
		break;

		case 2:
		Mac_Line(hdcDest, 5, -3);
		break;

		case 3:
		Mac_Line(hdcDest, 6, 0);
		break;

		case 4:
		Mac_Line(hdcDest, 5, 3);
		break;

		case 5:
		Mac_Line(hdcDest, 3, 5);
		break;

		case 6:
		Mac_Line(hdcDest, 0, 6);
		break;

		case 7:
		Mac_Line(hdcDest, -3, 5);
		break;

		case 8:
		Mac_Line(hdcDest, -5, 3);
		break;

		case 9:
		Mac_Line(hdcDest, -6, 0);
		break;

		case 10:
		Mac_Line(hdcDest, -5, -3);
		break;

		case 11:
		Mac_Line(hdcDest, -3, -5);
		break;
	}

	SelectPen(hdcDest, wasPen);
}

//--------------------------------------------------------------  DrawSimplePrizes

void DrawSimplePrizes (HDC hdcDest, SInt16 what, const Rect *theRect)
{
	if (what < 0 || what >= ARRAYSIZE(g_srcRects))
		return;

	Mac_CopyMask(g_bonusSrcMap, g_bonusMaskMap, hdcDest,
			&g_srcRects[what], &g_srcRects[what], theRect);
}

//--------------------------------------------------------------  DrawGreaseRt

void DrawGreaseRt (HDC hdcDest, const Rect *theRect, SInt16 distance, Boolean state)
{
	Rect spill, dest;

	dest = *theRect;
	if (state)  // grease upright
	{
		Mac_CopyMask(g_bonusSrcMap, g_bonusMaskMap, hdcDest,
				&g_greaseSrcRt[0], &g_greaseSrcRt[0], &dest);
	}
	else  // grease spilled
	{
		QOffsetRect(&dest, 6, 0);
		Mac_CopyMask(g_bonusSrcMap, g_bonusMaskMap, hdcDest,
				&g_greaseSrcRt[3], &g_greaseSrcRt[3], &dest);

		QSetRect(&spill, 0, -2, distance - 5, 0);
		QOffsetRect(&spill, dest.right - 1, dest.bottom);
		Mac_PaintRect(hdcDest, &spill, GetStockBrush(BLACK_BRUSH));
	}
}

//--------------------------------------------------------------  DrawGreaseLf

void DrawGreaseLf (HDC hdcDest, const Rect *theRect, SInt16 distance, Boolean state)
{
	Rect spill, dest;

	dest = *theRect;
	if (state)  // grease upright
	{
		Mac_CopyMask(g_bonusSrcMap, g_bonusMaskMap, hdcDest,
				&g_greaseSrcLf[0], &g_greaseSrcLf[0], &dest);
	}
	else  // grease spilled
	{
		QOffsetRect(&dest, -6, 0);
		Mac_CopyMask(g_bonusSrcMap, g_bonusMaskMap, hdcDest,
				&g_greaseSrcLf[3], &g_greaseSrcLf[3], &dest);

		QSetRect(&spill, -distance + 5, -2, 0, 0);
		QOffsetRect(&spill, dest.left + 1, dest.bottom);
		Mac_PaintRect(hdcDest, &spill, GetStockBrush(BLACK_BRUSH));
	}
}

//--------------------------------------------------------------  DrawBands

void DrawFoil (HDC hdcDest, const Rect *theRect)
{
	Mac_CopyMask(g_bonusSrcMap, g_bonusMaskMap, hdcDest,
			&g_srcRects[kFoil], &g_srcRects[kFoil], theRect);
}

//--------------------------------------------------------------  DrawInvisBonus

void DrawInvisBonus (HDC hdcDest, const Rect *theRect)
{
	ColorFrameOval(hdcDest, theRect, 227);
}

//--------------------------------------------------------------  DrawSlider

void DrawSlider (HDC hdcDest, const Rect *theRect)
{
	Mac_FrameRect(hdcDest, theRect, GetStockBrush(BLACK_BRUSH), 1, 1);
}

//--------------------------------------------------------------  DrawMailboxLeft

void DrawMailboxLeft (HDC hdcDest, const Rect *theRect, SInt16 down)
{
	Rect bounds;
	HDC tempMap;
	HDC tempMask;
	SInt32 darkGrayC, lightWoodC, darkWoodC;

	darkGrayC = k8DkGray2Color;
	lightWoodC = k8PissYellowColor;
	darkWoodC = k8BrownColor;

	if (theRect->bottom < down + kMailboxBase)
	{
		ColorLine(hdcDest, theRect->left + 49, theRect->bottom,
				theRect->left + 49, down + kMailboxBase, darkGrayC);
		ColorLine(hdcDest, theRect->left + 50, theRect->bottom,
				theRect->left + 50, down + kMailboxBase + 1, lightWoodC);
		ColorLine(hdcDest, theRect->left + 51, theRect->bottom,
				theRect->left + 51, down + kMailboxBase + 2, lightWoodC);
		ColorLine(hdcDest, theRect->left + 52, theRect->bottom,
				theRect->left + 52, down + kMailboxBase + 3, lightWoodC);
		ColorLine(hdcDest, theRect->left + 53, theRect->bottom,
				theRect->left + 53, down + kMailboxBase + 3, darkWoodC);
		ColorLine(hdcDest, theRect->left + 54, theRect->bottom,
				theRect->left + 54, down + kMailboxBase + 3, darkWoodC);
		ColorLine(hdcDest, theRect->left + 55, theRect->bottom,
				theRect->left + 55, down + kMailboxBase + 3, darkWoodC);
		ColorLine(hdcDest, theRect->left + 56, theRect->bottom,
				theRect->left + 56, down + kMailboxBase + 3, darkWoodC);
		ColorLine(hdcDest, theRect->left + 57, theRect->bottom,
				theRect->left + 57, down + kMailboxBase + 3, darkWoodC);
		ColorLine(hdcDest, theRect->left + 58, theRect->bottom,
				theRect->left + 58, down + kMailboxBase + 3, darkWoodC);
		ColorLine(hdcDest, theRect->left + 59, theRect->bottom,
				theRect->left + 59, down + kMailboxBase + 3, darkWoodC);
		ColorLine(hdcDest, theRect->left + 60, theRect->bottom,
				theRect->left + 60, down + kMailboxBase + 3, darkWoodC);
		ColorLine(hdcDest, theRect->left + 61, theRect->bottom,
				theRect->left + 61, down + kMailboxBase + 3, darkWoodC);
		ColorLine(hdcDest, theRect->left + 62, theRect->bottom,
				theRect->left + 62, down + kMailboxBase + 3, darkGrayC);
	}

	bounds = g_srcRects[kMailboxLf];
	tempMap = CreateOffScreenGWorld(&bounds, kPreferredDepth);
	LoadGraphic(tempMap, g_theHouseFile, kMailboxLeftPictID);

	tempMask = CreateOffScreenGWorld(&bounds, 1);
	LoadGraphic(tempMask, g_theHouseFile, kMailboxLeftMaskID);

	Mac_CopyMask(tempMap, tempMask, hdcDest,
			&g_srcRects[kMailboxLf], &g_srcRects[kMailboxLf], theRect);

	DisposeGWorld(tempMap);
	DisposeGWorld(tempMask);
}

//--------------------------------------------------------------  DrawMailboxRight

void DrawMailboxRight (HDC hdcDest, const Rect *theRect, SInt16 down)
{
	Rect bounds;
	HDC tempMap;
	HDC tempMask;
	SInt32 darkGrayC, lightWoodC, darkWoodC;

	darkGrayC = k8DkGray2Color;
	lightWoodC = k8PissYellowColor;
	darkWoodC = k8BrownColor;

	if (theRect->bottom < down + kMailboxBase)
	{
		ColorLine(hdcDest, theRect->left + 34, theRect->bottom,
				theRect->left + 34, down + kMailboxBase, darkGrayC);
		ColorLine(hdcDest, theRect->left + 35, theRect->bottom,
				theRect->left + 35, down + kMailboxBase + 1, lightWoodC);
		ColorLine(hdcDest, theRect->left + 36, theRect->bottom,
				theRect->left + 36, down + kMailboxBase + 2, lightWoodC);
		ColorLine(hdcDest, theRect->left + 37, theRect->bottom,
				theRect->left + 37, down + kMailboxBase + 3, lightWoodC);
		ColorLine(hdcDest, theRect->left + 38, theRect->bottom,
				theRect->left + 38, down + kMailboxBase + 3, darkWoodC);
		ColorLine(hdcDest, theRect->left + 39, theRect->bottom,
				theRect->left + 39, down + kMailboxBase + 3, darkWoodC);
		ColorLine(hdcDest, theRect->left + 40, theRect->bottom,
				theRect->left + 40, down + kMailboxBase + 3, darkWoodC);
		ColorLine(hdcDest, theRect->left + 41, theRect->bottom,
				theRect->left + 41, down + kMailboxBase + 3, darkWoodC);
		ColorLine(hdcDest, theRect->left + 42, theRect->bottom,
				theRect->left + 42, down + kMailboxBase + 3, darkWoodC);
		ColorLine(hdcDest, theRect->left + 43, theRect->bottom,
				theRect->left + 43, down + kMailboxBase + 3, darkWoodC);
		ColorLine(hdcDest, theRect->left + 44, theRect->bottom,
				theRect->left + 44, down + kMailboxBase + 3, darkWoodC);
		ColorLine(hdcDest, theRect->left + 45, theRect->bottom,
				theRect->left + 45, down + kMailboxBase + 3, darkWoodC);
		ColorLine(hdcDest, theRect->left + 46, theRect->bottom,
				theRect->left + 46, down + kMailboxBase + 3, darkWoodC);
		ColorLine(hdcDest, theRect->left + 47, theRect->bottom,
				theRect->left + 47, down + kMailboxBase + 3, darkGrayC);
	}

	bounds = g_srcRects[kMailboxRt];
	tempMap = CreateOffScreenGWorld(&bounds, kPreferredDepth);
	LoadGraphic(tempMap, g_theHouseFile, kMailboxRightPictID);

	tempMask = CreateOffScreenGWorld(&bounds, 1);
	LoadGraphic(tempMask, g_theHouseFile, kMailboxRightMaskID);

	Mac_CopyMask(tempMap, tempMask, hdcDest,
			&g_srcRects[kMailboxRt], &g_srcRects[kMailboxRt], theRect);

	DisposeGWorld(tempMap);
	DisposeGWorld(tempMask);
}

//--------------------------------------------------------------  DrawSimpleTransport

void DrawSimpleTransport (HDC hdcDest, SInt16 what, const Rect *theRect)
{
	if (what < 0 || what >= ARRAYSIZE(g_srcRects))
		return;

	Mac_CopyMask(g_transSrcMap, g_transMaskMap, hdcDest,
			&g_srcRects[what], &g_srcRects[what], theRect);
}

//--------------------------------------------------------------  DrawInvisTransport

void DrawInvisTransport (HDC hdcDest, const Rect *theRect)
{
	ColorFrameRect(hdcDest, theRect, 32);
}

//--------------------------------------------------------------  DrawLightSwitch

void DrawLightSwitch (HDC hdcDest, const Rect *theRect, Boolean state)
{
	if (state)
	{
		Mac_CopyBits(g_switchSrcMap, hdcDest,
				&g_lightSwitchSrc[0], theRect, srcCopy, nil);
	}
	else
	{
		Mac_CopyBits(g_switchSrcMap, hdcDest,
				&g_lightSwitchSrc[1], theRect, srcCopy, nil);
	}
}

//--------------------------------------------------------------  DrawMachineSwitch

void DrawMachineSwitch (HDC hdcDest, const Rect *theRect, Boolean state)
{
	if (state)
		Mac_CopyBits(g_switchSrcMap, hdcDest,
				&g_machineSwitchSrc[0], theRect, srcCopy, nil);
	else
		Mac_CopyBits(g_switchSrcMap, hdcDest,
				&g_machineSwitchSrc[1], theRect, srcCopy, nil);
}

//--------------------------------------------------------------  DrawThermostat

void DrawThermostat (HDC hdcDest, const Rect *theRect, Boolean state)
{
	if (state)
		Mac_CopyBits(g_switchSrcMap, hdcDest,
				&g_thermostatSrc[0], theRect, srcCopy, nil);
	else
		Mac_CopyBits(g_switchSrcMap, hdcDest,
				&g_thermostatSrc[1], theRect, srcCopy, nil);
}

//--------------------------------------------------------------  DrawPowerSwitch

void DrawPowerSwitch (HDC hdcDest, const Rect *theRect, Boolean state)
{
	if (state)
		Mac_CopyBits(g_switchSrcMap, hdcDest,
				&g_powerSrc[0], theRect, srcCopy, nil);
	else
		Mac_CopyBits(g_switchSrcMap, hdcDest,
				&g_powerSrc[1], theRect, srcCopy, nil);
}

//--------------------------------------------------------------  DrawKnifeSwitch

void DrawKnifeSwitch (HDC hdcDest, const Rect *theRect, Boolean state)
{
	if (state)
		Mac_CopyBits(g_switchSrcMap, hdcDest,
				&g_knifeSwitchSrc[0], theRect, srcCopy, nil);
	else
		Mac_CopyBits(g_switchSrcMap, hdcDest,
				&g_knifeSwitchSrc[1], theRect, srcCopy, nil);
}

//--------------------------------------------------------------  DrawInvisibleSwitch

void DrawInvisibleSwitch (HDC hdcDest, const Rect *theRect)
{
	ColorFrameRect(hdcDest, theRect, kIntenseGreenColor);
}

//--------------------------------------------------------------  DrawTrigger

void DrawTrigger (HDC hdcDest, const Rect *theRect)
{
	ColorFrameRect(hdcDest, theRect, kIntenseBlueColor);
}

//--------------------------------------------------------------  DrawSoundTrigger

void DrawSoundTrigger (HDC hdcDest, const Rect *theRect)
{
	ColorFrameRect(hdcDest, theRect, kIntenseYellowColor);
}

//--------------------------------------------------------------  DrawSimpleLight

void DrawSimpleLight (HDC hdcDest, SInt16 what, const Rect *theRect)
{
	if (what < 0 || what >= ARRAYSIZE(g_srcRects))
		return;

	Mac_CopyMask(g_lightSrcMap, g_lightMaskMap, hdcDest,
			&g_srcRects[what], &g_srcRects[what], theRect);
}

//--------------------------------------------------------------  DrawFlourescent

void DrawFlourescent (HDC hdcDest, const Rect *theRect)
{
	Rect partRect;
	SInt32 grayC, gray2C, gray3C, gray4C, violetC;

	grayC = k8LtGrayColor;
	gray2C = k8LtstGray5Color;
	gray3C = k8LtstGray4Color;
	gray4C = k8LtstGrayColor;
	violetC = kPaleVioletColor;

	ColorLine(hdcDest, theRect->left + 16, theRect->top,
			theRect->right - 17, theRect->top, grayC);
	ColorLine(hdcDest, theRect->left + 16, theRect->top + 1,
			theRect->right - 17, theRect->top + 1, gray2C);
	ColorLine(hdcDest, theRect->left + 16, theRect->top + 2,
			theRect->right - 17, theRect->top + 2, gray2C);
	ColorLine(hdcDest, theRect->left + 16, theRect->top + 3,
			theRect->right - 17, theRect->top + 3, gray3C);
	ColorLine(hdcDest, theRect->left + 16, theRect->top + 4,
			theRect->right - 17, theRect->top + 4, gray4C);
	ColorLine(hdcDest, theRect->left + 16, theRect->top + 5,
			theRect->right - 17, theRect->top + 5, violetC);
	ColorLine(hdcDest, theRect->left + 16, theRect->top + 6,
			theRect->right - 17, theRect->top + 6, k8WhiteColor);
	ColorLine(hdcDest, theRect->left + 16, theRect->top + 7,
			theRect->right - 17, theRect->top + 7, k8WhiteColor);
	ColorLine(hdcDest, theRect->left + 16, theRect->top + 8,
			theRect->right - 17, theRect->top + 8, k8WhiteColor);
	ColorLine(hdcDest, theRect->left + 16, theRect->top + 9,
			theRect->right - 17, theRect->top + 9, k8WhiteColor);
	ColorLine(hdcDest, theRect->left + 16, theRect->top + 10,
			theRect->right - 17, theRect->top + 10, k8WhiteColor);
	ColorLine(hdcDest, theRect->left + 16, theRect->top + 11,
			theRect->right - 17, theRect->top + 11, violetC);

	partRect = g_flourescentSrc1;
	ZeroRectCorner(&partRect);
	QOffsetRect(&partRect, theRect->left, theRect->top);

	Mac_CopyMask(g_lightSrcMap, g_lightMaskMap, hdcDest,
			&g_flourescentSrc1, &g_flourescentSrc1, &partRect);

	partRect = g_flourescentSrc2;
	ZeroRectCorner(&partRect);
	QOffsetRect(&partRect, -partRect.right, 0);
	QOffsetRect(&partRect, theRect->right, theRect->top);

	Mac_CopyMask(g_lightSrcMap, g_lightMaskMap, hdcDest,
			&g_flourescentSrc2, &g_flourescentSrc2, &partRect);
}

//--------------------------------------------------------------  DrawTrackLight

void DrawTrackLight (HDC hdcDest, const Rect *theRect)
{
	#define kTrackLightSpacing  64
	Rect partRect;
	SInt32 grayC, gray2C, gray3C, gray4C;
	SInt16 which, howMany, i, spread;

	grayC = k8LtGrayColor;
	gray2C = k8Gray2Color;
	gray3C = k8LtstGray4Color;
	gray4C = k8DkGrayColor;

	ColorLine(hdcDest, theRect->left, theRect->top - 3,
			theRect->right - 1, theRect->top - 3, gray2C);
	ColorLine(hdcDest, theRect->left, theRect->top - 2,
			theRect->right - 1, theRect->top - 2, grayC);
	ColorLine(hdcDest, theRect->left, theRect->top - 1,
			theRect->right - 1, theRect->top - 1, grayC);
	ColorLine(hdcDest, theRect->left, theRect->top,
			theRect->right - 1, theRect->top, gray3C);
	ColorLine(hdcDest, theRect->left, theRect->top + 1,
			theRect->right - 1, theRect->top + 1, gray4C);
	ColorLine(hdcDest, theRect->left, theRect->top + 2,
			theRect->right - 1, theRect->top + 2, gray3C);

	// left most track light
	partRect = g_trackLightSrc[0];
	ZeroRectCorner(&partRect);
	QOffsetRect(&partRect, theRect->left, theRect->top);
	which = 0;
	Mac_CopyMask(g_lightSrcMap, g_lightMaskMap, hdcDest,
			&g_trackLightSrc[which], &g_trackLightSrc[which], &partRect);

	// right most track light
	partRect = g_trackLightSrc[0];
	ZeroRectCorner(&partRect);
	QOffsetRect(&partRect, -partRect.right, 0);
	QOffsetRect(&partRect, theRect->right, theRect->top);
	which = 2;
	Mac_CopyMask(g_lightSrcMap, g_lightMaskMap, hdcDest,
			&g_trackLightSrc[which], &g_trackLightSrc[which], &partRect);

	howMany = ((RectWide(theRect) - RectWide(&g_trackLightSrc[0])) /
			kTrackLightSpacing) - 1;
	if (howMany > 0)
	{
		which = 0;
		spread = (RectWide(theRect) - RectWide(&g_trackLightSrc[0])) / (howMany + 1);
		for (i = 0; i < howMany; i++)
		{
			// filler track lights
			partRect = g_trackLightSrc[0];
			ZeroRectCorner(&partRect);
			QOffsetRect(&partRect, theRect->left, theRect->top);
			QOffsetRect(&partRect, spread * (i + 1), 0);
			which++;
			if (which >= kNumTrackLights)
				which = 0;
			Mac_CopyMask(g_lightSrcMap, g_lightMaskMap, hdcDest,
					&g_trackLightSrc[which], &g_trackLightSrc[which], &partRect);
		}
	}
}

//--------------------------------------------------------------  DrawInvisLight

void DrawInvisLight (HDC hdcDest, const Rect *theRect)
{
	ColorFrameOval(hdcDest, theRect, 17);
}

//--------------------------------------------------------------  DrawSimpleAppliance

void DrawSimpleAppliance (HDC hdcDest, SInt16 what, const Rect *theRect)
{
	if (what < 0 || what >= ARRAYSIZE(g_srcRects))
		return;

	Mac_CopyMask(g_applianceSrcMap, g_applianceMaskMap, hdcDest,
			&g_srcRects[what], &g_srcRects[what], theRect);
}

//--------------------------------------------------------------  DrawMacPlus

void DrawMacPlus (HDC hdcDest, const Rect *theRect, Boolean isOn, Boolean isLit)
{
	Rect screen;

	if (isLit)
	{
		Mac_CopyMask(g_applianceSrcMap, g_applianceMaskMap, hdcDest,
				&g_srcRects[kMacPlus], &g_srcRects[kMacPlus], theRect);
	}

	screen = g_plusScreen1;
	ZeroRectCorner(&screen);
	QOffsetRect(&screen, theRect->left + 10, theRect->top + 7);
	if (isOn)
	{
		Mac_CopyBits(g_applianceSrcMap, hdcDest,
				&g_plusScreen2, &screen, srcCopy, nil);
	}
	else
	{
		Mac_CopyBits(g_applianceSrcMap, hdcDest,
				&g_plusScreen1, &screen, srcCopy, nil);
	}
}

//--------------------------------------------------------------  DrawTV

void DrawTV (HDC hdcDest, const Rect *theRect, Boolean isOn, Boolean isLit)
{
	Rect bounds;
	HDC tempMap;
	HDC tempMask;

	if (isLit)
	{
		bounds = g_srcRects[kTV];
		tempMap = CreateOffScreenGWorld(&bounds, kPreferredDepth);
		LoadGraphic(tempMap, g_theHouseFile, kTVPictID);

		tempMask = CreateOffScreenGWorld(&bounds, 1);
		LoadGraphic(tempMask, g_theHouseFile, kTVMaskID);

		Mac_CopyMask(tempMap, tempMask, hdcDest,
				&g_srcRects[kTV], &g_srcRects[kTV], theRect);

		DisposeGWorld(tempMap);
		DisposeGWorld(tempMask);
	}

	bounds = g_tvScreen1;
	ZeroRectCorner(&bounds);
	QOffsetRect(&bounds, theRect->left + 17, theRect->top + 10);
	if (isOn)
	{
		Mac_CopyBits(g_applianceSrcMap, hdcDest,
				&g_tvScreen2, &bounds, srcCopy, nil);
	}
	else
	{
		Mac_CopyBits(g_applianceSrcMap, hdcDest,
				&g_tvScreen1, &bounds, srcCopy, nil);
	}
}

//--------------------------------------------------------------  DrawCoffee

void DrawCoffee (HDC hdcDest, const Rect *theRect, Boolean isOn, Boolean isLit)
{
	Rect light;

	if (isLit)
	{
		Mac_CopyMask(g_applianceSrcMap, g_applianceMaskMap, hdcDest,
				&g_srcRects[kCoffee], &g_srcRects[kCoffee], theRect);
	}

	light = g_coffeeLight1;
	ZeroRectCorner(&light);
	QOffsetRect(&light, theRect->left + 32, theRect->top + 57);
	if (isOn)
	{
		Mac_CopyBits(g_applianceSrcMap, hdcDest,
				&g_coffeeLight2, &light, srcCopy, nil);
	}
	else
	{
		Mac_CopyBits(g_applianceSrcMap, hdcDest,
				&g_coffeeLight1, &light, srcCopy, nil);
	}
}

//--------------------------------------------------------------  DrawOutlet

void DrawOutlet (HDC hdcDest, const Rect *theRect)
{
	Mac_CopyMask(g_applianceSrcMap, g_applianceMaskMap, hdcDest,
			&g_srcRects[kOutlet], &g_srcRects[kOutlet], theRect);
}

//--------------------------------------------------------------  DrawVCR

void DrawVCR (HDC hdcDest, const Rect *theRect, Boolean isOn, Boolean isLit)
{
	Rect bounds;
	HDC tempMap;
	HDC tempMask;

	if (isLit)
	{
		bounds = g_srcRects[kVCR];
		tempMap = CreateOffScreenGWorld(&bounds, kPreferredDepth);
		LoadGraphic(tempMap, g_theHouseFile, kVCRPictID);

		tempMask = CreateOffScreenGWorld(&bounds, 1);
		LoadGraphic(tempMask, g_theHouseFile, kVCRMaskID);

		Mac_CopyMask(tempMap, tempMask, hdcDest,
				&g_srcRects[kVCR], &g_srcRects[kVCR], theRect);

		DisposeGWorld(tempMap);
		DisposeGWorld(tempMask);
	}

	bounds = g_vcrTime1;
	ZeroRectCorner(&bounds);
	QOffsetRect(&bounds, theRect->left + 64, theRect->top + 6);
	if (isOn)
	{
		Mac_CopyBits(g_applianceSrcMap, hdcDest,
				&g_vcrTime2, &bounds, srcCopy, nil);
	}
	else
	{
		Mac_CopyBits(g_applianceSrcMap, hdcDest,
				&g_vcrTime1, &bounds, srcCopy, nil);
	}
}

//--------------------------------------------------------------  DrawStereo

void DrawStereo (HDC hdcDest, const Rect *theRect, Boolean isOn, Boolean isLit)
{
	Rect bounds;
	HDC tempMap;
	HDC tempMask;

	if (isLit)
	{
		bounds = g_srcRects[kStereo];
		tempMap = CreateOffScreenGWorld(&bounds, kPreferredDepth); 
		LoadGraphic(tempMap, g_theHouseFile, kStereoPictID);

		tempMask = CreateOffScreenGWorld(&bounds, 1);
		LoadGraphic(tempMask, g_theHouseFile, kStereoMaskID);

		Mac_CopyMask(tempMap, tempMask, hdcDest,
				&g_srcRects[kStereo], &g_srcRects[kStereo], theRect);

		DisposeGWorld(tempMap);
		DisposeGWorld(tempMask);
	}

	bounds = g_stereoLight1;
	ZeroRectCorner(&bounds);
	QOffsetRect(&bounds, theRect->left + 56, theRect->top + 20);
	if (isOn)
	{
		Mac_CopyBits(g_applianceSrcMap, hdcDest,
				&g_stereoLight2, &bounds, srcCopy, nil);
	}
	else
	{
		Mac_CopyBits(g_applianceSrcMap, hdcDest,
				&g_stereoLight1, &bounds, srcCopy, nil);
	}
}

//--------------------------------------------------------------  DrawMicrowave

void DrawMicrowave (HDC hdcDest, const Rect *theRect, Boolean isOn, Boolean isLit)
{
	Rect bounds;
	HDC tempMap;
	HDC tempMask;

	if (isLit)
	{
		bounds = g_srcRects[kMicrowave];
		tempMap = CreateOffScreenGWorld(&bounds, kPreferredDepth);
		LoadGraphic(tempMap, g_theHouseFile, kMicrowavePictID);

		tempMask = CreateOffScreenGWorld(&bounds, 1);
		LoadGraphic(tempMask, g_theHouseFile, kMicrowaveMaskID);

		Mac_CopyMask(tempMap, tempMask, hdcDest,
				&g_srcRects[kMicrowave], &g_srcRects[kMicrowave], theRect);

		DisposeGWorld(tempMap);
		DisposeGWorld(tempMask);
	}

	bounds = g_microOn;
	ZeroRectCorner(&bounds);
	QOffsetRect(&bounds, theRect->left + 14, theRect->top + 13);
	if (isOn)
	{
		Mac_CopyBits(g_applianceSrcMap, hdcDest,
				&g_microOn, &bounds, srcCopy, nil);
		QOffsetRect(&bounds, 16, 0);
		Mac_CopyBits(g_applianceSrcMap, hdcDest,
				&g_microOn, &bounds, srcCopy, nil);
		QOffsetRect(&bounds, 16, 0);
		Mac_CopyBits(g_applianceSrcMap, hdcDest,
				&g_microOn, &bounds, srcCopy, nil);
	}
	else if (isLit)
	{
		Mac_CopyBits(g_applianceSrcMap, hdcDest,
				&g_microOff, &bounds, srcCopy, nil);
		QOffsetRect(&bounds, 16, 0);
		Mac_CopyBits(g_applianceSrcMap, hdcDest,
				&g_microOff, &bounds, srcCopy, nil);
		QOffsetRect(&bounds, 16, 0);
		Mac_CopyBits(g_applianceSrcMap, hdcDest,
				&g_microOff, &bounds, srcCopy, nil);
	}
}

//--------------------------------------------------------------  DrawBalloon

void DrawBalloon (HDC hdcDest, const Rect *theRect)
{
	Mac_CopyMask(g_balloonSrcMap, g_balloonMaskMap, hdcDest,
			&g_balloonSrc[1], &g_balloonSrc[1], theRect);
}

//--------------------------------------------------------------  DrawCopter

void DrawCopter (HDC hdcDest, const Rect *theRect)
{
	Mac_CopyMask(g_copterSrcMap, g_copterMaskMap, hdcDest,
			&g_copterSrc[1], &g_copterSrc[1], theRect);
}

//--------------------------------------------------------------  DrawDart

void DrawDart (HDC hdcDest, const Rect *theRect, SInt16 which)
{
	if (which == kDartLf)
	{
		Mac_CopyMask(g_dartSrcMap, g_dartMaskMap, hdcDest,
				&g_dartSrc[0], &g_dartSrc[0], theRect);
	}
	else
	{
		Mac_CopyMask(g_dartSrcMap, g_dartMaskMap, hdcDest,
				&g_dartSrc[2], &g_dartSrc[2], theRect);
	}
}

//--------------------------------------------------------------  DrawBall

void DrawBall (HDC hdcDest, SInt16 what, const Rect *theRect)
{
	if (what < 0 || what >= ARRAYSIZE(g_srcRects))
		return;

	Mac_CopyMask(g_ballSrcMap, g_ballMaskMap, hdcDest,
			&g_srcRects[what], &g_srcRects[what], theRect);
}

//--------------------------------------------------------------  DrawFish

void DrawFish (HDC hdcDest, SInt16 what, const Rect *theRect)
{
	if (what < 0 || what >= ARRAYSIZE(g_srcRects))
		return;

	Mac_CopyMask(g_enemySrcMap, g_enemyMaskMap, hdcDest,
			&g_srcRects[what], &g_srcRects[what], theRect);
}

//--------------------------------------------------------------  DrawDrip

void DrawDrip (HDC hdcDest, const Rect *theRect)
{
	Mac_CopyMask(g_dripSrcMap, g_dripMaskMap, hdcDest,
			&g_dripSrc[3], &g_dripSrc[3], theRect);
}

//--------------------------------------------------------------  DrawMirror

void DrawMirror (HDC hdcDest, const Rect *mirror)
{
	Rect tempRect;
	SInt32 grayC;

	grayC = k8DkGray2Color;

	tempRect = *mirror;
	ColorRect(hdcDest, &tempRect, k8WhiteColor);
	ColorFrameRect(hdcDest, &tempRect, grayC);
	QInsetRect(&tempRect, 1, 1);
	ColorFrameRect(hdcDest, &tempRect, k8EarthBlueColor);
	QInsetRect(&tempRect, 1, 1);
	ColorFrameRect(hdcDest, &tempRect, k8EarthBlueColor);
	QInsetRect(&tempRect, 1, 1);
	ColorFrameRect(hdcDest, &tempRect, grayC);
}

//--------------------------------------------------------------  DrawSimpleClutter

void DrawSimpleClutter (HDC hdcDest, SInt16 what, const Rect *theRect)
{
	if (what < 0 || what >= ARRAYSIZE(g_srcRects))
		return;

	Mac_CopyMask(g_clutterSrcMap, g_clutterMaskMap, hdcDest,
			&g_srcRects[what], &g_srcRects[what], theRect);
}

//--------------------------------------------------------------  DrawFlower

void DrawFlower (HDC hdcDest, const Rect *theRect, SInt16 which)
{
	if (which < 0 || which >= ARRAYSIZE(g_flowerSrc))
		return;

	Mac_CopyMask(g_clutterSrcMap, g_clutterMaskMap, hdcDest,
			&g_flowerSrc[which], &g_flowerSrc[which], theRect);
}

//--------------------------------------------------------------  DrawWallWindow

void DrawWallWindow (HDC hdcDest, const Rect *window)
{
	#define kWindowSillThick  7
	Rect tempRect, tempRect2;
	SInt32 brownC, tanC, dkstRedC;
	SInt16 halfWay;

	brownC = k8BrownColor;
	tanC = k8TanColor;
	dkstRedC = k8DkRed2Color;

	tempRect = *window;
	QInsetRect(&tempRect, 3, 0);
	ColorRect(hdcDest, &tempRect, brownC);
	HiliteRect(hdcDest, &tempRect, tanC, dkstRedC);

	// top sill
	tempRect = *window;
	tempRect.bottom = tempRect.top + kWindowSillThick;
	tempRect.left++;
	tempRect.right--;
	ColorRect(hdcDest, &tempRect, brownC);
	HiliteRect(hdcDest, &tempRect, tanC, dkstRedC);
	tempRect.left--;
	tempRect.right++;
	tempRect.top += 2;
	tempRect.bottom -= 2;
	ColorRect(hdcDest, &tempRect, brownC);
	HiliteRect(hdcDest, &tempRect, tanC, dkstRedC);

	// bottom sill
	tempRect = *window;
	tempRect.top = tempRect.bottom - kWindowSillThick;
	QOffsetRect(&tempRect, 0, -4);
	tempRect.left++;
	tempRect.right--;
	ColorRect(hdcDest, &tempRect, brownC);
	HiliteRect(hdcDest, &tempRect, tanC, dkstRedC);
	tempRect.left--;
	tempRect.right++;
	tempRect.top += 2;
	tempRect.bottom -= 2;
	ColorRect(hdcDest, &tempRect, brownC);
	HiliteRect(hdcDest, &tempRect, tanC, dkstRedC);

	// inside frame
	tempRect = *window;
	tempRect.left += 8;
	tempRect.right -= 8;
	tempRect.top += 11;
	tempRect.bottom -= 15;
	HiliteRect(hdcDest, &tempRect, dkstRedC, tanC);

	halfWay = (tempRect.top + tempRect.bottom) / 2;

	// top pane
	tempRect2 = tempRect;
	tempRect2.bottom = halfWay + 2;
	QInsetRect(&tempRect2, 5, 5);
	HiliteRect(hdcDest, &tempRect2, dkstRedC, tanC);
	QInsetRect(&tempRect2, 1, 1);
	ColorRect(hdcDest, &tempRect2, k8SkyColor);

	// bottom pane
	tempRect2 = tempRect;
	tempRect2.top = halfWay - 3;
	QInsetRect(&tempRect2, 5, 5);
	HiliteRect(hdcDest, &tempRect2, dkstRedC, tanC);
	QInsetRect(&tempRect2, 1, 1);
	ColorRect(hdcDest, &tempRect2, k8SkyColor);

	ColorLine(hdcDest, tempRect2.left - 5, tempRect2.top - 7,
			tempRect2.right + 5, tempRect2.top - 7, tanC);
}

//--------------------------------------------------------------  DrawCalendar

void DrawCalendar (HDC hdcDest, const Rect *theRect)
{
	SYSTEMTIME localTime;
	Rect bounds;
	HBITMAP thePicture;
	WCHAR monthStr[256];
	INT monthLen;
	HFONT theFont;

	thePicture = Gp_LoadImage(g_theHouseFile, kCalendarPictID);
	if (thePicture == NULL)
		RedAlert(kErrFailedGraphicLoad);

	GetGraphicRect(thePicture, &bounds);
	QOffsetRect(&bounds, theRect->left, theRect->top);
	Mac_DrawPicture(hdcDest, thePicture, &bounds);
	DeleteBitmap(thePicture);

	GetLocalTime(&localTime);
	monthLen = LoadString(HINST_THISCOMPONENT,
			kMonthStringBase + localTime.wMonth,
			monthStr, ARRAYSIZE(monthStr));

	SaveDC(hdcDest);
	SetBkMode(hdcDest, TRANSPARENT);
	SetTextAlign(hdcDest, TA_CENTER | TA_BASELINE);
	SetTextColor(hdcDest, Index2ColorRef(kDarkFleshColor));
	theFont = CreateTahomaFont(-9, FW_BOLD);
	SelectFont(hdcDest, theFont);
	TextOut(hdcDest, theRect->left + 32, theRect->top + 55, monthStr, monthLen);
	RestoreDC(hdcDest, -1);
	DeleteFont(theFont);
}

//--------------------------------------------------------------  DrawBulletin

void DrawBulletin (HDC hdcDest, const Rect *theRect)
{
	Rect bounds;
	HBITMAP thePicture;

	thePicture = Gp_LoadImage(g_theHouseFile, kBulletinPictID);
	if (thePicture == NULL)
		RedAlert(kErrFailedGraphicLoad);

	GetGraphicRect(thePicture, &bounds);
	QOffsetRect(&bounds, theRect->left, theRect->top);
	Mac_DrawPicture(hdcDest, thePicture, &bounds);
	DeleteBitmap(thePicture);
}

//--------------------------------------------------------------  DrawPictObject

void DrawPictObject (HDC hdcDest, SInt16 what, const Rect *theRect)
{
	Rect bounds;
	HBITMAP thePicture;
	SInt16 pictID;

	switch (what)
	{
		case kFilingCabinet:
		pictID = kFilingCabinetPictID;
		break;

		case kDownStairs:
		pictID = kDownStairsPictID;
		break;

		case kDoorExRt:
		pictID = kDoorExRightPictID;
		break;

		case kDoorExLf:
		pictID = kDoorExLeftPictID;
		break;

		case kWindowExRt:
		pictID = kWindowExRightPictID;
		break;

		case kWindowExLf:
		pictID = kWindowExLeftPictID;
		break;

		case kOzma:
		pictID = kOzmaPictID;
		break;

		default:
		return;
	}

	thePicture = Gp_LoadImage(g_theHouseFile, pictID);
	if (thePicture == NULL)
		RedAlert(kErrFailedGraphicLoad);

	bounds = g_srcRects[what];
	QOffsetRect(&bounds, theRect->left, theRect->top);
	Mac_DrawPicture(hdcDest, thePicture, &bounds);
	DeleteBitmap(thePicture);
}

//--------------------------------------------------------------  DrawPictWithMaskObject

void DrawPictWithMaskObject (HDC hdcDest, SInt16 what, const Rect *theRect)
{
	Rect bounds;
	HDC tempMap;
	HDC tempMask;
	SInt16 pictID, maskID;

	switch (what)
	{
		case kCobweb:
		pictID = kCobwebPictID;
		maskID = kCobwebMaskID;
		break;

		case kCloud:
		pictID = kCloudPictID;
		maskID = kCloudMaskID;
		break;

		default:
		return;
	}

	bounds = g_srcRects[what];
	tempMap = CreateOffScreenGWorld(&bounds, kPreferredDepth);
	LoadGraphic(tempMap, g_theHouseFile, pictID);

	tempMask = CreateOffScreenGWorld(&bounds, 1);
	LoadGraphic(tempMask, g_theHouseFile, maskID);

	Mac_CopyMask(tempMap, tempMask, hdcDest,
			&g_srcRects[what], &g_srcRects[what], theRect);

	DisposeGWorld(tempMap);
	DisposeGWorld(tempMask);
}

//--------------------------------------------------------------  CopyBitsSansWhite

void CopyBitsSansWhite (HDC srcBits, HDC dstBits, const Rect *srcRect, const Rect *dstRect)
{
	if (srcRect->left >= srcRect->right || srcRect->top >= srcRect->bottom)
	{
		return;
	}
	if (dstRect->left >= dstRect->right || dstRect->top >= dstRect->bottom)
	{
		return;
	}
	TransparentBlt(
		dstBits,
		dstRect->left,
		dstRect->top,
		dstRect->right - dstRect->left,
		dstRect->bottom - dstRect->top,
		srcBits,
		srcRect->left,
		srcRect->top,
		srcRect->right - srcRect->left,
		srcRect->bottom - srcRect->top,
		RGB(0xFF, 0xFF, 0xFF)
	);
}

//--------------------------------------------------------------  DrawPictSansWhiteObject

void DrawPictSansWhiteObject (HDC hdcDest, SInt16 what, const Rect *theRect)
{
	Rect bounds;
	HDC tempMap;
	SInt16 pictID;

	switch (what)
	{
		case kBBQ:
		pictID = kBBQPictID;
		break;

		case kTrunk:
		pictID = kTrunkPictID;
		break;

		case kManhole:
		pictID = kManholePictID;
		break;

		case kBooks:
		pictID = kBooksPictID;
		break;

		case kUpStairs:
		pictID = kUpStairsPictID;
		break;

		case kDoorInLf:
		pictID = kDoorInLeftPictID;
		break;

		case kDoorInRt:
		pictID = kDoorInRightPictID;
		break;

		case kWindowInLf:
		pictID = kWindowInLeftPictID;
		break;

		case kWindowInRt:
		pictID = kWindowInRightPictID;
		break;

		case kHipLamp:
		pictID = kHipLampPictID;
		break;

		case kDecoLamp:
		pictID = kDecoLampPictID;
		break;

		case kGuitar:
		pictID = kGuitarPictID;
		break;

		case kCinderBlock:
		pictID = kCinderPictID;
		break;

		case kFlowerBox:
		pictID = kFlowerBoxPictID;
		break;

		case kFireplace:
		pictID = kFireplacePictID;
		break;

		case kBear:
		pictID = kBearPictID;
		break;

		case kVase1:
		pictID = kVase1PictID;
		break;

		case kVase2:
		pictID = kVase2PictID;
		break;

		case kRug:
		pictID = kRugPictID;
		break;

		case kChimes:
		pictID = kChimesPictID;
		break;

		default:
		return;
	}

	bounds = g_srcRects[what];
	if (bounds.left >= bounds.right || bounds.top >= bounds.bottom)
		return;

	tempMap = CreateOffScreenGWorld(&bounds, kPreferredDepth);
	LoadGraphic(tempMap, g_theHouseFile, pictID);
	CopyBitsSansWhite(tempMap, hdcDest, &g_srcRects[what], theRect);
	DisposeGWorld(tempMap);
}

//--------------------------------------------------------------  DrawCustPictSansWhite

void DrawCustPictSansWhite (HDC hdcDest, SInt16 pictID, const Rect *theRect)
{
	Rect bounds;
	HDC tempMap;

	if (theRect->left >= theRect->right || theRect->top >= theRect->bottom)
		return;

	bounds = *theRect;
	ZeroRectCorner(&bounds);
	tempMap = CreateOffScreenGWorld(&bounds, kPreferredDepth);
	LoadGraphic(tempMap, g_theHouseFile, pictID);
	CopyBitsSansWhite(tempMap, hdcDest, &bounds, theRect);
	DisposeGWorld(tempMap);
}
