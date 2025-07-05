//============================================================================
//----------------------------------------------------------------------------
//                               RoomGraphics.c
//----------------------------------------------------------------------------
//============================================================================

#include "RoomGraphics.h"

#include "DynamicMaps.h"
#include "Dynamics.h"
#include "Environ.h"
#include "House.h"
#include "HouseIO.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "ObjectDrawAll.h"
#include "Objects.h"
#include "Player.h"
#include "RectUtils.h"
#include "Render.h"
#include "ResourceIDs.h"
#include "ResourceLoader.h"
#include "Room.h"
#include "RubberBands.h"
#include "Sound.h"
#include "Transit.h"
#include "Triggers.h"
#include "Trip.h"
#include "Utilities.h"

void LoadGraphicSpecial (HDC hdc, SInt16 resID);
void DrawRoomBackground (SInt16 who, SInt16 where, SInt16 elevation);
void DrawFloorSupport (void);
void ReadyBackMap (void);
void RestoreWorkMap (void);
void DrawLighting (void);

Rect g_suppSrcRect;
HDC g_suppSrcMap;
Rect g_localRoomsDest[9];
Rect g_houseRect;
SInt16 g_numNeighbors;
SInt16 g_numLights;
SInt16 g_thisTiles[kNumTiles];
SInt16 g_localNumbers[9];
SInt16 g_thisBackground;
Boolean g_wardBitSet;

static Boolean g_isStructure[9];

//==============================================================  Functions
//--------------------------------------------------------------  DrawLocale

void DrawLocale (void)
{
	SInt16 i, roomV;

	ZeroFlamesAndTheLike();
	ZeroDinahs();
	KillAllBands();
	ZeroMirrorRegion();
	ZeroTriggers();
	g_numTempManholes = 0;
	FlushAnyTriggerPlaying();
	DumpTriggerSound();
	g_tvInRoom = false;
	g_tvWithMovieNumber = -1;

	roomV = g_thisHouse.rooms[g_thisRoomNumber].floor;

	for (i = 0; i < 9; i++)
	{
		g_localNumbers[i] = GetNeighborRoomNumber(&g_thisHouse, g_thisRoomNumber, i);
		g_isStructure[i] = IsRoomAStructure(g_localNumbers[i]);
	}
	ListAllLocalObjects();

	Mac_PaintRect(g_backSrcMap, &g_backSrcRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

	if (g_numNeighbors > 3)
	{
		g_numLights = GetNumberOfLights(g_localNumbers[kNorthWestRoom]);
		DrawRoomBackground(g_localNumbers[kNorthWestRoom], kNorthWestRoom, roomV + 1);
		DrawARoomsObjects(kNorthWestRoom, false);

		g_numLights = GetNumberOfLights(g_localNumbers[kNorthEastRoom]);
		DrawRoomBackground(g_localNumbers[kNorthEastRoom], kNorthEastRoom, roomV + 1);
		DrawARoomsObjects(kNorthEastRoom, false);

		g_numLights = GetNumberOfLights(g_localNumbers[kNorthRoom]);
		DrawRoomBackground(g_localNumbers[kNorthRoom], kNorthRoom, roomV + 1);
		DrawARoomsObjects(kNorthRoom, false);

		g_numLights = GetNumberOfLights(g_localNumbers[kSouthWestRoom]);
		DrawRoomBackground(g_localNumbers[kSouthWestRoom], kSouthWestRoom, roomV - 1);
		DrawARoomsObjects(kSouthWestRoom, false);

		g_numLights = GetNumberOfLights(g_localNumbers[kSouthEastRoom]);
		DrawRoomBackground(g_localNumbers[kSouthEastRoom], kSouthEastRoom, roomV - 1);
		DrawARoomsObjects(kSouthEastRoom, false);

		g_numLights = GetNumberOfLights(g_localNumbers[kSouthRoom]);
		DrawRoomBackground(g_localNumbers[kSouthRoom], kSouthRoom, roomV - 1);
		DrawARoomsObjects(kSouthRoom, false);
	}

	if (g_numNeighbors > 1)
	{
		g_numLights = GetNumberOfLights(g_localNumbers[kWestRoom]);
		DrawRoomBackground(g_localNumbers[kWestRoom], kWestRoom, roomV);
		DrawARoomsObjects(kWestRoom, false);
		DrawLighting();

		g_numLights = GetNumberOfLights(g_localNumbers[kEastRoom]);
		DrawRoomBackground(g_localNumbers[kEastRoom], kEastRoom, roomV);
		DrawARoomsObjects(kEastRoom, false);
		DrawLighting();
	}

	g_numLights = GetNumberOfLights(g_localNumbers[kCentralRoom]);
	DrawRoomBackground(g_localNumbers[kCentralRoom], kCentralRoom, roomV);
	DrawARoomsObjects(kCentralRoom, false);
	DrawLighting();

	if (g_numNeighbors > 3)
		DrawFloorSupport();
	RestoreWorkMap();
	g_shadowVisible = IsShadowVisible();
	g_takingTheStairs = false;
}

//--------------------------------------------------------------  LoadGraphicSpecial

void LoadGraphicSpecial (HDC hdc, SInt16 resID)
{
	Rect bounds;
	HBITMAP thePicture;

	thePicture = Gp_LoadImage(g_theHouseFile, resID);
	if (thePicture == NULL)
	{
		thePicture = Gp_LoadImage(g_theHouseFile, kSimpleRoom);
		if (thePicture == NULL)
		{
			RedAlert(kErrFailedGraphicLoad);
		}
	}

	GetGraphicRect(thePicture, &bounds);
	Mac_DrawPicture(hdc, thePicture, &bounds);

	DeleteObject(thePicture);
}

//--------------------------------------------------------------  DrawRoomBackground

void DrawRoomBackground (SInt16 who, SInt16 where, SInt16 elevation)
{
	Rect src, dest;
	SInt16 i, pictID;
	SInt16 tiles[kNumTiles];

	if (where == kCentralRoom)
	{
		g_thisBackground = g_thisHouse.rooms[who].background;
		for (i = 0; i < kNumTiles; i++)
			g_thisTiles[i] = g_thisHouse.rooms[who].tiles[i];
	}

	if ((g_numLights == 0) && (who != kRoomIsEmpty))
	{
		Mac_PaintRect(g_backSrcMap, &g_localRoomsDest[where], (HBRUSH)GetStockObject(BLACK_BRUSH));
		return;
	}

	if (who == kRoomIsEmpty)  // This call should be smarter than this
	{
		if (g_wardBitSet)
		{
			Mac_PaintRect(g_backSrcMap, &g_localRoomsDest[where], (HBRUSH)GetStockObject(BLACK_BRUSH));
			return;
		}

		if (elevation > 1)
		{
			pictID = kSky;
			for (i = 0; i < kNumTiles; i++)
				tiles[i] = 2;
		}
		else if (elevation == 1)
		{
			pictID = kMeadow;
			for (i = 0; i < kNumTiles; i++)
				tiles[i] = 0;
		}
		else
		{
			pictID = kDirt;
			for (i = 0; i < kNumTiles; i++)
				tiles[i] = 0;
		}
	}
	else
	{
		pictID = g_thisHouse.rooms[who].background;
		for (i = 0; i < kNumTiles; i++)
			tiles[i] = g_thisHouse.rooms[who].tiles[i];
	}

	LoadGraphicSpecial(g_workSrcMap, pictID);

	QSetRect(&src, 0, 0, kTileWide, kTileHigh);
	QSetRect(&dest, 0, 0, kTileWide, kTileHigh);
	QOffsetRect(&dest, g_localRoomsDest[where].left, g_localRoomsDest[where].top);
	for (i = 0; i < kNumTiles; i++)
	{
		src.left = tiles[i] * kTileWide;
		src.right = src.left + kTileWide;
		Mac_CopyBits(g_workSrcMap, g_backSrcMap,
				&src, &dest, srcCopy, nil);
		QOffsetRect(&dest, kTileWide, 0);
	}
}

//--------------------------------------------------------------  DrawFloorSupport

void DrawFloorSupport (void)
{
	Rect src, dest, whoCares;
	SInt16 i;

	src = g_suppSrcRect;

	if (g_isStructure[kNorthWestRoom])
	{
		dest = g_suppSrcRect;  // left room's ceiling
		QOffsetRect(&dest, g_localRoomsDest[kWestRoom].left,
				g_localRoomsDest[kCentralRoom].top - g_suppSrcRect.bottom);
		Mac_CopyBits(g_suppSrcMap, g_backSrcMap,
				&src, &dest, srcCopy, nil);

		for (i = 0; i < g_numTempManholes; i++)
			if (QSectRect(&dest, &g_tempManholes[i], &whoCares))
			{
				g_tempManholes[i].top = dest.top;
				g_tempManholes[i].bottom = dest.bottom;
				LoadScaledGraphic(g_backSrcMap, g_theHouseFile,
					kManholeThruFloor, &g_tempManholes[i]);
			}
	}

	if (g_isStructure[kWestRoom])
	{
		dest = g_suppSrcRect;  // left room's floor
		QOffsetRect(&dest, g_localRoomsDest[kWestRoom].left,
				g_localRoomsDest[kCentralRoom].bottom);
		Mac_CopyBits(g_suppSrcMap, g_backSrcMap,
				&src, &dest, srcCopy, nil);

		for (i = 0; i < g_numTempManholes; i++)
			if (QSectRect(&dest, &g_tempManholes[i], &whoCares))
			{
				g_tempManholes[i].top = dest.top;
				g_tempManholes[i].bottom = dest.bottom;
				LoadScaledGraphic(g_backSrcMap, g_theHouseFile,
					kManholeThruFloor, &g_tempManholes[i]);
			}
	}

	if (g_isStructure[kNorthRoom])
	{
		dest = g_suppSrcRect;  // directly above main room
		QOffsetRect(&dest, g_localRoomsDest[kCentralRoom].left,
				g_localRoomsDest[kCentralRoom].top - g_suppSrcRect.bottom);
		Mac_CopyBits(g_suppSrcMap, g_backSrcMap,
				&src, &dest, srcCopy, nil);
		for (i = 0; i < g_numTempManholes; i++)
			if (QSectRect(&dest, &g_tempManholes[i], &whoCares))
			{
				g_tempManholes[i].top = dest.top;
				g_tempManholes[i].bottom = dest.bottom;
				LoadScaledGraphic(g_backSrcMap, g_theHouseFile,
					kManholeThruFloor, &g_tempManholes[i]);
			}
	}

	if (g_isStructure[kCentralRoom])
	{
		dest = g_suppSrcRect;  // directly below main room
		QOffsetRect(&dest, g_localRoomsDest[kCentralRoom].left,
				g_localRoomsDest[kCentralRoom].bottom);
		Mac_CopyBits(g_suppSrcMap, g_backSrcMap,
				&src, &dest, srcCopy, nil);

		for (i = 0; i < g_numTempManholes; i++)
			if (QSectRect(&dest, &g_tempManholes[i], &whoCares))
			{
				g_tempManholes[i].top = dest.top;
				g_tempManholes[i].bottom = dest.bottom;
				LoadScaledGraphic(g_backSrcMap, g_theHouseFile,
					kManholeThruFloor, &g_tempManholes[i]);
			}
	}

	if (g_isStructure[kNorthEastRoom])
	{
		dest = g_suppSrcRect;
		QOffsetRect(&dest, g_localRoomsDest[kEastRoom].left,
				g_localRoomsDest[kCentralRoom].top - g_suppSrcRect.bottom);
		Mac_CopyBits(g_suppSrcMap, g_backSrcMap,
				&src, &dest, srcCopy, nil);

		for (i = 0; i < g_numTempManholes; i++)
			if (QSectRect(&dest, &g_tempManholes[i], &whoCares))
			{
				g_tempManholes[i].top = dest.top;
				g_tempManholes[i].bottom = dest.bottom;
				LoadScaledGraphic(g_backSrcMap, g_theHouseFile,
					kManholeThruFloor, &g_tempManholes[i]);
			}
	}

	if (g_isStructure[kEastRoom])
	{
		dest = g_suppSrcRect;
		QOffsetRect(&dest, g_localRoomsDest[kEastRoom].left,
				g_localRoomsDest[kCentralRoom].bottom);
		Mac_CopyBits(g_suppSrcMap, g_backSrcMap,
				&src, &dest, srcCopy, nil);

		for (i = 0; i < g_numTempManholes; i++)
			if (QSectRect(&dest, &g_tempManholes[i], &whoCares))
			{
				g_tempManholes[i].top = dest.top;
				g_tempManholes[i].bottom = dest.bottom;
				LoadScaledGraphic(g_backSrcMap, g_theHouseFile,
					kManholeThruFloor, &g_tempManholes[i]);
			}
	}
}

//--------------------------------------------------------------  ReadyBackMap

void ReadyBackMap (void)
{
	Mac_CopyBits(g_workSrcMap, g_backSrcMap,
			&g_workSrcRect, &g_workSrcRect, srcCopy, nil);
}

//--------------------------------------------------------------  RestoreWorkMap

void RestoreWorkMap (void)
{
	Mac_CopyBits(g_backSrcMap, g_workSrcMap,
			&g_backSrcRect, &g_backSrcRect, srcCopy, nil);
}

//--------------------------------------------------------------  ReadyLevel

void ReadyLevel (void)
{
	NilSavedMaps();

#ifdef COMPILEQT
	if ((g_thisMac.hasQT) && (g_hasMovie) && (g_tvInRoom))
	{
		g_tvInRoom = false;
		g_tvWithMovieNumber = -1;
		StopMovie(g_theMovie);
	}
#endif

	DetermineRoomOpenings();
	DrawLocale();
	InitGarbageRects();
}

//--------------------------------------------------------------  DrawLighting

void DrawLighting (void)
{
	if (g_numLights == 0)
		return;
	else
	{
		// for future construction
	}
}

//--------------------------------------------------------------  RedrawRoomLighting

void RedrawRoomLighting (void)
{
	SInt16 roomV;
	Boolean wasLit, isLit;

	roomV = g_thisHouse.rooms[g_thisRoomNumber].floor;

	wasLit = g_numLights > 0;
	g_numLights = GetNumberOfLights(g_localNumbers[kCentralRoom]);
	isLit = g_numLights > 0;
	if (wasLit != isLit)
	{
		DrawRoomBackground(g_localNumbers[kCentralRoom], kCentralRoom, roomV);
		DrawARoomsObjects(kCentralRoom, true);
		DrawLighting();
		UpdateOutletsLighting(g_localNumbers[kCentralRoom], g_numLights);

		if (g_numNeighbors > 3)
			DrawFloorSupport();
		RestoreWorkMap();
		AddRectToWorkRects(&g_localRoomsDest[kCentralRoom]);
		g_shadowVisible = IsShadowVisible();
	}
}
