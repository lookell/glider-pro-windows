#ifndef BANNER_H_
#define BANNER_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Banner.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"

extern SInt16 numStarsRemaining;
extern Boolean bannerStarCountOn;

SInt16 CountStarsInHouse (void);
void BringUpBanner (void);
void DisplayStarsRemaining (void);

#endif
