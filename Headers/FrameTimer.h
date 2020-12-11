#ifndef FRAME_TIMER_H_
#define FRAME_TIMER_H_

#include "WinAPI.h"

#define FRAME_TIMER_DEFAULT_FPS 30

LONGLONG GetPerformanceCounter(void);
LONGLONG GetPerformanceFrequency(void);
DWORD GetFrameRate(void);
void SetFrameRate(DWORD NewFrameRate);
void WaitUntilNextFrame(void);
BOOL PeekMessageOrWaitForFrame(LPMSG lpMsg, HWND hWnd,
	UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);

#endif
