
//============================================================================
//----------------------------------------------------------------------------
//									 Play.c
//----------------------------------------------------------------------------
//============================================================================


//#include <Resources.h>
#include "Macintosh.h"
#include "Externs.h"
#include "Environ.h"
#include "House.h"
#include "MainWindow.h"
#include "RectUtils.h"
#include "Scoreboard.h"


#define kHouseBannerAlert		1009
#define kInitialGliders			2
#define kRingDelay				90
#define kRingSpread				25000	// 25000
#define kRingBaseDelay			5000	// 5000
#define kChimeDelay				180


typedef struct
{
	SInt16		nextRing;
	SInt16		rings;
	SInt16		delay;
} phoneType, *phonePtr;


void InitGlider (gliderPtr, SInt16);
void SetHouseToFirstRoom (void);
void SetHouseToSavedRoom (void);
void HandlePlayEvent (void);
void PlayGame (void);
void HandleRoomVisitation (void);
void SetObjectsToDefaults (void);
void InitTelephone (void);
void HandleTelephone (void);
Boolean DoesStarCodeExist (SInt16);
SInt16 GetNumStarsRemaining (SInt16, SInt16);


phoneType	thePhone, theChimes;
Rect		glidSrcRect, justRoomsRect;
HDC			glidSrcMap, glid2SrcMap;
HDC			glidMaskMap;
SInt32		gameFrame;
SInt16		batteryTotal, bandsTotal, foilTotal, mortals;
Boolean		playing, evenFrame, twoPlayerGame, showFoil, demoGoing;
Boolean		doBackground, playerSuicide, phoneBitSet, tvOn;

extern	HWND			menuWindow;
extern	HMENU			theMenuBar;
extern	houseSpecPtr	theHousesSpecs;
extern	demoPtr			demoData;
extern	gameType		smallGame;
extern	Rect			gliderSrc[kNumGliderSrcRects];
extern	Rect			boardDestRect, boardSrcRect;
extern	SInt32			incrementModeTime;
extern	SInt16			numBands, otherPlayerEscaped, demoIndex, demoHouseIndex;
extern	SInt16			splashOriginH, splashOriginV, countDown, thisHouseIndex;
extern	SInt16			numStarsRemaining, numChimes, saidFollow;
extern	Boolean			quitting, isMusicOn, gameOver, hasMirror, onePlayerLeft;
extern	Boolean			isPlayMusicIdle, failedMusic, quickerTransitions;
extern	Boolean			switchedOut;


//==============================================================  Functions
//--------------------------------------------------------------  NewGame

void NewGame (HWND ownerWindow, SInt16 mode)
{
	Rect		tempRect;
	//Size		freeBytes, growBytes;
	OSErr		theErr;
	Boolean		wasPlayMusicPref;
	HDC			mainWindowDC;

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

	//SetPort((GrafPtr)mainWindow);		// paint strip on screen black
	tempRect = thisMac.screen;
	tempRect.top = tempRect.bottom - 20;	// thisMac.menuHigh
	mainWindowDC = GetMainWindowDC();
	Mac_PaintRect(mainWindowDC, &tempRect, GetStockObject(BLACK_BRUSH));
	ReleaseMainWindowDC(mainWindowDC);

#ifdef COMPILEQT
	if ((thisMac.hasQT) && (hasMovie))
	{
		SetMovieGWorld(theMovie, (CGrafPtr)mainWindow, nil);
	}
#endif

	//SetPort((GrafPtr)workSrcMap);
	Mac_PaintRect(workSrcMap, &workSrcRect, GetStockObject(BLACK_BRUSH));
	if (quickerTransitions)
		DissBitsChunky(&workSrcRect);
	else
		DissBits(&workSrcRect);
	SetMenu(mainWindow, NULL);
	UpdateWindow(mainWindow);

//	DebugStr("\pIf screen isn't black, exit to shell.");	// TEMP TEMP TEMP

	DrawLocale();
	RefreshScoreboard(kNormalTitleMode);
	if (quickerTransitions)
		DissBitsChunky(&justRoomsRect);
	else
		DissBits(&justRoomsRect);
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

	//freeBytes = MaxMem(&growBytes);

#ifdef CREATEDEMODATA
	SysBeep(1);
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

	playing = true;		// everything before this line is game set-up
	PlayGame();			// everything following is after a game has ended

#ifdef CREATEDEMODATA
	DumpToResEditFile((Ptr)demoData, sizeof(demoType) * (long)demoIndex);
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
	//InitCursor();
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
	//SetPortWindowPort(mainWindow);
	//BlackenScoreboard();
	UpdateMenus(false);

	if (!gameOver)
	{
		Mac_InvalWindowRect(mainWindow, &mainWindowRect);

		Mac_PaintRect(workSrcMap, &workSrcRect, GetStockObject(BLACK_BRUSH));
		QSetRect(&tempRect, 0, 0, 640, 460);
		QOffsetRect(&tempRect, splashOriginH, splashOriginV);
		LoadScaledGraphic(workSrcMap, kSplash8BitPICT, &tempRect);
	}
	WaitCommandQReleased();
	demoGoing = false;
	incrementModeTime = MillisToTicks(GetTickCount()) + kIdleSplashTicks;
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
	incrementModeTime = MillisToTicks(GetTickCount()) + kIdleSplashTicks;
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

//--------------------------------------------------------------  HandlePlayEvent

void HandlePlayEvent (void)
{
	return;
#if 0
	EventRecord	theEvent;
	GrafPtr		wasPort;
	long		sleep = 2;

	if (WaitNextEvent(everyEvent, &theEvent, sleep, nil))
	{
		if ((theEvent.what == updateEvt) &&
				((WindowPtr)theEvent.message == mainWindow))
		{
			GetPort(&wasPort);
			SetPortWindowPort(mainWindow);
			BeginUpdate(mainWindow);
			CopyBits((BitMap *)*GetGWorldPixMap(workSrcMap),
					GetPortBitMapForCopyBits(GetWindowPort(mainWindow)),
					&justRoomsRect, &justRoomsRect, srcCopy, nil);
			RefreshScoreboard(kNormalTitleMode);
			EndUpdate(mainWindow);
			SetPort(wasPort);
		}
		else if ((theEvent.what == osEvt) && (theEvent.message & 0x01000000))
		{
			if (theEvent.message & 0x00000001)	// resume event
			{
				switchedOut = false;
				ToggleMusicWhilePlaying();
				HideCursor();
//				HideMenuBarOld();	// TEMP
			}
			else								// suspend event
			{
				InitCursor();
				switchedOut = true;
				ToggleMusicWhilePlaying();
//				ShowMenuBarOld();		// TEMP replace with Carbon calls
			}
		}
	}
#endif
}

//--------------------------------------------------------------  PlayGame

void PlayGame (void)
{
	MSG msg;

	while ((playing) && (!quitting))
	{
		gameFrame++;
		evenFrame = !evenFrame;

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				PostQuitMessage((int)msg.wParam);
				quitting = true;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
			break;

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
				//CGrafPtr	wasCPort;
				//GDHandle	wasWorld;
#if BUILD_ARCADE_VERSION
				HDC			mainWindowDC;
#endif

				//GetGWorld(&wasCPort, &wasWorld);

				HideGlider(&theGlider);
				RefreshScoreboard(kNormalTitleMode);

#if BUILD_ARCADE_VERSION
			// Need to paint over the scoreboard black.

				//SetGWorld(boardSrcMap, nil);
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
					thePicture = GetPicture(kScoreboardPictID);
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

				//SetGWorld(wasCPort, wasWorld);
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
		thePicture = GetPicture(kScoreboardPictID);
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

	numRooms = thisHouse->nRooms;

	for (r = 0; r < numRooms; r++)
	{
		thisHouse->rooms[r].visited = false;
		for (i = 0; i < kMaxRoomObs; i++)
		{
			switch (thisHouse->rooms[r].objects[i].what)
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
				thisHouse->rooms[r].objects[i].data.a.state =
					thisHouse->rooms[r].objects[i].data.a.initial;
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
				thisHouse->rooms[r].objects[i].data.c.state =
					thisHouse->rooms[r].objects[i].data.c.initial;
				break;

				case kDeluxeTrans:
				initState = (thisHouse->rooms[r].objects[i].data.d.wide & 0xF0) >> 4;
				thisHouse->rooms[r].objects[i].data.d.wide &= 0xF0;
				thisHouse->rooms[r].objects[i].data.d.wide += initState;
				break;

				case kCeilingLight:
				case kLightBulb:
				case kTableLamp:
				case kHipLamp:
				case kDecoLamp:
				case kFlourescent:
				case kTrackLight:
				case kInvisLight:
				thisHouse->rooms[r].objects[i].data.f.state =
					thisHouse->rooms[r].objects[i].data.f.initial;
				break;

				case kStereo:
				thisHouse->rooms[r].objects[i].data.g.state = isPlayMusicGame;
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
				thisHouse->rooms[r].objects[i].data.g.state =
					thisHouse->rooms[r].objects[i].data.g.initial;
				break;

				case kBalloon:
				case kCopterLf:
				case kCopterRt:
				case kDartLf:
				case kDartRt:
				case kBall:
				case kDrip:
				case kFish:
				thisHouse->rooms[r].objects[i].data.h.state =
					thisHouse->rooms[r].objects[i].data.h.initial;
				break;

			}
		}
	}
}

//--------------------------------------------------------------  HideGlider

void HideGlider (gliderPtr thisGlider)
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

	//SetPort((GrafPtr)mainWindow);
	tempRect = thisMac.screen;
	mainWindowDC = GetMainWindowDC();
	Mac_PaintRect(mainWindowDC, &tempRect, GetStockObject(BLACK_BRUSH));
	ReleaseMainWindowDC(mainWindowDC);

	DrawLocale();
	RefreshScoreboard(kNormalTitleMode);
	if (quickerTransitions)
		DissBitsChunky(&justRoomsRect);
	else
		DissBits(&justRoomsRect);
}

