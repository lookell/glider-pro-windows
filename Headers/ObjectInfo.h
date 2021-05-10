//============================================================================
//----------------------------------------------------------------------------
//                                ObjectInfo.h
//----------------------------------------------------------------------------
//============================================================================

#ifndef OBJECT_INFO_H_
#define OBJECT_INFO_H_

#include "MacTypes.h"
#include "WinAPI.h"

HRESULT GetObjectName (SInt16 objectType, PWSTR *ppszObjectName);
void DoObjectInfo (HWND hwndOwner);

#endif
