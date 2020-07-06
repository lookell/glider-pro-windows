#include "ObjectDraw.h"

//============================================================================
//----------------------------------------------------------------------------
//                                ObjectDraw.c
//----------------------------------------------------------------------------
//============================================================================


#include "ColorUtils.h"
#include "Environ.h"
#include "Macintosh.h"
#include "Objects.h"
#include "RectUtils.h"
#include "Room.h"
#include "Utilities.h"


#define k8WhiteColor			0
#define kYellowColor			5
#define kGoldColor				11
#define k8RedColor				35
#define kPaleVioletColor		42
#define k8LtTanColor			52
#define k8BambooColor			53
#define kDarkFleshColor			58
#define k8TanColor				94
#define k8PissYellowColor		95
#define k8OrangeColor			59
#define k8BrownColor			137
#define k8Red4Color				143
#define k8SkyColor				150
#define k8EarthBlueColor		170
#define k8DkRedColor			222
#define k8DkRed2Color			223
#define kIntenseGreenColor		225
#define kIntenseBlueColor		235
#define k8PumpkinColor			101
#define k8LtstGrayColor			245
#define k8LtstGray2Color		246
#define k8LtstGray3Color		43
#define k8LtstGray4Color		247
#define k8LtstGray5Color		248
#define k8LtGrayColor			249
#define k8GrayColor				250
#define k8Gray2Color			251
#define k8DkGrayColor			252
#define k8DkGray2Color			253
#define k8DkGray3Color			172
#define k8DkstGrayColor			254
#define k8BlackColor			255


void DrawClockDigit (SInt16 number, Rect *dest);
void DrawClockHands (Point where, SInt16 bigHand, SInt16 littleHand);
void DrawLargeClockHands (Point where, SInt16 bigHand, SInt16 littleHand);


//==============================================================  Functions
//--------------------------------------------------------------  DrawSimpleBlowers

void DrawSimpleBlowers (SInt16 what, Rect *theRect)
{
	Mac_CopyMask(blowerSrcMap, blowerMaskMap, backSrcMap,
			&srcRects[what], &srcRects[what], theRect);
}

//--------------------------------------------------------------  DrawTiki

void DrawTiki (Rect *theRect, SInt16 down)
{
#define kTikiPoleBase	300
	SInt32		darkGrayC, lightWoodC, darkWoodC;

	if (thisMac.isDepth == 4)
	{
		darkGrayC = 14;
		lightWoodC = 6;
		darkWoodC = 9;
	}
	else
	{
		darkGrayC = k8DkstGrayColor;
		lightWoodC = k8BambooColor;
		darkWoodC = k8PissYellowColor;
	}

	if (theRect->bottom < kTikiPoleBase + down)
	{
		ColorLine(backSrcMap, theRect->left + 11, theRect->bottom - 1,
				theRect->left + 11, kTikiPoleBase + down - 1, darkGrayC);
		ColorLine(backSrcMap, theRect->left + 12, theRect->bottom - 1,
				theRect->left + 12, kTikiPoleBase + down, lightWoodC);
		ColorLine(backSrcMap, theRect->left + 13, theRect->bottom - 1,
				theRect->left + 13, kTikiPoleBase + down, darkWoodC);
		ColorLine(backSrcMap, theRect->left + 14, theRect->bottom - 1,
				theRect->left + 14, kTikiPoleBase + down, darkWoodC);
		ColorLine(backSrcMap, theRect->left + 15, theRect->bottom - 1,
				theRect->left + 15, kTikiPoleBase + down - 1, darkGrayC);
	}

	Mac_CopyMask(blowerSrcMap, blowerMaskMap, backSrcMap,
			&srcRects[kTiki], &srcRects[kTiki], theRect);
}

//--------------------------------------------------------------  DrawInvisibleBlower

void DrawInvisibleBlower (Rect *theRect)
{
	Rect		tempRect;

	QSetRect(&tempRect, 0, 0, 24, 24);
	QOffsetRect(&tempRect, theRect->left, theRect->top);

	ColorFrameRect(backSrcMap, &tempRect, 192);
}

//--------------------------------------------------------------  DrawLiftArea

void DrawLiftArea (Rect *theRect)
{
	ColorFrameRect(backSrcMap, theRect, 192);
}

//--------------------------------------------------------------  DrawTable

void DrawTable (Rect *tableTop, SInt16 down)
{
	#define		kTableBaseTop		296
	#define		kTableShadowTop		312
	#define		kTableShadowOffset	12
	Rect		tempRect;
	SInt32		brownC, tanC, dkRedC, blackC;
	SInt16		hCenter, vShadow;

	if (thisMac.isDepth == 4)
	{
		brownC = 11;
		tanC = 9;
		dkRedC = 14;
		blackC = 15;
	}
	else
	{
		brownC = k8BrownColor;
		tanC = k8TanColor;
		dkRedC = k8DkRed2Color;
		blackC = k8BlackColor;
	}

	QSetRect(&tempRect, tableTop->left, 0, tableTop->right,
			RectWide(tableTop) / 10);
	QOffsetRect(&tempRect, 0,
			-HalfRectTall(&tempRect) + kTableShadowTop + down);
	QOffsetRect(&tempRect, kTableShadowOffset, -kTableShadowOffset);
	if (thisMac.isDepth == 4)
		ColorShadowOval(backSrcMap, &tempRect, 15);
	else
		ColorShadowOval(backSrcMap, &tempRect, k8DkstGrayColor);

	QInsetRect(tableTop, 0, 1);
	ColorRect(backSrcMap, tableTop, brownC);
	QInsetRect(tableTop, 0, -1);

	ColorLine(backSrcMap, tableTop->left, tableTop->top + 1,
			tableTop->left, tableTop->top + 1, k8WhiteColor);
	ColorLine(backSrcMap, tableTop->left + 1, tableTop->top,
			tableTop->right - 2, tableTop->top, k8WhiteColor);
	ColorLine(backSrcMap, tableTop->right - 1, tableTop->top + 1,
			tableTop->right - 1, tableTop->top + 1, k8WhiteColor);

	ColorLine(backSrcMap, tableTop->left + 1, tableTop->top + 1,
			tableTop->right - 2, tableTop->top + 1, tanC);
	ColorLine(backSrcMap, tableTop->left, tableTop->top + 2,
			tableTop->left, tableTop->bottom - 2, tanC);

	ColorLine(backSrcMap, tableTop->left + 1, tableTop->bottom - 1,
			tableTop->right - 2, tableTop->bottom - 1, blackC);
	ColorLine(backSrcMap, tableTop->right - 1, tableTop->top + 2,
			tableTop->right - 1, tableTop->bottom - 2, blackC);

	ColorLine(backSrcMap, tableTop->left + 1, tableTop->bottom - 2,
			tableTop->right - 2, tableTop->bottom - 2, dkRedC);

	if (tableTop->bottom < kTableBaseTop + down)
	{
		hCenter = (tableTop->left + tableTop->right) / 2;

		ColorLine(backSrcMap, hCenter - 3, tableTop->bottom,
				hCenter - 3, kTableBaseTop + down, blackC);
		ColorLine(backSrcMap, hCenter - 2, tableTop->bottom,
				hCenter - 2, kTableBaseTop + down, k8LtGrayColor);
		ColorLine(backSrcMap, hCenter - 1, tableTop->bottom,
				hCenter - 1, kTableBaseTop + down, k8GrayColor);
		ColorLine(backSrcMap, hCenter, tableTop->bottom,
				hCenter, kTableBaseTop + down, k8DkGrayColor);
		ColorLine(backSrcMap, hCenter + 1, tableTop->bottom,
				hCenter + 1, kTableBaseTop + down, blackC);

		vShadow = tableTop->bottom + RectWide(tableTop) / 4 - 2;
		if (vShadow > kTableBaseTop + down)
		{
			ColorLine(backSrcMap, hCenter - 2, tableTop->bottom,
					hCenter - 2, kTableBaseTop + down, k8DkGrayColor);
			ColorLine(backSrcMap, hCenter - 1, tableTop->bottom,
					hCenter - 1, kTableBaseTop + down, k8DkGrayColor);
			ColorLine(backSrcMap, hCenter, tableTop->bottom,
					hCenter, kTableBaseTop + down, blackC);
		}
		else
		{
			ColorLine(backSrcMap, hCenter - 2, tableTop->bottom,
					hCenter - 2, vShadow, k8DkGrayColor);
			ColorLine(backSrcMap, hCenter - 1, tableTop->bottom,
					hCenter - 1, vShadow + 1, k8DkGrayColor);
			ColorLine(backSrcMap, hCenter, tableTop->bottom,
					hCenter, vShadow + 2, blackC);
		}
	}

	tempRect = tableSrc;
	QOffsetRect(&tempRect, -HalfRectWide(&tableSrc) + tableTop->left +
			HalfRectWide(tableTop), kTableBaseTop + down);
	Mac_CopyMask(furnitureSrcMap, furnitureMaskMap, backSrcMap,
			&tableSrc, &tableSrc, &tempRect);
}

//--------------------------------------------------------------  DrawShelf

void DrawShelf (Rect *shelfTop)
{
	#define		kBracketInset		18
	#define		kShelfDeep			4
	#define		kBracketThick		5
	#define		kShelfShadowOff		12
	Rect		tempRect;
	SInt32		brownC, ltTanC, tanC, dkRedC, blackC;
	HRGN		shadowRgn;

	if (thisMac.isDepth == 4)
	{
		brownC = 11;
		ltTanC = 7;
		tanC = 9;
		dkRedC = 14;
		blackC = 15;
	}
	else
	{
		brownC = k8BrownColor;
		ltTanC = k8LtTanColor;
		tanC = k8TanColor;
		dkRedC = k8DkRed2Color;
		blackC = k8BlackColor;
	}

	BeginPath(backSrcMap);
	MoveToEx(backSrcMap, shelfTop->left, shelfTop->bottom, NULL);
	Mac_Line(backSrcMap, kShelfShadowOff, kShelfShadowOff);
	Mac_Line(backSrcMap, RectWide(shelfTop) - kShelfDeep, 0);
	Mac_Line(backSrcMap, 0, -kShelfThick + 1);
	Mac_Line(backSrcMap, -kShelfShadowOff, -kShelfShadowOff);
	Mac_LineTo(backSrcMap, shelfTop->left, shelfTop->bottom);
	EndPath(backSrcMap);
	shadowRgn = PathToRegion(backSrcMap);
	if (shadowRgn == NULL)
		RedAlert(kErrUnnaccounted);
	if (thisMac.isDepth == 4)
		ColorShadowRegion(backSrcMap, shadowRgn, 15);
	else
		ColorShadowRegion(backSrcMap, shadowRgn, k8DkstGrayColor);
	DeleteObject(shadowRgn);

	QInsetRect(shelfTop, 0, 1);
	ColorRect(backSrcMap, shelfTop, brownC);
	QInsetRect(shelfTop, 0, -1);

	ColorLine(backSrcMap, shelfTop->left + 1, shelfTop->top,
			shelfTop->left + 1 + kShelfDeep, shelfTop->top, ltTanC);
	ColorLine(backSrcMap, shelfTop->left, shelfTop->top + 1,
			shelfTop->left + kShelfDeep, shelfTop->top + 1, tanC);
	ColorLine(backSrcMap, shelfTop->left, shelfTop->top + 2,
			shelfTop->left + kShelfDeep, shelfTop->top + 2, tanC);
	ColorLine(backSrcMap, shelfTop->left, shelfTop->top + 3,
			shelfTop->left + kShelfDeep, shelfTop->top + 3, tanC);
	ColorLine(backSrcMap, shelfTop->left + 1, shelfTop->bottom - 1,
			shelfTop->left + 1 + kShelfDeep, shelfTop->bottom - 1, dkRedC);
	ColorLine(backSrcMap, shelfTop->left + 2 + kShelfDeep, shelfTop->bottom - 1,
			shelfTop->right - 2, shelfTop->bottom - 1, blackC);
	ColorLine(backSrcMap, shelfTop->left + 2 + kShelfDeep, shelfTop->top,
			shelfTop->right - 2, shelfTop->top, tanC);
	ColorLine(backSrcMap, shelfTop->right - 1, shelfTop->top + 1,
			shelfTop->right - 1, shelfTop->bottom - 2, blackC);

	tempRect = shelfSrc;
	ZeroRectCorner(&tempRect);
	QOffsetRect(&tempRect, shelfTop->left + kBracketInset, shelfTop->bottom);
	Mac_CopyMask(furnitureSrcMap, furnitureMaskMap, backSrcMap,
			&shelfSrc, &shelfSrc, &tempRect);

	ZeroRectCorner(&tempRect);
	QOffsetRect(&tempRect, shelfTop->right - kBracketInset - kShelfDeep -
			kBracketThick, shelfTop->bottom);
	Mac_CopyMask(furnitureSrcMap, furnitureMaskMap, backSrcMap,
			&shelfSrc, &shelfSrc, &tempRect);
}

//--------------------------------------------------------------  DrawCabinet

void DrawCabinet (Rect *cabinet)
{
	#define		kCabinetDeep		4
	#define		kCabinetShadowOff	6
	Rect		tempRect;
	SInt32		brownC, dkGrayC, ltTanC, tanC, dkRedC, blackC;
	HRGN		shadowRgn;

	if (thisMac.isDepth == 4)
	{
		brownC = 11;
		dkGrayC = 14;
		ltTanC = 7;
		tanC = 9;
		dkRedC = 14;
		blackC = 15;
	}
	else
	{
		brownC = k8BrownColor;
		dkGrayC = k8DkstGrayColor;
		ltTanC = k8LtTanColor;
		tanC = k8TanColor;
		dkRedC = k8DkRed2Color;
		blackC = k8BlackColor;
	}

	BeginPath(backSrcMap);
	MoveToEx(backSrcMap, cabinet->left, cabinet->bottom, NULL);
	Mac_Line(backSrcMap, kCabinetShadowOff, kCabinetShadowOff);
	Mac_Line(backSrcMap, RectWide(cabinet), 0);
	Mac_Line(backSrcMap, 0, -RectTall(cabinet) + kCabinetDeep);
	Mac_Line(backSrcMap, -kCabinetShadowOff, -kCabinetShadowOff);
	Mac_LineTo(backSrcMap, cabinet->left, cabinet->bottom);
	EndPath(backSrcMap);
	shadowRgn = PathToRegion(backSrcMap);
	if (shadowRgn == NULL)
		RedAlert(kErrUnnaccounted);
	if (thisMac.isDepth == 4)
		ColorShadowRegion(backSrcMap, shadowRgn, 15);
	else
		ColorShadowRegion(backSrcMap, shadowRgn, dkGrayC);
	DeleteObject(shadowRgn);

	QInsetRect(cabinet, 1, 1);	// fill bulk of cabinet brown
	ColorRect(backSrcMap, cabinet, brownC);
	QInsetRect(cabinet, -1, -1);

	tempRect = *cabinet;			// add lighter left side
	tempRect.right = tempRect.left + kCabinetDeep;
	ColorRect(backSrcMap, &tempRect, tanC);
									// hilight top edge
	ColorLine(backSrcMap, cabinet->left + 1, cabinet->top + 1,
			cabinet->left + kCabinetDeep, cabinet->top + 1, ltTanC);
	ColorLine(backSrcMap, cabinet->left + kCabinetDeep, cabinet->top + 1,
			cabinet->right - 3, cabinet->top + 1, tanC);
									// shadow bottom edge

	ColorLine(backSrcMap, cabinet->left + kCabinetDeep + 3, cabinet->top + 5,
			cabinet->left + kCabinetDeep + 3, cabinet->bottom - 6, tanC);
	ColorLine(backSrcMap, cabinet->left + kCabinetDeep + 4, cabinet->top + 5,
			cabinet->left + kCabinetDeep + 4, cabinet->bottom - 6, tanC);
	ColorLine(backSrcMap, cabinet->left + kCabinetDeep + 9, cabinet->top + 10,
			cabinet->left + kCabinetDeep + 9, cabinet->bottom - 11, dkGrayC);

	ColorLine(backSrcMap, cabinet->right - 4, cabinet->top + 6,
			cabinet->right - 4, cabinet->bottom - 5, dkRedC);
	ColorLine(backSrcMap, cabinet->right - 5, cabinet->top + 5,
			cabinet->right - 5, cabinet->bottom - 6, dkGrayC);
	ColorLine(backSrcMap, cabinet->right - 10, cabinet->top + 10,
			cabinet->right - 10, cabinet->bottom - 11, tanC);

	ColorLine(backSrcMap, cabinet->left + kCabinetDeep + 4, cabinet->top + 4,
			cabinet->left + kCabinetDeep + 4, cabinet->top + 4, ltTanC);
	ColorLine(backSrcMap, cabinet->left + kCabinetDeep + 5, cabinet->top + 4,
			cabinet->right - 6, cabinet->top + 4, tanC);
	ColorLine(backSrcMap, cabinet->left + kCabinetDeep + 10, cabinet->top + 9,
			cabinet->right - 11, cabinet->top + 9, dkGrayC);

	ColorLine(backSrcMap, cabinet->right - 5, cabinet->bottom - 5,
			cabinet->right - 5, cabinet->bottom - 5, dkRedC);
	ColorLine(backSrcMap, cabinet->left + kCabinetDeep + 6, cabinet->bottom - 4,
			cabinet->right - 5, cabinet->bottom - 4, dkRedC);
	ColorLine(backSrcMap, cabinet->left + kCabinetDeep + 5, cabinet->bottom - 5,
			cabinet->right - 6, cabinet->bottom - 5, dkGrayC);

	ColorLine(backSrcMap, cabinet->left + kCabinetDeep + 10, cabinet->bottom - 10,
			cabinet->right - 11, cabinet->bottom - 10, tanC);

	tempRect = hingeSrc;
	ZeroRectCorner(&tempRect);
	QOffsetRect(&tempRect, cabinet->left + kCabinetDeep + 2, cabinet->top + 10);
	Mac_CopyMask(furnitureSrcMap, furnitureMaskMap, backSrcMap,
			&hingeSrc, &hingeSrc, &tempRect);

	tempRect = hingeSrc;
	ZeroRectCorner(&tempRect);
	QOffsetRect(&tempRect, cabinet->left + kCabinetDeep + 2, cabinet->bottom - 26);
	Mac_CopyMask(furnitureSrcMap, furnitureMaskMap, backSrcMap,
			&hingeSrc, &hingeSrc, &tempRect);

	tempRect = handleSrc;
	ZeroRectCorner(&tempRect);
	QOffsetRect(&tempRect, cabinet->right - 8, cabinet->top +
			HalfRectTall(cabinet) - HalfRectTall(&handleSrc));
	Mac_CopyMask(furnitureSrcMap, furnitureMaskMap, backSrcMap,
			&handleSrc, &handleSrc, &tempRect);

	Mac_FrameRect(backSrcMap, cabinet, GetStockObject(BLACK_BRUSH), 1, 1);
}

//--------------------------------------------------------------  DrawSimpleFurniture

void DrawSimpleFurniture (SInt16 what, Rect *theRect)
{
	Mac_CopyMask(furnitureSrcMap, furnitureMaskMap, backSrcMap,
			&srcRects[what], &srcRects[what], theRect);
}

//--------------------------------------------------------------  DrawCounter

void DrawCounter (Rect *counter)
{
	#define		kCounterFooterHigh	12
	#define		kCounterStripWide	6
	#define		kCounterStripTall	29
	#define		kCounterPanelDrop	12
	Rect		tempRect;
	HRGN		shadowRgn;
	SInt32		brownC, dkGrayC, tanC, blackC, dkstRedC;
	SInt16		nRects, width, i;
	
	if (thisMac.isDepth == 4)
	{
		brownC = 11;
		dkGrayC = 14;
		tanC = 9;
		blackC = 15;
		dkstRedC = 15;
	}
	else
	{
		brownC = k8BrownColor;
		dkGrayC = k8DkstGrayColor;
		tanC = k8TanColor;
		blackC = k8BlackColor;
		dkstRedC = k8DkRed2Color;
	}

	BeginPath(backSrcMap);
	MoveToEx(backSrcMap, counter->right - 2, counter->bottom, NULL);
	Mac_Line(backSrcMap, 10, -10);
	Mac_Line(backSrcMap, 0, -RectTall(counter) + 29);
	Mac_Line(backSrcMap, 2, 0);
	Mac_Line(backSrcMap, 0, -7);
	Mac_Line(backSrcMap, -12, -12);
	Mac_LineTo(backSrcMap, counter->right - 2, counter->bottom);
	EndPath(backSrcMap);
	shadowRgn = PathToRegion(backSrcMap);
	if (shadowRgn == NULL)
		RedAlert(kErrUnnaccounted);
	if (thisMac.isDepth == 4)
		ColorShadowRegion(backSrcMap, shadowRgn, 15);
	else
		ColorShadowRegion(backSrcMap, shadowRgn, dkGrayC);
	DeleteObject(shadowRgn);

	QInsetRect(counter, 2, 2);
	ColorRect(backSrcMap, counter, brownC);
	QInsetRect(counter, -2, -2);

	tempRect = *counter;
	tempRect.top = tempRect.bottom - kCounterFooterHigh;
	tempRect.left += 2;
	tempRect.right -= 2;
	ColorRect(backSrcMap, &tempRect, dkGrayC);
	ColorLine(backSrcMap, counter->left + 2, counter->bottom - kCounterFooterHigh,
			counter->right - 3, counter->bottom - kCounterFooterHigh, blackC);
	ColorLine(backSrcMap, counter->left + 2, counter->bottom - kCounterFooterHigh + 1,
			counter->right - 3, counter->bottom - kCounterFooterHigh + 1, blackC);
	ColorLine(backSrcMap, counter->right - 3, counter->bottom - kCounterFooterHigh,
			counter->right - 3, counter->bottom - 1, blackC);
	ColorLine(backSrcMap, counter->left + 2, counter->bottom - kCounterFooterHigh,
			counter->left + 2, counter->bottom - 1, k8DkGrayColor);

	ColorLine(backSrcMap, counter->right - 2, counter->top,
			counter->right - 2, counter->bottom - kCounterFooterHigh - 1, dkstRedC);
	ColorLine(backSrcMap, counter->left + 1, counter->top + 8,
			counter->left + 1, counter->bottom - kCounterFooterHigh - 1, tanC);

	if (thisMac.isDepth == 4)
	{
		ColorLine(backSrcMap, counter->left - 1, counter->top,
				counter->right, counter->top, 1);
		ColorLine(backSrcMap, counter->left - 1, counter->top + 1,
				counter->right, counter->top + 1, 2);
		ColorLine(backSrcMap, counter->left - 1, counter->top + 2,
				counter->right, counter->top + 2, 3);
		ColorLine(backSrcMap, counter->left - 1, counter->top + 3,
				counter->right, counter->top + 3, 4);
		ColorLine(backSrcMap, counter->left - 1, counter->top + 4,
				counter->right, counter->top + 4, 5);
		ColorLine(backSrcMap, counter->left - 1, counter->top + 5,
				counter->right, counter->top + 5, 5);
		ColorLine(backSrcMap, counter->left - 1, counter->top + 6,
				counter->right, counter->top + 6, 5);
		ColorLine(backSrcMap, counter->left - 1, counter->top,
				counter->left - 1, counter->top + 6, 1);
	}
	else
	{
		ColorLine(backSrcMap, counter->left - 1, counter->top,
				counter->right, counter->top, k8LtstGrayColor);
		ColorLine(backSrcMap, counter->left - 1, counter->top + 1,
				counter->right, counter->top + 1, k8LtstGray2Color);
		ColorLine(backSrcMap, counter->left - 1, counter->top + 2,
				counter->right, counter->top + 2, k8LtstGray3Color);
		ColorLine(backSrcMap, counter->left - 1, counter->top + 3,
				counter->right, counter->top + 3, k8LtstGray4Color);
		ColorLine(backSrcMap, counter->left - 1, counter->top + 4,
				counter->right, counter->top + 4, k8LtstGray5Color);
		ColorLine(backSrcMap, counter->left - 1, counter->top + 5,
				counter->right, counter->top + 5, k8LtstGray5Color);
		ColorLine(backSrcMap, counter->left - 1, counter->top + 6,
				counter->right, counter->top + 6, k8LtstGray5Color);
		ColorLine(backSrcMap, counter->left - 1, counter->top,
				counter->left - 1, counter->top + 6, k8LtstGrayColor);
	}

	ColorLine(backSrcMap, counter->right, counter->top,
			counter->right, counter->top + 6, k8LtGrayColor);
	ColorLine(backSrcMap, counter->left + 1, counter->top + 7,
			counter->right - 2, counter->top + 7, dkstRedC);
	ColorLine(backSrcMap, counter->left + 1, counter->top + 8,
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
		HiliteRect(backSrcMap, &tempRect, tanC, dkstRedC);
		QInsetRect(&tempRect, 4, 4);
		HiliteRect(backSrcMap, &tempRect, dkstRedC, tanC);
		QInsetRect(&tempRect, -4, -4);
		QOffsetRect(&tempRect, kCounterStripWide + width, 0);
	}
}

//--------------------------------------------------------------  DrawDresser

void DrawDresser (Rect *dresser)
{
	#define		kDresserTopThick	4
	#define		kDresserCrease		9
	#define		kDresserDrawerDrop	12
	#define		kDresserSideSpare	14
	Rect		tempRect, dest;
	SInt32		yellowC, brownC, dkGrayC, ltTanC, dkstRedC;
	HRGN		shadowRgn;
	SInt16		nRects, height, i;

	if (thisMac.isDepth == 4)
	{
		yellowC = 9;
		brownC = 11;
		dkGrayC = 14;
		ltTanC = 7;
		dkstRedC = 15;
	}
	else
	{
		yellowC = k8PissYellowColor;
		brownC = k8BrownColor;
		dkGrayC = k8DkstGrayColor;
		ltTanC = k8LtTanColor;
		dkstRedC = k8DkRed2Color;
	}

	BeginPath(backSrcMap);
	MoveToEx(backSrcMap, dresser->left + 10, dresser->bottom + 9, NULL);
	Mac_Line(backSrcMap, RectWide(dresser) - 11, 0);
	Mac_Line(backSrcMap, 9, -9);
	Mac_Line(backSrcMap, 0, -RectTall(dresser) + 12);
	Mac_Line(backSrcMap, -9, -9);
	Mac_Line(backSrcMap, -RectWide(dresser) + 11, 0);
	Mac_LineTo(backSrcMap, dresser->left + 10, dresser->bottom + 9);
	EndPath(backSrcMap);
	shadowRgn = PathToRegion(backSrcMap);
	if (shadowRgn == NULL)
		RedAlert(kErrUnnaccounted);
	if (thisMac.isDepth == 4)
		ColorShadowRegion(backSrcMap, shadowRgn, 15);
	else
		ColorShadowRegion(backSrcMap, shadowRgn, k8DkstGrayColor);
	DeleteObject(shadowRgn);

	QInsetRect(dresser, 2, 2);
	ColorRect(backSrcMap, dresser, k8PumpkinColor);
	HiliteRect(backSrcMap, dresser, k8OrangeColor, dkstRedC);
	QInsetRect(dresser, -2, -2);

	tempRect = *dresser;
	tempRect.bottom = tempRect.top + kDresserTopThick;
	ColorRect(backSrcMap, &tempRect, k8PissYellowColor);
	HiliteRect(backSrcMap, &tempRect, ltTanC, dkstRedC);
	ColorLine(backSrcMap, dresser->left + 2, dresser->top + kDresserTopThick,
			dresser->right - 3, dresser->top + kDresserTopThick, k8Red4Color);

	ColorLine(backSrcMap, dresser->left + kDresserCrease, dresser->top + kDresserTopThick + 1,
			dresser->left + kDresserCrease, dresser->bottom - 4, k8Red4Color);
	ColorLine(backSrcMap, dresser->right - kDresserCrease, dresser->top + kDresserTopThick + 1,
			dresser->right - kDresserCrease, dresser->bottom - 4, k8OrangeColor);

	nRects = RectTall(dresser) / 30;
	if (nRects == 0)
		nRects = 1;
	height = (RectTall(dresser) - 14) / nRects - 4;
	QSetRect(&tempRect, 0, 0, RectWide(dresser) - kDresserSideSpare, height);
	QOffsetRect(&tempRect, dresser->left + 7, dresser->top + 10);
	for (i = 0; i < nRects; i++)
	{
		ColorLine(backSrcMap, tempRect.left + 1, tempRect.bottom,
				tempRect.right, tempRect.bottom, dkstRedC);
		ColorLine(backSrcMap, tempRect.right, tempRect.top + 1,
				tempRect.right, tempRect.bottom, dkstRedC);
		ColorRect(backSrcMap, &tempRect, yellowC);
		HiliteRect(backSrcMap, &tempRect, ltTanC, brownC);
		QInsetRect(&tempRect, 1, 1);
		HiliteRect(backSrcMap, &tempRect, ltTanC, brownC);
		QInsetRect(&tempRect, -1, -1);

		QSetRect(&dest, -4, -4, 4, 4);
		QOffsetRect(&dest, HalfRectTall(&tempRect), HalfRectTall(&tempRect));
		QOffsetRect(&dest, tempRect.left, tempRect.top);
		Mac_CopyBits(furnitureSrcMap, backSrcMap,
				&knobSrc, &dest, srcCopy, nil);

		QSetRect(&dest, -4, -4, 4, 4);
		QOffsetRect(&dest, -HalfRectTall(&tempRect), HalfRectTall(&tempRect));
		QOffsetRect(&dest, tempRect.right, tempRect.top);
		Mac_CopyBits(furnitureSrcMap, backSrcMap,
				&knobSrc, &dest, srcCopy, nil);

		QOffsetRect(&tempRect, 0, kDresserTopThick + height);
	}

	dest = leftFootSrc;
	ZeroRectCorner(&dest);
	QOffsetRect(&dest, dresser->left + 6, dresser->bottom - 2);

	Mac_CopyMask(furnitureSrcMap, furnitureMaskMap, backSrcMap,
			&leftFootSrc, &leftFootSrc, &dest);

	dest = rightFootSrc;
	ZeroRectCorner(&dest);
	QOffsetRect(&dest, dresser->right - 19, dresser->bottom - 2);

	Mac_CopyMask(furnitureSrcMap, furnitureMaskMap, backSrcMap,
			&rightFootSrc, &rightFootSrc, &dest);
}

//--------------------------------------------------------------  DrawDeckTable

void DrawDeckTable (Rect *tableTop, SInt16 down)
{
	#define		kTableBaseTop		296
	#define		kTableShadowTop		312
	#define		kTableShadowOffset	12

	Rect		tempRect;
	SInt32		bambooC, brownC, dkGrayC;
	SInt16		hCenter, vShadow;

	if (thisMac.isDepth == 4)
	{
		bambooC = 6;
		brownC = 11;
		dkGrayC = 14;
	}
	else
	{
		bambooC = k8BambooColor;
		brownC = k8BrownColor;
		dkGrayC = k8DkstGrayColor;
	}

	QSetRect(&tempRect, tableTop->left, 0, tableTop->right,
			RectWide(tableTop) / 10);
	QOffsetRect(&tempRect, 0,
			-HalfRectTall(&tempRect) + kTableShadowTop + down);
	QOffsetRect(&tempRect, kTableShadowOffset, -kTableShadowOffset);
	ColorShadowOval(backSrcMap, &tempRect, dkGrayC);

	QInsetRect(tableTop, 0, 1);
	ColorRect(backSrcMap, tableTop, kGoldColor);
	QInsetRect(tableTop, 0, -1);

	ColorLine(backSrcMap, tableTop->left, tableTop->top + 1,
			tableTop->left, tableTop->top + 1, k8WhiteColor);
	ColorLine(backSrcMap, tableTop->left + 1, tableTop->top,
			tableTop->right - 2, tableTop->top, k8WhiteColor);
	ColorLine(backSrcMap, tableTop->right - 1, tableTop->top + 1,
			tableTop->right - 1, tableTop->top + 1, k8WhiteColor);

	ColorLine(backSrcMap, tableTop->left + 1, tableTop->top + 1,
			tableTop->right - 2, tableTop->top + 1, kYellowColor);
	ColorLine(backSrcMap, tableTop->left, tableTop->top + 2,
			tableTop->left, tableTop->bottom - 2, kYellowColor);

	ColorLine(backSrcMap, tableTop->left + 1, tableTop->bottom - 1,
			tableTop->right - 2, tableTop->bottom - 1, brownC);
	ColorLine(backSrcMap, tableTop->right - 1, tableTop->top + 2,
			tableTop->right - 1, tableTop->bottom - 2, brownC);

	ColorLine(backSrcMap, tableTop->left + 1, tableTop->bottom - 2,
			tableTop->right - 2, tableTop->bottom - 2, bambooC);

	if (tableTop->bottom < kTableBaseTop + down)
	{
		hCenter = (tableTop->left + tableTop->right) / 2;

		ColorLine(backSrcMap, hCenter - 3, tableTop->bottom,
				hCenter - 3, kTableBaseTop + down, dkGrayC);
		ColorLine(backSrcMap, hCenter - 2, tableTop->bottom,
				hCenter - 2, kTableBaseTop + down, k8WhiteColor);
		ColorLine(backSrcMap, hCenter - 1, tableTop->bottom,
				hCenter - 1, kTableBaseTop + down, k8WhiteColor);
		ColorLine(backSrcMap, hCenter, tableTop->bottom,
				hCenter, kTableBaseTop + down, k8LtGrayColor);
		ColorLine(backSrcMap, hCenter + 1, tableTop->bottom,
				hCenter + 1, kTableBaseTop + down, dkGrayC);

		vShadow = tableTop->bottom + RectWide(tableTop) / 4 - 2;
		if (vShadow > kTableBaseTop + down)
		{
			ColorLine(backSrcMap, hCenter - 2, tableTop->bottom,
					hCenter - 2, kTableBaseTop + down, k8LtGrayColor);
			ColorLine(backSrcMap, hCenter - 1, tableTop->bottom,
					hCenter - 1, kTableBaseTop + down, k8LtGrayColor);
			ColorLine(backSrcMap, hCenter, tableTop->bottom,
					hCenter, kTableBaseTop + down, dkGrayC);
		}
		else
		{
			ColorLine(backSrcMap, hCenter - 2, tableTop->bottom,
					hCenter - 2, vShadow, k8LtGrayColor);
			ColorLine(backSrcMap, hCenter - 1, tableTop->bottom,
					hCenter - 1, vShadow + 1, k8LtGrayColor);
			ColorLine(backSrcMap, hCenter, tableTop->bottom,
					hCenter, vShadow + 2, dkGrayC);
		}
	}

	tempRect = deckSrc;
	ZeroRectCorner(&tempRect);
	QOffsetRect(&tempRect, -HalfRectWide(&deckSrc) + tableTop->left +
			HalfRectWide(tableTop), kTableBaseTop + down);
	Mac_CopyMask(furnitureSrcMap, furnitureMaskMap, backSrcMap,
			&deckSrc, &deckSrc, &tempRect);
}

//--------------------------------------------------------------  DrawStool

void DrawStool (Rect *theRect, SInt16 down)
{
	#define		kStoolBase	304
	SInt32		grayC, dkGrayC;

	if (thisMac.isDepth == 4)
	{
		grayC = 13;
		dkGrayC = 14;
	}
	else
	{
		grayC = k8DkGray2Color;
		dkGrayC = k8DkstGrayColor;
	}

	if (theRect->bottom < kStoolBase + down)
	{
		ColorLine(backSrcMap, theRect->left + 21, theRect->bottom - 1,
				theRect->left + 21, kStoolBase + down - 1, k8DkGrayColor);
		ColorLine(backSrcMap, theRect->left + 22, theRect->bottom - 1,
				theRect->left + 22, kStoolBase + down, k8Gray2Color);
		ColorLine(backSrcMap, theRect->left + 23, theRect->bottom - 1,
				theRect->left + 23, kStoolBase + down, k8DkGrayColor);
		ColorLine(backSrcMap, theRect->left + 24, theRect->bottom - 1,
				theRect->left + 24, kStoolBase + down, k8DkGray3Color);
		ColorLine(backSrcMap, theRect->left + 25, theRect->bottom - 1,
				theRect->left + 25, kStoolBase + down, grayC);
		ColorLine(backSrcMap, theRect->left + 26, theRect->bottom - 1,
				theRect->left + 26, kStoolBase + down - 1, dkGrayC);
	}

	Mac_CopyMask(furnitureSrcMap, furnitureMaskMap, backSrcMap,
			&srcRects[kStool], &srcRects[kStool], theRect);
}

//--------------------------------------------------------------  DrawInvisObstacle

void DrawInvisObstacle (Rect *theRect)
{
	ColorFrameRect(backSrcMap, theRect, k8BrownColor);
}

//--------------------------------------------------------------  DrawInvisBounce

void DrawInvisBounce (Rect *theRect)
{
	ColorFrameRect(backSrcMap, theRect, k8RedColor);
}

//--------------------------------------------------------------  DrawRedClock

void DrawRedClock (Rect *theRect)
{
	SYSTEMTIME	localTime;
	Rect		dest;
	SInt16		hour, minutes;

	Mac_CopyMask(bonusSrcMap, bonusMaskMap, backSrcMap,
			&srcRects[kRedClock], &srcRects[kRedClock], theRect);

	GetLocalTime(&localTime);
	hour = localTime.wHour % 12;
	if (hour == 0)
		hour = 12;
	minutes = localTime.wMinute;

	QSetRect(&dest, 0, 0, 4, 6);
	QOffsetRect(&dest, theRect->left + 5, theRect->top + 7);
	if (hour > 9)
		DrawClockDigit(hour / 10, &dest);
	QOffsetRect(&dest, 4, 0);
	DrawClockDigit(hour % 10, &dest);
	QOffsetRect(&dest, 6, 0);
	DrawClockDigit(minutes / 10, &dest);
	QOffsetRect(&dest, 4, 0);
	DrawClockDigit(minutes % 10, &dest);
}

//--------------------------------------------------------------  DrawClockDigit

void DrawClockDigit (SInt16 number, Rect *dest)
{
	Mac_CopyBits(bonusSrcMap, backSrcMap,
			&digits[number], dest, srcCopy, nil);
}

//--------------------------------------------------------------  DrawBlueClock

void DrawBlueClock (Rect *theRect)
{
	SYSTEMTIME	localTime;
	Point		dest;
	SInt16		hour, minutes;

	Mac_CopyMask(bonusSrcMap, bonusMaskMap, backSrcMap,
			&srcRects[kBlueClock], &srcRects[kBlueClock], theRect);

	dest.h = theRect->left + 13;
	dest.v = theRect->top + 13;
	GetLocalTime(&localTime);
	hour = localTime.wHour % 12;
	minutes = ((localTime.wMinute + 2) / 5) % 12;
	DrawClockHands(dest, minutes, hour);
}

//--------------------------------------------------------------  DrawYellowClock

void DrawYellowClock (Rect *theRect)
{
	SYSTEMTIME	localTime;
	Point		dest;
	SInt16		hour, minutes;

	Mac_CopyMask(bonusSrcMap, bonusMaskMap, backSrcMap,
			&srcRects[kYellowClock], &srcRects[kYellowClock], theRect);

	dest.h = theRect->left + 13;
	dest.v = theRect->top + 15;
	GetLocalTime(&localTime);
	hour = localTime.wHour % 12;
	minutes = ((localTime.wMinute + 2) / 5) % 12;
	DrawClockHands(dest, minutes, hour);
}

//--------------------------------------------------------------  DrawCuckoo

void DrawCuckoo (Rect *theRect)
{
	SYSTEMTIME	localTime;
	Point		dest;
	SInt16		hour, minutes;

	Mac_CopyMask(bonusSrcMap, bonusMaskMap, backSrcMap,
			&srcRects[kCuckoo], &srcRects[kCuckoo], theRect);

	dest.h = theRect->left + 19;
	dest.v = theRect->top + 31;
	GetLocalTime(&localTime);
	hour = localTime.wHour % 12;
	minutes = ((localTime.wMinute + 2) / 5) % 12;
	DrawLargeClockHands(dest, minutes, hour);
}

//--------------------------------------------------------------  DrawClockHands

void DrawClockHands (Point where, SInt16 bigHand, SInt16 littleHand)
{
	HGDIOBJ		wasPen;

	wasPen = SelectObject(backSrcMap, GetStockObject(BLACK_PEN));

	MoveToEx(backSrcMap, where.h, where.v, NULL);
	switch (bigHand)
	{
		case 0:
		Mac_Line(backSrcMap, 0, -6);
		break;

		case 1:
		Mac_Line(backSrcMap, 3, -5);
		break;

		case 2:
		Mac_Line(backSrcMap, 5, -3);
		break;

		case 3:
		Mac_Line(backSrcMap, 6, 0);
		break;

		case 4:
		Mac_Line(backSrcMap, 5, 3);
		break;

		case 5:
		Mac_Line(backSrcMap, 3, 5);
		break;

		case 6:
		Mac_Line(backSrcMap, 0, 6);
		break;

		case 7:
		Mac_Line(backSrcMap, -3, 5);
		break;

		case 8:
		Mac_Line(backSrcMap, -5, 3);
		break;

		case 9:
		Mac_Line(backSrcMap, -6, 0);
		break;

		case 10:
		Mac_Line(backSrcMap, -5, -3);
		break;

		case 11:
		Mac_Line(backSrcMap, -3, -5);
		break;
	}

	MoveToEx(backSrcMap, where.h, where.v, NULL);
	switch (littleHand)
	{
		case 0:
		Mac_Line(backSrcMap, 0, -4);
		break;

		case 1:
		Mac_Line(backSrcMap, 2, -3);
		break;

		case 2:
		Mac_Line(backSrcMap, 3, -2);
		break;

		case 3:
		Mac_Line(backSrcMap, 4, 0);
		break;

		case 4:
		Mac_Line(backSrcMap, 3, 2);
		break;

		case 5:
		Mac_Line(backSrcMap, 2, 3);
		break;

		case 6:
		Mac_Line(backSrcMap, 0, 4);
		break;

		case 7:
		Mac_Line(backSrcMap, -2, 3);
		break;

		case 8:
		Mac_Line(backSrcMap, -3, 2);
		break;

		case 9:
		Mac_Line(backSrcMap, -4, 0);
		break;

		case 10:
		Mac_Line(backSrcMap, -3, -2);
		break;

		case 11:
		Mac_Line(backSrcMap, -2, -3);
		break;
	}

	SelectObject(backSrcMap, wasPen);
}

//--------------------------------------------------------------  DrawClockHands

void DrawLargeClockHands (Point where, SInt16 bigHand, SInt16 littleHand)
{
	HGDIOBJ		wasPen;

	wasPen = SelectObject(backSrcMap, GetStockObject(WHITE_PEN));

	MoveToEx(backSrcMap, where.h, where.v, NULL);
	switch (bigHand)
	{
		case 0:
		Mac_Line(backSrcMap, 0, -10);
		break;

		case 1:
		Mac_Line(backSrcMap, 5, -9);
		break;

		case 2:
		Mac_Line(backSrcMap, 9, -5);
		break;

		case 3:
		Mac_Line(backSrcMap, 10, 0);
		break;

		case 4:
		Mac_Line(backSrcMap, 9, 5);
		break;

		case 5:
		Mac_Line(backSrcMap, 5, 9);
		break;

		case 6:
		Mac_Line(backSrcMap, 0, 10);
		break;

		case 7:
		Mac_Line(backSrcMap, -5, 9);
		break;

		case 8:
		Mac_Line(backSrcMap, -9, 5);
		break;

		case 9:
		Mac_Line(backSrcMap, -10, 0);
		break;

		case 10:
		Mac_Line(backSrcMap, -9, -5);
		break;

		case 11:
		Mac_Line(backSrcMap, -5, -9);
		break;
	}

	MoveToEx(backSrcMap, where.h, where.v, NULL);
	switch (littleHand)
	{
		case 0:
		Mac_Line(backSrcMap, 0, -6);
		break;

		case 1:
		Mac_Line(backSrcMap, 3, -5);
		break;

		case 2:
		Mac_Line(backSrcMap, 5, -3);
		break;

		case 3:
		Mac_Line(backSrcMap, 6, 0);
		break;

		case 4:
		Mac_Line(backSrcMap, 5, 3);
		break;

		case 5:
		Mac_Line(backSrcMap, 3, 5);
		break;

		case 6:
		Mac_Line(backSrcMap, 0, 6);
		break;

		case 7:
		Mac_Line(backSrcMap, -3, 5);
		break;

		case 8:
		Mac_Line(backSrcMap, -5, 3);
		break;

		case 9:
		Mac_Line(backSrcMap, -6, 0);
		break;

		case 10:
		Mac_Line(backSrcMap, -5, -3);
		break;

		case 11:
		Mac_Line(backSrcMap, -3, -5);
		break;
	}

	SelectObject(backSrcMap, wasPen);
}

//--------------------------------------------------------------  DrawSimplePrizes

void DrawSimplePrizes (SInt16 what, Rect *theRect)
{
	Mac_CopyMask(bonusSrcMap, bonusMaskMap, backSrcMap,
			&srcRects[what], &srcRects[what], theRect);
}

//--------------------------------------------------------------  DrawGreaseRt

void DrawGreaseRt (Rect *theRect, SInt16 distance, Boolean state)
{
	Rect		spill, dest;

	dest = *theRect;
	if (state)		// grease upright
	{
		Mac_CopyMask(bonusSrcMap, bonusMaskMap, backSrcMap,
				&greaseSrcRt[0], &greaseSrcRt[0], &dest);
	}
	else			// grease spilled
	{
		QOffsetRect(&dest, 6, 0);
		Mac_CopyMask(bonusSrcMap, bonusMaskMap, backSrcMap,
				&greaseSrcRt[3], &greaseSrcRt[3], &dest);

		QSetRect(&spill, 0, -2, distance - 5, 0);
		QOffsetRect(&spill, dest.right - 1, dest.bottom);
		Mac_PaintRect(backSrcMap, &spill, GetStockObject(BLACK_BRUSH));
	}
}

//--------------------------------------------------------------  DrawGreaseLf

void DrawGreaseLf (Rect *theRect, SInt16 distance, Boolean state)
{
	Rect		spill, dest;

	dest = *theRect;
	if (state)		// grease upright
	{
		Mac_CopyMask(bonusSrcMap, bonusMaskMap, backSrcMap,
				&greaseSrcLf[0], &greaseSrcLf[0], &dest);
	}
	else			// grease spilled
	{
		QOffsetRect(&dest, -6, 0);
		Mac_CopyMask(bonusSrcMap, bonusMaskMap, backSrcMap,
				&greaseSrcLf[3], &greaseSrcLf[3], &dest);

		QSetRect(&spill, -distance + 5, -2, 0, 0);
		QOffsetRect(&spill, dest.left + 1, dest.bottom);
		Mac_PaintRect(backSrcMap, &spill, GetStockObject(BLACK_BRUSH));
	}
}

//--------------------------------------------------------------  DrawBands

void DrawFoil (Rect *theRect)
{
	Mac_CopyMask(bonusSrcMap, bonusMaskMap, backSrcMap,
			&srcRects[kFoil], &srcRects[kFoil], theRect);
}

//--------------------------------------------------------------  DrawInvisBonus

void DrawInvisBonus (Rect *theRect)
{
	ColorFrameOval(backSrcMap, theRect, 227);
}

//--------------------------------------------------------------  DrawSlider

void DrawSlider (Rect *theRect)
{
	Mac_FrameRect(backSrcMap, theRect, GetStockObject(BLACK_BRUSH), 1, 1);
}

