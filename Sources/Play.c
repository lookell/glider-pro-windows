#define GP_USE_WINAPI_H

#include "Play.h"

//============================================================================
//----------------------------------------------------------------------------
//                                   Play.c
//----------------------------------------------------------------------------
//============================================================================


#include "AnimCursor.h"
#include "Banner.h"
#include "DynamicMaps.h"
#include "Dynamics.h"
#include "Environ.h"
#include "Events.h"
#include "GameOver.h"
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


#define kInitialGliders			2
#define kRingDelay				90
#define kRingSpread				25000	// 25000
#define kRingBaseDelay			5000	// 5000
#define kChimeDelay				180


typedef struct phoneType
{
	SInt16 nextRing;
	SInt16 rings;
	SInt16 delay;
} phoneType, *phonePtr;


void InitGlider (gliderPtr thisGlider, SInt16 mode);
void SetHouseToFirstRoom (void);
void SetHouseToSavedRoom (void);
void PlayGame (void);
void HandleRoomVisitation (void);
void SetObjectsToDefaults (void);
void InitTelephone (void);
void HandleTelephone (void);


Rect glidSrcRect;
Rect justRoomsRect;
HDC glidSrcMap;
HDC glid2SrcMap;
HDC glidMaskMap;
SInt32 gameFrame;
SInt16 batteryTotal;
SInt16 bandsTotal;
SInt16 foilTotal;
SInt16 mortals;
Boolean playing;
Boolean evenFrame;
Boolean twoPlayerGame;
Boolean showFoil;
Boolean demoGoing;
Boolean playerSuicide;
Boolean phoneBitSet;
Boolean tvOn;

static phoneType thePhone;
static phoneType theChimes;


//==============================================================  Functions
//--------------------------------------------------------------  NewGame

void NewGame (HWND ownerWindow, SInt16 mode)
{
	Rect		tempRect;
	OSErr		theErr;
	Boolean		wasPlayMusicPref;
	HDC			mainWindowDC;

	TIMECAPS timeCaps;
	MMRESULT mmResult;

	AdjustScoreboardHeight();
	gameOver = false;
	theMode = kPlayMode;
	if (isPlayMusicGame)
	{
		if (!isMusicOn)
		{
			theErr = StartMusic();
			if (theErr != noErr)
			{
				YellowAlert(ownerWindow, kYellowNoMusic, theErr);
				failedMusic = true;
			}
		}
		SetMusicalMode(kPlayGameScoreMode);
	}
	else
	{
		if (isMusicOn)
			StopTheMusic();
	}
	if (mode != kResumeGameMode)
		SetObjectsToDefaults();
	//HideCursor();
	if (mode == kResumeGameMode)
		SetHouseToSavedRoom();
	else if (mode == kNewGameMode)
		SetHouseToFirstRoom();
	DetermineRoomOpenings();
	NilSavedMaps();

	gameFrame = 0L;
	numBands = 0;
	demoIndex = 0;
	saidFollow = 0;
	otherPlayerEscaped = kNoOneEscaped;
	onePlayerLeft = false;
	playerSuicide = false;

	if (twoPlayerGame)					// initialize glider(s)
	{
		InitGlider(&theGlider, kNewGameMode);
		InitGlider(&theGlider2, kNewGameMode);
		LoadGraphic(glidSrcMap, kGliderPictID);
		LoadGraphic(glid2SrcMap, kGlider2PictID);
	}
	else
	{
		InitGlider(&theGlider, mode);
		LoadGraphic(glidSrcMap, kGliderPictID);
		LoadGraphic(glid2SrcMap, kGliderFoilPictID);
	}

#if !BUILD_ARCADE_VERSION
//	HideMenuBarOld();		// TEMP
#endif

	tempRect = thisMac.screen;          // paint strip on screen black
	tempRect.top = tempRect.bottom - 20;	// thisMac.menuHigh
	mainWindowDC = GetMainWindowDC();
	Mac_PaintRect(mainWindowDC, &tempRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
	ReleaseMainWindowDC(mainWindowDC);

#ifdef COMPILEQT
	if ((thisMac.hasQT) && (hasMovie))
	{
		SetMovieGWorld(theMovie, (CGrafPtr)mainWindow, nil);
	}
#endif

	Mac_PaintRect(workSrcMap, &workSrcRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
	DissolveScreenOn(&workSrcRect);
	SetMenu(mainWindow, NULL);
	UpdateWindow(mainWindow);

//	DebugStr("\pIf screen isn't black, exit to shell.");	// TEMP TEMP TEMP

	DrawLocale();
	RefreshScoreboard(kNormalTitleMode);
	DissolveScreenOn(&justRoomsRect);
	if (mode == kNewGameMode)
	{
		BringUpBanner();
		DumpScreenOn(&justRoomsRect);
	}
	else if (mode == kResumeGameMode)
	{
		DisplayStarsRemaining();
		DumpScreenOn(&justRoomsRect);
	}
	else
	{
		DumpScreenOn(&justRoomsRect);
	}

	InitGarbageRects();
	StartGliderFadingIn(&theGlider);
	if (twoPlayerGame)
	{
		StartGliderFadingIn(&theGlider2);
		TagGliderIdle(&theGlider2);
		theGlider2.dontDraw = true;
	}
	InitTelephone();
	wasPlayMusicPref = isPlayMusicGame;

#ifdef CREATEDEMODATA
	MessageBeep(MB_ICONINFORMATION);
#endif

#ifdef COMPILEQT
	if ((thisMac.hasQT) && (hasMovie) && (tvInRoom))
	{
		SetMovieActive(theMovie, true);
		if (tvOn)
		{
			StartMovie(theMovie);
			MoviesTask(theMovie, 0);
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

	playing = true;		// everything before this line is game set-up
	PlayGame();			// everything following is after a game has ended

	if (mmResult == MMSYSERR_NOERROR)
	{
		timeEndPeriod(timeCaps.wPeriodMin);
	}

#ifdef CREATEDEMODATA
	DumpDemoData();
#endif

	isPlayMusicGame = wasPlayMusicPref;
	ZeroMirrorRegion();

#ifdef COMPILEQT
	if ((thisMac.hasQT) && (hasMovie) && (tvInRoom))
	{
		tvInRoom = false;
		StopMovie(theMovie);
		SetMovieActive(theMovie, false);
	}
#endif

	twoPlayerGame = false;
	theMode = kSplashMode;
	InitCursor();
	if (isPlayMusicIdle)
	{
		if (!isMusicOn)
		{
			theErr = StartMusic();
			if (theErr != noErr)
			{
				YellowAlert(ownerWindow, kYellowNoMusic, theErr);
				failedMusic = true;
			}
		}
		SetMusicalMode(kPlayWholeScoreMode);
	}
	else
	{
		if (isMusicOn)
			StopTheMusic();
	}
	NilSavedMaps();
	UpdateMenus(false);

	if (!gameOver)
	{
		Mac_InvalWindowRect(mainWindow, &mainWindowRect);

		Mac_PaintRect(workSrcMap, &workSrcRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
		QSetRect(&tempRect, 0, 0, 640, 460);
		QOffsetRect(&tempRect, splashOriginH, splashOriginV);
		LoadScaledGraphic(workSrcMap, kSplash8BitPICT, &tempRect);
	}
	WaitCommandQReleased();
	demoGoing = false;
	incrementModeTime = timeGetTime() + TicksToMillis(kIdleSplashTicks);
}

//--------------------------------------------------------------  DoDemoGame

void DoDemoGame (HWND ownerWindow)
{
	SInt16		wasHouseIndex;
	Boolean		whoCares;

	wasHouseIndex = thisHouseIndex;
	whoCares = CloseHouse(ownerWindow);
	thisHouseIndex = demoHouseIndex;
	PasStringCopy(theHousesSpecs[thisHouseIndex].name, thisHouseName);
	if (OpenHouse(ownerWindow))
	{
		whoCares = ReadHouse(ownerWindow);
		demoGoing = true;
		NewGame(ownerWindow, kNewGameMode);
	}
	whoCares = CloseHouse(ownerWindow);
	thisHouseIndex = wasHouseIndex;
	PasStringCopy(theHousesSpecs[thisHouseIndex].name, thisHouseName);
	if (OpenHouse(ownerWindow))
		whoCares = ReadHouse(ownerWindow);
	incrementModeTime = timeGetTime() + TicksToMillis(kIdleSplashTicks);
}

//--------------------------------------------------------------  InitGlider

void InitGlider (gliderPtr thisGlider, SInt16 mode)
{
	WhereDoesGliderBegin(&thisGlider->dest, mode);

	if (mode == kResumeGameMode)
		numStarsRemaining = smallGame.wasStarsLeft;
	else if (mode == kNewGameMode)
		numStarsRemaining = CountStarsInHouse();

	if (mode == kResumeGameMode)
	{
		theScore = smallGame.score;
		mortals = smallGame.numGliders;
		batteryTotal = smallGame.energy;
		bandsTotal = smallGame.bands;
		foilTotal = smallGame.foil;
		thisGlider->mode = smallGame.gliderState;
		thisGlider->facing = smallGame.facing;
		showFoil = smallGame.showFoil;

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
		theScore = 0L;
		mortals = kInitialGliders;
		if (twoPlayerGame)
			mortals += kInitialGliders;
		batteryTotal = 0;
		bandsTotal = 0;
		foilTotal = 0;
		thisGlider->mode = kGliderNormal;
		thisGlider->facing = kFaceRight;
		thisGlider->src = gliderSrc[0];
		thisGlider->mask = gliderSrc[0];
		showFoil = false;
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
	SInt16		firstRoom;

	firstRoom = GetFirstRoomNumber();
	ForceThisRoom(firstRoom);
}

//--------------------------------------------------------------  SetHouseToSavedRoom

void SetHouseToSavedRoom (void)
{
	ForceThisRoom(smallGame.roomNumber);
}

//--------------------------------------------------------------  PlayGame

void PlayGame (void)
{
	while ((playing) && (!quitting))
	{
		gameFrame++;
		evenFrame = !evenFrame;

		HandleTelephone();

		if (twoPlayerGame)
		{
			HandleDynamics();
			if (!gameOver)
			{
				GetInput(&theGlider);
				GetInput(&theGlider2);
				HandleInteraction();
			}
			HandleTriggers();
			HandleBands();
			if (!gameOver)
			{
				HandleGlider(&theGlider);
				HandleGlider(&theGlider2);
			}
			if (playing)
			{
#ifdef COMPILEQT
				if ((thisMac.hasQT) && (hasMovie) && (tvInRoom) && (tvOn))
						MoviesTask(theMovie, 0);
#endif
				RenderFrame();
				HandleDynamicScoreboard();
			}
		}
		else
		{
			HandleDynamics();
			if (!gameOver)
			{
				if (demoGoing)
					GetDemoInput(&theGlider);
				else
					GetInput(&theGlider);
				HandleInteraction();
			}
			HandleTriggers();
			HandleBands();
			if (!gameOver)
				HandleGlider(&theGlider);
			if (playing)
			{
#ifdef COMPILEQT
				if ((thisMac.hasQT) && (hasMovie) && (tvInRoom) && (tvOn))
						MoviesTask(theMovie, 0);
#endif
				RenderFrame();
				HandleDynamicScoreboard();
			}
		}

		if (gameOver)
		{
			countDown--;
			if (countDown <= 0)
			{
#if BUILD_ARCADE_VERSION
				HDC			mainWindowDC;
#endif

				HideGlider(&theGlider);
				RefreshScoreboard(kNormalTitleMode);

#if BUILD_ARCADE_VERSION
			// Need to paint over the scoreboard black.

				Mac_PaintRect(boardSrcMap, &boardSrcRect, GetStockObject(BLACK_BRUSH));

				mainWindowDC = GetMainWindowDC();
				Mac_CopyBits(boardSrcMap, mainWindowDC,
						&boardSrcRect, &boardDestRect, srcCopy, 0L);
				ReleaseMainWindowDC(mainWindowDC);

				{
					Rect		bounds;
					HBITMAP		thePicture;
					BITMAP		bmInfo;
					SInt16		hOffset;

					if (boardSrcRect.right >= 640)
						hOffset = (RectWide(&boardSrcRect) - kMaxViewWidth) / 2;
					else
						hOffset = -576;
					thePicture = Gp_LoadImage(kScoreboardPictID);
					if (!thePicture)
						RedAlert(kErrFailedGraphicLoad);
					GetObject(thePicture, sizeof(bmInfo), &bmInfo);
					QSetRect(&bounds, 0, 0, (SInt16)bmInfo.bmWidth, (SInt16)bmInfo.bmHeight);
					QOffsetRect(&bounds, hOffset, 0);
					Mac_DrawPicture(boardSrcMap, thePicture, &bounds);
					DeleteObject(thePicture);
				}
#else
//				ShowMenuBarOld();	// TEMP
				SetMenu(mainWindow, theMenuBar);
				UpdateWindow(mainWindow);
#endif

				if (mortals < 0)
					DoDiedGameOver();
				else
					DoGameOver();
			}
		}
	}

#if BUILD_ARCADE_VERSION
	{
		HDC			mainWindowDC;

		Mac_PaintRect(boardSrcMap, &boardSrcRect, GetStockObject(BLACK_BRUSH));

		mainWindowDC = GetMainWindowDC();
		Mac_CopyBits(boardSrcMap, mainWindowDC,
				&boardSrcRect, &boardDestRect, srcCopy, 0L);
		ReleaseMainWindowDC(mainWindowDC);
	}

	{
		Rect		bounds;
		HBITMAP		thePicture;
		BITMAP		bmInfo;
		SInt16		hOffset;

		if (boardSrcRect.right >= 640)
			hOffset = (RectWide(&boardSrcRect) - kMaxViewWidth) / 2;
		else
			hOffset = -576;
		thePicture = Gp_LoadImage(kScoreboardPictID);
		if (!thePicture)
			RedAlert(kErrFailedGraphicLoad);
		GetObject(thePicture, sizeof(bmInfo), &bmInfo);
		QSetRect(&bounds, 0, 0, (SInt16)bmInfo.bmWidth, (SInt16)bmInfo.bmHeight);
		QOffsetRect(&bounds, hOffset, 0);
		Mac_DrawPicture(boardSrcMap, thePicture, &bounds);
		DeleteObject(thePicture);
	}

#else

//	ShowMenuBarOld();	// TEMP
	SetMenu(mainWindow, theMenuBar);
	UpdateWindow(mainWindow); 

#endif
}

//--------------------------------------------------------------  SetObjectsToDefaults

void SetObjectsToDefaults (void)
{
	SInt16		numRooms;
	SInt16		r, i;
	Boolean		initState;

	numRooms = thisHouse.nRooms;

	for (r = 0; r < numRooms; r++)
	{
		thisHouse.rooms[r].visited = false;
		for (i = 0; i < kMaxRoomObs; i++)
		{
			switch (thisHouse.rooms[r].objects[i].what)
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
				thisHouse.rooms[r].objects[i].data.a.state =
					thisHouse.rooms[r].objects[i].data.a.initial;
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
				thisHouse.rooms[r].objects[i].data.c.state =
					thisHouse.rooms[r].objects[i].data.c.initial;
				break;

				case kDeluxeTrans:
				initState = (thisHouse.rooms[r].objects[i].data.d.wide & 0xF0) >> 4;
				thisHouse.rooms[r].objects[i].data.d.wide &= 0xF0;
				thisHouse.rooms[r].objects[i].data.d.wide += initState;
				break;

				case kCeilingLight:
				case kLightBulb:
				case kTableLamp:
				case kHipLamp:
				case kDecoLamp:
				case kFlourescent:
				case kTrackLight:
				case kInvisLight:
				thisHouse.rooms[r].objects[i].data.f.state =
					thisHouse.rooms[r].objects[i].data.f.initial;
				break;

				case kStereo:
				thisHouse.rooms[r].objects[i].data.g.state = isPlayMusicGame;
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
				thisHouse.rooms[r].objects[i].data.g.state =
					thisHouse.rooms[r].objects[i].data.g.initial;
				break;

				case kBalloon:
				case kCopterLf:
				case kCopterRt:
				case kDartLf:
				case kDartRt:
				case kBall:
				case kDrip:
				case kFish:
				thisHouse.rooms[r].objects[i].data.h.state =
					thisHouse.rooms[r].objects[i].data.h.initial;
				break;

			}
		}
	}
}

//--------------------------------------------------------------  HideGlider

void HideGlider (const gliderType *thisGlider)
{
	Rect		tempRect;

	tempRect = thisGlider->whole;
	QOffsetRect(&tempRect, playOriginH, playOriginV);
	CopyRectWorkToMain(&tempRect);

	if (hasMirror)
	{
		QOffsetRect(&tempRect, -20, -16);
		CopyRectWorkToMain(&tempRect);
	}

	tempRect = thisGlider->wholeShadow;
	QOffsetRect(&tempRect, playOriginH, playOriginV);
	CopyRectWorkToMain(&tempRect);
}

//--------------------------------------------------------------  InitTelephone

void InitTelephone (void)
{
	thePhone.nextRing = RandomInt(kRingSpread) + kRingBaseDelay;
	thePhone.rings = RandomInt(3) + 3;
	thePhone.delay = kRingDelay;

	theChimes.nextRing = RandomInt(kChimeDelay) + 1;
}

//--------------------------------------------------------------  HandleTelephone

void HandleTelephone (void)
{
	SInt16		delayTime;

	if (!phoneBitSet)
	{
		if (thePhone.nextRing == 0)
		{
			if (thePhone.delay == 0)
			{
				thePhone.delay = kRingDelay;
				PlayPrioritySound(kPhoneRingSound, kPhoneRingPriority);
				thePhone.rings--;
				if (thePhone.rings == 0)
				{
					thePhone.nextRing = RandomInt(kRingSpread) + kRingBaseDelay;
					thePhone.rings = RandomInt(3) + 3;
				}
			}
			else
				thePhone.delay--;
		}
		else
			thePhone.nextRing--;
	}
	// handle also the wind chimes (if they are present)

	if (numChimes > 0)
	{
		if (theChimes.nextRing == 0)
		{
			if (RandomInt(2) == 0)
				PlayPrioritySound(kChime1Sound, kChime1Priority);
			else
				PlayPrioritySound(kChime2Sound, kChime2Priority);

			delayTime = kChimeDelay / numChimes;
			if (delayTime < 2)
				delayTime = 2;

			theChimes.nextRing = RandomInt(delayTime) + 1;
		}
		else
			theChimes.nextRing--;
	}
}

//--------------------------------------------------------------  StrikeChime

void StrikeChime (void)
{
	theChimes.nextRing = 0;
}

//--------------------------------------------------------------  RestoreEntireGameScreen

void RestoreEntireGameScreen (void)
{
	Rect		tempRect;
	HDC			mainWindowDC;

	//HideCursor();

#if !BUILD_ARCADE_VERSION
//	HideMenuBarOld();		// TEMP
#endif

	tempRect = thisMac.screen;
	mainWindowDC = GetMainWindowDC();
	Mac_PaintRect(mainWindowDC, &tempRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
	ReleaseMainWindowDC(mainWindowDC);

	DrawLocale();
	RefreshScoreboard(kNormalTitleMode);
	DissolveScreenOn(&justRoomsRect);
}

