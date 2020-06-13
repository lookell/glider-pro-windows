#ifndef SCRAP_H_
#define SCRAP_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Scrap.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"

void PutRoomScrap (void);
void PutObjectScrap (void);
void GetRoomScrap (void);
void GetObjectScrap (void);
void SeeIfValidScrapAvailable (Boolean updateMenus);
Boolean DragRoom (EventRecord *theEvent, Rect *roomSrc, SInt16 roomNumber);
//Boolean HasDragManager (void);

#endif
