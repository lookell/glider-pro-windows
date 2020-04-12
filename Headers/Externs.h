
//============================================================================
//----------------------------------------------------------------------------
//								Externs.h
//----------------------------------------------------------------------------
//============================================================================


#pragma once


//#include <Menus.h>
#include "WinAPI.h"
#include "Macintosh.h"


#define kPreferredDepth				8


#define	kNilPointer					0L
#define	kPutInFront					(WindowPtr)-1L
#define	kNormalUpdates				TRUE
#define kOneKilobyte				1024
#define kOkayButton					1
#define kCancelButton				2
#define kControlActive				0
#define kControlInactive			255
#define kAsynch						TRUE
#define kSynch						FALSE

#define kHomeKeyASCII				0x01
#define kEnterKeyASCII				0x03
#define kEndKeyASCII				0x04
#define kHelpKeyASCII				0x05
#define	kDeleteKeyASCII				0x08
#define kTabKeyASCII				0x09
#define kPageUpKeyASCII				0x0B
#define kPageDownKeyASCII			0x0C
#define	kReturnKeyASCII				0x0D
#define	kFunctionKeyASCII			0x10
#define	kClearKeyASCII				0x1A
#define	kEscapeKeyASCII				0x1B
#define	kLeftArrowKeyASCII			0x1C
#define kRightArrowKeyASCII			0x1D
#define	kUpArrowKeyASCII			0x1E
#define kDownArrowKeyASCII			0x1F
#define kSpaceBarASCII				0x20
#define kExclamationASCII			0x21
#define kPlusKeyASCII				0x2B
#define kMinusKeyASCII				0x2D
#define k0KeyASCII					0x30
#define k1KeyASCII					0x31
#define k2KeyASCII					0x32
#define k3KeyASCII					0x33
#define k4KeyASCII					0x34
#define k5KeyASCII					0x35
#define k6KeyASCII					0x36
#define k7KeyASCII					0x37
#define k8KeyASCII					0x38
#define k9KeyASCII					0x39

#define kCapAKeyASCII				0x41
#define kCapBKeyASCII				0x42
#define kCapCKeyASCII				0x43
#define kCapDKeyASCII				0x44
#define kCapEKeyASCII				0x45
#define kCapFKeyASCII				0x46
#define kCapGKeyASCII				0x47
#define kCapHKeyASCII				0x48
#define kCapIKeyASCII				0x49
#define kCapJKeyASCII				0x4A
#define kCapKKeyASCII				0x4B
#define kCapLKeyASCII				0x4C
#define kCapMKeyASCII				0x4D
#define kCapNKeyASCII				0x4E
#define kCapOKeyASCII				0x4F
#define kCapPKeyASCII				0x50
#define kCapQKeyASCII				0x51
#define kCapRKeyASCII				0x52
#define kCapSKeyASCII				0x53
#define kCapTKeyASCII				0x54
#define kCapUKeyASCII				0x55
#define kCapVKeyASCII				0x56
#define kCapWKeyASCII				0x57
#define kCapXKeyASCII				0x58
#define kCapYKeyASCII				0x59
#define kCapZKeyASCII				0x5A

#define kAKeyASCII					0x61
#define kBKeyASCII					0x62
#define kCKeyASCII					0x63
#define kDKeyASCII					0x64
#define kEKeyASCII					0x65
#define kFKeyASCII					0x66
#define kGKeyASCII					0x67
#define kHKeyASCII					0x68
#define kIKeyASCII					0x69
#define kJKeyASCII					0x6A
#define kKKeyASCII					0x6B
#define kLKeyASCII					0x6C
#define kMKeyASCII					0x6D
#define kNKeyASCII					0x6E
#define kOKeyASCII					0x6F
#define kPKeyASCII					0x70
#define kQKeyASCII					0x71
#define kRKeyASCII					0x72
#define kSKeyASCII					0x73
#define kTKeyASCII					0x74
#define kUKeyASCII					0x75
#define kVKeyASCII					0x76
#define kWKeyASCII					0x77
#define kXKeyASCII					0x78
#define kYKeyASCII					0x79
#define kZKeyASCII					0x7A
#define kForwardDeleteASCII			0x7F

#define	kPlusKeypadMap				66		// key map offset for + on keypad
#define	kMinusKeypadMap				73		// key map offset for - on keypad
#define	kTimesKeypadMap				68		// key map offset for * on keypad
#define	k0KeypadMap					85		// key map offset for 0 on keypad
#define	k1KeypadMap					84		// key map offset for 1 on keypad
#define	k2KeypadMap					83		// key map offset for 2 on keypad
#define	k3KeypadMap					82		// key map offset for 3 on keypad
#define k4KeypadMap					81		// key map offset for 4 on keypad
#define k5KeypadMap					80		// key map offset for 5 on keypad
#define k6KeypadMap					95		// key map offset for 6 on keypad
#define k7KeypadMap					94		// key map offset for 7 on keypad
#define k8KeypadMap					92		// key map offset for 8 on keypad
#define k9KeypadMap					91		// key map offset for 9 on keypad

#define kUpArrowKeyMap				121		// key map offset for up arrow
#define kDownArrowKeyMap			122		// key map offset for down arrow
#define kRightArrowKeyMap			123		// key map offset for right arrow
#define kLeftArrowKeyMap			124		// key map offset for left arrow

#define kAKeyMap					7
#define	kBKeyMap					12
#define	kCKeyMap					15
#define	kDKeyMap					5
#define	kEKeyMap					9
#define	kFKeyMap					4
#define	kGKeyMap					2
#define	kHKeyMap					3
#define	kMKeyMap					41
#define	kNKeyMap					42
#define	kOKeyMap					24
#define	kPKeyMap					36
#define	kQKeyMap					11
#define	kRKeyMap					8
#define kSKeyMap					6
#define kTKeyMap					22
#define kVKeyMap					14
#define kWKeyMap					10
#define kXKeyMap					0
#define kZKeyMap					1
#define kPeriodKeyMap				40
#define	kCommandKeyMap				48
#define	kEscKeyMap					50
#define	kDeleteKeyMap				52
#define kSpaceBarMap				54
#define	kTabKeyMap					55
#define	kControlKeyMap				60
#define	kOptionKeyMap				61
#define	kCapsLockKeyMap				62
#define	kShiftKeyMap				63

#define kTabRawKey					0x30	// key map offset for Tab key
#define kClearRawKey				0x47	// key map offset for Clear key
#define kF5RawKey					0x60	// key map offset for F5
#define kF6RawKey					0x61	// key map offset for F6
#define kF7RawKey					0x62	// key map offset for F7
#define kF3RawKey					0x63	// key map offset for F3
#define kF8RawKey					0x64	// key map offset for F8
#define kF9RawKey					0x65	// key map offset for F9
#define kF11RawKey					0x67	// key map offset for F11
#define kF13RawKey					0x69	// key map offset for F13
#define kF14RawKey					0x6B	// key map offset for F14
#define kF10RawKey					0x6D	// key map offset for F10
#define kF12RawKey					0x6F	// key map offset for F12
#define kF15RawKey					0x71	// key map offset for F15
#define kF4RawKey					0x76	// key map offset for F4
#define kF2RawKey					0x78	// key map offset for F2
#define kF1RawKey					0x7A	// key map offset for F1

#define	kErrUnnaccounted			1
#define	kErrNoMemory				2
#define kErrDialogDidntLoad			3
#define kErrFailedResourceLoad		4
#define kErrFailedGraphicLoad		5
#define kErrFailedOurDirect			6
#define kErrFailedValidation		7
#define kErrNeedSystem7				8
#define kErrFailedGetDevice			9
#define kErrFailedMemoryOperation	10
#define kErrFailedCatSearch			11
#define kErrNeedColorQD				12
#define kErrNeed16Or256Colors		13

#define iAbout					1
#define iNewGame				1
#define iTwoPlayer				2
#define iOpenSavedGame			3
#define iLoadHouse				5
#define iQuit					7
#define iEditor					1
#define iHighScores				3
#define iPrefs					4
#define iHelp					5
#define iNewHouse				1
#define iSave					2
#define iHouse					4
#define iRoom					5
#define iObject					6
#define iCut					8
#define iCopy					9
#define iPaste					10
#define iClear					11
#define iDuplicate				12
#define iBringForward			14
#define iSendBack				15
#define iGoToRoom				17
#define iMapWindow				19
#define iObjectWindow			20
#define iCoordinateWindow		21

#define WC_MAINWINDOW			L"GliderMainWindow"

//--------------------------------------------------------------  Structs
/*
typedef	SInt16		SICN[16];
typedef	SICN		*SICNList;
typedef	SICNList	*SICNHand;
*/

//#pragma options align=mac68k

typedef struct
{
	Str32		wasDefaultName;
	Str15		wasLeftName, wasRightName;
	Str15		wasBattName, wasBandName;
	Str15		wasHighName;
	Str31		wasHighBanner;
//	SInt32		encrypted, fakeLong;
	SInt32		wasLeftMap, wasRightMap;
	SInt32		wasBattMap, wasBandMap;
	SInt16		wasVolume;
	SInt16		prefVersion;
	SInt16		wasMaxFiles;
	SInt16		wasEditH, wasEditV;
	SInt16		wasMapH, wasMapV;
	SInt16		wasMapWide, wasMapHigh;
	SInt16		wasToolsH, wasToolsV;
	SInt16		wasLinkH, wasLinkV;
	SInt16		wasCoordH, wasCoordV;
	SInt16		isMapLeft, isMapTop;
	SInt16		wasNumNeighbors;
	SInt16		wasDepthPref;
	SInt16		wasToolGroup;
	SInt16		smWarnings;
	SInt16		wasFloor, wasSuite;
	Boolean		wasZooms, wasMusicOn;
	Boolean		wasAutoEdit, wasDoColorFade;
	Boolean		wasMapOpen, wasToolsOpen;
	Boolean		wasCoordOpen, wasQuickTrans;
	Boolean		wasIdleMusic, wasGameMusic;
	Boolean		wasEscPauseKey;
	Boolean		wasDoAutoDemo, wasScreen2;
	Boolean		wasDoBackground, wasHouseChecks;
	Boolean		wasPrettyMap, wasBitchDialogs;
} prefsInfo;

//#pragma options align=reset

//--------------------------------------------------------------  Prototypes

void DoAbout (void);									// --- About.c

void LoadCursors (void);								// --- AnimCursor.c
void DisposCursors (void);
void IncrementCursor (void);
void DecrementCursor (void);
void SpinCursor (SInt16);
void BackSpinCursor (SInt16);

COLORREF Index2ColorRef (SInt32);						// --- ColorUtils.c
void ColorText (HDC, StringPtr, SInt32);
void ColorRect (HDC, Rect *, SInt32);
void ColorOval (HDC, Rect *, SInt32);
void ColorRegion (RgnHandle, SInt32);
void ColorLine (SInt16, SInt16, SInt16, SInt16, SInt32);
void HiliteRect (Rect *, SInt16, SInt16);
void ColorFrameRect (Rect *, SInt32);
void ColorFrameWHRect (SInt16, SInt16, SInt16, SInt16, SInt32);
void ColorFrameOval (Rect *, SInt32);
void LtGrayForeColor (void);
void GrayForeColor (void);
void DkGrayForeColor (void);
void RestoreColorsSlam (void);

void MonitorWait (void);								// --- DebugUtils.c
void DisplayRect (Rect *);
void FlashRect (Rect *);
void CheckLegitRect(Rect *, Rect *);
void DisplayLong (SInt32);
void DisplayShort (SInt16);
void FlashLong (SInt32);
void FlashShort (SInt16);
void DoBarGraph (SInt16, SInt16, SInt16, SInt16);
SInt16 BetaOkay (void);
void DebugNum (SInt32);
void DisplayCTSeed (CGrafPtr);
void FillScreenRed (void);
void DumpToResEditFile (Ptr, SInt32);

void HandleEvent (void);								// --- Event.c
void HiliteAllWindows (void);
void IgnoreThisClick (void);

SInt16 WhatsOurDepth (void);							// --- Environs.c
void SwitchToDepth (SInt16, Boolean);
void CheckOurEnvirons (void);
//void ReflectSecondMonitorEnvirons (Boolean, Boolean, Boolean);
void HandleDepthSwitching (void);
void RestoreColorDepth (void);
void CheckMemorySize (void);
void SetAppMemorySize (SInt32);

Boolean CheckFileError (SInt16, StringPtr);				// --- File Error.c

Boolean SavePrefs (prefsInfo *, SInt16);				// --- Prefs.c
Boolean LoadPrefs (prefsInfo *, SInt16);

void PasStringCopy (StringPtr, StringPtr);				// --- StringUtils.c
void PasStringCopyC (const char *, StringPtr);
SInt16 WhichStringFirst (StringPtr, StringPtr);
void PasStringCopyNum (StringPtr, StringPtr, SInt16);
void PasStringConcat (StringPtr, StringPtr);
void PasStringConcatC (StringPtr, const char *);
void GetLineOfText (StringPtr, SInt16, StringPtr);
void WrapText (StringPtr, SInt16);
void GetFirstWordOfString (StringPtr, StringPtr);
void CollapseStringToWidth (StringPtr, SInt16);
void GetChooserName (StringPtr);
StringPtr GetLocalizedString (SInt16, StringPtr);

Point MyGetGlobalMouse (void);							// --- Utilities.c
void ToolBoxInit (void);
void FindOurDevice (void);
SInt16 RandomInt (SInt16);
SInt32 RandomLong (SInt32);
void InitRandomLongQUS (void);
UInt32 RandomLongQUS (void);
//void CenterAlert (SInt16);
void RedAlert (SInt16);
//void CreateOffScreenBitMap (Rect *, GrafPtr *);
//void CreateOffScreenPixMap (Rect *, CGrafPtr *);
//void KillOffScreenPixMap (CGrafPtr);
//void KillOffScreenBitMap (GrafPtr);
HBITMAP GetPicture (SInt16);
void LoadGraphic (HDC, SInt16);
void LoadScaledGraphic (HDC, SInt16, Rect *);
//void PlotSICN (Rect *, SICNHand, SInt32);
void LargeIconPlot (Rect *, SInt16);
void DrawCIcon (SInt16, SInt16, SInt16);
char KeyMapOffsetFromRawKey (char);
SInt32 LongSquareRoot (SInt32);
//void HideMenuBarOld (void);
//void ShowMenuBarOld (void);
Boolean WaitForInputEvent (SInt16);
void WaitCommandQReleased (void);
char GetKeyMapFromMessage (SInt32);
void GetKeyName (SInt32, StringPtr);
Boolean OptionKeyDown (void);
SInt32 ExtractCTSeed (CGrafPtr);
//void ForceCTSeed (CGrafPtr, SInt32);
void DelayTicks (SInt32);
void UnivGetSoundVolume (SInt16 *, Boolean);
void  UnivSetSoundVolume (SInt16, Boolean);

Boolean ValidInstallation (Boolean);					// --- Validate.c

void GetWindowLeftTop (HWND, SInt16 *, SInt16 *);	// --- WindowUtils.c
void Gp_GetWindowRect (HWND, Rect *);
void GetLocalWindowRect (HWND, Rect *);
//void FlagWindowFloating (WindowPtr);
//Boolean	IsWindowFloating (WindowPtr);
void OpenMessageWindow (StringPtr);
void SetMessageWindowMessage (StringPtr);
void CloseMessageWindow (void);
void CloseThisWindow (HWND *);

#ifdef powerc
//	extern pascal void SetSoundVol(SInt16 level);		// for old Sound Manager
//	extern pascal void GetSoundVol(SInt16 *level)
//	THREEWORDINLINE(0x4218, 0x10B8, 0x0260);
#endif

#include "GliderDefines.h"
#include "GliderStructs.h"
#include "GliderVars.h"
#include "GliderProtos.h"

struct byteio;
int ReadPrefsInfo(struct byteio *, prefsInfo *);		// --- GliderStructs.c
int WritePrefsInfo(struct byteio *, const prefsInfo *);
int ReadBlowerType(struct byteio *, blowerType *);
int WriteBlowerType(struct byteio *, const blowerType *);
int ReadFurnitureType(struct byteio *, furnitureType *);
int WriteFurnitureType(struct byteio *, const furnitureType *);
int ReadBonusType(struct byteio *, bonusType *);
int WriteBonusType(struct byteio *, const bonusType *);
int ReadTransportType(struct byteio *, transportType *);
int WriteTransportType(struct byteio *, const transportType *);
int ReadSwitchType(struct byteio *, switchType *);
int WriteSwitchType(struct byteio *, const switchType *);
int ReadLightType(struct byteio *, lightType *);
int WriteLightType(struct byteio *, const lightType *);
int ReadApplianceType(struct byteio *, applianceType *);
int WriteApplianceType(struct byteio *, const applianceType *);
int ReadEnemyType(struct byteio *, enemyType *);
int WriteEnemyType(struct byteio *, const enemyType *);
int ReadClutterType(struct byteio *, clutterType *);
int WriteClutterType(struct byteio *, const clutterType *);
int ReadObjectType(struct byteio *, objectType *);
int WriteObjectType(struct byteio *, const objectType *);
int ReadScoresType(struct byteio *, scoresType *);
int WriteScoresType(struct byteio *, const scoresType *);
int ReadGameType(struct byteio *, gameType *);
int WriteGameType(struct byteio *, const gameType *);
int ReadSavedRoom(struct byteio *, savedRoom *);
int WriteSavedRoom(struct byteio *, const savedRoom *);
int ReadGame2Type(struct byteio *, game2Type *);
int WriteGame2Type(struct byteio *, const game2Type *);
int ReadRoomType(struct byteio *, roomType *);
int WriteRoomType(struct byteio *, const roomType *);
int ReadHouseType(struct byteio *, houseType *);
int WriteHouseType(struct byteio *, const houseType *);
int ReadBoundsType(struct byteio *, boundsType *);
int WriteBoundsType(struct byteio *, const boundsType *);
int ReadDemoType(struct byteio *, demoType *);
int WriteDemoType(struct byteio *, const demoType *);
