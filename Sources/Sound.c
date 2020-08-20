#include "Sound.h"

//============================================================================
//----------------------------------------------------------------------------
//                                  Sound.c
//----------------------------------------------------------------------------
//============================================================================


#include "Audio.h"
#include "DialogUtils.h"
#include "HouseIO.h"
#include "MacTypes.h"
#include "ResourceIDs.h"
#include "ResourceLoader.h"


#define kBaseBufferSoundID			1000
#define kMaxSounds					64
#define kNumSoundChannels			3
#define kNoSoundPlaying				-1

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


SoundOutput soundOutputs[kNumSoundChannels];
SoundOutput triggerSoundOutput;
WaveData theSoundData[kMaxSounds];
Boolean dontLoadSounds;
Boolean isSoundOn;
Boolean failedSound;


//==============================================================  Functions
//--------------------------------------------------------------  PlayPrioritySound

void PlayPrioritySound (SInt16 which, SInt16 priority)
{
	SInt16 whosLowest;
	SInt16 lowestPriority;
	SInt16 i;

	if (failedSound || dontLoadSounds)
	{
		return;
	}

	if (priority == kTriggerPriority)
	{
		CheckSoundOutput(&triggerSoundOutput);
		if (triggerSoundOutput.priority != kTriggerPriority)
		{
			Gp_PlaySound(&triggerSoundOutput, which, priority);
		}
		return;
	}

	CheckSoundOutput(&soundOutputs[0]);
	whosLowest = 0;
	lowestPriority = soundOutputs[0].priority;
	for (i = 1; i < kNumSoundChannels; i++)
	{
		CheckSoundOutput(&soundOutputs[i]);
		if (soundOutputs[i].priority < lowestPriority)
		{
			whosLowest = i;
			lowestPriority = soundOutputs[whosLowest].priority;
		}
	}

	if (priority >= lowestPriority)
	{
		Gp_PlaySound(&soundOutputs[whosLowest], which, priority);
	}
}

//--------------------------------------------------------------  FlushAnyTriggerPlaying

void FlushAnyTriggerPlaying (void)
{
	if (triggerSoundOutput.channel != NULL)
	{
		AudioChannel_ClearQueuedAudio(triggerSoundOutput.channel);
	}
}

//--------------------------------------------------------------  PlaySound

void Gp_PlaySound (SoundOutput *output, SInt16 soundID, SInt16 priority)
{
	AudioEntry entry;

	if (failedSound || dontLoadSounds)
	{
		return;
	}

	if (isSoundOn)
	{
		AudioChannel_ClearQueuedAudio(output->channel);

		entry.buffer = theSoundData[soundID].dataBytes;
		entry.length = theSoundData[soundID].dataLength;
		entry.callback = NULL;
		entry.userdata = NULL;
		if (AudioChannel_QueueAudio(output->channel, &entry))
		{
			output->priority = priority;
			output->soundID = soundID;
		}
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
	if ((dontLoadSounds) || (theSoundData[kTriggerSound].dataBytes != NULL))
	{
		return -1;
	}

//	FlushAnyTriggerPlaying();

	if (FAILED(Gp_LoadHouseSound(soundID, &theSoundData[kTriggerSound])))
	{
		ZeroMemory(&theSoundData[kTriggerSound], sizeof(theSoundData[kTriggerSound]));
		return -1;
	}

	triggerSoundOutput.channel = AudioChannel_Open(&theSoundData[kTriggerSound].format);
	if (triggerSoundOutput.channel == NULL)
	{
		free((void *)theSoundData[kTriggerSound].dataBytes);
		theSoundData[kTriggerSound].dataBytes = NULL;
		theSoundData[kTriggerSound].dataLength = 0;
		return -1;
	}

	return noErr;
}

//--------------------------------------------------------------  DumpTriggerSound

void DumpTriggerSound (void)
{
	if (triggerSoundOutput.channel != NULL)
	{
		AudioChannel_Close(triggerSoundOutput.channel);
		triggerSoundOutput.channel = NULL;
		triggerSoundOutput.priority = 0;
		triggerSoundOutput.soundID = kNoSoundPlaying;
	}
	free((void *)theSoundData[kTriggerSound].dataBytes);
	theSoundData[kTriggerSound].dataBytes = NULL;
	theSoundData[kTriggerSound].dataLength = 0;
}

//--------------------------------------------------------------  LoadBufferSounds

OSErr LoadBufferSounds (void)
{
	SInt16 i;
	HRESULT hr;
	WaveFormat expectedFormat;

	for (i = 0; i < kMaxSounds - 1; i++)
	{
		hr = Gp_LoadBuiltInSound(i + kBaseBufferSoundID, &theSoundData[i]);
		if (FAILED(hr))
		{
			return -1;
		}
	}

	// Make sure that all built-in sounds have the same format
	expectedFormat = theSoundData[0].format;
	for (i = 0; i < kMaxSounds - 1; i++)
	{
		if (theSoundData[i].format.channels != expectedFormat.channels)
		{
			return -2;
		}
		if (theSoundData[i].format.bitsPerSample != expectedFormat.bitsPerSample)
		{
			return -2;
		}
		if (theSoundData[i].format.samplesPerSec != expectedFormat.samplesPerSec)
		{
			return -2;
		}
	}

	theSoundData[kMaxSounds - 1].dataBytes = NULL;
	theSoundData[kMaxSounds - 1].dataLength = 0;

	return noErr;
}

//--------------------------------------------------------------  DumpBufferSounds

void DumpBufferSounds (void)
{
	SInt16 i;

	for (i = 0; i < kMaxSounds - 1; i++)
	{
		free((void *)theSoundData[i].dataBytes);
		theSoundData[i].dataBytes = NULL;
		theSoundData[i].dataLength = 0;
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
		soundOutputs[i].channel = AudioChannel_Open(&theSoundData[0].format);
		if (soundOutputs[i].channel == NULL)
		{
			theErr = -5;
			break;
		}
	}

	triggerSoundOutput.channel = NULL;
	triggerSoundOutput.priority = 0;
	triggerSoundOutput.soundID = kNoSoundPlaying;

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
		if (soundOutputs[i].channel != NULL)
		{
			AudioChannel_Close(soundOutputs[i].channel);
			soundOutputs[i].channel = NULL;
		}
	}

	return theErr;
}

//--------------------------------------------------------------  InitSound

void InitSound (HWND ownerWindow)
{
	OSErr		theErr;
	SInt16		i;

	if (dontLoadSounds)
	{
		return;
	}

	failedSound = false;

	for (i = 0; i < kNumSoundChannels; i++)
	{
		soundOutputs[i].channel = NULL;
		soundOutputs[i].priority = 0;
		soundOutputs[i].soundID = kNoSoundPlaying;
	}

	theErr = LoadBufferSounds();
	if (theErr != noErr)
	{
		YellowAlert(ownerWindow, kYellowFailedSound, theErr);
		failedSound = true;
	}

	if (!failedSound)
	{
		theErr = OpenSoundChannels();
		if (theErr != noErr)
		{
			YellowAlert(ownerWindow, kYellowFailedSound, theErr);
			failedSound = true;
		}
	}
}

//--------------------------------------------------------------  KillSound

void KillSound (void)
{
	if (dontLoadSounds)
	{
		return;
	}

	DumpBufferSounds();
	CloseSoundChannels();
}

//--------------------------------------------------------------  SoundBytesNeeded

SInt32 SoundBytesNeeded (void)
{
	SInt32 totalBytes;
	SInt16 i;

	totalBytes = 0;
	for (i = 0; i < kMaxSounds - 1; i++)
	{
		if (!Gp_BuiltInSoundExists(i + kBaseBufferSoundID))
		{
			return -1;
		}
		totalBytes += (SInt32)Gp_BuiltInSoundSize(i + kBaseBufferSoundID);
	}
	return totalBytes;
}

//--------------------------------------------------------------  TellHerNoSounds

void TellHerNoSounds (HWND ownerWindow)
{
	Alert(kNoMemForSoundsAlert, ownerWindow, NULL);
}

