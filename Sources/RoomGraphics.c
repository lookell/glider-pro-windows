#define GP_USE_WINAPI_H

#include "RoomGraphics.h"

//============================================================================
//----------------------------------------------------------------------------
//                               RoomGraphics.c
//----------------------------------------------------------------------------
//============================================================================


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


Rect suppSrcRect;
HDC suppSrcMap;
Rect localRoomsDest[9];
Rect houseRect;
SInt16 numNeighbors;
SInt16 numLights;
SInt16 thisTiles[kNumTiles];
SInt16 localNumbers[9];
SInt16 thisBackground;
Boolean wardBitSet;

static Boolean isStructure[9];


//==============================================================  Functions
//--------------------------------------------------------------  DrawLocale

void DrawLocale (void)
{
	SInt16		i, roomV;

	ZeroFlamesAndTheLike();
	ZeroDinahs();
	KillAllBands();
	ZeroMirrorRegion();
	ZeroTriggers();
	numTempManholes = 0;
	FlushAnyTriggerPlaying();
	DumpTriggerSound();
	tvInRoom = false;
	tvWithMovieNumber = -1;

	roomV = thisHouse.rooms[thisRoomNumber].floor;

	for (i = 0; i < 9; i++)
	{
		localNumbers[i] = GetNeighborRoomNumber(i);
		isStructure[i] = IsRoomAStructure(localNumbers[i]);
	}
	ListAllLocalObjects();

	Mac_PaintRect(backSrcMap, &backSrcRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

	if (numNeighbors > 3)
	{
		numLights = GetNumberOfLights(localNumbers[kNorthWestRoom]);
		DrawRoomBackground(localNumbers[kNorthWestRoom], kNorthWestRoom, roomV + 1);
		DrawARoomsObjects(kNorthWestRoom, false);

		numLights = GetNumberOfLights(localNumbers[kNorthEastRoom]);
		DrawRoomBackground(localNumbers[kNorthEastRoom], kNorthEastRoom, roomV + 1);
		DrawARoomsObjects(kNorthEastRoom, false);

		numLights = GetNumberOfLights(localNumbers[kNorthRoom]);
		DrawRoomBackground(localNumbers[kNorthRoom], kNorthRoom, roomV + 1);
		DrawARoomsObjects(kNorthRoom, false);

		numLights = GetNumberOfLights(localNumbers[kSouthWestRoom]);
		DrawRoomBackground(localNumbers[kSouthWestRoom], kSouthWestRoom, roomV - 1);
		DrawARoomsObjects(kSouthWestRoom, false);

		numLights = GetNumberOfLights(localNumbers[kSouthEastRoom]);
		DrawRoomBackground(localNumbers[kSouthEastRoom], kSouthEastRoom, roomV - 1);
		DrawARoomsObjects(kSouthEastRoom, false);

		numLights = GetNumberOfLights(localNumbers[kSouthRoom]);
		DrawRoomBackground(localNumbers[kSouthRoom], kSouthRoom, roomV - 1);
		DrawARoomsObjects(kSouthRoom, false);
	}

	if (numNeighbors > 1)
	{
		numLights = GetNumberOfLights(localNumbers[kWestRoom]);
		DrawRoomBackground(localNumbers[kWestRoom], kWestRoom, roomV);
		DrawARoomsObjects(kWestRoom, false);
		DrawLighting();

		numLights = GetNumberOfLights(localNumbers[kEastRoom]);
		DrawRoomBackground(localNumbers[kEastRoom], kEastRoom, roomV);
		DrawARoomsObjects(kEastRoom, false);
		DrawLighting();
	}

	numLights = GetNumberOfLights(localNumbers[kCentralRoom]);
	DrawRoomBackground(localNumbers[kCentralRoom], kCentralRoom, roomV);
	DrawARoomsObjects(kCentralRoom, false);
	DrawLighting();

	if (numNeighbors > 3)
		DrawFloorSupport();
	RestoreWorkMap();
	shadowVisible = IsShadowVisible();
	takingTheStairs = false;
}

//--------------------------------------------------------------  LoadGraphicSpecial

void LoadGraphicSpecial (HDC hdc, SInt16 resID)
{
	Rect		bounds;
	HBITMAP		thePicture;
	BITMAP		bmInfo;

	thePicture = Gp_LoadImage(g_theHouseFile, resID);
	if (thePicture == NULL)
	{
		thePicture = Gp_LoadImage(g_theHouseFile, kSimpleRoom);
		if (thePicture == NULL)
		{
			RedAlert(kErrFailedGraphicLoad);
		}
	}

	GetObject(thePicture, sizeof(bmInfo), &bmInfo);
	QSetRect(&bounds, 0, 0, (SInt16)bmInfo.bmWidth, (SInt16)bmInfo.bmHeight);
	Mac_DrawPicture(hdc, thePicture, &bounds);

	DeleteObject(thePicture);
}

//--------------------------------------------------------------  DrawRoomBackground

void DrawRoomBackground (SInt16 who, SInt16 where, SInt16 elevation)
{
	Rect		src, dest;
	SInt16		i, pictID;
	SInt16		tiles[kNumTiles];

	if (where == kCentralRoom)
	{
		thisBackground = thisHouse.rooms[who].background;
		for (i = 0; i < kNumTiles; i++)
			thisTiles[i] = thisHouse.rooms[who].tiles[i];
	}

	if ((numLights == 0) && (who != kRoomIsEmpty))
	{
		Mac_PaintRect(backSrcMap, &localRoomsDest[where], (HBRUSH)GetStockObject(BLACK_BRUSH));
		return;
	}

	if (who == kRoomIsEmpty)		// This call should be smarter than this
	{
		if (wardBitSet)
		{
			Mac_PaintRect(backSrcMap, &localRoomsDest[where], (HBRUSH)GetStockObject(BLACK_BRUSH));
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
		pictID = thisHouse.rooms[who].background;
		for (i = 0; i < kNumTiles; i++)
			tiles[i] = thisHouse.rooms[who].tiles[i];
	}

	LoadGraphicSpecial(workSrcMap, pictID);

	QSetRect(&src, 0, 0, kTileWide, kTileHigh);
	QSetRect(&dest, 0, 0, kTileWide, kTileHigh);
	QOffsetRect(&dest, localRoomsDest[where].left, localRoomsDest[where].top);
	for (i = 0; i < kNumTiles; i++)
	{
		src.left = tiles[i] * kTileWide;
		src.right = src.left + kTileWide;
		Mac_CopyBits(workSrcMap, backSrcMap,
				&src, &dest, srcCopy, nil);
		QOffsetRect(&dest, kTileWide, 0);
	}
}

//--------------------------------------------------------------  DrawFloorSupport

void DrawFloorSupport (void)
{
	Rect		src, dest, whoCares;
	SInt16		i;

	src = suppSrcRect;

	if (isStructure[kNorthWestRoom])
	{
		dest = suppSrcRect;			// left room's ceiling
		QOffsetRect(&dest, localRoomsDest[kWestRoom].left,
				localRoomsDest[kCentralRoom].top - suppSrcRect.bottom);
		Mac_CopyBits(suppSrcMap, backSrcMap,
				&src, &dest, srcCopy, nil);

		for (i = 0; i < numTempManholes; i++)
			if (QSectRect(&dest, &tempManholes[i], &whoCares))
			{
				tempManholes[i].top = dest.top;
				tempManholes[i].bottom = dest.bottom;
				LoadScaledGraphic(backSrcMap, g_theHouseFile,
					kManholeThruFloor, &tempManholes[i]);
			}
	}

	if (isStructure[kWestRoom])
	{
		dest = suppSrcRect;			// left room's floor
		QOffsetRect(&dest, localRoomsDest[kWestRoom].left,
				localRoomsDest[kCentralRoom].bottom);
		Mac_CopyBits(suppSrcMap, backSrcMap,
				&src, &dest, srcCopy, nil);

		for (i = 0; i < numTempManholes; i++)
			if (QSectRect(&dest, &tempManholes[i], &whoCares))
			{
				tempManholes[i].top = dest.top;
				tempManholes[i].bottom = dest.bottom;
				LoadScaledGraphic(backSrcMap, g_theHouseFile,
					kManholeThruFloor, &tempManholes[i]);
			}
	}

	if (isStructure[kNorthRoom])
	{
		dest = suppSrcRect;			// directly above main room
		QOffsetRect(&dest, localRoomsDest[kCentralRoom].left,
				localRoomsDest[kCentralRoom].top - suppSrcRect.bottom);
		Mac_CopyBits(suppSrcMap, backSrcMap,
				&src, &dest, srcCopy, nil);
		for (i = 0; i < numTempManholes; i++)
			if (QSectRect(&dest, &tempManholes[i], &whoCares))
			{
				tempManholes[i].top = dest.top;
				tempManholes[i].bottom = dest.bottom;
				LoadScaledGraphic(backSrcMap, g_theHouseFile,
					kManholeThruFloor, &tempManholes[i]);
			}
	}

	if (isStructure[kCentralRoom])
	{
		dest = suppSrcRect;			// directly below main room
		QOffsetRect(&dest, localRoomsDest[kCentralRoom].left,
				localRoomsDest[kCentralRoom].bottom);
		Mac_CopyBits(suppSrcMap, backSrcMap,
				&src, &dest, srcCopy, nil);

		for (i = 0; i < numTempManholes; i++)
			if (QSectRect(&dest, &tempManholes[i], &whoCares))
			{
				tempManholes[i].top = dest.top;
				tempManholes[i].bottom = dest.bottom;
				LoadScaledGraphic(backSrcMap, g_theHouseFile,
					kManholeThruFloor, &tempManholes[i]);
			}
	}

	if (isStructure[kNorthEastRoom])
	{
		dest = suppSrcRect;
		QOffsetRect(&dest, localRoomsDest[kEastRoom].left,
				localRoomsDest[kCentralRoom].top - suppSrcRect.bottom);
		Mac_CopyBits(suppSrcMap, backSrcMap,
				&src, &dest, srcCopy, nil);

		for (i = 0; i < numTempManholes; i++)
			if (QSectRect(&dest, &tempManholes[i], &whoCares))
			{
				tempManholes[i].top = dest.top;
				tempManholes[i].bottom = dest.bottom;
				LoadScaledGraphic(backSrcMap, g_theHouseFile,
					kManholeThruFloor, &tempManholes[i]);
			}
	}

	if (isStructure[kEastRoom])
	{
		dest = suppSrcRect;
		QOffsetRect(&dest, localRoomsDest[kEastRoom].left,
				localRoomsDest[kCentralRoom].bottom);
		Mac_CopyBits(suppSrcMap, backSrcMap,
				&src, &dest, srcCopy, nil);

		for (i = 0; i < numTempManholes; i++)
			if (QSectRect(&dest, &tempManholes[i], &whoCares))
			{
				tempManholes[i].top = dest.top;
				tempManholes[i].bottom = dest.bottom;
				LoadScaledGraphic(backSrcMap, g_theHouseFile,
					kManholeThruFloor, &tempManholes[i]);
			}
	}
}

//--------------------------------------------------------------  ReadyBackMap

void ReadyBackMap (void)
{
	Mac_CopyBits(workSrcMap, backSrcMap,
			&workSrcRect, &workSrcRect, srcCopy, nil);
}

//--------------------------------------------------------------  RestoreWorkMap

void RestoreWorkMap (void)
{
	Mac_CopyBits(backSrcMap, workSrcMap,
			&backSrcRect, &backSrcRect, srcCopy, nil);
}

//--------------------------------------------------------------  ReadyLevel

void ReadyLevel (void)
{
	NilSavedMaps();

#ifdef COMPILEQT
	if ((thisMac.hasQT) && (hasMovie) && (tvInRoom))
	{
		tvInRoom = false;
		tvWithMovieNumber = -1;
		StopMovie(theMovie);
	}
#endif

	DetermineRoomOpenings();
	DrawLocale();
	InitGarbageRects();
}

//--------------------------------------------------------------  DrawLighting

void DrawLighting (void)
{
	if (numLights == 0)
		return;
	else
	{
		// for future construction
	}
}

//--------------------------------------------------------------  RedrawRoomLighting

void RedrawRoomLighting (void)
{
	SInt16		roomV;
	Boolean		wasLit, isLit;

	roomV = thisHouse.rooms[thisRoomNumber].floor;

	wasLit = numLights > 0;
	numLights = GetNumberOfLights(localNumbers[kCentralRoom]);
	isLit = numLights > 0;
	if (wasLit != isLit)
	{
		DrawRoomBackground(localNumbers[kCentralRoom], kCentralRoom, roomV);
		DrawARoomsObjects(kCentralRoom, true);
		DrawLighting();
		UpdateOutletsLighting(localNumbers[kCentralRoom], numLights);

		if (numNeighbors > 3)
			DrawFloorSupport();
		RestoreWorkMap();
		AddRectToWorkRects(&localRoomsDest[kCentralRoom]);
		shadowVisible = IsShadowVisible();
	}
}

