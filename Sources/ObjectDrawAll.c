#include "ObjectDrawAll.h"

//============================================================================
//----------------------------------------------------------------------------
//                              ObjectDrawAll.c
//----------------------------------------------------------------------------
//============================================================================

#include "DynamicMaps.h"
#include "Dynamics.h"
#include "Environ.h"
#include "Grease.h"
#include "House.h"
#include "HouseIO.h"
#include "Link.h"
#include "MainWindow.h"
#include "Music.h"
#include "ObjectDraw.h"
#include "ObjectRects.h"
#include "Objects.h"
#include "Play.h"
#include "RectUtils.h"
#include "Render.h"
#include "Room.h"
#include "RoomGraphics.h"

//==============================================================  Functions
//--------------------------------------------------------------  DrawARoomsObjects

void DrawARoomsObjects (SInt16 neighbor, Boolean redraw)
{
	objectType	thisObject;
	Rect		whoCares, itsRect, rectA, rectB, testRect;
#ifdef COMPILEQT
	HRGN		theRgn;
#endif
	SInt16		i, legit, dynamicNum, n;
	SInt16		floor, suite, room, obj;
	Boolean		isLit;

	if (neighbor < 0 || neighbor >= 9)
		return;
	if ((g_localNumbers[neighbor] < 0) || (g_localNumbers[neighbor] >= g_thisHouse.nRooms))
		return;

	testRect = g_houseRect;
	ZeroRectCorner(&testRect);
	isLit = (g_numLights > 0);

	for (i = 0; i < kMaxRoomObs; i++)
	{
		dynamicNum = -1;
		legit = -1;

		if (IsThisValid(g_localNumbers[neighbor], i))
		{
			thisObject = g_thisHouse.rooms[g_localNumbers[neighbor]].objects[i];
			switch (thisObject.what)
			{
				case kObjectIsEmpty:
				break;

				case kFloorVent:
				case kCeilingVent:
				case kFloorBlower:
				case kCeilingBlower:
				case kSewerGrate:
				case kLeftFan:
				case kRightFan:
				case kGrecoVent:
				case kSewerBlower:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if ((QSectRect(&itsRect, &testRect, &whoCares)) && isLit)
					DrawSimpleBlowers(thisObject.what, &itsRect);
				break;

				case kTaper:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					if (isLit)
						DrawSimpleBlowers(thisObject.what, &itsRect);
					if (neighbor == kCentralRoom)
					{
						if (redraw)
							ReBackUpFlames(g_localNumbers[neighbor], i);
						else
							AddCandleFlame(g_localNumbers[neighbor], i,
									itsRect.left + 10, itsRect.top + 7);
					}
					else
					{
						QSetRect(&rectA, 0, 0, 16, 15);
						QOffsetRect(&rectA, itsRect.left + 10 - 8, itsRect.top + 7 - 15);
						rectB = g_localRoomsDest[kCentralRoom];
						rectB.top -= kFloorSupportTall;
						rectB.bottom += kFloorSupportTall;
						if (!QSectRect(&rectA, &rectB, &whoCares))
						{
							if (redraw)
								ReBackUpFlames(g_localNumbers[neighbor], i);
							else
								AddCandleFlame(g_localNumbers[neighbor], i,
										itsRect.left + 10, itsRect.top + 7);
						}
					}
				}
				break;

				case kCandle:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					if (isLit)
						DrawSimpleBlowers(thisObject.what, &itsRect);
					if (neighbor == kCentralRoom)
					{
						if (redraw)
							ReBackUpFlames(g_localNumbers[neighbor], i);
						else
							AddCandleFlame(g_localNumbers[neighbor], i,
									itsRect.left + 14, itsRect.top + 7);
					}
					else
					{
						QSetRect(&rectA, 0, 0, 16, 15);
						QOffsetRect(&rectA, itsRect.left + 14 - 8, itsRect.top + 7 - 15);
						rectB = g_localRoomsDest[kCentralRoom];
						rectB.top -= kFloorSupportTall;
						rectB.bottom += kFloorSupportTall;
						if (!QSectRect(&rectA, &rectB, &whoCares))
						{
							if (redraw)
								ReBackUpFlames(g_localNumbers[neighbor], i);
							else
								AddCandleFlame(g_localNumbers[neighbor], i,
										itsRect.left + 14, itsRect.top + 7);
						}
					}
				}
				break;

				case kStubby:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					if (isLit)
						DrawSimpleBlowers(thisObject.what, &itsRect);
					if (neighbor == kCentralRoom)
					{
						if (redraw)
							ReBackUpFlames(g_localNumbers[neighbor], i);
						else
							AddCandleFlame(g_localNumbers[neighbor], i,
									itsRect.left + 9, itsRect.top + 7);
					}
					else
					{
						QSetRect(&rectA, 0, 0, 16, 15);
						QOffsetRect(&rectA, itsRect.left + 9 - 8, itsRect.top + 7 - 15);
						rectB = g_localRoomsDest[kCentralRoom];
						rectB.top -= kFloorSupportTall;
						rectB.bottom += kFloorSupportTall;
						if (!QSectRect(&rectA, &rectB, &whoCares))
						{
							if (redraw)
								ReBackUpFlames(g_localNumbers[neighbor], i);
							else
								AddCandleFlame(g_localNumbers[neighbor], i,
										itsRect.left + 9, itsRect.top + 7);
						}
					}
				}
				break;

				case kTiki:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (isLit)
					DrawTiki(&itsRect, g_playOriginV + VerticalRoomOffset(neighbor));
				if (redraw)
					ReBackUpTikiFlames(g_localNumbers[neighbor], i);
				else
					AddTikiFlame(g_localNumbers[neighbor], i,
							itsRect.left + 10, itsRect.top - 9);
				break;

				case kBBQ:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					if (isLit)
						DrawPictSansWhiteObject(thisObject.what, &itsRect);
					if (redraw)
						ReBackUpBBQCoals(g_localNumbers[neighbor], i);
					else
						AddBBQCoals(g_localNumbers[neighbor], i,
								itsRect.left + 16, itsRect.top + 9);
				}
				break;

				case kInvisBlower:
				case kLiftArea:
				break;

				case kTable:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (isLit)
					DrawTable(&itsRect, g_playOriginV);
				break;

				case kShelf:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (isLit)
					DrawShelf(&itsRect);
				break;

				case kCabinet:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if ((QSectRect(&itsRect, &testRect, &whoCares)) && isLit)
					DrawCabinet(&itsRect);
				break;

				case kFilingCabinet:
				case kOzma:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if ((QSectRect(&itsRect, &testRect, &whoCares)) && isLit)
					DrawPictObject(thisObject.what, &itsRect);
				break;

				case kWasteBasket:
				case kMilkCrate:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if ((QSectRect(&itsRect, &testRect, &whoCares)) && isLit)
					DrawSimpleFurniture(thisObject.what, &itsRect);
				break;

				case kCounter:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if ((QSectRect(&itsRect, &testRect, &whoCares)) && isLit)
					DrawCounter(&itsRect);
				break;

				case kDresser:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (isLit)
					DrawDresser(&itsRect);
				break;

				case kDeckTable:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (isLit)
					DrawDeckTable(&itsRect, g_playOriginV);
				break;

				case kStool:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (isLit)
					DrawStool(&itsRect, g_playOriginV + VerticalRoomOffset(neighbor));
				break;

				case kInvisObstacle:
				break;

				case kManhole:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					AddTempManholeRect(&itsRect);
					if (isLit)
						DrawPictSansWhiteObject(thisObject.what, &itsRect);
				}
				break;

				case kInvisBounce:
				break;

				case kRedClock:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					if (redraw)
						legit = ReBackUpSavedMap(&itsRect, g_localNumbers[neighbor], i);
					else
						legit = BackUpToSavedMap(&itsRect, g_localNumbers[neighbor], i);
					if (legit != -1)
						DrawRedClock(&itsRect);
				}
				break;

				case kBlueClock:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					if (redraw)
						legit = ReBackUpSavedMap(&itsRect, g_localNumbers[neighbor], i);
					else
						legit = BackUpToSavedMap(&itsRect, g_localNumbers[neighbor], i);
					if (legit != -1)
						DrawBlueClock(&itsRect);
				}
				break;

				case kYellowClock:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					if (redraw)
						legit = ReBackUpSavedMap(&itsRect, g_localNumbers[neighbor], i);
					else
						legit = BackUpToSavedMap(&itsRect, g_localNumbers[neighbor], i);
					if (legit != -1)
						DrawYellowClock(&itsRect);
				}
				break;

				case kCuckoo:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					if (redraw)
						legit = ReBackUpSavedMap(&itsRect, g_localNumbers[neighbor], i);
					else
						legit = BackUpToSavedMap(&itsRect, g_localNumbers[neighbor], i);
					if (legit != -1)
					{
						DrawCuckoo(&itsRect);
						if (redraw)
							ReBackUpPendulum(g_localNumbers[neighbor], i);
						else
							AddPendulum(g_localNumbers[neighbor], i,
									itsRect.left + 4, itsRect.top + 46);
					}
				}
				break;

				case kPaper:
				case kBattery:
				case kBands:
				case kHelium:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					if (redraw)
						legit = ReBackUpSavedMap(&itsRect, g_localNumbers[neighbor], i);
					else
						legit = BackUpToSavedMap(&itsRect, g_localNumbers[neighbor], i);
					if (legit != -1)
						DrawSimplePrizes(thisObject.what, &itsRect);
				}
				break;

				case kGreaseRt:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (thisObject.data.c.state)		// standing
				{
					if (QSectRect(&itsRect, &testRect, &whoCares))
					{
						if (redraw)
							dynamicNum = ReBackUpGrease(g_localNumbers[neighbor], i);
						else
							dynamicNum = AddGrease(g_localNumbers[neighbor], i,
									itsRect.left, itsRect.top,
									thisObject.data.c.length, true);
						if (dynamicNum != -1)
							DrawGreaseRt(&itsRect, thisObject.data.c.length, true);
					}
				}
				else								// fallen
					DrawGreaseRt(&itsRect, thisObject.data.c.length, false);
				break;

				case kGreaseLf:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (thisObject.data.c.state)
				{
					if (QSectRect(&itsRect, &testRect, &whoCares))
					{
						if (redraw)
							dynamicNum = ReBackUpGrease(g_localNumbers[neighbor], i);
						else
							dynamicNum = AddGrease(g_localNumbers[neighbor], i,
									itsRect.left, itsRect.top,
									thisObject.data.c.length, false);
						if (dynamicNum != -1)
							DrawGreaseLf(&itsRect, thisObject.data.c.length, true);
					}
				}
				else
					DrawGreaseLf(&itsRect, thisObject.data.c.length, false);
				break;

				case kFoil:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					if (redraw)
						legit = ReBackUpSavedMap(&itsRect, g_localNumbers[neighbor], i);
					else
						legit = BackUpToSavedMap(&itsRect, g_localNumbers[neighbor], i);
					if (legit != -1)
						DrawFoil(&itsRect);
				}
				break;

				case kInvisBonus:
				case kSlider:
				break;

				case kStar:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					if (redraw)
						legit = ReBackUpSavedMap(&itsRect, g_localNumbers[neighbor], i);
					else
						legit = BackUpToSavedMap(&itsRect, g_localNumbers[neighbor], i);
					if (legit != -1)
					{
						if (redraw)
							ReBackUpStar(g_localNumbers[neighbor], i);
						else
							AddStar(g_localNumbers[neighbor], i, itsRect.left,
									itsRect.top);
						DrawSimplePrizes(thisObject.what, &itsRect);
					}
				}
				break;

				case kSparkle:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					if ((!redraw) && (neighbor == kCentralRoom))
					{
						rectA = itsRect;
						QOffsetRect(&rectA, -g_playOriginH, -g_playOriginV);
						dynamicNum = AddDynamicObject(kSparkle, &rectA, &thisObject,
								g_localNumbers[neighbor], i, thisObject.data.c.state);
					}
				}
				break;

				case kUpStairs:
				case kDoorInLf:
				case kDoorInRt:
				case kWindowInLf:
				case kWindowInRt:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
					DrawPictSansWhiteObject(thisObject.what, &itsRect);
				break;

				case kDownStairs:
				case kDoorExRt:
				case kDoorExLf:
				case kWindowExRt:
				case kWindowExLf:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
					DrawPictObject(thisObject.what, &itsRect);
				break;

				case kMailboxLf:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				DrawMailboxLeft(&itsRect, g_playOriginV + VerticalRoomOffset(neighbor));
				break;

				case kMailboxRt:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				DrawMailboxRight(&itsRect, g_playOriginV + VerticalRoomOffset(neighbor));
				break;

				case kFloorTrans:
				case kCeilingTrans:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
					DrawSimpleTransport(thisObject.what, &itsRect);
				break;

				case kInvisTrans:
				case kDeluxeTrans:
				break;

				case kLightSwitch:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					ExtractFloorSuite(thisObject.data.e.where, &floor, &suite);
					room = GetRoomNumber(floor, suite);
					obj = (SInt16)thisObject.data.e.who;
					DrawLightSwitch(&itsRect, GetObjectState(room, obj));
				}
				dynamicNum = g_masterObjects[i].hotNum;
				break;

				case kMachineSwitch:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					ExtractFloorSuite(thisObject.data.e.where, &floor, &suite);
					room = GetRoomNumber(floor, suite);
					obj = (SInt16)thisObject.data.e.who;
					DrawMachineSwitch(&itsRect, GetObjectState(room, obj));
				}
				dynamicNum = g_masterObjects[i].hotNum;
				break;

				case kThermostat:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					ExtractFloorSuite(thisObject.data.e.where, &floor, &suite);
					room = GetRoomNumber(floor, suite);
					obj = (SInt16)thisObject.data.e.who;
					DrawThermostat(&itsRect, GetObjectState(room, obj));
				}
				dynamicNum = g_masterObjects[i].hotNum;
				break;

				case kPowerSwitch:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					ExtractFloorSuite(thisObject.data.e.where, &floor, &suite);
					room = GetRoomNumber(floor, suite);
					obj = (SInt16)thisObject.data.e.who;
					DrawPowerSwitch(&itsRect, GetObjectState(room, obj));
				}
				dynamicNum = g_masterObjects[i].hotNum;
				break;

				case kKnifeSwitch:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					ExtractFloorSuite(thisObject.data.e.where, &floor, &suite);
					room = GetRoomNumber(floor, suite);
					obj = (SInt16)thisObject.data.e.who;
					DrawKnifeSwitch(&itsRect, GetObjectState(room, obj));
				}
				dynamicNum = g_masterObjects[i].hotNum;
				break;

				case kInvisSwitch:
				dynamicNum = g_masterObjects[i].hotNum;
				break;

				case kTrigger:
				case kLgTrigger:
				case kSoundTrigger:
				break;

				case kCeilingLight:
				case kLightBulb:
				case kTableLamp:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if ((QSectRect(&itsRect, &testRect, &whoCares)) && isLit)
					DrawSimpleLight(thisObject.what, &itsRect);
				break;

				case kTrunk:
				case kBooks:
				case kHipLamp:
				case kDecoLamp:
				case kGuitar:
				case kCinderBlock:
				case kFlowerBox:
				case kFireplace:
				case kBear:
				case kVase1:
				case kVase2:
				case kRug:
				case kChimes:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if ((QSectRect(&itsRect, &testRect, &whoCares)) && isLit)
					DrawPictSansWhiteObject(thisObject.what, &itsRect);
				break;

				case kCustomPict:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if ((QSectRect(&itsRect, &testRect, &whoCares)) && isLit)
					DrawCustPictSansWhite(thisObject.data.g.height, &itsRect);
				break;

				case kFlourescent:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if ((QSectRect(&itsRect, &testRect, &whoCares)) && isLit)
					DrawFlourescent(&itsRect);
				break;

				case kTrackLight:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if ((QSectRect(&itsRect, &testRect, &whoCares)) && isLit)
					DrawTrackLight(&itsRect);
				break;

				case kInvisLight:
				break;

				case kShredder:
				case kCDs:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if ((QSectRect(&itsRect, &testRect, &whoCares)) && isLit)
					DrawSimpleAppliance(thisObject.what, &itsRect);
				break;

				case kToaster:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					DrawSimpleAppliance(thisObject.what, &itsRect);
					if ((!redraw) && (neighbor == kCentralRoom))
					{
						rectA = itsRect;
						QOffsetRect(&rectA, -g_playOriginH, -g_playOriginV);
						dynamicNum = AddDynamicObject(kToaster, &rectA, &thisObject,
								g_localNumbers[neighbor], i, thisObject.data.g.state);
					}
				}
				break;

				case kMacPlus:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					DrawMacPlus(&itsRect, thisObject.data.g.state, isLit);
					if (!redraw)
					{
						rectA = itsRect;
						QOffsetRect(&rectA, -g_playOriginH, -g_playOriginV);
						dynamicNum = AddDynamicObject(kMacPlus, &rectA, &thisObject,
								g_localNumbers[neighbor], i, thisObject.data.g.state);
					}
				}
				break;

				case kTV:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
#ifdef COMPILEQT
					if ((g_thisMac.hasQT) && (g_hasMovie) && (neighbor == kCentralRoom) &&
							(!g_tvInRoom))
					{
						whoCares = g_tvScreen1;
						ZeroRectCorner(&whoCares);
						OffsetRect(&whoCares, itsRect.left + 17, itsRect.top + 10);
						GetMovieBox(g_theMovie, &g_movieRect);
						CenterRectInRect(&g_movieRect, &whoCares);
						SetMovieBox(g_theMovie, &g_movieRect);
						theRgn = NewRgn();
						RectRgn(theRgn, &whoCares);
						SetMovieDisplayClipRgn(g_theMovie, theRgn);
						DisposeRgn(theRgn);
						g_tvOn = thisObject.data.g.state;
					}
#endif
					DrawTV(&itsRect, thisObject.data.g.state, isLit);
					if (!redraw)
					{
						rectA = itsRect;
						QOffsetRect(&rectA, -g_playOriginH, -g_playOriginV);
						dynamicNum = AddDynamicObject(kTV, &rectA, &thisObject,
								g_localNumbers[neighbor], i, thisObject.data.g.state);
#ifdef COMPILEQT
						if ((g_thisMac.hasQT) && (g_hasMovie) && (neighbor == kCentralRoom) &&
								(!g_tvInRoom))
						{
							g_tvWithMovieNumber = dynamicNum;
							g_tvInRoom = true;
						}
#endif
					}
				}
				break;

				case kCoffee:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					DrawCoffee(&itsRect, thisObject.data.g.state, isLit);
					if (!redraw)
					{
						rectA = itsRect;
						QOffsetRect(&rectA, -g_playOriginH, -g_playOriginV);
						dynamicNum = AddDynamicObject(kCoffee, &rectA, &thisObject,
								g_localNumbers[neighbor], i, thisObject.data.g.state);
					}
				}
				break;

				case kOutlet:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					if (isLit)
						DrawOutlet(&itsRect);
					if (!redraw)
					{
						rectA = itsRect;
						QOffsetRect(&rectA, -g_playOriginH, -g_playOriginV);
						dynamicNum = AddDynamicObject(kOutlet, &rectA, &thisObject,
								g_localNumbers[neighbor], i, thisObject.data.g.state);
					}
				}
				break;

				case kVCR:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					DrawVCR(&itsRect, thisObject.data.g.state, isLit);
					if (!redraw)
					{
						rectA = itsRect;
						QOffsetRect(&rectA, -g_playOriginH, -g_playOriginV);
						dynamicNum = AddDynamicObject(kVCR, &rectA, &thisObject,
								g_localNumbers[neighbor], i, thisObject.data.g.state);
					}
				}
				break;

				case kStereo:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					DrawStereo(&itsRect, g_isPlayMusicGame, isLit);
					if (!redraw)
					{
						rectA = itsRect;
						QOffsetRect(&rectA, -g_playOriginH, -g_playOriginV);
						dynamicNum = AddDynamicObject(kStereo, &rectA, &thisObject,
								g_localNumbers[neighbor], i, thisObject.data.g.state);
					}
				}
				break;

				case kMicrowave:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					DrawMicrowave(&itsRect, thisObject.data.g.state, isLit);
					if (!redraw)
					{
						rectA = itsRect;
						QOffsetRect(&rectA, -g_playOriginH, -g_playOriginV);
						dynamicNum = AddDynamicObject(kMicrowave, &rectA, &thisObject,
								g_localNumbers[neighbor], i, thisObject.data.g.state);
					}
				}
				break;

				case kBalloon:
				if ((neighbor == kCentralRoom) && (!redraw))
				{
					GetObjectRect(&thisObject, &itsRect);
					OffsetRectRoomRelative(&itsRect, neighbor);
					QOffsetRect(&itsRect, -g_playOriginH, -g_playOriginV);
					dynamicNum = AddDynamicObject(kBalloon, &itsRect, &thisObject,
							g_localNumbers[neighbor], i, thisObject.data.h.state);
				}
				break;

				case kCopterLf:
				if ((neighbor == kCentralRoom) && (!redraw))
				{
					GetObjectRect(&thisObject, &itsRect);
					OffsetRectRoomRelative(&itsRect, neighbor);
					QOffsetRect(&itsRect, -g_playOriginH, -g_playOriginV);
					dynamicNum = AddDynamicObject(kCopterLf, &itsRect, &thisObject,
							g_localNumbers[neighbor], i, thisObject.data.h.state);
				}
				break;

				case kCopterRt:
				if ((neighbor == kCentralRoom) && (!redraw))
				{
					GetObjectRect(&thisObject, &itsRect);
					OffsetRectRoomRelative(&itsRect, neighbor);
					QOffsetRect(&itsRect, -g_playOriginH, -g_playOriginV);
					dynamicNum = AddDynamicObject(kCopterRt, &itsRect, &thisObject,
							g_localNumbers[neighbor], i, thisObject.data.h.state);
				}
				break;

				case kDartLf:
				if ((neighbor == kCentralRoom) && (!redraw))
				{
					GetObjectRect(&thisObject, &itsRect);
					OffsetRectRoomRelative(&itsRect, neighbor);
					QOffsetRect(&itsRect, -g_playOriginH, -g_playOriginV);
					dynamicNum = AddDynamicObject(kDartLf, &itsRect, &thisObject,
							g_localNumbers[neighbor], i, thisObject.data.h.state);
				}
				break;

				case kDartRt:
				if ((neighbor == kCentralRoom) && (!redraw))
				{
					GetObjectRect(&thisObject, &itsRect);
					OffsetRectRoomRelative(&itsRect, neighbor);
					QOffsetRect(&itsRect, -g_playOriginH, -g_playOriginV);
					dynamicNum = AddDynamicObject(kDartRt, &itsRect, &thisObject,
							g_localNumbers[neighbor], i, thisObject.data.h.state);
				}
				break;

				case kBall:
				if ((neighbor == kCentralRoom) && (!redraw))
				{
					GetObjectRect(&thisObject, &itsRect);
					OffsetRectRoomRelative(&itsRect, neighbor);
					QOffsetRect(&itsRect, -g_playOriginH, -g_playOriginV);
					dynamicNum = AddDynamicObject(kBall, &itsRect, &thisObject,
							g_localNumbers[neighbor], i, thisObject.data.h.state);
				}
				break;

				case kDrip:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					DrawDrip(&itsRect);
					if ((!redraw) && (neighbor == kCentralRoom))
					{
						rectA = itsRect;
						QOffsetRect(&rectA, -g_playOriginH, -g_playOriginV);
						dynamicNum = AddDynamicObject(kDrip, &rectA, &thisObject,
								g_localNumbers[neighbor], i, thisObject.data.h.state);
					}
				}
				break;

				case kFish:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
				{
					DrawFish(thisObject.what, &itsRect);
					if ((!redraw) && (neighbor == kCentralRoom))
					{
						rectA = itsRect;
						QOffsetRect(&rectA, -g_playOriginH, -g_playOriginV);
						dynamicNum = AddDynamicObject(kFish, &rectA, &thisObject,
								g_localNumbers[neighbor], i, thisObject.data.h.state);
					}
				}
				break;

				case kCobweb:
				case kCloud:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if ((QSectRect(&itsRect, &testRect, &whoCares)) && isLit)
					DrawPictWithMaskObject(thisObject.what, &itsRect);
				break;

				case kMirror:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if ((QSectRect(&itsRect, &testRect, &whoCares)) && isLit)
					DrawMirror(&itsRect);
				if ((neighbor == kCentralRoom) && (!redraw))
				{
					QInsetRect(&itsRect, 4, 4);
					AddToMirrorRegion(&itsRect);
				}
				break;

				case kMousehole:
				case kFaucet:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if ((QSectRect(&itsRect, &testRect, &whoCares)) && isLit)
					DrawSimpleClutter(thisObject.what, &itsRect);
				break;

				case kFlower:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if ((QSectRect(&itsRect, &testRect, &whoCares)) && isLit)
					DrawFlower(&itsRect, thisObject.data.i.pict);
				break;

				case kWallWindow:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if (QSectRect(&itsRect, &testRect, &whoCares))
					DrawWallWindow(&itsRect);
				break;

				case kCalendar:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if ((QSectRect(&itsRect, &testRect, &whoCares)) && isLit)
					DrawCalendar(&itsRect);
				break;

				case kBulletin:
				GetObjectRect(&thisObject, &itsRect);
				OffsetRectRoomRelative(&itsRect, neighbor);
				if ((QSectRect(&itsRect, &testRect, &whoCares)) && isLit)
					DrawBulletin(&itsRect);
				break;

			}
		}

		if (!redraw)									// set up links
		{
			for (n = 0; n < g_numMasterObjects; n++)
			{
				if ((g_masterObjects[n].objectNum == i) &&
						(g_masterObjects[n].roomNum == g_localNumbers[neighbor]))
					g_masterObjects[n].dynaNum = dynamicNum;
			}
		}
	}
}
