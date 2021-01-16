//============================================================================
//----------------------------------------------------------------------------
//                                   Room.c
//----------------------------------------------------------------------------
//============================================================================

#include "Room.h"

#include "ColorUtils.h"
#include "DialogUtils.h"
#include "House.h"
#include "HouseIO.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "Map.h"
#include "Menu.h"
#include "ObjectEdit.h"
#include "RectUtils.h"
#include "ResourceIDs.h"
#include "ResourceLoader.h"
#include "StringUtils.h"

#include <strsafe.h>

#include <stdlib.h>

#define kYesDoDeleteRoom        IDOK

Boolean QueryDeleteRoom (HWND ownerWindow);
void SetToNearestNeighborRoom (SInt16 wasFloor_, SInt16 wasSuite_);
SInt16 GetOriginalBounding (SInt16 theID);
Boolean DoesRoomHaveFloor (void);
Boolean DoesRoomHaveCeiling (void);

roomPtr g_thisRoom;
Rect g_backSrcRect;
HDC g_backSrcMap;
SInt16 g_thisRoomNumber;
SInt16 g_previousRoom;
SInt16 g_leftThresh;
SInt16 g_rightThresh;
SInt16 g_lastBackground;
Boolean g_autoRoomEdit;
Boolean g_newRoomNow;
Boolean g_noRoomAtAll;
Boolean g_leftOpen;
Boolean g_rightOpen;
Boolean g_topOpen;
Boolean g_bottomOpen;
Boolean g_doBitchDialogs;

//==============================================================  Functions
//--------------------------------------------------------------  SetInitialTiles

void SetInitialTiles (SInt16 background, SInt16 *theTiles)
{
	SInt16 i;

	switch (background)
	{
		case kSimpleRoom:
		case kPaneledRoom:
		case kBasement:
		case kChildsRoom:
		case kAsianRoom:
		case kUnfinishedRoom:
		case kSwingersRoom:
		case kBathroom:
		case kLibrary:
		for (i = 0; i < kNumTiles; i++)
		{
			theTiles[i] = 1;
		}
		theTiles[0] = 0;
		theTiles[kNumTiles - 1] = kNumTiles - 1;
		break;

		case kSkywalk:
		for (i = 0; i < kNumTiles; i++)
		{
			theTiles[i] = i;
		}
		break;

		case kField:
		case kGarden:
		case kDirt:
		for (i = 0; i < kNumTiles; i++)
		{
			theTiles[i] = 0;
		}
		break;

		case kMeadow:
		for (i = 0; i < kNumTiles; i++)
		{
			theTiles[i] = 1;
		}
		break;

		case kRoof:
		for (i = 0; i < kNumTiles; i++)
		{
			theTiles[i] = 3;
		}
		break;

		case kSky:
		for (i = 0; i < kNumTiles; i++)
		{
			theTiles[i] = 2;
		}
		break;

		case kStratosphere:
		case kStars:
		for (i = 0; i < kNumTiles; i++)
		{
			theTiles[i] = i;
		}
		break;

		default: // user backgrounds
		for (i = 0; i < kNumTiles; i++)
		{
			theTiles[i] = i;
		}
		break;
	}
}

//--------------------------------------------------------------  CreateNewRoom

Boolean CreateNewRoom (HWND ownerWindow, SInt16 h, SInt16 v)
{
	SInt16 i;
	SInt16 availableRoom;
	size_t newRoomsCount;
	roomPtr newRoomsPtr;

	// save off current room
	CopyThisRoomToRoom();

	// fill out fields of new room
	PasStringCopyC("Untitled Room", g_thisRoom->name);
	g_thisRoom->leftStart = 32;
	g_thisRoom->rightStart = 32;
	g_thisRoom->bounds = 0;
	g_thisRoom->unusedByte = 0;
	g_thisRoom->visited = false;
	g_thisRoom->background = g_lastBackground;
	SetInitialTiles(g_thisRoom->background, g_thisRoom->tiles);
	g_thisRoom->floor = v;
	g_thisRoom->suite = h;
	g_thisRoom->openings = 0;
	g_thisRoom->numObjects = 0;
	// zero out all objects
	for (i = 0; i < kMaxRoomObs; i++)
		g_thisRoom->objects[i].what = kObjectIsEmpty;

	// assume no available rooms
	availableRoom = -1;
	if (g_thisHouse.nRooms > 0)
	{
		// look for an empty room
		for (i = 0; i < g_thisHouse.nRooms; i++)
		{
			if (g_thisHouse.rooms[i].suite == kRoomIsEmpty)
			{
				availableRoom = i;
				break;
			}
		}
	}

	if (availableRoom == -1)  // found no available rooms
	{
		if (g_thisHouse.nRooms < 0)
			g_thisHouse.nRooms = 0;
		newRoomsCount = (size_t)g_thisHouse.nRooms + 1;
		newRoomsPtr = (roomType *)realloc(g_thisHouse.rooms,
			newRoomsCount * sizeof(*g_thisHouse.rooms));
		if (newRoomsPtr == NULL)
		{
			YellowAlert(ownerWindow, kYellowUnaccounted, -1);
			return (false);
		}
		g_thisHouse.rooms = newRoomsPtr;
		g_thisHouse.nRooms++;  // increment nRooms
		g_previousRoom = g_thisRoomNumber;
		g_thisRoomNumber = g_thisHouse.nRooms - 1;
	}
	else
	{
		g_previousRoom = g_thisRoomNumber;
		g_thisRoomNumber = availableRoom;
	}

	if (g_noRoomAtAll)
		g_thisHouse.firstRoom = g_thisRoomNumber;

	CopyThisRoomToRoom();
	UpdateEditWindowTitle();
	g_noRoomAtAll = false;
	g_fileDirty = true;
	UpdateMenus(false);

	if (GetKeyState(VK_SHIFT) < 0)
		g_newRoomNow = false;
	else
		g_newRoomNow = g_autoRoomEdit;  // Flag to bring up RoomInfo

	return (true);
}

//--------------------------------------------------------------  ReadyBackground

void ReadyBackground (SInt16 theID, const SInt16 *theTiles)
{
	Rect src, dest;
	HBITMAP thePicture;
	BITMAP bmInfo;
	COLORREF wasColor;
	SInt16 i;
	NONCLIENTMETRICS ncm;
	HFONT hFont;
	wchar_t theString[256];

	if ((g_noRoomAtAll) || (!g_houseUnlocked))
	{
		wasColor = SetDCBrushColor(g_workSrcMap, LtGrayForeColor());
		Mac_PaintRect(g_workSrcMap, &g_workSrcRect, (HBRUSH)GetStockObject(DC_BRUSH));
		SetDCBrushColor(g_workSrcMap, wasColor);

		ncm.cbSize = sizeof(ncm);
		hFont = NULL;
		if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0))
		{
			hFont = CreateFontIndirect(&ncm.lfMessageFont);
		}
		if (hFont == NULL)
		{
			hFont = (HFONT)GetStockObject(SYSTEM_FONT);
		}

		if (g_houseUnlocked)
		{
			StringCchCopy(theString, ARRAYSIZE(theString), L"No Rooms");
		}
		else
		{
			StringCchCopy(theString, ARRAYSIZE(theString), L"Nothing to show");
		}

		SaveDC(g_workSrcMap);
		SetTextColor(g_workSrcMap, blackColor);
		SetTextAlign(g_workSrcMap, TA_TOP | TA_LEFT | TA_NOUPDATECP);
		SetBkMode(g_workSrcMap, TRANSPARENT);
		SelectObject(g_workSrcMap, hFont);
		TextOut(g_workSrcMap, 10, 10, theString, (int)wcslen(theString));
		RestoreDC(g_workSrcMap, -1);
		DeleteObject(hFont);

		Mac_CopyBits(g_workSrcMap, g_backSrcMap,
				&g_workSrcRect, &g_workSrcRect, srcCopy, nil);
		return;
	}

	thePicture = Gp_LoadImage(g_theHouseFile, theID);
	if (thePicture == NULL)
	{
		YellowAlert(g_mainWindow, kYellowNoBackground, 0);
		return;
	}

	GetObject(thePicture, sizeof(bmInfo), &bmInfo);
	QSetRect(&dest, 0, 0, (SInt16)bmInfo.bmWidth, (SInt16)bmInfo.bmHeight);
	Mac_DrawPicture(g_workSrcMap, thePicture, &dest);
	DeleteObject(thePicture);

	QSetRect(&src, 0, 0, kTileWide, kTileHigh);
	QSetRect(&dest, 0, 0, kTileWide, kTileHigh);
	for (i = 0; i < kNumTiles; i++)
	{
		src.left = theTiles[i] * kTileWide;
		src.right = src.left + kTileWide;
		Mac_CopyBits(g_workSrcMap, g_backSrcMap,
				&src, &dest, srcCopy, nil);
		QOffsetRect(&dest, kTileWide, 0);
	}

	QSetRect(&src, 0, 0, kRoomWide, kTileHigh);
	QSetRect(&dest, 0, 0, kRoomWide, kTileHigh);
	Mac_CopyBits(g_backSrcMap, g_workSrcMap,
			&src, &dest, srcCopy, nil);
}

//--------------------------------------------------------------  ReflectCurrentRoom

void ReflectCurrentRoom (Boolean forceMapRedraw)
{
	if (COMPILEDEMO)
		return;

	if (g_theMode != kEditMode)
		return;

	if ((g_noRoomAtAll) || (!g_houseUnlocked))
	{
		CenterMapOnRoom(64, 1);
		UpdateMapWindow();
	}
	else
	{
		if ((!ThisRoomVisibleOnMap()) || (forceMapRedraw))
		{
			CenterMapOnRoom(g_thisRoom->suite, g_thisRoom->floor);
			UpdateMapWindow();  // whole map window redrawm
		}
		else
		{
			FindNewActiveRoomRect();  // find newly selected room rect
			FlagMapRoomsForUpdate();  // redraw only the portions required
		}
	}
	GenerateRetroLinks();
	UpdateEditWindowTitle();
	ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
	GetThisRoomsObjRects();
	DrawThisRoomsObjects();
	InvalidateRect(g_mainWindow, NULL, TRUE);
}

//--------------------------------------------------------------  CopyRoomToThisRoom

void CopyRoomToThisRoom (SInt16 roomNumber)
{
	if (roomNumber == -1)
		return;

	CopyThisRoomToRoom();  // copy back to house
	ForceThisRoom(roomNumber);  // load new room from house
}

//--------------------------------------------------------------  CopyThisRoomToRoom

void CopyThisRoomToRoom (void)
{
	if (g_noRoomAtAll)
		return;
	if (g_thisRoomNumber < 0 || g_thisRoomNumber >= g_thisHouse.nRooms)
		return;

	g_thisHouse.rooms[g_thisRoomNumber] = *g_thisRoom;  // copy back to house
}

//--------------------------------------------------------------  ForceThisRoom

void ForceThisRoom (SInt16 roomNumber)
{
	if (roomNumber < 0 || roomNumber >= g_thisHouse.nRooms)
		return;

	if (roomNumber < g_thisHouse.nRooms)
		*g_thisRoom = g_thisHouse.rooms[roomNumber];
	else
		YellowAlert(g_mainWindow, kYellowIllegalRoomNum, 0);

	g_previousRoom = g_thisRoomNumber;
	g_thisRoomNumber = roomNumber;
}

//--------------------------------------------------------------  RoomExists

Boolean RoomExists (const houseType *house, SInt16 suite, SInt16 floor, SInt16 *roomNum)
{
	// pass in a suite and floor; returns true is it is a legitimate room
	SInt16 i;
	Boolean foundIt;

	foundIt = false;

	if (suite < 0)
		return (foundIt);

	for (i = 0; i < house->nRooms; i++)
	{
		if ((house->rooms[i].floor == floor) && (house->rooms[i].suite == suite))
		{
			foundIt = true;
			*roomNum = i;
			break;
		}
	}

	return (foundIt);
}

//--------------------------------------------------------------  RoomNumExists

Boolean RoomNumExists (SInt16 roomNum)
{
	SInt16 floor, suite, whoCares;
	Boolean exists;

	exists = false;
	if (GetRoomFloorSuite(roomNum, &floor, &suite))
		exists = RoomExists(&g_thisHouse, suite, floor, &whoCares);

	return (exists);
}

//--------------------------------------------------------------  DeleteRoom

void DeleteRoom (HWND ownerWindow, Boolean doWarn)
{
	SInt16 wasFloor_, wasSuite_;
	Boolean firstDeleted;

	if (COMPILEDEMO)
		return;
	if ((g_theMode != kEditMode) || (g_noRoomAtAll))
		return;
	if (g_thisRoomNumber < 0 || g_thisRoomNumber >= g_thisHouse.nRooms)
		return;

	if (doWarn)
	{
		if (!QueryDeleteRoom(ownerWindow))
			return;
	}

	DeselectObject();

	wasFloor_ = g_thisHouse.rooms[g_thisRoomNumber].floor;
	wasSuite_ = g_thisHouse.rooms[g_thisRoomNumber].suite;
	firstDeleted = (g_thisHouse.firstRoom == g_thisRoomNumber);  // is room "first"
	g_thisRoom->suite = kRoomIsEmpty;
	g_thisHouse.rooms[g_thisRoomNumber].suite = kRoomIsEmpty;

	g_noRoomAtAll = (RealRoomNumberCount() == 0);  // see if now no rooms
	if (g_noRoomAtAll)
		g_thisRoomNumber = kRoomIsEmpty;
	else
		SetToNearestNeighborRoom(wasFloor_, wasSuite_);

	if (firstDeleted)
	{
		g_thisHouse.firstRoom = g_thisRoomNumber;
	}

	g_newRoomNow = false;
	g_fileDirty = true;
	UpdateMenus(false);
	ReflectCurrentRoom(false);
}

//--------------------------------------------------------------  QueryDeleteRoom

Boolean QueryDeleteRoom (HWND ownerWindow)
{
	SInt16 hitWhat;

	hitWhat = Alert(kDeleteRoomAlert, ownerWindow, NULL);
	if (hitWhat == kYesDoDeleteRoom)
		return (true);
	else
		return (false);
}

//--------------------------------------------------------------  DoesNeighborRoomExist

SInt16 DoesNeighborRoomExist (SInt16 whichNeighbor)
{
	SInt16 newH, newV, newRoomNumber;

	if (COMPILEDEMO)
		return(-1);

	if (g_theMode != kEditMode)
		return(-1);

	newH = g_thisRoom->suite;
	newV = g_thisRoom->floor;

	switch (whichNeighbor)
	{
		case kRoomAbove:
		newV++;
		break;

		case kRoomBelow:
		newV--;
		break;

		case kRoomToRight:
		newH++;
		break;

		case kRoomToLeft:
		newH--;
		break;
	}

	if (RoomExists(&g_thisHouse, newH, newV, &newRoomNumber))
		return (newRoomNumber);
	else
		return (-1);
}

//--------------------------------------------------------------  SelectNeighborRoom

void SelectNeighborRoom (SInt16 whichNeighbor)
{
	SInt16 newRoomNumber;

	if (COMPILEDEMO)
		return;

	newRoomNumber = DoesNeighborRoomExist(whichNeighbor);

	if (newRoomNumber != -1)
	{
		DeselectObject();
		CopyRoomToThisRoom(newRoomNumber);
		ReflectCurrentRoom(false);
	}
}

//--------------------------------------------------------------  GetNeighborRoomNumber

SInt16 GetNeighborRoomNumber (SInt16 which)
{
	SInt16 hDelta, vDelta, i;
	SInt16 roomH, roomV;
	SInt16 roomNum;

	switch (which)
	{
		case kCentralRoom:
		hDelta = 0;
		vDelta = 0;
		break;

		case kNorthRoom:
		hDelta = 0;
		vDelta = 1;
		break;

		case kNorthEastRoom:
		hDelta = 1;
		vDelta = 1;
		break;

		case kEastRoom:
		hDelta = 1;
		vDelta = 0;
		break;

		case kSouthEastRoom:
		hDelta = 1;
		vDelta = -1;
		break;

		case kSouthRoom:
		hDelta = 0;
		vDelta = -1;
		break;

		case kSouthWestRoom:
		hDelta = -1;
		vDelta = -1;
		break;

		case kWestRoom:
		hDelta = -1;
		vDelta = 0;
		break;

		case kNorthWestRoom:
		hDelta = -1;
		vDelta = 1;
		break;

		default:
		return kRoomIsEmpty;
		break;
	}

	if (g_thisRoomNumber < 0 || g_thisRoomNumber >= g_thisHouse.nRooms)
		return kRoomIsEmpty;

	roomNum = kRoomIsEmpty;
	roomH = g_thisHouse.rooms[g_thisRoomNumber].suite + hDelta;
	roomV = g_thisHouse.rooms[g_thisRoomNumber].floor + vDelta;

	for (i = 0; i < g_thisHouse.nRooms; i++)
	{
		if ((g_thisHouse.rooms[i].suite == roomH) &&
				(g_thisHouse.rooms[i].floor == roomV))
		{
			roomNum = i;
			break;
		}
	}

	return (roomNum);
}

//--------------------------------------------------------------  SetToNearestNeighborRoom

void SetToNearestNeighborRoom (SInt16 wasFloor_, SInt16 wasSuite_)
{
	// searches in a clockwise spiral pattern (from g_thisRoom) for a
	// legitimate neighboring room - then sets g_thisRoom to it
	SInt16 distance, h, v;
	SInt16 hStep, vStep;
	SInt16 testRoomNum, testH, testV;
	Boolean finished;

	finished = false;
	distance = 1;  // we begin our walk a distance of one from source room
	h = -1;  // we begin with the neighbor to the left
	v = 0;  // and on the same floor
	hStep = 0;  // we don't 'walk' left or right
	vStep = -1;  // instead, we 'walk' up

	do
	{
		testH = wasSuite_ + h;
		testV = wasFloor_ + v;

		// if a legitimate room
		if (RoomExists(&g_thisHouse, testH, testV, &testRoomNum))
		{
			CopyRoomToThisRoom(testRoomNum);
			finished = true;
		}
		else
		{
			h += hStep;
			v += vStep;
			if ((h > distance) || (h < -distance) || (v > distance) || (v < -distance))
			{
				// we have walked beyond the bounds of our spiral
				if ((hStep == -1) && (vStep == 0))  // we expand our spiral out
				{
					distance++;
					// begin travelling up again
					hStep = 0;
					vStep = -1;
				}
				else
				{
					// first, back up a step
					h -= hStep;
					v -= vStep;

					if (hStep == 0)
					{
						// we were travelling up or down
						if (vStep == -1)  // we were travelling up
							hStep = 1;  // so begin travelling right
						else  // we were travelling down
							hStep = -1;  // so begin travelling left
						vStep = 0;
					}
					else
					{
						// begin travelling down
						hStep = 0;
						vStep = 1;
					}
					// proceed a step now
					h += hStep;
					v += vStep;
				}
			}
		}
	} while (!finished);
}

//--------------------------------------------------------------  GetRoomFloorSuite

Boolean GetRoomFloorSuite (SInt16 room, SInt16 *floor, SInt16 *suite)
{
	Boolean isRoom;

	if (room < 0 || room >= g_thisHouse.nRooms)
	{
		*floor = 0;
		*suite = kRoomIsEmpty;
		isRoom = false;
	}
	else if (g_thisHouse.rooms[room].suite == kRoomIsEmpty)
	{
		*floor = 0;
		*suite = kRoomIsEmpty;
		isRoom = false;
	}
	else
	{
		*suite = g_thisHouse.rooms[room].suite;
		*floor = g_thisHouse.rooms[room].floor;
		isRoom = true;
	}

	return (isRoom);
}

//--------------------------------------------------------------  GetRoomNumber

SInt16 GetRoomNumber (SInt16 floor, SInt16 suite)
{
	// pass in a floor and suite; returns the room index into the house file
	SInt16 roomNum, i;

	roomNum = kRoomIsEmpty;

	for (i = 0; i < g_thisHouse.nRooms; i++)
	{
		if ((g_thisHouse.rooms[i].suite == suite) &&
				(g_thisHouse.rooms[i].floor == floor))
		{
			roomNum = i;
			break;
		}
	}

	return (roomNum);
}

//--------------------------------------------------------------  IsRoomAStructure

Boolean IsRoomAStructure (SInt16 roomNum)
{
	Boolean isStructure;

	if (roomNum < 0 || roomNum >= g_thisHouse.nRooms)
		return (false);

	if (g_thisHouse.rooms[roomNum].background >= kUserBackground)
	{
		if (g_thisHouse.rooms[roomNum].bounds != 0)
		{
			isStructure = ((g_thisHouse.rooms[roomNum].bounds & 32) == 32);
		}
		else
		{
			if (g_thisHouse.rooms[roomNum].background < kUserStructureRange)
				isStructure = true;
			else
				isStructure = false;
		}
	}
	else
	{
		switch (g_thisHouse.rooms[roomNum].background)
		{
			case kPaneledRoom:
			case kSimpleRoom:
			case kChildsRoom:
			case kAsianRoom:
			case kUnfinishedRoom:
			case kSwingersRoom:
			case kBathroom:
			case kLibrary:
			case kSkywalk:
			case kRoof:
			isStructure = true;
			break;

			default:
			isStructure = false;
			break;
		}
	}

	return (isStructure);
}

//--------------------------------------------------------------  DetermineRoomOpenings

void DetermineRoomOpenings (void)
{
	SInt16 whichBack, leftTile, rightTile;
	SInt16 boundsCode;

	whichBack = g_thisRoom->background;
	leftTile = g_thisRoom->tiles[0];
	rightTile = g_thisRoom->tiles[kNumTiles - 1];

	if (whichBack >= kUserBackground)
	{
		if (g_thisRoom->bounds != 0)
			boundsCode = g_thisRoom->bounds >> 1;
		else
			boundsCode = GetOriginalBounding(whichBack);
		g_leftOpen = ((boundsCode & 0x0001) == 0x0001);
		g_rightOpen = ((boundsCode & 0x0004) == 0x0004);

		if (g_leftOpen)
			g_leftThresh = kNoLeftWallLimit;
		else
			g_leftThresh = kLeftWallLimit;

		if (g_rightOpen)
			g_rightThresh = kNoRightWallLimit;
		else
			g_rightThresh = kRightWallLimit;
	}
	else
	{
		switch (whichBack)
		{
			case kSimpleRoom:
			case kPaneledRoom:
			case kBasement:
			case kChildsRoom:
			case kAsianRoom:
			case kUnfinishedRoom:
			case kSwingersRoom:
			case kBathroom:
			case kLibrary:
			case kSky:
			if (leftTile == 0)
				g_leftThresh = kLeftWallLimit;
			else
				g_leftThresh = kNoLeftWallLimit;
			if (rightTile == (kNumTiles - 1))
				g_rightThresh = kRightWallLimit;
			else
				g_rightThresh = kNoRightWallLimit;
			g_leftOpen = (leftTile != 0);
			g_rightOpen = (rightTile != (kNumTiles - 1));
			break;

			case kDirt:
			if (leftTile == 1)
				g_leftThresh = kLeftWallLimit;
			else
				g_leftThresh = kNoLeftWallLimit;
			if (rightTile == (kNumTiles - 1))
				g_rightThresh = kRightWallLimit;
			else
				g_rightThresh = kNoRightWallLimit;
			g_leftOpen = (leftTile != 0);
			g_rightOpen = (rightTile != (kNumTiles - 1));
			break;

			case kMeadow:
			if (leftTile == 6)
				g_leftThresh = kLeftWallLimit;
			else
				g_leftThresh = kNoLeftWallLimit;
			if (rightTile == 7)
				g_rightThresh = kRightWallLimit;
			else
				g_rightThresh = kNoRightWallLimit;
			g_leftOpen = (leftTile != 6);
			g_rightOpen = (rightTile != 7);
			break;

			case kGarden:
			case kSkywalk:
			case kField:
			case kStratosphere:
			case kStars:
			g_leftThresh = kNoLeftWallLimit;
			g_rightThresh = kNoRightWallLimit;
			g_leftOpen = true;
			g_rightOpen = true;
			break;

			default:
			if (leftTile == 0)
				g_leftThresh = kLeftWallLimit;
			else
				g_leftThresh = kNoLeftWallLimit;

			if (rightTile == (kNumTiles - 1))
				g_rightThresh = kRightWallLimit;
			else
				g_rightThresh = kNoRightWallLimit;

			g_leftOpen = (leftTile != 0);
			g_rightOpen = (rightTile != (kNumTiles - 1));
			break;
		}
	}

	if (DoesRoomHaveFloor())
		g_bottomOpen = false;
	else
		g_bottomOpen = true;

	if (DoesRoomHaveCeiling())
		g_topOpen = false;
	else
		g_topOpen = true;
}

//--------------------------------------------------------------  GetOriginalBounding

SInt16 GetOriginalBounding (SInt16 theID)
{
	boundsType boundsRes;
	SInt16 boundCode;

	boundCode = 0;
	if (FAILED(Gp_LoadHouseBounding(g_theHouseFile, theID, &boundsRes)))
	{
		if (Gp_HouseImageExists(g_theHouseFile, theID))
		{
			YellowAlert(g_mainWindow, kYellowNoBoundsRes, 0);
		}
	}
	else
	{
		if (boundsRes.left)
		{
			boundCode += 1;
		}
		if (boundsRes.top)
		{
			boundCode += 2;
		}
		if (boundsRes.right)
		{
			boundCode += 4;
		}
		if (boundsRes.bottom)
		{
			boundCode += 8;
		}
	}

	return boundCode;
}

//--------------------------------------------------------------  GetNumberOfLights

SInt16 GetNumberOfLights (SInt16 where)
{
	SInt16 i, count;

	if (g_theMode == kEditMode)
	{
		switch (g_thisRoom->background)
		{
			case kGarden:
			case kSkywalk:
			case kMeadow:
			case kField:
			case kRoof:
			case kSky:
			case kStratosphere:
			case kStars:
			count = 1;
			break;

			case kDirt:
			count = 0;
			if ((g_thisRoom->tiles[0] == 0) && (g_thisRoom->tiles[1] == 0) &&
					(g_thisRoom->tiles[2] == 0) && (g_thisRoom->tiles[3] == 0) &&
					(g_thisRoom->tiles[4] == 0) && (g_thisRoom->tiles[5] == 0) &&
					(g_thisRoom->tiles[6] == 0) && (g_thisRoom->tiles[7] == 0))
				count = 1;
			break;

			default:
			count = 0;
			break;
		}
		if (count == 0)
		{
			for (i = 0; i < kMaxRoomObs; i++)
			{
				switch (g_thisRoom->objects[i].what)
				{
					case kDoorInLf:
					case kDoorInRt:
					case kWindowInLf:
					case kWindowInRt:
					case kWallWindow:
					count++;
					break;

					case kCeilingLight:
					case kLightBulb:
					case kTableLamp:
					case kHipLamp:
					case kDecoLamp:
					case kFlourescent:
					case kTrackLight:
					case kInvisLight:
					if (g_thisRoom->objects[i].data.f.initial)
						count++;
					break;
				}
			}
		}
	}
	else
	{
		if (where < 0 || where >= g_thisHouse.nRooms)
			return 0;
		switch (g_thisHouse.rooms[where].background)
		{
			case kGarden:
			case kSkywalk:
			case kMeadow:
			case kField:
			case kRoof:
			case kSky:
			case kStratosphere:
			case kStars:
			count = 1;
			break;

			case kDirt:
			count = 0;
			if ((g_thisHouse.rooms[where].tiles[0] == 0) &&
					(g_thisHouse.rooms[where].tiles[1] == 0) &&
					(g_thisHouse.rooms[where].tiles[2] == 0) &&
					(g_thisHouse.rooms[where].tiles[3] == 0) &&
					(g_thisHouse.rooms[where].tiles[4] == 0) &&
					(g_thisHouse.rooms[where].tiles[5] == 0) &&
					(g_thisHouse.rooms[where].tiles[6] == 0) &&
					(g_thisHouse.rooms[where].tiles[7] == 0))
				count = 1;
			break;

			default:
			count = 0;
			break;
		}
		if (count == 0)
		{
			for (i = 0; i < kMaxRoomObs; i++)
			{
				switch (g_thisHouse.rooms[where].objects[i].what)
				{
					case kDoorInLf:
					case kDoorInRt:
					case kWindowInLf:
					case kWindowInRt:
					case kWallWindow:
					count++;
					break;

					case kCeilingLight:
					case kLightBulb:
					case kTableLamp:
					case kHipLamp:
					case kDecoLamp:
					case kFlourescent:
					case kTrackLight:
					case kInvisLight:
					if (g_thisHouse.rooms[where].objects[i].data.f.state)
						count++;
					break;
				}
			}
		}
	}
	return (count);
}

//--------------------------------------------------------------  IsShadowVisible

Boolean IsShadowVisible (void)
{
	SInt16 boundsCode;
	Boolean hasFloor;

	if (g_thisRoom->background >= kUserBackground)
	{
		// is this a version 2.0 house?
		if (g_thisRoom->bounds != 0)
			boundsCode = (g_thisRoom->bounds >> 1);
		else
			boundsCode = GetOriginalBounding(g_thisRoom->background);
		hasFloor = ((boundsCode & 0x0008) != 0x0008);
	}
	else
	{
		switch (g_thisRoom->background)
		{
			case kRoof:
			case kSky:
			case kStratosphere:
			case kStars:
			hasFloor = false;
			break;

			default:
			hasFloor = true;
			break;
		}
	}

	return (hasFloor);
}

//--------------------------------------------------------------  DoesRoomHaveFloor

Boolean DoesRoomHaveFloor (void)
{
	SInt16 boundsCode;
	Boolean hasFloor;

	if (g_thisRoom->background >= kUserBackground)
	{
		// is this a version 2.0 house?
		if (g_thisRoom->bounds != 0)
			boundsCode = (g_thisRoom->bounds >> 1);
		else
			boundsCode = GetOriginalBounding(g_thisRoom->background);
		hasFloor = ((boundsCode & 0x0008) != 0x0008);
	}
	else
	{
		switch (g_thisRoom->background)
		{
			case kSky:
			case kStratosphere:
			case kStars:
			hasFloor = false;
			break;

			default:
			hasFloor = true;
			break;
		}
	}

	return (hasFloor);
}

//--------------------------------------------------------------  DoesRoomHaveCeiling

Boolean DoesRoomHaveCeiling (void)
{
	SInt16 boundsCode;
	Boolean hasCeiling;

	if (g_thisRoom->background >= kUserBackground)
	{
		// is this a version 2.0 house?
		if (g_thisRoom->bounds != 0)
			boundsCode = (g_thisRoom->bounds >> 1);
		else
			boundsCode = GetOriginalBounding(g_thisRoom->background);
		hasCeiling = ((boundsCode & 0x0002) != 0x0002);
	}
	else
	{
		switch (g_thisRoom->background)
		{
			case kGarden:
			case kMeadow:
			case kField:
			case kRoof:
			case kSky:
			case kStratosphere:
			case kStars:
			hasCeiling = false;
			break;

			default:
			hasCeiling = true;
			break;
		}
	}
	return (hasCeiling);
}
