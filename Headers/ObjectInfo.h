#ifndef OBJECT_INFO_H_
#define OBJECT_INFO_H_

//============================================================================
//----------------------------------------------------------------------------
//                                ObjectInfo.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

#include <wchar.h>

void GetObjectName (wchar_t *pszDest, size_t cchDest, SInt16 objectType);
void DoObjectInfo (HWND hwndOwner);

#endif
