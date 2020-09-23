#ifndef FRAME_TIMER_H_
#define FRAME_TIMER_H_

#ifdef GP_USE_WINAPI_H

#include "WinAPI.h"

#define FRAME_TIMER_DEFAULT_FPS 30

DWORD GetFrameRate(void);
void SetFrameRate(DWORD NewFrameRate);
void WaitUntilNextFrame(void);
void WaitUntilNextFrameOrMessage(BOOL *MessageReceived);

#endif

#endif
