//============================================================================
//----------------------------------------------------------------------------
//							   StructuresInit.c
//----------------------------------------------------------------------------
//============================================================================


//#include <Resources.h>
#include <stddef.h>
#include "Macintosh.h"
#include "Externs.h"
#include "Objects.h"
#include "Play.h"
#include "Player.h"
#include "RectUtils.h"
#include "RubberBands.h"
#include "Scoreboard.h"
#include "Utilities.h"


#define kShadowPictID			3998
#define kBlowerPictID			4000
#define kFurniturePictID		4001
#define kBonusPictID			4002
#define kSwitchPictID			4003
#define kLightPictID			4004
#define kAppliancePictID		4005
#define kPointsPictID			4006
#define kRubberBandsPictID		4007
#define kTransportPictID		4008
#define kToastPictID			4009
#define kShreddedPictID			4010
#define kBalloonPictID			4011
#define kCopterPictID			4012
#define kDartPictID				4013
#define kBallPictID				4014
#define kDripPictID				4015
#define kEnemyPictID			4016
#define kFishPictID				4017

#define kBadgePictID			1996


extern	Rect		glidSrcRect, leftStartGliderSrc, rightStartGliderSrc;
extern	Rect		gliderSrc[], shadowSrcRect, shadowSrc[];
extern	Rect		bandsSrcRect, bandRects[], boardSrcRect, boardDestRect;
extern	Rect		boardTSrcRect, boardTDestRect, badgeSrcRect;
extern	Rect		boardGSrcRect, boardGDestRect, boardPSrcRect, boardPDestRect;
extern	Rect		boardPQDestRect, boardGQDestRect, badgesBlankRects[];
extern	Rect		badgesBadgesRects[], badgesDestRects[];
extern	Rect		nailSrcRect, sparkleSrc[];
extern	Rect		pointsSrc[], breadSrc[];
extern	SInt16		wasScoreboardMode;


//==============================================================  Functions
//--------------------------------------------------------------  InitScoreboardMap
// Any graphics and structures relating to the scoreboard that appears…
// across the top of the game are initialized and loaded up here.

void InitScoreboardMap (void)
{
	return;
#if 0
	Rect		bounds;
	PicHandle	thePicture;
	CGrafPtr	wasCPort;
	GDHandle	wasWorld;
	OSErr		theErr;
	short		hOffset;

	GetGWorld(&wasCPort, &wasWorld);

	wasScoreboardMode = kScoreboardHigh;
	boardSrcRect = houseRect;
	ZeroRectCorner(&boardSrcRect);
	boardSrcRect.bottom = kScoreboardTall;
	theErr = CreateOffScreenGWorld(&boardSrcMap, &boardSrcRect, kPreferredDepth);
	SetGWorld(boardSrcMap, nil);

	if (boardSrcRect.right >= 640)
		hOffset = (RectWide(&boardSrcRect) - kMaxViewWidth) / 2;
	else
		hOffset = -576;
	thePicture = GetPicture(kScoreboardPictID);
	if (thePicture == nil)
		RedAlert(kErrFailedGraphicLoad);
	HLock((Handle)thePicture);
	bounds = (*thePicture)->picFrame;
	HUnlock((Handle)thePicture);
	QOffsetRect(&bounds, -bounds.left, -bounds.top);
	QOffsetRect(&bounds, hOffset, 0);
	DrawPicture(thePicture, &bounds);
	ReleaseResource((Handle)thePicture);

	QSetRect(&badgeSrcRect, 0, 0, 32, 66);				// 2144 pixels
	theErr = CreateOffScreenGWorld(&badgeSrcMap, &badgeSrcRect, kPreferredDepth);
	SetGWorld(badgeSrcMap, nil);
	LoadGraphic(kBadgePictID);

	boardDestRect = boardSrcRect;
	QOffsetRect(&boardDestRect, 0, -kScoreboardTall);

	hOffset = (RectWide(&houseRect) - 640) / 2;
	if (hOffset < 0)
		hOffset = -128;

	QSetRect(&boardTSrcRect, 0, 0, 256, 12);			// room title
	theErr = CreateOffScreenGWorld(&boardTSrcMap, &boardTSrcRect, kPreferredDepth);
	SetGWorld(boardTSrcMap, nil);
	boardTDestRect = boardTSrcRect;
	QOffsetRect(&boardTDestRect, 137 + hOffset, 5);
	TextFont(applFont);
	TextSize(12);
	TextFace(bold);

	QSetRect(&boardGSrcRect, 0, 0, 20, 10);				// # gliders
	theErr = CreateOffScreenGWorld(&boardGSrcMap, &boardGSrcRect, kPreferredDepth);
	SetGWorld(boardGSrcMap, nil);
	boardGDestRect = boardGSrcRect;
	QOffsetRect(&boardGDestRect, 526 + hOffset, 5);
	TextFont(applFont);
	TextSize(12);
	TextFace(bold);

	QSetRect(&boardPSrcRect, 0, 0, 64, 10);				// points
	theErr = CreateOffScreenGWorld(&boardPSrcMap, &boardPSrcRect, kPreferredDepth);
	SetGWorld(boardPSrcMap, nil);
	boardPDestRect = boardPSrcRect;
	QOffsetRect(&boardPDestRect, 570 + hOffset, 5);		// total = 6396 pixels
	boardPQDestRect = boardPDestRect;
	QOffsetRect(&boardPQDestRect, 0, -kScoreboardTall);
	boardGQDestRect = boardGDestRect;
	QOffsetRect(&boardGQDestRect, 0, -kScoreboardTall);
	TextFont(applFont);
	TextSize(12);
	TextFace(bold);

	QSetRect(&badgesBlankRects[0], 0, 0, 16, 16);		// foil
	QOffsetRect(&badgesBlankRects[0], 0, 0);
	QSetRect(&badgesBlankRects[1], 0, 0, 16, 16);		// rubber bands
	QOffsetRect(&badgesBlankRects[1], 0, 16);
	QSetRect(&badgesBlankRects[2], 0, 0, 16, 17);		// battery
	QOffsetRect(&badgesBlankRects[2], 0, 32);
	QSetRect(&badgesBlankRects[3], 0, 0, 16, 17);		// helium
	QOffsetRect(&badgesBlankRects[3], 0, 49);

	QSetRect(&badgesBadgesRects[0], 0, 0, 16, 16);		// foil
	QOffsetRect(&badgesBadgesRects[0], 16, 0);
	QSetRect(&badgesBadgesRects[1], 0, 0, 16, 16);		// rubber bands
	QOffsetRect(&badgesBadgesRects[1], 16, 16);
	QSetRect(&badgesBadgesRects[2], 0, 0, 16, 17);		// battery
	QOffsetRect(&badgesBadgesRects[2], 16, 32);
	QSetRect(&badgesBadgesRects[3], 0, 0, 16, 17);		// helium
	QOffsetRect(&badgesBadgesRects[3], 16, 49);

	QSetRect(&badgesDestRects[0], 0, 0, 16, 16);		// foil
	QOffsetRect(&badgesDestRects[0], 432 + hOffset, 2 - kScoreboardTall);
	QSetRect(&badgesDestRects[1], 0, 0, 16, 16);		// rubber bands
	QOffsetRect(&badgesDestRects[1], 449 + hOffset, 2 - kScoreboardTall);
	QSetRect(&badgesDestRects[2], 0, 0, 16, 17);		// battery
	QOffsetRect(&badgesDestRects[2], 467 + hOffset, 1 - kScoreboardTall);
	QSetRect(&badgesDestRects[3], 0, 0, 16, 17);		// helium
	QOffsetRect(&badgesDestRects[3], 467 + hOffset, 1 - kScoreboardTall);

	SetGWorld(wasCPort, wasWorld);
#endif
}

//--------------------------------------------------------------  InitGliderMap
// Graphics and structures relating to the little paper glider (the…
// player) are cretaed, loaded up and initialized here.

void InitGliderMap (void)
{
	return;
#if 0
	CGrafPtr	wasCPort;
	GDHandle	wasWorld;
	OSErr		theErr;
	short		i;

	GetGWorld(&wasCPort, &wasWorld);

	QSetRect(&glidSrcRect, 0, 0, kGliderWide, 668);	// 32112 pixels
	theErr = CreateOffScreenGWorld(&glidSrcMap, &glidSrcRect, kPreferredDepth);
	SetGWorld(glidSrcMap, nil);
	LoadGraphic(kGliderPictID);

	theErr = CreateOffScreenGWorld(&glid2SrcMap, &glidSrcRect, kPreferredDepth);
	SetGWorld(glid2SrcMap, nil);
	LoadGraphic(kGlider2PictID);

	theErr = CreateOffScreenGWorld(&glidMaskMap, &glidSrcRect, 1);
	SetGWorld(glidMaskMap, nil);
	LoadGraphic(kGliderPictID + 1000);

	for (i = 0; i <= 20; i++)
	{
		QSetRect(&gliderSrc[i], 0, 0, kGliderWide, kGliderHigh);
		QOffsetRect(&gliderSrc[i], 0, kGliderHigh * i);
	}
	for (i = 21; i <= 28; i++)
	{
		QSetRect(&gliderSrc[i], 0, 0, kGliderWide, kGliderBurningHigh);
		QOffsetRect(&gliderSrc[i], 0, 420 + (kGliderBurningHigh * (i - 21)));
	}

	QSetRect(&gliderSrc[29], 0, 0, kGliderWide, kGliderHigh);
	QOffsetRect(&gliderSrc[29], 0, 628);
	QSetRect(&gliderSrc[30], 0, 0, kGliderWide, kGliderHigh);
	QOffsetRect(&gliderSrc[30], 0, 648);

	QSetRect(&shadowSrcRect, 0, 0, kGliderWide, kShadowHigh * kNumShadowSrcRects);
	theErr = CreateOffScreenGWorld(&shadowSrcMap, &shadowSrcRect, kPreferredDepth);
	SetGWorld(shadowSrcMap, nil);
	LoadGraphic(kShadowPictID);

	theErr = CreateOffScreenGWorld(&shadowMaskMap, &shadowSrcRect, 1);
	SetGWorld(shadowMaskMap, nil);
	LoadGraphic(kShadowPictID + 1000);

	for (i = 0; i < kNumShadowSrcRects; i++)
	{
		QSetRect(&shadowSrc[i], 0, 0, kGliderWide, kShadowHigh);
		QOffsetRect(&shadowSrc[i], 0, kShadowHigh * i);
	}

	QSetRect(&bandsSrcRect, 0, 0, 16, 18);		// 304 pixels
	theErr = CreateOffScreenGWorld(&bandsSrcMap, &bandsSrcRect, kPreferredDepth);
	SetGWorld(bandsSrcMap, nil);
	LoadGraphic(kRubberBandsPictID);

	theErr = CreateOffScreenGWorld(&bandsMaskMap, &bandsSrcRect, 1);
	SetGWorld(bandsMaskMap, nil);
	LoadGraphic(kRubberBandsPictID + 1000);

	for (i = 0; i < 3; i++)
	{
		QSetRect(&bandRects[i], 0, 0, 16, 6);
		QOffsetRect(&bandRects[i], 0, 6 * i);
	}

	SetGWorld(wasCPort, wasWorld);
#endif
}

//--------------------------------------------------------------  InitBlowers
// All blower graphics and structures are loaded up and initialized here.
// Blowers include vents, ducts, candles, fans, etc.

void InitBlowers (void)
{
	size_t		i;
	OSErr		theErr;

	QSetRect(&blowerSrcRect, 0, 0, 48, 402);	// 19344 pixels
	theErr = CreateOffScreenGWorld(&blowerSrcMap, &blowerSrcRect, kPreferredDepth);
	LoadGraphic(blowerSrcMap, kBlowerPictID);

	theErr = CreateOffScreenGWorld(&blowerMaskMap, &blowerSrcRect, 1);
	LoadGraphic(blowerMaskMap, kBlowerPictID + 1000);

	for (i = 0; i < kNumCandleFlames; i++)
	{
		QSetRect(&flame[i], 0, 0, 16, 15);
		QOffsetRect(&flame[i], 32, 179 + (i * 15));
	}

	for (i = 0; i < kNumTikiFlames; i++)
	{
		QSetRect(&tikiFlame[i], 0, 0, 8, 10);
		QOffsetRect(&tikiFlame[i], 40, 69 + (i * 10));
	}

	for (i = 0; i < kNumBBQCoals; i++)
	{
		QSetRect(&coals[i], 0, 0, 32, 9);
		QOffsetRect(&coals[i], 0, 304 + (i * 9));
	}

	QSetRect(&leftStartGliderSrc, 0, 0, 48, 16);
	QOffsetRect(&leftStartGliderSrc, 0, 358);

	QSetRect(&rightStartGliderSrc, 0, 0, 48, 16);
	QOffsetRect(&rightStartGliderSrc, 0, 374);
}

//--------------------------------------------------------------  InitFurniture
// Structures and graphics relating to the furniture is loaded up.
// Furniture includes tables, cabinets, shelves, etc.

void InitFurniture (void)
{
	OSErr		theErr;

	QSetRect(&furnitureSrcRect, 0, 0, 64, 278);		// 17856 pixels
	theErr = CreateOffScreenGWorld(&furnitureSrcMap, &furnitureSrcRect, kPreferredDepth);
	LoadGraphic(furnitureSrcMap, kFurniturePictID);

	theErr = CreateOffScreenGWorld(&furnitureMaskMap, &furnitureSrcRect, 1);
	LoadGraphic(furnitureMaskMap, kFurniturePictID + 1000);

	QSetRect(&tableSrc, 0, 0, 64, 22);
	QOffsetRect(&tableSrc, 0, 0);

	QSetRect(&shelfSrc, 0, 0, 16, 21);
	QOffsetRect(&shelfSrc, 0, 22);

	QSetRect(&hingeSrc, 0, 0, 4, 16);
	QOffsetRect(&hingeSrc, 16, 22);

	QSetRect(&handleSrc, 0, 0, 4, 21);
	QOffsetRect(&handleSrc, 20, 22);

	QSetRect(&knobSrc, 0, 0, 8, 8);
	QOffsetRect(&knobSrc, 24, 22);

	QSetRect(&leftFootSrc, 0, 0, 16, 16);
	QOffsetRect(&leftFootSrc, 32, 22);

	QSetRect(&rightFootSrc, 0, 0, 16, 16);
	QOffsetRect(&rightFootSrc, 48, 22);

	QSetRect(&deckSrc, 0, 0, 64, 21);
	QOffsetRect(&deckSrc, 0, 162);
}

//--------------------------------------------------------------  InitPrizes
// Structures and graphics relating to the prizes (bonuses) are loaded up.
// Prizes includes clocks, rubber bands, extra gliders, etc.

void InitPrizes (void)
{
	size_t		i;
	OSErr		theErr;

	QSetRect(&bonusSrcRect, 0, 0, 88, 378);		// 33264 pixels
	theErr = CreateOffScreenGWorld(&bonusSrcMap, &bonusSrcRect, kPreferredDepth);
	LoadGraphic(bonusSrcMap, kBonusPictID);

	theErr = CreateOffScreenGWorld(&bonusMaskMap, &bonusSrcRect, 1);
	LoadGraphic(bonusMaskMap, kBonusPictID + 1000);

	for (i = 0; i < 11; i++)
	{
		QSetRect(&digits[i], 0, 0, 4, 6);
		QOffsetRect(&digits[i], 28, i * 6);
	}

	for (i = 0; i < 3; i++)
	{
		QSetRect(&pendulumSrc[i], 0, 0, 32, 28);
		QOffsetRect(&pendulumSrc[i], 56, 186 + (i * 28));
	}

	QSetRect(&greaseSrcRt[0], 0, 0, 32, 27);
	QOffsetRect(&greaseSrcRt[0], 0, 243);
	QSetRect(&greaseSrcRt[1], 0, 0, 32, 27);
	QOffsetRect(&greaseSrcRt[1], 0, 270);
	QSetRect(&greaseSrcRt[2], 0, 0, 32, 27);
	QOffsetRect(&greaseSrcRt[2], 0, 297);
	QSetRect(&greaseSrcRt[3], 0, 0, 32, 27);
	QOffsetRect(&greaseSrcRt[3], 32, 297);

	QSetRect(&greaseSrcLf[0], 0, 0, 32, 27);
	QOffsetRect(&greaseSrcLf[0], 0, 324);
	QSetRect(&greaseSrcLf[1], 0, 0, 32, 27);
	QOffsetRect(&greaseSrcLf[1], 32, 324);
	QSetRect(&greaseSrcLf[2], 0, 0, 32, 27);
	QOffsetRect(&greaseSrcLf[2], 0, 351);
	QSetRect(&greaseSrcLf[3], 0, 0, 32, 27);
	QOffsetRect(&greaseSrcLf[3], 32, 351);

	for (i = 0; i < 6; i++)
	{
		QSetRect(&starSrc[i], 0, 0, 32, 31);
		QOffsetRect(&starSrc[i], 48, i * 31);
	}

	for (i = 0; i < 3; i++)
	{
		QSetRect(&sparkleSrc[i + 2], 0, 0, 20, 19);
		QOffsetRect(&sparkleSrc[i + 2], 0, 70 + (i * 19));
	}
	sparkleSrc[0] = sparkleSrc[4];
	sparkleSrc[1] = sparkleSrc[3];

	QSetRect(&pointsSrcRect, 0, 0, 24, 120);		// 2880 pixels
	theErr = CreateOffScreenGWorld(&pointsSrcMap, &pointsSrcRect, kPreferredDepth);
	LoadGraphic(pointsSrcMap, kPointsPictID);

	theErr = CreateOffScreenGWorld(&pointsMaskMap, &pointsSrcRect, 1);
	LoadGraphic(pointsMaskMap, kPointsPictID + 1000);

	for (i = 0; i < 15; i++)
	{
		QSetRect(&pointsSrc[i], 0, 0, 24, 8);
		QOffsetRect(&pointsSrc[i], 0, i * 8);
	}
}

//--------------------------------------------------------------  InitTransports
// Structures and graphics relating to the transports is loaded up.
// Transports includes transport ducts, mailboxes, etc.

void InitTransports (void)
{
	OSErr		theErr;

	QSetRect(&transSrcRect, 0, 0, 56, 32);	// 1848 pixels
	theErr = CreateOffScreenGWorld(&transSrcMap, &transSrcRect, kPreferredDepth);
	LoadGraphic(transSrcMap, kTransportPictID);

	theErr = CreateOffScreenGWorld(&transMaskMap, &transSrcRect, 1);
	LoadGraphic(transMaskMap, kTransportPictID + 1000);
}

//--------------------------------------------------------------  InitSwitches
// Structures and graphics relating to switches are loaded up.
// Switches includes triggers, light switches, etc.

void InitSwitches (void)
{
	OSErr		theErr;

	QSetRect(&switchSrcRect, 0, 0, 32, 104);	// 3360 pixels
	theErr = CreateOffScreenGWorld(&switchSrcMap, &switchSrcRect, kPreferredDepth);
	LoadGraphic(switchSrcMap, kSwitchPictID);

	QSetRect(&lightSwitchSrc[0], 0, 0, 15, 24);
	QOffsetRect(&lightSwitchSrc[0], 0, 0);
	QSetRect(&lightSwitchSrc[1], 0, 0, 15, 24);
	QOffsetRect(&lightSwitchSrc[1], 16, 0);

	QSetRect(&machineSwitchSrc[0], 0, 0, 16, 24);
	QOffsetRect(&machineSwitchSrc[0], 0, 24);
	QSetRect(&machineSwitchSrc[1], 0, 0, 16, 24);
	QOffsetRect(&machineSwitchSrc[1], 16, 24);

	QSetRect(&thermostatSrc[0], 0, 0, 15, 24);
	QOffsetRect(&thermostatSrc[0], 0, 48);
	QSetRect(&thermostatSrc[1], 0, 0, 15, 24);
	QOffsetRect(&thermostatSrc[1], 16, 48);

	QSetRect(&powerSrc[0], 0, 0, 8, 8);
	QOffsetRect(&powerSrc[0], 0, 72);
	QSetRect(&powerSrc[1], 0, 0, 8, 8);
	QOffsetRect(&powerSrc[1], 8, 72);

	QSetRect(&knifeSwitchSrc[0], 0, 0, 16, 24);
	QOffsetRect(&knifeSwitchSrc[0], 0, 80);
	QSetRect(&knifeSwitchSrc[1], 0, 0, 16, 24);
	QOffsetRect(&knifeSwitchSrc[1], 16, 80);
}

//--------------------------------------------------------------  InitLights
// Structures and graphics relating to lights are loaded up.
// Lights includes table lamps, flourescent lights, track lights, etc.

void InitLights (void)
{
	size_t		i;
	OSErr		theErr;

	QSetRect(&lightSrcRect, 0, 0, 72, 126);		// 9144 pixels
	theErr = CreateOffScreenGWorld(&lightSrcMap, &lightSrcRect, kPreferredDepth);
	LoadGraphic(lightSrcMap, kLightPictID);

	theErr = CreateOffScreenGWorld(&lightMaskMap, &lightSrcRect, 1);
	LoadGraphic(lightMaskMap, kLightPictID + 1000);

	QSetRect(&flourescentSrc1, 0, 0, 16, 12);
	QOffsetRect(&flourescentSrc1, 0, 78);

	QSetRect(&flourescentSrc2, 0, 0, 16, 12);
	QOffsetRect(&flourescentSrc2, 0, 90);

	for (i = 0; i < kNumTrackLights; i++)
	{
		QSetRect(&trackLightSrc[i], 0, 0, 24, 24);
		QOffsetRect(&trackLightSrc[i], 24 * i, 102);
	}
}

//--------------------------------------------------------------  InitAppliances
// Structures and graphics relating to appliances are loaded up.
// Appliances includes toasters, T.V.s, etc.

void InitAppliances (void)
{
	size_t		i;
	OSErr		theErr;

	QSetRect(&applianceSrcRect, 0, 0, 80, 269);		// 21600 pixels
	theErr = CreateOffScreenGWorld(&applianceSrcMap, &applianceSrcRect, kPreferredDepth);
	LoadGraphic(applianceSrcMap, kAppliancePictID);

	theErr = CreateOffScreenGWorld(&applianceMaskMap, &applianceSrcRect, 1);
	LoadGraphic(applianceMaskMap, kAppliancePictID + 1000);

	QSetRect(&toastSrcRect, 0, 0, 32, 174);			// 5600 pixels
	theErr = CreateOffScreenGWorld(&toastSrcMap, &toastSrcRect, kPreferredDepth);
	LoadGraphic(toastSrcMap, kToastPictID);

	theErr = CreateOffScreenGWorld(&toastMaskMap, &toastSrcRect, 1);
	LoadGraphic(toastMaskMap, kToastPictID + 1000);

	QSetRect(&shredSrcRect, 0, 0, 40, 35);			// 1440 pixels
	theErr = CreateOffScreenGWorld(&shredSrcMap, &shredSrcRect, kPreferredDepth);
	LoadGraphic(shredSrcMap, kShreddedPictID);

	theErr = CreateOffScreenGWorld(&shredMaskMap, &shredSrcRect, 1);
	LoadGraphic(shredMaskMap, kShreddedPictID + 1000);

	QSetRect(&plusScreen1, 0, 0, 32, 22);
	QOffsetRect(&plusScreen1, 48, 127);
	QSetRect(&plusScreen2, 0, 0, 32, 22);
	QOffsetRect(&plusScreen2, 48, 149);

	QSetRect(&tvScreen1, 0, 0, 64, 49);
	QOffsetRect(&tvScreen1, 0, 171);
	QSetRect(&tvScreen2, 0, 0, 64, 49);
	QOffsetRect(&tvScreen2, 0, 220);

	QSetRect(&coffeeLight1, 0, 0, 8, 4);
	QOffsetRect(&coffeeLight1, 72, 171);
	QSetRect(&coffeeLight2, 0, 0, 8, 4);
	QOffsetRect(&coffeeLight2, 72, 175);

	for (i = 0; i < kNumOutletPicts; i++)
	{
		QSetRect(&outletSrc[i], 0, 0, 16, 24);
		QOffsetRect(&outletSrc[i], 64, 22 + (i * 24));
	}

	for (i = 0; i < kNumBreadPicts; i++)
	{
		QSetRect(&breadSrc[i], 0, 0, 32, 29);
		QOffsetRect(&breadSrc[i], 0, i * 29);
	}

	QSetRect(&vcrTime1, 0, 0, 16, 4);
	QOffsetRect(&vcrTime1, 64, 179);
	QSetRect(&vcrTime2, 0, 0, 16, 4);
	QOffsetRect(&vcrTime2, 64, 183);

	QSetRect(&stereoLight1, 0, 0, 4, 1);
	QOffsetRect(&stereoLight1, 68, 171);
	QSetRect(&stereoLight2, 0, 0, 4, 1);
	QOffsetRect(&stereoLight2, 68, 172);

	QSetRect(&microOn, 0, 0, 16, 35);
	QOffsetRect(&microOn, 64, 222);
	QSetRect(&microOff, 0, 0, 16, 35);
	QOffsetRect(&microOff, 64, 187);
}

//--------------------------------------------------------------  InitEnemies
// Structures and graphics relating to enemies are loaded up.
// Enemies includes darts, balloons, fish, etc.

void InitEnemies (void)
{
	size_t		i;
	OSErr		theErr;

	QSetRect(&balloonSrcRect, 0, 0, 24, 30 * kNumBalloonFrames);
	theErr = CreateOffScreenGWorld(&balloonSrcMap, &balloonSrcRect, kPreferredDepth);
	LoadGraphic(balloonSrcMap, kBalloonPictID);

	theErr = CreateOffScreenGWorld(&balloonMaskMap, &balloonSrcRect, 1);
	LoadGraphic(balloonMaskMap, kBalloonPictID + 1000);

	QSetRect(&copterSrcRect, 0, 0, 32, 30 * kNumCopterFrames);
	theErr = CreateOffScreenGWorld(&copterSrcMap, &copterSrcRect, kPreferredDepth);
	LoadGraphic(copterSrcMap, kCopterPictID);

	theErr = CreateOffScreenGWorld(&copterMaskMap, &copterSrcRect, 1);
	LoadGraphic(copterMaskMap, kCopterPictID + 1000);

	QSetRect(&dartSrcRect, 0, 0, 64, 19 * kNumDartFrames);
	theErr = CreateOffScreenGWorld(&dartSrcMap, &dartSrcRect, kPreferredDepth);
	LoadGraphic(dartSrcMap, kDartPictID);

	theErr = CreateOffScreenGWorld(&dartMaskMap, &dartSrcRect, 1);
	LoadGraphic(dartMaskMap, kDartPictID + 1000);

	QSetRect(&ballSrcRect, 0, 0, 32, 32 * kNumBallFrames);
	theErr = CreateOffScreenGWorld(&ballSrcMap, &ballSrcRect, kPreferredDepth);
	LoadGraphic(ballSrcMap, kBallPictID);

	theErr = CreateOffScreenGWorld(&ballMaskMap, &ballSrcRect, 1);
	LoadGraphic(ballMaskMap, kBallPictID + 1000);

	QSetRect(&dripSrcRect, 0, 0, 16, 12 * kNumDripFrames);
	theErr = CreateOffScreenGWorld(&dripSrcMap, &dripSrcRect, kPreferredDepth);
	LoadGraphic(dripSrcMap, kDripPictID);

	theErr = CreateOffScreenGWorld(&dripMaskMap, &dripSrcRect, 1);
	LoadGraphic(dripMaskMap, kDripPictID + 1000);

	QSetRect(&enemySrcRect, 0, 0, 36, 33);
	theErr = CreateOffScreenGWorld(&enemySrcMap, &enemySrcRect, kPreferredDepth);
	LoadGraphic(enemySrcMap, kEnemyPictID);

	theErr = CreateOffScreenGWorld(&enemyMaskMap, &enemySrcRect, 1);
	LoadGraphic(enemyMaskMap, kEnemyPictID + 1000);

	QSetRect(&fishSrcRect, 0, 0, 16, 16 * kNumFishFrames);
	theErr = CreateOffScreenGWorld(&fishSrcMap, &fishSrcRect, kPreferredDepth);
	LoadGraphic(fishSrcMap, kFishPictID);

	theErr = CreateOffScreenGWorld(&fishMaskMap, &fishSrcRect, 1);
	LoadGraphic(fishMaskMap, kFishPictID + 1000);

	for (i = 0; i < kNumBalloonFrames; i++)
	{
		QSetRect(&balloonSrc[i], 0, 0, 24, 30);
		QOffsetRect(&balloonSrc[i], 0, 30 * i);
	}

	for (i = 0; i < kNumCopterFrames; i++)
	{
		QSetRect(&copterSrc[i], 0, 0, 32, 30);
		QOffsetRect(&copterSrc[i], 0, 30 * i);
	}

	for (i = 0; i < kNumDartFrames; i++)
	{
		QSetRect(&dartSrc[i], 0, 0, 64, 19);
		QOffsetRect(&dartSrc[i], 0, 19 * i);
	}

	for (i = 0; i < kNumBallFrames; i++)
	{
		QSetRect(&ballSrc[i], 0, 0, 32, 32);
		QOffsetRect(&ballSrc[i], 0, 32 * i);
	}

	for (i = 0; i < kNumDripFrames; i++)
	{
		QSetRect(&dripSrc[i], 0, 0, 16, 12);
		QOffsetRect(&dripSrc[i], 0, 12 * i);
	}

	for (i = 0; i < kNumFishFrames; i++)
	{
		QSetRect(&fishSrc[i], 0, 0, 16, 16);
		QOffsetRect(&fishSrc[i], 0, 16 * i);
	}
}

