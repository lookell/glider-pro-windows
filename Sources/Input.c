#include "Input.h"

//============================================================================
//----------------------------------------------------------------------------
//                                  Input.c
//----------------------------------------------------------------------------
//============================================================================


#include "ByteIO.h"
#include "DialogUtils.h"
#include "HouseIO.h"
#include "Interactions.h"
#include "Macintosh.h"
#include "Main.h"
#include "MainWindow.h"
#include "Modes.h"
#include "Play.h"
#include "Player.h"
#include "RectUtils.h"
#include "Render.h"
#include "ResourceIDs.h"
#include "RoomGraphics.h"
#include "RubberBands.h"
#include "SavedGames.h"
#include "Scoreboard.h"
#include "Sound.h"
#include "StructIO.h"
#include "Transit.h"
#include "Utilities.h"


#define kNormalThrust		5
#define kHyperThrust		8
#define kHeliumLift			4


#define IsKeyDown(vkey) (GetActiveWindow() == mainWindow && GetAsyncKeyState(vkey) < 0)
#define IsKeyUp(vkey) (GetActiveWindow() != mainWindow || GetAsyncKeyState(vkey) >= 0)


void LogDemoKey (Byte keyIs);
void DoCommandKey (void);
void DoPause (void);
void DoBatteryEngaged (gliderPtr thisGlider);
void DoHeliumEngaged (gliderPtr thisGlider);
Boolean QuerySaveGame (HWND ownerWindow);


demoType demoData[2000];
SInt16 demoIndex;
Boolean isEscPauseKey;
Boolean paused;

static SInt16 batteryFrame;
static Boolean batteryWasEngaged;


//==============================================================  Functions
//--------------------------------------------------------------  LogDemoKey

void LogDemoKey (Byte keyIs)
{
	if (demoIndex >= ARRAYSIZE(demoData))
	{
		return;
	}
	demoData[demoIndex].frame = gameFrame;
	demoData[demoIndex].key = keyIs;
	demoIndex++;
}

//--------------------------------------------------------------  DumpDemoData

void DumpDemoData (void)
{
	HANDLE demoFileHandle;
	byteio demoWriter;
	SInt16 i;

	demoFileHandle = CreateFile(L"demo.bin", GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (demoFileHandle == INVALID_HANDLE_VALUE)
	{
		return;
	}
	if (!byteio_init_handle_writer(&demoWriter, demoFileHandle))
	{
		RedAlert(kErrNoMemory);
	}
	for (i = 0; i < demoIndex; i++)
	{
		WriteDemoType(&demoWriter, &demoData[i]);
	}
	byteio_close(&demoWriter);
	CloseHandle(demoFileHandle);
}

//--------------------------------------------------------------  DoCommandKeyQuit

void DoCommandKeyQuit (void)
{
	playing = false;
	paused = false;
	if ((!twoPlayerGame) && (!demoGoing))
	{
		if (QuerySaveGame(mainWindow))
		{
			SaveGame2(mainWindow); // New save game.
		}
	}
}

//--------------------------------------------------------------  DoCommandKeySave

void DoCommandKeySave (void)
{
	RefreshScoreboard(kSavingTitleMode);
	SaveGame2(mainWindow); // New save game.
	CopyRectWorkToMain(&workSrcRect);
	RefreshScoreboard(kNormalTitleMode);
}

//--------------------------------------------------------------  DoCommandKey

void DoCommandKey (void)
{
	if (IsKeyDown('Q'))
	{
		DoCommandKeyQuit();
	}
	else if ((IsKeyDown('S')) && (!twoPlayerGame) && (!demoGoing))
	{
		DoCommandKeySave();
	}
}

//--------------------------------------------------------------  DoPause

void DoPause (void)
{
	Rect		bounds;
	MSG			msg;
	HDC			mainWindowDC;

	QSetRect(&bounds, 0, 0, 214, 54);
	CenterRectInRect(&bounds, &houseRect);
	mainWindowDC = GetMainWindowDC();
	if (isEscPauseKey)
		LoadScaledGraphic(mainWindowDC, g_theHouseFile, kEscPausePictID, &bounds);
	else
		LoadScaledGraphic(mainWindowDC, g_theHouseFile, kTabPausePictID, &bounds);
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

//
// NOTE: This function does not perfectly reproduce the input that can be
// received in normal gameplay. It only allows for one keystroke per frame.
//

//--------------------------------------------------------------  GetDemoInput

void GetDemoInput (gliderPtr thisGlider)
{
	SInt32 demoFrame;
	Byte demoKey;

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
		thisGlider->tipped = false;

		demoFrame = -1;
		demoKey = 255;
		if (demoIndex < ARRAYSIZE(demoData))
		{
			demoFrame = demoData[demoIndex].frame;
			demoKey = demoData[demoIndex].key;
		}
		if (gameFrame == demoFrame)
		{
			switch (demoKey)
			{
				case 0: // right key
				thisGlider->hDesiredVel += kNormalThrust;
				thisGlider->tipped = (thisGlider->facing == kFaceLeft);
				thisGlider->heldRight = true;
				thisGlider->fireHeld = false;
				break;

				case 1: // left key
				thisGlider->hDesiredVel -= kNormalThrust;
				thisGlider->tipped = (thisGlider->facing == kFaceRight);
				thisGlider->heldLeft = true;
				thisGlider->fireHeld = false;
				break;

				case 2: // battery key
				if (batteryTotal > 0)
					DoBatteryEngaged(thisGlider);
				else
					DoHeliumEngaged(thisGlider);
				thisGlider->fireHeld = false;
				break;

				case 3: // rubber band key
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
			if (CREATEDEMODATA)
				LogDemoKey(0);

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
			if (CREATEDEMODATA)
				LogDemoKey(1);

			thisGlider->hDesiredVel -= kNormalThrust;
			thisGlider->tipped = (thisGlider->facing == kFaceRight);
			thisGlider->heldLeft = true;
		}
		else
			thisGlider->tipped = false;

		if ((IsKeyDown(thisGlider->battKey)) && (batteryTotal != 0) &&
				(thisGlider->mode == kGliderNormal))
		{
			if (CREATEDEMODATA)
				LogDemoKey(2);

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
			if (CREATEDEMODATA)
				LogDemoKey(3);

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

Boolean QuerySaveGame (HWND ownerWindow)
{
	#define			kYesSaveGameButton	IDYES
	SInt16			hitWhat;

	hitWhat = Alert(kSaveGameAlert, ownerWindow, NULL);
	if (hitWhat == kYesSaveGameButton)
		return (true);
	else
		return (false);
}

