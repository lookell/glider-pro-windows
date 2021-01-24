//============================================================================
//----------------------------------------------------------------------------
//                               Transitions.c
//----------------------------------------------------------------------------
//============================================================================

#include "Transitions.h"

#include "FrameTimer.h"
#include "GliderDefines.h"
#include "Macintosh.h"
#include "Main.h"
#include "MainWindow.h"
#include "RectUtils.h"
#include "WinAPI.h"

#define kNumWipeFrames  12

Rect MakeWipeRect (SInt16 direction, const Rect *theRect, SInt16 index, SInt16 count);
void DissBits (const Rect *theRect);
void DissBitsChunky (const Rect *theRect);

//==============================================================  Functions
//--------------------------------------------------------------  WipeScreenOn

void WipeScreenOn (SInt16 direction, const Rect *theRect)
{
	Rect wipeRect;
	SInt16 count;
	DWORD prevFrameRate;
	SInt16 i;
	HDC mainWindowDC;

	if (theRect->left >= theRect->right || theRect->top >= theRect->bottom)
		return;

	switch (direction)
	{
	case kAbove:
	case kToRight:
	case kBelow:
	case kToLeft:
		break;

	default:
		DumpScreenOn(theRect);
		return;
	}

	count = kNumWipeFrames;
	prevFrameRate = GetFrameRate();
	SetFrameRate(60);
	for (i = 0; i < count; i++)
	{
		wipeRect = MakeWipeRect(direction, theRect, i, count);
		mainWindowDC = GetMainWindowDC();
		Mac_CopyBits(g_workSrcMap, mainWindowDC,
				&wipeRect, &wipeRect, srcCopy, nil);
		ReleaseMainWindowDC(mainWindowDC);
		WaitUntilNextFrame();
	}
	SetFrameRate(prevFrameRate);
}

//--------------------------------------------------------------  MakeWipeRect

Rect MakeWipeRect (SInt16 direction, const Rect *theRect, SInt16 index, SInt16 count)
{
	SInt16 destWidth;
	SInt16 destHeight;
	Rect wipeRect;

	destWidth = theRect->right - theRect->left;
	destHeight = theRect->bottom - theRect->top;

	switch (direction)
	{
	case kAbove:
		wipeRect.left = 0;
		wipeRect.top = index * destHeight / count;
		wipeRect.right = destWidth;
		wipeRect.bottom = (index + 1) * destHeight / count;
		break;

	case kToRight:
		wipeRect.left = (count - index - 1) * destWidth / count;
		wipeRect.top = 0;
		wipeRect.right = (count - index) * destWidth / count;
		wipeRect.bottom = destHeight;
		break;

	case kBelow:
		wipeRect.left = 0;
		wipeRect.top = (count - index - 1) * destHeight / count;
		wipeRect.right = destWidth;
		wipeRect.bottom = (count - index) * destHeight / count;
		break;

	case kToLeft:
		wipeRect.left = index * destWidth / count;
		wipeRect.top = 0;
		wipeRect.right = (index + 1) * destWidth / count;
		wipeRect.bottom = destHeight;
		break;

	default:
		wipeRect.left = 0;
		wipeRect.top = 0;
		wipeRect.right = 0;
		wipeRect.bottom = 0;
		break;
	}

	QOffsetRect(&wipeRect, theRect->left, theRect->top);
	return wipeRect;
}

//--------------------------------------------------------------  DumpScreenOn

void DumpScreenOn (const Rect *theRect)
{
	HDC mainWindowDC;

	mainWindowDC = GetMainWindowDC();
	Mac_CopyBits(g_workSrcMap, mainWindowDC,
			theRect, theRect, srcCopy, nil);
	ReleaseMainWindowDC(mainWindowDC);
}

//--------------------------------------------------------------  DissolveScreenOn

void DissolveScreenOn (const Rect *theRect)
{
	if (g_quickerTransitions)
	{
		DissBitsChunky(theRect);
	}
	else
	{
		DissBits(theRect);
	}
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
// the screen is treated as if it's very large (2048 x 1024). Any rectangles
// generated that lie outside of the visible area will be skipped.

//--------------------------------------------------------------  DissBitsImpl

void DissBitsImpl (const Rect *theRect, Boolean doChunky)
{
	UINT32 lfsrMask;
	INT chunkSize;
	HDC mainWindowDC;
	RECT clipRect;
	HRGN theClipRgn;
	INT chunkH;
	INT chunkV;
	UInt32 state;

	if (theRect->left >= theRect->right || theRect->top >= theRect->bottom)
		return;

	if (doChunky)
	{
		lfsrMask = 0x4016; // 1 to 32767 (2^15 - 1)
		chunkSize = 8;
	}
	else
	{
		lfsrMask = 0x10016; // 1 to 131071 (2^17 - 1)
		chunkSize = 4;
	}

	mainWindowDC = GetMainWindowDC();
	SaveDC(mainWindowDC);
	clipRect.left = theRect->left;
	clipRect.top = theRect->top;
	clipRect.right = theRect->right;
	clipRect.bottom = theRect->bottom;
	LPtoDP(mainWindowDC, (LPPOINT)&clipRect, 2);
	theClipRgn = CreateRectRgnIndirect(&clipRect);
	ExtSelectClipRgn(mainWindowDC, theClipRgn, RGN_AND);
	DeleteRgn(theClipRgn);

	state = 1;
	do
	{
		if (state & 1)
			state = (state >> 1) ^ lfsrMask;
		else
			state = (state >> 1);

		if (doChunky)
		{
			chunkH = chunkSize * (state & 0xFF); // 8 * (0 to 255)
			chunkV = chunkSize * ((state >> 8) & 0x7F); // 8 * (0 to 127)
		}
		else
		{
			chunkH = chunkSize * (state & 0x1FF); // 4 * (0 to 511)
			chunkV = chunkSize * ((state >> 9) & 0xFF); // 4 * (0 to 255)
		}
		if ((theRect->left > chunkH + chunkSize - 1) || (chunkH >= theRect->right))
			continue;
		if ((theRect->top > chunkV + chunkSize - 1) || (chunkV >= theRect->bottom))
			continue;
		BitBlt(mainWindowDC, chunkH, chunkV, chunkSize, chunkSize,
				g_workSrcMap, chunkH, chunkV, SRCCOPY);
	} while (state != 1);
	BitBlt(mainWindowDC, 0, 0, chunkSize, chunkSize, g_workSrcMap, 0, 0, SRCCOPY);

	RestoreDC(mainWindowDC, -1);
	ReleaseMainWindowDC(mainWindowDC);
}

//--------------------------------------------------------------  DissBits

void DissBits (const Rect *theRect)
{
	DissBitsImpl(theRect, false);
}

//--------------------------------------------------------------  DissBitsChunky

void DissBitsChunky (const Rect *theRect)
{
	DissBitsImpl(theRect, true);
}
