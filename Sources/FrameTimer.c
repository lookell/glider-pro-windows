#define GP_USE_WINAPI_H

#include "FrameTimer.h"

#include "WinAPI.h"

// On Windows XP and later (our target platforms), the performance counter
// functions will not fail, so their return values are not checked.

static DWORD CurrentFrameRate = FRAME_TIMER_DEFAULT_FPS;
static LONGLONG FrameStart = 0;
static LONGLONG FrameNext = 0;

LONGLONG GetPerformanceCounter(void)
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return li.QuadPart;
}

LONGLONG GetPerformanceFrequency(void)
{
	static LONGLONG QPC_Frequency = 0;

	if (QPC_Frequency == 0)
	{
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		QPC_Frequency = li.QuadPart;
	}
	return QPC_Frequency;
}

DWORD GetFrameRate(void)
{
	return CurrentFrameRate;
}

void SetFrameRate(DWORD NewFrameRate)
{
	CurrentFrameRate = NewFrameRate;
}

static LONGLONG GetCountsPerFrame(void)
{
	return GetPerformanceFrequency() / GetFrameRate();
}

static LONGLONG GetCountsPerMS(void)
{
	return GetPerformanceFrequency() / 1000;
}

static DWORD GetWaitTimeUntilNextFrame(void)
{
	LONGLONG CountsPerFrame;
	LONGLONG CountsPerMS;
	LONGLONG FrameNow;
	LONGLONG CountsElapsed;
	LONGLONG CountsToSleep;
	DWORD SleepMS;

	CountsPerFrame = GetCountsPerFrame();
	CountsPerMS = GetCountsPerMS();
	FrameNow = GetPerformanceCounter();

	CountsElapsed = FrameNow - FrameStart;
	if (CountsElapsed >= CountsPerFrame)
	{
		FrameNext = FrameNow;
		SleepMS = 0;
	}
	else
	{
		FrameNext = FrameStart + CountsPerFrame;
		CountsToSleep = FrameNext - FrameNow;
		SleepMS = (DWORD)(CountsToSleep / CountsPerMS);
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
