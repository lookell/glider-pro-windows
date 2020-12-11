#ifndef WINDOW_UTILS_H_
#define WINDOW_UTILS_H_

//============================================================================
//----------------------------------------------------------------------------
//                               WindowUtils.h
//----------------------------------------------------------------------------
//============================================================================

#include "WinAPI.h"

HWND OpenMessageWindow (PCWSTR title, HWND hwndOwner);
void SetMessageWindowMessage (HWND mssgWindow, PCWSTR message);
void SetMessageTextColor (HWND mssgWindow, COLORREF textColor);
void CloseMessageWindow (HWND mssgWindow);

#endif
