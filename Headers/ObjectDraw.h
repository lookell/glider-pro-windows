#ifndef OBJECT_DRAW_H_
#define OBJECT_DRAW_H_

//============================================================================
//----------------------------------------------------------------------------
//                                ObjectDraw.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"

void DrawSimpleBlowers (SInt16 what, Rect *theRect);
void DrawTiki (Rect *theRect, SInt16 down);
void DrawInvisibleBlower (Rect *theRect);
void DrawLiftArea (Rect *theRect);
void DrawTable (Rect *tableTop, SInt16 down);
void DrawShelf (Rect *shelfTop);
void DrawCabinet (Rect *cabinet);
void DrawSimpleFurniture (SInt16 what, Rect *theRect);
void DrawCounter (Rect *counter);
void DrawDresser (Rect *dresser);
void DrawDeckTable (Rect *tableTop, SInt16 down);
void DrawStool (Rect *theRect, SInt16 down);
void DrawInvisObstacle (Rect *theRect);
void DrawInvisBounce (Rect *theRect);
void DrawRedClock (Rect *theRect);
void DrawBlueClock (Rect *theRect);
void DrawYellowClock (Rect *theRect);
void DrawCuckoo (Rect *theRect);
void DrawSimplePrizes (SInt16 what, Rect *theRect);
void DrawGreaseRt (Rect *theRect, SInt16 distance, Boolean state);
void DrawGreaseLf (Rect *theRect, SInt16 distance, Boolean state);
void DrawFoil (Rect *theRect);
void DrawInvisBonus (Rect *theRect);
void DrawSlider (Rect *theRect);
void DrawMailboxLeft (Rect *theRect, SInt16 down);
void DrawMailboxRight (Rect *theRect, SInt16 down);
void DrawSimpleTransport (SInt16 what, Rect *theRect);
void DrawInvisTransport (Rect *theRect);
void DrawLightSwitch (Rect *theRect, Boolean state);
void DrawMachineSwitch (Rect *theRect, Boolean state);
void DrawThermostat (Rect *theRect, Boolean state);
void DrawPowerSwitch (Rect *theRect, Boolean state);
void DrawKnifeSwitch (Rect *theRect, Boolean state);
void DrawInvisibleSwitch (Rect *theRect);
void DrawTrigger (Rect *theRect);
void DrawSoundTrigger (Rect *theRect);
void DrawSimpleLight (SInt16 what, Rect *theRect);
void DrawFlourescent (Rect *theRect);
void DrawTrackLight (Rect *theRect);
void DrawInvisLight (Rect *theRect);
void DrawSimpleAppliance (SInt16 what, Rect *theRect);
void DrawMacPlus (Rect *theRect, Boolean isOn, Boolean isLit);
void DrawTV (Rect * theRect, Boolean isOn, Boolean isLit);
void DrawCoffee (Rect *theRect, Boolean isOn, Boolean isLit);
void DrawOutlet (Rect *theRect);
void DrawVCR (Rect *theRect, Boolean isOn, Boolean isLit);
void DrawStereo (Rect *theRect, Boolean isOn, Boolean isLit);
void DrawMicrowave (Rect *theRect, Boolean isOn, Boolean isLit);
void DrawBalloon (Rect *theRect);
void DrawCopter (Rect *theRect);
void DrawDart (Rect *theRect, SInt16 which);
void DrawBall (SInt16 what, Rect *theRect);
void DrawFish (SInt16 what, Rect *theRect);
void DrawDrip (Rect *theRect);
void DrawMirror (Rect *mirror);
void DrawSimpleClutter (SInt16 what, Rect *theRect);
void DrawFlower (Rect *theRect, SInt16 which);
void DrawWallWindow (Rect *window);
void DrawCalendar (Rect *theRect);
void DrawBulletin (Rect *theRect);
void DrawPictObject (SInt16 what, Rect *theRect);
void DrawPictWithMaskObject (SInt16 what, Rect *theRect);
void DrawPictSansWhiteObject (SInt16 what, Rect *theRect);
void DrawCustPictSansWhite (SInt16 pictID, Rect *theRect);

#endif
