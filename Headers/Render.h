#ifndef RENDER_H_
#define RENDER_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Render.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderStructs.h"
#include "MacTypes.h"
#include "WinAPI.h"

extern Rect shieldRect;
extern HRGN mirrorRgn;
extern Point shieldPt;
extern SInt16 numWork2Main;
extern SInt16 numBack2Work;
extern Boolean hasMirror;

void AddRectToWorkRects (Rect *theRect);
void AddRectToBackRects (Rect *theRect);
void AddRectToWorkRectsWhole (Rect *theRect);
void RenderGlider (gliderPtr thisGlider, Boolean oneOrTwo);
void CopyRectsQD (void);
void RenderFrame (void);
void InitGarbageRects (void);
void CopyRectBackToWork (Rect *theRect);
void CopyRectWorkToBack (Rect *theRect);
void CopyRectWorkToMain (Rect *theRect);
void CopyRectMainToWork (Rect *theRect);
void CopyRectMainToBack (Rect *theRect);
void AddToMirrorRegion (Rect *theRect);
void ZeroMirrorRegion (void);

//void DirectWork2Main8 (Rect *);
//void DirectBack2Work8 (Rect *);
//void DirectGeneric2Work8 (SInt32, SInt32, Rect *, Rect *);
//void DirectWork2Main4 (Rect *);
//void DirectBack2Work4 (Rect *);
//void DirectGeneric2Work4 (SInt32, SInt32, Rect *, Rect *);
//void CopyRectsAssm (void);
//void DirectFillBack8 (Rect *, Byte);
//void DirectFillWork8 (Rect *, Byte);
//void DirectFillBack4 (Rect *, Byte);
//void DirectFillWork4 (Rect *, Byte);

#endif
