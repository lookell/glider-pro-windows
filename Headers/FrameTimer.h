#ifndef FRAME_TIMER_H_
#define FRAME_TIMER_H_

#include "WinAPI.h"

LONGLONG GetPerformanceCounter(void);
LONGLONG GetPerformanceFrequency(void);
void WaitUntilNextFrame(void);
void WaitUntilNextFrameOrMessage(BOOL *MessageReceived);

#endif
