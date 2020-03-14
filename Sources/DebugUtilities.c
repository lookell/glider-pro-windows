/*============================================================*/
/*============================================================*/
/*==														==*/
/*==				Debugging Utility Routines				==*/
/*==														==*/
/*============================================================*/
/*============================================================*/


//#include <NumberFormatting.h>
#include "Macintosh.h"
#include "Externs.h"


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
	return;
#if 0
	Rect			dummyRect;

	if ((srcRect->left > srcRect->right) || (srcRect->top > srcRect->bottom))
		DebugStr("\pSource Rectangle not dimensional");

	if (!SectRect(srcRect, inRect, &dummyRect))
		DebugStr("\pSource Rectangle not Secting Target Rectangle");
#endif
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
	return 0;
#if 0
	DateTimeRec		dateRecord;
	UInt32	theseSeconds;
	Boolean			stillCool;

	GetDateTime(&theseSeconds);
	SecondsToDate(theseSeconds, &dateRecord);

	if (dateRecord.month < 8)
		stillCool = true;
	else
		stillCool = false;

	return((short)stillCool);
#endif
}

//--------------------------------------------------------------  DebugNum

void DebugNum (SInt32 theNum)
{
	return;
#if 0
	Str255		theStr;
	NumToString(theNum, theStr);
	DebugStr(theStr);
#endif
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

