//============================================================================
//----------------------------------------------------------------------------
//									Input.c
//----------------------------------------------------------------------------
//============================================================================


//#include <ToolUtils.h>
#include "Macintosh.h"
#include "DialogUtils.h"
#include "Externs.h"
#include "MainWindow.h"
#include "RectUtils.h"
#include "ResourceIDs.h"


#define kNormalThrust		5
#define kHyperThrust		8
#define kHeliumLift			4
#define kEscPausePictID		1015
#define kTabPausePictID		1016
#define	kSavingGameDial		1042


#define IsKeyDown(vkey) (GetActiveWindow() == mainWindow && GetAsyncKeyState(vkey) < 0)
#define IsKeyUp(vkey) (GetActiveWindow() != mainWindow || GetAsyncKeyState(vkey) >= 0)


void LogDemoKey (Byte);
void DoCommandKey (void);
void DoPause (void);
void DoBatteryEngaged (gliderPtr);
void DoHeliumEngaged (gliderPtr);
Boolean QuerySaveGame (void);


demoPtr		demoData;
KeyMap		theKeys;
DialogPtr	saveDial;
SInt16		demoIndex, batteryFrame;
Boolean		isEscPauseKey, paused, batteryWasEngaged;

extern	SInt32		gameFrame;
extern	SInt16		otherPlayerEscaped;
extern	Boolean		quitting, playing, onePlayerLeft, twoPlayerGame, demoGoing;


//==============================================================  Functions
//--------------------------------------------------------------  LogDemoKey

void LogDemoKey (Byte keyIs)
{
	demoData[demoIndex].frame = gameFrame;
	demoData[demoIndex].key = keyIs;
	demoIndex++;
}

//--------------------------------------------------------------  DoCommandKey

void DoCommandKey (void)
{
	if (IsKeyDown('Q'))
	{
		playing = false;
		paused = false;
		if ((!twoPlayerGame) && (!demoGoing))
		{
			if (QuerySaveGame())
				SaveGame2();		// New save game.
		}
	}
	else if ((IsKeyDown('S')) && (!twoPlayerGame) && (!demoGoing))
	{
		RefreshScoreboard(kSavingTitleMode);
		SaveGame2();				// New save game.
		//HideCursor();
		// TODO: is this bitblt necessary? game works better without it
		//CopyRectWorkToMain(&workSrcRect);
		RefreshScoreboard(kNormalTitleMode);
	}
}

//--------------------------------------------------------------  DoPause

void DoPause (void)
{
	Rect		bounds;
	MSG			msg;
	HDC			mainWindowDC;

	//SetPort((GrafPtr)mainWindow);
	QSetRect(&bounds, 0, 0, 214, 54);
	CenterRectInRect(&bounds, &houseRect);
	mainWindowDC = GetMainWindowDC();
	if (isEscPauseKey)
		LoadScaledGraphic(mainWindowDC, kEscPausePictID, &bounds);
	else
		LoadScaledGraphic(mainWindowDC, kTabPausePictID, &bounds);
	ReleaseMainWindowDC(mainWindowDC);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		// continue once the pause key is up
		if ((isEscPauseKey && IsKeyUp(VK_ESCAPE)) ||
				(!isEscPauseKey && IsKeyUp(VK_TAB)))
			break;
	}
	if (msg.message == WM_QUIT)
	{
		PostQuitMessage((int)msg.wParam);
		quitting = true;
		return;
	}

	paused = true;
	while (paused && GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		// continue once the pause key is down again
		if ((isEscPauseKey && IsKeyDown(VK_ESCAPE)) ||
				(!isEscPauseKey && IsKeyDown(VK_TAB)))
			paused = false;
		else if (IsKeyDown(VK_CONTROL))
			DoCommandKey();
	}
	if (msg.message == WM_QUIT)
	{		
		PostQuitMessage((int)msg.wParam);
		paused = false;
		quitting = true;
		return;
	}

	mainWindowDC = GetMainWindowDC();
	Mac_CopyBits(workSrcMap, mainWindowDC,
			&bounds, &bounds, srcCopy, nil);
	ReleaseMainWindowDC(mainWindowDC);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		// continue once the pause key is up
		if ((isEscPauseKey && IsKeyUp(VK_ESCAPE)) ||
				(!isEscPauseKey && IsKeyUp(VK_TAB)))
			break;
	}
	if (msg.message == WM_QUIT)
	{
		PostQuitMessage((int)msg.wParam);
		quitting = true;
		return;
	}
}

//--------------------------------------------------------------  DoBatteryEngaged

void DoBatteryEngaged (gliderPtr thisGlider)
{
	if (thisGlider->facing == kFaceLeft)
	{
		if (thisGlider->tipped)
			thisGlider->hVel += kHyperThrust;
		else
			thisGlider->hVel -= kHyperThrust;
	}
	else
	{
		if (thisGlider->tipped)
			thisGlider->hVel -= kHyperThrust;
		else
			thisGlider->hVel += kHyperThrust;
	}

	batteryTotal--;

	if (batteryTotal == 0)
	{
		QuickBatteryRefresh(false);
		PlayPrioritySound(kFizzleSound, kFizzlePriority);
	}
	else
	{
		if (!batteryWasEngaged)
			batteryFrame = 0;
		if (batteryFrame == 0)
			PlayPrioritySound(kThrustSound, kThrustPriority);
		batteryFrame++;
		if (batteryFrame >= 4)
			batteryFrame = 0;
		batteryWasEngaged = true;
	}
}

//--------------------------------------------------------------  DoHeliumEngaged

void DoHeliumEngaged (gliderPtr thisGlider)
{
	thisGlider->vDesiredVel = -kHeliumLift;
	batteryTotal++;

	if (batteryTotal == 0)
	{
		QuickBatteryRefresh(false);
		PlayPrioritySound(kFizzleSound, kFizzlePriority);
		batteryWasEngaged = false;
	}
	else
	{
		if (!batteryWasEngaged)
			batteryFrame = 0;
		if (batteryFrame == 0)
			PlayPrioritySound(kHissSound, kHissPriority);
		batteryFrame++;
		if (batteryFrame >= 4)
			batteryFrame = 0;
		batteryWasEngaged = true;
	}
}

//--------------------------------------------------------------  GetDemoInput

void GetDemoInput (gliderPtr thisGlider)
{
	if (thisGlider->which == kPlayer1)
	{
#if BUILD_ARCADE_VERSION

		if ((IsKeyDown(thisGlider->leftKey)) ||
				(IsKeyDown(thisGlider->rightKey)) ||
				(IsKeyDown(thisGlider->battKey)) ||
				(IsKeyDown(thisGlider->bandKey)))
		{
			playing = false;
			paused = false;
		}

#else

		if (IsKeyDown(VK_CONTROL))
			DoCommandKey();

#endif
	}

	if (thisGlider->mode == kGliderBurning)
	{
		if (thisGlider->facing == kFaceLeft)
			thisGlider->hDesiredVel -= kNormalThrust;
		else
			thisGlider->hDesiredVel += kNormalThrust;
	}
	else
	{
		thisGlider->heldLeft = false;
		thisGlider->heldRight = false;
		thisGlider->tipped = false;

		if (gameFrame == demoData[demoIndex].frame)
		{
			switch (demoData[demoIndex].key)
			{
				case 0:		// left key
				thisGlider->hDesiredVel += kNormalThrust;
				thisGlider->tipped = (thisGlider->facing == kFaceLeft);
				thisGlider->heldRight = true;
				thisGlider->fireHeld = false;
				break;

				case 1:		// right key
				thisGlider->hDesiredVel -= kNormalThrust;
				thisGlider->tipped = (thisGlider->facing == kFaceRight);
				thisGlider->heldLeft = true;
				thisGlider->fireHeld = false;
				break;

				case 2:		// battery key
				if (batteryTotal > 0)
					DoBatteryEngaged(thisGlider);
				else
					DoHeliumEngaged(thisGlider);
				thisGlider->fireHeld = false;
				break;

				case 3:		// rubber band key
				if (!thisGlider->fireHeld)
				{
					if (AddBand(thisGlider, thisGlider->dest.left + 24,
							thisGlider->dest.top + 10, thisGlider->facing))
					{
						bandsTotal--;
						if (bandsTotal <= 0)
							QuickBandsRefresh(false);

						thisGlider->fireHeld = true;
					}
				}
				break;
			}

			demoIndex++;
		}
		else
			thisGlider->fireHeld = false;

		if ((isEscPauseKey && IsKeyDown(VK_ESCAPE)) ||
				(!isEscPauseKey && IsKeyDown(VK_TAB)))
		{
			DoPause();
		}
	}
}

//--------------------------------------------------------------  GetInput

void GetInput (gliderPtr thisGlider)
{
	if (thisGlider->which == kPlayer1)
	{
		if (IsKeyDown(VK_CONTROL))
			DoCommandKey();
	}

	if (thisGlider->mode == kGliderBurning)
	{
		if (thisGlider->facing == kFaceLeft)
			thisGlider->hDesiredVel -= kNormalThrust;
		else
			thisGlider->hDesiredVel += kNormalThrust;
	}
	else
	{
		thisGlider->heldLeft = false;
		thisGlider->heldRight = false;
		if (IsKeyDown(thisGlider->rightKey))			// right key
		{
		#ifdef CREATEDEMODATA
			LogDemoKey(0);
		#endif
			if (IsKeyDown(thisGlider->leftKey))
			{
				ToggleGliderFacing(thisGlider);
				thisGlider->heldLeft = true;
			}
			else
			{
				thisGlider->hDesiredVel += kNormalThrust;
				thisGlider->tipped = (thisGlider->facing == kFaceLeft);
				thisGlider->heldRight = true;
			}
		}
		else if (IsKeyDown(thisGlider->leftKey))		// left key
		{
		#ifdef CREATEDEMODATA
			LogDemoKey(1);
		#endif
			thisGlider->hDesiredVel -= kNormalThrust;
			thisGlider->tipped = (thisGlider->facing == kFaceRight);
			thisGlider->heldLeft = true;
		}
		else
			thisGlider->tipped = false;

		if ((IsKeyDown(thisGlider->battKey)) && (batteryTotal != 0) &&
				(thisGlider->mode == kGliderNormal))
		{
		#ifdef CREATEDEMODATA
			LogDemoKey(2);
		#endif
			if (batteryTotal > 0)
				DoBatteryEngaged(thisGlider);
			else
				DoHeliumEngaged(thisGlider);
		}
		else
			batteryWasEngaged = false;

		if ((IsKeyDown(thisGlider->bandKey)) && (bandsTotal > 0) &&
				(thisGlider->mode == kGliderNormal))
		{
		#ifdef CREATEDEMODATA
			LogDemoKey(3);
		#endif
			if (!thisGlider->fireHeld)
			{
				if (AddBand(thisGlider, thisGlider->dest.left + 24,
						thisGlider->dest.top + 10, thisGlider->facing))
				{
					bandsTotal--;
					if (bandsTotal <= 0)
						QuickBandsRefresh(false);

					thisGlider->fireHeld = true;
				}
			}
		}
		else
			thisGlider->fireHeld = false;

		if ((otherPlayerEscaped != kNoOneEscaped) &&
				(IsKeyDown(VK_DELETE)) &&
				(thisGlider->which) && (!onePlayerLeft))
		{
			ForceKillGlider();
		}

		if ((isEscPauseKey && IsKeyDown(VK_ESCAPE)) ||
				(!isEscPauseKey && IsKeyDown(VK_TAB)))
		{
			DoPause();
		}
	}
}

//--------------------------------------------------------------  QuerySaveGame

Boolean QuerySaveGame (void)
{
	#define			kYesSaveGameButton	IDYES
	SInt16			hitWhat;
	DialogParams	params = { 0 };

	params.hwndParent = mainWindow;
	hitWhat = Alert(kSaveGameAlert, &params);
	if (hitWhat == kYesSaveGameButton)
		return (true);
	else
		return (false);
}

