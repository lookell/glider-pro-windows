#ifndef LINK_H_
#define LINK_H_

//============================================================================
//----------------------------------------------------------------------------
//                                   Link.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

extern HWND g_linkWindow;
extern SInt16 g_isLinkH;
extern SInt16 g_isLinkV;
extern SInt16 g_linkRoom;
extern SInt16 g_linkType;
extern Byte g_linkObject;
extern Boolean g_linkerIsSwitch;

SInt16 MergeFloorSuite (SInt16 floor, SInt16 suite);
SInt16 MergeFloorSuiteVer1 (SInt16 floor, SInt16 suite);
SInt16 MergeFloorSuiteVer2 (SInt16 floor, SInt16 suite);
void ExtractFloorSuite (SInt16 combo, SInt16 *floor, SInt16 *suite);
void ExtractFloorSuiteVer1 (SInt16 combo, SInt16 *floor, SInt16 *suite);
void ExtractFloorSuiteVer2 (SInt16 combo, SInt16 *floor, SInt16 *suite);
void UpdateLinkControl (void);
void OpenLinkWindow (void);
void CloseLinkWindow (void);

#endif
