
//============================================================================
//----------------------------------------------------------------------------
//								 Transitions.c
//----------------------------------------------------------------------------
//============================================================================


#include "Environ.h"
#include "Externs.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "RectUtils.h"


//==============================================================  Functions
//--------------------------------------------------------------  PourScreenOn

void PourScreenOn (Rect *theRect)
{
	#define		kMaxColumnsWide	96
	#define		kChipHigh		20
	#define		kChipWide		16
	Rect		columnRects[kMaxColumnsWide];
	SInt16		columnProgress[kMaxColumnsWide];
	SInt16		i, colsComplete, colWide, rowTall;
	Boolean		working;
	HDC			mainWindowDC;

	colWide = theRect->right / kChipWide;			// determine # of cols
	rowTall = (theRect->bottom / kChipHigh) + 1;	// determine # of rows

	working = true;
	colsComplete = 0;
	for (i = 0; i < colWide; i++)
	{
		columnProgress[i] = 0;
		QSetRect(&columnRects[i], 0, 0, kChipWide, kChipHigh);
		QOffsetRect(&columnRects[i], (i * kChipWide) + theRect->left, theRect->top);
	}

	mainWindowDC = GetMainWindowDC();
	while (working)
	{
		do
		{
			i = RandomInt(colWide);
		}
		while (columnProgress[i] >= rowTall);

		if (columnRects[i].left < theRect->left)
			columnRects[i].left = theRect->left;
		if (columnRects[i].top < theRect->top)
			columnRects[i].top = theRect->top;
		if (columnRects[i].right > theRect->right)
			columnRects[i].right = theRect->right;
		if (columnRects[i].bottom > theRect->bottom)
			columnRects[i].bottom = theRect->bottom;

		Mac_CopyBits(workSrcMap, mainWindowDC,
				&columnRects[i], &columnRects[i], srcCopy, nil);

		QOffsetRect(&columnRects[i], 0, kChipHigh);
		columnProgress[i]++;
		if (columnProgress[i] >= rowTall)
		{
			colsComplete++;
			if (colsComplete >= colWide)
				working = false;
		}
	}
	ReleaseMainWindowDC(mainWindowDC);
}

//--------------------------------------------------------------  WipeScreenOn

void WipeScreenOn (SInt16 direction, Rect *theRect)
{
	#define		kWipeRectThick	4
	Rect		wipeRect;
	SInt16		hOffset, vOffset;
	SInt16		i, count;
	HDC			mainWindowDC;

	wipeRect = *theRect;
	switch (direction)
	{
		case kAbove:
		wipeRect.bottom = wipeRect.top + kWipeRectThick;
		hOffset = 0;
		vOffset = kWipeRectThick;
		count = ((theRect->bottom - theRect->top) / kWipeRectThick) + 1;
		break;

		case kToRight:
		wipeRect.left = wipeRect.right - kWipeRectThick;
		hOffset = -kWipeRectThick;
		vOffset = 0;
		count = workSrcRect.right / kWipeRectThick;
		break;

		case kBelow:
		wipeRect.top = wipeRect.bottom - kWipeRectThick;
		hOffset = 0;
		vOffset = -kWipeRectThick;
		count = ((theRect->bottom - theRect->top) / kWipeRectThick) + 1;
		break;

		case kToLeft:
		wipeRect.right = wipeRect.left + kWipeRectThick;
		hOffset = kWipeRectThick;
		vOffset = 0;
		count = workSrcRect.right / kWipeRectThick;
		break;

		default:
		return;
	}

	mainWindowDC = GetMainWindowDC();
	for (i = 0; i < count; i++)
	{
		Mac_CopyBits(workSrcMap, mainWindowDC,
				&wipeRect, &wipeRect, srcCopy, nil);

		QOffsetRect(&wipeRect, hOffset, vOffset);

		if (wipeRect.top < theRect->top)
			wipeRect.top = theRect->top;
		else if (wipeRect.top > theRect->bottom)
			wipeRect.top = theRect->bottom;
		if (wipeRect.bottom < theRect->top)
			wipeRect.bottom = theRect->top;
		else if (wipeRect.bottom > theRect->bottom)
			wipeRect.bottom = theRect->bottom;
	}
	ReleaseMainWindowDC(mainWindowDC);
}

//--------------------------------------------------------------  DumpScreenOn

void DumpScreenOn (Rect *theRect)
{
	HDC			mainWindowDC;

	mainWindowDC = GetMainWindowDC();
	Mac_CopyBits(workSrcMap, mainWindowDC,
			theRect, theRect, srcCopy, nil);
	ReleaseMainWindowDC(mainWindowDC);
}

// The functions DissBits and DissBitsChunky were originally missing,
// but I found some implementations in other source code releases from
// softdorothy:
//
// * https://github.com/softdorothy/glider_4/blob/master/Glider_405/Sources/G-GlobalUtils.p
//   - procedure DissBlocks (total area 512 x 512, chunk 16 x 16)
// * https://github.com/softdorothy/pararena_2/blob/master/Sources/DissBits.c
//   - function DissBits13QD (total area 640 x 512, chunk 8 x 10)
//   - function DissBits12QD (total area 512 x 384, chunk 8 x 12)
//
// The general technique used in these routines is to generate a pseudo-random
// sequence of numbers from one up to (but not including) a power of two.
// Each number is converted into coordinates, and those coordinates correspond
// to a chunk of pixels which is blitted to the destination. The special case
// of zero is dealt with separately, in the same fashion. In this way, the
// screen dissolves from one state to another by blitting pixel chunks in a
// seemingly random order.
//
// To generate the pseudo-random number sequence, DissBits uses a Galois
// linear-feedback shift register (LFSR) implemented as follows:
//
//   unsigned int lfsr_step(unsigned int state, unsigned int mask)
//   {
//     if (state & 1)
//       return (state >> 1) ^ mask;
//     else
//       return (state >> 1);
//   }
//
// With careful selection of the 'mask' value, the state will cycle through from
// 1 to 2^n - 1, as desired.
//
// In Glider PRO, DissBits uses 4 x 4 chunks, and DissBitsChunky uses 8 x 8 chunks.
// These chunks are used to transition only the primary rectangle of the screen,
// and not the menu bar or scoreboard. To adapt to a variable screen resolution,
// the screen is treated as if it's very large (roughly 2048 x 2048). Any
// coordinates generated that lie outside the visible area will be skipped.

//--------------------------------------------------------------  DissBits

void DissBits (Rect *theRect)
{
	UInt32		lfsrMask = 0x20013;		// 1 to 262143 (2^18 - 1)
	HDC			mainWindowDC;
	POINT		topLeft, botRight;
	HRGN		theClipRgn;
	INT			chunkH, chunkV;
	INT			chunkSize = 4;
	UInt32		state;

	mainWindowDC = GetMainWindowDC();
	SaveDC(mainWindowDC);
	topLeft.x = theRect->left;
	topLeft.y = theRect->top;
	botRight.x = theRect->right;
	botRight.y = theRect->bottom;
	LPtoDP(mainWindowDC, &topLeft, 1);
	LPtoDP(mainWindowDC, &botRight, 1);
	theClipRgn = CreateRectRgn(topLeft.x, topLeft.y, botRight.x, botRight.y);
	ExtSelectClipRgn(mainWindowDC, theClipRgn, RGN_AND);
	DeleteObject(theClipRgn);
	state = 1;
	do
	{
		if (state & 1)
			state = (state >> 1) ^ lfsrMask;
		else
			state = (state >> 1);
		chunkH = chunkSize * (state & 0x1FF); // 4 * (0 to 511)
		if ((theRect->left > chunkH + chunkSize - 1) || (chunkH >= theRect->right))
			continue;
		chunkV = chunkSize * ((state >> 9) & 0x1FF); // 4 * (0 to 511)
		if ((theRect->top > chunkV + chunkSize - 1) || (chunkV >= theRect->bottom))
			continue;
		BitBlt(mainWindowDC, chunkH, chunkV, chunkSize, chunkSize,
				workSrcMap, chunkH, chunkV, SRCCOPY);
	} while (state != 1);
	BitBlt(mainWindowDC, 0, 0, chunkSize, chunkSize, workSrcMap, 0, 0, SRCCOPY);
	RestoreDC(mainWindowDC, -1);
	ReleaseMainWindowDC(mainWindowDC);
}

//--------------------------------------------------------------  DissBitsChunky

void DissBitsChunky(Rect* theRect)
{
	UInt32		lfsrMask = 0x8016;		// 1 to 65535 (2^16 - 1)
	HDC			mainWindowDC;
	POINT		topLeft, botRight;
	HRGN		theClipRgn;
	INT			chunkH, chunkV;
	INT			chunkSize = 8;
	UInt32		state;

	mainWindowDC = GetMainWindowDC();
	SaveDC(mainWindowDC);
	topLeft.x = theRect->left;
	topLeft.y = theRect->top;
	botRight.x = theRect->right;
	botRight.y = theRect->bottom;
	LPtoDP(mainWindowDC, &topLeft, 1);
	LPtoDP(mainWindowDC, &botRight, 1);
	theClipRgn = CreateRectRgn(topLeft.x, topLeft.y, botRight.x, botRight.y);
	ExtSelectClipRgn(mainWindowDC, theClipRgn, RGN_AND);
	DeleteObject(theClipRgn);
	state = 1;
	do
	{
		if (state & 1)
			state = (state >> 1) ^ lfsrMask;
		else
			state = (state >> 1);
		chunkH = chunkSize * (state & 0xFF); // 8 * (0 to 255)
		if ((theRect->left > chunkH + chunkSize - 1) || (chunkH >= theRect->right))
			continue;
		chunkV = chunkSize * ((state >> 8) & 0xFF); // 8 * (0 to 255)
		if ((theRect->top > chunkV + chunkSize - 1) || (chunkV >= theRect->bottom))
			continue;
		BitBlt(mainWindowDC, chunkH, chunkV, chunkSize, chunkSize,
			workSrcMap, chunkH, chunkV, SRCCOPY);
	} while (state != 1);
	BitBlt(mainWindowDC, 0, 0, chunkSize, chunkSize, workSrcMap, 0, 0, SRCCOPY);
	RestoreDC(mainWindowDC, -1);
	ReleaseMainWindowDC(mainWindowDC);
}

