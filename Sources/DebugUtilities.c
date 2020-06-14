#include "DebugUtilities.h"

/*============================================================*/
/*============================================================*/
/*==                                                        ==*/
/*==               Debugging Utility Routines               ==*/
/*==                                                        ==*/
/*============================================================*/
/*============================================================*/


#include "Macintosh.h"
#include "WinAPI.h"


SInt16			barGraphHori = 0;


//==============================================================  Functions
//--------------------------------------------------------------  MonitorWait

void MonitorWait (void)
{
	return;
#if 0
	GrafPtr			wasPort, tempPort;
	Rect			tempRect;

	GetPort(&wasPort);

	tempPort = (GrafPtr)NewPtrClear(sizeof(GrafPort));
	OpenPort(tempPort);

	SetRect(&tempRect, 8, 28, 16, 36);
	InvertRect(&tempRect);

	ClosePort(tempPort);

	SetPort((GrafPtr)wasPort);
#endif
}

//--------------------------------------------------------------  DisplayRect

void DisplayRect (Rect *theRect)
{
	return;
#if 0
	GrafPtr			wasPort, tempPort;

	GetPort(&wasPort);

	tempPort = (GrafPtr)NewPtrClear(sizeof(GrafPort));
	OpenPort(tempPort);

	InvertRect(theRect);

	ClosePort(tempPort);
	SetPort((GrafPtr)wasPort);
#endif
}


//--------------------------------------------------------------  FlashRect

void FlashRect (Rect *theRect)

{
	return;
#if 0
	GrafPtr			wasPort, tempPort;

	GetPort(&wasPort);

	tempPort = (GrafPtr)NewPtrClear(sizeof(GrafPort));
	OpenPort(tempPort);

	InvertRect(theRect);
	InvertRect(theRect);
	InvertRect(theRect);
	InvertRect(theRect);

	ClosePort(tempPort);
	SetPort((GrafPtr)wasPort);
#endif
}

//--------------------------------------------------------------  CheckLegitRect

void CheckLegitRect(Rect *srcRect, Rect *inRect)
{
	Rect			dummyRect;

	if ((srcRect->left > srcRect->right) || (srcRect->top > srcRect->bottom))
		OutputDebugString(L"Source Rectangle not dimensional\n");

	if (!Mac_SectRect(srcRect, inRect, &dummyRect))
		OutputDebugString(L"Source Rectangle not Secting Target Rectangle\n");
}


/*==============================================================  DisplayLong  */

void DisplayLong (SInt32 theValue)

{
	return;
#if 0
	GrafPtr			wasPort, tempPort;
	Str255			tempStr;
	Rect			tempRect;

	GetPort(&wasPort);

	tempPort = (GrafPtr)NewPtrClear(sizeof(GrafPort));
	OpenPort(tempPort);

	NumToString(theValue, tempStr);
	MoveTo(20,40);
	SetRect(&tempRect, 18, 20, 122, 42);
	EraseRect(&tempRect);
	DrawString(tempStr);

	while (Button())
	{
	}
	while (!Button())
	{
	}

	ClosePort(tempPort);
	SetPort((GrafPtr)wasPort);
#endif
}


/*==============================================================  DisplayShort  */

void DisplayShort(SInt16 theValue)

{
	return;
#if 0
	GrafPtr			wasPort, tempPort;
	Str255			tempStr;
	Rect			tempRect;

	GetPort(&wasPort);

	tempPort = (GrafPtr)NewPtrClear(sizeof(GrafPort));
	OpenPort(tempPort);

	NumToString((long)theValue, tempStr);
	MoveTo(20,40);
	SetRect(&tempRect, 18, 20, 122, 42);
	EraseRect(&tempRect);
	DrawString(tempStr);

	while (Button())
	{
	}
	while (!Button())
	{
	}

	ClosePort(tempPort);
	SetPort((GrafPtr)wasPort);
#endif
}


/*==============================================================  FlashLong  */

void FlashLong(SInt32 theValue)
{
	return;
#if 0
	GrafPtr			wasPort, tempPort;
	Str255			tempStr;
	Rect			tempRect;

	GetPort(&wasPort);

	tempPort = (GrafPtr)NewPtrClear(sizeof(GrafPort));
	OpenPort(tempPort);

	NumToString(theValue, tempStr);
	MoveTo(20,40);
	SetRect(&tempRect, 18, 20, 122, 42);
	EraseRect(&tempRect);
	DrawString(tempStr);

	ClosePort(tempPort);
	SetPort((GrafPtr)wasPort);
#endif
}


/*==============================================================  FlashShort  */

void FlashShort (SInt16 theValue)
{
	return;
#if 0
	GrafPtr			wasPort, tempPort;
	Str255			tempStr;
	Rect			tempRect;

	GetPort(&wasPort);

	tempPort = (GrafPtr)NewPtrClear(sizeof(GrafPort));
	OpenPort(tempPort);

	NumToString((long)theValue, tempStr);
	MoveTo(20,40);
	SetRect(&tempRect, 18, 20, 122, 42);
	EraseRect(&tempRect);
	DrawString(tempStr);

	ClosePort(tempPort);
	SetPort((GrafPtr)wasPort);
#endif
}


/*==============================================================  DoBarGraph  */

void DoBarGraph (SInt16 theValue, SInt16 downScreen, SInt16 maxDown, SInt16 scaleIt)

{
	return;
#if 0
	GrafPtr			wasPort, tempPort;

	GetPort(&wasPort);

	tempPort = (GrafPtr)NewPtrClear(sizeof(GrafPort));
	OpenPort(tempPort);

	theValue *= scaleIt;

	PenPat(&qd.white);
	MoveTo(barGraphHori, 0);
	Line(0, maxDown);
	MoveTo(barGraphHori, downScreen);
	PenPat(&qd.black);
	if (theValue == 0)
	{
		theValue = 1;
		ForeColor(greenColor);
	}
	else if (theValue < 0)
	{
		if (theValue < -downScreen)
		{
			theValue = -downScreen;
			ForeColor(magentaColor);
		}
		else
			ForeColor(redColor);
	}
	else
	{
		if (theValue > downScreen)
		{
			ForeColor(cyanColor);
			theValue = downScreen;
		}
		else
			ForeColor(blueColor);
	}
	Line(0, -theValue);
	ForeColor(blackColor);

	barGraphHori++;
	if (barGraphHori >= 512)
		barGraphHori = 0;

	ClosePort(tempPort);
	SetPort((GrafPtr)wasPort);
#endif
}


/*==============================================================  BetaOkay  */

SInt16 BetaOkay (void)
{
	SYSTEMTIME		systemTime;
	Boolean			stillCool;

	GetLocalTime(&systemTime);

	if (systemTime.wMonth < 8)
		stillCool = true;
	else
		stillCool = false;

	return (SInt16)stillCool;
}

//--------------------------------------------------------------  DebugNum

void DebugNum (SInt32 theNum)
{
	WCHAR theStr[256];
	StringCchPrintf(theStr, ARRAYSIZE(theStr), L"%ld\n", (long)theNum);
	OutputDebugString(theStr);
}

//--------------------------------------------------------------  DisplayCTSeed

void DisplayCTSeed (CGrafPtr who)
{
	return;
#if 0
	long		theSeed;

	theSeed = (**((**(who->portPixMap)).pmTable)).ctSeed;
	DisplayLong(theSeed);
#endif
}

//--------------------------------------------------------------  FillScreenRed

void FillScreenRed (void)
{
	return;
#if 0
	GrafPtr			wasPort, tempPort;

	GetPort(&wasPort);
	tempPort = (GrafPtr)NewPtrClear(sizeof(GrafPort));
	OpenPort(tempPort);
	PenNormal();
	ForeColor(redColor);
	PaintRect(&qd.screenBits.bounds);
	ClosePort(tempPort);
	SetPort((GrafPtr)wasPort);
#endif
}

//--------------------------------------------------------------  DumpToResEditFile

void DumpToResEditFile (Ptr data, SInt32 dataSize)
{
	return;
#if 0
	DateTimeRec		timeRec;
	Str32			filesName, dateStr;
	Handle			newResource;
	UInt32			dateTime;
	long			tempLong;
	OSErr			theErr;
	short			iFileRef;

	PasStringCopy("\pTerrain ", filesName);

	GetDateTime(&dateTime);
	SecondsToDate(dateTime, &timeRec);
	tempLong = (long)timeRec.hour;
	NumToString(tempLong, dateStr);
	PasStringConcat(filesName, dateStr);
	PasStringConcat(filesName, "\p-");
	tempLong = (long)timeRec.minute;
	NumToString(tempLong, dateStr);
	PasStringConcat(filesName, dateStr);

	theErr = Create(filesName, 0, 'RSED', 'rsrc');
	if (theErr != noErr)
		DebugStr("\p Create");

	CreateResFile(filesName);
	if (ResError() != noErr)
		DebugStr("\p CreateResFile");

	iFileRef = OpenResFile(filesName);
	if ((ResError() != noErr) || (iFileRef == -1))
		DebugStr("\p OpenResFile");

	if (PtrToHand(data, &newResource, dataSize) != noErr)
		DebugStr("\pPtrToHand");
	AddResource(newResource, 'demo', 128, "\p");
	ChangedResource(newResource);
#endif
}

