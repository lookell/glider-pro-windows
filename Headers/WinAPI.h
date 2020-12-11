#ifndef WINAPI_H_
#define WINAPI_H_

// Ask for strict type definitions
#define STRICT

// Use wide character APIs as the default
#define UNICODE

// Include OEM resource IDs
#define OEMRESOURCE

// Ask for Windows XP and later APIs
#define WINVER _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#define _WIN32_IE_ _WIN32_IE_WINXP

// Slim down the amount of stuff in <windows.h>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

// Do the '#include's
#include <windows.h>
#include <windowsx.h>

// The following pseudo-variable gives access to the hInstance
// of the current module.
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

#endif
