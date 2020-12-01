#ifndef FILE_ERROR_H_
#define FILE_ERROR_H_

//============================================================================
//----------------------------------------------------------------------------
//                                FileError.h
//----------------------------------------------------------------------------
//============================================================================

#ifdef GP_USE_WINAPI_H

#include "MacTypes.h"
#include "WinAPI.h"

Boolean CheckFileError (HWND ownerWindow, HRESULT resultCode, PCWSTR fileName);

#endif

#endif
