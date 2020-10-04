#include "RectUtils.h"

//============================================================================
//----------------------------------------------------------------------------
//                                RectUtils.c
//----------------------------------------------------------------------------
//============================================================================


//==============================================================  Functions
//--------------------------------------------------------------  NormalizeRect
// This function ensures that a rect's top is less than it's bottom…
// and that left is less than right.

void NormalizeRect (Rect *theRect)
{
	SInt16 tempSide;

	if (theRect->left > theRect->right)
	{
		tempSide = theRect->left;
		theRect->left = theRect->right;
		theRect->right = tempSide;
	}

	if (theRect->top > theRect->bottom)
	{
		tempSide = theRect->top;
		theRect->top = theRect->bottom;
		theRect->bottom = tempSide;
	}
}

//--------------------------------------------------------------  ZeroRectCorner
// The rect passed in is slid over so that its top left corner is…
// at coordinates (0, 0).

void ZeroRectCorner (Rect *theRect)		// Offset rect to (0, 0)
{
	theRect->right -= theRect->left;
	theRect->bottom -= theRect->top;
	theRect->left = 0;
	theRect->top = 0;
}

//--------------------------------------------------------------  CenterRectOnPoint
// Given a rectangle and a point, this function centers the rectangle…
// on that point.

void CenterRectOnPoint (Rect *theRect, Point where)
{
	ZeroRectCorner(theRect);
	QOffsetRect(theRect, -HalfRectWide(theRect), -HalfRectTall(theRect));
	QOffsetRect(theRect, where.h, where.v);
}

//--------------------------------------------------------------  HalfRectWide
// Given a rectangle, this function returns the rect's width divided by 2.

SInt16 HalfRectWide (const Rect *theRect)
{
	return ((theRect->right - theRect->left) / 2);
}

//--------------------------------------------------------------  HalfRectTall
// Given a rectangle, this function returns the rect's height divided by 2.

SInt16 HalfRectTall (const Rect *theRect)
{
	return ((theRect->bottom - theRect->top) / 2);
}

//--------------------------------------------------------------  RectWide
// Given a rectangle, this simple function returns the rect's width.

SInt16 RectWide (const Rect *theRect)
{
	return (theRect->right - theRect->left);
}

//--------------------------------------------------------------  RectTall
// Given a rectangle, this simple function returns the rect's height.

SInt16 RectTall (const Rect *theRect)
{
	return (theRect->bottom - theRect->top);
}

//--------------------------------------------------------------  CenterRectInRect
// Given two rectangles, this function centers the first rectangle…
// within the second.  The second rect is unchanged.

void CenterRectInRect (Rect *rectA, const Rect *rectB)
{
	SInt16 widthA, tallA;

	widthA = RectWide(rectA);
	tallA = RectTall(rectA);

	rectA->left = rectB->left + (RectWide(rectB) - widthA) / 2;
	rectA->right = rectA->left + widthA;

	rectA->top = rectB->top + (RectTall(rectB) - tallA) / 2;
	rectA->bottom = rectA->top + tallA;
}

//--------------------------------------------------------------  HOffsetRect
// Just a simple function to offset a rectangle horizontally only.

void HOffsetRect (Rect *theRect, SInt16 h)
{
	theRect->left += h;
	theRect->right += h;
}

//--------------------------------------------------------------  VOffsetRect
// Just a simple function to offset a rectangle vertically only.

void VOffsetRect (Rect *theRect, SInt16 v)
{
	theRect->top += v;
	theRect->bottom += v;
}

//--------------------------------------------------------------  IsRectLeftOfRect
// Given two rects, this function returns true if the first rectangle…
// is to the left of the second.

Boolean IsRectLeftOfRect (const Rect *rect1, const Rect *rect2)
{
	SInt16 offset;

	offset = (rect1->right - rect1->left) - (rect2->right - rect2->left) / 2;
	if ((rect1->left) < (rect2->left + offset))
		return (true);
	else
		return (false);
}

//--------------------------------------------------------------  QOffsetRect
// This duplicates a Mac ToolBox call, but since it's local, it's faster.
// It offsets a rectangle both vertically and horizontally.

void QOffsetRect (Rect *theRect, SInt16 h, SInt16 v)
{
	theRect->right += h;
	theRect->left += h;
	theRect->bottom += v;
	theRect->top += v;
}

//--------------------------------------------------------------  QSetRect
// This also duplicates a ToolBox call.  It's needed often though, so…
// any gains in speed are nice.  It sets up a rect structure.

void QSetRect (Rect *theRect, SInt16 l, SInt16 t, SInt16 r, SInt16 b)
{
	theRect->left = l;
	theRect->top = t;
	theRect->right = r;
	theRect->bottom = b;
}

//--------------------------------------------------------------  ForceRectInRect
// Given a source rectangle and a bounding rectangle, this function…
// will clip the source rect so that it is entirely within the bounding…
// rect.  It returns true if any clippiung was necessary.

Boolean ForceRectInRect (Rect *small_, const Rect *large)
{
	SInt16 hOff, vOff;
	Boolean changed;

	changed = false;

	NormalizeRect(small_);

	if ((small_->bottom - small_->top) > (large->bottom - large->top))
	{
		small_->bottom = small_->top + (large->bottom - large->top);
		changed = true;
	}

	if ((small_->right - small_->left) > (large->right - large->left))
	{
		small_->right = small_->left + (large->right - large->left);
		changed = true;
	}

	hOff = large->left - small_->left;
	if (hOff > 0)
	{
		QOffsetRect(small_, hOff, 0);
		changed = true;
	}
	hOff = large->right - small_->right;
	if (hOff < 0)
	{
		QOffsetRect(small_, hOff, 0);
		changed = true;
	}
	vOff = large->top - small_->top;
	if (vOff > 0)
	{
		QOffsetRect(small_, 0, vOff);
		changed = true;
	}
	vOff = large->bottom - small_->bottom;
	if (vOff < 0)
	{
		QOffsetRect(small_, 0, vOff);
		changed = true;
	}

	return changed;
}

//--------------------------------------------------------------  QUnionSimilarRect
// Given 2 rects that are assumed to have the same width and height,…
// this function returns a 3rd rect that is the union of those two.

void QUnionSimilarRect (const Rect *rectA, const Rect *rectB, Rect *rectC)
{
	if (rectA->left < rectB->left)
		rectC->left = rectA->left;
	else
		rectC->left = rectB->left;

	if (rectA->top < rectB->top)
		rectC->top = rectA->top;
	else
		rectC->top = rectB->top;

	if (rectA->right > rectB->right)
		rectC->right = rectA->right;
	else
		rectC->right = rectB->right;

	if (rectA->bottom > rectB->bottom)
		rectC->bottom = rectA->bottom;
	else
		rectC->bottom = rectB->bottom;
}

//--------------------------------------------------------------  QInsetRect
// Shrink or expand the given rectangle. The rectangle's sides
// are moved inwards by the given deltas. To expand the rectangle,
// call this function with negative delta values. No correction is
// applied if the resulting rectangle has a negative width or height.

void QInsetRect (Rect *r, SInt16 dh, SInt16 dv)
{
	if (r == NULL)
		return;

	r->left += dh;
	r->top += dv;
	r->right -= dh;
	r->bottom -= dv;
}

//--------------------------------------------------------------  QPtInRect
// Return whether the point is enclosed within the rectangle's bounds.

Boolean QPtInRect (Point pt, const Rect *r)
{
	if (r == NULL)
		return false;
	return (pt.h >= r->left) && (pt.h < r->right) &&
		(pt.v >= r->top) && (pt.v < r->bottom);
}

//--------------------------------------------------------------  QSectRect
// Calculate the intersection of the two source rectangles, and write
// that intersection into the destination rectangle. If the source
// rectangles do not intersect, then the destination rectangle is set
// to all zeroes. The return value is TRUE if the source rectangles
// intersect, and FALSE if they do not intersect.

Boolean QSectRect (const Rect *src1, const Rect *src2, Rect *dstRect)
{
	Rect sectRect;

	sectRect.left = 0;
	sectRect.top = 0;
	sectRect.right = 0;
	sectRect.bottom = 0;
	// If either source rectangle is empty, then the intersection is empty.
	if ((src1->left >= src1->right) || (src1->top >= src1->bottom) ||
		(src2->left >= src2->right) || (src2->top >= src2->bottom))
	{
		*dstRect = sectRect;
		return false;
	}
	// If the first rectangle is above, below, or to the side of the second
	// rectangle (touching or not), then the intersection is empty.
	if ((src1->bottom <= src2->top) || (src2->bottom <= src1->top) ||
		(src1->right <= src2->left) || (src2->right <= src1->left))
	{
		*dstRect = sectRect;
		return false;
	}
	// The two rectangles intersect, so calculate the intersection's boundary.
	sectRect.left = (src1->left > src2->left) ? src1->left : src2->left;
	sectRect.top = (src1->top > src2->top) ? src1->top : src2->top;
	sectRect.right = (src1->right <= src2->right) ? src1->right : src2->right;
	sectRect.bottom = (src1->bottom <= src2->bottom) ? src1->bottom : src2->bottom;
	*dstRect = sectRect;
	return true;
}

