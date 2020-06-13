#ifndef TRANSIT_H_
#define TRANSIT_H_

//============================================================================
//----------------------------------------------------------------------------
//                                 Transit.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderStructs.h"
#include "MacTypes.h"

SInt16 WhatAreWeLinkedTo (SInt16 where, Byte who);
void ReadyGliderFromTransit (gliderPtr thisGlider, SInt16 toWhat);
void MoveRoomToRoom (gliderPtr thisGlider, SInt16 where);
void TransportRoomToRoom (gliderPtr thisGlider);
void MoveDuctToDuct (gliderPtr thisGlider);
void MoveMailToMail (gliderPtr thisGlider);
void ForceKillGlider (void);
void FollowTheLeader (void);

#endif
