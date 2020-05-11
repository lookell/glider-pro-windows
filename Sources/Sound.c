
//============================================================================
//----------------------------------------------------------------------------
//									Sound.c
//----------------------------------------------------------------------------
//============================================================================


//#include <Resources.h>
//#include <Sound.h>
#include "Macintosh.h"
#include "Audio.h"
#include "DialogUtils.h"
#include "Externs.h"
#include "ResourceIDs.h"


#define kBaseBufferSoundID			1000
#define kMaxSounds					64
#define kNoSoundPlaying				-1


void CallBack0 (SndChannelPtr, SndCommand *);
void CallBack1 (SndChannelPtr, SndCommand *);
void CallBack2 (SndChannelPtr, SndCommand *);
OSErr LoadBufferSounds (void);
void DumpBufferSounds (void);
OSErr OpenSoundChannels (void);
OSErr CloseSoundChannels (void);


SndCallBackUPP		callBack0UPP, callBack1UPP, callBack2UPP;
SndChannelPtr		channel0, channel1, channel2;
WaveData			theSoundData[kMaxSounds];
SInt16				numSoundsLoaded, priority0, priority1, priority2;
SInt16				soundPlaying0, soundPlaying1, soundPlaying2;
Boolean				soundLoaded[kMaxSounds], dontLoadSounds;
Boolean				channelOpen, isSoundOn, failedSound;


//==============================================================  Functions
//--------------------------------------------------------------  PlayPrioritySound

void PlayPrioritySound (SInt16 which, SInt16 priority)
{
	SInt16		lowestPriority, whosLowest;

	if (failedSound || dontLoadSounds)
		return;

	if ((priority == kTriggerPriority) &&
			((priority0 == kTriggerPriority) ||
			((priority1 == kTriggerPriority)) ||
			((priority2 == kTriggerPriority))))
		return;

	whosLowest = 0;
	lowestPriority = priority0;

	if (priority1 < lowestPriority)
	{
		lowestPriority = priority1;
		whosLowest = 1;
	}

	if (priority2 < lowestPriority)
	{
		lowestPriority = priority2;
		whosLowest = 2;
	}

	if (priority >= lowestPriority)
	{
		switch (whosLowest)
		{
			case 0:
			PlaySound0(which, priority);
			break;

			case 1:
			PlaySound1(which, priority);
			break;

			case 2:
			PlaySound2(which, priority);
			break;
		}
	}
}

//--------------------------------------------------------------  FlushAnyTriggerPlaying

void FlushAnyTriggerPlaying (void)
{
	return;
#if 0
	SndCommand	theCommand;
	OSErr		theErr;

	if (priority0 == kTriggerPriority)
	{
		theCommand.cmd = quietCmd;
		theCommand.param1 = 0;
		theCommand.param2 = 0;
		theErr = SndDoImmediate(channel0, &theCommand);
		theCommand.cmd = flushCmd;
		theCommand.param1 = 0;
		theCommand.param2 = 0;
		theErr = SndDoImmediate(channel0, &theCommand);
	}

	if (priority1 == kTriggerPriority)
	{
		theCommand.cmd = quietCmd;
		theCommand.param1 = 0;
		theCommand.param2 = 0;
		theErr = SndDoImmediate(channel1, &theCommand);
		theCommand.cmd = flushCmd;
		theCommand.param1 = 0;
		theCommand.param2 = 0;
		theErr = SndDoImmediate(channel1, &theCommand);
	}

	if (priority2 == kTriggerPriority)
	{
		theCommand.cmd = quietCmd;
		theCommand.param1 = 0;
		theCommand.param2 = 0;
		theErr = SndDoImmediate(channel2, &theCommand);
		theCommand.cmd = flushCmd;
		theCommand.param1 = 0;
		theCommand.param2 = 0;
		theErr = SndDoImmediate(channel2, &theCommand);
	}
#endif
}

//--------------------------------------------------------------  PlaySound0

void PlaySound0 (SInt16 soundID, SInt16 priority)
{
	return;
#if 0
	SndCommand	theCommand;
	OSErr		theErr;

	if (failedSound || dontLoadSounds)
		return;

	theErr = noErr;
	if (isSoundOn)
	{
		priority0 = priority;
		soundPlaying0 = soundID;

		theCommand.cmd = bufferCmd;
		theCommand.param1 = 0;
		theCommand.param2 = (long)(theSoundData[soundID]);
		theErr = SndDoImmediate(channel0, &theCommand);

		theCommand.cmd = callBackCmd;
		theCommand.param1 = 0;
		theCommand.param2 = SetCurrentA5();
		theErr = SndDoCommand(channel0, &theCommand, true);
	}
#endif
}

//--------------------------------------------------------------  PlaySound1

void PlaySound1 (SInt16 soundID, SInt16 priority)
{
	return;
#if 0
	SndCommand	theCommand;
	OSErr		theErr;

	if (failedSound || dontLoadSounds)
		return;

	theErr = noErr;
	if (isSoundOn)
	{
		priority1 = priority;
		soundPlaying1 = soundID;

		theCommand.cmd = bufferCmd;
		theCommand.param1 = 0;
		theCommand.param2 = (long)(theSoundData[soundID]);
		theErr = SndDoImmediate(channel1, &theCommand);

		theCommand.cmd = callBackCmd;
		theCommand.param1 = 0;
		theCommand.param2 = SetCurrentA5();
		theErr = SndDoCommand(channel1, &theCommand, true);
	}
#endif
}

//--------------------------------------------------------------  PlaySound2

void PlaySound2 (SInt16 soundID, SInt16 priority)
{
	return;
#if 0
	SndCommand	theCommand;
	OSErr		theErr;

	if (failedSound || dontLoadSounds)
		return;

	theErr = noErr;
	if (isSoundOn)
	{
		theCommand.cmd = bufferCmd;
		theCommand.param1 = 0;
		theCommand.param2 = (long)(theSoundData[soundID]);
		theErr = SndDoImmediate(channel2, &theCommand);

		theCommand.cmd = callBackCmd;
		theCommand.param1 = 0;
		theCommand.param2 = SetCurrentA5();
		theErr = SndDoCommand(channel2, &theCommand, true);

		priority2 = priority;
		soundPlaying2 = soundID;
	}
#endif
}

//--------------------------------------------------------------  CallBack0

void CallBack0 (SndChannelPtr theChannel, SndCommand *theCommand)
{
	return;
#if 0
#pragma unused (theChannel)
	long		thisA5, gameA5;

	gameA5 = theCommand->param2;
	thisA5 = SetA5(gameA5);

	priority0 = 0;
	soundPlaying0 = kNoSoundPlaying;

	thisA5 = SetA5(thisA5);
#endif
}

//--------------------------------------------------------------  CallBack1

void CallBack1 (SndChannelPtr theChannel, SndCommand *theCommand)
{
	return;
#if 0
#pragma unused (theChannel)
	long		thisA5, gameA5;

	gameA5 = theCommand->param2;
	thisA5 = SetA5(gameA5);

	priority1 = 0;
	soundPlaying1 = kNoSoundPlaying;

	thisA5 = SetA5(thisA5);
#endif
}

//--------------------------------------------------------------  CallBack2

void CallBack2 (SndChannelPtr theChannel, SndCommand *theCommand)
{
	return;
#if 0
#pragma unused (theChannel)
	long		thisA5, gameA5;

	gameA5 = theCommand->param2;
	thisA5 = SetA5(gameA5);

	priority2 = 0;
	soundPlaying2 = kNoSoundPlaying;

	thisA5 = SetA5(thisA5);
#endif
}

//--------------------------------------------------------------  LoadTriggerSound

OSErr LoadTriggerSound (SInt16 soundID)
{
	return (-1);
#if 0
	Handle		theSound;
	long		soundDataSize;
	OSErr		theErr;

	if ((dontLoadSounds) || (theSoundData[kMaxSounds - 1] != nil))
		theErr = -1;
	else
	{
//		FlushAnyTriggerPlaying();

		theErr = noErr;

		theSound = GetResource('snd ', soundID);
		if (theSound == nil)
		{
			theErr = -1;
		}
		else
		{
			soundDataSize = GetHandleSize(theSound) - 20L;
			theSoundData[kMaxSounds - 1] = NewPtr(soundDataSize);
			HLock(theSound);
			if (theSoundData[kMaxSounds - 1] == nil)
			{
				ReleaseResource(theSound);
				theErr = MemError();
			}
			else
			{
				BlockMove((Ptr)(*theSound + 20L), theSoundData[kMaxSounds - 1], soundDataSize);
				ReleaseResource(theSound);
			}
		}
	}

	return (theErr);
#endif
}

//--------------------------------------------------------------  DumpTriggerSound

void DumpTriggerSound (void)
{
	return;
#if 0
	if (theSoundData[kMaxSounds - 1] != nil)
		DisposePtr(theSoundData[kMaxSounds - 1]);
	theSoundData[kMaxSounds - 1] = nil;
#endif
}

//--------------------------------------------------------------  LoadBufferSounds

OSErr LoadBufferSounds (void)
{
	WORD i;

	for (i = 0; i < kMaxSounds - 1; i++)
	{
		if (!ReadWAVFromResource(HINST_THISCOMPONENT,
				i + kBaseBufferSoundID, &theSoundData[i]))
		{
			return -1;
		}
	}

	theSoundData[kMaxSounds - 1].dataLength = 0;
	theSoundData[kMaxSounds - 1].dataBytes = NULL;

	return noErr;
}

//--------------------------------------------------------------  DumpBufferSounds

void DumpBufferSounds (void)
{
	// The pointers to the built-in sound data are actually pointers into
	// the resources of this program. These resources don't go away until
	// the program is unloaded, so the pointers don't need to be freed.
	return;
}

//--------------------------------------------------------------  OpenSoundChannels

OSErr OpenSoundChannels (void)
{
	return (-1);
#if 0
	OSErr		theErr;

	callBack0UPP = NewSndCallBackProc(CallBack0);
	callBack1UPP = NewSndCallBackProc(CallBack1);
	callBack2UPP = NewSndCallBackProc(CallBack2);

	theErr = noErr;

	if (channelOpen)
		return (theErr);

	theErr = SndNewChannel(&channel0,
			sampledSynth, initNoInterp + initMono,
			(SndCallBackUPP)callBack0UPP);
	if (theErr == noErr)
		channelOpen = true;
	else
		return (theErr);

	theErr = SndNewChannel(&channel1,
			sampledSynth, initNoInterp + initMono,
			(SndCallBackUPP)callBack1UPP);
	if (theErr == noErr)
		channelOpen = true;
	else
		return (theErr);

	theErr = SndNewChannel(&channel2,
			sampledSynth, initNoInterp + initMono,
			(SndCallBackUPP)callBack2UPP);
	if (theErr == noErr)
		channelOpen = true;

	return (theErr);
#endif
}

//--------------------------------------------------------------  CloseSoundChannels

OSErr CloseSoundChannels (void)
{
	return (-1);
#if 0
	OSErr		theErr;

	theErr = noErr;

	if (!channelOpen)
		return (theErr);

	if (channel0 != nil)
		theErr = SndDisposeChannel(channel0, true);
	channel0 = nil;

	if (channel1 != nil)
		theErr = SndDisposeChannel(channel1, true);
	channel1 = nil;

	if (channel2 != nil)
		theErr = SndDisposeChannel(channel2, true);
	channel2 = nil;

	if (theErr == noErr)
		channelOpen = false;

	DisposeSndCallBackUPP(callBack0UPP);
	DisposeSndCallBackUPP(callBack1UPP);
	DisposeSndCallBackUPP(callBack2UPP);

	return (theErr);
#endif
}

//--------------------------------------------------------------  InitSound

void InitSound (void)
{
	OSErr		theErr;

	if (dontLoadSounds)
		return;

	failedSound = false;

	channel0 = nil;
	channel1 = nil;
	channel2 = nil;

	priority0 = 0;
	priority1 = 0;
	priority2 = 0;
	soundPlaying0 = kNoSoundPlaying;
	soundPlaying1 = kNoSoundPlaying;
	soundPlaying2 = kNoSoundPlaying;

	theErr = LoadBufferSounds();
	if (theErr != noErr)
	{
		YellowAlert(kYellowFailedSound, theErr);
		failedSound = true;
	}

	if (!failedSound)
	{
		theErr = OpenSoundChannels();
		if (theErr != noErr)
		{
			YellowAlert(kYellowFailedSound, theErr);
			failedSound = true;
		}
	}
}

//--------------------------------------------------------------  KillSound

void KillSound (void)
{
	OSErr		theErr;

	if (dontLoadSounds)
		return;

	DumpBufferSounds();
	theErr = CloseSoundChannels();
}

//--------------------------------------------------------------  SoundBytesNeeded

SInt32 SoundBytesNeeded (void)
{
	HRSRC		hRsrc;
	DWORD		totalBytes;
	WORD		i;

	totalBytes = 0L;
	for (i = 0; i < kMaxSounds - 1; i++)
	{
		hRsrc = FindResource(HINST_THISCOMPONENT,
				MAKEINTRESOURCE(i + kBaseBufferSoundID), L"WAVE");
		if (hRsrc == NULL)
			return -1;
		totalBytes += SizeofResource(HINST_THISCOMPONENT, hRsrc);
	}
	return (SInt32)totalBytes;
}

//--------------------------------------------------------------  TellHerNoSounds

void TellHerNoSounds (void)
{
	SInt16			hitWhat;
	DialogParams	params = { 0 };

	params.hwndParent = mainWindow;
	hitWhat = Alert(kNoMemForSoundsAlert, &params);
}

//--------------------------------------------------------------  BitchAboutSM3

void BitchAboutSM3 (void)
{
	SInt16			hitWhat;
	DialogParams	params = { 0 };

	params.hwndParent = mainWindow;
	hitWhat = Alert(kNoSoundManager3Alert, &params);
}

