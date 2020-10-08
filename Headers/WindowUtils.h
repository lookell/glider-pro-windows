#ifndef WINDOW_UTILS_H_
#define WINDOW_UTILS_H_

//============================================================================
//----------------------------------------------------------------------------
//                               WindowUtils.h
//----------------------------------------------------------------------------
//============================================================================

#ifdef GP_USE_WINAPI_H

#include "WinAPI.h"

void OpenMessageWindow (PCWSTR title, HWND hwndOwner);
void SetMessageWindowMessage (PCWSTR message);
void SetMessageTextColor (COLORREF textColor);
void CloseMessageWindow (void);

#endif

#endif
