//============================================================================
//----------------------------------------------------------------------------
//                                 Settings.c
//----------------------------------------------------------------------------
//============================================================================

#include "Settings.h"

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
#include "Utilities.h"

#include <commctrl.h>
#include <shlwapi.h> // for DLLGETVERSIONPROC and DLLGETVERSIONINFO

#define kDisplayButton          1003
#define kSoundButton            1004
#define kControlsButton         1005
#define kBrainsButton           1006
#define kAllDefaultsButton      1011

#define kDisplay1Item           1003
#define kDisplay3Item           1004
#define kDisplay9Item           1005
#define kDoColorFadeItem        1009
#define kCurrentDepth           1010
#define k256Depth               1011
#define k16Depth                1012
#define kDispDefault            1015
#define kUseQDItem              1016
#define kUseScreen2Item         1017

#define kVolumeSliderItem       1003
#define kVolNumberItem          1007
#define kIdleMusicItem          1008
#define kPlayMusicItem          1009
#define kSoundDefault           1013

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

#define kMaxFilesItem           1005
#define kQuickTransitCheck      1007
#define kDoZoomsCheck           1008
#define kBrainsDefault          1009
#define kDoDemoCheck            1010
#define kDoBackgroundCheck      1011
#define kDoErrorCheck           1012
#define kDoPrettyMapCheck       1013
#define kDoBitchDlgsCheck       1014

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

static Boolean g_nextRestartChange;
static BYTE g_tempLeftKeyOne;
static BYTE g_tempRightKeyOne;
static BYTE g_tempBattKeyOne;
static BYTE g_tempBandKeyOne;
static BYTE g_tempLeftKeyTwo;
static BYTE g_tempRightKeyTwo;
static BYTE g_tempBattKeyTwo;
static BYTE g_tempBandKeyTwo;
static SInt16 g_wasLoudness;

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
	SetDlgItemInt(prefDlg, kMaxFilesItem, g_willMaxFiles, FALSE);
	CheckDlgButton(prefDlg, kQuickTransitCheck, (g_quickerTransitions != 0));
	CheckDlgButton(prefDlg, kDoZoomsCheck, BST_CHECKED);
	CheckDlgButton(prefDlg, kDoDemoCheck, (g_doAutoDemo != 0));
	CheckDlgButton(prefDlg, kDoBackgroundCheck, BST_CHECKED);
	CheckDlgButton(prefDlg, kDoErrorCheck, (g_isHouseChecks != 0));
	CheckDlgButton(prefDlg, kDoPrettyMapCheck, (g_doPrettyMap != 0));
	CheckDlgButton(prefDlg, kDoBitchDlgsCheck, (g_doBitchDialogs != 0));
}

//--------------------------------------------------------------  BrainsApply

void BrainsApply (HWND prefDlg)
{
	SInt16 wasMaxFiles;
	Boolean wasDoPrettyMap;
	UINT tmp;

	wasMaxFiles = g_willMaxFiles;
	wasDoPrettyMap = g_doPrettyMap;
	tmp = GetDlgItemInt(prefDlg, kMaxFilesItem, NULL, FALSE);
	if (tmp > 500)
		tmp = 500;
	else if (tmp < 12)
		tmp = 12;
	g_willMaxFiles = (SInt16)tmp;
	if (g_willMaxFiles != wasMaxFiles)
		g_nextRestartChange = true;
	g_quickerTransitions = (IsDlgButtonChecked(prefDlg, kQuickTransitCheck) != 0);
	g_doAutoDemo = (IsDlgButtonChecked(prefDlg, kDoDemoCheck) != 0);
	g_isHouseChecks = (IsDlgButtonChecked(prefDlg, kDoErrorCheck) != 0);
	g_doPrettyMap = (IsDlgButtonChecked(prefDlg, kDoPrettyMapCheck) != 0);
	g_doBitchDialogs = (IsDlgButtonChecked(prefDlg, kDoBitchDlgsCheck) != 0);
	if ((wasDoPrettyMap != g_doPrettyMap) && (g_mapWindow != NULL))
	{
		InvalidateRect(g_mapWindow, NULL, FALSE);
	}
}

//--------------------------------------------------------------  BrainsFilter

INT_PTR CALLBACK BrainsFilter (HWND prefDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	(void)lParam;

	switch (message)
	{
	case WM_INITDIALOG:
		CenterDialogOverOwner(prefDlg);
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
		virtualKey = g_tempLeftKeyOne;
		break;
	case kRightControlOne:
		virtualKey = g_tempRightKeyOne;
		break;
	case kBattControlOne:
		virtualKey = g_tempBattKeyOne;
		break;
	case kBandControlOne:
		virtualKey = g_tempBandKeyOne;
		break;
	case kLeftControlTwo:
		virtualKey = g_tempLeftKeyTwo;
		break;
	case kRightControlTwo:
		virtualKey = g_tempRightKeyTwo;
		break;
	case kBattControlTwo:
		virtualKey = g_tempBattKeyTwo;
		break;
	case kBandControlTwo:
		virtualKey = g_tempBandKeyTwo;
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
		g_tempLeftKeyOne = virtualKey;
		break;
	case kRightControlOne:
		g_tempRightKeyOne = virtualKey;
		break;
	case kBattControlOne:
		g_tempBattKeyOne = virtualKey;
		break;
	case kBandControlOne:
		g_tempBandKeyOne = virtualKey;
		break;
	case kLeftControlTwo:
		g_tempLeftKeyTwo = virtualKey;
		break;
	case kRightControlTwo:
		g_tempRightKeyTwo = virtualKey;
		break;
	case kBattControlTwo:
		g_tempBattKeyTwo = virtualKey;
		break;
	case kBandControlTwo:
		g_tempBandKeyTwo = virtualKey;
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
	SetHotKeyValue(prefDlg, kLeftControlOne, (BYTE)g_theGlider.leftKey);
	SetHotKeyValue(prefDlg, kRightControlOne, (BYTE)g_theGlider.rightKey);
	SetHotKeyValue(prefDlg, kBattControlOne, (BYTE)g_theGlider.battKey);
	SetHotKeyValue(prefDlg, kBandControlOne, (BYTE)g_theGlider.bandKey);
	SetHotKeyValue(prefDlg, kLeftControlTwo, (BYTE)g_theGlider2.leftKey);
	SetHotKeyValue(prefDlg, kRightControlTwo, (BYTE)g_theGlider2.rightKey);
	SetHotKeyValue(prefDlg, kBattControlTwo, (BYTE)g_theGlider2.battKey);
	SetHotKeyValue(prefDlg, kBandControlTwo, (BYTE)g_theGlider2.bandKey);
	if (g_isEscPauseKey)
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
	g_theGlider.leftKey = g_tempLeftKeyOne;
	g_theGlider.rightKey = g_tempRightKeyOne;
	g_theGlider.battKey = g_tempBattKeyOne;
	g_theGlider.bandKey = g_tempBandKeyOne;
	g_theGlider2.leftKey = g_tempLeftKeyTwo;
	g_theGlider2.rightKey = g_tempRightKeyTwo;
	g_theGlider2.battKey = g_tempBattKeyTwo;
	g_theGlider2.bandKey = g_tempBandKeyTwo;
	if (IsDlgButtonChecked(prefDlg, kESCPausesRadio))
	{
		g_isEscPauseKey = true;
	}
	else if (IsDlgButtonChecked(prefDlg, kTABPausesRadio))
	{
		g_isEscPauseKey = false;
	}
	else
	{
		g_isEscPauseKey = false;
	}
}

//--------------------------------------------------------------  ControlFilter

INT_PTR CALLBACK ControlFilter (HWND prefDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	(void)lParam;

	switch (message)
	{
	case WM_INITDIALOG:
		CenterDialogOverOwner(prefDlg);
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

	CheckDlgButton(prefDlg, kIdleMusicItem, (g_isPlayMusicIdle != 0));
	CheckDlgButton(prefDlg, kPlayMusicItem, (g_isPlayMusicGame != 0));
}

//--------------------------------------------------------------  SoundPrefsApply

void SoundPrefsApply (HWND prefDlg)
{
	SInt16 tempVolume;

	g_isPlayMusicIdle = (IsDlgButtonChecked(prefDlg, kIdleMusicItem) != 0);
	g_isPlayMusicGame = (IsDlgButtonChecked(prefDlg, kPlayMusicItem) != 0);

	UnivGetSoundVolume(&tempVolume);
	g_isSoundOn = (tempVolume != 0);
}

//--------------------------------------------------------------  HandleSoundMusicChange

void HandleSoundMusicChange (HWND prefDlg, SInt16 newVolume, Boolean sayIt)
{
	OSErr theErr;

	g_isSoundOn = (newVolume != 0);

	if (IsDlgButtonChecked(prefDlg, kIdleMusicItem))
	{
		if (newVolume == 0)
			StopTheMusic();
		else
		{
			if (!g_isMusicOn)
			{
				theErr = StartMusic();
				if (theErr != noErr)
				{
					YellowAlert(prefDlg, kYellowNoMusic, theErr);
					g_failedMusic = true;
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
	SInt16 tempVolume;
	Boolean wasIdle;

	switch (message)
	{
	case WM_INITDIALOG:
		CenterDialogOverOwner(prefDlg);
		UnivGetSoundVolume(&g_wasLoudness);
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
			UnivSetSoundVolume(g_wasLoudness);
			HandleSoundMusicChange(prefDlg, g_wasLoudness, false);
			g_isPlayMusicIdle = (g_isPlayMusicIdle != 0);
			wasIdle = (IsDlgButtonChecked(prefDlg, kIdleMusicItem) != 0);
			if (g_isPlayMusicIdle != wasIdle)
			{
				if (g_isPlayMusicIdle)
				{
					if (g_wasLoudness != 0)
					{
						OSErr theErr = StartMusic();
						if (theErr != noErr)
						{
							YellowAlert(prefDlg, kYellowNoMusic, theErr);
							g_failedMusic = true;
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
						g_failedMusic = true;
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
	if (g_isViewportWidth <= kMinScreenWidth)
	{
		CheckRadioButton(hDlg, kDisplay1Item, kDisplay9Item, kDisplay1Item);
	}
	else
	{
		CheckRadioButton(hDlg, kDisplay1Item, kDisplay9Item, kDisplay9Item);
	}
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

	if (g_thisMac.screen.right <= kRoomWide)
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

	if (g_numNeighbors == 1)
		CheckRadioButton(hDlg, kDisplay1Item, kDisplay9Item, kDisplay1Item);
	else if (g_numNeighbors == 3)
		CheckRadioButton(hDlg, kDisplay1Item, kDisplay9Item, kDisplay3Item);
	else if (g_numNeighbors == 9)
		CheckRadioButton(hDlg, kDisplay1Item, kDisplay9Item, kDisplay9Item);
	else
		CheckRadioButton(hDlg, kDisplay1Item, kDisplay9Item, kDisplay9Item);

	CheckRadioButton(hDlg, kCurrentDepth, k16Depth, kCurrentDepth);

	if (g_isDoColorFade)
		CheckDlgButton(hDlg, kDoColorFadeItem, BST_CHECKED);
	else
		CheckDlgButton(hDlg, kDoColorFadeItem, BST_UNCHECKED);

	CheckDlgButton(hDlg, kUseQDItem, BST_CHECKED);

	if (g_isUseSecondScreen)
		CheckDlgButton(hDlg, kUseScreen2Item, BST_CHECKED);
	else
		CheckDlgButton(hDlg, kUseScreen2Item, BST_UNCHECKED);
}

//--------------------------------------------------------------  DisplayApply

void DisplayApply (HWND hDlg)
{
	Boolean wasScreen2;

	if (IsDlgButtonChecked(hDlg, kDisplay1Item))
		g_numNeighbors = 1;
	else if (IsDlgButtonChecked(hDlg, kDisplay3Item))
		g_numNeighbors = 3;
	else if (IsDlgButtonChecked(hDlg, kDisplay9Item))
		g_numNeighbors = 9;
	else
		g_numNeighbors = 9;

	g_isDoColorFade = (IsDlgButtonChecked(hDlg, kDoColorFadeItem) != 0);

	wasScreen2 = (g_isUseSecondScreen != 0);
	g_isUseSecondScreen = (IsDlgButtonChecked(hDlg, kUseScreen2Item) != 0);
	if (wasScreen2 != g_isUseSecondScreen)
		g_nextRestartChange = true;
}

//--------------------------------------------------------------  DisplayFilter

INT_PTR CALLBACK DisplayFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	(void)lParam;

	switch (message)
	{
	case WM_INITDIALOG:
		CenterDialogOverOwner(hDlg);
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
	OSErr theErr;

	// Default brain settings
	g_willMaxFiles = 48;
	g_doAutoDemo = true;
	g_isHouseChecks = true;
	g_doPrettyMap = true;
	g_doBitchDialogs = true;

	// Default control settings
	g_theGlider.leftKey = VK_LEFT;
	g_theGlider.rightKey = VK_RIGHT;
	g_theGlider.battKey = VK_DOWN;
	g_theGlider.bandKey = VK_UP;
	g_theGlider2.leftKey = 'A';
	g_theGlider2.rightKey = 'D';
	g_theGlider2.battKey = 'S';
	g_theGlider2.bandKey = 'W';
	g_isEscPauseKey = false;

	// Default sound settings
	g_isPlayMusicIdle = true;
	g_isPlayMusicGame = true;
	UnivSetSoundVolume(3);
	g_isSoundOn = true;
	if (!g_isMusicOn)
	{
		theErr = StartMusic();
		if (theErr != noErr)
		{
			YellowAlert(ownerWindow, kYellowNoMusic, theErr);
			g_failedMusic = true;
		}
	}

	// Default display settings
	if (g_isViewportWidth <= kMinScreenWidth)
	{
		g_numNeighbors = 1;
	}
	else
	{
		g_numNeighbors = 9;
	}
	g_quickerTransitions = false;
	g_isDoColorFade = true;
}

//--------------------------------------------------------------  PrefsFilter

INT_PTR CALLBACK PrefsFilter (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HICON hIcon;

	(void)lParam;

	switch (message)
	{
	case WM_INITDIALOG:
		CenterDialogOverOwner(hDlg);

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
			if ((OptionKeyDown()) && (!g_houseUnlocked))
			{
				g_houseUnlocked = true;
				g_changeLockStateOfHouse = true;
				g_saveHouseLocked = false;
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
	g_nextRestartChange = false;

	DialogBox(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(kMainPrefsDialID),
			ownerWindow, PrefsFilter);

	if (g_nextRestartChange)
		BitchAboutChanges(ownerWindow);
}

//--------------------------------------------------------------  BitchAboutChanges

void BitchAboutChanges (HWND ownerWindow)
{
	Alert(kChangesEffectAlert, ownerWindow, NULL);
}
