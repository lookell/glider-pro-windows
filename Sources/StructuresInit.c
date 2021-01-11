//============================================================================
//----------------------------------------------------------------------------
//                              StructuresInit.c
//----------------------------------------------------------------------------
//============================================================================

#include "StructuresInit.h"

#include "ByteIO.h"
#include "DynamicMaps.h"
#include "Dynamics.h"
#include "GameOver.h"
#include "GliderDefines.h"
#include "Input.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "ObjectEdit.h"
#include "Objects.h"
#include "Play.h"
#include "Player.h"
#include "RectUtils.h"
#include "ResourceIDs.h"
#include "ResourceLoader.h"
#include "Room.h"
#include "RoomGraphics.h"
#include "RoomInfo.h"
#include "RubberBands.h"
#include "Scoreboard.h"
#include "SelectHouse.h"
#include "StructIO.h"
#include "Utilities.h"
#include "WinAPI.h"

#include <stdlib.h>

void InitScoreboardMap (void);
void InitGliderMap (void);
void InitBlowers (void);
void InitFurniture (void);
void InitPrizes (void);
void InitTransports (void);
void InitSwitches (void);
void InitLights (void);
void InitAppliances (void);
void InitEnemies (void);
void InitClutter (void);
void InitSupport (void);
void InitAngel (void);
void InitSplashScreenMap (void);
Boolean GetDemoDataPointer (void **ppDemoData, size_t *pDemoSize);

//==============================================================  Functions
//--------------------------------------------------------------  InitScoreboardMap
// Any graphics and structures relating to the scoreboard that appears
// across the top of the game are initialized and loaded up here.

void InitScoreboardMap (void)
{
	Rect		bounds;
	HBITMAP		thePicture;
	BITMAP		bmInfo;
	HFONT		scoreboardFont;
	SInt16		hOffset;

	g_wasScoreboardMode = kScoreboardHigh;
	g_boardSrcRect = g_houseRect;
	ZeroRectCorner(&g_boardSrcRect);
	g_boardSrcRect.bottom = kScoreboardTall;
	g_boardSrcMap = CreateOffScreenGWorld(&g_boardSrcRect, kPreferredDepth);

	if (g_boardSrcRect.right >= 640)
		hOffset = (RectWide(&g_boardSrcRect) - kMaxViewWidth) / 2;
	else
		hOffset = -576;
	thePicture = Gp_LoadBuiltInImage(kScoreboardPictID);
	if (thePicture == NULL)
		RedAlert(kErrFailedGraphicLoad);
	GetObject(thePicture, sizeof(bmInfo), &bmInfo);
	QSetRect(&bounds, 0, 0, (SInt16)bmInfo.bmWidth, (SInt16)bmInfo.bmHeight);
	QOffsetRect(&bounds, hOffset, 0);
	Mac_DrawPicture(g_boardSrcMap, thePicture, &bounds);
	DeleteObject(thePicture);

	QSetRect(&g_badgeSrcRect, 0, 0, 32, 66);				// 2144 pixels
	g_badgeSrcMap = CreateOffScreenGWorld(&g_badgeSrcRect, kPreferredDepth);
	LoadGraphic(g_badgeSrcMap, GP_BUILTIN_ASSETS, kBadgePictID);

	g_boardDestRect = g_boardSrcRect;
	QOffsetRect(&g_boardDestRect, 0, -kScoreboardTall);

	hOffset = (RectWide(&g_houseRect) - 640) / 2;
	if (hOffset < 0)
		hOffset = -128;

	QSetRect(&g_boardTSrcRect, 0, 0, 256, 15);			// room title
	g_boardTSrcMap = CreateOffScreenGWorld(&g_boardTSrcRect, kPreferredDepth);
	g_boardTDestRect = g_boardTSrcRect;
	QOffsetRect(&g_boardTDestRect, 137 + hOffset, 2);
	scoreboardFont = CreateTahomaFont(14, FW_BOLD);
	SelectObject(g_boardTSrcMap, scoreboardFont);

	QSetRect(&g_boardGSrcRect, 0, 0, 20, 15);				// # gliders
	g_boardGSrcMap = CreateOffScreenGWorld(&g_boardGSrcRect, kPreferredDepth);
	g_boardGDestRect = g_boardGSrcRect;
	QOffsetRect(&g_boardGDestRect, 526 + hOffset, 2);
	g_boardGQDestRect = g_boardGDestRect;
	QOffsetRect(&g_boardGQDestRect, 0, -kScoreboardTall);
	scoreboardFont = CreateTahomaFont(14, FW_BOLD);
	SelectObject(g_boardGSrcMap, scoreboardFont);

	QSetRect(&g_boardPSrcRect, 0, 0, 64, 15);				// points
	g_boardPSrcMap = CreateOffScreenGWorld(&g_boardPSrcRect, kPreferredDepth);
	g_boardPDestRect = g_boardPSrcRect;
	QOffsetRect(&g_boardPDestRect, 570 + hOffset, 2);		// total = 6396 pixels
	g_boardPQDestRect = g_boardPDestRect;
	QOffsetRect(&g_boardPQDestRect, 0, -kScoreboardTall);
	scoreboardFont = CreateTahomaFont(14, FW_BOLD);
	SelectObject(g_boardPSrcMap, scoreboardFont);

	QSetRect(&g_badgesBlankRects[kFoilBadge], 0, 0, 16, 16);
	QOffsetRect(&g_badgesBlankRects[kFoilBadge], 0, 0);
	QSetRect(&g_badgesBlankRects[kBandsBadge], 0, 0, 16, 16);
	QOffsetRect(&g_badgesBlankRects[kBandsBadge], 0, 16);
	QSetRect(&g_badgesBlankRects[kBatteryBadge], 0, 0, 16, 17);
	QOffsetRect(&g_badgesBlankRects[kBatteryBadge], 0, 32);
	QSetRect(&g_badgesBlankRects[kHeliumBadge], 0, 0, 16, 17);
	QOffsetRect(&g_badgesBlankRects[kHeliumBadge], 0, 49);

	QSetRect(&g_badgesBadgesRects[kFoilBadge], 0, 0, 16, 16);
	QOffsetRect(&g_badgesBadgesRects[kFoilBadge], 16, 0);
	QSetRect(&g_badgesBadgesRects[kBandsBadge], 0, 0, 16, 16);
	QOffsetRect(&g_badgesBadgesRects[kBandsBadge], 16, 16);
	QSetRect(&g_badgesBadgesRects[kBatteryBadge], 0, 0, 16, 17);
	QOffsetRect(&g_badgesBadgesRects[kBatteryBadge], 16, 32);
	QSetRect(&g_badgesBadgesRects[kHeliumBadge], 0, 0, 16, 17);
	QOffsetRect(&g_badgesBadgesRects[kHeliumBadge], 16, 49);

	QSetRect(&g_badgesDestRects[kFoilBadge], 0, 0, 16, 16);
	QOffsetRect(&g_badgesDestRects[kFoilBadge], 432 + hOffset, 2 - kScoreboardTall);
	QSetRect(&g_badgesDestRects[kBandsBadge], 0, 0, 16, 16);
	QOffsetRect(&g_badgesDestRects[kBandsBadge], 449 + hOffset, 2 - kScoreboardTall);
	QSetRect(&g_badgesDestRects[kBatteryBadge], 0, 0, 16, 17);
	QOffsetRect(&g_badgesDestRects[kBatteryBadge], 467 + hOffset, 1 - kScoreboardTall);
	QSetRect(&g_badgesDestRects[kHeliumBadge], 0, 0, 16, 17);
	QOffsetRect(&g_badgesDestRects[kHeliumBadge], 467 + hOffset, 1 - kScoreboardTall);
}

//--------------------------------------------------------------  InitGliderMap
// Graphics and structures relating to the little paper glider (the
// player) are cretaed, loaded up and initialized here.

void InitGliderMap (void)
{
	SInt16		i;

	QSetRect(&g_glidSrcRect, 0, 0, kGliderWide, 668);	// 32112 pixels
	g_glidSrcMap = CreateOffScreenGWorld(&g_glidSrcRect, kPreferredDepth);
	LoadGraphic(g_glidSrcMap, GP_BUILTIN_ASSETS, kGliderPictID);

	g_glid2SrcMap = CreateOffScreenGWorld(&g_glidSrcRect, kPreferredDepth);
	LoadGraphic(g_glid2SrcMap, GP_BUILTIN_ASSETS, kGlider2PictID);

	g_glidMaskMap = CreateOffScreenGWorld(&g_glidSrcRect, 1);
	LoadGraphic(g_glidMaskMap, GP_BUILTIN_ASSETS, kGliderMaskID);

	for (i = 0; i <= 20; i++)
	{
		QSetRect(&g_gliderSrc[i], 0, 0, kGliderWide, kGliderHigh);
		QOffsetRect(&g_gliderSrc[i], 0, kGliderHigh * i);
	}
	for (i = 21; i <= 28; i++)
	{
		QSetRect(&g_gliderSrc[i], 0, 0, kGliderWide, kGliderBurningHigh);
		QOffsetRect(&g_gliderSrc[i], 0, 420 + (kGliderBurningHigh * (i - 21)));
	}

	QSetRect(&g_gliderSrc[29], 0, 0, kGliderWide, kGliderHigh);
	QOffsetRect(&g_gliderSrc[29], 0, 628);
	QSetRect(&g_gliderSrc[30], 0, 0, kGliderWide, kGliderHigh);
	QOffsetRect(&g_gliderSrc[30], 0, 648);

	QSetRect(&g_shadowSrcRect, 0, 0, kGliderWide, kShadowHigh * kNumShadowSrcRects);
	g_shadowSrcMap = CreateOffScreenGWorld(&g_shadowSrcRect, kPreferredDepth);
	LoadGraphic(g_shadowSrcMap, GP_BUILTIN_ASSETS, kShadowPictID);

	g_shadowMaskMap = CreateOffScreenGWorld(&g_shadowSrcRect, 1);
	LoadGraphic(g_shadowMaskMap, GP_BUILTIN_ASSETS, kShadowMaskID);

	for (i = 0; i < kNumShadowSrcRects; i++)
	{
		QSetRect(&g_shadowSrc[i], 0, 0, kGliderWide, kShadowHigh);
		QOffsetRect(&g_shadowSrc[i], 0, kShadowHigh * i);
	}

	QSetRect(&g_bandsSrcRect, 0, 0, 16, 18);		// 304 pixels
	g_bandsSrcMap = CreateOffScreenGWorld(&g_bandsSrcRect, kPreferredDepth);
	LoadGraphic(g_bandsSrcMap, GP_BUILTIN_ASSETS, kRubberBandsPictID);

	g_bandsMaskMap = CreateOffScreenGWorld(&g_bandsSrcRect, 1);
	LoadGraphic(g_bandsMaskMap, GP_BUILTIN_ASSETS, kRubberBandsMaskID);

	for (i = 0; i < 3; i++)
	{
		QSetRect(&g_bandRects[i], 0, 0, 16, 6);
		QOffsetRect(&g_bandRects[i], 0, 6 * i);
	}
}

//--------------------------------------------------------------  InitBlowers
// All blower graphics and structures are loaded up and initialized here.
// Blowers include vents, ducts, candles, fans, etc.

void InitBlowers (void)
{
	SInt16		i;

	QSetRect(&g_blowerSrcRect, 0, 0, 48, 402);	// 19344 pixels
	g_blowerSrcMap = CreateOffScreenGWorld(&g_blowerSrcRect, kPreferredDepth);
	LoadGraphic(g_blowerSrcMap, GP_BUILTIN_ASSETS, kBlowerPictID);

	g_blowerMaskMap = CreateOffScreenGWorld(&g_blowerSrcRect, 1);
	LoadGraphic(g_blowerMaskMap, GP_BUILTIN_ASSETS, kBlowerMaskID);

	for (i = 0; i < kNumCandleFlames; i++)
	{
		QSetRect(&g_flame[i], 0, 0, 16, 15);
		QOffsetRect(&g_flame[i], 32, 179 + (i * 15));
	}

	for (i = 0; i < kNumTikiFlames; i++)
	{
		QSetRect(&g_tikiFlame[i], 0, 0, 8, 10);
		QOffsetRect(&g_tikiFlame[i], 40, 69 + (i * 10));
	}

	for (i = 0; i < kNumBBQCoals; i++)
	{
		QSetRect(&g_coals[i], 0, 0, 32, 9);
		QOffsetRect(&g_coals[i], 0, 304 + (i * 9));
	}

	QSetRect(&g_leftStartGliderSrc, 0, 0, 48, 16);
	QOffsetRect(&g_leftStartGliderSrc, 0, 358);

	QSetRect(&g_rightStartGliderSrc, 0, 0, 48, 16);
	QOffsetRect(&g_rightStartGliderSrc, 0, 374);
}

//--------------------------------------------------------------  InitFurniture
// Structures and graphics relating to the furniture is loaded up.
// Furniture includes tables, cabinets, shelves, etc.

void InitFurniture (void)
{
	QSetRect(&g_furnitureSrcRect, 0, 0, 64, 278);		// 17856 pixels
	g_furnitureSrcMap = CreateOffScreenGWorld(&g_furnitureSrcRect, kPreferredDepth);
	LoadGraphic(g_furnitureSrcMap, GP_BUILTIN_ASSETS, kFurniturePictID);

	g_furnitureMaskMap = CreateOffScreenGWorld(&g_furnitureSrcRect, 1);
	LoadGraphic(g_furnitureMaskMap, GP_BUILTIN_ASSETS, kFurnitureMaskID);

	QSetRect(&g_tableSrc, 0, 0, 64, 22);
	QOffsetRect(&g_tableSrc, 0, 0);

	QSetRect(&g_shelfSrc, 0, 0, 16, 21);
	QOffsetRect(&g_shelfSrc, 0, 22);

	QSetRect(&g_hingeSrc, 0, 0, 4, 16);
	QOffsetRect(&g_hingeSrc, 16, 22);

	QSetRect(&g_handleSrc, 0, 0, 4, 21);
	QOffsetRect(&g_handleSrc, 20, 22);

	QSetRect(&g_knobSrc, 0, 0, 8, 8);
	QOffsetRect(&g_knobSrc, 24, 22);

	QSetRect(&g_leftFootSrc, 0, 0, 16, 16);
	QOffsetRect(&g_leftFootSrc, 32, 22);

	QSetRect(&g_rightFootSrc, 0, 0, 16, 16);
	QOffsetRect(&g_rightFootSrc, 48, 22);

	QSetRect(&g_deckSrc, 0, 0, 64, 21);
	QOffsetRect(&g_deckSrc, 0, 162);
}

//--------------------------------------------------------------  InitPrizes
// Structures and graphics relating to the prizes (bonuses) are loaded up.
// Prizes includes clocks, rubber bands, extra gliders, etc.

void InitPrizes (void)
{
	SInt16		i;

	QSetRect(&g_bonusSrcRect, 0, 0, 88, 378);		// 33264 pixels
	g_bonusSrcMap = CreateOffScreenGWorld(&g_bonusSrcRect, kPreferredDepth);
	LoadGraphic(g_bonusSrcMap, GP_BUILTIN_ASSETS, kBonusPictID);

	g_bonusMaskMap = CreateOffScreenGWorld(&g_bonusSrcRect, 1);
	LoadGraphic(g_bonusMaskMap, GP_BUILTIN_ASSETS, kBonusMaskID);

	for (i = 0; i < 11; i++)
	{
		QSetRect(&g_digits[i], 0, 0, 4, 6);
		QOffsetRect(&g_digits[i], 28, i * 6);
	}

	for (i = 0; i < 3; i++)
	{
		QSetRect(&g_pendulumSrc[i], 0, 0, 32, 28);
		QOffsetRect(&g_pendulumSrc[i], 56, 186 + (i * 28));
	}

	QSetRect(&g_greaseSrcRt[0], 0, 0, 32, 27);
	QOffsetRect(&g_greaseSrcRt[0], 0, 243);
	QSetRect(&g_greaseSrcRt[1], 0, 0, 32, 27);
	QOffsetRect(&g_greaseSrcRt[1], 0, 270);
	QSetRect(&g_greaseSrcRt[2], 0, 0, 32, 27);
	QOffsetRect(&g_greaseSrcRt[2], 0, 297);
	QSetRect(&g_greaseSrcRt[3], 0, 0, 32, 27);
	QOffsetRect(&g_greaseSrcRt[3], 32, 297);

	QSetRect(&g_greaseSrcLf[0], 0, 0, 32, 27);
	QOffsetRect(&g_greaseSrcLf[0], 0, 324);
	QSetRect(&g_greaseSrcLf[1], 0, 0, 32, 27);
	QOffsetRect(&g_greaseSrcLf[1], 32, 324);
	QSetRect(&g_greaseSrcLf[2], 0, 0, 32, 27);
	QOffsetRect(&g_greaseSrcLf[2], 0, 351);
	QSetRect(&g_greaseSrcLf[3], 0, 0, 32, 27);
	QOffsetRect(&g_greaseSrcLf[3], 32, 351);

	for (i = 0; i < 6; i++)
	{
		QSetRect(&g_starSrc[i], 0, 0, 32, 31);
		QOffsetRect(&g_starSrc[i], 48, i * 31);
	}

	for (i = 0; i < 3; i++)
	{
		QSetRect(&g_sparkleSrc[i + 2], 0, 0, 20, 19);
		QOffsetRect(&g_sparkleSrc[i + 2], 0, 70 + (i * 19));
	}
	g_sparkleSrc[0] = g_sparkleSrc[4];
	g_sparkleSrc[1] = g_sparkleSrc[3];

	QSetRect(&g_pointsSrcRect, 0, 0, 24, 120);		// 2880 pixels
	g_pointsSrcMap = CreateOffScreenGWorld(&g_pointsSrcRect, kPreferredDepth);
	LoadGraphic(g_pointsSrcMap, GP_BUILTIN_ASSETS, kPointsPictID);

	g_pointsMaskMap = CreateOffScreenGWorld(&g_pointsSrcRect, 1);
	LoadGraphic(g_pointsMaskMap, GP_BUILTIN_ASSETS, kPointsMaskID);

	for (i = 0; i < 15; i++)
	{
		QSetRect(&g_pointsSrc[i], 0, 0, 24, 8);
		QOffsetRect(&g_pointsSrc[i], 0, i * 8);
	}
}

//--------------------------------------------------------------  InitTransports
// Structures and graphics relating to the transports is loaded up.
// Transports includes transport ducts, mailboxes, etc.

void InitTransports (void)
{
	QSetRect(&g_transSrcRect, 0, 0, 56, 32);	// 1848 pixels
	g_transSrcMap = CreateOffScreenGWorld(&g_transSrcRect, kPreferredDepth);
	LoadGraphic(g_transSrcMap, GP_BUILTIN_ASSETS, kTransportPictID);

	g_transMaskMap = CreateOffScreenGWorld(&g_transSrcRect, 1);
	LoadGraphic(g_transMaskMap, GP_BUILTIN_ASSETS, kTransportMaskID);
}

//--------------------------------------------------------------  InitSwitches
// Structures and graphics relating to switches are loaded up.
// Switches includes triggers, light switches, etc.

void InitSwitches (void)
{
	QSetRect(&g_switchSrcRect, 0, 0, 32, 104);	// 3360 pixels
	g_switchSrcMap = CreateOffScreenGWorld(&g_switchSrcRect, kPreferredDepth);
	LoadGraphic(g_switchSrcMap, GP_BUILTIN_ASSETS, kSwitchPictID);

	QSetRect(&g_lightSwitchSrc[0], 0, 0, 15, 24);
	QOffsetRect(&g_lightSwitchSrc[0], 0, 0);
	QSetRect(&g_lightSwitchSrc[1], 0, 0, 15, 24);
	QOffsetRect(&g_lightSwitchSrc[1], 16, 0);

	QSetRect(&g_machineSwitchSrc[0], 0, 0, 16, 24);
	QOffsetRect(&g_machineSwitchSrc[0], 0, 24);
	QSetRect(&g_machineSwitchSrc[1], 0, 0, 16, 24);
	QOffsetRect(&g_machineSwitchSrc[1], 16, 24);

	QSetRect(&g_thermostatSrc[0], 0, 0, 15, 24);
	QOffsetRect(&g_thermostatSrc[0], 0, 48);
	QSetRect(&g_thermostatSrc[1], 0, 0, 15, 24);
	QOffsetRect(&g_thermostatSrc[1], 16, 48);

	QSetRect(&g_powerSrc[0], 0, 0, 8, 8);
	QOffsetRect(&g_powerSrc[0], 0, 72);
	QSetRect(&g_powerSrc[1], 0, 0, 8, 8);
	QOffsetRect(&g_powerSrc[1], 8, 72);

	QSetRect(&g_knifeSwitchSrc[0], 0, 0, 16, 24);
	QOffsetRect(&g_knifeSwitchSrc[0], 0, 80);
	QSetRect(&g_knifeSwitchSrc[1], 0, 0, 16, 24);
	QOffsetRect(&g_knifeSwitchSrc[1], 16, 80);
}

//--------------------------------------------------------------  InitLights
// Structures and graphics relating to lights are loaded up.
// Lights includes table lamps, flourescent lights, track lights, etc.

void InitLights (void)
{
	SInt16		i;

	QSetRect(&g_lightSrcRect, 0, 0, 72, 126);		// 9144 pixels
	g_lightSrcMap = CreateOffScreenGWorld(&g_lightSrcRect, kPreferredDepth);
	LoadGraphic(g_lightSrcMap, GP_BUILTIN_ASSETS, kLightPictID);

	g_lightMaskMap = CreateOffScreenGWorld(&g_lightSrcRect, 1);
	LoadGraphic(g_lightMaskMap, GP_BUILTIN_ASSETS, kLightMaskID);

	QSetRect(&g_flourescentSrc1, 0, 0, 16, 12);
	QOffsetRect(&g_flourescentSrc1, 0, 78);

	QSetRect(&g_flourescentSrc2, 0, 0, 16, 12);
	QOffsetRect(&g_flourescentSrc2, 0, 90);

	for (i = 0; i < kNumTrackLights; i++)
	{
		QSetRect(&g_trackLightSrc[i], 0, 0, 24, 24);
		QOffsetRect(&g_trackLightSrc[i], 24 * i, 102);
	}
}

//--------------------------------------------------------------  InitAppliances
// Structures and graphics relating to appliances are loaded up.
// Appliances includes toasters, T.V.s, etc.

void InitAppliances (void)
{
	SInt16		i;

	QSetRect(&g_applianceSrcRect, 0, 0, 80, 269);		// 21600 pixels
	g_applianceSrcMap = CreateOffScreenGWorld(&g_applianceSrcRect, kPreferredDepth);
	LoadGraphic(g_applianceSrcMap, GP_BUILTIN_ASSETS, kAppliancePictID);

	g_applianceMaskMap = CreateOffScreenGWorld(&g_applianceSrcRect, 1);
	LoadGraphic(g_applianceMaskMap, GP_BUILTIN_ASSETS, kApplianceMaskID);

	QSetRect(&g_toastSrcRect, 0, 0, 32, 174);			// 5600 pixels
	g_toastSrcMap = CreateOffScreenGWorld(&g_toastSrcRect, kPreferredDepth);
	LoadGraphic(g_toastSrcMap, GP_BUILTIN_ASSETS, kToastPictID);

	g_toastMaskMap = CreateOffScreenGWorld(&g_toastSrcRect, 1);
	LoadGraphic(g_toastMaskMap, GP_BUILTIN_ASSETS, kToastMaskID);

	QSetRect(&g_shredSrcRect, 0, 0, 40, 35);			// 1440 pixels
	g_shredSrcMap = CreateOffScreenGWorld(&g_shredSrcRect, kPreferredDepth);
	LoadGraphic(g_shredSrcMap, GP_BUILTIN_ASSETS, kShreddedPictID);

	g_shredMaskMap = CreateOffScreenGWorld(&g_shredSrcRect, 1);
	LoadGraphic(g_shredMaskMap, GP_BUILTIN_ASSETS, kShreddedMaskID);

	QSetRect(&g_plusScreen1, 0, 0, 32, 22);
	QOffsetRect(&g_plusScreen1, 48, 127);
	QSetRect(&g_plusScreen2, 0, 0, 32, 22);
	QOffsetRect(&g_plusScreen2, 48, 149);

	QSetRect(&g_tvScreen1, 0, 0, 64, 49);
	QOffsetRect(&g_tvScreen1, 0, 171);
	QSetRect(&g_tvScreen2, 0, 0, 64, 49);
	QOffsetRect(&g_tvScreen2, 0, 220);

	QSetRect(&g_coffeeLight1, 0, 0, 8, 4);
	QOffsetRect(&g_coffeeLight1, 72, 171);
	QSetRect(&g_coffeeLight2, 0, 0, 8, 4);
	QOffsetRect(&g_coffeeLight2, 72, 175);

	for (i = 0; i < kNumOutletPicts; i++)
	{
		QSetRect(&g_outletSrc[i], 0, 0, 16, 24);
		QOffsetRect(&g_outletSrc[i], 64, 22 + (i * 24));
	}

	for (i = 0; i < kNumBreadPicts; i++)
	{
		QSetRect(&g_breadSrc[i], 0, 0, 32, 29);
		QOffsetRect(&g_breadSrc[i], 0, i * 29);
	}

	QSetRect(&g_vcrTime1, 0, 0, 16, 4);
	QOffsetRect(&g_vcrTime1, 64, 179);
	QSetRect(&g_vcrTime2, 0, 0, 16, 4);
	QOffsetRect(&g_vcrTime2, 64, 183);

	QSetRect(&g_stereoLight1, 0, 0, 4, 1);
	QOffsetRect(&g_stereoLight1, 68, 171);
	QSetRect(&g_stereoLight2, 0, 0, 4, 1);
	QOffsetRect(&g_stereoLight2, 68, 172);

	QSetRect(&g_microOn, 0, 0, 16, 35);
	QOffsetRect(&g_microOn, 64, 222);
	QSetRect(&g_microOff, 0, 0, 16, 35);
	QOffsetRect(&g_microOff, 64, 187);
}

//--------------------------------------------------------------  InitEnemies
// Structures and graphics relating to enemies are loaded up.
// Enemies includes darts, balloons, fish, etc.

void InitEnemies (void)
{
	SInt16		i;

	QSetRect(&g_balloonSrcRect, 0, 0, 24, 30 * kNumBalloonFrames);
	g_balloonSrcMap = CreateOffScreenGWorld(&g_balloonSrcRect, kPreferredDepth);
	LoadGraphic(g_balloonSrcMap, GP_BUILTIN_ASSETS, kBalloonPictID);

	g_balloonMaskMap = CreateOffScreenGWorld(&g_balloonSrcRect, 1);
	LoadGraphic(g_balloonMaskMap, GP_BUILTIN_ASSETS, kBalloonMaskID);

	QSetRect(&g_copterSrcRect, 0, 0, 32, 30 * kNumCopterFrames);
	g_copterSrcMap = CreateOffScreenGWorld(&g_copterSrcRect, kPreferredDepth);
	LoadGraphic(g_copterSrcMap, GP_BUILTIN_ASSETS, kCopterPictID);

	g_copterMaskMap = CreateOffScreenGWorld(&g_copterSrcRect, 1);
	LoadGraphic(g_copterMaskMap, GP_BUILTIN_ASSETS, kCopterMaskID);

	QSetRect(&g_dartSrcRect, 0, 0, 64, 19 * kNumDartFrames);
	g_dartSrcMap = CreateOffScreenGWorld(&g_dartSrcRect, kPreferredDepth);
	LoadGraphic(g_dartSrcMap, GP_BUILTIN_ASSETS, kDartPictID);

	g_dartMaskMap = CreateOffScreenGWorld(&g_dartSrcRect, 1);
	LoadGraphic(g_dartMaskMap, GP_BUILTIN_ASSETS, kDartMaskID);

	QSetRect(&g_ballSrcRect, 0, 0, 32, 32 * kNumBallFrames);
	g_ballSrcMap = CreateOffScreenGWorld(&g_ballSrcRect, kPreferredDepth);
	LoadGraphic(g_ballSrcMap, GP_BUILTIN_ASSETS, kBallPictID);

	g_ballMaskMap = CreateOffScreenGWorld(&g_ballSrcRect, 1);
	LoadGraphic(g_ballMaskMap, GP_BUILTIN_ASSETS, kBallMaskID);

	QSetRect(&g_dripSrcRect, 0, 0, 16, 12 * kNumDripFrames);
	g_dripSrcMap = CreateOffScreenGWorld(&g_dripSrcRect, kPreferredDepth);
	LoadGraphic(g_dripSrcMap, GP_BUILTIN_ASSETS, kDripPictID);

	g_dripMaskMap = CreateOffScreenGWorld(&g_dripSrcRect, 1);
	LoadGraphic(g_dripMaskMap, GP_BUILTIN_ASSETS, kDripMaskID);

	QSetRect(&g_enemySrcRect, 0, 0, 36, 33);
	g_enemySrcMap = CreateOffScreenGWorld(&g_enemySrcRect, kPreferredDepth);
	LoadGraphic(g_enemySrcMap, GP_BUILTIN_ASSETS, kEnemyPictID);

	g_enemyMaskMap = CreateOffScreenGWorld(&g_enemySrcRect, 1);
	LoadGraphic(g_enemyMaskMap, GP_BUILTIN_ASSETS, kEnemyMaskID);

	QSetRect(&g_fishSrcRect, 0, 0, 16, 16 * kNumFishFrames);
	g_fishSrcMap = CreateOffScreenGWorld(&g_fishSrcRect, kPreferredDepth);
	LoadGraphic(g_fishSrcMap, GP_BUILTIN_ASSETS, kFishPictID);

	g_fishMaskMap = CreateOffScreenGWorld(&g_fishSrcRect, 1);
	LoadGraphic(g_fishMaskMap, GP_BUILTIN_ASSETS, kFishMaskID);

	for (i = 0; i < kNumBalloonFrames; i++)
	{
		QSetRect(&g_balloonSrc[i], 0, 0, 24, 30);
		QOffsetRect(&g_balloonSrc[i], 0, 30 * i);
	}

	for (i = 0; i < kNumCopterFrames; i++)
	{
		QSetRect(&g_copterSrc[i], 0, 0, 32, 30);
		QOffsetRect(&g_copterSrc[i], 0, 30 * i);
	}

	for (i = 0; i < kNumDartFrames; i++)
	{
		QSetRect(&g_dartSrc[i], 0, 0, 64, 19);
		QOffsetRect(&g_dartSrc[i], 0, 19 * i);
	}

	for (i = 0; i < kNumBallFrames; i++)
	{
		QSetRect(&g_ballSrc[i], 0, 0, 32, 32);
		QOffsetRect(&g_ballSrc[i], 0, 32 * i);
	}

	for (i = 0; i < kNumDripFrames; i++)
	{
		QSetRect(&g_dripSrc[i], 0, 0, 16, 12);
		QOffsetRect(&g_dripSrc[i], 0, 12 * i);
	}

	for (i = 0; i < kNumFishFrames; i++)
	{
		QSetRect(&g_fishSrc[i], 0, 0, 16, 16);
		QOffsetRect(&g_fishSrc[i], 0, 16 * i);
	}
}

//--------------------------------------------------------------  InitClutter
// Structures and graphics relating to clutter are loaded up.
// Clutter includes mirrors, teddy bears, fireplaces, calendars, etc.

void InitClutter (void)
{
	QSetRect(&g_clutterSrcRect, 0, 0, 128, 69);
	g_clutterSrcMap = CreateOffScreenGWorld(&g_clutterSrcRect, kPreferredDepth);
	LoadGraphic(g_clutterSrcMap, GP_BUILTIN_ASSETS, kClutterPictID);

	g_clutterMaskMap = CreateOffScreenGWorld(&g_clutterSrcRect, 1);
	LoadGraphic(g_clutterMaskMap, GP_BUILTIN_ASSETS, kClutterMaskID);

	QSetRect(&g_flowerSrc[0], 0, 0, 10, 28);
	QOffsetRect(&g_flowerSrc[0], 0, 23);

	QSetRect(&g_flowerSrc[1], 0, 0, 24, 35);
	QOffsetRect(&g_flowerSrc[1], 10, 16);

	QSetRect(&g_flowerSrc[2], 0, 0, 34, 35);
	QOffsetRect(&g_flowerSrc[2], 34, 16);

	QSetRect(&g_flowerSrc[3], 0, 0, 27, 23);
	QOffsetRect(&g_flowerSrc[3], 68, 14);

	QSetRect(&g_flowerSrc[4], 0, 0, 27, 14);
	QOffsetRect(&g_flowerSrc[4], 68, 37);

	QSetRect(&g_flowerSrc[5], 0, 0, 32, 51);
	QOffsetRect(&g_flowerSrc[5], 95, 0);
}

//--------------------------------------------------------------  InitSupport
// The floor support grphic is loaded up.  It is only visible when
// playing in 9-room mode.  It is the horizontal wooden beam that
// seperates floors from one another.

void InitSupport (void)
{
	QSetRect(&g_suppSrcRect, 0, 0, kRoomWide, kFloorSupportTall);		// 44
	g_suppSrcMap = CreateOffScreenGWorld(&g_suppSrcRect, kPreferredDepth);
	LoadGraphic(g_suppSrcMap, GP_BUILTIN_ASSETS, kSupportPictID);
}

//--------------------------------------------------------------  InitAngel
// This loads the graphic of the girl riding the glider.  It is seen
// only upon completing a house.  She flies across the screen dropping
// stars below.

void InitAngel (void)
{
	QSetRect(&g_angelSrcRect, 0, 0, 96, 44);
	g_angelSrcMap = CreateOffScreenGWorld(&g_angelSrcRect, kPreferredDepth);
	LoadGraphic(g_angelSrcMap, GP_BUILTIN_ASSETS, kAngelPictID);

	g_angelMaskMap = CreateOffScreenGWorld(&g_angelSrcRect, 1);
	LoadGraphic(g_angelMaskMap, GP_BUILTIN_ASSETS, kAngelMaskID);
}

//--------------------------------------------------------------  InitSplashScreenMap
// This loads the graphic for the splash screen.

void InitSplashScreenMap (void)
{
	QSetRect(&g_splashSrcRect, 0, 0, 640, 460);
	g_splashSrcMap = CreateOffScreenGWorld(&g_splashSrcRect, kPreferredDepth);
	LoadGraphic(g_splashSrcMap, GP_BUILTIN_ASSETS, kSplash8BitPICT);
}

//--------------------------------------------------------------  CreateOffscreens
// All "utility" or "work" offscreen pix/bit maps are created here.
// These would be offscreens that are reused throughout a game - they
// are not static (mere repositories for graphics).  Most are used
// to facilitate the animation when a game is in progress.

void CreateOffscreens (void)
{
	g_justRoomsRect = g_houseRect;
	ZeroRectCorner(&g_justRoomsRect);

	g_workSrcRect = g_houseRect;			// Set up work map
	ZeroRectCorner(&g_workSrcRect);
	g_workSrcMap = CreateOffScreenGWorld(&g_workSrcRect, kPreferredDepth);

	g_backSrcRect = g_houseRect;			// Set up background map
	ZeroRectCorner(&g_backSrcRect);
	g_backSrcMap = CreateOffScreenGWorld(&g_backSrcRect, kPreferredDepth);

	InitScoreboardMap();
	InitGliderMap();
	InitBlowers();
	InitFurniture();
	InitPrizes();
	InitTransports();
	InitSwitches();
	InitLights();
	InitAppliances();
	InitEnemies();
	InitClutter();
	InitSupport();
	InitAngel();
	InitSplashScreenMap();

	QSetRect(&g_tileSrcRect, 0, 0, 128, 80);
	g_tileSrcMap = NULL;
//	????
}

//--------------------------------------------------------------  GetDemoDataPointer

Boolean GetDemoDataPointer (void **ppDemoData, size_t *pDemoSize)
{
	HRSRC hResInfo;
	HGLOBAL hResData;
	DWORD resSize;
	LPVOID resData;

	hResInfo = FindResource(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDR_DEMO), RT_DEMO);
	if (hResInfo == NULL)
	{
		return false;
	}
	hResData = LoadResource(HINST_THISCOMPONENT, hResInfo);
	if (hResData == NULL)
	{
		return false;
	}
	resSize = SizeofResource(HINST_THISCOMPONENT, hResInfo);
	if (resSize == 0)
	{
		return false;
	}
	resData = LockResource(hResData);
	if (resData == NULL)
	{
		return false;
	}
	*ppDemoData = resData;
	*pDemoSize = resSize;
	return true;
}

//--------------------------------------------------------------  CreatePointers
// This function allocates other large structures.  Pointers to hold
// large arrays, etc.

void CreatePointers (void)
{
	size_t demoResourceSize;
	void *demoResourceData;
	byteio *demoReader;
	size_t i;
	HRESULT hr;

	g_thisRoom = NULL;
	g_thisRoom = (roomPtr)malloc(sizeof(roomType));
	if (g_thisRoom == NULL)
		RedAlert(kErrNoMemory);

	for (i = 0; i < kMaxSavedMaps; i++)
		g_savedMaps[i].map = NULL;

	g_theHousesSpecs = NULL;
	g_theHousesSpecs = (houseSpecPtr)malloc(sizeof(houseSpec) * g_maxFiles);
	if (g_theHousesSpecs == NULL)
		RedAlert(kErrNoMemory);

	// Make sure that g_demoData can hold the demo resource data.
	C_ASSERT((kDemoLength / demoTypeByteSize) <= ARRAYSIZE(g_demoData));

	if (!CREATEDEMODATA)
	{
		if (!GetDemoDataPointer(&demoResourceData, &demoResourceSize))
			RedAlert(kErrFailedResourceLoad);
		if (demoResourceSize != kDemoLength)
			RedAlert(kErrFailedResourceLoad);

		demoReader = byteio_init_memory_reader(demoResourceData, demoResourceSize);
		if (demoReader == NULL)
			RedAlert(kErrNoMemory);
		for (i = 0; i < (kDemoLength / demoTypeByteSize); i++)
		{
			hr = ReadDemoType(demoReader, &g_demoData[i]);
			if (FAILED(hr))
			{
				RedAlert(kErrFailedResourceLoad);
			}
		}
		byteio_close(demoReader);
	}
}

//--------------------------------------------------------------  InitSrcRects
// This is a nasty, ugly function that initializes all global rectangles
// used in Glider PRO.

void InitSrcRects (void)
{
	QSetRect(&g_srcRects[kFloorVent], 0, 0, 48, 11);		// Blowers
	QOffsetRect(&g_srcRects[kFloorVent], 0, 0);
	QSetRect(&g_srcRects[kCeilingVent], 0, 0, 48, 11);
	QOffsetRect(&g_srcRects[kCeilingVent], 0, 11);
	QSetRect(&g_srcRects[kFloorBlower], 0, 0, 48, 15);
	QOffsetRect(&g_srcRects[kFloorBlower], 0, 22);
	QSetRect(&g_srcRects[kCeilingBlower], 0, 0, 48, 15);
	QOffsetRect(&g_srcRects[kCeilingBlower], 0, 37);
	QSetRect(&g_srcRects[kSewerGrate], 0, 0, 48, 17);
	QOffsetRect(&g_srcRects[kSewerGrate], 0, 52);
	QSetRect(&g_srcRects[kLeftFan], 0, 0, 40, 55);
	QOffsetRect(&g_srcRects[kLeftFan], 0, 69);
	QSetRect(&g_srcRects[kRightFan], 0, 0, 40, 55);
	QOffsetRect(&g_srcRects[kRightFan], 0, 124);
	QSetRect(&g_srcRects[kTaper], 0, 0, 20, 59);
	QOffsetRect(&g_srcRects[kTaper], 0, 209);
	QSetRect(&g_srcRects[kCandle], 0, 0, 32, 30);
	QOffsetRect(&g_srcRects[kCandle], 0, 179);
	QSetRect(&g_srcRects[kStubby], 0, 0, 20, 36);
	QOffsetRect(&g_srcRects[kStubby], 0, 268);
	QSetRect(&g_srcRects[kTiki], 0, 0, 27, 28);
	QOffsetRect(&g_srcRects[kTiki], 21, 268);
	QSetRect(&g_srcRects[kBBQ], 0, 0, 64, 33);
	QSetRect(&g_srcRects[kInvisBlower], 0, 0, 24, 24);
	QSetRect(&g_srcRects[kGrecoVent], 0, 0, 48, 18);
	QOffsetRect(&g_srcRects[kGrecoVent], 0, 340);
	QSetRect(&g_srcRects[kSewerBlower], 0, 0, 32, 12);
	QOffsetRect(&g_srcRects[kSewerBlower], 0, 390);
	QSetRect(&g_srcRects[kLiftArea], 0, 0, 64, 32);

	QSetRect(&g_srcRects[kTable], 0, 0, 64, kTableThick);		// Furniture
	QSetRect(&g_srcRects[kShelf], 0, 0, 64, kShelfThick);
	QSetRect(&g_srcRects[kCabinet], 0, 0, 64, 64);
	QSetRect(&g_srcRects[kFilingCabinet], 0, 0, 74, 107);
	QSetRect(&g_srcRects[kWasteBasket], 0, 0, 64, 61);
	QOffsetRect(&g_srcRects[kWasteBasket], 0, 43);
	QSetRect(&g_srcRects[kMilkCrate], 0, 0, 64, 58);
	QOffsetRect(&g_srcRects[kMilkCrate], 0, 104);
	QSetRect(&g_srcRects[kCounter], 0, 0, 128, 64);
	QSetRect(&g_srcRects[kDresser], 0, 0, 128, 64);
	QSetRect(&g_srcRects[kDeckTable], 0, 0, 64, kTableThick);
	QSetRect(&g_srcRects[kStool], 0, 0, 48, 38);
	QOffsetRect(&g_srcRects[kStool], 0, 183);
	QSetRect(&g_srcRects[kTrunk], 0, 0, 144, 80);
	QSetRect(&g_srcRects[kInvisObstacle], 0, 0, 64, 64);
	QSetRect(&g_srcRects[kManhole], 0, 0, 123, 22);
	QSetRect(&g_srcRects[kBooks], 0, 0, 64, 51);
	QSetRect(&g_srcRects[kInvisBounce], 0, 0, 64, 64);

	QSetRect(&g_srcRects[kRedClock], 0, 0, 28, 17);			// Prizes
	QSetRect(&g_srcRects[kBlueClock], 0, 0, 28, 25);
	QOffsetRect(&g_srcRects[kBlueClock], 0, 17);
	QSetRect(&g_srcRects[kYellowClock], 0, 0, 28, 28);
	QOffsetRect(&g_srcRects[kYellowClock], 0, 42);
	QSetRect(&g_srcRects[kCuckoo], 0, 0, 40, 80);
	QOffsetRect(&g_srcRects[kCuckoo], 0, 148);
	QSetRect(&g_srcRects[kPaper], 0, 0, 48, 21);
	QOffsetRect(&g_srcRects[kPaper], 0, 127);
	QSetRect(&g_srcRects[kBattery], 0, 0, 16, 25);
	QOffsetRect(&g_srcRects[kBattery], 32, 0);
	QSetRect(&g_srcRects[kBands], 0, 0, 28, 23);
	QOffsetRect(&g_srcRects[kBands], 20, 70);
	QSetRect(&g_srcRects[kGreaseRt], 0, 0, 32, 27);
	QOffsetRect(&g_srcRects[kGreaseRt], 0, 243);
	QSetRect(&g_srcRects[kGreaseLf], 0, 0, 32, 27);
	QOffsetRect(&g_srcRects[kGreaseLf], 0, 324);
	QSetRect(&g_srcRects[kFoil], 0, 0, 55, 15);
	QOffsetRect(&g_srcRects[kFoil], 0, 228);
	QSetRect(&g_srcRects[kInvisBonus], 0, 0, 24, 24);
	QSetRect(&g_srcRects[kStar], 0, 0, 32, 31);
	QOffsetRect(&g_srcRects[kStar], 48, 0);
	QSetRect(&g_srcRects[kSparkle], 0, 0, 20, 19);
	QOffsetRect(&g_srcRects[kSparkle], 0, 70);
	QSetRect(&g_srcRects[kHelium], 0, 0, 56, 16);
	QOffsetRect(&g_srcRects[kHelium], 32, 270);
	QSetRect(&g_srcRects[kSlider], 0, 0, 64, 16);

	QSetRect(&g_srcRects[kUpStairs], 0, 0, 160, 267);			// Transport
	QSetRect(&g_srcRects[kDownStairs], 0, 0, 160, 267);
	QSetRect(&g_srcRects[kMailboxLf], 0, 0, 94, 80);
	QSetRect(&g_srcRects[kMailboxRt], 0, 0, 94, 80);
	QSetRect(&g_srcRects[kFloorTrans], 0, 0, 56, 15);
	QOffsetRect(&g_srcRects[kFloorTrans], 0, 1);
	QSetRect(&g_srcRects[kCeilingTrans], 0, 0, 56, 15);
	QOffsetRect(&g_srcRects[kCeilingTrans], 0, 16);
	QSetRect(&g_srcRects[kDoorInLf], 0, 0, 144, 322);
	QSetRect(&g_srcRects[kDoorInRt], 0, 0, 144, 322);
	QSetRect(&g_srcRects[kDoorExRt], 0, 0, 16, 322);
	QSetRect(&g_srcRects[kDoorExLf], 0, 0, 16, 322);
	QSetRect(&g_srcRects[kWindowInLf], 0, 0, 20, 170);
	QSetRect(&g_srcRects[kWindowInRt], 0, 0, 20, 170);
	QSetRect(&g_srcRects[kWindowExRt], 0, 0, 16, 170);
	QSetRect(&g_srcRects[kWindowExLf], 0, 0, 16, 170);
	QSetRect(&g_srcRects[kInvisTrans], 0, 0, 64, 32);
	QSetRect(&g_srcRects[kDeluxeTrans], 0, 0, 64, 64);

	QSetRect(&g_srcRects[kLightSwitch], 0, 0, 15, 24);		// Switch
	QSetRect(&g_srcRects[kMachineSwitch], 0, 0, 16, 24);
	QOffsetRect(&g_srcRects[kMachineSwitch], 0, 48);
	QSetRect(&g_srcRects[kThermostat], 0, 0, 15, 24);
	QOffsetRect(&g_srcRects[kThermostat], 0, 48);
	QSetRect(&g_srcRects[kPowerSwitch], 0, 0, 8, 8);
	QOffsetRect(&g_srcRects[kPowerSwitch], 0, 72);
	QSetRect(&g_srcRects[kKnifeSwitch], 0, 0, 16, 24);
	QOffsetRect(&g_srcRects[kKnifeSwitch], 0, 80);
	QSetRect(&g_srcRects[kInvisSwitch], 0, 0, 12, 12);
	QSetRect(&g_srcRects[kTrigger], 0, 0, 12, 12);
	QSetRect(&g_srcRects[kLgTrigger], 0, 0, 48, 48);
	QSetRect(&g_srcRects[kSoundTrigger], 0, 0, 32, 32);

	QSetRect(&g_srcRects[kCeilingLight], 0, 0, 64, 20);		// Lights
	QOffsetRect(&g_srcRects[kCeilingLight], 0, 0);
	QSetRect(&g_srcRects[kLightBulb], 0, 0, 16, 28);
	QOffsetRect(&g_srcRects[kLightBulb], 0, 20);
	QSetRect(&g_srcRects[kTableLamp], 0, 0, 48, 70);
	QOffsetRect(&g_srcRects[kTableLamp], 16, 20);
	QSetRect(&g_srcRects[kHipLamp], 0, 0, 72, 276);
	QSetRect(&g_srcRects[kDecoLamp], 0, 0, 64, 212);
	QSetRect(&g_srcRects[kFlourescent], 0, 0, 64, 12);
	QSetRect(&g_srcRects[kTrackLight], 0, 0, 64, 24);
	QSetRect(&g_srcRects[kInvisLight], 0, 0, 16, 16);

	QSetRect(&g_srcRects[kShredder], 0, 0, 73, 22);			// Appliances
	QSetRect(&g_srcRects[kToaster], 0, 0, 48, 27);
	QOffsetRect(&g_srcRects[kToaster], 0, 22);
	QSetRect(&g_srcRects[kMacPlus], 0, 0, 48, 58);
	QOffsetRect(&g_srcRects[kMacPlus], 0, 49);
	QSetRect(&g_srcRects[kGuitar], 0, 0, 64, 172);
	QSetRect(&g_srcRects[kTV], 0, 0, 92, 77);
	QSetRect(&g_srcRects[kCoffee], 0, 0, 43, 64);
	QOffsetRect(&g_srcRects[kCoffee], 0, 107);
	QSetRect(&g_srcRects[kOutlet], 0, 0, 16, 24);
	QOffsetRect(&g_srcRects[kOutlet], 64, 22);
	QSetRect(&g_srcRects[kVCR], 0, 0, 96, 22);
	QSetRect(&g_srcRects[kStereo], 0, 0, 128, 53);
	QSetRect(&g_srcRects[kMicrowave], 0, 0, 92, 59);
	QSetRect(&g_srcRects[kCinderBlock], 0, 0, 40, 62);
	QSetRect(&g_srcRects[kFlowerBox], 0, 0, 80, 32);
	QSetRect(&g_srcRects[kCDs], 0, 0, 16, 30);
	QOffsetRect(&g_srcRects[kCDs], 48, 22);
	QSetRect(&g_srcRects[kCustomPict], 0, 0, 72, 34);

	QSetRect(&g_srcRects[kBalloon], 0, 0, 24, 30);			// Enemies
	QSetRect(&g_srcRects[kCopterLf], 0, 0, 32, 30);
	QSetRect(&g_srcRects[kCopterRt], 0, 0, 32, 30);
	QSetRect(&g_srcRects[kDartLf], 0, 0, 64, 19);
	QSetRect(&g_srcRects[kDartRt], 0, 0, 64, 19);
	QSetRect(&g_srcRects[kBall], 0, 0, 32, 32);
	QSetRect(&g_srcRects[kDrip], 0, 0, 16, 12);
	QSetRect(&g_srcRects[kFish], 0, 0, 36, 33);
	QSetRect(&g_srcRects[kCobweb], 0, 0, 54, 45);

	QSetRect(&g_srcRects[kOzma], 0, 0, 102, 92);				// Clutter
	QSetRect(&g_srcRects[kMirror], 0, 0, 64, 64);
	QSetRect(&g_srcRects[kMousehole], 0, 0, 10, 11);
	QSetRect(&g_srcRects[kFireplace], 0, 0, 180, 142);
	QSetRect(&g_srcRects[kWallWindow], 0, 0, 64, 80);
	QSetRect(&g_srcRects[kBear], 0, 0, 56, 58);
	QSetRect(&g_srcRects[kCalendar], 0, 0, 63, 92);
	QSetRect(&g_srcRects[kVase1], 0, 0, 36, 45);
	QSetRect(&g_srcRects[kVase2], 0, 0, 35, 57);
	QSetRect(&g_srcRects[kBulletin], 0, 0, 80, 58);
	QSetRect(&g_srcRects[kCloud], 0, 0, 128, 30);
	QSetRect(&g_srcRects[kFaucet], 0, 0, 56, 18);
	QOffsetRect(&g_srcRects[kFaucet], 0, 51);
	QSetRect(&g_srcRects[kRug], 0, 0, 144, 18);
	QSetRect(&g_srcRects[kChimes], 0, 0, 28, 74);
}
