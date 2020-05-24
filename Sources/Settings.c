
//============================================================================
//----------------------------------------------------------------------------
//									Settings.c
//----------------------------------------------------------------------------
//============================================================================


//#include <Sound.h>
//#include <TextUtils.h>
#include "Macintosh.h"
#include "DialogUtils.h"
#include "Externs.h"
#include "Environ.h"
#include "House.h"
#include "ResourceIDs.h"
#include <shlwapi.h> // for DLLGETVERSIONPROC and DLLGETVERSIONINFO


#define kDisplayButton			1003
#define kSoundButton			1004
#define kControlsButton			1005
#define kBrainsButton			1006
#define kAllDefaultsButton		1011

#define kDisplay1Item			1003
#define kDisplay3Item			1004
#define kDisplay9Item			1005
#define kDoColorFadeItem		1009
#define kCurrentDepth			1010
#define k256Depth				1011
#define k16Depth				1012
#define kDispDefault			1015
#define kUseQDItem				1016
#define kUseScreen2Item			1017

#define kSofterItem				1004
#define kLouderItem				1005
#define kVolNumberItem			1007
#define kIdleMusicItem			1008
#define kPlayMusicItem			1009
#define kSoundDefault			1013

#define kRightControl			1005
#define kLeftControl			1006
#define kBattControl			1007
#define kBandControl			1008
#define kControlDefaults		1013
#define kESCPausesRadio			1014
#define kTABPausesRadio			1015

#define kMaxFilesItem			1005
#define kQuickTransitCheck		1007
#define kDoZoomsCheck			1008
#define kBrainsDefault			1009
#define kDoDemoCheck			1010
#define kDoBackgroundCheck		1011
#define kDoErrorCheck			1012
#define kDoPrettyMapCheck		1013
#define kDoBitchDlgsCheck		1014


void SetBrainsToDefaults (DialogPtr);
void UpdateSettingsBrains (DialogPtr);
Boolean BrainsFilter (DialogPtr, EventRecord *, SInt16 *);
void DoBrainsPrefs (HWND);
void SetControlsToDefaults (DialogPtr);
void UpdateControlKeyName (DialogPtr);
void UpdateSettingsControl (DialogPtr);
Boolean ControlFilter (DialogPtr, EventRecord *, SInt16 *);
void DoControlPrefs (HWND);
void SoundDefaults (DialogPtr);
void UpdateSettingsSound (DialogPtr);
void HandleSoundMusicChange (SInt16, Boolean);
Boolean SoundFilter (DialogPtr, EventRecord *, SInt16 *);
void DoSoundPrefs (HWND);
void DisplayDefaults (HWND);
void DisplayInit (HWND);
void DisplayApply (HWND);
INT_PTR CALLBACK DisplayFilter (HWND, UINT, WPARAM, LPARAM);
void DoDisplayPrefs (HWND);
void SetAllDefaults (HWND);
//void FlashSettingsButton (SInt16);
//void UpdateSettingsMain (DialogPtr);
INT_PTR CALLBACK PrefsFilter (HWND, UINT, WPARAM, LPARAM);
void BitchAboutChanges (HWND);


//Rect		prefButton[4];
Rect		controlRects[4];
Str15		leftName, rightName, batteryName, bandName;
Str15		tempLeftStr, tempRightStr, tempBattStr, tempBandStr;
SInt32		tempLeftMap, tempRightMap, tempBattMap, tempBandMap;
SInt16		whichCtrl;
Boolean		wasIdle, wasPlay, wasTransit, wasZooms, wasBackground;
Boolean		wasEscPauseKey, wasDemos, nextRestartChange, wasErrorCheck;
Boolean		wasPrettyMap, wasBitchDialogs;

extern	SInt16		numNeighbors, isDepthPref, maxFiles, willMaxFiles;
extern	Boolean		isDoColorFade, isPlayMusicIdle, isUseSecondScreen;
extern	Boolean		isHouseChecks, doBitchDialogs;
extern	Boolean		isEscPauseKey, failedMusic, isSoundOn, doBackground;
extern	Boolean		isMusicOn, quickerTransitions, doAutoDemo;
extern	Boolean		changeLockStateOfHouse, saveHouseLocked, doPrettyMap;


//==============================================================  Functions
//--------------------------------------------------------------  SetBrainsToDefaults

void SetBrainsToDefaults (DialogPtr theDialog)
{
	return;
#if 0
	SetDialogNumToStr(theDialog, kMaxFilesItem, 24L);
#ifdef powerc
	wasTransit = false;
#else
	wasTransit = true;
#endif
	wasZooms = true;
	wasDemos = true;
	wasBackground = false;
	wasErrorCheck = true;
	wasPrettyMap = true;
	wasBitchDialogs = true;
	SetDialogItemValue(theDialog, kQuickTransitCheck, (short)wasTransit);
	SetDialogItemValue(theDialog, kDoZoomsCheck, (short)wasZooms);
	SetDialogItemValue(theDialog, kDoDemoCheck, (short)wasDemos);
	SetDialogItemValue(theDialog, kDoBackgroundCheck, (short)wasBackground);
	SetDialogItemValue(theDialog, kDoErrorCheck, (short)wasErrorCheck);
	SetDialogItemValue(theDialog, kDoPrettyMapCheck, (short)wasPrettyMap);
	SetDialogItemValue(theDialog, kDoBitchDlgsCheck, (short)wasBitchDialogs);
#endif
}

//--------------------------------------------------------------  UpdateSettingsBrains

void UpdateSettingsBrains (DialogPtr theDialog)
{
	return;
#if 0
	DrawDialog(theDialog);
	DrawDefaultButton(theDialog);

	SetDialogNumToStr(theDialog, kMaxFilesItem, (long)willMaxFiles);
	SelectDialogItemText(theDialog, kMaxFilesItem, 0, 1024);

	FrameDialogItemC(theDialog, 3, kRedOrangeColor8);
#endif
}

//--------------------------------------------------------------  BrainsFilter

Boolean BrainsFilter (DialogPtr dial, EventRecord *event, SInt16 *item)
{
	return false;
#if 0
	switch (event->what)
	{
		case keyDown:
		switch ((event->message) & charCodeMask)
		{
			case kReturnKeyASCII:
			case kEnterKeyASCII:
			FlashDialogButton(dial, kOkayButton);
			*item = kOkayButton;
			return(true);
			break;

			case kEscapeKeyASCII:
			FlashDialogButton(dial, kCancelButton);
			*item = kCancelButton;
			return(true);
			break;

			case kCapAKeyASCII:
			case kAKeyASCII:
			*item = kDoDemoCheck;
			return(true);
			break;

			case kCapBKeyASCII:
			case kBKeyASCII:
			*item = kDoBackgroundCheck;
			return(true);
			break;

			case kCapDKeyASCII:
			case kDKeyASCII:
			*item = kBrainsDefault;
			FlashDialogButton(dial, kBrainsDefault);
			return(true);
			break;

			case kCapEKeyASCII:
			case kEKeyASCII:
			*item = kDoErrorCheck;
			return(true);
			break;

			case kCapQKeyASCII:
			case kQKeyASCII:
			*item = kQuickTransitCheck;
			return(true);
			break;

			case kCapZKeyASCII:
			case kZKeyASCII:
			*item = kDoZoomsCheck;
			return(true);
			break;

			default:
			return(false);
		}
		break;

		case mouseDown:
		return(false);
		break;

		case updateEvt:
		SetPort((GrafPtr)dial);
		BeginUpdate(GetDialogWindow(dial));
		UpdateSettingsBrains(dial);
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

//--------------------------------------------------------------  DoBrainsPrefs

void DoBrainsPrefs (HWND ownerWindow)
{
	MessageBox(ownerWindow, L"DoBrainsPrefs()", NULL, MB_ICONHAND);
	return;
#if 0
	DialogPtr		prefDlg;
	long			tempLong;
	short			itemHit, wasMaxFiles;
	Boolean			leaving;
	ModalFilterUPP	brainsFilterUPP;

	brainsFilterUPP = NewModalFilterUPP(BrainsFilter);

	BringUpDialog(&prefDlg, kBrainsPrefsDialID);
	leaving = false;
	wasMaxFiles = willMaxFiles;

	wasTransit = quickerTransitions;
	wasZooms = doZooms;
	wasDemos = doAutoDemo;
	wasBackground = doBackground;
	wasErrorCheck = isHouseChecks;
	wasPrettyMap = doPrettyMap;
	wasBitchDialogs = doBitchDialogs;

	SetDialogItemValue(prefDlg, kQuickTransitCheck, (short)wasTransit);
	SetDialogItemValue(prefDlg, kDoZoomsCheck, (short)wasZooms);
	SetDialogItemValue(prefDlg, kDoDemoCheck, (short)wasDemos);
	SetDialogItemValue(prefDlg, kDoBackgroundCheck, (short)wasBackground);
	SetDialogItemValue(prefDlg, kDoErrorCheck, (short)wasErrorCheck);
	SetDialogItemValue(prefDlg, kDoPrettyMapCheck, (short)wasPrettyMap);
	SetDialogItemValue(prefDlg, kDoBitchDlgsCheck, (short)wasBitchDialogs);

	while (!leaving)
	{
		ModalDialog(brainsFilterUPP, &itemHit);
		switch (itemHit)
		{
			case kOkayButton:
			GetDialogNumFromStr(prefDlg, kMaxFilesItem, &tempLong);
			if (tempLong > 500)
				tempLong = 500;
			else if (tempLong < 12)
				tempLong = 12;
			willMaxFiles = tempLong;
			if (willMaxFiles != wasMaxFiles)
				nextRestartChange = true;
			quickerTransitions = wasTransit;
			doZooms = wasZooms;
			doAutoDemo = wasDemos;
			doBackground = wasBackground;
			isHouseChecks = wasErrorCheck;
			doPrettyMap = wasPrettyMap;
			doBitchDialogs = wasBitchDialogs;
			leaving = true;
			break;

			case kCancelButton:
			willMaxFiles = wasMaxFiles;
			leaving = true;
			break;

			case kQuickTransitCheck:
			wasTransit = !wasTransit;
			SetDialogItemValue(prefDlg, kQuickTransitCheck, (short)wasTransit);
			break;

			case kDoZoomsCheck:
			wasZooms = !wasZooms;
			SetDialogItemValue(prefDlg, kDoZoomsCheck, (short)wasZooms);
			break;

			case kDoDemoCheck:
			wasDemos = !wasDemos;
			SetDialogItemValue(prefDlg, kDoDemoCheck, (short)wasDemos);
			break;

			case kDoBackgroundCheck:
			wasBackground = !wasBackground;
			SetDialogItemValue(prefDlg, kDoBackgroundCheck, (short)wasBackground);
			break;

			case kBrainsDefault:
			SetBrainsToDefaults(prefDlg);
			break;

			case kDoErrorCheck:
			wasErrorCheck = !wasErrorCheck;
			SetDialogItemValue(prefDlg, kDoErrorCheck, (short)wasErrorCheck);
			break;

			case kDoPrettyMapCheck:
			wasPrettyMap = !wasPrettyMap;
			SetDialogItemValue(prefDlg, kDoPrettyMapCheck, (short)wasPrettyMap);
			break;

			case kDoBitchDlgsCheck:
			wasBitchDialogs = !wasBitchDialogs;
			SetDialogItemValue(prefDlg, kDoBitchDlgsCheck, (short)wasBitchDialogs);
			break;
		}
	}

	DisposeDialog(prefDlg);
	DisposeModalFilterUPP(brainsFilterUPP);
#endif
}

//--------------------------------------------------------------  SetControlsToDefaults

void SetControlsToDefaults (DialogPtr theDialog)
{
	return;
#if 0
	PasStringCopy("\plf arrow", tempLeftStr);
	PasStringCopy("\prt arrow", tempRightStr);
	PasStringCopy("\pdn arrow", tempBattStr);
	PasStringCopy("\pup arrow", tempBandStr);
	tempLeftMap = kLeftArrowKeyMap;
	tempRightMap = kRightArrowKeyMap;
	tempBattMap = kDownArrowKeyMap;
	tempBandMap = kUpArrowKeyMap;
	wasEscPauseKey = false;
	SelectFromRadioGroup(theDialog, kTABPausesRadio,
				kESCPausesRadio, kTABPausesRadio);
#endif
}

//--------------------------------------------------------------  UpdateControlKeyName

void UpdateControlKeyName (DialogPtr theDialog)
{
	return;
#if 0
	DrawDialogUserText(theDialog, kRightControl + 4, tempRightStr, whichCtrl == 0);
	DrawDialogUserText(theDialog, kLeftControl + 4, tempLeftStr, whichCtrl == 1);
	DrawDialogUserText(theDialog, kBattControl + 4, tempBattStr, whichCtrl == 2);
	DrawDialogUserText(theDialog, kBandControl + 4, tempBandStr, whichCtrl == 3);
#endif
}

//--------------------------------------------------------------  UpdateSettingsControl

void UpdateSettingsControl (DialogPtr theDialog)
{
	return;
#if 0
	short		i;

	DrawDialog(theDialog);

	PenSize(2, 2);
	ForeColor(whiteColor);
	for (i = 0; i < 4; i++)
		FrameRect(&controlRects[i]);
	ForeColor(redColor);
	FrameRect(&controlRects[whichCtrl]);
	ForeColor(blackColor);
	PenNormal();
	UpdateControlKeyName(theDialog);
	FrameDialogItemC(theDialog, 3, kRedOrangeColor8);
#endif
}

//--------------------------------------------------------------  ControlFilter

Boolean ControlFilter (DialogPtr dial, EventRecord *event, SInt16 *item)
{
	return false;
#if 0
	long		wasKeyMap;

	switch (event->what)
	{
		case keyDown:
		switch (whichCtrl)
		{
			case 0:
			wasKeyMap = (long)GetKeyMapFromMessage(event->message);
			if ((wasKeyMap == tempLeftMap) || (wasKeyMap == tempBattMap) ||
					(wasKeyMap == tempBandMap) || (wasKeyMap == kTabKeyMap) ||
					(wasKeyMap == kEscKeyMap) || (wasKeyMap == kDeleteKeyMap))
			{
				if (wasKeyMap == kEscKeyMap)
				{
					FlashDialogButton(dial, kCancelButton);
					*item = kCancelButton;
					return(true);
				}
				else
					SysBeep(1);
			}
			else
			{
				GetKeyName(event->message, tempRightStr);
				tempRightMap = wasKeyMap;
			}
			break;

			case 1:
			wasKeyMap = (long)GetKeyMapFromMessage(event->message);
			if ((wasKeyMap == tempRightMap) || (wasKeyMap == tempBattMap) ||
					(wasKeyMap == tempBandMap) || (wasKeyMap == kTabKeyMap) ||
					(wasKeyMap == kEscKeyMap) || (wasKeyMap == kDeleteKeyMap))
			{
				if (wasKeyMap == kEscKeyMap)
				{
					FlashDialogButton(dial, kCancelButton);
					*item = kCancelButton;
					return(true);
				}
				else
					SysBeep(1);
			}
			else
			{
				GetKeyName(event->message, tempLeftStr);
				tempLeftMap = wasKeyMap;
			}
			break;

			case 2:
			wasKeyMap = (long)GetKeyMapFromMessage(event->message);
			if ((wasKeyMap == tempRightMap) || (wasKeyMap == tempLeftMap) ||
					(wasKeyMap == tempBandMap) || (wasKeyMap == kTabKeyMap) ||
					(wasKeyMap == kEscKeyMap) || (wasKeyMap == kDeleteKeyMap))
			{
				if (wasKeyMap == kEscKeyMap)
				{
					FlashDialogButton(dial, kCancelButton);
					*item = kCancelButton;
					return(true);
				}
				else
					SysBeep(1);
			}
			else
			{
				GetKeyName(event->message, tempBattStr);
				tempBattMap = wasKeyMap;
			}
			break;

			case 3:
			wasKeyMap = (long)GetKeyMapFromMessage(event->message);
			if ((wasKeyMap == tempRightMap) || (wasKeyMap == tempLeftMap) ||
					(wasKeyMap == tempBattMap) || (wasKeyMap == kTabKeyMap) ||
					(wasKeyMap == kEscKeyMap) || (wasKeyMap == kDeleteKeyMap))
			{
				if (wasKeyMap == kEscKeyMap)
				{
					FlashDialogButton(dial, kCancelButton);
					*item = kCancelButton;
					return(true);
				}
				else
					SysBeep(1);
			}
			else
			{
				GetKeyName(event->message, tempBandStr);
				tempBandMap = wasKeyMap;
			}
			break;
		}
		UpdateControlKeyName(dial);
		return(false);
		break;

		case mouseDown:
		return(false);
		break;

		case updateEvt:
		SetPort((GrafPtr)dial);
		BeginUpdate(GetDialogWindow(dial));
		UpdateSettingsControl(dial);
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

//--------------------------------------------------------------  DoControlPrefs

void DoControlPrefs (HWND ownerWindow)
{
	MessageBox(ownerWindow, L"DoControlPrefs()", NULL, MB_ICONHAND);
	return;
#if 0
	DialogPtr		prefDlg;
	short			i, itemHit;
	Boolean			leaving;
	ModalFilterUPP	controlFilterUPP;

	controlFilterUPP = NewModalFilterUPP(ControlFilter);

//	CenterDialog(kControlPrefsDialID);
	prefDlg = GetNewDialog(kControlPrefsDialID, nil, kPutInFront);
	if (prefDlg == nil)
		RedAlert(kErrDialogDidntLoad);
	SetPort((GrafPtr)prefDlg);
	for (i = 0; i < 4; i++)
	{
		GetDialogItemRect(prefDlg, i + kRightControl, &controlRects[i]);
		InsetRect(&controlRects[i], -3, -3);
	}
	whichCtrl = 1;

	PasStringCopy(leftName, tempLeftStr);
	PasStringCopy(rightName, tempRightStr);
	PasStringCopy(batteryName, tempBattStr);
	PasStringCopy(bandName, tempBandStr);
	tempLeftMap = theGlider.leftKey;
	tempRightMap = theGlider.rightKey;
	tempBattMap = theGlider.battKey;
	tempBandMap = theGlider.bandKey;
	wasEscPauseKey = isEscPauseKey;

	leaving = false;

	ShowWindow(GetDialogWindow(prefDlg));
	if (isEscPauseKey)
		SelectFromRadioGroup(prefDlg, kESCPausesRadio,
				kESCPausesRadio, kTABPausesRadio);
	else
		SelectFromRadioGroup(prefDlg, kTABPausesRadio,
				kESCPausesRadio, kTABPausesRadio);

	while (!leaving)
	{
		ModalDialog(controlFilterUPP, &itemHit);
		switch (itemHit)
		{
			case kOkayButton:
			PasStringCopy(tempLeftStr, leftName);
			PasStringCopy(tempRightStr, rightName);
			PasStringCopy(tempBattStr, batteryName);
			PasStringCopy(tempBandStr, bandName);
			theGlider.leftKey = tempLeftMap;
			theGlider.rightKey = tempRightMap;
			theGlider.battKey = tempBattMap;
			theGlider.bandKey = tempBandMap;
			isEscPauseKey = wasEscPauseKey;
			leaving = true;
			break;

			case kCancelButton:
			leaving = true;
			break;

			case kRightControl:
			case kLeftControl:
			case kBattControl:
			case kBandControl:
			PenSize(2, 2);
			ForeColor(whiteColor);
			FrameRect(&controlRects[whichCtrl]);
			whichCtrl = itemHit - kRightControl;
			ForeColor(redColor);
			FrameRect(&controlRects[whichCtrl]);
			ForeColor(blackColor);
			PenNormal();
			UpdateControlKeyName(prefDlg);
			break;

			case kESCPausesRadio:
			case kTABPausesRadio:
			SelectFromRadioGroup(prefDlg, itemHit, kESCPausesRadio, kTABPausesRadio);
			wasEscPauseKey = !wasEscPauseKey;
			break;

			case kControlDefaults:
			SetControlsToDefaults(prefDlg);
			UpdateControlKeyName(prefDlg);
			break;
		}
	}

	DisposeDialog(prefDlg);
	DisposeModalFilterUPP(controlFilterUPP);
#endif
}

//--------------------------------------------------------------  SoundDefaults

void SoundDefaults (DialogPtr theDialog)
{
	return;
#if 0
	wasIdle = true;
	wasPlay = true;
	SetDialogItemValue(theDialog, kIdleMusicItem, (short)wasIdle);
	SetDialogItemValue(theDialog, kPlayMusicItem, (short)wasPlay);
	UnivSetSoundVolume(3, thisMac.hasSM3);
	SetDialogNumToStr(theDialog, kVolNumberItem, 3L);
	HandleSoundMusicChange(3, true);
#endif
}

//--------------------------------------------------------------  UpdateSettingsSound

void UpdateSettingsSound (DialogPtr theDialog)
{
	return;
#if 0
	short		howLoudNow;

	DrawDialog(theDialog);
	DrawDefaultButton(theDialog);

	UnivGetSoundVolume(&howLoudNow, thisMac.hasSM3);

	if (howLoudNow >= 7)
		SetDialogNumToStr(theDialog, kVolNumberItem, 11L);
	else
		SetDialogNumToStr(theDialog, kVolNumberItem, (long)howLoudNow);

	FrameDialogItemC(theDialog, 11, kRedOrangeColor8);
#endif
}

//--------------------------------------------------------------  HandleSoundMusicChange

void HandleSoundMusicChange (SInt16 newVolume, Boolean sayIt)
{
	return;
#if 0
	OSErr		theErr;

	isSoundOn = (newVolume != 0);

	if (wasIdle)
	{
		if (newVolume == 0)
			StopTheMusic();
		else
		{
			if (!isMusicOn)
			{
				theErr = StartMusic();
				if (theErr != noErr)
				{
					YellowAlert(kYellowNoMusic, theErr);
					failedMusic = true;
				}
			}
		}
	}

	if ((newVolume != 0) && (sayIt))
		PlayPrioritySound(kChord2Sound, kChord2Priority);
#endif
}

//--------------------------------------------------------------  SoundFilter

Boolean SoundFilter (DialogPtr dial, EventRecord *event, SInt16 *item)
{
	return false;
#if 0
	short		newVolume;

	switch (event->what)
	{
		case keyDown:
		switch ((event->message) & charCodeMask)
		{
			case kReturnKeyASCII:
			case kEnterKeyASCII:
			FlashDialogButton(dial, kOkayButton);
			*item = kOkayButton;
			return(true);
			break;

			case kEscapeKeyASCII:
			FlashDialogButton(dial, kCancelButton);
			*item = kCancelButton;
			return(true);
			break;

			case kUpArrowKeyASCII:
			*item = kLouderItem;
			return(true);
			break;

			case kDownArrowKeyASCII:
			*item = kSofterItem;
			return(true);
			break;

			case k0KeyASCII:
			case k1KeyASCII:
			case k2KeyASCII:
			case k3KeyASCII:
			case k4KeyASCII:
			case k5KeyASCII:
			case k6KeyASCII:
			case k7KeyASCII:
			newVolume = (((event->message) & charCodeMask) - k0KeyASCII);
			if (newVolume == 7L)
				SetDialogNumToStr(dial, kVolNumberItem, 11L);
			else
				SetDialogNumToStr(dial, kVolNumberItem, (long)newVolume);

			UnivSetSoundVolume(newVolume, thisMac.hasSM3);

			HandleSoundMusicChange(newVolume, true);
			return(false);
			break;

			case kCapDKeyASCII:
			case kDKeyASCII:
			*item = kSoundDefault;
			FlashDialogButton(dial, kSoundDefault);
			return(true);
			break;

			case kCapGKeyASCII:
			case kGKeyASCII:
			*item = kPlayMusicItem;
			return(true);
			break;

			case kCapIKeyASCII:
			case kIKeyASCII:
			*item = kIdleMusicItem;
			return(true);
			break;

			default:
			return(false);
		}
		break;

		case mouseDown:
		return(false);
		break;

		case updateEvt:
		SetPort((GrafPtr)dial);
		BeginUpdate(GetDialogWindow(dial));
		UpdateSettingsSound(dial);
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

//--------------------------------------------------------------  DoSettingsMain

void DoSoundPrefs (HWND ownerWindow)
{
	MessageBox(ownerWindow, L"DoSoundPrefs", NULL, MB_ICONHAND);
	return;
#if 0
	Rect			tempRect;
	DialogPtr		prefDlg;
	short			wasLoudness, tempVolume;
	OSErr			theErr;
	short			itemHit;
	Boolean			leaving;
	ModalFilterUPP	soundFilterUPP;

	soundFilterUPP = NewModalFilterUPP(SoundFilter);

	BringUpDialog(&prefDlg, kSoundPrefsDialID);

	UnivGetSoundVolume(&wasLoudness, thisMac.hasSM3);

	wasIdle = isPlayMusicIdle;
	wasPlay = isPlayMusicGame;
	SetDialogItemValue(prefDlg, kIdleMusicItem, (short)wasIdle);
	SetDialogItemValue(prefDlg, kPlayMusicItem, (short)wasPlay);
	leaving = false;

	while (!leaving)
	{
		ModalDialog(soundFilterUPP, &itemHit);
		switch (itemHit)
		{
			case kOkayButton:
			isPlayMusicIdle = wasIdle;
			isPlayMusicGame = wasPlay;
			leaving = true;
			UnivGetSoundVolume(&tempVolume, thisMac.hasSM3);
			isSoundOn = (tempVolume != 0);
			break;

			case kCancelButton:
			UnivSetSoundVolume(wasLoudness, thisMac.hasSM3);
			HandleSoundMusicChange(wasLoudness, false);
			if (isPlayMusicIdle != wasIdle)
			{
				if (isPlayMusicIdle)
				{
					if (wasLoudness != 0)
					{
						theErr = StartMusic();
						if (theErr != noErr)
						{
							YellowAlert(kYellowNoMusic, theErr);
							failedMusic = true;
						}
					}
				}
				else
					StopTheMusic();
			}
			leaving = true;
			break;

			case kSofterItem:
			UnivGetSoundVolume(&tempVolume, thisMac.hasSM3);
			if (tempVolume > 0)
			{
				GetDialogItemRect(prefDlg, kSofterItem, &tempRect);
				DrawCIcon(1034, tempRect.left, tempRect.top);
				tempVolume--;
				SetDialogNumToStr(prefDlg, kVolNumberItem, (long)tempVolume);
				UnivSetSoundVolume(tempVolume, thisMac.hasSM3);
				HandleSoundMusicChange(tempVolume, true);
				InvalWindowRect(GetDialogWindow(prefDlg), &tempRect);
				DelayTicks(8);
			}
			break;

			case kLouderItem:
			UnivGetSoundVolume(&tempVolume, thisMac.hasSM3);
			if (tempVolume < 7)
			{
				GetDialogItemRect(prefDlg, kLouderItem, &tempRect);
				DrawCIcon(1033, tempRect.left, tempRect.top);
				tempVolume++;
				if (tempVolume == 7)
					SetDialogNumToStr(prefDlg, kVolNumberItem, 11L);
				else
					SetDialogNumToStr(prefDlg, kVolNumberItem, tempVolume);
				UnivSetSoundVolume(tempVolume, thisMac.hasSM3);
				HandleSoundMusicChange(tempVolume, true);
				InvalWindowRect(GetDialogWindow(prefDlg), &tempRect);
				DelayTicks(8);
			}
			break;

			case kIdleMusicItem:
			wasIdle = !wasIdle;
			SetDialogItemValue(prefDlg, kIdleMusicItem, (short)wasIdle);
			if (wasIdle)
			{
				UnivGetSoundVolume(&tempVolume, thisMac.hasSM3);
				if (tempVolume != 0)
				{
					theErr = StartMusic();
					if (theErr != noErr)
					{
						YellowAlert(kYellowNoMusic, theErr);
						failedMusic = true;
					}
				}
			}
			else
				StopTheMusic();
			break;

			case kPlayMusicItem:
			wasPlay = !wasPlay;
			SetDialogItemValue(prefDlg, kPlayMusicItem, (short)wasPlay);
			break;

			case kSoundDefault:
			SoundDefaults(prefDlg);
			break;
		}
	}

	DisposeDialog(prefDlg);
	DisposeModalFilterUPP(soundFilterUPP);
#endif
}

//--------------------------------------------------------------  DisplayDefaults

void DisplayDefaults (HWND prefsDialog)
{
	CheckRadioButton(prefsDialog, kDisplay1Item, kDisplay9Item, kDisplay9Item);
	CheckRadioButton(prefsDialog, kCurrentDepth, k16Depth, kCurrentDepth);
	CheckDlgButton(prefsDialog, kDoColorFadeItem, BST_CHECKED);
	CheckDlgButton(prefsDialog, kUseScreen2Item, BST_UNCHECKED);
}

//--------------------------------------------------------------  DisplayInit

static BOOL AreCommonControlsVersionSix(void)
{
	HANDLE comctl32;
	DLLGETVERSIONPROC comctl32_DllGetVersion;
	DLLVERSIONINFO versionInfo;
	HRESULT hr;

	// NOTE: This assumes that comctl32.dll has already been loaded by the
	// process (which is true for this program, because we call the function
	// InitCommonControlsEx).
	comctl32 = GetModuleHandle(L"comctl32");
	if (comctl32 == NULL)
	{
		return FALSE;
	}
	comctl32_DllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(comctl32, "DllGetVersion");
	if (comctl32_DllGetVersion == NULL)
	{
		return FALSE;
	}
	ZeroMemory(&versionInfo, sizeof(versionInfo));
	versionInfo.cbSize = sizeof(versionInfo);
	hr = comctl32_DllGetVersion(&versionInfo);
	if (FAILED(hr))
	{
		return FALSE;
	}
	return (versionInfo.dwMajorVersion >= 6);
}

void DisplayInit (HWND hDlg)
{
	HWND display1Control, display3Control, display9Control;
	UINT loadFlags, enabledLoadFlags, disabledLoadFlags;
	HICON display1Icon, display3Icon, display9Icon;

	display1Control = GetDlgItem(hDlg, kDisplay1Item);
	display3Control = GetDlgItem(hDlg, kDisplay3Item);
	display9Control = GetDlgItem(hDlg, kDisplay9Item);

	if (AreCommonControlsVersionSix())
	{
		// Version 6 of the common controls will render a disabled icon radio button
		// with a faded-out version, so enabled and disabled icons are loaded in the
		// same way.
		enabledLoadFlags = LR_DEFAULTSIZE | LR_DEFAULTCOLOR;
		disabledLoadFlags = LR_DEFAULTSIZE | LR_DEFAULTCOLOR;
	}
	else
	{
		// Previous versions of the common controls render a disabled icon radio button
		// by first converting the icon to monochrome and then tinting it. To allow this
		// to happen nicely, load our monochrome icons instead of letting our color icons
		// be converted to a completely black blob.
		enabledLoadFlags = LR_DEFAULTSIZE | LR_DEFAULTCOLOR;
		disabledLoadFlags = LR_DEFAULTSIZE | LR_MONOCHROME;
	}

	if (!thisMac.can8Bit)
	{
		EnableWindow(GetDlgItem(hDlg, kDoColorFadeItem), FALSE);
		EnableWindow(GetDlgItem(hDlg, k256Depth), FALSE);
	}
	if (!thisMac.can4Bit)
		EnableWindow(GetDlgItem(hDlg, k16Depth), FALSE);
	if (thisMac.numScreens < 2)
		EnableWindow(GetDlgItem(hDlg, kUseScreen2Item), FALSE);
	if (thisMac.screen.right <= 512)
	{
		EnableWindow(display3Control, FALSE);
		EnableWindow(display9Control, FALSE);
	}

	if (IsWindowEnabled(display1Control))
		loadFlags = enabledLoadFlags;
	else
		loadFlags = disabledLoadFlags;
	display1Icon = LoadImage(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(1020), IMAGE_ICON, 0, 0, loadFlags);
	SendMessage(display1Control, BM_SETIMAGE, IMAGE_ICON, (LPARAM)display1Icon);

	if (IsWindowEnabled(display3Control))
		loadFlags = enabledLoadFlags;
	else
		loadFlags = disabledLoadFlags;
	display3Icon = LoadImage(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(1021), IMAGE_ICON, 0, 0, loadFlags);
	SendMessage(display3Control, BM_SETIMAGE, IMAGE_ICON, (LPARAM)display3Icon);

	if (IsWindowEnabled(display9Control))
		loadFlags = enabledLoadFlags;
	else
		loadFlags = disabledLoadFlags;
	display9Icon = LoadImage(HINST_THISCOMPONENT,
		MAKEINTRESOURCE(1022), IMAGE_ICON, 0, 0, loadFlags);
	SendMessage(display9Control, BM_SETIMAGE, IMAGE_ICON, (LPARAM)display9Icon);

	if (numNeighbors == 1)
		CheckRadioButton(hDlg, kDisplay1Item, kDisplay9Item, kDisplay1Item);
	else if (numNeighbors == 3)
		CheckRadioButton(hDlg, kDisplay1Item, kDisplay9Item, kDisplay3Item);
	else if (numNeighbors == 9)
		CheckRadioButton(hDlg, kDisplay1Item, kDisplay9Item, kDisplay9Item);
	else
		CheckRadioButton(hDlg, kDisplay1Item, kDisplay9Item, kDisplay9Item);

	if (isDepthPref == kSwitchIfNeeded)
		CheckRadioButton(hDlg, kCurrentDepth, k16Depth, kCurrentDepth);
	else if (isDepthPref == kSwitchTo256Colors)
		CheckRadioButton(hDlg, kCurrentDepth, k16Depth, k256Depth);
	else if (isDepthPref == kSwitchTo16Grays)
		CheckRadioButton(hDlg, kCurrentDepth, k16Depth, k16Depth);
	else
		CheckRadioButton(hDlg, kCurrentDepth, k16Depth, kCurrentDepth);

	if (isDoColorFade)
		CheckDlgButton(hDlg, kDoColorFadeItem, BST_CHECKED);
	else
		CheckDlgButton(hDlg, kDoColorFadeItem, BST_UNCHECKED);

	CheckDlgButton(hDlg, kUseQDItem, BST_CHECKED);

	if (isUseSecondScreen)
		CheckDlgButton(hDlg, kUseScreen2Item, BST_CHECKED);
	else
		CheckDlgButton(hDlg, kUseScreen2Item, BST_UNCHECKED);
}

//--------------------------------------------------------------  DisplayApply

void DisplayApply (HWND hDlg)
{
	Boolean wasScreen2;

	if (IsDlgButtonChecked(hDlg, kDisplay1Item))
		numNeighbors = 1;
	else if (IsDlgButtonChecked(hDlg, kDisplay3Item))
		numNeighbors = 3;
	else if (IsDlgButtonChecked(hDlg, kDisplay9Item))
		numNeighbors = 9;
	else
		numNeighbors = 9;

	if (IsDlgButtonChecked(hDlg, kCurrentDepth))
		isDepthPref = kSwitchIfNeeded;
	else if (IsDlgButtonChecked(hDlg, k256Depth))
		isDepthPref = kSwitchTo256Colors;
	else if (IsDlgButtonChecked(hDlg, k16Depth))
		isDepthPref = kSwitchTo16Grays;
	else
		isDepthPref = kSwitchIfNeeded;

	if (IsDlgButtonChecked(hDlg, kDoColorFadeItem))
		isDoColorFade = true;
	else
		isDoColorFade = false;

	wasScreen2 = (isUseSecondScreen != 0);
	if (IsDlgButtonChecked(hDlg, kUseScreen2Item))
		isUseSecondScreen = true;
	else
		isUseSecondScreen = false;
	if (wasScreen2 != isUseSecondScreen)
		nextRestartChange = true;
}

//--------------------------------------------------------------  DisplayFilter

INT_PTR CALLBACK DisplayFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		CenterOverOwner(hDlg);
		DisplayInit(hDlg);
		return TRUE;

	case WM_DESTROY:
	{
		HICON hIcon;
		hIcon = (HICON)SendDlgItemMessage(hDlg, kDisplay1Item,
				BM_SETIMAGE, IMAGE_ICON, (LPARAM)NULL);
		if (hIcon != NULL)
			DestroyIcon(hIcon);
		hIcon = (HICON)SendDlgItemMessage(hDlg, kDisplay3Item,
				BM_SETIMAGE, IMAGE_ICON, (LPARAM)NULL);
		if (hIcon != NULL)
			DestroyIcon(hIcon);
		hIcon = (HICON)SendDlgItemMessage(hDlg, kDisplay9Item,
				BM_SETIMAGE, IMAGE_ICON, (LPARAM)NULL);
		if (hIcon != NULL)
			DestroyIcon(hIcon);
		return FALSE;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			DisplayApply(hDlg);
			EndDialog(hDlg, IDOK);
			break;

		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;

		case kDispDefault:
			DisplayDefaults(hDlg);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  DoDisplayPrefs

void DoDisplayPrefs (HWND ownerWindow)
{
	DialogBox(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(kDisplayPrefsDialID),
			ownerWindow, DisplayFilter);
}

//--------------------------------------------------------------  SetAllDefaults

void SetAllDefaults (HWND ownerWindow)
{
	OSErr		theErr;
								// Default brain settings
	willMaxFiles = 48;
	doZooms = true;
	doAutoDemo = true;
	doBackground = false;
	isHouseChecks = true;
	doPrettyMap = true;
	doBitchDialogs = true;
								// Default control settings
	PasStringCopyC("lf arrow", leftName);
	PasStringCopyC("rt arrow", rightName);
	PasStringCopyC("dn arrow", batteryName);
	PasStringCopyC("up arrow", bandName);
	theGlider.leftKey = VK_LEFT;
	theGlider.rightKey = VK_RIGHT;
	theGlider.battKey = VK_DOWN;
	theGlider.bandKey = VK_UP;
	isEscPauseKey = false;
								// Default sound settings
	isPlayMusicIdle = true;
	isPlayMusicGame = true;
	UnivSetSoundVolume(3, thisMac.hasSM3);
	isSoundOn = true;
	if (!isMusicOn)
	{
		theErr = StartMusic();
		if (theErr != noErr)
		{
			YellowAlert(ownerWindow, kYellowNoMusic, theErr);
			failedMusic = true;
		}
	}
								// Default display settings
	numNeighbors = 9;
	quickerTransitions = false;
	isDepthPref = kSwitchIfNeeded;
	isDoColorFade = true;
}

//--------------------------------------------------------------  FlashSettingsButton
/*
void FlashSettingsButton (SInt16 who)
{
	#define		kNormalSettingsIcon		1010
	#define		kInvertedSettingsIcon	1014
	short		theID;

	theID = kInvertedSettingsIcon + who;
	DrawCIcon (theID, prefButton[who].left + 4, prefButton[who].top + 4);
	DelayTicks(8);
	theID = kNormalSettingsIcon + who;
	DrawCIcon (theID, prefButton[who].left + 4, prefButton[who].top + 4);
}
*/
//--------------------------------------------------------------  UpdateSettingsMain
/*
void UpdateSettingsMain (DialogPtr theDialog)
{
	Str255		theStr;

	DrawDialog(theDialog);

	DrawDefaultButton(theDialog);

	// NOTE: the string table "129" uses string table base index "kPrefMainStringBase"
	GetIndString(theStr, 129, 1);
	DrawDialogUserText(theDialog, 7, theStr, false);
	GetIndString(theStr, 129, 2);
	DrawDialogUserText(theDialog, 8, theStr, false);
	GetIndString(theStr, 129, 3);
	DrawDialogUserText(theDialog, 9, theStr, false);
	GetIndString(theStr, 129, 4);
	DrawDialogUserText(theDialog, 10, theStr, false);

	ColorFrameRect(&prefButton[0], kRedOrangeColor8);
	ColorFrameRect(&prefButton[1], kRedOrangeColor8);
	ColorFrameRect(&prefButton[2], kRedOrangeColor8);
	ColorFrameRect(&prefButton[3], kRedOrangeColor8);
#endif
}
*/
//--------------------------------------------------------------  PrefsFilter

INT_PTR CALLBACK PrefsFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		CenterOverOwner(hDlg);
		SendDlgItemMessage(hDlg, kDisplayButton, BM_SETIMAGE, IMAGE_ICON,
				(LPARAM)LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(1010)));
		SendDlgItemMessage(hDlg, kSoundButton, BM_SETIMAGE, IMAGE_ICON,
				(LPARAM)LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(1011)));
		SendDlgItemMessage(hDlg, kControlsButton, BM_SETIMAGE, IMAGE_ICON,
				(LPARAM)LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(1012)));
		SendDlgItemMessage(hDlg, kBrainsButton, BM_SETIMAGE, IMAGE_ICON,
				(LPARAM)LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(1013)));
		FocusDefaultButton(hDlg);
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			break;

		case kDisplayButton:
			DoDisplayPrefs(hDlg);
			break;

		case kSoundButton:
			DoSoundPrefs(hDlg);
			break;

		case kControlsButton:
			DoControlPrefs(hDlg);
			break;

		case kBrainsButton:
			// TODO: reconsider using the Option key (Alt in Windows) as a
			// modifier key, because it already has special meaning in Windows.
			if ((OptionKeyDown()) && (!houseUnlocked))
			{
				houseUnlocked = true;
				changeLockStateOfHouse = true;
				saveHouseLocked = false;
			}
			DoBrainsPrefs(hDlg);
			break;

		case kAllDefaultsButton:
			SetAllDefaults(hDlg);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  DoSettingsMain

void DoSettingsMain (HWND ownerWindow)
{
	nextRestartChange = false;

	DialogBox(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(kMainPrefsDialID),
			ownerWindow, PrefsFilter);

	if (nextRestartChange)
		BitchAboutChanges(ownerWindow);
}

//--------------------------------------------------------------  BitchAboutChanges

void BitchAboutChanges (HWND ownerWindow)
{
	Alert(kChangesEffectAlert, ownerWindow, NULL);
}

