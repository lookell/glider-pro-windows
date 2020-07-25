#include "Music.h"

//============================================================================
//----------------------------------------------------------------------------
//                                  Music.c
//----------------------------------------------------------------------------
//============================================================================


#include "Audio.h"
#include "DialogUtils.h"
#include "Environ.h"
#include "GliderDefines.h"
#include "HouseIO.h"
#include "MacTypes.h"
#include "ResourceIDs.h"
#include "ResourceLoader.h"
#include "Sound.h"
#include "Utilities.h"


#define kBaseBufferMusicID      2000
#define kMaxMusic               7
#define kLastMusicPiece         16
#define kLastGamePiece          6
#define kMusicTickMS            50


typedef struct MusicBuffer {
	const unsigned char *dataBytes;     // pointer to sound data
	DWORD dataLength;                   // number of bytes in sound data
	DWORD numBytesPlayed;               // number of bytes that have been played so far
	DWORD numBytesWritten;              // number of bytes that have been written so far
} MusicBuffer;


HRESULT WriteToMusicChannel (DWORD offset, const void *data, DWORD length);
VOID CALLBACK DoMusicTick (PVOID lpParameter, BOOLEAN TimerOrWaitFired);
HRESULT DoMusicTickImpl (DWORD offset, DWORD numToWrite, DWORD *numWritten);
void MusicCallBack (void);
OSErr LoadMusicSounds (void);
OSErr DumpMusicSounds (void);
OSErr OpenMusicChannel (void);
OSErr CloseMusicChannel (void);


static MusicBuffer playingBuffer, waitingBuffer;
static LPDIRECTSOUNDBUFFER8 musicChannel;
static unsigned char *musicChannelShadow;
static DWORD lastPlayedCursor, lastWrittenCursor, musicChannelByteSize;
static DWORD bytesPerMusicTick;
static HANDLE musicTimerHandle;
static CRITICAL_SECTION musicCriticalSection;

WaveData		theMusicData[kMaxMusic];
SInt16			musicSoundID, musicCursor;
SInt16			musicScore[kLastMusicPiece];
SInt16			gameScore[kLastGamePiece];
SInt16			musicMode;
Boolean			isMusicOn, isPlayMusicIdle, isPlayMusicGame;
Boolean			failedMusic, dontLoadMusic;


//==============================================================  Functions
//--------------------------------------------------------------  StartMusic

OSErr StartMusic (void)
{
	OSErr theErr;
	SInt16 soundVolume;
	BOOL succeeded;

	theErr = noErr;

	if (dontLoadMusic)
		return theErr;

	EnterCriticalSection(&musicCriticalSection);

	UnivGetSoundVolume(&soundVolume);
	if ((!isMusicOn) && (soundVolume != 0) && (!failedMusic))
	{
		IDirectSoundBuffer8_Stop(musicChannel);

		playingBuffer.dataBytes = theMusicData[musicSoundID].dataBytes;
		playingBuffer.dataLength = (DWORD)theMusicData[musicSoundID].dataLength;
		playingBuffer.numBytesPlayed = 0;
		playingBuffer.numBytesWritten = 0;

		musicCursor++;
		if (musicCursor >= kLastMusicPiece)
			musicCursor = 0;
		musicSoundID = musicScore[musicCursor];

		waitingBuffer.dataBytes = theMusicData[musicSoundID].dataBytes;
		waitingBuffer.dataLength = (DWORD)theMusicData[musicSoundID].dataLength;
		waitingBuffer.numBytesPlayed = 0;
		waitingBuffer.numBytesWritten = 0;

		DoMusicTickImpl(0, musicChannelByteSize / 2, NULL);
		lastPlayedCursor = 0;
		lastWrittenCursor = musicChannelByteSize / 2;

		IDirectSoundBuffer8_SetCurrentPosition(musicChannel, 0);
		succeeded = TRUE;
		if (musicTimerHandle == NULL)
		{
			succeeded = CreateTimerQueueTimer(&musicTimerHandle, NULL, DoMusicTick,
				NULL, 2 * kMusicTickMS, kMusicTickMS, WT_EXECUTEDEFAULT);
		}
		if (succeeded)
		{
			IDirectSoundBuffer8_Play(musicChannel, 0, 0, DSBPLAY_LOOPING);
			isMusicOn = true;
		}
		else
		{
			musicTimerHandle = NULL;
			failedMusic = true;
			theErr = -1;
		}
	}

	LeaveCriticalSection(&musicCriticalSection);

	return theErr;
}

//--------------------------------------------------------------  StopTheMusic

void StopTheMusic (void)
{
	if (dontLoadMusic)
		return;

	if (musicTimerHandle != NULL)
	{
		DeleteTimerQueueTimer(NULL, musicTimerHandle, INVALID_HANDLE_VALUE);
		musicTimerHandle = NULL;
	}

	EnterCriticalSection(&musicCriticalSection);

	if ((isMusicOn) && (!failedMusic))
	{
		IDirectSoundBuffer8_Stop(musicChannel);
		isMusicOn = false;
	}

	LeaveCriticalSection(&musicCriticalSection);
}

//--------------------------------------------------------------  ToggleMusicWhilePlaying

void ToggleMusicWhilePlaying (void)
{
	if (dontLoadMusic)
		return;

	if (isPlayMusicGame)
	{
		if (!isMusicOn)
			StartMusic();
	}
	else
	{
		if (isMusicOn)
			StopTheMusic();
	}
}

//--------------------------------------------------------------  SetMusicalPiece

void SetMusicalMode (SInt16 newMode)
{
	if (dontLoadMusic)
		return;

	EnterCriticalSection(&musicCriticalSection);

	switch (newMode)
	{
		case kKickGameScoreMode:
		musicCursor = 2;
		break;

		case kProdGameScoreMode:
		musicCursor = -1;
		break;

		default:
		musicMode = newMode;
		musicCursor = 0;
		break;
	}

	LeaveCriticalSection(&musicCriticalSection);
}

//--------------------------------------------------------------  WriteToMusicChannel

HRESULT WriteToMusicChannel (DWORD offset, const void *data, DWORD length)
{
	LPVOID firstPointer, secondPointer;
	DWORD firstLength, secondLength;
	const unsigned char *soundBytes = (const unsigned char *)data;
	HRESULT hr;

	if (offset >= musicChannelByteSize || data == NULL || length > musicChannelByteSize)
		return E_INVALIDARG;

	if (length == 0)
		return S_OK;
	
	EnterCriticalSection(&musicCriticalSection);

	hr = IDirectSoundBuffer8_Lock(musicChannel, offset, length,
			&firstPointer, &firstLength, &secondPointer, &secondLength, 0);
	if (hr == DSERR_BUFFERLOST)
	{
		hr = IDirectSoundBuffer8_Restore(musicChannel);
		if (FAILED(hr))
		{
			LeaveCriticalSection(&musicCriticalSection);
			return hr;
		}

		IDirectSoundBuffer8_Stop(musicChannel);
		hr = IDirectSoundBuffer8_Lock(musicChannel, 0, 0,
				&firstPointer, &firstLength, NULL, NULL, DSBLOCK_ENTIREBUFFER);
		if (FAILED(hr))
		{
			LeaveCriticalSection(&musicCriticalSection);
			return hr;
		}

		memcpy(firstPointer, musicChannelShadow, firstLength);
		IDirectSoundBuffer8_Unlock(musicChannel, firstPointer, firstLength, NULL, 0);
		IDirectSoundBuffer8_Play(musicChannel, 0, 0, DSBPLAY_LOOPING);

		hr = IDirectSoundBuffer8_Lock(musicChannel, offset, length,
			&firstPointer, &firstLength, &secondPointer, &secondLength, 0);
	}
	if (SUCCEEDED(hr))
	{
		if (firstLength != 0)
		{
			memcpy(&musicChannelShadow[offset], &soundBytes[0], firstLength);
			memcpy(firstPointer, &soundBytes[0], firstLength);
		}
		if (secondLength != 0)
		{
			memcpy(&musicChannelShadow[0], &soundBytes[firstLength], secondLength);
			memcpy(secondPointer, &soundBytes[firstLength], secondLength);
		}
		IDirectSoundBuffer8_Unlock(musicChannel,
			firstPointer, firstLength,
			secondPointer, secondLength);
	}

	LeaveCriticalSection(&musicCriticalSection);
	return hr;
}

//--------------------------------------------------------------  DoMusicTick

VOID CALLBACK DoMusicTick(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
	DWORD DSPlayCursor;
	DWORD invalidLength, validLength;
	DWORD numBytesPlayed;
	DWORD numBytesLeftToPlay;
	DWORD nextBufferPlayed;
	HRESULT hr;

	UNREFERENCED_PARAMETER(lpParameter);
	UNREFERENCED_PARAMETER(TimerOrWaitFired);

	EnterCriticalSection(&musicCriticalSection);

	if (dontLoadMusic || !isMusicOn)
	{
		LeaveCriticalSection(&musicCriticalSection);
		return;
	}

	hr = IDirectSoundBuffer8_GetCurrentPosition(musicChannel, &DSPlayCursor, NULL);
	if (hr == DSERR_BUFFERLOST)
	{
		IDirectSoundBuffer8_Restore(musicChannel);
		WriteToMusicChannel(0, musicChannelShadow, musicChannelByteSize);
		hr = IDirectSoundBuffer8_GetCurrentPosition(musicChannel, &DSPlayCursor, NULL);
	}
	if (FAILED(hr))
	{
		IDirectSoundBuffer8_Stop(musicChannel);
		isMusicOn = false;
		failedMusic = true;
		LeaveCriticalSection(&musicCriticalSection);
		return;
	}

	// Work out number of bytes between last played cursor and play cursor.
	// (That is, the number of bytes that have been played since last tick.)
	if (lastPlayedCursor == DSPlayCursor)
	{
		numBytesPlayed = 0;
	}
	else if (lastPlayedCursor < DSPlayCursor)
	{
		numBytesPlayed = DSPlayCursor - lastPlayedCursor;
	}
	else // (lastPlayedCursor > DSPlayCursor)
	{
		numBytesPlayed = musicChannelByteSize - lastPlayedCursor + DSPlayCursor;
	}
	playingBuffer.numBytesPlayed += numBytesPlayed;
	lastPlayedCursor = DSPlayCursor;

	// For the first couple of music ticks, there is already a waiting buffer of
	// sound data ready to go. For all subsequent ticks, though, the waiting buffer
	// is only set up if the next music tick would be playing that buffer.
	// This makes the music react properly to the musical mode changing.

	// Queue up the next buffer if we're getting close to its play time, and
	// we haven't set the buffer up yet.
	if (playingBuffer.dataLength > playingBuffer.numBytesPlayed)
	{
		numBytesLeftToPlay = playingBuffer.dataLength - playingBuffer.numBytesPlayed;
	}
	else
	{
		numBytesLeftToPlay = 0;
	}
	if ((waitingBuffer.dataBytes == NULL) && (numBytesLeftToPlay <= 2 * bytesPerMusicTick))
	{
		MusicCallBack();
	}

	// If the waiting buffer has started to play, acknowledge this by
	// making it the new playing buffer and zeroing out the waiting buffer.
	if (playingBuffer.numBytesPlayed >= playingBuffer.dataLength)
	{
		nextBufferPlayed = playingBuffer.numBytesPlayed - playingBuffer.dataLength;
		playingBuffer = waitingBuffer;
		playingBuffer.numBytesPlayed = nextBufferPlayed;
		ZeroMemory(&waitingBuffer, sizeof(waitingBuffer));
	}

	// Work out number of bytes between last written cursor and play cursor.
	// (That is, the number of bytes to be written with new data.)
	if (lastWrittenCursor == DSPlayCursor)
	{
		LeaveCriticalSection(&musicCriticalSection);
		return;
	}
	else if (lastWrittenCursor < DSPlayCursor)
	{
		invalidLength = DSPlayCursor - lastWrittenCursor;
	}
	else // (lastWrittenCursor > DSPlayCursor)
	{
		invalidLength = musicChannelByteSize - lastWrittenCursor + DSPlayCursor;
	}

	hr = DoMusicTickImpl(lastWrittenCursor, invalidLength, &validLength);
	if (SUCCEEDED(hr))
	{
		lastWrittenCursor += validLength;
		lastWrittenCursor %= musicChannelByteSize;
	}
	else
	{
		IDirectSoundBuffer8_Stop(musicChannel);
		isMusicOn = false;
		failedMusic = true;
	}

	LeaveCriticalSection(&musicCriticalSection);
}

//--------------------------------------------------------------  DoMusicTickImpl

HRESULT DoMusicTickImpl (DWORD offset, DWORD numToWrite, DWORD *pNumWritten)
{
	unsigned char *outputBytes;
	DWORD totalBytesWritten, totalBytesLeft;
	DWORD dstOffset, srcOffset, copySize;
	MusicBuffer *buffers[2];
	HRESULT hr = S_OK;
	size_t i;

	outputBytes = calloc(numToWrite, sizeof(*outputBytes));
	if (outputBytes == NULL)
		return E_OUTOFMEMORY;

	EnterCriticalSection(&musicCriticalSection);

	totalBytesWritten = 0;
	totalBytesLeft = numToWrite;
	buffers[0] = &playingBuffer;
	buffers[1] = &waitingBuffer;

	for (i = 0; i < ARRAYSIZE(buffers); i++)
	{
		// Check if this buffer has been exhausted of all of its data
		if (buffers[i]->numBytesWritten >= buffers[i]->dataLength)
			continue;

		// Calculate the number of bytes to copy from this buffer
		copySize = buffers[i]->dataLength - buffers[i]->numBytesWritten;
		if (copySize > totalBytesLeft)
			copySize = totalBytesLeft;

		// Copy the bytes to the output buffer, and update the number of bytes
		// written and number of bytes left
		dstOffset = totalBytesWritten;
		srcOffset = buffers[i]->numBytesWritten;
		memcpy(&outputBytes[dstOffset], &buffers[i]->dataBytes[srcOffset], copySize);
		buffers[i]->numBytesWritten += copySize;
		totalBytesWritten += copySize;
		totalBytesLeft -= copySize;

		// Check to see if we've got enough bytes to write to the music channel
		if (totalBytesLeft == 0)
			break;
	}

	hr = WriteToMusicChannel(offset, outputBytes, totalBytesWritten);
	free(outputBytes);
	if (pNumWritten)
		*pNumWritten = totalBytesWritten;

	LeaveCriticalSection(&musicCriticalSection);
	return hr;
}

//--------------------------------------------------------------  MusicCallBack

void MusicCallBack (void)
{
	EnterCriticalSection(&musicCriticalSection);

	switch (musicMode)
	{
		case kPlayGameScoreMode:
		musicCursor++;
		if (musicCursor >= kLastGamePiece)
			musicCursor = 1;
		musicSoundID = gameScore[musicCursor];
		if (musicSoundID < 0)
		{
			musicCursor += musicSoundID;
			musicSoundID = gameScore[musicCursor];
		}
		break;

		case kPlayWholeScoreMode:
		musicCursor++;
		if (musicCursor >= kLastMusicPiece - 1)
			musicCursor = 0;
		musicSoundID = musicScore[musicCursor];
		break;

		default:
		musicSoundID = musicMode;
		break;
	}

	waitingBuffer.dataBytes = theMusicData[musicSoundID].dataBytes;
	waitingBuffer.dataLength = (DWORD)theMusicData[musicSoundID].dataLength;
	waitingBuffer.numBytesPlayed = 0;
	waitingBuffer.numBytesWritten = 0;

	LeaveCriticalSection(&musicCriticalSection);
}

//--------------------------------------------------------------  LoadMusicSounds

OSErr LoadMusicSounds (void)
{
	SInt16 i;
	HRESULT hr;

	for (i = 0; i < kMaxMusic; i++)
	{
		theMusicData[i].dataLength = 0;
		theMusicData[i].dataBytes = NULL;
	}

	for (i = 0; i < kMaxMusic; i++)
	{
		hr = Gp_LoadBuiltInSound(i + kBaseBufferMusicID, &theMusicData[i]);
		if (FAILED(hr))
		{
			return -1;
		}
	}

	// Test to make sure that all music resources have the same format
	for (i = 1; i < kMaxMusic; i++)
	{
		if (theMusicData[i].channels != theMusicData[0].channels)
			return -2;
		if (theMusicData[i].samplesPerSec != theMusicData[0].samplesPerSec)
			return -2;
		if (theMusicData[i].bitsPerSample != theMusicData[0].bitsPerSample)
			return -2;
	}

	return noErr;
}

//--------------------------------------------------------------  DumpMusicSounds

OSErr DumpMusicSounds (void)
{
	SInt16 i;

	for (i = 0; i < kMaxMusic; i++)
	{
		free((void *)theMusicData[i].dataBytes);
		theMusicData[i].dataBytes = NULL;
		theMusicData[i].dataLength = 0;
	}
	return noErr;
}

//--------------------------------------------------------------  OpenMusicChannel

OSErr OpenMusicChannel (void)
{
	WAVEFORMATEX musicFormat;
	DSBUFFERDESC bufferDesc;
	HRESULT hr;

	musicFormat.wFormatTag = WAVE_FORMAT_PCM;
	musicFormat.nChannels = theMusicData[0].channels;
	musicFormat.nSamplesPerSec = theMusicData[0].samplesPerSec;
	musicFormat.wBitsPerSample = theMusicData[0].bitsPerSample;
	musicFormat.cbSize = 0;
	musicFormat.nBlockAlign = musicFormat.nChannels * musicFormat.wBitsPerSample / 8;
	musicFormat.nAvgBytesPerSec = musicFormat.nSamplesPerSec * musicFormat.nBlockAlign;

	bytesPerMusicTick = musicFormat.nAvgBytesPerSec * kMusicTickMS / 1000;

	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.dwSize = sizeof(bufferDesc);
	bufferDesc.dwFlags = DSBCAPS_LOCSOFTWARE
		| DSBCAPS_CTRLVOLUME
		| DSBCAPS_GLOBALFOCUS
		| DSBCAPS_GETCURRENTPOSITION2;
	bufferDesc.dwBufferBytes = 2 * musicFormat.nAvgBytesPerSec;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &musicFormat;
	bufferDesc.guid3DAlgorithm = DS3DALG_DEFAULT;

	musicTimerHandle = NULL;

	musicChannelByteSize = bufferDesc.dwBufferBytes;
	musicChannelShadow = calloc(musicChannelByteSize, sizeof(*musicChannelShadow));
	if (musicChannelShadow == NULL)
		return -2;

	hr = Audio_CreateSoundBuffer(&bufferDesc, &musicChannel, NULL);
	if (FAILED(hr))
	{
		free(musicChannelShadow);
		musicChannelShadow = NULL;
		return -1;
	}

	return noErr;
}

//--------------------------------------------------------------  CloseMusicChannel

OSErr CloseMusicChannel (void)
{
	if (musicChannel != NULL)
	{
		Audio_ReleaseSoundBuffer(musicChannel);
		musicChannel = NULL;
	}
	return noErr;
}

//--------------------------------------------------------------  InitMusic

void InitMusic (HWND ownerWindow)
{
	OSErr		theErr;

	if (dontLoadMusic)
		return;

	InitializeCriticalSection(&musicCriticalSection);

	musicChannel = NULL;

	failedMusic = false;
	isMusicOn = false;
	theErr = LoadMusicSounds();
	if (theErr != noErr)
	{
		YellowAlert(ownerWindow, kYellowNoMusic, theErr);
		failedMusic = true;
		return;
	}
	theErr = OpenMusicChannel();
	if (theErr != noErr)
	{
		YellowAlert(ownerWindow, kYellowNoMusic, theErr);
		failedMusic = true;
		return;
	}

	musicScore[0] = 0;
	musicScore[1] = 1;
	musicScore[2] = 2;
	musicScore[3] = 3;
	musicScore[4] = 4;
	musicScore[5] = 4;
	musicScore[6] = 0;
	musicScore[7] = 1;
	musicScore[8] = 2;
	musicScore[9] = 3;
	musicScore[10] = kPlayChorus;
	musicScore[11] = kPlayChorus;
	musicScore[12] = kPlayRefrainSparse1;
	musicScore[13] = kPlayRefrainSparse2;
	musicScore[14] = kPlayChorus;
	musicScore[15] = kPlayChorus;

	gameScore[0] = kPlayRefrainSparse2;
	gameScore[1] = kPlayRefrainSparse1;
	gameScore[2] = -1;
	gameScore[3] = kPlayRefrainSparse2;
	gameScore[4] = kPlayChorus;
	gameScore[5] = kPlayChorus;

	musicCursor = 0;
	musicSoundID = musicScore[musicCursor];
	musicMode = kPlayWholeScoreMode;
}

//--------------------------------------------------------------  KillMusic

void KillMusic (void)
{
	if (dontLoadMusic)
		return;

	StopTheMusic();
	DumpMusicSounds();
	CloseMusicChannel();
	DeleteCriticalSection(&musicCriticalSection);
}

//--------------------------------------------------------------  MusicBytesNeeded

SInt32 MusicBytesNeeded (void)
{
	SInt32 totalBytes;
	SInt16 i;

	totalBytes = 0;
	for (i = 0; i < kMaxMusic; i++)
	{
		if (!Gp_BuiltInSoundExists(i + kBaseBufferMusicID))
		{
			return -1;
		}
		totalBytes += (SInt32)Gp_BuiltInSoundSize(i + kBaseBufferMusicID);
	}
	return totalBytes;
}

//--------------------------------------------------------------  TellHerNoMusic

void TellHerNoMusic (HWND ownerWindow)
{
	Alert(kNoMemForMusicAlert, ownerWindow, NULL);
}

