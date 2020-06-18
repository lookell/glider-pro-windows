#include "FrameTimer.h"
#include "WinAPI.h"

// On Windows XP and later (our target platforms), the performance counter
// functions will not fail, so their return values are not checked.

const LONGLONG FRAMES_PER_SECOND = 30;
const LONGLONG MS_PER_SECOND = 1000;

static LONGLONG QPC_CountsPerFrame = 0;
static LONGLONG QPC_CountsPerMS = 0;
static BOOL FrameTimer_IsInitialized = FALSE;

static LONGLONG FrameStart = 0;
static LONGLONG FrameNext = 0;

static void FrameTimer_Initialize(void)
{
	LONGLONG CountsPerSecond;

	CountsPerSecond = GetPerformanceFrequency();
	QPC_CountsPerFrame = CountsPerSecond / FRAMES_PER_SECOND;
	QPC_CountsPerMS = CountsPerSecond / MS_PER_SECOND;
	FrameTimer_IsInitialized = TRUE;
}

LONGLONG GetPerformanceCounter(void)
{
	LARGE_INTEGER li;

	QueryPerformanceCounter(&li);
	return li.QuadPart;
}

LONGLONG GetPerformanceFrequency(void)
{
	LARGE_INTEGER li;

	QueryPerformanceFrequency(&li);
	return li.QuadPart;
}

static DWORD GetWaitTimeUntilNextFrame(void)
{
	LONGLONG FrameNow;
	LONGLONG CountsElapsed;
	LONGLONG CountsToSleep;
	DWORD SleepMS;

	if (!FrameTimer_IsInitialized)
	{
		FrameTimer_Initialize();
	}
	FrameNow = GetPerformanceCounter();
	CountsElapsed = FrameNow - FrameStart;
	if (CountsElapsed >= QPC_CountsPerFrame)
	{
		FrameNext = FrameNow;
		SleepMS = 0;
	}
	else
	{
		FrameNext = FrameStart + QPC_CountsPerFrame;
		CountsToSleep = FrameNext - FrameNow;
		SleepMS = (DWORD)(CountsToSleep / QPC_CountsPerMS);
	}
	return SleepMS;
}

void WaitUntilNextFrame(void)
{
	DWORD SleepMS;

	SleepMS = GetWaitTimeUntilNextFrame();
	if (SleepMS != 0)
	{
		Sleep(SleepMS);
	}
	FrameStart = FrameNext;
}

void WaitUntilNextFrameOrMessage(BOOL *MessageReceived)
{
	DWORD SleepMS;
	DWORD WaitResult;

	*MessageReceived = FALSE;
	SleepMS = GetWaitTimeUntilNextFrame();
	if (SleepMS != 0)
	{
		WaitResult = MsgWaitForMultipleObjects(0, NULL, FALSE, SleepMS, QS_ALLINPUT);
		*MessageReceived = (WaitResult == WAIT_OBJECT_0);
	}
	if (*MessageReceived == FALSE)
	{
		FrameStart = FrameNext;
	}
}
