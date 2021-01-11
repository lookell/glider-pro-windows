//============================================================================
//----------------------------------------------------------------------------
//                                  Render.h
//----------------------------------------------------------------------------
//============================================================================

#ifndef RENDER_H_
#define RENDER_H_

#include "MacTypes.h"

#define kReflectionOffsetH  (-20)
#define KReflectionOffsetV  (-16)

extern SInt16 g_numWork2Main;
extern SInt16 g_numBack2Work;
extern Boolean g_hasMirror;

void AddRectToWorkRects (const Rect *theRect);
void AddRectToBackRects (const Rect *theRect);
void AddRectToWorkRectsWhole (const Rect *theRect);
void CopyRectsQD (void);
void RenderFrame (void);
void InitGarbageRects (void);
void CopyRectBackToWork (const Rect *theRect);
void CopyRectWorkToBack (const Rect *theRect);
void CopyRectWorkToMain (const Rect *theRect);
void CopyRectMainToWork (const Rect *theRect);
void CopyRectMainToBack (const Rect *theRect);
void AddToMirrorRegion (const Rect *theRect);
void ZeroMirrorRegion (void);

#endif
