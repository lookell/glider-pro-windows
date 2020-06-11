//============================================================================
//----------------------------------------------------------------------------
//									Room.c
//----------------------------------------------------------------------------
//============================================================================


#include "ByteIO.h"
#include "DialogUtils.h"
#include "Externs.h"
#include "House.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "RectUtils.h"
#include "ResourceIDs.h"


#define kYesDoDeleteRoom		IDOK


Boolean QueryDeleteRoom (HWND);
void SetToNearestNeighborRoom (SInt16, SInt16);


roomPtr		thisRoom;
Rect		backSrcRect;
HDC			backSrcMap;
SInt16		numberRooms, thisRoomNumber, previousRoom;
SInt16		leftThresh, rightThresh, lastBackground;
Boolean		autoRoomEdit, newRoomNow, noRoomAtAll;
Boolean		leftOpen, rightOpen, topOpen, bottomOpen;
Boolean		doBitchDialogs;

extern	SInt16		tempTiles[];
extern	HMODULE		houseResFork;


//==============================================================  Functions
//--------------------------------------------------------------  SetInitialTiles

#ifndef COMPILEDEMO
void SetInitialTiles (SInt16 background, Boolean doRoom)
{
	SInt16		i;

	if (background >= kUserBackground)
	{
		for (i = 0; i < kNumTiles; i++)
		{
			if (doRoom)
				thisRoom->tiles[i] = i;
			else
				tempTiles[i] = i;
		}
	}
	else
	{
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
				if (doRoom)
					thisRoom->tiles[i] = 1;
				else
					tempTiles[i] = 1;
			}
			if (doRoom)
			{
				thisRoom->tiles[0] = 0;
				thisRoom->tiles[kNumTiles - 1] = kNumTiles - 1;
			}
			else
			{
				tempTiles[0] = 0;
				tempTiles[kNumTiles - 1] = kNumTiles - 1;
			}
			break;

			case kSkywalk:
			for (i = 0; i < kNumTiles; i++)
			{
				if (doRoom)
					thisRoom->tiles[i] = i;
				else
					tempTiles[i] = i;
			}
			break;

			case kField:
			case kGarden:
			case kDirt:
			for (i = 0; i < kNumTiles; i++)
			{
				if (doRoom)
					thisRoom->tiles[i] = 0;
				else
					tempTiles[i] = 0;
			}
			break;

			case kMeadow:
			for (i = 0; i < kNumTiles; i++)
			{
				if (doRoom)
					thisRoom->tiles[i] = 1;
				else
					tempTiles[i] = 1;
			}
			break;

			case kRoof:
			for (i = 0; i < kNumTiles; i++)
			{
				if (doRoom)
					thisRoom->tiles[i] = 3;
				else
					tempTiles[i] = 3;
			}
			break;

			case kSky:
			for (i = 0; i < kNumTiles; i++)
			{
				if (doRoom)
					thisRoom->tiles[i] = 2;
				else
					tempTiles[i] = 2;
			}
			break;

			case kStratosphere:
			case kStars:
			for (i = 0; i < kNumTiles; i++)
			{
				if (doRoom)
					thisRoom->tiles[i] = i;
				else
					tempTiles[i] = i;
			}
			break;

			default:
			break;
		}
	}
}
#endif

//--------------------------------------------------------------  CreateNewRoom

#ifndef COMPILEDEMO
Boolean CreateNewRoom (HWND ownerWindow, SInt16 h, SInt16 v)
{
	SInt16		i, availableRoom;
	roomPtr		newRoomsPtr;

	CopyThisRoomToRoom();					// save off current room

	PasStringCopyC("Untitled Room", thisRoom->name);
	thisRoom->leftStart = 32;				// fill out fields of new room
	thisRoom->rightStart = 32;
	thisRoom->bounds = 0;
	thisRoom->unusedByte = 0;
	thisRoom->visited = false;
	thisRoom->background = lastBackground;
	SetInitialTiles(thisRoom->background, true);
	thisRoom->floor = v;
	thisRoom->suite = h;
	thisRoom->openings = 0;
	thisRoom->numObjects = 0;
	for (i = 0; i < kMaxRoomObs; i++)		// zero out all objects
		thisRoom->objects[i].what = kObjectIsEmpty;

	availableRoom = -1;						// assume no available rooms
	if (thisHouse->nRooms > 0)				// look for an empty room
		for (i = 0; i < thisHouse->nRooms; i++)
			if (thisHouse->rooms[i].suite == kRoomIsEmpty)
			{
				availableRoom = i;
				break;
			}

	if (availableRoom == -1)				// found no available rooms
	{
		if (thisHouse->nRooms < 0)
			thisHouse->nRooms = 0;
		newRoomsPtr = realloc(
			thisHouse->rooms,
			(thisHouse->nRooms + 1) * sizeof(*thisHouse->rooms)
		);
		if (newRoomsPtr == NULL)
		{
			YellowAlert(ownerWindow, kYellowUnaccounted, -1);
			return (false);
		}
		thisHouse->rooms = newRoomsPtr;
		thisHouse->nRooms++;				// increment nRooms
		numberRooms = thisHouse->nRooms;
		previousRoom = thisRoomNumber;
		thisRoomNumber = numberRooms - 1;
	}
	else
	{
		previousRoom = thisRoomNumber;
		thisRoomNumber = availableRoom;
	}

	if (noRoomAtAll)
		thisHouse->firstRoom = thisRoomNumber;

	CopyThisRoomToRoom();
	UpdateEditWindowTitle();
	noRoomAtAll = false;
	fileDirty = true;
	UpdateMenus(false);

	if (GetKeyState(VK_SHIFT) < 0)
		newRoomNow = false;
	else
		newRoomNow = autoRoomEdit;			// Flag to bring up RoomInfo

	return (true);
}
#endif

//--------------------------------------------------------------  ReadyBackground

void ReadyBackground (SInt16 theID, SInt16 *theTiles)
{
	Rect		src, dest;
	HBITMAP		thePicture;
	BITMAP		bmInfo;
	COLORREF	wasColor;
	Str255		theString;
	SInt16		i;

//	SetPort((GrafPtr)workSrcMap);

	if ((noRoomAtAll) || (!houseUnlocked))
	{
		wasColor = SetDCBrushColor(workSrcMap, LtGrayForeColor());
		Mac_PaintRect(workSrcMap, &workSrcRect, GetStockObject(DC_BRUSH));
		SetDCBrushColor(workSrcMap, wasColor);
		MoveToEx(workSrcMap, 10, 20, NULL);
		if (houseUnlocked)
			PasStringCopyC("No Rooms", theString);
		else
			PasStringCopyC("Nothing to show", theString);
		wasColor = SetTextColor(workSrcMap, blackColor);
		Mac_DrawString(workSrcMap, theString);
		SetTextColor(workSrcMap, wasColor);

		Mac_CopyBits(workSrcMap, backSrcMap,
				&workSrcRect, &workSrcRect, srcCopy, nil);
		return;
	}

	thePicture = GetPicture(theID);
	if (thePicture == NULL)
	{
		// TODO: Find out what the 'Date' resource type is
		//thePicture = (PicHandle)GetResource('Date', theID);
		//if (thePicture == NULL)
		{
			YellowAlert(mainWindow, kYellowNoBackground, 0);
			return;
		}
	}

	GetObject(thePicture, sizeof(bmInfo), &bmInfo);
	QSetRect(&dest, 0, 0, (SInt16)bmInfo.bmWidth, (SInt16)bmInfo.bmHeight);
	Mac_DrawPicture(workSrcMap, thePicture, &dest);
	DeleteObject(thePicture);

	QSetRect(&src, 0, 0, kTileWide, kTileHigh);
	QSetRect(&dest, 0, 0, kTileWide, kTileHigh);
	for (i = 0; i < kNumTiles; i++)
	{
		src.left = theTiles[i] * kTileWide;
		src.right = src.left + kTileWide;
		Mac_CopyBits(workSrcMap, backSrcMap,
				&src, &dest, srcCopy, nil);
		QOffsetRect(&dest, kTileWide, 0);
	}

	QSetRect(&src, 0, 0, kRoomWide, kTileHigh);
	QSetRect(&dest, 0, 0, kRoomWide, kTileHigh);
	Mac_CopyBits(backSrcMap, workSrcMap,
			&src, &dest, srcCopy, nil);
}

//--------------------------------------------------------------  ReflectCurrentRoom

void ReflectCurrentRoom (Boolean forceMapRedraw)
{
#ifndef COMPILEDEMO
	if (theMode != kEditMode)
		return;

	if ((noRoomAtAll) || (!houseUnlocked))
	{
		CenterMapOnRoom(64, 1);
		UpdateMapWindow();
	}
	else
	{
		if ((!ThisRoomVisibleOnMap()) || (forceMapRedraw))
		{
			CenterMapOnRoom(thisRoom->suite, thisRoom->floor);
			UpdateMapWindow();			// whole map window redrawm
		}
		else
		{
			FindNewActiveRoomRect();	// find newly selected room rect
			FlagMapRoomsForUpdate();	// redraw only the portions required
		}
	}
	GenerateRetroLinks();
	UpdateEditWindowTitle();
	ReadyBackground(thisRoom->background, thisRoom->tiles);
	GetThisRoomsObjRects();
	DrawThisRoomsObjects();
	Mac_InvalWindowRect(mainWindow, &mainWindowRect);
#endif
}

//--------------------------------------------------------------  CopyRoomToThisRoom

void CopyRoomToThisRoom (SInt16 roomNumber)
{
	if (roomNumber == -1)
		return;

	CopyThisRoomToRoom();			// copy back to house
	ForceThisRoom(roomNumber);		// load new room from house
}

//--------------------------------------------------------------  CopyThisRoomToRoom

void CopyThisRoomToRoom (void)
{
	if ((noRoomAtAll) || (thisRoomNumber == -1))
		return;

	thisHouse->rooms[thisRoomNumber] = *thisRoom;	// copy back to house
}

//--------------------------------------------------------------  ForceThisRoom

void ForceThisRoom (SInt16 roomNumber)
{
	if (roomNumber == -1)
		return;

	if (roomNumber < thisHouse->nRooms)
		*thisRoom = thisHouse->rooms[roomNumber];
	else
		YellowAlert(mainWindow, kYellowIllegalRoomNum, 0);

	previousRoom = thisRoomNumber;
	thisRoomNumber = roomNumber;
}

//--------------------------------------------------------------  RoomExists

Boolean RoomExists (SInt16 suite, SInt16 floor, SInt16 *roomNum)
{
	// pass in a suite and floor; returns true is it is a legitimate room
	SInt16		i;
	Boolean		foundIt;

	foundIt = false;

	if (suite < 0)
		return (foundIt);

	for (i = 0; i < numberRooms; i++)
	{
		if ((thisHouse->rooms[i].floor == floor) &&
				(thisHouse->rooms[i].suite == suite))
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
	SInt16		floor, suite, whoCares;
	Boolean		exists;

	exists = false;
	if (GetRoomFloorSuite(roomNum, &floor, &suite))
		exists = RoomExists(suite, floor, &whoCares);

	return (exists);
}

//--------------------------------------------------------------  DeleteRoom

void DeleteRoom (HWND ownerWindow, Boolean doWarn)
{
#ifndef COMPILEDEMO
	SInt16		wasFloor, wasSuite;
	Boolean		firstDeleted;

	if ((theMode != kEditMode) || (noRoomAtAll))
		return;

	if (doWarn)
	{
		if (!QueryDeleteRoom(ownerWindow))
			return;
	}

	DeselectObject();

	wasFloor = thisHouse->rooms[thisRoomNumber].floor;
	wasSuite = thisHouse->rooms[thisRoomNumber].suite;
	firstDeleted = (thisHouse->firstRoom == thisRoomNumber);	// is room "first"
	thisRoom->suite = kRoomIsEmpty;
	thisHouse->rooms[thisRoomNumber].suite = kRoomIsEmpty;

	noRoomAtAll = (RealRoomNumberCount() == 0);					// see if now no rooms
	if (noRoomAtAll)
		thisRoomNumber = kRoomIsEmpty;
	else
		SetToNearestNeighborRoom(wasFloor, wasSuite);

	if (firstDeleted)
	{
		thisHouse->firstRoom = thisRoomNumber;
	}

	newRoomNow = false;
	fileDirty = true;
	UpdateMenus(false);
	ReflectCurrentRoom(false);
#endif
}

//--------------------------------------------------------------  QueryDeleteRoom

#ifndef COMPILEDEMO
Boolean QueryDeleteRoom (HWND ownerWindow)
{
	SInt16 hitWhat;

	hitWhat = Alert(kDeleteRoomAlert, ownerWindow, NULL);
	if (hitWhat == kYesDoDeleteRoom)
		return (true);
	else
		return (false);
}
#endif

//--------------------------------------------------------------  DoesNeighborRoomExist

SInt16 DoesNeighborRoomExist (SInt16 whichNeighbor)
{
#ifdef COMPILEDEMO
	return(-1);
#else
	SInt16		newH, newV, newRoomNumber;

	if (theMode != kEditMode)
		return(-1);

	newH = thisRoom->suite;
	newV = thisRoom->floor;

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

	if (RoomExists(newH, newV, &newRoomNumber))
		return (newRoomNumber);
	else
		return (-1);
#endif
}

//--------------------------------------------------------------  SelectNeighborRoom

void SelectNeighborRoom (SInt16 whichNeighbor)
{
#ifndef COMPILEDEMO
	SInt16		newRoomNumber;

	newRoomNumber = DoesNeighborRoomExist(whichNeighbor);

	if (newRoomNumber != -1)
	{
		DeselectObject();
		CopyRoomToThisRoom(newRoomNumber);
		ReflectCurrentRoom(false);
	}
#endif
}

//--------------------------------------------------------------  GetNeighborRoomNumber

SInt16 GetNeighborRoomNumber (SInt16 which)
{
	SInt16		hDelta, vDelta, i;
	SInt16		roomH, roomV;
	SInt16		roomNum;

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

	roomNum = kRoomIsEmpty;
	roomH = thisHouse->rooms[thisRoomNumber].suite + hDelta;
	roomV = thisHouse->rooms[thisRoomNumber].floor + vDelta;

	for (i = 0; i < numberRooms; i++)
	{
		if ((thisHouse->rooms[i].suite == roomH) &&
				(thisHouse->rooms[i].floor == roomV))
		{
			roomNum = i;
			break;
		}
	}

	return (roomNum);
}

//--------------------------------------------------------------  SetToNearestNeighborRoom

void SetToNearestNeighborRoom (SInt16 wasFloor, SInt16 wasSuite)
{
	// searches in a clockwise spiral pattern (from thisRoom) for a…
	// legitimate neighboring room - then sets thisRoom to it
	SInt16		distance, h, v;
	SInt16		hStep, vStep;
	SInt16		testRoomNum, testH, testV;
	Boolean		finished;

	finished = false;
	distance = 1;	// we begin our walk a distance of one from source room
	h = -1;			// we begin with the neighbor to the left…
	v = 0;			// and on the same floor
	hStep = 0;		// we don't 'walk' left or right…
	vStep = -1;		// instead, we 'walk' up

	do
	{
		testH = wasSuite + h;
		testV = wasFloor + v;

		if (RoomExists(testH, testV, &testRoomNum))		// if a legitimate room
		{
			CopyRoomToThisRoom(testRoomNum);
			finished = true;
		}
		else
		{
			h += hStep;
			v += vStep;
			if ((h > distance) || (h < -distance) || (v > distance) || (v < -distance))
			{			// we have walked beyond the bounds of our spiral
				if ((hStep == -1) && (vStep == 0))	// we expand our spiral out
				{
					distance++;
					hStep = 0;						// begin travelling up again
					vStep = -1;
				}
				else
				{
					h -= hStep;						// first, back up a step
					v -= vStep;

					if (hStep == 0)					// we were travelling up or down
					{
						if (vStep == -1)			// we were travelling up…
							hStep = 1;				// so begin travelling right
						else						// we were travelling down…
							hStep = -1;				// so begin travelling left
						vStep = 0;
					}
					else
					{
						hStep = 0;					// begin travelling down
						vStep = 1;
					}
					h += hStep;						// proceed a step now
					v += vStep;
				}
			}
		}
	} while (!finished);
}

//--------------------------------------------------------------  GetRoomFloorSuite

Boolean GetRoomFloorSuite (SInt16 room, SInt16 *floor, SInt16 *suite)
{
	Boolean		isRoom;

	if (thisHouse->rooms[room].suite == kRoomIsEmpty)
	{
		*floor = 0;
		*suite = kRoomIsEmpty;
		isRoom = false;
	}
	else
	{
		*suite = thisHouse->rooms[room].suite;
		*floor = thisHouse->rooms[room].floor;
		isRoom = true;
	}

	return (isRoom);
}

//--------------------------------------------------------------  GetRoomNumber

SInt16 GetRoomNumber (SInt16 floor, SInt16 suite)
{
	// pass in a floor and suite; returns the room index into the house file
	SInt16		roomNum, i;

	roomNum = kRoomIsEmpty;

	for (i = 0; i < numberRooms; i++)
	{
		if ((thisHouse->rooms[i].suite == suite) &&
				(thisHouse->rooms[i].floor == floor))
		{
			roomNum = i;
			break;
		}
	}

	return (roomNum);
}

//--------------------------------------------------------------  IsRoomAStructure

Boolean	IsRoomAStructure (SInt16 roomNum)
{
	Boolean		isStructure;

	if (roomNum == kRoomIsEmpty)
		return (false);

	if (thisHouse->rooms[roomNum].background >= kUserBackground)
	{
		if (thisHouse->rooms[roomNum].bounds != 0)
		{
			isStructure = ((thisHouse->rooms[roomNum].bounds & 32) == 32);
		}
		else
		{
			if (thisHouse->rooms[roomNum].background < kUserStructureRange)
				isStructure = true;
			else
				isStructure = false;
		}
	}
	else
	{
		switch (thisHouse->rooms[roomNum].background)
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
	SInt16		whichBack, leftTile, rightTile;
	SInt16		boundsCode;

	whichBack = thisRoom->background;
	leftTile = thisRoom->tiles[0];
	rightTile = thisRoom->tiles[kNumTiles - 1];

	if (whichBack >= kUserBackground)
	{
		if (thisRoom->bounds != 0)
			boundsCode = thisRoom->bounds >> 1;
		else
			boundsCode = GetOriginalBounding(whichBack);
		leftOpen = ((boundsCode & 0x0001) == 0x0001);
		rightOpen = ((boundsCode & 0x0004) == 0x0004);

		if (leftOpen)
			leftThresh = kNoLeftWallLimit;
		else
			leftThresh = kLeftWallLimit;

		if (rightOpen)
			rightThresh = kNoRightWallLimit;
		else
			rightThresh = kRightWallLimit;
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
				leftThresh = kLeftWallLimit;
			else
				leftThresh = kNoLeftWallLimit;
			if (rightTile == (kNumTiles - 1))
				rightThresh = kRightWallLimit;
			else
				rightThresh = kNoRightWallLimit;
			leftOpen = (leftTile != 0);
			rightOpen = (rightTile != (kNumTiles - 1));
			break;

			case kDirt:
			if (leftTile == 1)
				leftThresh = kLeftWallLimit;
			else
				leftThresh = kNoLeftWallLimit;
			if (rightTile == (kNumTiles - 1))
				rightThresh = kRightWallLimit;
			else
				rightThresh = kNoRightWallLimit;
			leftOpen = (leftTile != 0);
			rightOpen = (rightTile != (kNumTiles - 1));
			break;

			case kMeadow:
			if (leftTile == 6)
				leftThresh = kLeftWallLimit;
			else
				leftThresh = kNoLeftWallLimit;
			if (rightTile == 7)
				rightThresh = kRightWallLimit;
			else
				rightThresh = kNoRightWallLimit;
			leftOpen = (leftTile != 6);
			rightOpen = (rightTile != 7);
			break;

			case kGarden:
			case kSkywalk:
			case kField:
			case kStratosphere:
			case kStars:
			leftThresh = kNoLeftWallLimit;
			rightThresh = kNoRightWallLimit;
			leftOpen = true;
			rightOpen = true;
			break;

			default:
			if (leftTile == 0)
				leftThresh = kLeftWallLimit;
			else
				leftThresh = kNoLeftWallLimit;

			if (rightTile == (kNumTiles - 1))
				rightThresh = kRightWallLimit;
			else
				rightThresh = kNoRightWallLimit;

			leftOpen = (leftTile != 0);
			rightOpen = (rightTile != (kNumTiles - 1));
			break;
		}
	}

	if (DoesRoomHaveFloor())
		bottomOpen = false;
	else
		bottomOpen = true;

	if (DoesRoomHaveCeiling())
		topOpen = false;
	else
		topOpen = true;
}

//--------------------------------------------------------------  GetOriginalBounding

SInt16 GetOriginalBounding (SInt16 theID)
{
	boundsType	boundsRes;
	HRSRC		resBlock;
	DWORD		resByteSize;
	HGLOBAL		resHandle;
	LPVOID		resPointer;
	byteio		byteReader;
	SInt16		boundCode;

	if (houseResFork == NULL)
		return 0;

	resPointer = NULL;
	resByteSize = 0;
	resBlock = FindResource(houseResFork, MAKEINTRESOURCE(theID), L"BOUNDS");
	if (resBlock != NULL)
	{
		resByteSize = SizeofResource(houseResFork, resBlock);
		if (resByteSize != 0)
		{
			resHandle = LoadResource(houseResFork, resBlock);
			if (resHandle != NULL)
			{
				resPointer = LockResource(resHandle);
			}
		}
	}
	if (resPointer == NULL)
	{
		if (PictIDExists(theID))
			YellowAlert(mainWindow, kYellowNoBoundsRes, 0);
		boundCode = 0;
	}
	else
	{
		boundCode = 0;
		if (!byteio_init_memory_reader(&byteReader, resPointer, resByteSize))
			RedAlert(kErrNoMemory);
		if (ReadBoundsType(&byteReader, &boundsRes))
		{
			if (boundsRes.left)
				boundCode += 1;
			if (boundsRes.top)
				boundCode += 2;
			if (boundsRes.right)
				boundCode += 4;
			if (boundsRes.bottom)
				boundCode += 8;
		}
		byteio_close(&byteReader);
	}

	return (boundCode);
}

//--------------------------------------------------------------  GetNumberOfLights

SInt16 GetNumberOfLights (SInt16 where)
{
	SInt16		i, count;

	if (theMode == kEditMode)
	{
		switch (thisRoom->background)
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
			if ((thisRoom->tiles[0] == 0) && (thisRoom->tiles[1] == 0) &&
					(thisRoom->tiles[2] == 0) && (thisRoom->tiles[3] == 0) &&
					(thisRoom->tiles[4] == 0) && (thisRoom->tiles[5] == 0) &&
					(thisRoom->tiles[6] == 0) && (thisRoom->tiles[7] == 0))
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
				switch (thisRoom->objects[i].what)
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
					if (thisRoom->objects[i].data.f.initial)
						count++;
					break;
				}
			}
		}
	}
	else
	{
		if (where < 0 || where >= thisHouse->nRooms)
			return 0;
		switch (thisHouse->rooms[where].background)
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
			if ((thisHouse->rooms[where].tiles[0] == 0) &&
					(thisHouse->rooms[where].tiles[1] == 0) &&
					(thisHouse->rooms[where].tiles[2] == 0) &&
					(thisHouse->rooms[where].tiles[3] == 0) &&
					(thisHouse->rooms[where].tiles[4] == 0) &&
					(thisHouse->rooms[where].tiles[5] == 0) &&
					(thisHouse->rooms[where].tiles[6] == 0) &&
					(thisHouse->rooms[where].tiles[7] == 0))
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
				switch (thisHouse->rooms[where].objects[i].what)
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
					if (thisHouse->rooms[where].objects[i].data.f.state)
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
	SInt16		boundsCode;
	Boolean		hasFloor;

	if (thisRoom->background >= kUserBackground)
	{
		if (thisRoom->bounds != 0)			// is this a version 2.0 house?
			boundsCode = (thisRoom->bounds >> 1);
		else
			boundsCode = GetOriginalBounding(thisRoom->background);
		hasFloor = ((boundsCode & 0x0008) != 0x0008);
	}
	else
	{
		switch (thisRoom->background)
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
	SInt16		boundsCode;
	Boolean		hasFloor;

	if (thisRoom->background >= kUserBackground)
	{
		if (thisRoom->bounds != 0)			// is this a version 2.0 house?
			boundsCode = (thisRoom->bounds >> 1);
		else
			boundsCode = GetOriginalBounding(thisRoom->background);
		hasFloor = ((boundsCode & 0x0008) != 0x0008);
	}
	else
	{
		switch (thisRoom->background)
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
	SInt16		boundsCode;
	Boolean		hasCeiling;

	if (thisRoom->background >= kUserBackground)
	{
		if (thisRoom->bounds != 0)			// is this a version 2.0 house?
			boundsCode = (thisRoom->bounds >> 1);
		else
			boundsCode = GetOriginalBounding(thisRoom->background);
		hasCeiling = ((boundsCode & 0x0002) != 0x0002);
	}
	else
	{
		switch (thisRoom->background)
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

