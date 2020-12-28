#ifndef TRANSIT_H_
#define TRANSIT_H_

//============================================================================
//----------------------------------------------------------------------------
//                                 Transit.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderStructs.h"
#include "MacTypes.h"

extern SInt16 g_linkedToWhat;
extern Boolean g_takingTheStairs;
extern Boolean g_firstPlayer;

SInt16 WhatAreWeLinkedTo (SInt16 where, Byte who);
void MoveRoomToRoom (gliderPtr thisGlider, SInt16 where);
void TransportRoomToRoom (gliderPtr thisGlider);
void MoveDuctToDuct (gliderPtr thisGlider);
void MoveMailToMail (gliderPtr thisGlider);
void ForceKillGlider (void);
void FollowTheLeader (void);

#endif
