#define GP_USE_WINAPI_H

#include "Utilities.h"

//============================================================================
//----------------------------------------------------------------------------
//                                Utilities.c
//----------------------------------------------------------------------------
//============================================================================


#include "AnimCursor.h"
#include "Audio.h"
#include "DialogUtils.h"
#include "Events.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "RectUtils.h"
#include "ResourceIDs.h"
#include "ResourceLoader.h"
#include "StringUtils.h"
#include "WinAPI.h"

#include <shlobj.h>
#include <strsafe.h>

#include <stdlib.h>


static UInt32 theSeed;


//==============================================================  Functions
//--------------------------------------------------------------  ToolBoxInit
// The standard ToolBox intialization that must happen when any Mac…
// program first launches.

void ToolBoxInit (void)
{
	INITCOMMONCONTROLSEX icc;
	WNDCLASSEX wcMain;

	icc.dwSize = sizeof(icc);
	icc.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icc);

	wcMain.cbSize = sizeof(wcMain);
	wcMain.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcMain.lpfnWndProc = MainWindowProc;
	wcMain.cbClsExtra = 0;
	wcMain.cbWndExtra = 0;
	wcMain.hInstance = HINST_THISCOMPONENT;
	wcMain.hIcon = LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDI_APPL));
	wcMain.hCursor = NULL;
	wcMain.hbrBackground = NULL;
	wcMain.lpszMenuName = NULL;
	wcMain.lpszClassName = WC_MAINWINDOW;
	wcMain.hIconSm = NULL;
	RegisterClassEx(&wcMain);

	InitRandomLongQUS();
	InitCursor();
	switchedOut = false;
}

//--------------------------------------------------------------  RandomInt
// Returns a random integer (short) within "range".

SInt16 RandomInt (SInt16 range)
{
	SInt32 rawResult;

	rawResult = RandomLongQUS() & 0x7FFF;
	rawResult = (rawResult * (SInt32)range) / 32768L;

	return ((SInt16)rawResult);
}

//--------------------------------------------------------------  InitRandomLongQUS
// Initializes random seed for quick & dirty long random number function (below).

void InitRandomLongQUS (void)
{
	Mac_GetDateTime(&theSeed);
}

//--------------------------------------------------------------  RandomLongQUS
// Very simple (but fast) pseudo-random number generator.

UInt32 RandomLongQUS (void)
{
	theSeed = theSeed * 1103515245 + 12345;
	return (theSeed);
}

//--------------------------------------------------------------  RedAlert
// Called when we must quit app.  Brings up a dialog informing user…
// of the problem and the exits to shell.

static BOOL CALLBACK DisableThreadWndProc(HWND window, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	EnableWindow(window, FALSE);
	return TRUE;
}

__declspec(noreturn) void RedAlert (SInt16 errorNumber)
{
	DialogParams params = { 0 };
	SInt16 dummyInt;
	INT loadResult;
	wchar_t errTitle[256];
	wchar_t errMessage[256];
	wchar_t errNumberString[32];

	if (errorNumber <= 0)       // <= 0 is unaccounted for
		errorNumber = 1;

	loadResult = LoadString(HINST_THISCOMPONENT, rErrTitleBase + errorNumber,
			errTitle, ARRAYSIZE(errTitle));
	if (loadResult <= 0)
		errTitle[0] = L'\0';

	loadResult = LoadString(HINST_THISCOMPONENT, rErrMssgBase + errorNumber,
			errMessage, ARRAYSIZE(errMessage));
	if (loadResult <= 0)
		errMessage[0] = L'\0';

	StringCchPrintf(errNumberString, ARRAYSIZE(errNumberString), L"%d", (int)errorNumber);

	EnumThreadWindows(GetCurrentThreadId(), DisableThreadWndProc, 0);
	params.arg[0] = errTitle;
	params.arg[1] = errMessage;
	params.arg[2] = errNumberString;
	dummyInt = Alert(rDeathAlertID, mainWindow, &params);

	if (mainWindow != NULL)
		DestroyWindow(mainWindow);
	exit(EXIT_FAILURE);
}

//--------------------------------------------------------------------  CreateOffScreenGWorld
// Creates an offscreen GWorld using the depth passed in.

OSErr CreateOffScreenGWorld (HDC *theGWorld, const Rect *bounds, SInt16 depth)
{
	HDC			hdcDisplay, hdcSurface;
	HBITMAP		hbmSurface;
	INT			cxSurface, cySurface;

	*theGWorld = NULL;

	hdcDisplay = GetDC(NULL);
	if (hdcDisplay == NULL)
		return (-1);
	hdcSurface = CreateCompatibleDC(hdcDisplay);
	cxSurface = bounds->right - bounds->left;
	cySurface = bounds->bottom - bounds->top;
	if (depth == 1)
		hbmSurface = CreateBitmap(cxSurface, cySurface, 1, 1, NULL);
	else
		hbmSurface = CreateCompatibleBitmap(hdcDisplay, cxSurface, cySurface);
	ReleaseDC(NULL, hdcDisplay);
	if (hdcSurface == NULL || hbmSurface == NULL)
	{
		if (hdcSurface)
			DeleteDC(hdcSurface);
		if (hbmSurface)
			DeleteObject(hbmSurface);
		return (-1);
	}
	SaveDC(hdcSurface);
	SetWindowOrgEx(hdcSurface, bounds->left, bounds->top, NULL);
	SelectObject(hdcSurface, hbmSurface);
	SelectObject(hdcSurface, GetStockObject(DC_BRUSH));
	SelectObject(hdcSurface, GetStockObject(DC_PEN));
	SetDCBrushColor(hdcSurface, RGB(0xFF, 0xFF, 0xFF));
	SetDCPenColor(hdcSurface, RGB(0x00, 0x00, 0x00));
	SetBkColor(hdcSurface, RGB(0xFF, 0xFF, 0xFF));
	SetTextColor(hdcSurface, RGB(0x00, 0x00, 0x00));
	*theGWorld = hdcSurface;
	return noErr;
}

//--------------------------------------------------------------  DisposeGWorld
// Destroys memory allocated by an offscreen GWorld.

void DisposeGWorld (HDC theGWorld)
{
	HGDIOBJ		hbmSurface;

	hbmSurface = GetCurrentObject(theGWorld, OBJ_BITMAP);
	RestoreDC(theGWorld, -1);
	DeleteDC(theGWorld);
	DeleteObject(hbmSurface);
}

//--------------------------------------------------------------  GetPicture
// Function loads the specified 'PICT' from the house's resources
// (if the house has that 'PICT') or the game's resources otherwise.

HBITMAP GetPicture (SInt16 resID)
{
	HBITMAP hbm;

	if (FAILED(Gp_LoadImage(resID, &hbm)))
	{
		hbm = NULL;
	}
	return hbm;
}

//--------------------------------------------------------------  LoadGraphic
// Function loads the specified 'PICT' from disk and draws it to…
// the current port (no scaling, clipping, etc, are done).  Always…
// draws in the upper left corner of current port.

void LoadGraphic (HDC hdc, SInt16 resID)
{
	Rect		bounds;
	HBITMAP		thePicture;
	BITMAP		bmInfo;

	thePicture = GetPicture(resID);
	if (thePicture == NULL)
		RedAlert(kErrFailedGraphicLoad);

	GetObject(thePicture, sizeof(bmInfo), &bmInfo);
	QSetRect(&bounds, 0, 0, (SInt16)bmInfo.bmWidth, (SInt16)bmInfo.bmHeight);
	Mac_DrawPicture(hdc, thePicture, &bounds);

	DeleteObject(thePicture);
}

//--------------------------------------------------------------  LoadScaledGraphic
// Loads the specified 'PICT' and draws it mapped to the rectangle…
// specified.  If this rect isn't the same size of the 'PICT', scaling…
// will occur.

void LoadScaledGraphic (HDC hdc, SInt16 resID, const Rect *theRect)
{
	HBITMAP		thePicture;

	thePicture = GetPicture(resID);
	if (thePicture == NULL)
		RedAlert(kErrFailedGraphicLoad);
	Mac_DrawPicture(hdc, thePicture, theRect);
	DeleteObject(thePicture);
}

//--------------------------------------------------------------  DrawCIcon
// Draws a standard color icon (32 x 32) - resource is a 'CICN'.

void DrawCIcon (HDC hdc, SInt16 theID, SInt16 h, SInt16 v)
{
	HICON hIcon;

	if (hdc == NULL)
		return;

	hIcon = LoadImage(HINST_THISCOMPONENT, MAKEINTRESOURCE(theID),
			IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
	if (hIcon != NULL)
	{
		DrawIconEx(hdc, h, v, hIcon, 32, 32, 0, NULL, DI_NORMAL);
		DestroyIcon(hIcon);
	}
}

//--------------------------------------------------------------  CreateTahomaFont
// Create an HFONT with the specified height and weight in typeface Tahoma.
// This is a shorthand for creating a LOGFONT with mostly default values,
// and then calling CreateFontIndirect. The `height` and `weight` parameters
// are assigned to LOGFONT::lfHeight and LOGFONT::lfWeight with no modification.

HFONT CreateTahomaFont (LONG height, LONG weight)
{
	LOGFONT lf;

	lf.lfHeight = height;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = weight;
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	StringCchCopy(lf.lfFaceName, ARRAYSIZE(lf.lfFaceName), TEXT("Tahoma"));
	return CreateFontIndirect(&lf);
}

//--------------------------------------------------------------  WaitForInputEvent
// Wait for either a key to be hit or the mouse button to be clicked.
// Also has a "timeout" parameter ("seconds").

Boolean WaitForInputEvent (UInt16 seconds)
{
	MSG			theEvent;
	DWORD		startTime, elapsedTime, timeout;
	DWORD		status;
	Boolean		waiting, didResume;

	if (seconds < 0)
		return false;

	startTime = GetTickCount();
	timeout = 1000 * seconds;
	waiting = true;
	didResume = false;

	while (waiting)
	{
		elapsedTime = GetTickCount() - startTime;
		if (elapsedTime > timeout)
			break;
		status = MsgWaitForMultipleObjects(0, NULL, FALSE,
				timeout - elapsedTime, QS_ALLINPUT);
		if (status != WAIT_OBJECT_0)
			break;
		while (PeekMessage(&theEvent, NULL, 0, 0, PM_REMOVE))
		{
			if (theEvent.message == WM_QUIT)
			{
				PostQuitMessage((int)theEvent.wParam);
				waiting = false;
				break;
			}
			TranslateMessage(&theEvent);
			DispatchMessage(&theEvent);
			switch (theEvent.message)
			{
				case WM_KEYDOWN:
				case WM_LBUTTONDOWN:
				case WM_MBUTTONDOWN:
				case WM_RBUTTONDOWN:
				case WM_XBUTTONDOWN:
				waiting = false;
				break;

				case WM_ACTIVATEAPP:
				if (theEvent.wParam)
				{
					didResume = true;
					waiting = false;
				}
				break;
			}
		}
	}
	return (didResume);
}

//--------------------------------------------------------------  WaitCommandQReleased
// Waits until the Command-Q key combination is released.

void WaitCommandQReleased (void)
{
	MSG msg;

	// if control key is up or Q key is up
	if ((GetAsyncKeyState(VK_CONTROL) >= 0) || (GetAsyncKeyState('Q') >= 0))
	{
		return;
	}
	while (GetMessage(&msg, NULL, WM_KEYFIRST, WM_KEYLAST))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_KEYUP)
		{
			if (msg.wParam == VK_CONTROL || msg.wParam == 'Q')
			{
				break;
			}
		}
	}
}

//--------------------------------------------------------------  OptionKeyDown
// Returns true is the Option key is being held down.

Boolean OptionKeyDown (void)
{
	return (GetKeyState(VK_MENU) < 0);
}

//--------------------------------------------------------------  DelayTicks
// Lil' function that just sits and waits a specified number of…
// Ticks (1/60 of a second).

void DelayTicks (SInt32 howLong)
{
	Sleep(TicksToMillis(howLong));
}

//--------------------------------------------------------------  UnivGetSoundVolume
// Returns the speaker volume (as set by the user) in the range of…
// zero to seven (handles Sound Manager 3 case as well).

void UnivGetSoundVolume (SInt16 *volume)
{
	float deviceVolume;

	deviceVolume = Audio_GetMasterVolume();
	*volume = (SInt16)(7.0f * deviceVolume);

	if (*volume > 7)
		*volume = 7;
	else if (*volume < 0)
		*volume = 0;
}

//--------------------------------------------------------------  UnivSetSoundVolume
// Sets the speaker volume to a specified value (in the range of…
// zero to seven (handles Sound Manager 3 case as well).

void UnivSetSoundVolume (SInt16 volume)
{
	float deviceVolume;

	if (volume > 7)
		volume = 7;
	else if (volume < 0)
		volume = 0;

	deviceVolume = (float)volume / 7.0f;
	Audio_SetMasterVolume(deviceVolume);
}

//-----------------------------------------------------------------  GetDataFolderPath

BOOL GetDataFolderPath (LPWSTR lpDataPath, DWORD cchDataPath)
{
	static int useProgramDirectory = -1;
	WCHAR pathBuffer[MAX_PATH];
	DWORD fileAttributes;
	DWORD result;
	PWCH sepPtr;
	HRESULT hr;

	if (lpDataPath == NULL)
		return FALSE;

	result = GetModuleFileName(HINST_THISCOMPONENT, pathBuffer, ARRAYSIZE(pathBuffer));
	if (result == 0 || result == ARRAYSIZE(pathBuffer))
		return FALSE;
	sepPtr = wcsrchr(pathBuffer, L'\\');
	if (sepPtr == NULL)
		sepPtr = &pathBuffer[0];
	*sepPtr = L'\0';

	if (useProgramDirectory == -1)
	{
		hr = StringCchCat(pathBuffer, ARRAYSIZE(pathBuffer), L"\\portable.dat");
		if (FAILED(hr))
			return FALSE;
		fileAttributes = GetFileAttributes(pathBuffer);
		*sepPtr = L'\0';
		useProgramDirectory = (fileAttributes != INVALID_FILE_ATTRIBUTES);
	}
	if (!useProgramDirectory)
	{
		hr = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, pathBuffer);
		if (FAILED(hr))
			return FALSE;
		hr = StringCchCat(pathBuffer, ARRAYSIZE(pathBuffer), L"\\glider-pro-windows");
		if (FAILED(hr))
			return FALSE;
		if (!CreateDirectory(pathBuffer, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
			return FALSE;
	}
	hr = StringCchCopy(lpDataPath, cchDataPath, pathBuffer);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

//--------------------------------------------------------------  LoadModuleResource

HRESULT LoadModuleResource (HMODULE hModule, LPCWSTR pResName, LPCWSTR pResType,
	LPVOID *ppResData, DWORD *pResByteSize)
{
	HRSRC infoHandle;
	DWORD dataSize;
	HGLOBAL dataHandle;
	LPVOID dataPointer;
	DWORD lastError;

	if (ppResData != NULL)
	{
		*ppResData = NULL;
	}
	if (pResByteSize != NULL)
	{
		*pResByteSize = 0;
	}

	if (pResName == NULL)
	{
		return E_INVALIDARG;
	}
	if (pResType == NULL)
	{
		return E_INVALIDARG;
	}
	if (ppResData == NULL)
	{
		return E_INVALIDARG;
	}
	if (pResByteSize == NULL)
	{
		return E_INVALIDARG;
	}

	infoHandle = FindResource(hModule, pResName, pResType);
	if (infoHandle == NULL)
	{
		lastError = GetLastError();
		return HRESULT_FROM_WIN32(lastError);
	}
	dataSize = SizeofResource(hModule, infoHandle);
	if (dataSize == 0)
	{
		lastError = GetLastError();
		return HRESULT_FROM_WIN32(lastError);
	}
	dataHandle = LoadResource(hModule, infoHandle);
	if (dataHandle == NULL)
	{
		lastError = GetLastError();
		return HRESULT_FROM_WIN32(lastError);
	}
	dataPointer = LockResource(dataHandle);
	if (dataPointer == NULL)
	{
		return E_FAIL;
	}

	*ppResData = dataPointer;
	*pResByteSize = dataSize;
	return S_OK;
}

