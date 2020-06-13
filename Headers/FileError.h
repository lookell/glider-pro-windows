#ifndef FILE_ERROR_H_
#define FILE_ERROR_H_

//============================================================================
//----------------------------------------------------------------------------
//                                FileError.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

Boolean CheckFileError (HWND ownerWindow, DWORD resultCode, StringPtr fileName);

#endif
