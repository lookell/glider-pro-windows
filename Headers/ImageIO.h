#ifndef IMAGE_IO_H_
#define IMAGE_IO_H_

#ifdef GP_USE_WINAPI_H

#include "WinAPI.h"

// Load a .BMP file from a memory buffer and return a compatible bitmap.
// The HBITMAP should be deleted by calling DeleteObject.
HBITMAP LoadMemoryBMP(const void *buffer, size_t length);

// Load a .BMP file from a memory buffer and return a DIB section bitmap.
// The HBITMAP should be deleted by calling DeleteObject.
HBITMAP LoadMemoryBMPAsDIBSection(const void *buffer, size_t length);

// Load a .ICO file from the given byte slice.
// The HICON should be destroyed by calling DestroyIcon.
//
// If `width` is zero, then `GetSystemMetrics(SM_CXICON)` is used instead.
// If `height` is zero, then `GetSystemMetrics(SM_CYICON)` is used instead.
HICON LoadMemoryICO(const void *buffer, size_t length, int width, int height);

#endif

#endif
