//============================================================================
//----------------------------------------------------------------------------
//                                   Play.c
//----------------------------------------------------------------------------
//============================================================================

#include "Play.h"

#include "Banner.h"
#include "DynamicMaps.h"
#include "Dynamics.h"
#include "Environ.h"
#include "Events.h"
#include "GameOver.h"
#include "HighScores.h"
#include "House.h"
#include "HouseIO.h"
#include "Input.h"
#include "Interactions.h"
#include "Macintosh.h"
#include "Main.h"
#include "MainWindow.h"
#include "Menu.h"
#include "Modes.h"
#include "Music.h"
#include "Player.h"
#include "RectUtils.h"
#include "Render.h"
#include "ResourceIDs.h"
#include "ResourceLoader.h"
#include "Room.h"
#include "RoomGraphics.h"
#include "RubberBands.h"
#include "SavedGames.h"
#include "Scoreboard.h"
#include "SelectHouse.h"
#include "Sound.h"
#include "StringUtils.h"
#include "Transitions.h"
#include "Triggers.h"
#include "Utilities.h"

#include <mmsystem.h>

#define kInitialGliders         2
#define kRingDelay              90
#define kRingSpread             25000   // 25000
#define kRingBaseDelay          5000    // 5000
#define kChimeDelay             180

typedef struct phoneType
{
	SInt16 nextRing;
	SInt16 rings;
	SInt16 delay;
} phoneType, *phonePtr;

void InitGlider (gliderPtr thisGlider, SInt16 mode);
void SetHouseToFirstRoom (void);
void SetHouseToSavedRoom (void);
void PlayGame (SInt16 splashHouseIndex);
void HandleRoomVisitation (void);
void SetObjectsToDefaults (void);
void InitTelephone (void);
void HandleTelephone (void);

Rect g_glidSrcRect;
Rect g_justRoomsRect;
HDC g_glidSrcMap;
HDC g_glid2SrcMap;
HDC g_glidMaskMap;
SInt32 g_gameFrame;
SInt16 g_batteryTotal;
SInt16 g_bandsTotal;
SInt16 g_foilTotal;
SInt16 g_mortals;
Boolean g_playing;
Boolean g_evenFrame;
Boolean g_twoPlayerGame;
Boolean g_showFoil;
Boolean g_demoGoing;
Boolean g_playerSuicide;
Boolean g_phoneBitSet;
Boolean g_tvOn;

static phoneType g_thePhone;
static phoneType g_theChimes;

//==============================================================  Functions
//--------------------------------------------------------------  NewGame

void NewGame (HWND ownerWindow, SInt16 mode, SInt16 splashHouseIndex)
{
	Rect tempRect;
	OSErr theErr;
	Boolean wasPlayMusicPref;
	TIMECAPS timeCaps;
	MMRESULT mmResult;

	AdjustScoreboardHeight();
	g_gameOver = false;
	g_theMode = kPlayMode;
	if (g_isPlayMusicGame)
	{
		if (!g_isMusicOn)
		{
			theErr = StartMusic();
			if (theErr != noErr)
			{
				YellowAlert(ownerWindow, kYellowNoMusic, theErr);
				g_failedMusic = true;
			}
		}
		SetMusicalMode(kPlayGameScoreMode);
	}
	else
	{
		if (g_isMusicOn)
			StopTheMusic();
	}
	if (mode != kResumeGameMode)
		SetObjectsToDefaults();
	if (mode == kResumeGameMode)
		SetHouseToSavedRoom();
	else if (mode == kNewGameMode)
		SetHouseToFirstRoom();
	DetermineRoomOpenings();
	NilSavedMaps();

	g_gameFrame = 0L;
	g_numBands = 0;
	g_demoIndex = 0;
	g_saidFollow = 0;
	g_otherPlayerEscaped = kNoOneEscaped;
	g_onePlayerLeft = false;
	g_playerSuicide = false;

	if (g_twoPlayerGame)  // initialize glider(s)
	{
		InitGlider(&g_theGlider, kNewGameMode);
		InitGlider(&g_theGlider2, kNewGameMode);
		LoadGraphic(g_glidSrcMap, g_theHouseFile, kGliderPictID);
		LoadGraphic(g_glid2SrcMap, g_theHouseFile, kGlider2PictID);
	}
	else
	{
		InitGlider(&g_theGlider, mode);
		LoadGraphic(g_glidSrcMap, g_theHouseFile, kGliderPictID);
		LoadGraphic(g_glid2SrcMap, g_theHouseFile, kGliderFoilPictID);
	}

#ifdef COMPILEQT
	if ((g_thisMac.hasQT) && (g_hasMovie))
	{
		SetMovieGWorld(g_theMovie, (CGrafPtr)g_mainWindow, nil);
	}
#endif

	Mac_PaintRect(g_workSrcMap, &g_workSrcRect, GetStockBrush(BLACK_BRUSH));
	DissolveScreenOn(&g_workSrcRect);
	SetMenu(g_mainWindow, NULL);
	UpdateWindow(g_mainWindow);

	DrawLocale();
	RefreshScoreboard(kNormalTitleMode);
	DissolveScreenOn(&g_justRoomsRect);
	if (mode == kNewGameMode)
	{
		BringUpBanner();
		DumpScreenOn(&g_justRoomsRect);
	}
	else if (mode == kResumeGameMode)
	{
		DisplayStarsRemaining();
		DumpScreenOn(&g_justRoomsRect);
	}
	else
	{
		DumpScreenOn(&g_justRoomsRect);
	}

	InitGarbageRects();
	StartGliderFadingIn(&g_theGlider);
	if (g_twoPlayerGame)
	{
		StartGliderFadingIn(&g_theGlider2);
		TagGliderIdle(&g_theGlider2);
		g_theGlider2.dontDraw = true;
	}
	InitTelephone();
	wasPlayMusicPref = g_isPlayMusicGame;

	if (CREATEDEMODATA)
	{
		MessageBeep(MB_ICONINFORMATION);
	}

#ifdef COMPILEQT
	if ((g_thisMac.hasQT) && (g_hasMovie) && (g_tvInRoom))
	{
		SetMovieActive(g_theMovie, true);
		if (g_tvOn)
		{
			StartMovie(g_theMovie);
			MoviesTask(g_theMovie, 0);
		}
	}
#endif

	mmResult = timeGetDevCaps(&timeCaps, sizeof(timeCaps));
	if (mmResult == MMSYSERR_NOERROR)
	{
		if (timeCaps.wPeriodMin < 1)
		{
			timeCaps.wPeriodMin = 1;
		}
		mmResult = timeBeginPeriod(timeCaps.wPeriodMin);
	}

	g_playing = true;  // everything before this line is game set-up
	PlayGame(splashHouseIndex);  // everything following is after a game has ended

	if (mmResult == MMSYSERR_NOERROR)
	{
		timeEndPeriod(timeCaps.wPeriodMin);
	}

	if (CREATEDEMODATA)
	{
		DumpDemoData();
	}

	g_isPlayMusicGame = wasPlayMusicPref;
	ZeroMirrorRegion();

#ifdef COMPILEQT
	if ((g_thisMac.hasQT) && (g_hasMovie) && (g_tvInRoom))
	{
		g_tvInRoom = false;
		StopMovie(g_theMovie);
		SetMovieActive(g_theMovie, false);
	}
#endif

	g_twoPlayerGame = false;
	g_theMode = kSplashMode;
	if (g_isPlayMusicIdle)
	{
		if (!g_isMusicOn)
		{
			theErr = StartMusic();
			if (theErr != noErr)
			{
				YellowAlert(ownerWindow, kYellowNoMusic, theErr);
				g_failedMusic = true;
			}
		}
		SetMusicalMode(kPlayWholeScoreMode);
	}
	else
	{
		if (g_isMusicOn)
			StopTheMusic();
	}
	NilSavedMaps();
	UpdateMenus(false);

	if (!g_gameOver)
	{
		InvalidateRect(g_mainWindow, NULL, TRUE);
		Mac_PaintRect(g_workSrcMap, &g_workSrcRect, GetStockBrush(BLACK_BRUSH));
		tempRect = g_splashSrcRect;
		ZeroRectCorner(&tempRect);
		QOffsetRect(&tempRect, g_splashOriginH, g_splashOriginV);
		Mac_CopyBits(g_splashSrcMap, g_workSrcMap, &g_splashSrcRect, &tempRect, srcCopy, nil);
	}
	g_demoGoing = false;
	g_incrementModeTime = timeGetTime() + kIdleSplashTime;
}

//--------------------------------------------------------------  DoDemoGame

void DoDemoGame (HWND ownerWindow)
{
	SInt16 wasHouseIndex;
	Boolean whoCares;

	wasHouseIndex = g_thisHouseIndex;
	whoCares = CloseHouse(ownerWindow);
	g_thisHouseIndex = g_demoHouseIndex;
	PasStringCopy(g_theHousesSpecs[g_thisHouseIndex].name, g_thisHouseName);
	if (OpenHouse(ownerWindow))
	{
		// NOTE: We aren't reloading the splash screen when reading the
		// demo house. This is so the previous splash screen will be shown
		// when the demo ends (whether by user abort or by completing).
		whoCares = ReadHouse(ownerWindow, false);
		g_demoGoing = true;
		// The previous house's name should be shown on the splash screen
		// when the demo reaches its game over point, so pass the previous
		// house's index instead of the index to the demo house.
		NewGame(ownerWindow, kNewGameMode, wasHouseIndex);
	}
	whoCares = CloseHouse(ownerWindow);
	g_thisHouseIndex = wasHouseIndex;
	PasStringCopy(g_theHousesSpecs[g_thisHouseIndex].name, g_thisHouseName);
	if (OpenHouse(ownerWindow))
		whoCares = ReadHouse(ownerWindow, true);
	g_incrementModeTime = timeGetTime() + kIdleSplashTime;
}

//--------------------------------------------------------------  InitGlider

void InitGlider (gliderPtr thisGlider, SInt16 mode)
{
	WhereDoesGliderBegin(&thisGlider->dest, mode);

	if (mode == kResumeGameMode)
		g_numStarsRemaining = g_smallGame.wasStarsLeft;
	else if (mode == kNewGameMode)
		g_numStarsRemaining = CountStarsInHouse(&g_thisHouse);

	if (mode == kResumeGameMode)
	{
		g_theScore = g_smallGame.score;
		g_mortals = g_smallGame.numGliders;
		g_batteryTotal = g_smallGame.energy;
		g_bandsTotal = g_smallGame.bands;
		g_foilTotal = g_smallGame.foil;
		thisGlider->mode = g_smallGame.gliderState;
		thisGlider->facing = g_smallGame.facing;
		g_showFoil = g_smallGame.showFoil;

		switch (thisGlider->mode)
		{
			case kGliderBurning:
			FlagGliderBurning(thisGlider);
			break;

			default:
			FlagGliderNormal(thisGlider);
			break;
		}
	}
	else
	{
		g_theScore = 0L;
		g_mortals = kInitialGliders;
		if (g_twoPlayerGame)
			g_mortals += kInitialGliders;
		g_batteryTotal = 0;
		g_bandsTotal = 0;
		g_foilTotal = 0;
		thisGlider->mode = kGliderNormal;
		thisGlider->facing = kFaceRight;
		thisGlider->src = g_gliderSrc[0];
		thisGlider->mask = g_gliderSrc[0];
		g_showFoil = false;
	}

	QSetRect(&thisGlider->destShadow, 0, 0, kGliderWide, kShadowHigh);
	QOffsetRect(&thisGlider->destShadow, thisGlider->dest.left, kShadowTop);
	thisGlider->wholeShadow = thisGlider->destShadow;

	thisGlider->hVel = 0;
	thisGlider->vVel = 0;
	thisGlider->hDesiredVel = 0;
	thisGlider->vDesiredVel = 0;

	thisGlider->tipped = false;
	thisGlider->sliding = false;
	thisGlider->dontDraw = false;
}

//--------------------------------------------------------------  SetHouseToFirstRoom

void SetHouseToFirstRoom (void)
{
	SInt16 firstRoom;

	firstRoom = GetFirstRoomNumber();
	ForceThisRoom(firstRoom);
}

//--------------------------------------------------------------  SetHouseToSavedRoom

void SetHouseToSavedRoom (void)
{
	ForceThisRoom(g_smallGame.roomNumber);
}

//--------------------------------------------------------------  PlayGame

void PlayGame (SInt16 splashHouseIndex)
{
	SInt16 originalMode;

	while ((g_playing) && (!g_quitting))
	{
		g_gameFrame++;
		g_evenFrame = !g_evenFrame;

		HandleTelephone();

		if (g_twoPlayerGame)
		{
			HandleDynamics();
			if (!g_gameOver)
			{
				GetInput(&g_theGlider);
				GetInput(&g_theGlider2);
				HandleInteraction();
			}
			HandleTriggers();
			HandleBands();
			if (!g_gameOver)
			{
				HandleGlider(&g_theGlider);
				HandleGlider(&g_theGlider2);
			}
			if (g_playing)
			{
#ifdef COMPILEQT
				if ((g_thisMac.hasQT) && (g_hasMovie) && (g_tvInRoom) && (g_tvOn))
						MoviesTask(g_theMovie, 0);
#endif
				RenderFrame();
				HandleDynamicScoreboard();
			}
		}
		else
		{
			HandleDynamics();
			if (!g_gameOver)
			{
				if (g_demoGoing)
					GetDemoInput(&g_theGlider);
				else
					GetInput(&g_theGlider);
				HandleInteraction();
			}
			HandleTriggers();
			HandleBands();
			if (!g_gameOver)
				HandleGlider(&g_theGlider);
			if (g_playing)
			{
#ifdef COMPILEQT
				if ((g_thisMac.hasQT) && (g_hasMovie) && (g_tvInRoom) && (g_tvOn))
						MoviesTask(g_theMovie, 0);
#endif
				RenderFrame();
				HandleDynamicScoreboard();
			}
		}

		if (g_gameOver)
		{
			g_countDown--;
			if (g_countDown <= 0)
			{
				HideGlider(&g_theGlider);

				originalMode = GetScoreboardMode();
				if (g_mortals < 0)
				{
					RefreshScoreboard(kNormalTitleMode);
					DoDiedGameOver();
				}
				else
				{
					SetScoreboardMode(kScoreboardHigh);
					RefreshScoreboard(kNormalTitleMode);
					DoGameOver();
				}
				if (!g_demoGoing)
				{
					if (TestHighScore(g_mainWindow))
					{
						SetScoreboardMode(kScoreboardHigh);
						SetMenu(g_mainWindow, g_theMenuBar);
						DoHighScores();
					}
				}
				SetScoreboardMode(originalMode);

				SetMenu(g_mainWindow, g_theMenuBar);
				if (!g_quitting)
				{
					RedrawSplashScreen(splashHouseIndex);
				}
			}
		}
	}
	SetMenu(g_mainWindow, g_theMenuBar);
}

//--------------------------------------------------------------  SetObjectsToDefaults

void SetObjectsToDefaults (void)
{
	SInt16 numRooms;
	SInt16 r, i;
	Boolean initState;

	numRooms = g_thisHouse.nRooms;

	for (r = 0; r < numRooms; r++)
	{
		g_thisHouse.rooms[r].visited = false;
		for (i = 0; i < kMaxRoomObs; i++)
		{
			switch (g_thisHouse.rooms[r].objects[i].what)
			{
				case kFloorVent:
				case kCeilingVent:
				case kFloorBlower:
				case kCeilingBlower:
				case kLeftFan:
				case kRightFan:
				case kSewerGrate:
				case kInvisBlower:
				case kGrecoVent:
				case kSewerBlower:
				case kLiftArea:
				g_thisHouse.rooms[r].objects[i].data.a.state =
					g_thisHouse.rooms[r].objects[i].data.a.initial;
				break;

				case kRedClock:
				case kBlueClock:
				case kYellowClock:
				case kCuckoo:
				case kPaper:
				case kBattery:
				case kBands:
				case kGreaseRt:
				case kGreaseLf:
				case kFoil:
				case kInvisBonus:
				case kStar:
				case kSparkle:
				case kHelium:
				g_thisHouse.rooms[r].objects[i].data.c.state =
					g_thisHouse.rooms[r].objects[i].data.c.initial;
				break;

				case kDeluxeTrans:
				initState = (g_thisHouse.rooms[r].objects[i].data.d.wide & 0xF0) >> 4;
				g_thisHouse.rooms[r].objects[i].data.d.wide &= 0xF0;
				g_thisHouse.rooms[r].objects[i].data.d.wide += initState;
				break;

				case kCeilingLight:
				case kLightBulb:
				case kTableLamp:
				case kHipLamp:
				case kDecoLamp:
				case kFlourescent:
				case kTrackLight:
				case kInvisLight:
				g_thisHouse.rooms[r].objects[i].data.f.state =
					g_thisHouse.rooms[r].objects[i].data.f.initial;
				break;

				case kStereo:
				g_thisHouse.rooms[r].objects[i].data.g.state = g_isPlayMusicGame;
				break;

				case kShredder:
				case kToaster:
				case kMacPlus:
				case kGuitar:
				case kTV:
				case kCoffee:
				case kOutlet:
				case kVCR:
				case kMicrowave:
				g_thisHouse.rooms[r].objects[i].data.g.state =
					g_thisHouse.rooms[r].objects[i].data.g.initial;
				break;

				case kBalloon:
				case kCopterLf:
				case kCopterRt:
				case kDartLf:
				case kDartRt:
				case kBall:
				case kDrip:
				case kFish:
				g_thisHouse.rooms[r].objects[i].data.h.state =
					g_thisHouse.rooms[r].objects[i].data.h.initial;
				break;

			}
		}
	}
}

//--------------------------------------------------------------  HideGlider

void HideGlider (const gliderType *thisGlider)
{
	Rect tempRect;

	tempRect = thisGlider->whole;
	QOffsetRect(&tempRect, g_playOriginH, g_playOriginV);
	CopyRectWorkToMain(&tempRect);

	if (g_hasMirror)
	{
		QOffsetRect(&tempRect, kReflectionOffsetH, KReflectionOffsetV);
		CopyRectWorkToMain(&tempRect);
	}

	tempRect = thisGlider->wholeShadow;
	QOffsetRect(&tempRect, g_playOriginH, g_playOriginV);
	CopyRectWorkToMain(&tempRect);
}

//--------------------------------------------------------------  InitTelephone

void InitTelephone (void)
{
	g_thePhone.nextRing = RandomInt(kRingSpread) + kRingBaseDelay;
	g_thePhone.rings = RandomInt(3) + 3;
	g_thePhone.delay = kRingDelay;

	g_theChimes.nextRing = RandomInt(kChimeDelay) + 1;
}

//--------------------------------------------------------------  HandleTelephone

void HandleTelephone (void)
{
	SInt16 delayTime;

	if (!g_phoneBitSet)
	{
		if (g_thePhone.nextRing == 0)
		{
			if (g_thePhone.delay == 0)
			{
				g_thePhone.delay = kRingDelay;
				PlayPrioritySound(kPhoneRingSound, kPhoneRingPriority);
				g_thePhone.rings--;
				if (g_thePhone.rings == 0)
				{
					g_thePhone.nextRing = RandomInt(kRingSpread) + kRingBaseDelay;
					g_thePhone.rings = RandomInt(3) + 3;
				}
			}
			else
				g_thePhone.delay--;
		}
		else
			g_thePhone.nextRing--;
	}
	// handle also the wind chimes (if they are present)

	if (g_numChimes > 0)
	{
		if (g_theChimes.nextRing == 0)
		{
			if (RandomInt(2) == 0)
				PlayPrioritySound(kChime1Sound, kChime1Priority);
			else
				PlayPrioritySound(kChime2Sound, kChime2Priority);

			delayTime = kChimeDelay / g_numChimes;
			if (delayTime < 2)
				delayTime = 2;

			g_theChimes.nextRing = RandomInt(delayTime) + 1;
		}
		else
			g_theChimes.nextRing--;
	}
}

//--------------------------------------------------------------  StrikeChime

void StrikeChime (void)
{
	g_theChimes.nextRing = 0;
}
