#define GP_USE_WINAPI_H

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
#include "Player.h"
#include "ResourceIDs.h"
#include "Room.h"
#include "RoomGraphics.h"
#include "SelectHouse.h"
#include "Sound.h"
#include "StringUtils.h"
#include "Utilities.h"

#include <commctrl.h>
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

#define kLeftControlOne         1001
#define kRightControlOne        1002
#define kBattControlOne         1003
#define kBandControlOne         1004
#define kLeftControlTwo         1005
#define kRightControlTwo        1006
#define kBattControlTwo         1007
#define kBandControlTwo         1008
#define kControlDefaults        1009
#define kESCPausesRadio         1010
#define kTABPausesRadio         1011

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

BYTE GetLastHotKeyValue (HWND prefDlg, int itemID);
void SetLastHotKeyValue (HWND prefDlg, int itemID, BYTE virtualKey);
BYTE GetHotKeyValue (HWND prefDlg, int itemID);
void SendSetHotKeyMessage (HWND prefDlg, int itemID, BYTE virtualKey);
void SetHotKeyValue (HWND prefDlg, int itemID, BYTE virtualKey);
void HandleHotKeyUserChange (HWND prefDlg, int itemID);
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


static Boolean nextRestartChange;
static BYTE tempLeftKeyOne;
static BYTE tempRightKeyOne;
static BYTE tempBattKeyOne;
static BYTE tempBandKeyOne;
static BYTE tempLeftKeyTwo;
static BYTE tempRightKeyTwo;
static BYTE tempBattKeyTwo;
static BYTE tempBandKeyTwo;


//==============================================================  Functions
//--------------------------------------------------------------  SetBrainsToDefaults

void SetBrainsToDefaults (HWND prefDlg)
{
	SetDlgItemInt(prefDlg, kMaxFilesItem, 24, FALSE);
	CheckDlgButton(prefDlg, kQuickTransitCheck, BST_CHECKED);
	CheckDlgButton(prefDlg, kDoZoomsCheck, BST_CHECKED);
	CheckDlgButton(prefDlg, kDoDemoCheck, BST_CHECKED);
	CheckDlgButton(prefDlg, kDoBackgroundCheck, BST_CHECKED);
	CheckDlgButton(prefDlg, kDoErrorCheck, BST_CHECKED);
	CheckDlgButton(prefDlg, kDoPrettyMapCheck, BST_CHECKED);
	CheckDlgButton(prefDlg, kDoBitchDlgsCheck, BST_CHECKED);
}

//--------------------------------------------------------------  BrainsInit

void BrainsInit (HWND prefDlg)
{
	SetDlgItemInt(prefDlg, kMaxFilesItem, willMaxFiles, FALSE);
	CheckDlgButton(prefDlg, kQuickTransitCheck, (quickerTransitions != 0));
	CheckDlgButton(prefDlg, kDoZoomsCheck, BST_CHECKED);
	CheckDlgButton(prefDlg, kDoDemoCheck, (doAutoDemo != 0));
	CheckDlgButton(prefDlg, kDoBackgroundCheck, BST_CHECKED);
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
	doAutoDemo = (IsDlgButtonChecked(prefDlg, kDoDemoCheck) != 0);
	isHouseChecks = (IsDlgButtonChecked(prefDlg, kDoErrorCheck) != 0);
	doPrettyMap = (IsDlgButtonChecked(prefDlg, kDoPrettyMapCheck) != 0);
	doBitchDialogs = (IsDlgButtonChecked(prefDlg, kDoBitchDlgsCheck) != 0);
}

//--------------------------------------------------------------  BrainsFilter

INT_PTR CALLBACK BrainsFilter (HWND prefDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	(void)lParam;

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

//--------------------------------------------------------------  GetLastHotKeyValue

BYTE GetLastHotKeyValue (HWND prefDlg, int itemID)
{
	BYTE virtualKey;

	(void)prefDlg;

	virtualKey = 0x00;
	switch (itemID)
	{
	case kLeftControlOne:
		virtualKey = tempLeftKeyOne;
		break;
	case kRightControlOne:
		virtualKey = tempRightKeyOne;
		break;
	case kBattControlOne:
		virtualKey = tempBattKeyOne;
		break;
	case kBandControlOne:
		virtualKey = tempBandKeyOne;
		break;
	case kLeftControlTwo:
		virtualKey = tempLeftKeyTwo;
		break;
	case kRightControlTwo:
		virtualKey = tempRightKeyTwo;
		break;
	case kBattControlTwo:
		virtualKey = tempBattKeyTwo;
		break;
	case kBandControlTwo:
		virtualKey = tempBandKeyTwo;
		break;
	}
	return virtualKey;
}

//--------------------------------------------------------------  SetLastHotKeyValue

void SetLastHotKeyValue (HWND prefDlg, int itemID, BYTE virtualKey)
{
	(void)prefDlg;

	switch (itemID)
	{
	case kLeftControlOne:
		tempLeftKeyOne = virtualKey;
		break;
	case kRightControlOne:
		tempRightKeyOne = virtualKey;
		break;
	case kBattControlOne:
		tempBattKeyOne = virtualKey;
		break;
	case kBandControlOne:
		tempBandKeyOne = virtualKey;
		break;
	case kLeftControlTwo:
		tempLeftKeyTwo = virtualKey;
		break;
	case kRightControlTwo:
		tempRightKeyTwo = virtualKey;
		break;
	case kBattControlTwo:
		tempBattKeyTwo = virtualKey;
		break;
	case kBandControlTwo:
		tempBandKeyTwo = virtualKey;
		break;
	}
}

//--------------------------------------------------------------  GetHotKeyValue

BYTE GetHotKeyValue (HWND prefDlg, int itemID)
{
	LRESULT result;
	WORD hotKeyCode;
	BYTE virtualKey;

	result = SendDlgItemMessage(prefDlg, itemID, HKM_GETHOTKEY, 0, 0);
	hotKeyCode = LOWORD(result);
	virtualKey = LOBYTE(hotKeyCode);
	return virtualKey;
}

//--------------------------------------------------------------  SendSetHotKeyMessage

void SendSetHotKeyMessage (HWND prefDlg, int itemID, BYTE virtualKey)
{
	WORD hotKeyCode;

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
		// extended keys need the HOTKEYF_EXT flag set
		hotKeyCode = MAKEWORD(virtualKey, HOTKEYF_EXT);
		break;
	default:
		hotKeyCode = MAKEWORD(virtualKey, 0);
		break;
	}

	SendDlgItemMessage(prefDlg, itemID, HKM_SETHOTKEY, hotKeyCode, 0);
}

//--------------------------------------------------------------  SetHotKeyValue

void SetHotKeyValue (HWND prefDlg, int itemID, BYTE virtualKey)
{
	SetLastHotKeyValue(prefDlg, itemID, virtualKey);
	SendSetHotKeyMessage(prefDlg, itemID, virtualKey);
}

//--------------------------------------------------------------  HandleHotKeyUserChange

void HandleHotKeyUserChange (HWND prefDlg, int itemID)
{
	const int items[] = {
		kLeftControlOne, kRightControlOne, kBattControlOne, kBandControlOne,
		kLeftControlTwo, kRightControlTwo, kBattControlTwo, kBandControlTwo,
	};

	BYTE oldVirtualKey;
	BYTE newVirtualKey;
	BYTE otherVirtualKey;
	size_t i;

	oldVirtualKey = GetLastHotKeyValue(prefDlg, itemID);
	newVirtualKey = GetHotKeyValue(prefDlg, itemID);

	// Don't let the hot key control be cleared.
	if (newVirtualKey == 0x00)
	{
		newVirtualKey = oldVirtualKey;
	}

	// If the new hot key matches another control, set the other
	// control to the old value of this hot key.
	for (i = 0; i < ARRAYSIZE(items); i++)
	{
		if (items[i] == itemID)
		{
			continue;
		}
		otherVirtualKey = GetHotKeyValue(prefDlg, items[i]);
		if (newVirtualKey == otherVirtualKey)
		{
			SetHotKeyValue(prefDlg, items[i], oldVirtualKey);
			break;
		}
	}

	// Set the new hot key's value
	SetHotKeyValue(prefDlg, itemID, newVirtualKey);
}

//--------------------------------------------------------------  SetControlsToDefaults

void SetControlsToDefaults (HWND prefDlg)
{
	SetHotKeyValue(prefDlg, kLeftControlOne, VK_LEFT);
	SetHotKeyValue(prefDlg, kRightControlOne, VK_RIGHT);
	SetHotKeyValue(prefDlg, kBattControlOne, VK_DOWN);
	SetHotKeyValue(prefDlg, kBandControlOne, VK_UP);
	SetHotKeyValue(prefDlg, kLeftControlTwo, 'A');
	SetHotKeyValue(prefDlg, kRightControlTwo, 'D');
	SetHotKeyValue(prefDlg, kBattControlTwo, 'S');
	SetHotKeyValue(prefDlg, kBandControlTwo, 'W');
	CheckRadioButton(prefDlg, kESCPausesRadio, kTABPausesRadio, kTABPausesRadio);
}

//--------------------------------------------------------------  ControlInit

void ControlInit (HWND prefDlg)
{
	SetHotKeyValue(prefDlg, kLeftControlOne, (BYTE)theGlider.leftKey);
	SetHotKeyValue(prefDlg, kRightControlOne, (BYTE)theGlider.rightKey);
	SetHotKeyValue(prefDlg, kBattControlOne, (BYTE)theGlider.battKey);
	SetHotKeyValue(prefDlg, kBandControlOne, (BYTE)theGlider.bandKey);
	SetHotKeyValue(prefDlg, kLeftControlTwo, (BYTE)theGlider2.leftKey);
	SetHotKeyValue(prefDlg, kRightControlTwo, (BYTE)theGlider2.rightKey);
	SetHotKeyValue(prefDlg, kBattControlTwo, (BYTE)theGlider2.battKey);
	SetHotKeyValue(prefDlg, kBandControlTwo, (BYTE)theGlider2.bandKey);
	if (isEscPauseKey)
	{
		CheckRadioButton(prefDlg, kESCPausesRadio, kTABPausesRadio, kESCPausesRadio);
	}
	else
	{
		CheckRadioButton(prefDlg, kESCPausesRadio, kTABPausesRadio, kTABPausesRadio);
	}
}

//--------------------------------------------------------------  ControlApply

void ControlApply (HWND prefDlg)
{
	theGlider.leftKey = tempLeftKeyOne;
	theGlider.rightKey = tempRightKeyOne;
	theGlider.battKey = tempBattKeyOne;
	theGlider.bandKey = tempBandKeyOne;
	theGlider2.leftKey = tempLeftKeyTwo;
	theGlider2.rightKey = tempRightKeyTwo;
	theGlider2.battKey = tempBattKeyTwo;
	theGlider2.bandKey = tempBandKeyTwo;
	if (IsDlgButtonChecked(prefDlg, kESCPausesRadio))
	{
		isEscPauseKey = true;
	}
	else if (IsDlgButtonChecked(prefDlg, kTABPausesRadio))
	{
		isEscPauseKey = false;
	}
	else
	{
		isEscPauseKey = false;
	}
}

//--------------------------------------------------------------  ControlFilter

INT_PTR CALLBACK ControlFilter (HWND prefDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	(void)lParam;

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

		case kLeftControlOne:
		case kRightControlOne:
		case kBattControlOne:
		case kBandControlOne:
		case kLeftControlTwo:
		case kRightControlTwo:
		case kBattControlTwo:
		case kBandControlTwo:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				HandleHotKeyUserChange(prefDlg, LOWORD(wParam));
			}
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

//--------------------------------------------------------------  DoSoundPrefs

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
	HMODULE comctl32;
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
	display1Icon = (HICON)LoadImage(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(kDisplay1Icon),
			IMAGE_ICON, 0, 0, loadFlags);
	SendMessage(display1Control, BM_SETIMAGE, IMAGE_ICON, (LPARAM)display1Icon);

	if (IsWindowEnabled(display3Control))
		loadFlags = enabledLoadFlags;
	else
		loadFlags = disabledLoadFlags;
	display3Icon = (HICON)LoadImage(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(kDisplay3Icon),
			IMAGE_ICON, 0, 0, loadFlags);
	SendMessage(display3Control, BM_SETIMAGE, IMAGE_ICON, (LPARAM)display3Icon);

	if (IsWindowEnabled(display9Control))
		loadFlags = enabledLoadFlags;
	else
		loadFlags = disabledLoadFlags;
	display9Icon = (HICON)LoadImage(HINST_THISCOMPONENT,
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

	isDoColorFade = (IsDlgButtonChecked(hDlg, kDoColorFadeItem) != 0);

	wasScreen2 = (isUseSecondScreen != 0);
	isUseSecondScreen = (IsDlgButtonChecked(hDlg, kUseScreen2Item) != 0);
	if (wasScreen2 != isUseSecondScreen)
		nextRestartChange = true;
}

//--------------------------------------------------------------  DisplayFilter

INT_PTR CALLBACK DisplayFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	(void)lParam;

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
	doAutoDemo = true;
	isHouseChecks = true;
	doPrettyMap = true;
	doBitchDialogs = true;
								// Default control settings
	theGlider.leftKey = VK_LEFT;
	theGlider.rightKey = VK_RIGHT;
	theGlider.battKey = VK_DOWN;
	theGlider.bandKey = VK_UP;
	theGlider2.leftKey = 'A';
	theGlider2.rightKey = 'D';
	theGlider2.battKey = 'S';
	theGlider2.bandKey = 'W';
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
	isDoColorFade = true;
}

//--------------------------------------------------------------  PrefsFilter

INT_PTR CALLBACK PrefsFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HICON hIcon;

	(void)lParam;

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

