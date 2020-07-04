#include "AnimCursor.h"

//============================================================================
//----------------------------------------------------------------------------
//                                AnimCursor.c
//----------------------------------------------------------------------------
//============================================================================


#include "ByteIO.h"
#include "Environ.h"
#include "MacTypes.h"
#include "MainWindow.h"
#include "ResourceIDs.h"
#include "Utilities.h"
#include "WinAPI.h"


typedef struct acurRec
{
	SInt16 n;
	SInt16 index;
	union acurFrame
	{
		HCURSOR cursorHdl;
		uint16_t resID;
	} *frame;
} acurRec, *acurPtr;


Boolean GetMonoCursors (acurPtr);
Boolean GetColorCursors (acurPtr);
Boolean ReadAcurResource (WORD, acurPtr);
void InitAnimatedCursor (WORD);


static acurRec animCursor = { 0 };


//==============================================================  Functions
//--------------------------------------------------------------  GetMonoCursors

// Loads b&w cursors (for animated beach ball).

Boolean GetMonoCursors (acurPtr ballCurs)
{
	SInt16 i, j;
	HCURSOR cursHdl;

	if (ballCurs)
	{
		j = ballCurs->n;
		for (i = 0; i < j; i++)
		{
			cursHdl = (HCURSOR)LoadImage(HINST_THISCOMPONENT,
				MAKEINTRESOURCE(ballCurs->frame[i].resID),
				IMAGE_CURSOR, 0, 0, LR_MONOCHROME);
			if (cursHdl == NULL)
			{
				for (j = 0; j < i; j++)
				{
					DestroyCursor(ballCurs->frame[j].cursorHdl);
				}
				return false;
			}
			else
			{
				ballCurs->frame[i].cursorHdl = cursHdl;
			}
		}
	}
	return true;
}

//--------------------------------------------------------------  GetColorCursors

// Loads color cursors (for animated beach ball).

Boolean GetColorCursors (acurPtr ballCurs)
{
	SInt16 i, j;
	HCURSOR cursHdl;

	if (ballCurs)
	{
		j = ballCurs->n;
		for (i = 0; i < j; i++)
		{
			cursHdl = (HCURSOR)LoadImage(HINST_THISCOMPONENT,
				MAKEINTRESOURCE(ballCurs->frame[i].resID),
				IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR);
			if (cursHdl == NULL)
			{
				for (j = 0; j < i; j++)
				{
					DestroyCursor(ballCurs->frame[j].cursorHdl);
				}
				return false;
			}
			else
			{
				ballCurs->frame[i].cursorHdl = cursHdl;
			}
		}
	}
	return true;
}

//--------------------------------------------------------------  ReadAcurResource

Boolean ReadAcurResource (WORD acurID, acurPtr ballCurs)
{
	LPVOID resData;
	DWORD resSize;
	byteio resReader;
	HRESULT hr;
	uint16_t i, count, cursID;

	hr = LoadModuleResource(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(acurID), RT_ACUR, &resData, &resSize);
	if (FAILED(hr))
		return false;

	ballCurs->n = 0;
	ballCurs->index = 0;
	ballCurs->frame = NULL;

	if (!byteio_init_memory_reader(&resReader, resData, resSize))
		return false;
	if (!byteio_read_le_u16(&resReader, &count))
		goto failed;
	ballCurs->n = count;
	ballCurs->frame = calloc(count, sizeof(*ballCurs->frame));
	if (ballCurs->frame == NULL)
		goto failed;
	for (i = 0; i < count; i++)
	{
		if (!byteio_read_le_u16(&resReader, &cursID))
			goto failed;
		ballCurs->frame[i].resID = cursID;
	}

	byteio_close(&resReader);
	return true;

failed:
	ballCurs->n = 0;
	ballCurs->index = 0;
	free(ballCurs->frame);
	ballCurs->frame = NULL;
	byteio_close(&resReader);
	return false;
}

//--------------------------------------------------------------  InitAnimatedCursor

// Loads and sets up animated beach ball cursor structures.

void InitAnimatedCursor (WORD acurID)
{
	acurRec ballCurs = { 0 };
	Boolean useColor;

	useColor = thisMac.hasColor;

	if (!ReadAcurResource(acurID, &ballCurs))
		RedAlert(kErrFailedResourceLoad);
	if (useColor)
		useColor = GetColorCursors(&ballCurs);
	if (!useColor && !GetMonoCursors(&ballCurs))
		RedAlert(kErrFailedResourceLoad);
	DisposCursors();
	animCursor = ballCurs;
	animCursor.index = 0;
}

//--------------------------------------------------------------  LoadCursors

// Just calls the above function.  Other code could be added here though…
// to add additional cursors.

void LoadCursors (void)
{
	InitAnimatedCursor(rAcurID);
}

//--------------------------------------------------------------  DisposCursors

// Disposes of all memory allocated by anaimated beach ball cursors.

void DisposCursors (void)
{
	SInt16 i, j;

	if (animCursor.frame != NULL)
	{
		j = animCursor.n;
		for (i = 0; i < j; i++)
		{
			if (animCursor.frame[i].cursorHdl != NULL)
			{
				DestroyCursor(animCursor.frame[i].cursorHdl);
				animCursor.frame[i].cursorHdl = NULL;
			}
		}
		free(animCursor.frame);
		animCursor.frame = NULL;
	}
}

//--------------------------------------------------------------  InitCursor

// Set the cursor to the standard arrow cursor.

void InitCursor (void)
{
	HCURSOR arrowCursor;

	arrowCursor = LoadCursor(NULL, IDC_ARROW);
	SetCursor(arrowCursor);
	SetMainWindowCursor(arrowCursor);
}

//--------------------------------------------------------------  IncrementCursor

// Advances the beach ball cursor one frame.

void IncrementCursor (void)
{
	HCURSOR waitCursor;

	waitCursor = NULL;
	if (animCursor.frame == NULL)
		InitAnimatedCursor(rAcurID);
	if (animCursor.frame)
	{
		animCursor.index++;
		animCursor.index %= animCursor.n;
		waitCursor = animCursor.frame[animCursor.index].cursorHdl;
	}

	if (waitCursor == NULL)
		waitCursor = LoadCursor(NULL, IDC_WAIT);
	if (waitCursor)
	{
		SetCursor(waitCursor);
		SetMainWindowCursor(waitCursor);
	}
}

//--------------------------------------------------------------  DecrementCursor

// Reverses the beach ball cursor one frame.

void DecrementCursor (void)
{
	HCURSOR waitCursor;

	waitCursor = NULL;
	if (animCursor.frame == NULL)
		InitAnimatedCursor(rAcurID);
	if (animCursor.frame)
	{
		animCursor.index--;
		if (animCursor.index < 0)
			animCursor.index = animCursor.n - 1;
		waitCursor = animCursor.frame[animCursor.index].cursorHdl;
	}

	if (waitCursor == NULL)
		waitCursor = LoadCursor(NULL, IDC_WAIT);
	if (waitCursor)
	{
		SetCursor(waitCursor);
		SetMainWindowCursor(waitCursor);
	}
}

//--------------------------------------------------------------  SpinCursor

// Advances the beach ball cursor the number of frames specified.

void SpinCursor (SInt16 incrementIndex)
{
	SInt16 i;

	for (i = 0; i < incrementIndex; i++)
	{
		IncrementCursor();
	}
}

//--------------------------------------------------------------  BackSpinCursor

// Reverses the beach ball cursor the number of frames specified.

void BackSpinCursor (SInt16 decrementIndex)
{
	SInt16 i;

	for (i = 0; i < decrementIndex; i++)
	{
		DecrementCursor();
	}
}

