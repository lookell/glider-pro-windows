#include "AnimCursor.h"

//============================================================================
//----------------------------------------------------------------------------
//                                AnimCursor.c
//----------------------------------------------------------------------------
//============================================================================


#include "MainWindow.h"
#include "WinAPI.h"


//==============================================================  Functions
//--------------------------------------------------------------  InitCursor
// Set the cursor to the standard arrow cursor.

void InitCursor (void)
{
	HCURSOR arrowCursor;

	arrowCursor = LoadCursor(NULL, IDC_ARROW);
	if (arrowCursor)
	{
		SetCursor(arrowCursor);
		SetMainWindowCursor(arrowCursor);
	}
}

//--------------------------------------------------------------  SpinCursor
// Advances the beach ball cursor the number of frames specified.

void SpinCursor (SInt16 incrementIndex)
{
	HCURSOR waitCursor;

	(void)incrementIndex;

	waitCursor = LoadCursor(NULL, IDC_WAIT);
	if (waitCursor)
	{
		SetCursor(waitCursor);
		SetMainWindowCursor(waitCursor);
	}
}

