
//============================================================================
//----------------------------------------------------------------------------
//								 Transitions.c
//----------------------------------------------------------------------------
//============================================================================


#include "Externs.h"
#include "Environ.h"
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

	mainWindowDC = GetDC(mainWindow);
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
	ReleaseDC(mainWindow, mainWindowDC);
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
	}

	mainWindowDC = GetDC(mainWindow);
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
	ReleaseDC(mainWindow, mainWindowDC);
}

//--------------------------------------------------------------  DumpScreenOn

void DumpScreenOn (Rect *theRect)
{
	HDC			mainWindowDC;

	mainWindowDC = GetDC(mainWindow);
	Mac_CopyBits(workSrcMap, mainWindowDC,
			theRect, theRect, srcCopy, nil);
	ReleaseDC(mainWindow, mainWindowDC);
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
	#define		lfsrMask		0x20013		// 1 to 262143 (2^18 - 1)
	HDC			mainWindowDC;
	HRGN		theClipRgn;
	INT			chunkH, chunkV;
	UInt32		state;

	mainWindowDC = GetDC(mainWindow);
	SaveDC(mainWindowDC);
	theClipRgn = CreateRectRgn(theRect->left, theRect->top,
		theRect->right, theRect->bottom);
	SelectClipRgn(mainWindowDC, theClipRgn);
	DeleteObject(theClipRgn);
	state = 1;
	do
	{
		if (state & 1)
			state = (state >> 1) ^ lfsrMask;
		else
			state = (state >> 1);
		chunkH = 4 * (state & 0x1FF); // 4 * (0 to 511)
		if ((theRect->left > chunkH) || (chunkH >= theRect->right))
			continue;
		chunkV = 4 * ((state >> 9) & 0x1FF); // 4 * (0 to 511)
		if ((theRect->top > chunkV) || (chunkV >= theRect->bottom))
			continue;
		BitBlt(mainWindowDC, chunkH, chunkV, 4, 4, workSrcMap, chunkH, chunkV, SRCCOPY);
	} while (state != 1);
	BitBlt(mainWindowDC, 0, 0, 4, 4, workSrcMap, 0, 0, SRCCOPY);
	RestoreDC(mainWindowDC, -1);
	ReleaseDC(mainWindow, mainWindowDC);
}

