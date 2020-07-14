#ifndef WINDOW_UTILS_H_
#define WINDOW_UTILS_H_

//============================================================================
//----------------------------------------------------------------------------
//                               WindowUtils.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

void OpenMessageWindow (ConstStringPtr title);
void SetMessageWindowMessage (ConstStringPtr message);
void SetMessageTextColor (COLORREF textColor);
void CloseMessageWindow (void);

#endif
