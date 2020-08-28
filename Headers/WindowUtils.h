#ifndef WINDOW_UTILS_H_
#define WINDOW_UTILS_H_

//============================================================================
//----------------------------------------------------------------------------
//                               WindowUtils.h
//----------------------------------------------------------------------------
//============================================================================

#include "WinAPI.h"

void OpenMessageWindow (PCWSTR title);
void SetMessageWindowMessage (PCWSTR message);
void SetMessageTextColor (COLORREF textColor);
void CloseMessageWindow (void);

#endif
