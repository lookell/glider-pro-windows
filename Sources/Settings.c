#include "Settings.h"

//============================================================================
//----------------------------------------------------------------------------
//                                 Settings.c
//----------------------------------------------------------------------------
//============================================================================


#include "DialogUtils.h"
#include "Environ.h"
#include "Events.h"
#include "House.h"
#include "HouseIO.h"
#include "HouseLegal.h"
#include "Input.h"
#include "MacTypes.h"
#include "Main.h"
#include "MainWindow.h"
#include "Map.h"
#include "Music.h"
#include "Play.h"
#include "Player.h"
#include "ResourceIDs.h"
#include "Room.h"
#include "RoomGraphics.h"
#include "SelectHouse.h"
#include "Sound.h"
#include "StringUtils.h"
#include "Utilities.h"

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

#define kVolumeSliderItem		1003
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


void SetBrainsToDefaults (HWND prefDlg);
void BrainsInit (HWND prefDlg);
void BrainsApply (HWND prefDlg);
INT_PTR CALLBACK BrainsFilter (HWND prefDlg, UINT message, WPARAM wParam, LPARAM lParam);
void DoBrainsPrefs (HWND ownerWindow);

BYTE GetHotKeyValue (HWND prefDlg, int itemID);
void SetHotKeyValue (HWND prefDlg, int itemID, BYTE virtualKey);
void HandleHotKeyChange (HWND prefDlg, int itemID);
void SetControlsToDefaults (HWND prefDlg);
void ControlInit (HWND prefDlg);
void ControlApply (HWND prefDlg);
INT_PTR CALLBACK ControlFilter (HWND prefDlg, UINT message, WPARAM wParam, LPARAM lParam);
void DoControlPrefs (HWND ownerWindow);

void SoundDefaults (HWND prefDlg);
void SoundPrefsInit (HWND prefDlg);
void SoundPrefsApply (HWND prefDlg);
void HandleSoundMusicChange (HWND prefDlg, SInt16 newVolume, Boolean sayIt);
INT_PTR CALLBACK SoundFilter (HWND prefDlg, UINT message, WPARAM wParam, LPARAM lParam);
void DoSoundPrefs (HWND ownerWindow);

void DisplayDefaults (HWND hDlg);
void DisplayInit (HWND hDlg);
void DisplayApply (HWND hDlg);
INT_PTR CALLBACK DisplayFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void DoDisplayPrefs (HWND ownerWindow);

void SetAllDefaults (HWND ownerWindow);
INT_PTR CALLBACK PrefsFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void BitchAboutChanges (HWND ownerWindow);


Str15		leftName, rightName, batteryName, bandName;
Boolean		nextRestartChange;

static	BYTE		tempLeftKey, tempRightKey, tempBattKey, tempBandKey;


//==============================================================  Functions
//--------------------------------------------------------------  SetBrainsToDefaults

void SetBrainsToDefaults (HWND prefDlg)
{
	SetDlgItemInt(prefDlg, kMaxFilesItem, 24, FALSE);
	CheckDlgButton(prefDlg, kQuickTransitCheck, BST_CHECKED);
	CheckDlgButton(prefDlg, kDoZoomsCheck, BST_CHECKED);
	CheckDlgButton(prefDlg, kDoDemoCheck, BST_CHECKED);
	CheckDlgButton(prefDlg, kDoBackgroundCheck, BST_UNCHECKED);
	CheckDlgButton(prefDlg, kDoErrorCheck, BST_CHECKED);
	CheckDlgButton(prefDlg, kDoPrettyMapCheck, BST_CHECKED);
	CheckDlgButton(prefDlg, kDoBitchDlgsCheck, BST_CHECKED);
}

//--------------------------------------------------------------  BrainsInit

void BrainsInit (HWND prefDlg)
{
	SetDlgItemInt(prefDlg, kMaxFilesItem, willMaxFiles, FALSE);
	CheckDlgButton(prefDlg, kQuickTransitCheck, (quickerTransitions != 0));
	CheckDlgButton(prefDlg, kDoZoomsCheck, (doZooms != 0));
	CheckDlgButton(prefDlg, kDoDemoCheck, (doAutoDemo != 0));
	CheckDlgButton(prefDlg, kDoBackgroundCheck, (doBackground != 0));
	CheckDlgButton(prefDlg, kDoErrorCheck, (isHouseChecks != 0));
	CheckDlgButton(prefDlg, kDoPrettyMapCheck, (doPrettyMap != 0));
	CheckDlgButton(prefDlg, kDoBitchDlgsCheck, (doBitchDialogs != 0));
}

//--------------------------------------------------------------  BrainsApply

void BrainsApply (HWND prefDlg)
{
	SInt16 wasMaxFiles;
	UINT tmp;

	wasMaxFiles = willMaxFiles;
	tmp = GetDlgItemInt(prefDlg, kMaxFilesItem, NULL, FALSE);
	if (tmp > 500)
		tmp = 500;
	else if (tmp < 12)
		tmp = 12;
	willMaxFiles = (SInt16)tmp;
	if (willMaxFiles != wasMaxFiles)
		nextRestartChange = true;
	quickerTransitions = (IsDlgButtonChecked(prefDlg, kQuickTransitCheck) != 0);
	doZooms = (IsDlgButtonChecked(prefDlg, kDoZoomsCheck) != 0);
	doAutoDemo = (IsDlgButtonChecked(prefDlg, kDoDemoCheck) != 0);
	doBackground = (IsDlgButtonChecked(prefDlg, kDoBackgroundCheck) != 0);
	isHouseChecks = (IsDlgButtonChecked(prefDlg, kDoErrorCheck) != 0);
	doPrettyMap = (IsDlgButtonChecked(prefDlg, kDoPrettyMapCheck) != 0);
	doBitchDialogs = (IsDlgButtonChecked(prefDlg, kDoBitchDlgsCheck) != 0);
}

//--------------------------------------------------------------  BrainsFilter

INT_PTR CALLBACK BrainsFilter (HWND prefDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		CenterOverOwner(prefDlg);
		BrainsInit(prefDlg);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			BrainsApply(prefDlg);
			EndDialog(prefDlg, IDOK);
			break;

		case IDCANCEL:
			EndDialog(prefDlg, IDCANCEL);
			break;

		case kBrainsDefault:
			SetBrainsToDefaults(prefDlg);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  DoBrainsPrefs

void DoBrainsPrefs (HWND ownerWindow)
{
	DialogBox(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(kBrainsPrefsDialID),
			ownerWindow, BrainsFilter);
}

//--------------------------------------------------------------  GetHotKeyValue

BYTE GetHotKeyValue (HWND prefDlg, int itemID)
{
	return LOBYTE(SendDlgItemMessage(prefDlg, itemID, HKM_GETHOTKEY, 0, 0));
}

//--------------------------------------------------------------  SetHotKeyValue

void SetHotKeyValue (HWND prefDlg, int itemID, BYTE virtualKey)
{
	SendDlgItemMessage(prefDlg, itemID, HKM_SETHOTKEY, MAKEWORD(virtualKey, 0), 0);
	HandleHotKeyChange(prefDlg, itemID);
}

//--------------------------------------------------------------  HandleHotKeyChange

void HandleHotKeyChange (HWND prefDlg, int itemID)
{
	WORD oldHotKey, newHotKey;
	BYTE virtualKey;

	oldHotKey = LOWORD(SendDlgItemMessage(prefDlg, itemID, HKM_GETHOTKEY, 0, 0));
	virtualKey = LOBYTE(oldHotKey);

	if (virtualKey == 0x00)
	{
		// Don't let the hotkey control be cleared
		switch (itemID)
		{
		case kLeftControl:
			virtualKey = tempLeftKey;
			break;
		case kRightControl:
			virtualKey = tempRightKey;
			break;
		case kBattControl:
			virtualKey = tempBattKey;
			break;
		case kBandControl:
			virtualKey = tempBandKey;
			break;
		}
	}

	// Block attempts to have one key bound to more than one action
	switch (itemID)
	{
	case kLeftControl:
		if (virtualKey == tempRightKey ||
			virtualKey == tempBattKey ||
			virtualKey == tempBandKey)
		{
			virtualKey = tempLeftKey;
			MessageBeep(MB_ICONWARNING);
		}
		tempLeftKey = virtualKey;
		break;

	case kRightControl:
		if (virtualKey == tempLeftKey ||
			virtualKey == tempBattKey ||
			virtualKey == tempBandKey)
		{
			virtualKey = tempRightKey;
			MessageBeep(MB_ICONWARNING);
		}
		tempRightKey = virtualKey;
		break;

	case kBattControl:
		if (virtualKey == tempLeftKey ||
			virtualKey == tempRightKey ||
			virtualKey == tempBandKey)
		{
			virtualKey = tempBattKey;
			MessageBeep(MB_ICONWARNING);
		}
		tempBattKey = virtualKey;
		break;

	case kBandControl:
		if (virtualKey == tempLeftKey ||
			virtualKey == tempRightKey ||
			virtualKey == tempBattKey)
		{
			virtualKey = tempBandKey;
			MessageBeep(MB_ICONWARNING);
		}
		tempBandKey = virtualKey;
		break;
	}

	switch (virtualKey)
	{
	case VK_RMENU:
	case VK_RCONTROL:
	case VK_INSERT:
	case VK_DELETE:
	case VK_HOME:
	case VK_END:
	case VK_PRIOR:
	case VK_NEXT:
	case VK_LEFT:
	case VK_UP:
	case VK_RIGHT:
	case VK_DOWN:
	case VK_NUMLOCK:
	case VK_CANCEL:
	case VK_SNAPSHOT:
	case VK_DIVIDE:
		// extended keys need their flag re-set
		newHotKey = MAKEWORD(virtualKey, HOTKEYF_EXT);
		break;

	default:
		newHotKey = MAKEWORD(virtualKey, 0);
		break;
	}

	if (newHotKey != oldHotKey)
		SendDlgItemMessage(prefDlg, itemID, HKM_SETHOTKEY, newHotKey, 0);
}

//--------------------------------------------------------------  SetControlsToDefaults

void SetControlsToDefaults (HWND prefDlg)
{
	SetHotKeyValue(prefDlg, kLeftControl, VK_LEFT);
	SetHotKeyValue(prefDlg, kRightControl, VK_RIGHT);
	SetHotKeyValue(prefDlg, kBattControl, VK_DOWN);
	SetHotKeyValue(prefDlg, kBandControl, VK_UP);
	CheckRadioButton(prefDlg, kESCPausesRadio, kTABPausesRadio, kTABPausesRadio);
}

//--------------------------------------------------------------  ControlInit

void ControlInit (HWND prefDlg)
{
	tempLeftKey = 0;
	tempRightKey = 0;
	tempBattKey = 0;
	tempBandKey = 0;
	SetHotKeyValue(prefDlg, kLeftControl, (BYTE)theGlider.leftKey);
	SetHotKeyValue(prefDlg, kRightControl, (BYTE)theGlider.rightKey);
	SetHotKeyValue(prefDlg, kBattControl, (BYTE)theGlider.battKey);
	SetHotKeyValue(prefDlg, kBandControl, (BYTE)theGlider.bandKey);
	if (isEscPauseKey)
		CheckRadioButton(prefDlg, kESCPausesRadio, kTABPausesRadio, kESCPausesRadio);
	else
		CheckRadioButton(prefDlg, kESCPausesRadio, kTABPausesRadio, kTABPausesRadio);
}

//--------------------------------------------------------------  ControlApply

void ControlApply (HWND prefDlg)
{
	theGlider.leftKey = tempLeftKey;
	theGlider.rightKey = tempRightKey;
	theGlider.battKey = tempBattKey;
	theGlider.bandKey = tempBandKey;
	if (IsDlgButtonChecked(prefDlg, kESCPausesRadio))
		isEscPauseKey = true;
	else if (IsDlgButtonChecked(prefDlg, kTABPausesRadio))
		isEscPauseKey = false;
	else
		isEscPauseKey = false;
}

//--------------------------------------------------------------  ControlFilter

INT_PTR CALLBACK ControlFilter (HWND prefDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		CenterOverOwner(prefDlg);
		ControlInit(prefDlg);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			ControlApply(prefDlg);
			EndDialog(prefDlg, IDOK);
			break;

		case IDCANCEL:
			EndDialog(prefDlg, IDCANCEL);
			break;

		case kLeftControl:
		case kRightControl:
		case kBattControl:
		case kBandControl:
			if (HIWORD(wParam) == EN_CHANGE)
				HandleHotKeyChange(prefDlg, LOWORD(wParam));
			break;

		case kControlDefaults:
			SetControlsToDefaults(prefDlg);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  DoControlPrefs

void DoControlPrefs (HWND ownerWindow)
{
	DialogBox(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(kControlPrefsDialID),
			ownerWindow, ControlFilter);
}

//--------------------------------------------------------------  SoundDefaults

void SoundDefaults (HWND prefDlg)
{
	CheckDlgButton(prefDlg, kIdleMusicItem, BST_CHECKED);
	CheckDlgButton(prefDlg, kPlayMusicItem, BST_CHECKED);
	SendDlgItemMessage(prefDlg, kVolumeSliderItem, TBM_SETPOS, TRUE, 3);
	UnivSetSoundVolume(3);
	SetDlgItemInt(prefDlg, kVolNumberItem, 3, FALSE);
	HandleSoundMusicChange(prefDlg, 3, true);
}

//--------------------------------------------------------------  SoundPrefsInit

void SoundPrefsInit (HWND prefDlg)
{
	HWND volumeSlider;
	SInt16 theVolume;

	UnivGetSoundVolume(&theVolume);

	volumeSlider = GetDlgItem(prefDlg, kVolumeSliderItem);
	SendMessage(volumeSlider, TBM_SETRANGE, FALSE, MAKELPARAM(0, 7));
	SendMessage(volumeSlider, TBM_SETPOS, TRUE, theVolume);
	if (theVolume >= 7)
		theVolume = 11;
	SetDlgItemInt(prefDlg, kVolNumberItem, theVolume, TRUE);

	CheckDlgButton(prefDlg, kIdleMusicItem, (isPlayMusicIdle != 0));
	CheckDlgButton(prefDlg, kPlayMusicItem, (isPlayMusicGame != 0));
}

//--------------------------------------------------------------  SoundPrefsApply

void SoundPrefsApply (HWND prefDlg)
{
	SInt16 tempVolume;

	isPlayMusicIdle = (IsDlgButtonChecked(prefDlg, kIdleMusicItem) != 0);
	isPlayMusicGame = (IsDlgButtonChecked(prefDlg, kPlayMusicItem) != 0);

	UnivGetSoundVolume(&tempVolume);
	isSoundOn = (tempVolume != 0);
}

//--------------------------------------------------------------  HandleSoundMusicChange

void HandleSoundMusicChange (HWND prefDlg, SInt16 newVolume, Boolean sayIt)
{
	OSErr		theErr;

	isSoundOn = (newVolume != 0);

	if (IsDlgButtonChecked(prefDlg, kIdleMusicItem))
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
					YellowAlert(prefDlg, kYellowNoMusic, theErr);
					failedMusic = true;
				}
			}
		}
	}

	if ((newVolume != 0) && (sayIt))
		PlayPrioritySound(kChord2Sound, kChord2Priority);
}

//--------------------------------------------------------------  SoundFilter

INT_PTR CALLBACK SoundFilter (HWND prefDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static SInt16 wasLoudness;
	SInt16 tempVolume;
	Boolean wasIdle;

	switch (message)
	{
	case WM_INITDIALOG:
		CenterOverOwner(prefDlg);
		UnivGetSoundVolume(&wasLoudness);
		SoundPrefsInit(prefDlg);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			SoundPrefsApply(prefDlg);
			EndDialog(prefDlg, IDOK);
			break;

		case IDCANCEL:
			UnivSetSoundVolume(wasLoudness);
			HandleSoundMusicChange(prefDlg, wasLoudness, false);
			isPlayMusicIdle = (isPlayMusicIdle != 0);
			wasIdle = (IsDlgButtonChecked(prefDlg, kIdleMusicItem) != 0);
			if (isPlayMusicIdle != wasIdle)
			{
				if (isPlayMusicIdle)
				{
					if (wasLoudness != 0)
					{
						OSErr theErr = StartMusic();
						if (theErr != noErr)
						{
							YellowAlert(prefDlg, kYellowNoMusic, theErr);
							failedMusic = true;
						}
					}
				}
				else
					StopTheMusic();
			}
			EndDialog(prefDlg, IDCANCEL);
			break;

		case kIdleMusicItem:
			if (IsDlgButtonChecked(prefDlg, kIdleMusicItem))
			{
				UnivGetSoundVolume(&tempVolume);
				if (tempVolume != 0)
				{
					OSErr theErr = StartMusic();
					if (theErr != noErr)
					{
						YellowAlert(prefDlg, kYellowNoMusic, theErr);
						failedMusic = true;
					}
				}
			}
			else
				StopTheMusic();
			break;

		case kSoundDefault:
			SoundDefaults(prefDlg);
			break;
		}
		return TRUE;

	case WM_HSCROLL:
		if (GetDlgItem(prefDlg, kVolumeSliderItem) != (HWND)lParam)
			return FALSE;
		switch (LOWORD(wParam))
		{
		case TB_TOP:
		case TB_BOTTOM:
		case TB_LINEUP:
		case TB_LINEDOWN:
		case TB_PAGEUP:
		case TB_PAGEDOWN:
		case TB_THUMBPOSITION:
			tempVolume = (SInt16)SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
			if (tempVolume >= 7)
				SetDlgItemInt(prefDlg, kVolNumberItem, 11, FALSE);
			else
				SetDlgItemInt(prefDlg, kVolNumberItem, tempVolume, FALSE);

			UnivSetSoundVolume(tempVolume);
			HandleSoundMusicChange(prefDlg, tempVolume, true);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------  DoSettingsMain

void DoSoundPrefs (HWND ownerWindow)
{
	DialogBox(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(kSoundPrefsDialID),
			ownerWindow, SoundFilter);
}

//--------------------------------------------------------------  DisplayDefaults

void DisplayDefaults (HWND hDlg)
{
	CheckRadioButton(hDlg, kDisplay1Item, kDisplay9Item, kDisplay9Item);
	CheckRadioButton(hDlg, kCurrentDepth, k16Depth, kCurrentDepth);
	CheckDlgButton(hDlg, kDoColorFadeItem, BST_CHECKED);
	CheckDlgButton(hDlg, kUseScreen2Item, BST_UNCHECKED);
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
			MAKEINTRESOURCE(kDisplay1Icon),
			IMAGE_ICON, 0, 0, loadFlags);
	SendMessage(display1Control, BM_SETIMAGE, IMAGE_ICON, (LPARAM)display1Icon);

	if (IsWindowEnabled(display3Control))
		loadFlags = enabledLoadFlags;
	else
		loadFlags = disabledLoadFlags;
	display3Icon = LoadImage(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(kDisplay3Icon),
			IMAGE_ICON, 0, 0, loadFlags);
	SendMessage(display3Control, BM_SETIMAGE, IMAGE_ICON, (LPARAM)display3Icon);

	if (IsWindowEnabled(display9Control))
		loadFlags = enabledLoadFlags;
	else
		loadFlags = disabledLoadFlags;
	display9Icon = LoadImage(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(kDisplay9Icon),
			IMAGE_ICON, 0, 0, loadFlags);
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

	isDoColorFade = (IsDlgButtonChecked(hDlg, kDoColorFadeItem) != 0);

	wasScreen2 = (isUseSecondScreen != 0);
	isUseSecondScreen = (IsDlgButtonChecked(hDlg, kUseScreen2Item) != 0);
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
	UnivSetSoundVolume(3);
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

//--------------------------------------------------------------  PrefsFilter

INT_PTR CALLBACK PrefsFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HICON hIcon;

	switch (message)
	{
	case WM_INITDIALOG:
		CenterOverOwner(hDlg);

		hIcon = LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(kNormalDisplayIcon));
		SendDlgItemMessage(hDlg, kDisplayButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);

		hIcon = LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(kNormalSoundsIcon));
		SendDlgItemMessage(hDlg, kSoundButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);

		hIcon = LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(kNormalControlsIcon));
		SendDlgItemMessage(hDlg, kControlsButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);

		hIcon = LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(kNormalBrainsIcon));
		SendDlgItemMessage(hDlg, kBrainsButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);

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

