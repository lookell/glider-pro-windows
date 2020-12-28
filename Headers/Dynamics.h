#ifndef DYNAMICS_H_
#define DYNAMICS_H_

//============================================================================
//----------------------------------------------------------------------------
//                                 Dynamics.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderDefines.h"
#include "GliderStructs.h"
#include "MacTypes.h"

typedef struct dynaType
{
	Rect dest;
	Rect whole;
	SInt16 hVel;
	SInt16 vVel;
	SInt16 type;
	SInt16 count;
	SInt16 frame;
	SInt16 timer;
	SInt16 position;
	SInt16 room;
	Byte byte0;
	Byte byte1;
	Boolean moving;
	Boolean active;
} dynaType;

extern Rect g_breadSrc[kNumBreadPicts];
extern dynaType g_dinahs[kMaxDynamicObs];
extern SInt16 g_numDynamics;

void HandleDynamics (void);
void RenderDynamics (void);
void ZeroDinahs (void);
SInt16 AddDynamicObject (SInt16 what, const Rect *where, const objectType *who,
	SInt16 room, SInt16 index, Boolean isOn);

#endif
