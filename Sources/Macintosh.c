#include "Macintosh.h"

//--------------------------------------------------------------  InsetRect
// Shrink or expand the given rectangle. The rectangle's sides
// are moved inwards by the given deltas. To expand the rectangle,
// call this function with negative delta values. If either the width
// or the height of the resulting rectangle is less than 1, then the
// rectangle is set to the empty rectangle (0,0,0,0).

void Mac_InsetRect(Rect *r, SInt16 dh, SInt16 dv)
{
	if (r == NULL)
		return;
	r->left += dh;
	r->top += dv;
	r->right -= dh;
	r->bottom -= dv;
	if (r->left > r->right || r->top > r->bottom)
	{
		r->left = 0;
		r->top = 0;
		r->right = 0;
		r->bottom = 0;
	}
}
