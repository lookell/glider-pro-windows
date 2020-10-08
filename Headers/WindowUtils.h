#ifndef WINDOW_UTILS_H_
#define WINDOW_UTILS_H_

//============================================================================
//----------------------------------------------------------------------------
//                               WindowUtils.h
//----------------------------------------------------------------------------
//============================================================================

#ifdef GP_USE_WINAPI_H

#include "WinAPI.h"

HWND OpenMessageWindow (PCWSTR title, HWND hwndOwner);
void SetMessageWindowMessage (HWND mssgWindow, PCWSTR message);
void SetMessageTextColor (HWND mssgWindow, COLORREF textColor);
void CloseMessageWindow (HWND mssgWindow);

#endif

#endif
