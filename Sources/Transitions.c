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

#define kNumWipeFrames 12
#define kNumDissolveSlowFrames 48
#define kNumDissolveFastFrames 24

Rect MakeWipeRect (SInt16 direction, const Rect *theRect, SInt16 index, SInt16 count);
SInt16 SInt16_RoundToPrevMultiple (SInt16 number, SInt16 step);
SInt16 SInt16_RoundToNextMultiple (SInt16 number, SInt16 step);
Boolean BlitChunk (RGBQUAD *destData, const RGBQUAD *srcData, const Rect *bitmapRect,
	SInt16 chunkX, SInt16 chunkY, UInt16 chunkWidth, UInt16 chunkHeight);
void DissBitsImpl (const Rect *theRect, Boolean doChunky);
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
		mainWindowDC = GetMainWindowDC(g_mainWindow);
		Mac_CopyBits(g_workSrcMap, mainWindowDC,
				&wipeRect, &wipeRect, srcCopy, nil);
		ReleaseMainWindowDC(g_mainWindow, mainWindowDC);
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

	mainWindowDC = GetMainWindowDC(g_mainWindow);
	Mac_CopyBits(g_workSrcMap, mainWindowDC,
			theRect, theRect, srcCopy, nil);
	ReleaseMainWindowDC(g_mainWindow, mainWindowDC);
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

//--------------------------------------------------------------  SInt16_RoundToPrevMultiple
// Round 'number' to the nearest multiple of 'step' that is less than 'number'.
// If 'number' is already a multiple of 'step', then 'number' is returned.

SInt16 SInt16_RoundToPrevMultiple (SInt16 number, SInt16 step)
{
	SInt16 result;

	result = number / step * step;
	if (result > number)
	{
		result -= step;
	}
	return result;
}

//--------------------------------------------------------------  SInt16_RoundToNextMultiple
// Round 'number' to the nearest multiple of 'step' that is greater than 'number'.
// If 'number' is already a multiple of 'step', then 'number' is returned.

SInt16 SInt16_RoundToNextMultiple (SInt16 number, SInt16 step)
{
	SInt16 result;

	result = number / step * step;
	if (result < number)
	{
		result += step;
	}
	return result;
}

//--------------------------------------------------------------  BlitChunk

Boolean BlitChunk (RGBQUAD *destData, const RGBQUAD *srcData, const Rect *bitmapRect,
	SInt16 chunkX, SInt16 chunkY, UInt16 chunkWidth, UInt16 chunkHeight)
{
	SInt16 bitmapWidth;
	SInt16 bitmapHeight;
	size_t chunkRowByteSize;
	size_t dataOffset;
	UInt16 currentY;

	chunkX -= bitmapRect->left;
	chunkY -= bitmapRect->top;
	bitmapWidth = bitmapRect->right - bitmapRect->left;
	bitmapHeight = bitmapRect->bottom - bitmapRect->top;
	if (chunkX < 0 || chunkX > (bitmapWidth - chunkWidth))
	{
		return false;
	}
	if (chunkY < 0 || chunkY > (bitmapHeight - chunkHeight))
	{
		return false;
	}

	chunkRowByteSize = sizeof(RGBQUAD) * (size_t)chunkWidth;
	dataOffset = ((size_t)chunkY * (size_t)bitmapWidth) + (size_t)chunkX;
	for (currentY = 0; currentY < chunkHeight; currentY++)
	{
		CopyMemory(&destData[dataOffset], &srcData[dataOffset], chunkRowByteSize);
		dataOffset += (size_t)bitmapWidth;
	}
	return true;
}

//--------------------------------------------------------------  DissBitsImpl

void DissBitsImpl (const Rect *theRect, Boolean doChunky)
{
	UInt32 lfsrMask;
	SInt16 chunkSize;
	SInt32 totalFrames;
	Rect bitmapRect = { 0 };
	SInt16 bitmapWidth;
	SInt16 bitmapHeight;
	SInt32 totalChunks;
	Rect externalRect = { 0 };
	Rect internalRect = { 0 };
	BITMAPINFO bmi;
	HDC tempDC;
	void *tempPtr;
	HBITMAP srcBitmap;
	RGBQUAD *srcData;
	HBITMAP destBitmap;
	RGBQUAD *destData;
	HBITMAP prevBitmap;
	HDC mainWindowDC;
	DWORD prevFrameRate;
	SInt32 framesBlitted;
	SInt32 chunksBlitted;
	UInt32 lfsrState;
	UInt16 chunkX;
	UInt16 chunkY;
	Boolean didBlit;

	if (theRect->left >= theRect->right || theRect->top >= theRect->bottom)
	{
		return;
	}

	if (doChunky)
	{
		lfsrMask = 0x4016; // 1 to 32767 (2^15 - 1)
		chunkSize = 8;
		totalFrames = kNumDissolveFastFrames;
	}
	else
	{
		lfsrMask = 0x10016; // 1 to 131071 (2^17 - 1)
		chunkSize = 4;
		totalFrames = kNumDissolveSlowFrames;
	}

	// The rectangle is expanded as necessary so that each side is a multiple of
	// the chunk size. This allows the dissolve animation to be aligned to the
	// origin point in the main window, instead of aligned to the top-left corner
	// of the output rectangle. This also makes it far easier to calculate the
	// total number of chunks to be copied to the main window.
	bitmapRect.left = SInt16_RoundToPrevMultiple(theRect->left, chunkSize);
	bitmapRect.top = SInt16_RoundToPrevMultiple(theRect->top, chunkSize);
	bitmapRect.right = SInt16_RoundToNextMultiple(theRect->right, chunkSize);
	bitmapRect.bottom = SInt16_RoundToNextMultiple(theRect->bottom, chunkSize);
	bitmapWidth = bitmapRect.right - bitmapRect.left;
	bitmapHeight = bitmapRect.bottom - bitmapRect.top;

	totalChunks = ((SInt32)bitmapWidth / chunkSize) * ((SInt32)bitmapHeight / chunkSize);
	externalRect = *theRect;
	internalRect = *theRect;
	QOffsetRect(&internalRect, -bitmapRect.left, -bitmapRect.top);

	ZeroMemory(&bmi, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = bitmapWidth;
	bmi.bmiHeader.biHeight = -bitmapHeight; // negative for top-down DIB section
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = 1;
	bmi.bmiHeader.biXPelsPerMeter = 0;
	bmi.bmiHeader.biYPelsPerMeter = 0;
	bmi.bmiHeader.biClrUsed = 0;
	bmi.bmiHeader.biClrImportant = 0;

	tempDC = CreateCompatibleDC(NULL);
	if (tempDC == NULL)
	{
		DumpScreenOn(theRect);
		return;
	}

	srcBitmap = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &tempPtr, NULL, 0);
	if (srcBitmap == NULL)
	{
		DeleteDC(tempDC);
		DumpScreenOn(theRect);
		return;
	}
	srcData = (RGBQUAD *)tempPtr;

	destBitmap = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &tempPtr, NULL, 0);
	if (destBitmap == NULL)
	{
		DeleteBitmap(srcBitmap);
		DeleteDC(tempDC);
		DumpScreenOn(theRect);
		return;
	}
	destData = (RGBQUAD *)tempPtr;

	prevBitmap = SelectBitmap(tempDC, srcBitmap);
	Mac_CopyBits(g_workSrcMap, tempDC, &externalRect, &internalRect, srcCopy, nil);
	SelectBitmap(tempDC, prevBitmap);

	prevBitmap = SelectBitmap(tempDC, destBitmap);
	mainWindowDC = GetMainWindowDC(g_mainWindow);
	Mac_CopyBits(mainWindowDC, tempDC, &externalRect, &internalRect, srcCopy, nil);
	ReleaseMainWindowDC(g_mainWindow, mainWindowDC);
	SelectBitmap(tempDC, prevBitmap);

	prevFrameRate = GetFrameRate();
	SetFrameRate(60);

	framesBlitted = 0;
	chunksBlitted = 0;
	lfsrState = 1;
	do
	{
		if (lfsrState & 1)
		{
			lfsrState = (lfsrState >> 1) ^ lfsrMask;
		}
		else
		{
			lfsrState = (lfsrState >> 1);
		}

		if (doChunky)
		{
			chunkX = chunkSize * (lfsrState & 0xFF); // (0 to 255)
			chunkY = chunkSize * ((lfsrState >> 8) & 0x7F); // (0 to 127)
		}
		else
		{
			chunkX = chunkSize * (lfsrState & 0x1FF); // (0 to 511)
			chunkY = chunkSize * ((lfsrState >> 9) & 0xFF); // (0 to 255)
		}
		didBlit = BlitChunk(destData, srcData, &bitmapRect, chunkX, chunkY, chunkSize, chunkSize);
		if (didBlit)
		{
			chunksBlitted += 1;
			if (chunksBlitted == ((framesBlitted + 1) * totalChunks / totalFrames))
			{
				GdiFlush();
				prevBitmap = SelectBitmap(tempDC, destBitmap);
				mainWindowDC = GetMainWindowDC(g_mainWindow);
				Mac_CopyBits(tempDC, mainWindowDC, &internalRect, &externalRect, srcCopy, nil);
				ReleaseMainWindowDC(g_mainWindow, mainWindowDC);
				SelectBitmap(tempDC, prevBitmap);
				GdiFlush();
				WaitUntilNextFrame();
				framesBlitted += 1;
			}
		}
	} while (lfsrState != 1);
	BlitChunk(destData, srcData, &bitmapRect, 0, 0, chunkSize, chunkSize);
	GdiFlush();
	prevBitmap = SelectBitmap(tempDC, destBitmap);
	mainWindowDC = GetMainWindowDC(g_mainWindow);
	Mac_CopyBits(tempDC, mainWindowDC, &internalRect, &externalRect, srcCopy, nil);
	ReleaseMainWindowDC(g_mainWindow, mainWindowDC);
	SelectBitmap(tempDC, prevBitmap);
	GdiFlush();

	SetFrameRate(prevFrameRate);

	DeleteBitmap(destBitmap);
	DeleteBitmap(srcBitmap);
	DeleteDC(tempDC);
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
