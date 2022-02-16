//============================================================================
//----------------------------------------------------------------------------
//                                  Input.c
//----------------------------------------------------------------------------
//============================================================================

#include "Input.h"

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

#define kNormalThrust       5
#define kHyperThrust        8
#define kHeliumLift         4

#define IsKeyDown(vkey) (GetActiveWindow() == g_mainWindow && GetAsyncKeyState(vkey) < 0)
#define IsKeyUp(vkey) (GetActiveWindow() != g_mainWindow || GetAsyncKeyState(vkey) >= 0)

void LogDemoKey (Byte keyIs);
void DoCommandKey (HWND ownerWindow);
void DoPause (void);
void DoBatteryEngaged (gliderPtr thisGlider);
void DoHeliumEngaged (gliderPtr thisGlider);
Boolean QuerySaveGame (HWND ownerWindow);

demoType g_demoData[2000];
SInt16 g_demoIndex;
Boolean g_isEscPauseKey;
Boolean g_paused;

static SInt16 g_batteryFrame;
static Boolean g_batteryWasEngaged;

//==============================================================  Functions
//--------------------------------------------------------------  LogDemoKey

void LogDemoKey (Byte keyIs)
{
	if (g_demoIndex >= ARRAYSIZE(g_demoData))
	{
		return;
	}
	g_demoData[g_demoIndex].frame = g_gameFrame;
	g_demoData[g_demoIndex].key = keyIs;
	g_demoIndex++;
}

//--------------------------------------------------------------  DumpDemoData

void DumpDemoData (void)
{
	HANDLE demoFileHandle;
	byteio *demoWriter;
	SInt16 i;

	demoFileHandle = CreateFile(L"demo.bin", GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (demoFileHandle == INVALID_HANDLE_VALUE)
	{
		return;
	}
	demoWriter = byteio_init_handle_writer(demoFileHandle);
	if (demoWriter == NULL)
	{
		RedAlert(kErrNoMemory);
	}
	for (i = 0; i < g_demoIndex; i++)
	{
		WriteDemoType(demoWriter, &g_demoData[i]);
	}
	byteio_close(demoWriter);
	CloseHandle(demoFileHandle);
}

//--------------------------------------------------------------  DoCommandKeyQuit

void DoCommandKeyQuit (HWND ownerWindow)
{
	g_playing = false;
	g_paused = false;
	if ((!g_twoPlayerGame) && (!g_demoGoing))
	{
		if (QuerySaveGame(ownerWindow))
		{
			SaveGame2(ownerWindow); // New save game.
		}
	}
}

//--------------------------------------------------------------  DoCommandKeySave

void DoCommandKeySave (HWND ownerWindow)
{
	RefreshScoreboard(kSavingTitleMode);
	SaveGame2(ownerWindow); // New save game.
	CopyRectWorkToMain(&g_workSrcRect);
	RefreshScoreboard(kNormalTitleMode);
}

//--------------------------------------------------------------  DoCommandKey

void DoCommandKey (HWND ownerWindow)
{
	if (IsKeyDown('Q'))
	{
		DoCommandKeyQuit(ownerWindow);
	}
	else if ((IsKeyDown('S')) && (!g_twoPlayerGame) && (!g_demoGoing))
	{
		DoCommandKeySave(ownerWindow);
	}
}

//--------------------------------------------------------------  DoPause

void DoPause (void)
{
	Rect bounds;
	MSG msg;
	HDC mainWindowDC;

	QSetRect(&bounds, 0, 0, 214, 54);
	CenterRectInRect(&bounds, &g_houseRect);
	mainWindowDC = GetMainWindowDC(g_mainWindow);
	if (g_isEscPauseKey)
		LoadScaledGraphic(mainWindowDC, g_theHouseFile, kEscPausePictID, &bounds);
	else
		LoadScaledGraphic(mainWindowDC, g_theHouseFile, kTabPausePictID, &bounds);
	ReleaseMainWindowDC(g_mainWindow, mainWindowDC);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		// continue once the pause key is up
		if ((g_isEscPauseKey && IsKeyUp(VK_ESCAPE)) ||
				(!g_isEscPauseKey && IsKeyUp(VK_TAB)))
			break;
	}
	if (msg.message == WM_QUIT)
	{
		PostQuitMessage((int)msg.wParam);
		g_quitting = true;
		return;
	}

	g_paused = true;
	while (g_paused && GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		// continue once the pause key is down again
		if ((g_isEscPauseKey && IsKeyDown(VK_ESCAPE)) ||
				(!g_isEscPauseKey && IsKeyDown(VK_TAB)))
			g_paused = false;
		else if (IsKeyDown(VK_CONTROL))
			DoCommandKey(g_mainWindow);
	}
	if (msg.message == WM_QUIT)
	{
		PostQuitMessage((int)msg.wParam);
		g_paused = false;
		g_quitting = true;
		return;
	}

	mainWindowDC = GetMainWindowDC(g_mainWindow);
	Mac_CopyBits(g_workSrcMap, mainWindowDC,
			&bounds, &bounds, srcCopy, nil);
	ReleaseMainWindowDC(g_mainWindow, mainWindowDC);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		// continue once the pause key is up
		if ((g_isEscPauseKey && IsKeyUp(VK_ESCAPE)) ||
				(!g_isEscPauseKey && IsKeyUp(VK_TAB)))
			break;
	}
	if (msg.message == WM_QUIT)
	{
		PostQuitMessage((int)msg.wParam);
		g_quitting = true;
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

	g_batteryTotal--;

	if (g_batteryTotal == 0)
	{
		QuickBatteryRefresh(false);
		PlayPrioritySound(kFizzleSound, kFizzlePriority);
	}
	else
	{
		if (!g_batteryWasEngaged)
			g_batteryFrame = 0;
		if (g_batteryFrame == 0)
			PlayPrioritySound(kThrustSound, kThrustPriority);
		g_batteryFrame++;
		if (g_batteryFrame >= 4)
			g_batteryFrame = 0;
		g_batteryWasEngaged = true;
	}
}

//--------------------------------------------------------------  DoHeliumEngaged

void DoHeliumEngaged (gliderPtr thisGlider)
{
	thisGlider->vDesiredVel = -kHeliumLift;
	g_batteryTotal++;

	if (g_batteryTotal == 0)
	{
		QuickBatteryRefresh(false);
		PlayPrioritySound(kFizzleSound, kFizzlePriority);
		g_batteryWasEngaged = false;
	}
	else
	{
		if (!g_batteryWasEngaged)
			g_batteryFrame = 0;
		if (g_batteryFrame == 0)
			PlayPrioritySound(kHissSound, kHissPriority);
		g_batteryFrame++;
		if (g_batteryFrame >= 4)
			g_batteryFrame = 0;
		g_batteryWasEngaged = true;
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
			DoCommandKey(g_mainWindow);
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
		if (g_demoIndex < ARRAYSIZE(g_demoData))
		{
			demoFrame = g_demoData[g_demoIndex].frame;
			demoKey = g_demoData[g_demoIndex].key;
		}
		if (g_gameFrame == demoFrame)
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
				if (g_batteryTotal > 0)
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
						g_bandsTotal--;
						if (g_bandsTotal <= 0)
							QuickBandsRefresh(false);

						thisGlider->fireHeld = true;
					}
				}
				break;
			}

			g_demoIndex++;
		}
		else
			thisGlider->fireHeld = false;

		if ((g_isEscPauseKey && IsKeyDown(VK_ESCAPE)) ||
				(!g_isEscPauseKey && IsKeyDown(VK_TAB)))
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
			DoCommandKey(g_mainWindow);
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
		if (IsKeyDown(thisGlider->rightKey))  // right key
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
		else if (IsKeyDown(thisGlider->leftKey))  // left key
		{
			if (CREATEDEMODATA)
				LogDemoKey(1);

			thisGlider->hDesiredVel -= kNormalThrust;
			thisGlider->tipped = (thisGlider->facing == kFaceRight);
			thisGlider->heldLeft = true;
		}
		else
			thisGlider->tipped = false;

		if ((IsKeyDown(thisGlider->battKey)) && (g_batteryTotal != 0) &&
				(thisGlider->mode == kGliderNormal))
		{
			if (CREATEDEMODATA)
				LogDemoKey(2);

			if (g_batteryTotal > 0)
				DoBatteryEngaged(thisGlider);
			else
				DoHeliumEngaged(thisGlider);
		}
		else
			g_batteryWasEngaged = false;

		if ((IsKeyDown(thisGlider->bandKey)) && (g_bandsTotal > 0) &&
				(thisGlider->mode == kGliderNormal))
		{
			if (CREATEDEMODATA)
				LogDemoKey(3);

			if (!thisGlider->fireHeld)
			{
				if (AddBand(thisGlider, thisGlider->dest.left + 24,
						thisGlider->dest.top + 10, thisGlider->facing))
				{
					g_bandsTotal--;
					if (g_bandsTotal <= 0)
						QuickBandsRefresh(false);

					thisGlider->fireHeld = true;
				}
			}
		}
		else
			thisGlider->fireHeld = false;

		if ((g_otherPlayerEscaped != kNoOneEscaped) &&
				(IsKeyDown(VK_BACK) || IsKeyDown(VK_DELETE)) &&
				(thisGlider->which) && (!g_onePlayerLeft))
		{
			ForceKillGlider();
		}

		if ((g_isEscPauseKey && IsKeyDown(VK_ESCAPE)) ||
				(!g_isEscPauseKey && IsKeyDown(VK_TAB)))
		{
			DoPause();
		}
	}
}

//--------------------------------------------------------------  QuerySaveGame

Boolean QuerySaveGame (HWND ownerWindow)
{
	SInt16 hitWhat;

	hitWhat = Alert(kSaveGameAlert, ownerWindow, NULL);
	if (hitWhat == IDYES)
		return (true);
	else
		return (false);
}
