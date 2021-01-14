//============================================================================
//----------------------------------------------------------------------------
//                                  Music.c
//----------------------------------------------------------------------------
//============================================================================

#include "Music.h"

#include "Audio.h"
#include "GliderDefines.h"
#include "HouseIO.h"
#include "ResourceIDs.h"
#include "ResourceLoader.h"
#include "Utilities.h"
#include "WinAPI.h"

#include <stdlib.h>

#define kBaseBufferMusicID      2000
#define kMaxMusic               7
#define kLastMusicPiece         16
#define kLastGamePiece          6

void MusicCallBack (AudioChannel *channel, void *userdata);
OSErr LoadMusicSounds (void);
OSErr DumpMusicSounds (void);
OSErr OpenMusicChannel (void);
OSErr CloseMusicChannel (void);

Boolean g_isMusicOn;
Boolean g_isPlayMusicIdle;
Boolean g_isPlayMusicGame;
Boolean g_failedMusic;
Boolean g_dontLoadMusic;

static AudioChannel *g_musicChannel;
static CRITICAL_SECTION g_musicCriticalSection;
static WaveData g_theMusicData[kMaxMusic];
static SInt16 g_musicSoundID;
static SInt16 g_musicCursor;
static SInt16 g_musicScore[kLastMusicPiece];
static SInt16 g_gameScore[kLastGamePiece];
static SInt16 g_musicMode;

//==============================================================  Functions
//--------------------------------------------------------------  StartMusic

OSErr StartMusic (void)
{
	AudioEntry entry;
	SInt16 soundVolume;

	if (g_dontLoadMusic)
	{
		return noErr;
	}

	EnterCriticalSection(&g_musicCriticalSection);

	UnivGetSoundVolume(&soundVolume);
	if ((!g_isMusicOn) && (soundVolume != 0) && (!g_failedMusic))
	{
		AudioChannel_ClearAudio(g_musicChannel);

		entry.buffer = g_theMusicData[g_musicSoundID].dataBytes;
		entry.length = g_theMusicData[g_musicSoundID].dataLength;
		entry.endingCallback = NULL;
		entry.destroyCallback = NULL;
		entry.userdata = NULL;
		AudioChannel_QueueAudio(g_musicChannel, &entry);

		g_musicCursor++;
		if (g_musicCursor >= kLastMusicPiece)
		{
			g_musicCursor = 0;
		}
		g_musicSoundID = g_musicScore[g_musicCursor];

		entry.buffer = g_theMusicData[g_musicSoundID].dataBytes;
		entry.length = g_theMusicData[g_musicSoundID].dataLength;
		entry.endingCallback = MusicCallBack;
		entry.destroyCallback = NULL;
		entry.userdata = NULL;
		AudioChannel_QueueAudio(g_musicChannel, &entry);

		g_isMusicOn = true;
	}

	LeaveCriticalSection(&g_musicCriticalSection);

	return noErr;
}

//--------------------------------------------------------------  StopTheMusic

void StopTheMusic (void)
{
	if (g_dontLoadMusic)
	{
		return;
	}

	EnterCriticalSection(&g_musicCriticalSection);

	if ((g_isMusicOn) && (!g_failedMusic))
	{
		AudioChannel_ClearAudio(g_musicChannel);
		g_isMusicOn = false;
	}

	LeaveCriticalSection(&g_musicCriticalSection);
}

//--------------------------------------------------------------  ToggleMusicWhilePlaying

void ToggleMusicWhilePlaying (void)
{
	if (g_dontLoadMusic)
	{
		return;
	}

	if (g_isPlayMusicGame)
	{
		if (!g_isMusicOn)
		{
			StartMusic();
		}
	}
	else
	{
		if (g_isMusicOn)
		{
			StopTheMusic();
		}
	}
}

//--------------------------------------------------------------  SetMusicalPiece

void SetMusicalMode (SInt16 newMode)
{
	if (g_dontLoadMusic)
	{
		return;
	}

	EnterCriticalSection(&g_musicCriticalSection);

	switch (newMode)
	{
		case kKickGameScoreMode:
		g_musicCursor = 2;
		break;

		case kProdGameScoreMode:
		g_musicCursor = -1;
		break;

		default:
		g_musicMode = newMode;
		g_musicCursor = 0;
		break;
	}

	LeaveCriticalSection(&g_musicCriticalSection);
}

//--------------------------------------------------------------  MusicCallBack

void MusicCallBack (AudioChannel *channel, void *userdata)
{
	AudioEntry entry;

	(void)userdata;

	EnterCriticalSection(&g_musicCriticalSection);

	switch (g_musicMode)
	{
		case kPlayGameScoreMode:
		g_musicCursor++;
		if (g_musicCursor >= kLastGamePiece)
		{
			g_musicCursor = 1;
		}
		g_musicSoundID = g_gameScore[g_musicCursor];
		if (g_musicSoundID < 0)
		{
			g_musicCursor += g_musicSoundID;
			g_musicSoundID = g_gameScore[g_musicCursor];
		}
		break;

		case kPlayWholeScoreMode:
		g_musicCursor++;
		if (g_musicCursor >= kLastMusicPiece - 1)
		{
			g_musicCursor = 0;
		}
		g_musicSoundID = g_musicScore[g_musicCursor];
		break;

		default:
		g_musicSoundID = g_musicMode;
		break;
	}

	entry.buffer = g_theMusicData[g_musicSoundID].dataBytes;
	entry.length = g_theMusicData[g_musicSoundID].dataLength;
	entry.endingCallback = MusicCallBack;
	entry.destroyCallback = NULL;
	entry.userdata = NULL;
	AudioChannel_QueueAudio(channel, &entry);

	LeaveCriticalSection(&g_musicCriticalSection);
}

//--------------------------------------------------------------  LoadMusicSounds

OSErr LoadMusicSounds (void)
{
	SInt16 i;
	HRESULT hr;

	for (i = 0; i < kMaxMusic; i++)
	{
		g_theMusicData[i].dataLength = 0;
		g_theMusicData[i].dataBytes = NULL;
	}

	for (i = 0; i < kMaxMusic; i++)
	{
		hr = Gp_LoadBuiltInSound(i + kBaseBufferMusicID, &g_theMusicData[i]);
		if (FAILED(hr))
		{
			return -1;
		}
	}

	// Test to make sure that all music resources have the same format
	for (i = 1; i < kMaxMusic; i++)
	{
		if (g_theMusicData[i].format.channels != g_theMusicData[0].format.channels)
		{
			return -2;
		}
		if (g_theMusicData[i].format.bitsPerSample != g_theMusicData[0].format.bitsPerSample)
		{
			return -2;
		}
		if (g_theMusicData[i].format.samplesPerSec != g_theMusicData[0].format.samplesPerSec)
		{
			return -2;
		}
	}

	return noErr;
}

//--------------------------------------------------------------  DumpMusicSounds

OSErr DumpMusicSounds (void)
{
	SInt16 i;

	for (i = 0; i < kMaxMusic; i++)
	{
		free((void *)g_theMusicData[i].dataBytes);
		g_theMusicData[i].dataBytes = NULL;
		g_theMusicData[i].dataLength = 0;
	}
	return noErr;
}

//--------------------------------------------------------------  OpenMusicChannel

OSErr OpenMusicChannel (void)
{
	g_musicChannel = AudioChannel_Open(&g_theMusicData[0].format);
	if (g_musicChannel == NULL)
	{
		return -1;
	}
	return noErr;
}

//--------------------------------------------------------------  CloseMusicChannel

OSErr CloseMusicChannel (void)
{
	if (g_musicChannel != NULL)
	{
		AudioChannel_Close(g_musicChannel);
		g_musicChannel = NULL;
	}
	return noErr;
}

//--------------------------------------------------------------  InitMusic

void InitMusic (HWND ownerWindow)
{
	OSErr theErr;

	if (g_dontLoadMusic)
	{
		return;
	}

	InitializeCriticalSection(&g_musicCriticalSection);

	g_musicChannel = NULL;

	g_failedMusic = false;
	g_isMusicOn = false;
	theErr = LoadMusicSounds();
	if (theErr != noErr)
	{
		YellowAlert(ownerWindow, kYellowNoMusic, theErr);
		g_failedMusic = true;
		return;
	}
	theErr = OpenMusicChannel();
	if (theErr != noErr)
	{
		YellowAlert(ownerWindow, kYellowNoMusic, theErr);
		g_failedMusic = true;
		return;
	}

	g_musicScore[0] = 0;
	g_musicScore[1] = 1;
	g_musicScore[2] = 2;
	g_musicScore[3] = 3;
	g_musicScore[4] = 4;
	g_musicScore[5] = 4;
	g_musicScore[6] = 0;
	g_musicScore[7] = 1;
	g_musicScore[8] = 2;
	g_musicScore[9] = 3;
	g_musicScore[10] = kPlayChorus;
	g_musicScore[11] = kPlayChorus;
	g_musicScore[12] = kPlayRefrainSparse1;
	g_musicScore[13] = kPlayRefrainSparse2;
	g_musicScore[14] = kPlayChorus;
	g_musicScore[15] = kPlayChorus;

	g_gameScore[0] = kPlayRefrainSparse2;
	g_gameScore[1] = kPlayRefrainSparse1;
	g_gameScore[2] = -1;
	g_gameScore[3] = kPlayRefrainSparse2;
	g_gameScore[4] = kPlayChorus;
	g_gameScore[5] = kPlayChorus;

	g_musicCursor = 0;
	g_musicSoundID = g_musicScore[g_musicCursor];
	g_musicMode = kPlayWholeScoreMode;
}

//--------------------------------------------------------------  KillMusic

void KillMusic (void)
{
	if (g_dontLoadMusic)
	{
		return;
	}

	StopTheMusic();
	CloseMusicChannel();
	DumpMusicSounds();
	DeleteCriticalSection(&g_musicCriticalSection);
}
