#include "AnimCursor.h"

//============================================================================
//----------------------------------------------------------------------------
//                                AnimCursor.c
//----------------------------------------------------------------------------
//============================================================================


#include "Environ.h"
#include "MacTypes.h"


#define	rAcurID					128
#define rHandCursorID			1000


typedef struct
{
	SInt16			n;
	SInt16			index;
	union
	{
		Handle		cursorHdl;
		SInt16		resID;
	} frame[1];
} acurRec, *acurPtr, **acurHandle;


Boolean GetMonoCursors (acurHandle);
Boolean GetColorCursors (acurHandle);
void InitAnimatedCursor (acurHandle);


acurHandle		animCursorH = nil;
Boolean			useColorCursor = false;


//==============================================================  Functions
//--------------------------------------------------------------  GetMonoCursors

// Loads b&w cursors (for animated beach ball).

Boolean GetMonoCursors (acurHandle ballCursH)
{
	return false;
#if 0
	short			i, j;
	CursHandle		cursHdl;

	if (ballCursH)							// Were we passed a legit acur handle?
	{
		j = (*ballCursH)->n;				// Get number of 'frames' in the acur
		for (i = 0; i < j; i++)				// Start walking the frames
		{
			cursHdl = GetCursor((*ballCursH)->frame[i].resID);
			if (cursHdl == nil)		// Did the cursor load? It didn't?...
			{								// Well then, toss what we got.
				for (j = 0; j < i; j++)
					DisposeHandle((*ballCursH)->frame[j].cursorHdl);
				return(false);				// And report this to mother.
			}								// However!...
			else							// If cursor loaded ok...
			{								// Detach it from the resource map...
				DetachResource((Handle)cursHdl);	// And assign to our struct
				(*ballCursH)->frame[i].cursorHdl = (Handle)cursHdl;
			}
		}
	}
	return(true);
#endif
}

//--------------------------------------------------------------  GetColorCursors

// Loads color cursors (for animated beach ball).

Boolean GetColorCursors (acurHandle ballCursH)
{
	return false;
#if 0
	short			i, j;
	CCrsrHandle		cursHdl;
	Boolean			result = true;

	if (ballCursH)
	{
		j = (*ballCursH)->n;				// Get the number of cursors
		HideCursor();						// Hide the cursor
		for (i = 0; i < j; i++)				// Walk through the acur resource
		{
			cursHdl = GetCCursor((*ballCursH)->frame[i].resID);	// Get the cursor
			if (cursHdl == nil)		// Make sure a real cursor was returned
			{								// If not, trash all cursors loaded
				for (j = 0; j < i; j++)
					DisposeCCursor((CCrsrHandle)(*ballCursH)->frame[j].cursorHdl);
				result = false;				// Tell calling proc we failed
				break;						// And break out of the loop
			}
			else							// But, if the cursor loaded ok
			{								// add it to our list or cursor handles
				(*ballCursH)->frame[i].cursorHdl = (Handle)cursHdl;
				SetCCursor((CCrsrHandle)(*ballCursH)->frame[i].cursorHdl);
			}
		}
		InitCursor();						// Show the cursor again (as arrow)
	}
	return(result);							// Return to calling proc w/ results
#endif
}

//--------------------------------------------------------------  InitAnimatedCursor

// Loads and sets up animated beach ball cursor structures.

void InitAnimatedCursor (acurHandle ballCursH)
{
	return;
#if 0
	Boolean			useColor;

	useColor = thisMac.hasColor;
	if (ballCursH == nil)
		ballCursH = (void *)GetResource('acur', 128);
	if (ballCursH && ballCursH != animCursorH)
	{
		HNoPurge((Handle)ballCursH);
		MoveHHi((Handle)ballCursH);
		HLock((Handle)ballCursH);
		if (useColor)
			useColor = GetColorCursors(ballCursH);
		if (!useColor && !GetMonoCursors(ballCursH))
			RedAlert(kErrFailedResourceLoad);
		DisposCursors();
		animCursorH = ballCursH;
		useColorCursor = useColor;
		(*ballCursH)->index = 0;
	}
	else
		RedAlert(kErrFailedResourceLoad);
#endif
}

//--------------------------------------------------------------  LoadCursors

// Just calls the above function.  Other code could be added here though…
// to add additional cursors.

void LoadCursors (void)
{
	return;
#if 0
	InitAnimatedCursor((acurHandle)GetResource('acur', rAcurID));
#endif
}

//--------------------------------------------------------------  DisposCursors

// Disposes of all memory allocated by anaimated beach ball cursors.

void DisposCursors (void)
{
	return;
#if 0
	register short		i, j;

	if (animCursorH != nil)
	{
		j = (*animCursorH)->n;
		if (useColorCursor)
		{
			for (i = 0; i < j; i++)
			{
				if ((*animCursorH)->frame[i].cursorHdl != nil)
					DisposeCCursor((CCrsrHandle)(*animCursorH)->frame[i].cursorHdl);
			}
		}
		else
		{
			for (i = 0; i < j; i++)
			{
				if ((*animCursorH)->frame[i].cursorHdl != nil)
					DisposeHandle((Handle)(*animCursorH)->frame[i].cursorHdl);
			}
		}
		ReleaseResource((Handle)animCursorH);
		animCursorH = nil;
	}
#endif
}

//--------------------------------------------------------------  IncrementCursor

// Advances the beach ball cursor one frame.

void IncrementCursor (void)
{
	return;
#if 0
	if (animCursorH == 0)
		InitAnimatedCursor(nil);
	if (animCursorH)
	{
		(*animCursorH)->index++;
		(*animCursorH)->index %= (*animCursorH)->n;
		if (useColorCursor)
		{
			SetCCursor((CCrsrHandle)(*animCursorH)->
					frame[(*animCursorH)->index].cursorHdl);
		}
		else
		{
			SetCursor((CursPtr)*(*animCursorH)->
					frame[(*animCursorH)->index].cursorHdl);
		}
	}
	else
		SetCursor((CursPtr)*GetCursor(watchCursor));
#endif
}

//--------------------------------------------------------------  DecrementCursor

// Reverses the beach ball cursor one frame.

void DecrementCursor (void)
{
	return;
#if 0
	if (animCursorH == 0)
		InitAnimatedCursor(nil);
	if (animCursorH)
	{
		(*animCursorH)->index--;
		if (((*animCursorH)->index) < 0)
			(*animCursorH)->index = ((*animCursorH)->n) - 1;
		if (useColorCursor)
		{
			SetCCursor((CCrsrHandle)(*animCursorH)->
					frame[(*animCursorH)->index].cursorHdl);
		}
		else
		{
			SetCursor((CursPtr)*(*animCursorH)->
					frame[(*animCursorH)->index].cursorHdl);
		}
	}
	else
		SetCursor((CursPtr)*GetCursor(watchCursor));
#endif
}

//--------------------------------------------------------------  SpinCursor

// Advances the beach ball cursor the number of frames specified.

void SpinCursor (SInt16 incrementIndex)
{
	return;
#if 0
	UInt32		dummyLong;
	short		i;

	for (i = 0; i < incrementIndex; i++)
	{
		IncrementCursor();
		Delay(1, &dummyLong);
	}
#endif
}

//--------------------------------------------------------------  BackSpinCursor

// Reverses the beach ball cursor the number of frames specified.

void BackSpinCursor (SInt16 decrementIndex)
{
	return;
#if 0
	UInt32		dummyLong;
	short		i;

	for (i = 0; i < decrementIndex; i++)
	{
		DecrementCursor();
		Delay(1, &dummyLong);
	}
#endif
}

