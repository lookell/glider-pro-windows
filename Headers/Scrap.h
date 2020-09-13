#ifndef SCRAP_H_
#define SCRAP_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Scrap.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"

extern Boolean hasScrap;
extern Boolean scrapIsARoom;

void PutRoomScrap (void);
void PutObjectScrap (void);
void GetRoomScrap (void);
void GetObjectScrap (void);
void SeeIfValidScrapAvailable (Boolean updateMenus);
void DragRoom (SInt16 clickX, SInt16 clickY, const Rect *roomSrc, SInt16 roomNumber);

#endif
