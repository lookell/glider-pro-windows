//============================================================================
//----------------------------------------------------------------------------
//                                  Scrap.c
//----------------------------------------------------------------------------
//============================================================================

#include "Scrap.h"

#include "GliderStructs.h"
#include "House.h"
#include "HouseIO.h"
#include "Link.h"
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

Boolean g_hasScrap;
Boolean g_scrapIsARoom;

static roomType g_savedRoomScrap;
static objectType g_savedObjectScrap;

//==============================================================  Functions
//--------------------------------------------------------------  PutRoomScrap
// This function copies the current room into the clipboard.

void PutRoomScrap (void)
{
	if (g_thisRoomNumber >= 0 && g_thisRoomNumber < g_thisHouse.nRooms)
	{
		g_savedRoomScrap = *g_thisRoom;
		if (!g_hasScrap)
		{
			g_hasScrap = true;
			UpdateMenus(false);
		}
		g_scrapIsARoom = true;
	}
}

//--------------------------------------------------------------  PutObjectScrap
// This function copies the currently selected object into the clipboard.

void PutObjectScrap (void)
{
	if (g_objActive >= 0 && g_objActive < kMaxRoomObs)
	{
		g_savedObjectScrap = g_thisRoom->objects[g_objActive];
		if (!g_hasScrap)
		{
			g_hasScrap = true;
			UpdateMenus(false);
		}
		g_scrapIsARoom = false;
	}
}

//--------------------------------------------------------------  GetRoomScrap
// This function pastes a room from the clipboard.

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

	if (g_hasScrap && g_scrapIsARoom)
	{
		DeselectObject();

		srcFloor = g_savedRoomScrap.floor;
		srcSuite = g_savedRoomScrap.suite;
		packedSrcCombo = MergeFloorSuite(&g_thisHouse, srcFloor, srcSuite);
		destFloor = g_thisRoom->floor;
		destSuite = g_thisRoom->suite;
		packedDestCombo = MergeFloorSuite(&g_thisHouse, destFloor, destSuite);

		*g_thisRoom = g_savedRoomScrap;
		g_thisRoom->floor = destFloor;
		g_thisRoom->suite = destSuite;

		// fix up links within this room
		for (i = 0; i < kMaxRoomObs; i++)
		{
			theObject = &g_thisRoom->objects[i];
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
		g_fileDirty = true;
		UpdateMenus(false);
	}
}

//--------------------------------------------------------------  GetObjectScrap
// This function pastes an object from the clipboard.

void GetObjectScrap (void)
{
	objectType tempObject;
	Point noPoint;

	if (g_hasScrap && !g_scrapIsARoom)
	{
		DeselectObject();

		noPoint.h = 100;
		noPoint.v = 100;
		tempObject = g_savedObjectScrap;
		if (AddNewObject(g_mainWindow, noPoint, tempObject.what, false))
		{
			g_thisRoom->objects[g_objActive] = tempObject;
			ReadyBackground(g_thisRoom->background, g_thisRoom->tiles);
			GetThisRoomsObjRects();
			DrawThisRoomsObjects();
			InvalidateRect(g_mainWindow, NULL, TRUE);
			StartMarqueeForActiveObject();
		}
	}
}

//--------------------------------------------------------------  SeeIfValidScrapAvailable

void SeeIfValidScrapAvailable (Boolean updateMenus)
{
	if (updateMenus)
	{
		UpdateClipboardMenus();
	}
}

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
			if (roomNumber >= 0 && roomNumber < g_thisHouse.nRooms)
			{
				backgroundID = g_thisHouse.rooms[roomNumber].background;
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
		himl = ImageList_Create(smallWidth, smallHeight, ILC_COLOR32 | ILC_MASK, 0, 1);
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
	ClientToScreen(g_mapWindow, &dragPoint);
	if (!DragDetect(g_mapWindow, dragPoint))
	{
		return;
	}

	dragImageList = GetRoomDragImageList(roomSrc, roomNumber);
	if (dragImageList == NULL)
	{
		YellowAlert(g_mainWindow, kYellowNoMemory, 0);
		return;
	}

	UpdateMapWindow();

	SetCapture(g_mapWindow);
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
			if (GetCapture() == g_mapWindow)
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
				if (GetCapture() == g_mapWindow)
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
		if (GetCapture() != g_mapWindow)
		{
			break;
		}
	}

	ImageList_DragLeave(NULL);
	ImageList_EndDrag();
	ImageList_Destroy(dragImageList);

	ScreenToClient(g_mapWindow, &dragPoint);
	GetClientRect(g_mapWindow, &clientRect);
	if (PtInRect(&clientRect, dragPoint))
	{
		Point pt;
		pt.h = (SInt16)dragPoint.x;
		pt.v = (SInt16)dragPoint.y;
		MoveRoom(pt);
	}
}
