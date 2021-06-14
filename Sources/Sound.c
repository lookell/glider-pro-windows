//============================================================================
//----------------------------------------------------------------------------
//                                  Sound.c
//----------------------------------------------------------------------------
//============================================================================

#include "Sound.h"

#include "Audio.h"
#include "HouseIO.h"
#include "ResourceIDs.h"
#include "ResourceLoader.h"

#include <stdlib.h>

#define kBaseBufferSoundID          1000
#define kMaxSounds                  64
#define kNumSoundChannels           3
#define kNoSoundPlaying             (-1)

C_ASSERT(kTriggerSound == kMaxSounds - 1);

typedef struct SoundOutput
{
	AudioChannel *channel;
	SInt16 priority;
	SInt16 soundID;
} SoundOutput;

void Gp_PlaySound (SoundOutput *output, SInt16 soundID, SInt16 priority);
void CheckSoundOutput (SoundOutput *output);
OSErr LoadBufferSounds (void);
void DumpBufferSounds (void);
OSErr OpenSoundChannels (void);
OSErr CloseSoundChannels (void);

Boolean g_dontLoadSounds;
Boolean g_isSoundOn;

static SoundOutput g_soundOutputs[kNumSoundChannels];
static SoundOutput g_triggerSoundOutput;
static WaveData g_theSoundData[kMaxSounds];
static Boolean g_failedSound;

//==============================================================  Functions
//--------------------------------------------------------------  PlayPrioritySound

void PlayPrioritySound (SInt16 which, SInt16 priority)
{
	SInt16 whosLowest;
	SInt16 lowestPriority;
	SInt16 i;

	if (g_failedSound || g_dontLoadSounds)
	{
		return;
	}

	if (priority == kTriggerPriority)
	{
		CheckSoundOutput(&g_triggerSoundOutput);
		if (g_triggerSoundOutput.priority != kTriggerPriority)
		{
			Gp_PlaySound(&g_triggerSoundOutput, which, priority);
		}
		return;
	}

	CheckSoundOutput(&g_soundOutputs[0]);
	whosLowest = 0;
	lowestPriority = g_soundOutputs[0].priority;
	for (i = 1; i < kNumSoundChannels; i++)
	{
		CheckSoundOutput(&g_soundOutputs[i]);
		if (g_soundOutputs[i].priority < lowestPriority)
		{
			whosLowest = i;
			lowestPriority = g_soundOutputs[whosLowest].priority;
		}
	}

	if (priority >= lowestPriority)
	{
		Gp_PlaySound(&g_soundOutputs[whosLowest], which, priority);
	}
}

//--------------------------------------------------------------  FlushAnyTriggerPlaying

void FlushAnyTriggerPlaying (void)
{
	if (g_triggerSoundOutput.channel != NULL)
	{
		AudioChannel_ClearAudio(g_triggerSoundOutput.channel);
	}
}

//--------------------------------------------------------------  PlaySound

void Gp_PlaySound (SoundOutput *output, SInt16 soundID, SInt16 priority)
{
	AudioEntry entry;

	if (g_failedSound || g_dontLoadSounds)
	{
		return;
	}
	if (soundID < 0 || soundID >= kMaxSounds)
	{
		return;
	}

	if (g_isSoundOn)
	{
		AudioChannel_ClearAudio(output->channel);

		entry.buffer = g_theSoundData[soundID].dataBytes;
		entry.length = g_theSoundData[soundID].dataLength;
		entry.endingCallback = NULL;
		entry.destroyCallback = NULL;
		entry.userdata = NULL;
		AudioChannel_QueueAudio(output->channel, &entry);

		output->priority = priority;
		output->soundID = soundID;
	}
}

//--------------------------------------------------------------  CheckSoundOutput

void CheckSoundOutput (SoundOutput *output)
{
	if (!AudioChannel_IsPlaying(output->channel))
	{
		output->priority = 0;
		output->soundID = kNoSoundPlaying;
	}
}

//--------------------------------------------------------------  LoadTriggerSound

OSErr LoadTriggerSound (SInt16 soundID)
{
	if ((g_dontLoadSounds) || (g_theSoundData[kTriggerSound].dataBytes != NULL))
	{
		return -1;
	}

	//FlushAnyTriggerPlaying();

	if (FAILED(Gp_LoadHouseSound(g_theHouseFile, soundID, &g_theSoundData[kTriggerSound])))
	{
		ZeroMemory(&g_theSoundData[kTriggerSound], sizeof(g_theSoundData[kTriggerSound]));
		return -1;
	}

	g_triggerSoundOutput.channel = AudioChannel_Open(
		g_theSoundData[kTriggerSound].channels,
		g_theSoundData[kTriggerSound].bitsPerSample,
		g_theSoundData[kTriggerSound].samplesPerSec
	);
	if (g_triggerSoundOutput.channel == NULL)
	{
		free((void *)g_theSoundData[kTriggerSound].dataBytes);
		g_theSoundData[kTriggerSound].dataBytes = NULL;
		g_theSoundData[kTriggerSound].dataLength = 0;
		return -1;
	}

	return noErr;
}

//--------------------------------------------------------------  DumpTriggerSound

void DumpTriggerSound (void)
{
	if (g_triggerSoundOutput.channel != NULL)
	{
		AudioChannel_Close(g_triggerSoundOutput.channel);
		g_triggerSoundOutput.channel = NULL;
		g_triggerSoundOutput.priority = 0;
		g_triggerSoundOutput.soundID = kNoSoundPlaying;
	}
	free((void *)g_theSoundData[kTriggerSound].dataBytes);
	g_theSoundData[kTriggerSound].dataBytes = NULL;
	g_theSoundData[kTriggerSound].dataLength = 0;
}

//--------------------------------------------------------------  LoadBufferSounds

OSErr LoadBufferSounds (void)
{
	SInt16 i;
	HRESULT hr;

	for (i = 0; i < kMaxSounds - 1; i++)
	{
		hr = Gp_LoadBuiltInSound(i + kBaseBufferSoundID, &g_theSoundData[i]);
		if (FAILED(hr))
		{
			return -1;
		}
	}

	// Make sure that all built-in sounds have the same format
	for (i = 0; i < kMaxSounds - 1; i++)
	{
		if (g_theSoundData[i].channels != g_theSoundData[0].channels)
		{
			return -2;
		}
		if (g_theSoundData[i].bitsPerSample != g_theSoundData[0].bitsPerSample)
		{
			return -2;
		}
		if (g_theSoundData[i].samplesPerSec != g_theSoundData[0].samplesPerSec)
		{
			return -2;
		}
	}

	g_theSoundData[kMaxSounds - 1].dataBytes = NULL;
	g_theSoundData[kMaxSounds - 1].dataLength = 0;

	return noErr;
}

//--------------------------------------------------------------  DumpBufferSounds

void DumpBufferSounds (void)
{
	SInt16 i;

	for (i = 0; i < kMaxSounds - 1; i++)
	{
		free((void *)g_theSoundData[i].dataBytes);
		g_theSoundData[i].dataBytes = NULL;
		g_theSoundData[i].dataLength = 0;
	}
}

//--------------------------------------------------------------  OpenSoundChannels

OSErr OpenSoundChannels (void)
{
	OSErr theErr;
	SInt16 i;

	theErr = noErr;

	for (i = 0; i < kNumSoundChannels; i++)
	{
		g_soundOutputs[i].channel = AudioChannel_Open(
			g_theSoundData[0].channels,
			g_theSoundData[0].bitsPerSample,
			g_theSoundData[0].samplesPerSec
		);
		if (g_soundOutputs[i].channel == NULL)
		{
			theErr = -5;
			break;
		}
	}

	g_triggerSoundOutput.channel = NULL;
	g_triggerSoundOutput.priority = 0;
	g_triggerSoundOutput.soundID = kNoSoundPlaying;

	if (theErr != noErr)
	{
		CloseSoundChannels();
	}

	return theErr;
}

//--------------------------------------------------------------  CloseSoundChannels

OSErr CloseSoundChannels (void)
{
	OSErr theErr;
	SInt16 i;

	theErr = noErr;

	for (i = 0; i < kNumSoundChannels; i++)
	{
		if (g_soundOutputs[i].channel != NULL)
		{
			AudioChannel_Close(g_soundOutputs[i].channel);
			g_soundOutputs[i].channel = NULL;
		}
	}

	return theErr;
}

//--------------------------------------------------------------  InitSound

void InitSound (HWND ownerWindow)
{
	OSErr theErr;
	SInt16 i;

	if (g_dontLoadSounds)
	{
		return;
	}

	g_failedSound = false;

	for (i = 0; i < kNumSoundChannels; i++)
	{
		g_soundOutputs[i].channel = NULL;
		g_soundOutputs[i].priority = 0;
		g_soundOutputs[i].soundID = kNoSoundPlaying;
	}

	theErr = LoadBufferSounds();
	if (theErr != noErr)
	{
		YellowAlert(ownerWindow, kYellowFailedSound, theErr);
		g_failedSound = true;
	}

	if (!g_failedSound)
	{
		theErr = OpenSoundChannels();
		if (theErr != noErr)
		{
			YellowAlert(ownerWindow, kYellowFailedSound, theErr);
			g_failedSound = true;
		}
	}
}

//--------------------------------------------------------------  KillSound

void KillSound (void)
{
	if (g_dontLoadSounds)
	{
		return;
	}
	CloseSoundChannels();
	DumpBufferSounds();
}
