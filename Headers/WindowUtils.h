#ifndef WINDOW_UTILS_H_
#define WINDOW_UTILS_H_

//============================================================================
//----------------------------------------------------------------------------
//                               WindowUtils.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

void OpenMessageWindow (StringPtr title);
void SetMessageWindowMessage (StringPtr message);
void SetMessageTextColor (COLORREF textColor);
void CloseMessageWindow (void);

#endif
