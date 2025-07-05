#ifndef WINAPI_H_
#define WINAPI_H_

// Ask for strict type definitions
#define STRICT

// Use wide character APIs as the default
#define UNICODE

// Include OEM resource IDs
#define OEMRESOURCE

// Ask for Windows XP and later APIs
#define WINVER          0x0501      // _WIN32_WINNT_WINXP
#define _WIN32_WINNT    0x0501      // _WIN32_WINNT_WINXP
#define _WIN32_IE       0x0600      // _WIN32_IE_XP
#define NTDDI_VERSION   0x05010000  // NTDDI_WINXP

// Slim down the amount of stuff in <windows.h>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

// From <windowsx.h>
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

// The following pseudo-variable gives access to the hInstance
// of the current module.
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

#endif
