
//============================================================================
//----------------------------------------------------------------------------
//								  HighScores.c
//----------------------------------------------------------------------------
//============================================================================


//#include <Folders.h>
//#include <NumberFormatting.h>
//#include <Script.h>
//#include <Sound.h>
//#include <StringCompare.h>
#include "Macintosh.h"
#include "DialogUtils.h"
#include "Externs.h"
#include "Environ.h"
#include "House.h"
#include "MainWindow.h"
#include "RectUtils.h"
#include "Utilities.h"


#define kHighScoresPictID		1994
#define kHighScoresMaskID		1998
#define kHighNameDialogID		1020
#define kHighBannerDialogID		1021
#define kHighNameItem			2
#define kNameNCharsItem			5
#define kHighBannerItem			2
#define kBannerScoreNCharsItem	5


void DrawHighScores (void);
void UpdateNameDialog (DialogPtr);
pascal Boolean NameFilter (DialogPtr, EventRecord *, SInt16 *);
void GetHighScoreName (SInt16);
void UpdateBannerDialog (DialogPtr);
pascal Boolean BannerFilter (DialogPtr, EventRecord *, SInt16 *);
void GetHighScoreBanner (void);
Boolean CreateScoresFolder (SInt32 *);
Boolean FindHighScoresFolder (SInt16 *, SInt32 *);
Boolean OpenHighScoresFile (FSSpec *, SInt16 *);


Str31		highBanner;
Str15		highName;
short		lastHighScore;
Boolean		keyStroke;

extern	short		splashOriginH, splashOriginV;
extern	Boolean		quickerTransitions, resumedSavedGame;


//==============================================================  Functions
//--------------------------------------------------------------  DoHighScores
// Handles fading in and cleaning up the high scores screen.

void DoHighScores (void)
{
	return;
#if 0
	Rect		tempRect;

	SpinCursor(3);
	SetPort((GrafPtr)workSrcMap);
	PaintRect(&workSrcRect);
	QSetRect(&tempRect, 0, 0, 640, 480);
	QOffsetRect(&tempRect, splashOriginH, splashOriginV);
	LoadScaledGraphic(kStarPictID, &tempRect);
//	if (quickerTransitions)
//		DissBitsChunky(&workSrcRect);
//	else
//		DissBits(&workSrcRect);
	SpinCursor(3);
	SetPort((GrafPtr)workSrcMap);
	DrawHighScores();
	SpinCursor(3);
//	if (quickerTransitions)
//		DissBitsChunky(&workSrcRect);
//	else
//		DissBits(&workSrcRect);
	InitCursor();
	DelayTicks(60);
	WaitForInputEvent(30);

	RedrawSplashScreen();
#endif
}

//--------------------------------------------------------------  DrawHighScores
// Draws the actual scores on the screen.

#define kScoreSpacing			18
#define kScoreWide				352
#define kKimsLifted				4

void DrawHighScores (void)
{
	return;
#if 0
	GWorldPtr	tempMap, tempMask;
	CGrafPtr	wasCPort;
	GDHandle	wasWorld;
	OSErr		theErr;
	houseType	*thisHousePtr;
	Rect		tempRect, tempRect2;
	Str255		tempStr;
	short		scoreLeft, bannerWidth, i, dropIt;
	char		wasState;

	scoreLeft = ((thisMac.screen.right - thisMac.screen.left) - kScoreWide) / 2;
	dropIt = 129 + splashOriginV;

	GetGWorld(&wasCPort, &wasWorld);

	QSetRect(&tempRect, 0, 0, 332, 30);
	theErr = CreateOffScreenGWorld(&tempMap, &tempRect, kPreferredDepth);
	SetGWorld(tempMap, nil);
	LoadGraphic(kHighScoresPictID);

	theErr = CreateOffScreenGWorld(&tempMask, &tempRect, 1);
	SetGWorld(tempMask, nil);
	LoadGraphic(kHighScoresMaskID);

	tempRect2 = tempRect;
	QOffsetRect(&tempRect2, scoreLeft + (kScoreWide - 332) / 2, dropIt - 60);

	CopyMask((BitMap *)*GetGWorldPixMap(tempMap),
			(BitMap *)*GetGWorldPixMap(tempMask),
			(BitMap *)*GetGWorldPixMap(workSrcMap),
			&tempRect, &tempRect, &tempRect2);

	DisposeGWorld(tempMap);
	DisposeGWorld(tempMask);

	SetGWorld(wasCPort, wasWorld);

	TextFont(applFont);
	TextFace(bold);
	TextSize(14);

	PasStringCopy("\p• ", tempStr);
	PasStringConcat(tempStr, thisHouseName);
	PasStringConcat(tempStr, "\p •");
	MoveTo(scoreLeft + ((kScoreWide - StringWidth(tempStr)) / 2) - 1, dropIt - 66);
	ForeColor(blackColor);
	DrawString(tempStr);
	MoveTo(scoreLeft + ((kScoreWide - StringWidth(tempStr)) / 2), dropIt - 65);
	ForeColor(cyanColor);
	DrawString(tempStr);
	ForeColor(blackColor);

	TextFont(applFont);
	TextFace(bold);
	TextSize(12);

	wasState = HGetState((Handle)thisHouse);
	HLock((Handle)thisHouse);
	thisHousePtr = *thisHouse;
													// message for score #1
	PasStringCopy(thisHousePtr->highScores.banner, tempStr);
	bannerWidth = StringWidth(tempStr);
	ForeColor(blackColor);
	MoveTo(scoreLeft + (kScoreWide - bannerWidth) / 2, dropIt - kKimsLifted);
	DrawString(tempStr);
	ForeColor(yellowColor);
	MoveTo(scoreLeft + (kScoreWide - bannerWidth) / 2, dropIt - kKimsLifted - 1);
	DrawString(tempStr);

	QSetRect(&tempRect, 0, 0, bannerWidth + 8, kScoreSpacing);
	QOffsetRect(&tempRect, scoreLeft - 3 + (kScoreWide - bannerWidth) / 2,
			dropIt + 5 - kScoreSpacing - kKimsLifted);
	ForeColor(blackColor);
	FrameRect(&tempRect);
	QOffsetRect(&tempRect, -1, -1);
	ForeColor(yellowColor);
	FrameRect(&tempRect);

	for (i = 0; i < kMaxScores; i++)
	{
		if (thisHousePtr->highScores.scores[i] > 0L)
		{
			SpinCursor(1);
			NumToString((long)i + 1L, tempStr);		// draw placing number
			ForeColor(blackColor);
			if (i == 0)
				MoveTo(scoreLeft + 1, dropIt - kScoreSpacing - kKimsLifted);
			else
				MoveTo(scoreLeft + 1, dropIt + (i * kScoreSpacing));
			DrawString(tempStr);
			if (i == lastHighScore)
				ForeColor(whiteColor);
			else
				ForeColor(cyanColor);
			if (i == 0)
				MoveTo(scoreLeft + 0, dropIt - 1 - kScoreSpacing - kKimsLifted);
			else
				MoveTo(scoreLeft + 0, dropIt - 1 + (i * kScoreSpacing));
			DrawString(tempStr);
													// draw high score name
			PasStringCopy(thisHousePtr->highScores.names[i], tempStr);
			ForeColor(blackColor);
			if (i == 0)
				MoveTo(scoreLeft + 31, dropIt - kScoreSpacing - kKimsLifted);
			else
				MoveTo(scoreLeft + 31, dropIt + (i * kScoreSpacing));
			DrawString(tempStr);
			if (i == lastHighScore)
				ForeColor(whiteColor);
			else
				ForeColor(yellowColor);
			if (i == 0)
				MoveTo(scoreLeft + 30, dropIt - 1 - kScoreSpacing - kKimsLifted);
			else
				MoveTo(scoreLeft + 30, dropIt - 1 + (i * kScoreSpacing));
			DrawString(tempStr);
													// draw level number
			NumToString(thisHousePtr->highScores.levels[i], tempStr);
			ForeColor(blackColor);
			if (i == 0)
				MoveTo(scoreLeft + 161, dropIt - kScoreSpacing - kKimsLifted);
			else
				MoveTo(scoreLeft + 161, dropIt + (i * kScoreSpacing));
			DrawString(tempStr);
			if (i == lastHighScore)
				ForeColor(whiteColor);
			else
				ForeColor(yellowColor);
			if (i == 0)
				MoveTo(scoreLeft + 160, dropIt - 1 - kScoreSpacing - kKimsLifted);
			else
				MoveTo(scoreLeft + 160, dropIt - 1 + (i * kScoreSpacing));
			DrawString(tempStr);
													// draw word "rooms"
			if (thisHousePtr->highScores.levels[i] == 1)
				GetLocalizedString(6, tempStr);
			else
				GetLocalizedString(7, tempStr);
			ForeColor(blackColor);
			if (i == 0)
				MoveTo(scoreLeft + 193, dropIt - kScoreSpacing - kKimsLifted);
			else
				MoveTo(scoreLeft + 193, dropIt + (i * kScoreSpacing));
			DrawString(tempStr);
			ForeColor(cyanColor);
			if (i == 0)
				MoveTo(scoreLeft + 192, dropIt - 1 - kScoreSpacing - kKimsLifted);
			else
				MoveTo(scoreLeft + 192, dropIt - 1 + (i * kScoreSpacing));
			DrawString(tempStr);
													// draw high score points
			NumToString(thisHousePtr->highScores.scores[i], tempStr);
			ForeColor(blackColor);
			if (i == 0)
				MoveTo(scoreLeft + 291, dropIt - kScoreSpacing - kKimsLifted);
			else
				MoveTo(scoreLeft + 291, dropIt + (i * kScoreSpacing));
			DrawString(tempStr);
			if (i == lastHighScore)
				ForeColor(whiteColor);
			else
				ForeColor(yellowColor);
			if (i == 0)
				MoveTo(scoreLeft + 290, dropIt - 1 - kScoreSpacing - kKimsLifted);
			else
				MoveTo(scoreLeft + 290, dropIt - 1 + (i * kScoreSpacing));
			DrawString(tempStr);
		}
	}

	ForeColor(blueColor);
	TextFont(applFont);
	TextFace(bold);
	TextSize(9);
	MoveTo(scoreLeft + 80, dropIt - 1 + (10 * kScoreSpacing));
	GetLocalizedString(8, tempStr);
	DrawString(tempStr);

	ForeColor(blackColor);
	HSetState((Handle)thisHouse, wasState);
#endif
}

//--------------------------------------------------------------  SortHighScores
// This does a simple sort of the high scores.

void SortHighScores (void)
{
	return;
#if 0
	scoresType	tempScores;
	houseType	*thisHousePtr;
	long		greatest;
	short		i, h, which;
	char		wasState;

	wasState = HGetState((Handle)thisHouse);
	HLock((Handle)thisHouse);
	thisHousePtr = *thisHouse;

	for (h = 0; h < kMaxScores; h++)
	{
		greatest = -1L;
		which = -1;
		for (i = 0; i < kMaxScores; i++)
		{
			if (thisHousePtr->highScores.scores[i] > greatest)
			{
				greatest = thisHousePtr->highScores.scores[i];
				which = i;
			}
		}
		if (which != -1)
		{
			PasStringCopy(thisHousePtr->highScores.names[which], tempScores.names[h]);
			tempScores.scores[h] = thisHousePtr->highScores.scores[which];
			tempScores.timeStamps[h] = thisHousePtr->highScores.timeStamps[which];
			tempScores.levels[h] = thisHousePtr->highScores.levels[which];
			thisHousePtr->highScores.scores[which] = -1L;
		}
	}
	PasStringCopy(thisHousePtr->highScores.banner, tempScores.banner);
	thisHousePtr->highScores = tempScores;

	HSetState((Handle)thisHouse, wasState);
#endif
}

//--------------------------------------------------------------  ZeroHighScores
// This funciton goes through and resets or "zeros" all high scores.

void ZeroHighScores (void)
{
	return;
#if 0
	houseType	*thisHousePtr;
	short		i;
	char		wasState;

	wasState = HGetState((Handle)thisHouse);
	HLock((Handle)thisHouse);
	thisHousePtr = *thisHouse;

	PasStringCopy(thisHouseName, thisHousePtr->highScores.banner);
	for (i = 0; i < kMaxScores; i++)
	{
		PasStringCopy("\p--------------", thisHousePtr->highScores.names[i]);
		thisHousePtr->highScores.scores[i] = 0L;
		thisHousePtr->highScores.timeStamps[i] = 0L;
		thisHousePtr->highScores.levels[i] = 0;
	}

	HSetState((Handle)thisHouse, wasState);
#endif
}

//--------------------------------------------------------------  ZeroAllButHighestScore
// Like the above, but this function preserves the highest score.

void ZeroAllButHighestScore (void)
{
	return;
#if 0
	houseType	*thisHousePtr;
	short		i;
	char		wasState;

	wasState = HGetState((Handle)thisHouse);
	HLock((Handle)thisHouse);
	thisHousePtr = *thisHouse;

	for (i = 1; i < kMaxScores; i++)
	{
		PasStringCopy("\p--------------", thisHousePtr->highScores.names[i]);
		thisHousePtr->highScores.scores[i] = 0L;
		thisHousePtr->highScores.timeStamps[i] = 0L;
		thisHousePtr->highScores.levels[i] = 0;
	}

	HSetState((Handle)thisHouse, wasState);
#endif
}

//--------------------------------------------------------------  TestHighScore
// This function is called after a game ends in order to test the…
// current high score against the high score list.  It returns true…
// if the player is on the high score list now.

Boolean TestHighScore (void)
{
	return (false);
#if 0
	houseType	*thisHousePtr;
	short		placing, i;
	char		wasState;

	if (resumedSavedGame)
		return (false);

	wasState = HGetState((Handle)thisHouse);
	HLock((Handle)thisHouse);
	thisHousePtr = *thisHouse;

	lastHighScore = -1;
	placing = -1;

	for (i = 0; i < kMaxScores; i++)
	{
		if (theScore > thisHousePtr->highScores.scores[i])
		{
			placing = i;
			lastHighScore = i;
			break;
		}
	}

	if (placing != -1)
	{
		FlushEvents(everyEvent, 0);
		GetHighScoreName(placing + 1);
		PasStringCopy(highName, thisHousePtr->highScores.names[kMaxScores - 1]);
		if (placing == 0)
		{
			GetHighScoreBanner();
			PasStringCopy(highBanner, thisHousePtr->highScores.banner);
		}
		thisHousePtr->highScores.scores[kMaxScores - 1] = theScore;
		GetDateTime(&thisHousePtr->highScores.timeStamps[kMaxScores - 1]);
		thisHousePtr->highScores.levels[kMaxScores - 1] = CountRoomsVisited();
		SortHighScores();
		gameDirty = true;
	}

	HSetState((Handle)thisHouse, wasState);

	if (placing != -1)
	{
		DoHighScores();
		return (true);
	}
	else
		return (false);
#endif
}

//--------------------------------------------------------------  UpdateNameDialog
// Redraws the "Enter High Score Name" dialog.

void UpdateNameDialog (DialogPtr theDialog)
{
	return;
#if 0
	short		nChars;

	DrawDialog(theDialog);
	DrawDefaultButton(theDialog);

	nChars = GetDialogStringLen(theDialog, kHighNameItem);
	SetDialogNumToStr(theDialog, kNameNCharsItem, (long)nChars);
#endif
}

//--------------------------------------------------------------  NameFilter
// Dialog filter for the "Enter High Score Name" dialog.

pascal Boolean NameFilter (DialogPtr dial, EventRecord *event, SInt16 *item)
{
	return (false);
#if 0
	short		nChars;

	if (keyStroke)
	{
		nChars = GetDialogStringLen(dial, kHighNameItem);
		SetDialogNumToStr(dial, kNameNCharsItem, (long)nChars);
		keyStroke = false;
	}

	switch (event->what)
	{
		case keyDown:
		keyStroke = true;
		switch ((event->message) & charCodeMask)
		{
			case kReturnKeyASCII:
			case kEnterKeyASCII:
			PlayPrioritySound(kCarriageSound, kCarriagePriority);
			FlashDialogButton(dial, kOkayButton);
			*item = kOkayButton;
			return(true);
			break;

			case kTabKeyASCII:
			SelectDialogItemText(dial, kHighNameItem, 0, 1024);
			return(false);
			break;

			default:
			PlayPrioritySound(kTypingSound, kTypingPriority);
			return(false);
		}
		break;

		case updateEvt:
		BeginUpdate(GetDialogWindow(dial));
		UpdateNameDialog(dial);
		EndUpdate(GetDialogWindow(dial));
		event->what = nullEvent;
		return(false);
		break;

		default:
		return(false);
		break;
	}
#endif
}

//--------------------------------------------------------------  GetHighScoreName
// Brings up a dialog to get player's name (due to a high score).

void GetHighScoreName (SInt16 place)
{
	return;
#if 0
	DialogPtr		theDial;
	Str255			scoreStr, placeStr, tempStr;
	short			item;
	Boolean			leaving;
	ModalFilterUPP	nameFilterUPP;

	nameFilterUPP = NewModalFilterUPP(NameFilter);

	InitCursor();
	NumToString(theScore, scoreStr);
	NumToString((long)place, placeStr);
	ParamText(scoreStr, placeStr, thisHouseName, "\p");
	PlayPrioritySound(kEnergizeSound, kEnergizePriority);
	BringUpDialog(&theDial, kHighNameDialogID);
	FlushEvents(everyEvent, 0);
	SetDialogString(theDial, kHighNameItem, highName);
	SelectDialogItemText(theDial, kHighNameItem, 0, 1024);
	leaving = false;

	while (!leaving)
	{
		ModalDialog(nameFilterUPP, &item);

		if (item == kOkayButton)
		{
			GetDialogString(theDial, kHighNameItem, tempStr);
			PasStringCopyNum(tempStr, highName, 15);
			leaving = true;
		}
	}

	DisposeDialog(theDial);
	DisposeModalFilterUPP(nameFilterUPP);
#endif
}

//--------------------------------------------------------------  UpdateBannerDialog
// Redraws the "Enter Message" dialog.

void UpdateBannerDialog (DialogPtr theDialog)
{
	return;
#if 0
	short		nChars;

	DrawDialog(theDialog);
	DrawDefaultButton(theDialog);

	nChars = GetDialogStringLen(theDialog, kHighBannerItem);
	SetDialogNumToStr(theDialog, kBannerScoreNCharsItem, (long)nChars);
#endif
}

//--------------------------------------------------------------  BannerFilter
// Dialog filter for the "Enter Message" dialog.

pascal Boolean BannerFilter (DialogPtr dial, EventRecord *event, SInt16 *item)
{
	return (false);
#if 0
	short		nChars;

	if (keyStroke)
	{
		nChars = GetDialogStringLen(dial, kHighBannerItem);
		SetDialogNumToStr(dial, kBannerScoreNCharsItem, (long)nChars);
		keyStroke = false;
	}

	switch (event->what)
	{

		case keyDown:
		keyStroke = true;
		switch ((event->message) & charCodeMask)
		{
			case kReturnKeyASCII:
			case kEnterKeyASCII:
			PlayPrioritySound(kCarriageSound, kCarriagePriority);
			FlashDialogButton(dial, kOkayButton);
			*item = kOkayButton;
			return(true);
			break;

			case kTabKeyASCII:
			SelectDialogItemText(dial, kHighBannerItem, 0, 1024);
			return(false);
			break;

			default:
			PlayPrioritySound(kTypingSound, kTypingPriority);
			return(false);
		}
		break;

		case updateEvt:
		BeginUpdate(GetDialogWindow(dial));
		UpdateBannerDialog(dial);
		EndUpdate(GetDialogWindow(dial));
		event->what = nullEvent;
		return(false);
		break;

		default:
		return(false);
		break;
	}
#endif
}

//--------------------------------------------------------------  GetHighScoreBanner
// A player who gets the #1 slot gets to enter a short message (that…
// appears across the top of the high scores list).  This dialog…
// gets that message.

void GetHighScoreBanner (void)
{
	return;
#if 0
	DialogPtr		theDial;
	Str255			tempStr;
	short			item;
	Boolean			leaving;
	ModalFilterUPP	bannerFilterUPP;

	bannerFilterUPP = NewModalFilterUPP(BannerFilter);

	PlayPrioritySound(kEnergizeSound, kEnergizePriority);
	BringUpDialog(&theDial, kHighBannerDialogID);
	SetDialogString(theDial, kHighBannerItem, highBanner);
	SelectDialogItemText(theDial, kHighBannerItem, 0, 1024);
	leaving = false;

	while (!leaving)
	{
		ModalDialog(bannerFilterUPP, &item);

		if (item == kOkayButton)
		{
			GetDialogString(theDial, kHighBannerItem, tempStr);
			PasStringCopyNum(tempStr, highBanner, 31);
			leaving = true;
		}
	}

	DisposeDialog(theDial);
	DisposeModalFilterUPP(bannerFilterUPP);
#endif
}

//--------------------------------------------------------------  CreateScoresFolder

Boolean CreateScoresFolder (SInt32 *scoresDirID)
{
	return (false);
#if 0
	FSSpec		scoresSpec;
	long		prefsDirID;
	OSErr		theErr;
	short		volRefNum;

	theErr = FindFolder(kOnSystemDisk, kPreferencesFolderType, kCreateFolder,
			&volRefNum, &prefsDirID);
	if (!CheckFileError(theErr, "\pPrefs Folder"))
		return (false);

	theErr = FSMakeFSSpec(volRefNum, prefsDirID, "\pG-PRO Scores ƒ", &scoresSpec);

	theErr = FSpDirCreate(&scoresSpec, smSystemScript, scoresDirID);
	if (!CheckFileError(theErr, "\pHigh Scores Folder"))
		return (false);

	return (true);
#endif
}

//--------------------------------------------------------------  FindHighScoresFolder

Boolean FindHighScoresFolder (SInt16 *volRefNum, SInt32 *scoresDirID)
{
	return false;
#if 0
	CInfoPBRec	theBlock;
	Str255		nameString;
	long		prefsDirID;
	OSErr		theErr;
	short		count;
	Boolean		foundIt;

	theErr = FindFolder(kOnSystemDisk, kPreferencesFolderType, kCreateFolder,
			volRefNum, &prefsDirID);
	if (!CheckFileError(theErr, "\pPrefs Folder"))
		return (false);

	PasStringCopy("\pG-PRO Scores ƒ", nameString);
	count = 1;
	foundIt = false;

	theBlock.dirInfo.ioCompletion = nil;
	theBlock.dirInfo.ioVRefNum = *volRefNum;
	theBlock.dirInfo.ioNamePtr = nameString;

	while ((theErr == noErr) && (!foundIt))
	{
		theBlock.dirInfo.ioFDirIndex = count;
		theBlock.dirInfo.ioDrDirID = prefsDirID;
		theErr = PBGetCatInfo(&theBlock, false);
		if (theErr == noErr)
		{
			if ((theBlock.dirInfo.ioFlAttrib & 0x10) == 0x10)
			{
				if (EqualString(theBlock.dirInfo.ioNamePtr, "\pG-PRO Scores ƒ",
						true, true))
				{
					foundIt = true;
					*scoresDirID = theBlock.dirInfo.ioDrDirID;
				}
			}
			count++;
		}
	}

	if (theErr == fnfErr)
	{
		if (CreateScoresFolder(scoresDirID))
			return (true);
		else
			return (false);
	}
	else
		return (true);
#endif
}

//--------------------------------------------------------------  OpenHighScoresFile

Boolean OpenHighScoresFile (FSSpec *scoreSpec, SInt16 *scoresRefNum)
{
	return false;
#if 0
	OSErr		theErr;

	theErr = FSpOpenDF(scoreSpec, fsCurPerm, scoresRefNum);
	if (theErr == fnfErr)
	{
		theErr = FSpCreate(scoreSpec, 'ozm5', 'gliS', smSystemScript);
		if (!CheckFileError(theErr, "\pNew High Scores File"))
			return (false);
		theErr = FSpOpenDF(scoreSpec, fsCurPerm, scoresRefNum);
		if (!CheckFileError(theErr, "\pHigh Score"))
			return (false);
	}
	else if (!CheckFileError(theErr, "\pHigh Score"))
		return (false);

	return (true);
#endif
}

//--------------------------------------------------------------  WriteScoresToDisk

Boolean WriteScoresToDisk (void)
{
	return false;
#if 0
	scoresType	*theScores;
	FSSpec		scoreSpec;
	long		dirID, byteCount;
	OSErr		theErr;
	short		volRefNum, scoresRefNum;
	char		wasState;

	if (!FindHighScoresFolder(&volRefNum, &dirID))
	{
		SysBeep(1);
		return (false);
	}

	theErr = FSMakeFSSpec(volRefNum, dirID, thisHouseName, &scoreSpec);
	if (!OpenHighScoresFile(&scoreSpec, &scoresRefNum))
	{
		SysBeep(1);
		return (false);
	}

	theErr = SetFPos(scoresRefNum, fsFromStart, 0L);
	if (!CheckFileError(theErr, "\pHigh Scores File"))
	{
		theErr = FSClose(scoresRefNum);
		return(false);
	}

	byteCount = sizeof(scoresType);
	wasState = HGetState((Handle)thisHouse);
	HLock((Handle)thisHouse);
	theScores = &((*thisHouse)->highScores);

	theErr = FSWrite(scoresRefNum, &byteCount, (Ptr)theScores);
	if (!CheckFileError(theErr, "\pHigh Scores File"))
	{
		HSetState((Handle)thisHouse, wasState);
		theErr = FSClose(scoresRefNum);
		return(false);
	}
	HSetState((Handle)thisHouse, wasState);

	theErr = SetEOF(scoresRefNum, byteCount);
	if (!CheckFileError(theErr, "\pHigh Scores File"))
	{
		theErr = FSClose(scoresRefNum);
		return(false);
	}

	theErr = FSClose(scoresRefNum);
	if (!CheckFileError(theErr, "\pHigh Scores File"))
		return(false);

	return (true);
#endif
}

//--------------------------------------------------------------  ReadScoresFromDisk

Boolean ReadScoresFromDisk (void)
{
	return false;
#if 0
	scoresType	*theScores;
	FSSpec		scoreSpec;
	long		dirID, byteCount;
	OSErr		theErr;
	short		volRefNum, scoresRefNum;
	char		wasState;

	if (!FindHighScoresFolder(&volRefNum, &dirID))
	{
		SysBeep(1);
		return (false);
	}

	theErr = FSMakeFSSpec(volRefNum, dirID, thisHouseName, &scoreSpec);
	if (!OpenHighScoresFile(&scoreSpec, &scoresRefNum))
	{
		SysBeep(1);
		return (false);
	}

	theErr = GetEOF(scoresRefNum, &byteCount);
	if (!CheckFileError(theErr, "\pHigh Scores File"))
	{
		theErr = FSClose(scoresRefNum);
		return (false);
	}

	theErr = SetFPos(scoresRefNum, fsFromStart, 0L);
	if (!CheckFileError(theErr, "\pHigh Scores File"))
	{
		theErr = FSClose(scoresRefNum);
		return (false);
	}

	wasState = HGetState((Handle)thisHouse);
	HLock((Handle)thisHouse);
	theScores = &((*thisHouse)->highScores);

	theErr = FSRead(scoresRefNum, &byteCount, theScores);
	if (!CheckFileError(theErr, "\pHigh Scores File"))
	{
		HSetState((Handle)thisHouse, wasState);
		theErr = FSClose(scoresRefNum);
		return (false);
	}
	HSetState((Handle)thisHouse, wasState);

	theErr = FSClose(scoresRefNum);
	if (!CheckFileError(theErr, "\pHigh Scores File"))
		return(false);

	return (true);
#endif
}

