#include "FrameTimer.h"

// On Windows XP and later (our target platforms), the performance counter
// functions will not fail, so their return values are not checked.

static DWORD g_CurrentFrameRate = FRAME_TIMER_DEFAULT_FPS;
static LONGLONG g_FrameStart = 0;
static LONGLONG g_FrameNext = 0;
static LONGLONG g_QpcFrequency = 0;

LONGLONG GetPerformanceCounter(void)
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return li.QuadPart;
}

LONGLONG GetPerformanceFrequency(void)
{
	if (g_QpcFrequency == 0)
	{
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		g_QpcFrequency = li.QuadPart;
	}
	return g_QpcFrequency;
}

DWORD GetFrameRate(void)
{
	return g_CurrentFrameRate;
}

void SetFrameRate(DWORD NewFrameRate)
{
	g_CurrentFrameRate = NewFrameRate;
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

	CountsElapsed = FrameNow - g_FrameStart;
	if (CountsElapsed >= CountsPerFrame)
	{
		g_FrameNext = FrameNow;
		SleepMS = 0;
	}
	else
	{
		g_FrameNext = g_FrameStart + CountsPerFrame;
		CountsToSleep = g_FrameNext - FrameNow;
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
	g_FrameStart = g_FrameNext;
}

BOOL PeekMessageOrWaitForFrame(LPMSG lpMsg, HWND hWnd,
	UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
	BOOL HasMessage;
	DWORD SleepMS;
	DWORD WaitResult;

	while (TRUE)
	{
		HasMessage = PeekMessage(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
		if (HasMessage)
		{
			return HasMessage;
		}
		SleepMS = GetWaitTimeUntilNextFrame();
		if (SleepMS == 0)
		{
			break;
		}
		WaitResult = MsgWaitForMultipleObjects(0, NULL, FALSE, SleepMS, QS_ALLINPUT);
		if (WaitResult != WAIT_OBJECT_0)
		{
			break;
		}
	}
	g_FrameStart = g_FrameNext;
	return FALSE;
}
