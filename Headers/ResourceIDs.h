#ifndef RESOURCE_IDS_H_
#define RESOURCE_IDS_H_

#include "GliderDefines.h"

#ifdef RC_INVOKED
#define RT_DEMO DEMO
#else
#define RT_DEMO L"DEMO"
#endif

#ifdef RC_INVOKED
#define RT_ACUR ACUR
#else
#define RT_ACUR L"ACUR"
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

#define IDA_SPLASH				1000
#define IDA_EDIT				1002

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

#define IDM_BACKGROUNDS         140
#define kBackgroundsMenuID      IDM_BACKGROUNDS

#define IDM_TOOLS               141

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

#define kCoordinateWindowID     10001
#define kMessageWindowID        10002
#define kLinkWindowID           10003
#define kToolsWindowID          10004

#define kColorDepthIcon         130
#define kFloppyDiskIcon         140
#define kAboutIcon              150
#define kNewPrefsIcon           160
#define kDeathIcon              170
#define kSetMemoryIcon          180
#define kNoPrintingIcon         900
#define kZeroScoresIcon         910
#define kPlainHouseIcon         1000
#define kResumeGameIcon         1001
#define kNewRoomIcon            1004
#define kDeleteRoomIcon         1005
#define kYellowAlertIcon        1006
#define kNoMoreObjectsIcon      1008
#define kNormalDisplayIcon      1010
#define kNormalSoundsIcon       1011
#define kNormalControlsIcon     1012
#define kNormalBrainsIcon       1013
#define kInvertedDisplayIcon    1014
#define kInvertedSoundsIcon     1015
#define kInvertedControlsIcon   1016
#define kInvertedBrainsIcon     1017
#define kDisplay1Icon           1020
#define kDisplay3Icon           1021
#define kDisplay9Icon           1022
#define kSoundSliderIcon        1030
#define kNormalLouderIcon       1031
#define kNormalSofterIcon       1032
#define kPressedLouderIcon      1033
#define kPressedSofterIcon      1034
#define kRightControlIcon       1040
#define kLeftControlIcon        1041
#define kBattControlIcon        1042
#define kBandControlIcon        1043
#define kScrollUpArrow          1050
#define kScrollDownArrow        1051
#define kGrayedOutUpArrow       1052
#define kGrayedOutDownArrow     1053
#define kLockHouseIcon          1060
#define kNoSoundManager3Icon    1070
#define kNoPICTFoundIcon        1071
#define kSaveGameIcon           1072
#define kNoHighScoreIcon        1073
#define kSelectionTool          2000

#define kHandCursorID           128
#define kVertCursorID           129
#define kHoriCursorID           130
#define kDiagCursorID           131
#define kBeachBallCursor12      149
#define kBeachBallCursor11      150
#define kBeachBallCursor10      151
#define kBeachBallCursor09      152
#define kBeachBallCursor08      153
#define kBeachBallCursor07      154
#define kBeachBallCursor06      155
#define kBeachBallCursor05      156
#define kBeachBallCursor04      157
#define kBeachBallCursor03      158
#define kBeachBallCursor02      159
#define kBeachBallCursor01      160

#define rAcurID                 128

#define kMarqueePatListID       128

#define kOkayButtPICTNotHiLit   150
#define kOkayButtPICTHiLit      151
#define kAboutBanner            153
#define kSplash8BitPICT         1000
#define kLoadTitlePict1         1001
#define kObjectInfoTitle        1002
#define kDefaultHousePict1      1003
#define kDefaultHousePict8      1004
#define kHouseInfoTitle         1005
#define kDisplayPrefsTitle      1006
#define kRoomInfoTitle          1007
#define kSoundPrefsTitle        1008
#define kRoomInfoInstruct       1009
#define kThumbnailPictID        1010
#define kToolsPictID            1011
#define kControlPrefsTitle      1012
#define kMainPrefsTitle         1013
#define kBrainsPrefsTitle       1014
#define kEscPausePictID         1015
#define kTabPausePictID         1016
#define kStarsRemainingPICT     1017
#define kStarRemainingPICT      1018
#define kAngelPictID            1019
#define kAngelMaskID            1020    // kAngelPictID + 1
#define kMilkywayPictID         1021
#define kMicrowaveInfoTitle     1022
#define kGoToTitle              1023
#define kBasementTileSrc        1202    // kBasement - 800
#define kDirtTileSrc            1211    // kDirt - 800
#define kStratosphereTileSrc    1216    // kStratosphere - 800
#define kStarsTileSrc           1217    // kStars - 800
#define kLettersPictID          1988
#define kPagesMaskID            1989
#define kPagesPictID            1990
#define kBannerPageBottomMask   1991
#define kBannerPageBottomPICT   1992
#define kBannerPageTopPICT      1993
#define kHighScoresPictID       1994
#define kStarPictID             1995
#define kBadgePictID            1996
#define kScoreboardPictID       1997
#define kHighScoresMaskID       1998
#define kSupportPictID          1999

// NOTE: Background PICT IDs are in GliderDefines.h

//#define kBBQMaskID              3900
//#define kUpStairsMaskID         3901
//#define kTrunkMaskID            3902
#define kMailboxRightMaskID     3903
#define kMailboxLeftMaskID      3904
//#define kDoorInLeftMaskID       3905
//#define kDoorInRightMaskID      3906
//#define kWindowInLeftMaskID     3907
//#define kWindowInRightMaskID    3908
//#define kHipLampMaskID          3909
//#define kDecoLampMaskID         3910
//#define kGuitarMaskID           3911
#define kTVMaskID               3912
#define kVCRMaskID              3913
#define kStereoMaskID           3914
#define kMicrowaveMaskID        3915
//#define kFireplaceMaskID        3916
//#define kBearMaskID             3917
//#define kVase1MaskID            3918
//#define kVase2MaskID            3919
//#define kManholeMaskID          3920
#define kCloudMaskID            3921
//#define kBooksMaskID            3922
//#define kRugMaskID              3923
//#define kChimesMaskID           3924
//#define kCinderMaskID           3925
//#define kFlowerBoxMaskID        3926
#define kCobwebMaskID           3927
#define kManholeThruFloor       3957
#define kCobwebPictID           3958
#define kFlowerBoxPictID        3959
#define kCinderPictID           3960
#define kChimesPictID           3961
#define kRugPictID              3962
#define kGliderFoil2PictID      3963
#define kBooksPictID            3964
#define kCloudPictID            3965
#define kBulletinPictID         3966
#define kManholePictID          3967
#define kVase2PictID            3968
#define kVase1PictID            3969
#define kCalendarPictID         3970
#define kMicrowavePictID        3971
#define kBearPictID             3972
#define kFireplacePictID        3973
#define kGlider2PictID          3974
#define kOzmaPictID             3975
#define kGliderFoilPictID       3976
#define kWindowExRightPictID    3977
#define kWindowExLeftPictID     3978
#define kWindowInRightPictID    3979
#define kWindowInLeftPictID     3980
#define kDoorExLeftPictID       3981
#define kDoorExRightPictID      3982
#define kDoorInRightPictID      3983
#define kDoorInLeftPictID       3984
#define kMailboxRightPictID     3985
#define kMailboxLeftPictID      3986
#define kTrunkPictID            3987
#define kBBQPictID              3988
#define kStereoPictID           3989
#define kVCRPictID              3990
#define kGuitarPictID           3991
#define kTVPictID               3992
#define kDecoLampPictID         3993
#define kHipLampPictID          3994
#define kFilingCabinetPictID    3995
#define kDownStairsPictID       3996
#define kUpStairsPictID         3997
#define kShadowPictID           3998
#define kGliderPictID           3999
#define kBlowerPictID           4000
#define kFurniturePictID        4001
#define kBonusPictID            4002
#define kSwitchPictID           4003
#define kLightPictID            4004
#define kAppliancePictID        4005
#define kPointsPictID           4006
#define kRubberBandsPictID      4007
#define kTransportPictID        4008
#define kToastPictID            4009
#define kShreddedPictID         4010
#define kBalloonPictID          4011
#define kCopterPictID           4012
#define kDartPictID             4013
#define kBallPictID             4014
#define kDripPictID             4015
#define kEnemyPictID            4016
#define kFishPictID             4017
#define kClutterPictID          4018
#define kShadowMaskID           4998    // kShadowPictID + 1000
#define kGliderMaskID           4999    // kGliderPictID + 1000
#define kBlowerMaskID           5000    // kBlowerPictID + 1000
#define kFurnitureMaskID        5001    // kFurniturePictID + 1000
#define kBonusMaskID            5002    // kBonusPictID + 1000
#define kLightMaskID            5004    // kLightPictID + 1000
#define kApplianceMaskID        5005    // kAppliancePictID + 1000
#define kPointsMaskID           5006    // kPointsPictID + 1000
#define kRubberBandsMaskID      5007    // kRubberBandsPictID + 1000
#define kTransportMaskID        5008    // kTransportPictID + 1000
#define kToastMaskID            5009    // kToastPictID + 1000
#define kShreddedMaskID         5010    // kShreddedPictID + 1000
#define kBalloonMaskID          5011    // kBalloonPictID + 1000
#define kCopterMaskID           5012    // kCopterPictID + 1000
#define kDartMaskID             5013    // kDartPictID + 1000
#define kBallMaskID             5014    // kBallPictID + 1000
#define kDripMaskID             5015    // kDripPictID + 1000
#define kEnemyMaskID            5016    // kEnemyPictID + 1000
#define kFishMaskID             5017    // kFishPictID + 1000
#define kClutterMaskID          5018    // kClutterPictID + 1000
#define kCustomPictFallbackID   10000

#define kHitWallWaveID          1000    // kBaseBufferSoundID + kHitWallSound
#define kFadeInWaveID           1001    // kBaseBufferSoundID + kFadeInSound
#define kFadeOutWaveID          1002    // kBaseBufferSoundID + kFadeOutSound
#define kBeepsWaveID            1003    // kBaseBufferSoundID + kBeepsSound
#define kBuzzerWaveID           1004    // kBaseBufferSoundID + kBuzzerSound
#define kDingWaveID             1005    // kBaseBufferSoundID + kDingSound
#define kEnergizeWaveID         1006    // kBaseBufferSoundID + kEnergizeSound
#define kFollowWaveID           1007    // kBaseBufferSoundID + kFollowSound
#define kMicrowavedWaveID       1008    // kBaseBufferSoundID + kMicrowavedSound
#define kSwitchWaveID           1009    // kBaseBufferSoundID + kSwitchSound
#define kBirdWaveID             1010    // kBaseBufferSoundID + kBirdSound
#define kCuckooWaveID           1011    // kBaseBufferSoundID + kCuckooSound
#define kTikWaveID              1012    // kBaseBufferSoundID + kTikSound
#define kTokWaveID              1013    // kBaseBufferSoundID + kTokSound
#define kBlowerOnWaveID         1014    // kBaseBufferSoundID + kBlowerOn
#define kBlowerOffWaveID        1015    // kBaseBufferSoundID + kBlowerOff
#define kCaughtFireWaveID       1016    // kBaseBufferSoundID + kCaughtFireSound
#define kScoreTikWaveID         1017    // kBaseBufferSoundID + kScoreTikSound
#define kThrustWaveID           1018    // kBaseBufferSoundID + kThrustSound
#define kFizzleWaveID           1019    // kBaseBufferSoundID + kFizzleSound
#define kFireBandWaveID         1020    // kBaseBufferSoundID + kFireBandSound
#define kBandReboundWaveID      1021    // kBaseBufferSoundID + kBandReboundSound
#define kGreaseSpillWaveID      1022    // kBaseBufferSoundID + kGreaseSpillSound
#define kChordWaveID            1023    // kBaseBufferSoundID + kChordSound
#define kVCRWaveID              1024    // kBaseBufferSoundID + kVCRSound
#define kFoilHitWaveID          1025    // kBaseBufferSoundID + kFoilHitSound
#define kShredWaveID            1026    // kBaseBufferSoundID + kShredSound
#define kToastLaunchWaveID      1027    // kBaseBufferSoundID + kToastLaunchSound
#define kToastLandWaveID        1028    // kBaseBufferSoundID + kToastLandSound
#define kMacOnWaveID            1029    // kBaseBufferSoundID + kMacOnSound
#define kMacBeepWaveID          1030    // kBaseBufferSoundID + kMacBeepSound
#define kMacOffWaveID           1031    // kBaseBufferSoundID + kMacOffSound
#define kTVOnWaveID             1032    // kBaseBufferSoundID + kTVOnSound
#define kTVOffWaveID            1033    // kBaseBufferSoundID + kTVOffSound
#define kCoffeeWaveID           1034    // kBaseBufferSoundID + kCoffeeSound
#define kMysticWaveID           1035    // kBaseBufferSoundID + kMysticSound
#define kZapWaveID              1036    // kBaseBufferSoundID + kZapSound
#define kPopWaveID              1037    // kBaseBufferSoundID + kPopSound
#define kEnemyInWaveID          1038    // kBaseBufferSoundID + kEnemyInSound
#define kEnemyOutWaveID         1039    // kBaseBufferSoundID + kEnemyOutSound
#define kPaperCrunchWaveID      1040    // kBaseBufferSoundID + kPaperCrunchSound
#define kBounceWaveID           1041    // kBaseBufferSoundID + kBounceSound
#define kDripWaveID             1042    // kBaseBufferSoundID + kDripSound
#define kDropWaveID             1043    // kBaseBufferSoundID + kDropSound
#define kFishOutWaveID          1044    // kBaseBufferSoundID + kFishOutSound
#define kFishInWaveID           1045    // kBaseBufferSoundID + kFishInSound
#define kDontExitWaveID         1046    // kBaseBufferSoundID + kDontExitSound
#define kSizzleWaveID           1047    // kBaseBufferSoundID + kSizzleSound
#define kPaper1WaveID           1048    // kBaseBufferSoundID + kPaper1Sound
#define kPaper2WaveID           1049    // kBaseBufferSoundID + kPaper2Sound
#define kPaper3WaveID           1050    // kBaseBufferSoundID + kPaper3Sound
#define kPaper4WaveID           1051    // kBaseBufferSoundID + kPaper4Sound
#define kTypingWaveID           1052    // kBaseBufferSoundID + kTypingSound
#define kCarriageWaveID         1053    // kBaseBufferSoundID + kCarriageSound
#define kChord2WaveID           1054    // kBaseBufferSoundID + kChord2Sound
#define kPhoneRingWaveID        1055    // kBaseBufferSoundID + kPhoneRingSound
#define kChime1WaveID           1056    // kBaseBufferSoundID + kChime1Sound
#define kChime2WaveID           1057    // kBaseBufferSoundID + kChime2Sound
#define kWebTwangWaveID         1058    // kBaseBufferSoundID + kWebTwangSound
#define kTransOutWaveID         1059    // kBaseBufferSoundID + kTransOutSound
#define kTransInWaveID          1060    // kBaseBufferSoundID + kTransInSound
#define kBonusWaveID            1061    // kBaseBufferSoundID + kBonusSound
#define kHissWaveID             1062    // kBaseBufferSoundID + kHissSound

#define kRefrain1WaveID         2000    // kBaseBufferMusicID + 0
#define kRefrain2WaveID         2001    // kBaseBufferMusicID + 1
#define kRefrain3WaveID         2002    // kBaseBufferMusicID + 2
#define kRefrain4WaveID         2003    // kBaseBufferMusicID + 3
#define kChorusWaveID           2004    // kBaseBufferMusicID + 4
#define kRefrainSparse1WaveID   2005    // kBaseBufferMusicID + 5
#define kRefrainSparse2WaveID   2006    // kBaseBufferMusicID + 6

#endif
