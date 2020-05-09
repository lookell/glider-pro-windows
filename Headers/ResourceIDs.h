#ifndef RESOURCE_IDS_H_
#define RESOURCE_IDS_H_

#ifdef RC_INVOKED
#define RT_DEMO DEMO
#else
#define RT_DEMO L"DEMO"
#endif

#define kJinjurStringBase			1000
#define kPrefMainStringBase			1100
#define rFileErrorStringsBase		1200
#define kLocalizedStringsBase		1300
#define kPrefsStringsBase			1400
#define rErrTitleBase				1500
#define rErrMssgBase				1600
#define kMonthStringBase			1700
#define kYellowAlertStringBase		1800
#define kObjectNameStringsBase		1900

#define IDR_DEMO				128

#define IDM_ROOT				1

#define IDM_APPLE				128
#define ID_ABOUT				40101

#define IDM_GAME				129
#define ID_NEW_GAME				40201
#define ID_TWO_PLAYER			40202
#define ID_OPEN_SAVED_GAME		40203
#define ID_LOAD_HOUSE			40205
#define ID_QUIT					40207

#define IDM_OPTIONS				130
#define ID_EDITOR				40301
#define ID_HIGH_SCORES			40303
#define ID_PREFS				40304
#define ID_DEMO					40305

#define IDM_HOUSE				131
#define ID_NEW_HOUSE			40401
#define ID_SAVE_HOUSE			40402
#define ID_HOUSE_INFO			40404
#define ID_ROOM_INFO			40405
#define ID_OBJECT_INFO			40406
#define ID_CUT					40408
#define ID_COPY					40409
#define ID_PASTE				40410
#define ID_CLEAR				40411
#define ID_DUPLICATE			40412
#define ID_BRING_FORWARD		40414
#define ID_SEND_BACK			40415
#define ID_GO_TO_ROOM			40417
#define ID_MAP_WINDOW			40419
#define ID_OBJECT_WINDOW		40420
#define ID_COORDINATE_WINDOW	40421

#define IDI_APPL				1
#define IDI_PREFS				2
#define IDI_HOUSE				3
#define IDI_GAME				4
#define IDI_SCORE				5
#define IDI_MOVIE				6

#define	kSwitchDepthAlert		130
#define rFileErrorAlert			140
#define kNewPrefsAlertID		160
#define rDeathAlertID			170
#define kSetMemoryAlert			180
#define kLowMemoryAlert			181
#define kSaveChangesAlert		1002
#define kNewRoomAlert			1004
#define kDeleteRoomAlert		1005
#define kYellowAlert			1006
#define kNoMoreObjectsAlert		1008
#define kHouseBannerAlert		1009
#define kNoMoreSpecialAlert		1028
#define kLockHouseAlert			1029
#define kNoSoundManager3Alert	1030
#define kNoPrintingAlert		1031
#define kZeroScoresAlert		1032
#define kNoPICTFoundAlert		1036
#define kNotInDemoAlert			1037
#define kNoMemForMusicAlert		1038
#define kNoMemForSoundsAlert	1039
#define kChangesEffectAlert		1040
#define kSaveGameAlert			1041
#define kColorSwitchedAlert		1042
#define kSavedGameErrorAlert	1044
#define kNoHighScoreAlert		1046

#define kAboutDialogID				150
#define kLoadHouseDialogID			1000
#define kHouseInfoDialogID			1001
#define kRoomInfoDialogID			1003
#define kBlowerInfoDialogID			1007
#define kFurnitureInfoDialogID		1010
#define kSwitchInfoDialogID			1011
#define kMainPrefsDialID			1012
#define kLightInfoDialogID			1013
#define kApplianceInfoDialogID		1014
#define kInvisBonusInfoDialogID		1015
#define kOriginalArtDialogID		1016
#define kDisplayPrefsDialID			1017
#define kSoundPrefsDialID			1018
#define kGreaseInfoDialogID			1019
#define kHighNameDialogID			1020
#define kHighBannerDialogID			1021
#define kTransInfoDialogID			1022
#define kControlPrefsDialID			1023
#define kBrainsPrefsDialID			1024
#define kResumeGameDial				1025
#define kMasterDialogID				1026
#define kEnemyInfoDialogID			1027
#define kFlowerInfoDialogID			1033
#define kTriggerInfoDialogID		1034
#define kMicrowaveInfoDialogID		1035
#define kGoToDialogID				1043
#define kCustPictInfoDialogID		1045

#endif
