//============================================================================
//----------------------------------------------------------------------------
//                                  Banner.h
//----------------------------------------------------------------------------
//============================================================================

#ifndef BANNER_H_
#define BANNER_H_

#include "GliderStructs.h"
#include "MacTypes.h"

extern SInt16 g_numStarsRemaining;
extern Boolean g_bannerStarCountOn;

SInt16 CountStarsInHouse (const houseType *house);
void BringUpBanner (void);
void DisplayStarsRemaining (void);

#endif
