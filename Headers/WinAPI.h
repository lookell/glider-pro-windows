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

// Do the '#include's
#include <windows.h>
#include <windowsx.h>

// The following pseudo-variable gives access to the hInstance
// of the current module.
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

// The following inline functions are based on helper macros from <windowsx.h>.
// They are redefined as functions to enable better type checking.

#undef DeletePen
__inline BOOL DeletePen(HPEN hpen)
{
	return DeleteObject((HGDIOBJ)hpen);
}

#undef SelectPen
__inline HPEN SelectPen(HDC hdc, HPEN hpen)
{
	return (HPEN)SelectObject(hdc, (HGDIOBJ)hpen);
}

#undef GetStockPen
__inline HPEN GetStockPen(int i)
{
	switch (i)
	{
	case WHITE_PEN:
	case BLACK_PEN:
	case NULL_PEN:
	case DC_PEN:
		return (HPEN)GetStockObject(i);

	default:
		return NULL;
	}
}

#undef DeleteBrush
__inline BOOL DeleteBrush(HBRUSH hbr)
{
	return DeleteObject((HGDIOBJ)hbr);
}

#undef SelectBrush
__inline HBRUSH SelectBrush(HDC hdc, HBRUSH hbr)
{
	return (HBRUSH)SelectObject(hdc, (HGDIOBJ)hbr);
}

#undef GetStockBrush
__inline HBRUSH GetStockBrush(int i)
{
	switch (i)
	{
	case WHITE_BRUSH:
	case LTGRAY_BRUSH:
	case GRAY_BRUSH:
	case DKGRAY_BRUSH:
	case BLACK_BRUSH:
	case NULL_BRUSH:
	case DC_BRUSH:
		return (HBRUSH)GetStockObject(i);

	default:
		return NULL;
	}
}

#undef DeleteRgn
__inline BOOL DeleteRgn(HRGN hrgn)
{
	return DeleteObject((HGDIOBJ)hrgn);
}

#undef DeleteFont
__inline BOOL DeleteFont(HFONT hfont)
{
	return DeleteObject((HGDIOBJ)hfont);
}

#undef SelectFont
__inline HFONT SelectFont(HDC hdc, HFONT hfont)
{
	return (HFONT)SelectObject(hdc, (HGDIOBJ)hfont);
}

#undef GetStockFont
__inline HFONT GetStockFont(int i)
{
	switch (i)
	{
	case OEM_FIXED_FONT:
	case ANSI_FIXED_FONT:
	case ANSI_VAR_FONT:
	case SYSTEM_FONT:
	case DEVICE_DEFAULT_FONT:
	case SYSTEM_FIXED_FONT:
	case DEFAULT_GUI_FONT:
		return (HFONT)GetStockObject(i);

	default:
		return NULL;
	}
}

#undef DeleteBitmap
__inline BOOL DeleteBitmap(HBITMAP hbm)
{
	return DeleteObject((HGDIOBJ)hbm);
}

#undef SelectBitmap
__inline HBITMAP SelectBitmap(HDC hdc, HBITMAP hbm)
{
	return (HBITMAP)SelectObject(hdc, (HGDIOBJ)hbm);
}

#undef MapWindowRect
__inline int MapWindowRect(HWND hwndFrom, HWND hwndTo, LPRECT lprc)
{
	return MapWindowPoints(hwndFrom, hwndTo, (LPPOINT)lprc, 2);
}

#endif
