#ifndef WINDOW_UTILS_H_
#define WINDOW_UTILS_H_

//============================================================================
//----------------------------------------------------------------------------
//                               WindowUtils.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

void GetWindowLeftTop (HWND, SInt16 *, SInt16 *);
void OpenMessageWindow (StringPtr);
void SetMessageWindowMessage (StringPtr);
void SetMessageTextColor (COLORREF);
void CloseMessageWindow (void);
void CloseThisWindow (HWND *);

#endif
