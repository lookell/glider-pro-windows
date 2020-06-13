#ifndef LINK_H_
#define LINK_H_

//============================================================================
//----------------------------------------------------------------------------
//                                   Link.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"

SInt16 MergeFloorSuite (SInt16 floor, SInt16 suite);
void ExtractFloorSuite (SInt16 combo, SInt16 *floor, SInt16 *suite);
void UpdateLinkControl (void);
void OpenLinkWindow (void);
void CloseLinkWindow (void);

#endif
