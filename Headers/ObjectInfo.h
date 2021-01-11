//============================================================================
//----------------------------------------------------------------------------
//                                ObjectInfo.h
//----------------------------------------------------------------------------
//============================================================================

#ifndef OBJECT_INFO_H_
#define OBJECT_INFO_H_

#include "MacTypes.h"
#include "WinAPI.h"

#include <wchar.h>

void GetObjectName (wchar_t *pszDest, size_t cchDest, SInt16 objectType);
void DoObjectInfo (HWND hwndOwner);

#endif
