
//============================================================================
//----------------------------------------------------------------------------
//								  AppleEvents.c
//----------------------------------------------------------------------------
//============================================================================


#include "Externs.h"
//#include <AppleEvents.h>
#include "Macintosh.h"
#include "DialogUtils.h"
#include "House.h"
#include "ResourceIDs.h"


#define errAEEventNotHandled		(-1708)


OSErr DoOpenAppAE (const AppleEvent *, AppleEvent *, UInt32);
OSErr DoOpenDocAE (const AppleEvent *, AppleEvent *, UInt32);
OSErr DoPrintDocAE (const AppleEvent *, AppleEvent *, UInt32);
OSErr DoQuitAE (const AppleEvent *, AppleEvent *, UInt32);
OSErr MyGotRequiredParams (const AppleEvent *);


AEEventHandlerUPP	openAppAEUPP, openDocAEUPP, printDocAEUPP, quitAEUPP;


extern	houseSpecPtr	theHousesSpecs;
extern	SInt32			incrementModeTime;
extern	SInt16			thisHouseIndex, splashOriginH, splashOriginV;
extern	Boolean			quitting;


//==============================================================  Functions
//--------------------------------------------------------------  DoOpenAppAE
// Handles an "Open Application" Apple Event.

OSErr DoOpenAppAE (const AppleEvent *theAE, AppleEvent *reply, UInt32 ref)
{
	return (-1);
#if 0
#pragma unused (reply, ref)
	OSErr		theErr;

	theErr = MyGotRequiredParams(theAE);
	return (theErr);
#endif
}

//--------------------------------------------------------------  DoOpenDocAE
// Handles an "Open Document" Apple Event.

OSErr DoOpenDocAE (const AppleEvent *theAE, AppleEvent *reply, UInt32 ref)
{
	return (-1);
#if 0
#pragma unused (reply, ref)
	FSSpec			oneFSS;
	FInfo			finderInfo;
	AEDescList		docList;
	long			itemsInList;
	Size			actualSize;
	AEKeyword		keywd;
	DescType		returnedType;
	OSErr			theErr, whoCares;
	short			i;

	theErr = AEGetParamDesc(theAE, keyDirectObject, typeAEList, &docList);
	if (theErr != noErr)
	{
		YellowAlert(kYellowAppleEventErr, theErr);
		return (theErr);
	}

	theErr = MyGotRequiredParams(theAE);
	if (theErr != noErr)
	{
		whoCares = AEDisposeDesc(&docList);
		return (theErr);
	}

	theErr = AECountItems(&docList, &itemsInList);
	if (theErr != noErr)
	{
		whoCares = AEDisposeDesc(&docList);
		return (theErr);
	}

#ifndef COMPILEDEMO
	for (i = 1; i <= itemsInList; i++)
	{
		theErr = AEGetNthPtr(&docList, i, typeFSS, &keywd, &returnedType,
				&oneFSS, sizeof(oneFSS), &actualSize);
		if (theErr == noErr)
		{
			theErr = FSpGetFInfo(&oneFSS, &finderInfo);
			if ((theErr == noErr) && (finderInfo.fdType == 'gliH'))
				AddExtraHouse(&oneFSS);
		}
	}
	if (itemsInList > 0)
	{
		theErr = AEGetNthPtr(&docList, 1, typeFSS, &keywd, &returnedType,
				&oneFSS, sizeof(oneFSS), &actualSize);
		if (theErr == noErr)
		{
			theErr = FSpGetFInfo(&oneFSS, &finderInfo);
			if ((theErr == noErr) && (finderInfo.fdType == 'gliH'))
			{
				whoCares = CloseHouse();
				PasStringCopy(oneFSS.name, thisHouseName);
				BuildHouseList();
				if (OpenHouse())
					whoCares = ReadHouse();
				PasStringCopy(theHousesSpecs[thisHouseIndex].name, thisHouseName);
				OpenCloseEditWindows();
				incrementModeTime = TickCount() + kIdleSplashTicks;
				if ((theMode == kSplashMode) || (theMode == kPlayMode))
				{
					Rect		updateRect;

					SetRect(&updateRect, splashOriginH + 474, splashOriginV + 304, splashOriginH + 474 + 166, splashOriginV + 304 + 12);
					InvalWindowRect(mainWindow, &updateRect);
				}
			}
		}
		InitCursor();
	}
#endif
	whoCares = AEDisposeDesc(&docList);

	return theErr;
#endif
}

//--------------------------------------------------------------  DoPrintDocAE
// Handles a "Print Document" Apple Event.

OSErr DoPrintDocAE (const AppleEvent *theAE, AppleEvent *reply, UInt32 ref)
{
	SInt16			hitWhat;

	UNREFERENCED_PARAMETER(theAE);
	UNREFERENCED_PARAMETER(reply);
	UNREFERENCED_PARAMETER(ref);

	hitWhat = Alert(kNoPrintingAlert, mainWindow, NULL);

	return errAEEventNotHandled;
}

//--------------------------------------------------------------  DoQuitAE
// Handles a "Quit Application" Apple Event.

OSErr DoQuitAE (const AppleEvent *theAE, AppleEvent *reply, UInt32 ref)
{
	return (-1);
#if 0
#pragma unused (reply, ref)
	OSErr			isHuman;

	isHuman = MyGotRequiredParams(theAE);
	if (isHuman == noErr)
		quitting = true;

	return isHuman;
#endif
}

//--------------------------------------------------------------  MyGotRequiredParams
// Have no clue!  :)

OSErr MyGotRequiredParams (const AppleEvent *theAE)
{
	return (-1);
#if 0
	DescType		returnedType;
	Size			actualSize;

	return (AEGetAttributePtr(theAE, keyMissedKeywordAttr, typeWildCard,
			&returnedType, 0L, 0, &actualSize) == errAEDescNotFound) ? noErr :
			errAEParamMissed;
#endif
}

//--------------------------------------------------------------  SetUpAppleEvents
// Initializes all handlers, etc. for dealing with Apple Events.

void SetUpAppleEvents (void)
{
	return;
#if 0
	OSErr		theErr;

	openAppAEUPP = NewAEEventHandlerProc(DoOpenAppAE);
	openDocAEUPP = NewAEEventHandlerProc(DoOpenDocAE);
	printDocAEUPP = NewAEEventHandlerProc(DoPrintDocAE);
	quitAEUPP = NewAEEventHandlerProc(DoQuitAE);

	theErr = AEInstallEventHandler(kCoreEventClass,		// install oapp
			kAEOpenApplication, openAppAEUPP, 0, false);
	if (theErr != noErr)
		YellowAlert(kYellowAppleEventErr, theErr);

	theErr = AEInstallEventHandler(kCoreEventClass, 	// install odoc
			kAEOpenDocuments, openDocAEUPP, 0, false);
	if (theErr != noErr)
		YellowAlert(kYellowAppleEventErr, theErr);

	theErr = AEInstallEventHandler(kCoreEventClass, 	// install pdoc
			kAEPrintDocuments, printDocAEUPP, 0, false);
	if (theErr != noErr)
		YellowAlert(kYellowAppleEventErr, theErr);

	theErr = AEInstallEventHandler(kCoreEventClass, 	// install quit
			kAEQuitApplication, quitAEUPP, 0, false);
	if (theErr != noErr)
		YellowAlert(kYellowAppleEventErr, theErr);

	theErr = AESetInteractionAllowed(kAEInteractWithAll);
	if (theErr != noErr)
		YellowAlert(kYellowAppleEventErr, theErr);
#endif
}

