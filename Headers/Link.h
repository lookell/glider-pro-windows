#ifndef LINK_H_
#define LINK_H_

//============================================================================
//----------------------------------------------------------------------------
//                                   Link.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

extern HWND linkWindow;
extern SInt16 isLinkH;
extern SInt16 isLinkV;
extern SInt16 linkRoom;
extern SInt16 linkType;
extern Byte linkObject;
extern Boolean linkerIsSwitch;

SInt16 MergeFloorSuite (SInt16 floor, SInt16 suite);
void ExtractFloorSuite (SInt16 combo, SInt16 *floor, SInt16 *suite);
void UpdateLinkControl (void);
void OpenLinkWindow (void);
void CloseLinkWindow (void);

#endif
