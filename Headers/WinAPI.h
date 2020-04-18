// The following definitions ask for the following:
//
// * strict type definitions (this is the default, but just to be sure...)
// * use Unicode for unsuffixed functions
// * all API definitions available since Windows XP

#define STRICT
#define UNICODE
#define OEMRESOURCE
#define WINVER _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#define _WIN32_IE_ _WIN32_IE_WINXP

#include <windows.h>
#include <strsafe.h>

// The following pseudo-variable gives access to the hInstance
// of the current module.

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
