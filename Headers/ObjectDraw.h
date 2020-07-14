#ifndef OBJECT_DRAW_H_
#define OBJECT_DRAW_H_

//============================================================================
//----------------------------------------------------------------------------
//                                ObjectDraw.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"

void DrawSimpleBlowers (SInt16 what, const Rect *theRect);
void DrawTiki (const Rect *theRect, SInt16 down);
void DrawInvisibleBlower (const Rect *theRect);
void DrawLiftArea (const Rect *theRect);
void DrawTable (const Rect *tableTop, SInt16 down);
void DrawShelf (const Rect *shelfTop);
void DrawCabinet (const Rect *cabinet);
void DrawSimpleFurniture (SInt16 what, const Rect *theRect);
void DrawCounter (const Rect *counter);
void DrawDresser (const Rect *dresser);
void DrawDeckTable (const Rect *tableTop, SInt16 down);
void DrawStool (const Rect *theRect, SInt16 down);
void DrawInvisObstacle (const Rect *theRect);
void DrawInvisBounce (const Rect *theRect);
void DrawRedClock (const Rect *theRect);
void DrawBlueClock (const Rect *theRect);
void DrawYellowClock (const Rect *theRect);
void DrawCuckoo (const Rect *theRect);
void DrawSimplePrizes (SInt16 what, const Rect *theRect);
void DrawGreaseRt (const Rect *theRect, SInt16 distance, Boolean state);
void DrawGreaseLf (const Rect *theRect, SInt16 distance, Boolean state);
void DrawFoil (const Rect *theRect);
void DrawInvisBonus (const Rect *theRect);
void DrawSlider (const Rect *theRect);
void DrawMailboxLeft (const Rect *theRect, SInt16 down);
void DrawMailboxRight (const Rect *theRect, SInt16 down);
void DrawSimpleTransport (SInt16 what, const Rect *theRect);
void DrawInvisTransport (const Rect *theRect);
void DrawLightSwitch (const Rect *theRect, Boolean state);
void DrawMachineSwitch (const Rect *theRect, Boolean state);
void DrawThermostat (const Rect *theRect, Boolean state);
void DrawPowerSwitch (const Rect *theRect, Boolean state);
void DrawKnifeSwitch (const Rect *theRect, Boolean state);
void DrawInvisibleSwitch (const Rect *theRect);
void DrawTrigger (const Rect *theRect);
void DrawSoundTrigger (const Rect *theRect);
void DrawSimpleLight (SInt16 what, const Rect *theRect);
void DrawFlourescent (const Rect *theRect);
void DrawTrackLight (const Rect *theRect);
void DrawInvisLight (const Rect *theRect);
void DrawSimpleAppliance (SInt16 what, const Rect *theRect);
void DrawMacPlus (const Rect *theRect, Boolean isOn, Boolean isLit);
void DrawTV (const Rect *theRect, Boolean isOn, Boolean isLit);
void DrawCoffee (const Rect *theRect, Boolean isOn, Boolean isLit);
void DrawOutlet (const Rect *theRect);
void DrawVCR (const Rect *theRect, Boolean isOn, Boolean isLit);
void DrawStereo (const Rect *theRect, Boolean isOn, Boolean isLit);
void DrawMicrowave (const Rect *theRect, Boolean isOn, Boolean isLit);
void DrawBalloon (const Rect *theRect);
void DrawCopter (const Rect *theRect);
void DrawDart (const Rect *theRect, SInt16 which);
void DrawBall (SInt16 what, const Rect *theRect);
void DrawFish (SInt16 what, const Rect *theRect);
void DrawDrip (const Rect *theRect);
void DrawMirror (const Rect *mirror);
void DrawSimpleClutter (SInt16 what, const Rect *theRect);
void DrawFlower (const Rect *theRect, SInt16 which);
void DrawWallWindow (const Rect *window);
void DrawCalendar (const Rect *theRect);
void DrawBulletin (const Rect *theRect);
void DrawPictObject (SInt16 what, const Rect *theRect);
void DrawPictWithMaskObject (SInt16 what, const Rect *theRect);
void DrawPictSansWhiteObject (SInt16 what, const Rect *theRect);
void DrawCustPictSansWhite (SInt16 pictID, const Rect *theRect);

#endif
