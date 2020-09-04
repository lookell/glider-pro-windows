#ifndef OBJECT_INFO_H_
#define OBJECT_INFO_H_

//============================================================================
//----------------------------------------------------------------------------
//                                ObjectInfo.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

void GetObjectName (wchar_t *pszDest, size_t cchDest, SInt16 objectType);
#ifdef GP_USE_WINAPI_H
void DoObjectInfo (HWND hwndOwner);
#endif

#endif
