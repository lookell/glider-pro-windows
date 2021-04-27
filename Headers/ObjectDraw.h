//============================================================================
//----------------------------------------------------------------------------
//                                ObjectDraw.h
//----------------------------------------------------------------------------
//============================================================================

#ifndef OBJECT_DRAW_H_
#define OBJECT_DRAW_H_

#include "MacTypes.h"
#include "WinAPI.h"

void DrawSimpleBlowers (HDC hdcDest, SInt16 what, const Rect *theRect);
void DrawTiki (HDC hdcDest, const Rect *theRect, SInt16 down);
void DrawInvisibleBlower (HDC hdcDest, const Rect *theRect);
void DrawLiftArea (HDC hdcDest, const Rect *theRect);
void DrawTable (HDC hdcDest, const Rect *tableTop, SInt16 down);
void DrawShelf (HDC hdcDest, const Rect *shelfTop);
void DrawCabinet (HDC hdcDest, const Rect *cabinet);
void DrawSimpleFurniture (HDC hdcDest, SInt16 what, const Rect *theRect);
void DrawCounter (HDC hdcDest, const Rect *counter);
void DrawDresser (HDC hdcDest, const Rect *dresser);
void DrawDeckTable (HDC hdcDest, const Rect *tableTop, SInt16 down);
void DrawStool (HDC hdcDest, const Rect *theRect, SInt16 down);
void DrawInvisObstacle (HDC hdcDest, const Rect *theRect);
void DrawInvisBounce (HDC hdcDest, const Rect *theRect);
void DrawRedClock (HDC hdcDest, const Rect *theRect);
void DrawBlueClock (HDC hdcDest, const Rect *theRect);
void DrawYellowClock (HDC hdcDest, const Rect *theRect);
void DrawCuckoo (HDC hdcDest, const Rect *theRect);
void DrawSimplePrizes (HDC hdcDest, SInt16 what, const Rect *theRect);
void DrawGreaseRt (HDC hdcDest, const Rect *theRect, SInt16 distance, Boolean state);
void DrawGreaseLf (HDC hdcDest, const Rect *theRect, SInt16 distance, Boolean state);
void DrawFoil (HDC hdcDest, const Rect *theRect);
void DrawInvisBonus (HDC hdcDest, const Rect *theRect);
void DrawSlider (HDC hdcDest, const Rect *theRect);
void DrawMailboxLeft (HDC hdcDest, const Rect *theRect, SInt16 down);
void DrawMailboxRight (HDC hdcDest, const Rect *theRect, SInt16 down);
void DrawSimpleTransport (HDC hdcDest, SInt16 what, const Rect *theRect);
void DrawInvisTransport (HDC hdcDest, const Rect *theRect);
void DrawLightSwitch (HDC hdcDest, const Rect *theRect, Boolean state);
void DrawMachineSwitch (HDC hdcDest, const Rect *theRect, Boolean state);
void DrawThermostat (HDC hdcDest, const Rect *theRect, Boolean state);
void DrawPowerSwitch (HDC hdcDest, const Rect *theRect, Boolean state);
void DrawKnifeSwitch (HDC hdcDest, const Rect *theRect, Boolean state);
void DrawInvisibleSwitch (HDC hdcDest, const Rect *theRect);
void DrawTrigger (HDC hdcDest, const Rect *theRect);
void DrawSoundTrigger (HDC hdcDest, const Rect *theRect);
void DrawSimpleLight (HDC hdcDest, SInt16 what, const Rect *theRect);
void DrawFlourescent (HDC hdcDest, const Rect *theRect);
void DrawTrackLight (HDC hdcDest, const Rect *theRect);
void DrawInvisLight (HDC hdcDest, const Rect *theRect);
void DrawSimpleAppliance (HDC hdcDest, SInt16 what, const Rect *theRect);
void DrawMacPlus (HDC hdcDest, const Rect *theRect, Boolean isOn, Boolean isLit);
void DrawTV (HDC hdcDest, const Rect *theRect, Boolean isOn, Boolean isLit);
void DrawCoffee (HDC hdcDest, const Rect *theRect, Boolean isOn, Boolean isLit);
void DrawOutlet (HDC hdcDest, const Rect *theRect);
void DrawVCR (HDC hdcDest, const Rect *theRect, Boolean isOn, Boolean isLit);
void DrawStereo (HDC hdcDest, const Rect *theRect, Boolean isOn, Boolean isLit);
void DrawMicrowave (HDC hdcDest, const Rect *theRect, Boolean isOn, Boolean isLit);
void DrawBalloon (HDC hdcDest, const Rect *theRect);
void DrawCopter (HDC hdcDest, const Rect *theRect);
void DrawDart (HDC hdcDest, const Rect *theRect, SInt16 which);
void DrawBall (HDC hdcDest, SInt16 what, const Rect *theRect);
void DrawFish (HDC hdcDest, SInt16 what, const Rect *theRect);
void DrawDrip (HDC hdcDest, const Rect *theRect);
void DrawMirror (HDC hdcDest, const Rect *mirror);
void DrawSimpleClutter (HDC hdcDest, SInt16 what, const Rect *theRect);
void DrawFlower (HDC hdcDest, const Rect *theRect, SInt16 which);
void DrawWallWindow (HDC hdcDest, const Rect *window);
void DrawCalendar (HDC hdcDest, const Rect *theRect);
void DrawBulletin (HDC hdcDest, const Rect *theRect);
void DrawPictObject (HDC hdcDest, SInt16 what, const Rect *theRect);
void DrawPictWithMaskObject (HDC hdcDest, SInt16 what, const Rect *theRect);
void DrawPictSansWhiteObject (HDC hdcDest, SInt16 what, const Rect *theRect);
void DrawCustPictSansWhite (HDC hdcDest, SInt16 pictID, const Rect *theRect);

#endif
