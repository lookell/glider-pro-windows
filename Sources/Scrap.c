#define GP_USE_WINAPI_H

#include "Scrap.h"

//============================================================================
//----------------------------------------------------------------------------
//                                  Scrap.c
//----------------------------------------------------------------------------
//============================================================================


#include "GliderStructs.h"
#include "House.h"
#include "HouseIO.h"
#include "Link.h"
#include "Macintosh.h"
#include "MainWindow.h"
#include "Map.h"
#include "Menu.h"
#include "ObjectAdd.h"
#include "ObjectEdit.h"
#include "Objects.h"
#include "RectUtils.h"
#include "Room.h"
#include "WinAPI.h"

#include <commctrl.h>


HIMAGELIST GetRoomDragImageList (const Rect *roomSrc, SInt16 roomNumber);


Boolean hasScrap;
Boolean scrapIsARoom;

static roomType savedRoomScrap;
static objectType savedObjectScrap;


//==============================================================  Functions
//--------------------------------------------------------------  PutRoomScrap
// This function copies the current room into the clipboard.

#ifndef COMPILEDEMO
void PutRoomScrap (void)
{
	if (thisRoomNumber >= 0 && thisRoomNumber < thisHouse.nRooms)
	{
		savedRoomScrap = *thisRoom;
		if (!hasScrap)
		{
			hasScrap = true;
			UpdateMenus(false);
		}
		scrapIsARoom = true;
	}
}
#endif

//--------------------------------------------------------------  PutObjectScrap
// This function copies the currently selected object into the clipboard.

#ifndef COMPILEDEMO
void PutObjectScrap (void)
{
	if (objActive >= 0 && objActive < kMaxRoomObs)
	{
		savedObjectScrap = thisRoom->objects[objActive];
		if (!hasScrap)
		{
			hasScrap = true;
			UpdateMenus(false);
		}
		scrapIsARoom = false;
	}
}
#endif

//--------------------------------------------------------------  GetRoomScrap
// This function pastes a room from the clipboard.

#ifndef COMPILEDEMO
void GetRoomScrap (void)
{
	SInt16 srcFloor;
	SInt16 srcSuite;
	SInt16 packedSrcCombo;
	SInt16 destFloor;
	SInt16 destSuite;
	SInt16 packedDestCombo;
	objectType *theObject;
	SInt16 i;

	if (hasScrap && scrapIsARoom)
	{
		DeselectObject();

		srcFloor = savedRoomScrap.floor;
		srcSuite = savedRoomScrap.suite;
		packedSrcCombo = MergeFloorSuite(srcFloor + kNumUndergroundFloors, srcSuite);
		destFloor = thisRoom->floor;
		destSuite = thisRoom->suite;
		packedDestCombo = MergeFloorSuite(destFloor + kNumUndergroundFloors, destSuite);

		*thisRoom = savedRoomScrap;
		thisRoom->floor = destFloor;
		thisRoom->suite = destSuite;

		// fix up links within this room
		for (i = 0; i < kMaxRoomObs; i++)
		{
			theObject = &thisRoom->objects[i];
			if (ObjectIsLinkSwitch(theObject))
			{
				if (theObject->data.e.where == packedSrcCombo)
				{
					theObject->data.e.where = packedDestCombo;
				}
			}
			else if (ObjectIsLinkTransport(theObject))
			{
				if (theObject->data.d.where == packedSrcCombo)
				{
					theObject->data.d.where = packedDestCombo;
				}
			}
		}

		CopyThisRoomToRoom();
		ReflectCurrentRoom(false);
		fileDirty = true;
		UpdateMenus(false);
	}
}
#endif

//--------------------------------------------------------------  GetObjectScrap
// This function pastes an object from the clipboard.

#ifndef COMPILEDEMO
void GetObjectScrap (void)
{
	objectType tempObject;
	Point noPoint;

	if (hasScrap && !scrapIsARoom)
	{
		DeselectObject();

		noPoint.h = 100;
		noPoint.v = 100;
		tempObject = savedObjectScrap;
		if (AddNewObject(mainWindow, noPoint, tempObject.what, false))
		{
			thisRoom->objects[objActive] = tempObject;
			ReadyBackground(thisRoom->background, thisRoom->tiles);
			GetThisRoomsObjRects();
			DrawThisRoomsObjects();
			Mac_InvalWindowRect(mainWindow, &mainWindowRect);
			StartMarqueeForActiveObject();
		}
	}
}
#endif

//--------------------------------------------------------------  SeeIfValidScrapAvailable

#ifndef COMPILEDEMO
void SeeIfValidScrapAvailable (Boolean updateMenus)
{
	if (updateMenus)
	{
		UpdateClipboardMenus();
	}
}
#endif

//--------------------------------------------------------------  GetRoomDragImageList

HIMAGELIST GetRoomDragImageList (const Rect *roomSrc, SInt16 roomNumber)
{
	SInt16 smallWidth;
	SInt16 smallHeight;
	HBITMAP roomImage;
	HDC tempDC;
	HIMAGELIST himl;

	smallWidth = roomSrc->right - roomSrc->left;
	smallHeight = roomSrc->bottom - roomSrc->top;

	roomImage = NULL;
	tempDC = CreateCompatibleDC(NULL);
	if (tempDC != NULL)
	{
		BITMAPINFO bmi = { 0 };
		void *roomImageBits;

		bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
		bmi.bmiHeader.biWidth = smallWidth;
		bmi.bmiHeader.biHeight = smallHeight;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = 0;
		bmi.bmiHeader.biXPelsPerMeter = 0;
		bmi.bmiHeader.biYPelsPerMeter = 0;
		bmi.bmiHeader.biClrUsed = 0;
		bmi.bmiHeader.biClrImportant = 0;
		roomImage = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &roomImageBits, NULL, 0);
		if (roomImage != NULL)
		{
			SInt16 backgroundID;
			Rect roomImageRect;

			SaveDC(tempDC);
			SelectObject(tempDC, roomImage);
			SetStretchBltMode(tempDC, HALFTONE);
			SetBrushOrgEx(tempDC, 0, 0, NULL);
			if (roomNumber >= 0 && roomNumber < thisHouse.nRooms)
			{
				backgroundID = thisHouse.rooms[roomNumber].background;
			}
			else
			{
				backgroundID = kSimpleRoom;
			}
			QSetRect(&roomImageRect, 0, 0, smallWidth, smallHeight);
			DrawMapRoomImage(tempDC, backgroundID, &roomImageRect);
			RestoreDC(tempDC, -1);
		}
		DeleteDC(tempDC);
	}

	himl = NULL;
	if (roomImage != NULL)
	{
		himl = ImageList_Create(smallWidth, smallHeight, ILC_COLOR32, 0, 1);
		if (himl != NULL)
		{
			if (ImageList_Add(himl, roomImage, NULL) < 0)
			{
				ImageList_Destroy(himl);
				himl = NULL;
			}
		}
		DeleteObject(roomImage);
	}
	return himl;
}

//--------------------------------------------------------------  DragRoom

void DragRoom (SInt16 clickX, SInt16 clickY, const Rect *roomSrc, SInt16 roomNumber)
{
	POINT startPoint;
	POINT dragPoint;
	HIMAGELIST dragImageList;
	MSG msg;
	RECT clientRect;

	dragPoint.x = clickX;
	dragPoint.y = clickY;
	ClientToScreen(mapWindow, &dragPoint);
	if (!DragDetect(mapWindow, dragPoint))
	{
		return;
	}

	dragImageList = GetRoomDragImageList(roomSrc, roomNumber);
	if (dragImageList == NULL)
	{
		YellowAlert(mainWindow, kYellowNoMemory, 0);
		return;
	}

	UpdateMapWindow();

	SetCapture(mapWindow);
	ImageList_BeginDrag(dragImageList, 0, clickX - roomSrc->left, clickY - roomSrc->top);
	ImageList_DragEnter(NULL, dragPoint.x, dragPoint.y);
	startPoint = dragPoint;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			dragPoint.x = GET_X_LPARAM(msg.lParam);
			dragPoint.y = GET_Y_LPARAM(msg.lParam);
			ClientToScreen(msg.hwnd, &dragPoint);
			ImageList_DragMove(dragPoint.x, dragPoint.y);
			break;

		case WM_LBUTTONUP:
			if (GetCapture() == mapWindow)
			{
				ReleaseCapture();
			}
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
			// ignore these events so that the current room doesn't change
			if (msg.wParam == VK_ESCAPE)
			{
				dragPoint = startPoint;
				if (GetCapture() == mapWindow)
				{
					ReleaseCapture();
				}
			}
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			// ignore these events so that the current room doesn't change
			break;

		default:
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			break;
		}
		if (GetCapture() != mapWindow)
		{
			break;
		}
	}

	ImageList_DragLeave(NULL);
	ImageList_EndDrag();
	ImageList_Destroy(dragImageList);

	ScreenToClient(mapWindow, &dragPoint);
	GetClientRect(mapWindow, &clientRect);
	if (PtInRect(&clientRect, dragPoint))
	{
		Point pt;
		pt.h = (SInt16)dragPoint.x;
		pt.v = (SInt16)dragPoint.y;
		MoveRoom(pt);
	}
}

