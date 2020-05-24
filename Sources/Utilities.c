//============================================================================
//----------------------------------------------------------------------------
//								  Utilities.c
//----------------------------------------------------------------------------
//============================================================================


//#include <Icons.h>
//#include <NumberFormatting.h>
//#include <Resources.h>
//#include <Sound.h>
//#include <TextUtils.h>
//#include <ToolUtils.h>
#include <stdlib.h>
#include "Macintosh.h"
#include "WinAPI.h"

#include "Audio.h"
#include "DialogUtils.h"
#include "Externs.h"
#include "RectUtils.h"
#include "ResourceIDs.h"
#include "Utilities.h"


GDHandle	thisGDevice;
UInt32		theSeed;


extern	Boolean		switchedOut;
extern	HMODULE		houseResFork;


//==============================================================  Functions
//--------------------------------------------------------------  MyGetGlobalMouse
// Returns the position of the mouse in global coordinates.

Point MyGetGlobalMouse (void)
{
	Point	globalWhere;
	POINT	cursorPos;

	globalWhere.h = 0;
	globalWhere.v = 0;
	if (GetCursorPos(&cursorPos))
	{
		globalWhere.h = (SInt16)cursorPos.x;
		globalWhere.v = (SInt16)cursorPos.y;
	}
	return globalWhere;
}

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
	wcMain.hIcon = LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(1));
	wcMain.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcMain.hbrBackground = GetStockObject(BLACK_BRUSH);
	wcMain.lpszMenuName = NULL;
	wcMain.lpszClassName = WC_MAINWINDOW;
	wcMain.hIconSm = NULL;
	RegisterClassEx(&wcMain);

	InitRandomLongQUS();
	//InitCursor();
	switchedOut = false;
}

//--------------------------------------------------------------  RandomInt
// Returns a random integer (short) within "range".

SInt16 RandomInt (SInt16 range)
{
	register SInt32	rawResult;

	rawResult = (RandomLongQUS() & 0x7FFF);
	rawResult = (rawResult * (SInt32)range) / 32768L;

	return ((SInt16)rawResult);
}

//--------------------------------------------------------------  RandomLong

// Returns a random long interger within "range".

SInt32 RandomLong (SInt32 range)
{
	return 0;
#if 0
	register long	highWord, lowWord;
	register long	rawResultHi, rawResultLo;

	highWord = (range & 0xFFFF0000) >> 16;
	lowWord = range & 0x0000FFFF;

	rawResultHi = Random();
	if (rawResultHi < 0L)
		rawResultHi *= -1L;
	rawResultHi = (rawResultHi * highWord) / 32768L;

	rawResultLo = Random();
	if (rawResultLo < 0L)
		rawResultLo *= -1L;
	rawResultLo = (rawResultLo * lowWord) / 32768L;

	rawResultHi = (rawResultHi << 16) + rawResultLo;

	return (rawResultHi);
#endif
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

void RedAlert (SInt16 errorNumber)
{
	SInt16			dummyInt;
	INT				loadResult;
	DialogParams	params = { 0 };

	if (errorNumber > 1)		// <= 0 is unaccounted for
	{
		loadResult = LoadString(HINST_THISCOMPONENT, rErrTitleBase + errorNumber,
				params.arg[0], ARRAYSIZE(params.arg[0]));
		if (loadResult <= 0)
			params.arg[0][0] = L'\0';
		loadResult = LoadString(HINST_THISCOMPONENT, rErrMssgBase + errorNumber,
				params.arg[1], ARRAYSIZE(params.arg[1]));
		if (loadResult <= 0)
			params.arg[1][0] = L'\0';
	}
	else
	{
		loadResult = LoadString(HINST_THISCOMPONENT, rErrTitleBase + 1,
				params.arg[0], ARRAYSIZE(params.arg[0]));
		if (loadResult <= 0)
			params.arg[0][0] = L'\0';
		loadResult = LoadString(HINST_THISCOMPONENT, rErrMssgBase + 1,
				params.arg[1], ARRAYSIZE(params.arg[1]));
		if (loadResult <= 0)
			params.arg[1][0] = L'\0';
	}
	StringCchPrintf(params.arg[2], ARRAYSIZE(params.arg[2]), L"%d", (int)errorNumber);

	EnumThreadWindows(GetCurrentThreadId(), DisableThreadWndProc, 0);
	dummyInt = Alert(rDeathAlertID, mainWindow, &params);

	if (mainWindow != NULL)
		DestroyWindow(mainWindow);
	CoUninitialize();
	exit(EXIT_FAILURE);
}

//--------------------------------------------------------------  FindOurDevice

// Finds the main device (monitor with the menu bar on it).

void FindOurDevice (void)
{
	return;
#if 0
	thisGDevice = GetMainDevice();
	if (thisGDevice == nil)
		RedAlert(kErrFailedGetDevice);
#endif
}

//--------------------------------------------------------------  CreateOffScreenBitMap
// Creates an offscreen bit map (b&w - 1 bit depth).

/*
void CreateOffScreenBitMap (Rect *theRect, GrafPtr *offScreen)
{
	GrafPtr		theBWPort;
	BitMap		theBitMap;
	long		theRowBytes;

	theBWPort = (GrafPtr)(NewPtr(sizeof(GrafPort)));
	OpenPort(theBWPort);
	theRowBytes = (long)((theRect->right - theRect->left + 15L) / 16L) * 2L;
	theBitMap.rowBytes = (short)theRowBytes;
	theBitMap.baseAddr = NewPtr((long)theBitMap.rowBytes *
		(theRect->bottom - theRect->top));
	if (theBitMap.baseAddr == nil)
		RedAlert(kErrNoMemory);
	theBitMap.bounds = *theRect;
	if (MemError() != noErr)
		RedAlert(kErrNoMemory);
	SetPortBits(&theBitMap);
	ClipRect(theRect);
	RectRgn(theBWPort->visRgn, theRect);
	EraseRect(theRect);
	*offScreen = theBWPort;
}
*/
//--------------------------------------------------------------  CreateOffScreenPixMap
// Creates an offscreen pix map using the depth of the current device.
/*
void CreateOffScreenPixMap (Rect *theRect, CGrafPtr *offScreen)
{
	CTabHandle	thisColorTable;
	GDHandle	oldDevice;
	CGrafPtr	newCGrafPtr;
	Ptr			theseBits;
	long		sizeOfOff, offRowBytes;
	OSErr		theErr;
	short		thisDepth;
	char		wasState;

	oldDevice = GetGDevice();
	SetGDevice(thisGDevice);
	newCGrafPtr = nil;
	newCGrafPtr = (CGrafPtr)NewPtr(sizeof(CGrafPort));
	if (newCGrafPtr != nil)
	{
		OpenCPort(newCGrafPtr);
		thisDepth = (**(*newCGrafPtr).portPixMap).pixelSize;
		offRowBytes = ((((long)thisDepth *
				(long)(theRect->right - theRect->left)) + 15L) >> 4L) << 1L;
		sizeOfOff = (long)(theRect->bottom - theRect->top + 1) * offRowBytes;
	//	sizeOfOff = (long)(theRect->bottom - theRect->top) * offRowBytes;
		OffsetRect(theRect, -theRect->left, -theRect->top);
		theseBits = NewPtr(sizeOfOff);
		if (theseBits != nil)
		{								// workaround
			(**(*newCGrafPtr).portPixMap).baseAddr = theseBits + offRowBytes;
		//	(**(*newCGrafPtr).portPixMap).baseAddr = theseBits;
			(**(*newCGrafPtr).portPixMap).rowBytes = (short)offRowBytes + 0x8000;
			(**(*newCGrafPtr).portPixMap).bounds = *theRect;
			wasState = HGetState((Handle)thisGDevice);
			HLock((Handle)thisGDevice);
			thisColorTable = (**(**thisGDevice).gdPMap).pmTable;
			HSetState((Handle)thisGDevice, wasState);
			theErr = HandToHand((Handle *)&thisColorTable);
			(**(*newCGrafPtr).portPixMap).pmTable = thisColorTable;
			ClipRect(theRect);
			RectRgn(newCGrafPtr->visRgn, theRect);
			ForeColor(blackColor);
			BackColor(whiteColor);
			EraseRect(theRect);
		}
		else
		{
			CloseCPort(newCGrafPtr);
			DisposePtr((Ptr)newCGrafPtr);
			newCGrafPtr = nil;
			RedAlert(kErrNoMemory);
		}
	}
	else
		RedAlert(kErrNoMemory);

	*offScreen = newCGrafPtr;
	SetGDevice(oldDevice);
}
*/
//--------------------------------------------------------------------  CreateOffScreenGWorld
// Creates an offscreen GWorld using the depth passed in.

OSErr CreateOffScreenGWorld (HDC *theGWorld, Rect *bounds, SInt16 depth)
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


//--------------------------------------------------------------  KillOffScreenPixMap
// Destroys memory allocated by an offscreen pix map.
/*
void KillOffScreenPixMap (CGrafPtr offScreen)
{
	Ptr		theseBits;

	if (offScreen != nil)
	{
		theseBits = (**(*offScreen).portPixMap).baseAddr;
		theseBits -= (**(*offScreen).portPixMap).rowBytes & 0x7FFF;	// workaround
		DisposePtr(theseBits);
		DisposeHandle((Handle)(**(*offScreen).portPixMap).pmTable);
		CloseCPort(offScreen);
		DisposePtr((Ptr)offScreen);
	}
}
*/
//--------------------------------------------------------------  KillOffScreenBitMap
// Destroys memory allocated by an offscreen bit map.
/*
void KillOffScreenBitMap (GrafPtr offScreen)
{
	if (offScreen != nil)
	{
		DisposePtr((Ptr)(offScreen->portBits.baseAddr));
		ClosePort(offScreen);
		DisposePtr((Ptr)offScreen);
	}
}
*/
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

	hbm = NULL;
	if (houseResFork != NULL)
	{
		hbm = LoadImage(houseResFork, MAKEINTRESOURCE(resID),
				IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	}
	if (hbm == NULL)
	{
		hbm = LoadImage(HINST_THISCOMPONENT, MAKEINTRESOURCE(resID),
				IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
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

void LoadScaledGraphic (HDC hdc, SInt16 resID, Rect *theRect)
{
	HBITMAP		thePicture;

	thePicture = GetPicture(resID);
	if (thePicture == NULL)
		RedAlert(kErrFailedGraphicLoad);
	Mac_DrawPicture(hdc, thePicture, theRect);
	DeleteObject(thePicture);
}

//--------------------------------------------------------------  PlotSICN
// Draws a small icon (16 x 16 pixels).
/*
void PlotSICN (Rect *theRect, SICNHand theSICN, SInt32 theIndex)
{
	char		state;
	BitMap		srcBits;

	if ((theSICN != nil) &&
			((GetHandleSize((Handle)theSICN) / sizeof(SICN)) > theIndex))
	{
		state = HGetState((Handle)theSICN);
		HLock((Handle)theSICN);

		srcBits.baseAddr = (Ptr)(*theSICN)[theIndex];
		srcBits.rowBytes = 2;
		SetRect(&srcBits.bounds, 0, 0, 16, 16);

		CopyBits(&srcBits,&(*qd.thePort).portBits,
				&srcBits.bounds, theRect, srcCopy, nil);

		HSetState((Handle) theSICN, state);
	}
}
*/
//--------------------------------------------------------------  LargeIconPlot
// Draws a standard b&w icon (32 x 32) - resource is an 'ICON'.

void LargeIconPlot (Rect *theRect, SInt16 theID)
{
	return;
#if 0
	OSErr		theErr;
	Handle		theSuite;

	theErr = GetIconSuite(&theSuite, theID, svAllLargeData);
	if (theErr == noErr)
		theErr = PlotIconSuite(theRect, atNone, ttNone, theSuite);
#endif
}

//--------------------------------------------------------------  DrawCIcon

// Draws a standard color icon (32 x 32) - resource is a 'CICN'.

void DrawCIcon (SInt16 theID, SInt16 h, SInt16 v)
{
	return;
#if 0
	CIconHandle	theIcon;
	Rect		theRect;

	theIcon = GetCIcon(theID);
	if (theIcon != nil)
	{
		SetRect(&theRect, 0, 0, 32, 32);
		OffsetRect(&theRect, h, v);
		PlotCIcon(&theRect, theIcon);
		DisposeCIcon(theIcon);
	}
#endif
}

//--------------------------------------------------------------  LongSquareRoot

// This is a quick and dirty square root function that returns pretty…
// accurate long integer results.  It uses no transcendental functions or…
// floating point.

SInt32 LongSquareRoot (SInt32 theNumber)
{
	SInt32		currentAnswer;
	SInt32		nextTrial;

	if (theNumber <= 1L)
		return (theNumber);

	nextTrial = theNumber / 2;

	do
	{
		currentAnswer = nextTrial;
		nextTrial = (nextTrial + theNumber / nextTrial) / 2;
	}
	while (nextTrial < currentAnswer);

	return(currentAnswer);
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
	return;
#if 0
	KeyMap		theKeys;
	Boolean		waiting;

	waiting = true;

	while (waiting)
	{
		GetKeys(theKeys);
		if ((!BitTst(&theKeys, kCommandKeyMap)) || (!BitTst(&theKeys, kQKeyMap)))
			waiting = false;
	}
	FlushEvents(everyEvent, 0);
#endif
}

//--------------------------------------------------------------  KeyMapOffsetFromRawKey
// Converts a raw key code to keymap offset (ugly stuff).

char KeyMapOffsetFromRawKey (char rawKeyCode)
{
	return 0;
#if 0
	char		hiByte, loByte, theOffset;

	hiByte = rawKeyCode & 0xF0;
	loByte = rawKeyCode & 0x0F;

	if (loByte <= 0x07)
		theOffset = hiByte + (0x07 - loByte);
	else
		theOffset = hiByte + (0x17 - loByte);

	return (theOffset);
#endif
}

//--------------------------------------------------------------  GetKeyMapFromMessage
// Gets the key map offset from a keyDown event's message field.

char GetKeyMapFromMessage (SInt32 message)
{
	return 0;
#if 0
	long		theVirtual;
	char		offset;

	theVirtual = (message & keyCodeMask) >> 8;
	offset = KeyMapOffsetFromRawKey((char)theVirtual);
	return (offset);
#endif
}

//--------------------------------------------------------------  GetKeyName
// Given a keyDown event (it's message field), this function returns…
// a string with that key's name (so we get "Shift" and "Esc", etc.).

void GetKeyName (SInt32 message, StringPtr theName)
{
	PasStringCopyC("", theName);
	return;
#if 0
	long		theASCII, theVirtual;

	theASCII = message & charCodeMask;
	theVirtual = (message & keyCodeMask) >> 8;

	if ((theASCII >= kExclamationASCII) && (theASCII <= kZKeyASCII))
	{

		if ((theVirtual >= 0x0041) && (theVirtual <= 0x005C))
		{
			PasStringCopy("\p( )", theName);
			theName[2] = (char)theASCII;
		}
		else
		{
			PasStringCopy("\p  key", theName);
			theName[1] = (char)theASCII;
		}
	}
	else
	{
		switch (theASCII)
		{
			case kHomeKeyASCII:
			PasStringCopy("\phome", theName);
			break;

			case kEnterKeyASCII:
			PasStringCopy("\penter", theName);
			break;

			case kEndKeyASCII:
			PasStringCopy("\pend", theName);
			break;

			case kHelpKeyASCII:
			PasStringCopy("\phelp", theName);
			break;

			case kDeleteKeyASCII:
			PasStringCopy("\pdelete", theName);
			break;

			case kTabKeyASCII:
			PasStringCopy("\ptab", theName);
			break;

			case kPageUpKeyASCII:
			PasStringCopy("\ppg up", theName);
			break;

			case kPageDownKeyASCII:
			PasStringCopy("\ppg dn", theName);
			break;

			case kReturnKeyASCII:
			PasStringCopy("\preturn", theName);
			break;

			case kFunctionKeyASCII:
			switch (theVirtual)
			{
				case 0x0060:
				PasStringCopy("\pF5", theName);
				break;
				case 0x0061:
				PasStringCopy("\pF6", theName);
				break;
				case 0x0062:
				PasStringCopy("\pF7", theName);
				break;
				case 0x0063:
				PasStringCopy("\pF3", theName);
				break;
				case 0x0064:
				PasStringCopy("\pF8", theName);
				break;
				case 0x0065:
				PasStringCopy("\pF9", theName);
				break;
				case 0x0067:
				PasStringCopy("\pF11", theName);
				break;
				case 0x0069:
				PasStringCopy("\pF13", theName);
				break;
				case 0x006B:
				PasStringCopy("\pF14", theName);
				break;
				case 0x006D:
				PasStringCopy("\pF10", theName);
				break;
				case 0x006F:
				PasStringCopy("\pF12", theName);
				break;
				case 0x0071:
				PasStringCopy("\pF15", theName);
				break;
				case 0x0076:
				PasStringCopy("\pF4", theName);
				break;
				case 0x0078:
				PasStringCopy("\pF2", theName);
				break;
				case 0x007A:
				PasStringCopy("\pF1", theName);
				break;
				default:
				NumToString(theVirtual, theName);
				break;
			}
			break;

			case kClearKeyASCII:
			PasStringCopy("\pclear", theName);
			break;

			case kEscapeKeyASCII:
			if (theVirtual == 0x0047)
				PasStringCopy("\pclear", theName);
			else
				PasStringCopy("\pesc", theName);
			break;

			case kLeftArrowKeyASCII:
			PasStringCopy("\plf arrow", theName);
			break;

			case kRightArrowKeyASCII:
			PasStringCopy("\prt arrow", theName);
			break;

			case kUpArrowKeyASCII:
			PasStringCopy("\pup arrow", theName);
			break;

			case kDownArrowKeyASCII:
			PasStringCopy("\pdn arrow", theName);
			break;

			case kSpaceBarASCII:
			PasStringCopy("\pspace", theName);
			break;

			case kForwardDeleteASCII:
			PasStringCopy("\pfrwd del", theName);
			break;

			default:
			PasStringCopy("\p????", theName);
			break;
		}
	}
#endif
}

//--------------------------------------------------------------  OptionKeyDown
// Returns true is the Option key is being held down.

Boolean OptionKeyDown (void)
{
	return (GetKeyState(VK_MENU) < 0);
}

//--------------------------------------------------------------  ExtractCTSeed
// Very esoteric - gets the "color table seed" from a specified graf port.
/*
SInt32 ExtractCTSeed (CGrafPtr porter)
{
	long		theSeed;

	theSeed = (**((**(porter->portPixMap)).pmTable)).ctSeed;
	return(theSeed);
}
*/
//--------------------------------------------------------------  ForceCTSeed
// Forces  the "color table seed" from a specified graf port to a…
// specified value.
/*
void ForceCTSeed (CGrafPtr porter, SInt32 newSeed)
{
	(**((**(porter->portPixMap)).pmTable)).ctSeed = newSeed;
}
*/
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

void UnivGetSoundVolume (SInt16 *volume, Boolean hasSM3)
{
	float deviceVolume;

	UNREFERENCED_PARAMETER(hasSM3);

	Audio_GetMasterVolume(&deviceVolume);
	*volume = (SInt16)(7.0f * deviceVolume);

	if (*volume > 7)
		*volume = 7;
	else if (*volume < 0)
		*volume = 0;
}

//--------------------------------------------------------------  UnivSetSoundVolume
// Sets the speaker volume to a specified value (in the range of…
// zero to seven (handles Sound Manager 3 case as well).

void UnivSetSoundVolume (SInt16 volume, Boolean hasSM3)
{
	float deviceVolume;

	UNREFERENCED_PARAMETER(hasSM3);

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

