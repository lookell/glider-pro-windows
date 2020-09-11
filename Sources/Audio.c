#define GP_USE_WINAPI_H

#define CINTERFACE
#define COBJMACROS

#include "Audio.h"
#include "WinAPI.h"

// NOTE: <mmsystem.h> MUST come before <dsound.h>
#include <mmsystem.h>

#include <dsound.h>

#include <math.h>
#include <stdlib.h>

#include "ResourceIDs.h"

static FOURCC read_fourcc(const unsigned char *buf)
{
	return MAKEFOURCC(buf[0], buf[1], buf[2], buf[3]);
}

static WORD read_word(const unsigned char *buf)
{
	return (WORD)buf[0] | ((WORD)buf[1] << 8);
}

static DWORD read_dword(const unsigned char *buf)
{
	return (DWORD)buf[0] | ((DWORD)buf[1] << 8) |
			((DWORD)buf[2] << 16) | ((DWORD)buf[3] << 24);
}

typedef struct RiffChunk
{
	FOURCC ckID;
	DWORD ckSize;
	const unsigned char* ckData;
} RiffChunk;

// Read a RIFF chunk from the given buffer.
// The buffer pointer and length are updated to point just past the chunk
// that was read. If the function fails (there is not enough data in the
// buffer, or a parameter is NULL), then the return value is zero and
// the output parameter values are not modified. If the function succeeds,
// then the return value is nonzero and the output parameters are updated
// accordingly.
static int ReadRiffChunk(const void **ppBuffer, size_t *pLength, RiffChunk *pChunk)
{
	const unsigned char *dataPointer;
	size_t dataLength;
	RiffChunk riffChunk;
	DWORD skipSize;

	if (ppBuffer == NULL || pLength == NULL || pChunk == NULL)
		return 0;
	dataPointer = *ppBuffer;
	dataLength = *pLength;
	if (dataPointer == NULL || dataLength < 8)
		return 0;
	riffChunk.ckID = read_fourcc(&dataPointer[0]);
	riffChunk.ckSize = read_dword(&dataPointer[4]);
	riffChunk.ckData = &dataPointer[8];
	skipSize = 8 + riffChunk.ckSize;
	// align to 2-byte word boundary
	if ((riffChunk.ckSize % 2) != 0)
		skipSize += 1;
	if (dataLength < skipSize)
		return 0;
	dataPointer += skipSize;
	dataLength -= skipSize;
	*ppBuffer = dataPointer;
	*pLength = dataLength;
	*pChunk = riffChunk;
	return 1;
}

#define CKID_RIFF MAKEFOURCC('R', 'I', 'F', 'F')
#define CKID_WAVE MAKEFOURCC('W', 'A', 'V', 'E')
#define CKID_fmt MAKEFOURCC('f', 'm', 't', ' ')
#define CKID_data MAKEFOURCC('d', 'a', 't', 'a')

int ReadWAVFromMemory(const void *buffer, size_t length, WaveData *waveData)
{
	RiffChunk chunk;
	WAVEFORMATEX format;

	if (buffer == NULL || length == 0 || waveData == NULL)
		return 0;

	if (!ReadRiffChunk(&buffer, &length, &chunk))
		return 0;
	if (chunk.ckID != CKID_RIFF)
		return 0;
	if (chunk.ckSize < 4)
		return 0;
	if (read_fourcc(&chunk.ckData[0]) != CKID_WAVE)
		return 0;
	buffer = &chunk.ckData[4];
	length = chunk.ckSize - 4;

	// read format chunk (defined to be the first chunk in the list)
	if (!ReadRiffChunk(&buffer, &length, &chunk))
		return 0;
	if (chunk.ckID != CKID_fmt)
		return 0;
	if (chunk.ckSize < 16)
		return 0;
	format.wFormatTag = read_word(&chunk.ckData[0]);
	format.nChannels = read_word(&chunk.ckData[2]);
	format.nSamplesPerSec = read_dword(&chunk.ckData[4]);
	format.nAvgBytesPerSec = read_dword(&chunk.ckData[8]);
	format.nBlockAlign = read_word(&chunk.ckData[12]);
	format.wBitsPerSample = read_word(&chunk.ckData[14]);
	if (chunk.ckSize > 16)
		format.cbSize = read_word(&chunk.ckData[16]);
	else
		format.cbSize = 0;

	// check if format chunk is valid and supported
	if (format.wFormatTag != WAVE_FORMAT_PCM)
		return 0;
	if (format.nChannels != 1 && format.nChannels != 2)
		return 0;
	if (format.nSamplesPerSec == 0)
		return 0;
	if (format.nAvgBytesPerSec != (format.nSamplesPerSec * format.nBlockAlign))
		return 0;
	if (format.nBlockAlign != (format.nChannels * format.wBitsPerSample / 8))
		return 0;
	if (format.wBitsPerSample != 8 && format.wBitsPerSample != 16)
		return 0;
	if (format.cbSize != 0)
		return 0;

	// find and read data chunk
	do
	{
		if (!ReadRiffChunk(&buffer, &length, &chunk))
			return 0;
	} while (chunk.ckID != CKID_data);

	// give the wave data to the caller
	waveData->format.channels = format.nChannels;
	waveData->format.bitsPerSample = format.wBitsPerSample;
	waveData->format.samplesPerSec = format.nSamplesPerSec;
	waveData->dataLength = chunk.ckSize;
	waveData->dataBytes = chunk.ckData;
	return 1;
}

//==============================================================

#define WC_AUDIOOWNER L"GliderAudioOwner"
#define AUDIO_TICK_MS 50

typedef struct AudioQueueEntry
{
	unsigned char *buffer;
	DWORD length;
	AudioCallback callback;
	void *userdata;
	DWORD numDelayBytes;
	DWORD numPlayedBytes;
	DWORD numWrittenBytes;
} AudioQueueEntry;

struct AudioChannel
{
	LPDIRECTSOUNDBUFFER8 audioBuffer;
	WAVEFORMATEX format;
	DWORD totalByteSize;
	DWORD lastPlayCursor;
	DWORD outputCursor;
	BOOL isRunningTick;
	AudioQueueEntry queue[16];
	size_t queueHead;
	size_t queueSize;
};

static LPDIRECTSOUND8 g_audioDevice;
static HWND g_audioWindow;
static float g_masterVolume;
static LONG g_masterAttenuation;
static AudioChannel g_audioChannels[8]; // enough for our purposes
static HANDLE g_audioTimerHandle = NULL;
static CRITICAL_SECTION g_csAudioLock;

int Audio_InitDevice(void)
{
	LPDIRECTSOUND8 newDevice;
	WNDCLASSEX wcx;
	HWND newWindow;
	size_t idx;
	HRESULT hr;

	if (g_audioDevice != NULL)
	{
		// audio device already initialized
		return false;
	}
	wcx.cbSize = sizeof(wcx);
	if (!GetClassInfoEx(HINST_THISCOMPONENT, WC_AUDIOOWNER, &wcx))
	{
		wcx.cbSize = sizeof(wcx);
		wcx.style = 0;
		wcx.lpfnWndProc = DefWindowProc;
		wcx.cbClsExtra = 0;
		wcx.cbWndExtra = 0;
		wcx.hInstance = HINST_THISCOMPONENT;
		wcx.hIcon = NULL;
		wcx.hCursor = NULL;
		wcx.hbrBackground = NULL;
		wcx.lpszMenuName = NULL;
		wcx.lpszClassName = WC_AUDIOOWNER;
		wcx.hIconSm = NULL;
		if (!RegisterClassEx(&wcx))
		{
			// cannot register audio owner window class
			return false;
		}
	}
	newWindow = CreateWindow(WC_AUDIOOWNER, L"", 0x00000000,
		0, 0, 0, 0, HWND_MESSAGE, NULL, HINST_THISCOMPONENT, NULL);
	if (newWindow == NULL)
	{
		// cannot create audio owner window
		return false;
	}
	hr = DirectSoundCreate8(&DSDEVID_DefaultPlayback, &newDevice, NULL);
	if (FAILED(hr))
	{
		// cannot get device pointer
		DestroyWindow(newWindow);
		return false;
	}
	hr = IDirectSound8_SetCooperativeLevel(newDevice, newWindow, DSSCL_PRIORITY);
	if (FAILED(hr))
	{
		// cannot set device cooperative level
		IDirectSound8_Release(newDevice);
		DestroyWindow(newWindow);
		return false;
	}

	InitializeCriticalSection(&g_csAudioLock);
	g_audioDevice = newDevice;
	g_audioWindow = newWindow;
	g_masterVolume = 1.0f;
	g_masterAttenuation = DSBVOLUME_MAX;
	for (idx = 0; idx < ARRAYSIZE(g_audioChannels); idx++)
	{
		g_audioChannels[idx].audioBuffer = NULL;
	}
	return true;
}

void Audio_KillDevice(void)
{
	if (g_audioTimerHandle != NULL)
	{
		DeleteTimerQueueTimer(NULL, g_audioTimerHandle, INVALID_HANDLE_VALUE);
		g_audioTimerHandle = NULL;
	}
	if (g_audioDevice != NULL)
	{
		// When the sound device is released, all sound buffers created
		// by the sound device are also released. The sound buffers do not
		// need to be released separately.
		IDirectSound8_Release(g_audioDevice);
		g_audioDevice = NULL;
	}
	if (g_audioWindow != NULL)
	{
		DestroyWindow(g_audioWindow);
		g_audioWindow = NULL;
	}
	DeleteCriticalSection(&g_csAudioLock);
}

static float ClampVolume(float volume)
{
	if (volume <= FLT_MIN)
	{
		return FLT_MIN;
	}
	else if (volume >= 1.0f)
	{
		return 1.0f;
	}
	else
	{
		return volume;
	}
}

static float VolumeToAmplitudeRatio(float volume)
{
	float amplitude;

	volume = ClampVolume(volume);
	amplitude = volume;
	return amplitude;
}

static float AmplitudeRatioToDecibels(float amplitude)
{
	return 20.0f * log10f(amplitude);
}

static LONG DecibelsToAttenuation(float decibels)
{
	return (LONG)(decibels * 100.0f);
}

static LONG ClampAttenuation(LONG attenuation)
{
	if (attenuation <= DSBVOLUME_MIN)
	{
		return DSBVOLUME_MIN;
	}
	else if (attenuation >= DSBVOLUME_MAX)
	{
		return DSBVOLUME_MAX;
	}
	else
	{
		return attenuation;
	}
}

static LONG VolumeToAttenuation(float volume)
{
	float amplitude;
	float decibels;
	LONG attenuation;

	volume = ClampVolume(volume);
	amplitude = VolumeToAmplitudeRatio(volume);
	decibels = AmplitudeRatioToDecibels(amplitude);
	attenuation = DecibelsToAttenuation(decibels);
	attenuation = ClampAttenuation(attenuation);
	return attenuation;
}

float Audio_GetMasterVolume(void)
{
	float volume;

	if (g_audioDevice == NULL)
	{
		return 0.0f;
	}
	EnterCriticalSection(&g_csAudioLock);

	volume = g_masterVolume;

	LeaveCriticalSection(&g_csAudioLock);
	return volume;
}

void Audio_SetMasterVolume(float newVolume)
{
	size_t i;

	if (g_audioDevice == NULL)
	{
		return;
	}
	EnterCriticalSection(&g_csAudioLock);

	g_masterVolume = newVolume;
	g_masterAttenuation = VolumeToAttenuation(newVolume);
	for (i = 0; i < ARRAYSIZE(g_audioChannels); i++)
	{
		if (g_audioChannels[i].audioBuffer != NULL)
		{
			IDirectSoundBuffer8_SetVolume(
				g_audioChannels[i].audioBuffer,
				g_masterAttenuation
			);
		}
	}

	LeaveCriticalSection(&g_csAudioLock);
}

//-------------------------------------

static void Audio_UpdatePlayState(AudioChannel *channel, DWORD numPlayedBytes)
{
	DWORD bytesLeft;
	size_t i;
	AudioQueueEntry *entry;
	DWORD playSize;

	bytesLeft = numPlayedBytes;
	for (i = 0; i < channel->queueSize; i++)
	{
		entry = &channel->queue[(channel->queueHead + i) % ARRAYSIZE(channel->queue)];
		// Check if entry cannot have been played yet
		if (entry->numWrittenBytes == 0)
		{
			break;
		}
		// Decrement delay byte counter
		if (entry->numDelayBytes > 0)
		{
			playSize = entry->numDelayBytes;
			if (playSize > bytesLeft)
			{
				playSize = bytesLeft;
			}
			entry->numDelayBytes -= playSize;
			bytesLeft -= playSize;
		}
		// Increment played byte counter
		if (entry->numPlayedBytes < entry->length)
		{
			playSize = entry->length - entry->numPlayedBytes;
			if (playSize > bytesLeft)
			{
				playSize = bytesLeft;
			}
			entry->numPlayedBytes += playSize;
			bytesLeft -= playSize;
		}
		if (bytesLeft == 0)
		{
			break;
		}
	}
}

static BOOL CursorInRange(AudioChannel *channel, DWORD start, DWORD stop, DWORD pos)
{
	start %= channel->totalByteSize;
	stop %= channel->totalByteSize;
	pos %= channel->totalByteSize;

	if (start <= stop)
	{
		return (start <= pos) && (pos < stop);
	}
	else
	{
		return (start <= pos) || (pos < stop);
	}
}

static DWORD BytesInRange(AudioChannel *channel, DWORD start, DWORD stop)
{
	start %= channel->totalByteSize;
	stop %= channel->totalByteSize;

	if (start <= stop)
	{
		return stop - start;
	}
	else
	{
		return channel->totalByteSize + stop - start;
	}
}

static void Audio_ExecuteCallbacks(AudioChannel *channel, DWORD playCursor, DWORD writeCursor)
{
	DWORD reservedGap;
	DWORD bytesPerTick;
	DWORD callbackThreshold;
	DWORD bytesLeftToPlay;
	size_t i;
	AudioQueueEntry *entry;

	reservedGap = BytesInRange(channel, playCursor, writeCursor);
	bytesPerTick = AUDIO_TICK_MS * channel->format.nAvgBytesPerSec / 1000;
	callbackThreshold = bytesPerTick;
	if (callbackThreshold < reservedGap)
	{
		callbackThreshold = reservedGap;
	}
	callbackThreshold *= 2;

	for (i = 0; i < channel->queueSize; i++)
	{
		entry = &channel->queue[(channel->queueHead + i) % ARRAYSIZE(channel->queue)];
		bytesLeftToPlay = entry->length - entry->numPlayedBytes + entry->numDelayBytes;
		if (bytesLeftToPlay > callbackThreshold)
		{
			break;
		}
		callbackThreshold -= bytesLeftToPlay;
		if (entry->callback != NULL)
		{
			entry->callback(channel, entry->userdata);
			// make sure callback function is executed only once
			entry->callback = NULL;
		}
	}
}

static void Audio_WriteSilence(AudioChannel *channel, unsigned char *buffer, DWORD length)
{
	if (channel->format.wBitsPerSample == 8)
	{
		memset(buffer, 128, length);
	}
	else
	{
		memset(buffer, 0, length);
	}
}

static DWORD Audio_WriteData(AudioChannel *channel, unsigned char *buffer, DWORD length)
{
	DWORD bytesWritten;
	DWORD bytesLeft;
	size_t i;
	AudioQueueEntry *entry;
	DWORD writeSize;

	bytesWritten = 0;
	bytesLeft = length;
	for (i = 0; i < channel->queueSize; i++)
	{
		entry = &channel->queue[(channel->queueHead + i) % ARRAYSIZE(channel->queue)];
		writeSize = entry->length - entry->numWrittenBytes;
		if (writeSize > bytesLeft)
		{
			writeSize = bytesLeft;
		}
		memcpy(&buffer[bytesWritten], &entry->buffer[entry->numWrittenBytes], writeSize);
		entry->numWrittenBytes += writeSize;
		bytesWritten += writeSize;
		bytesLeft -= writeSize;
		if (bytesLeft == 0)
		{
			break;
		}
	}
	Audio_WriteSilence(channel, &buffer[bytesWritten], bytesLeft);
	return bytesWritten;
}

static DWORD Audio_UpdateBuffer(AudioChannel *channel, DWORD start, DWORD stop)
{
	DWORD totalSoundBytes;
	DWORD lockSize;
	HRESULT hr;
	void *ptr1;
	DWORD size1;
	void *ptr2;
	DWORD size2;

	totalSoundBytes = 0;
	lockSize = BytesInRange(channel, start, stop);
	if (lockSize == 0)
	{
		return totalSoundBytes;
	}
	hr = IDirectSoundBuffer8_Lock(channel->audioBuffer,
		start, lockSize, &ptr1, &size1, &ptr2, &size2, 0);
	if (SUCCEEDED(hr))
	{
		totalSoundBytes += Audio_WriteData(channel, (unsigned char *)ptr1, size1);
		if (ptr2 != NULL)
		{
			totalSoundBytes += Audio_WriteData(channel, (unsigned char *)ptr2, size2);
		}
		IDirectSoundBuffer8_Unlock(channel->audioBuffer, ptr1, size1, ptr2, size2);
	}
	return totalSoundBytes;
}

static void Audio_DestroyChannelHead(AudioChannel *channel);

static void Audio_TrimChannelQueue(AudioChannel *channel)
{
	AudioQueueEntry *entry;

	while (channel->queueSize != 0)
	{
		entry = &channel->queue[channel->queueHead];
		if (entry->numPlayedBytes < entry->length)
		{
			break;
		}
		Audio_DestroyChannelHead(channel);
	}
}

static void Audio_HandleDelayBytes(AudioChannel *channel, DWORD playCursor, DWORD writeCursor)
{
	size_t i;
	AudioQueueEntry *prevEntry;
	AudioQueueEntry *entry;

	if (channel->queueSize == 0)
	{
		return;
	}

	prevEntry = NULL;
	entry = NULL;
	for (i = 0; i < channel->queueSize; i++)
	{
		prevEntry = entry;
		entry = &channel->queue[(channel->queueHead + i) % ARRAYSIZE(channel->queue)];
		if (entry->numWrittenBytes != 0)
		{
			continue;
		}

		if (prevEntry == NULL)
		{
			// There are no previous entries, so the delay is that between the
			// play cursor and write cursor of the DSound buffer.
			entry->numDelayBytes = BytesInRange(channel, playCursor, writeCursor);
			channel->outputCursor = writeCursor;
		}
		else if (prevEntry->numWrittenBytes < prevEntry->length)
		{
			// The previous entry hasn't been fully written yet, so there will be
			// no play delay between it and the new entry.
			entry->numDelayBytes = 0;
		}
		else if (CursorInRange(channel, playCursor, writeCursor, channel->outputCursor))
		{
			// The last output cursor is between the DSound play and write cursors,
			// so the delay is the space between the last output cursor and write cursor.
			entry->numDelayBytes = BytesInRange(channel, channel->outputCursor, writeCursor);
			channel->outputCursor = writeCursor;
		}
		else
		{
			// The last output cursor is not between the DSound play and write cursors,
			// so there is no delay.
			entry->numDelayBytes = 0;
		}
		break; // found it
	}
}

static void AudioChannel_RunTick(AudioChannel *channel)
{
	HRESULT hr;
	DWORD playCursor;
	DWORD writeCursor;
	DWORD numPlayedBytes;

	hr = IDirectSoundBuffer8_GetCurrentPosition(
		channel->audioBuffer, &playCursor, &writeCursor);
	if (FAILED(hr))
	{
		return;
	}

	numPlayedBytes = BytesInRange(channel, channel->lastPlayCursor, playCursor);
	Audio_UpdatePlayState(channel, numPlayedBytes);
	channel->lastPlayCursor = playCursor;

	Audio_HandleDelayBytes(channel, playCursor, writeCursor);

	channel->isRunningTick = TRUE;
	Audio_ExecuteCallbacks(channel, playCursor, writeCursor);
	channel->isRunningTick = FALSE;

	Audio_HandleDelayBytes(channel, playCursor, writeCursor);

	Audio_TrimChannelQueue(channel);

	if (channel->queueSize == 0)
	{
		channel->outputCursor = writeCursor;
	}
	channel->outputCursor += Audio_UpdateBuffer(channel, channel->outputCursor, playCursor);
	channel->outputCursor %= channel->totalByteSize;
}

static VOID CALLBACK RunAudioChannelTicks(PVOID Parameter, BOOLEAN TimerOrWaitFired)
{
	size_t i;

	UNREFERENCED_PARAMETER(Parameter);
	UNREFERENCED_PARAMETER(TimerOrWaitFired);

	EnterCriticalSection(&g_csAudioLock);

	for (i = 0; i < ARRAYSIZE(g_audioChannels); i++)
	{
		if (g_audioChannels[i].audioBuffer != NULL)
		{
			AudioChannel_RunTick(&g_audioChannels[i]);
		}
	}

	LeaveCriticalSection(&g_csAudioLock);
}

static void Audio_FillBufferWithSilence(AudioChannel *channel)
{
	LPVOID outputPtr;
	DWORD outputBytes;
	HRESULT hr;

	hr = IDirectSoundBuffer8_Lock(channel->audioBuffer,
		0, 0, &outputPtr, &outputBytes, NULL, NULL, DSBLOCK_ENTIREBUFFER);
	if (SUCCEEDED(hr))
	{
		Audio_WriteSilence(channel, (unsigned char *)outputPtr, outputBytes);
		IDirectSoundBuffer8_Unlock(channel->audioBuffer, outputPtr, outputBytes, NULL, 0);
	}
}

static LPDIRECTSOUNDBUFFER8 Audio_CreateSoundBuffer(LPCDSBUFFERDESC pcDSBufferDesc)
{
	LPDIRECTSOUNDBUFFER tempBuffer;
	LPDIRECTSOUNDBUFFER8 newBuffer;
	HRESULT hr;

	hr = IDirectSound8_CreateSoundBuffer(g_audioDevice,
		pcDSBufferDesc, &tempBuffer, NULL);
	if (FAILED(hr))
	{
		return NULL;
	}
	hr = IDirectSoundBuffer8_QueryInterface(tempBuffer,
		&IID_IDirectSoundBuffer8, (void **)&newBuffer);
	IDirectSoundBuffer_Release(tempBuffer);
	if (FAILED(hr))
	{
		return NULL;
	}
	IDirectSoundBuffer8_SetVolume(newBuffer, g_masterAttenuation);
	return newBuffer;
}

AudioChannel *AudioChannel_Open(const WaveFormat *format)
{
	BOOL succeeded;
	size_t i;
	size_t channelIndex;
	WAVEFORMATEX waveFormat = { 0 };
	DSBUFFERDESC bufferDesc = { 0 };
	LPDIRECTSOUNDBUFFER8 audioBuffer;
	AudioChannel *channel;

	if (g_audioDevice == NULL)
	{
		// audio interface not initialized
		return NULL;
	}
	EnterCriticalSection(&g_csAudioLock);

	if (g_audioTimerHandle == NULL)
	{
		succeeded = CreateTimerQueueTimer(&g_audioTimerHandle, NULL, RunAudioChannelTicks,
			NULL, AUDIO_TICK_MS, AUDIO_TICK_MS, WT_EXECUTEDEFAULT);
		if (!succeeded)
		{
			// audio tick timer cannot be created
			g_audioTimerHandle = NULL;
			LeaveCriticalSection(&g_csAudioLock);
			return NULL;
		}
	}

	channelIndex = ARRAYSIZE(g_audioChannels);
	for (i = 0; i < ARRAYSIZE(g_audioChannels); i++)
	{
		if (g_audioChannels[i].audioBuffer == NULL)
		{
			channelIndex = i;
			break;
		}
	}
	if (channelIndex == ARRAYSIZE(g_audioChannels))
	{
		// ran out of space for a new channel
		LeaveCriticalSection(&g_csAudioLock);
		return NULL;
	}

	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = format->channels;
	waveFormat.nSamplesPerSec = format->samplesPerSec;
	waveFormat.wBitsPerSample = format->bitsPerSample;
	waveFormat.nBlockAlign = waveFormat.nChannels * waveFormat.wBitsPerSample / 8;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	bufferDesc.dwSize = sizeof(bufferDesc);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2;
	bufferDesc.dwBufferBytes = 2 * waveFormat.nAvgBytesPerSec;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = DS3DALG_DEFAULT;

	audioBuffer = Audio_CreateSoundBuffer(&bufferDesc);
	if (audioBuffer != NULL)
	{
		channel = &g_audioChannels[channelIndex];
		channel->audioBuffer = audioBuffer;
		channel->format = waveFormat;
		channel->totalByteSize = bufferDesc.dwBufferBytes;
		channel->lastPlayCursor = 0;
		channel->outputCursor = 0;
		channel->isRunningTick = FALSE;
		channel->queueHead = 0;
		channel->queueSize = 0;
		Audio_FillBufferWithSilence(channel);
		IDirectSoundBuffer8_Play(channel->audioBuffer, 0, 0, DSBPLAY_LOOPING);
	}
	else
	{
		// couldn't create audio buffer
		channel = NULL;
	}

	LeaveCriticalSection(&g_csAudioLock);
	return channel;
}

void AudioChannel_Close(AudioChannel *channel)
{
	if (g_audioDevice == NULL)
	{
		// audio interface not initialized
		return;
	}
	EnterCriticalSection(&g_csAudioLock);

	AudioChannel_ClearQueuedAudio(channel);
	IDirectSoundBuffer8_Release(channel->audioBuffer);
	channel->audioBuffer = NULL;

	LeaveCriticalSection(&g_csAudioLock);
}

int AudioChannel_QueueAudio(AudioChannel *channel, const AudioEntry *entry)
{
	AudioQueueEntry queueEntry;
	int hasSpace;
	size_t queueIndex;

	if (g_audioDevice == NULL)
	{
		// audio interface not initialized
		return false;
	}

	if (entry->buffer != NULL)
	{
		queueEntry.buffer = malloc(entry->length);
		queueEntry.length = entry->length;
		if (queueEntry.buffer != NULL)
		{
			memcpy(queueEntry.buffer, entry->buffer, entry->length);
		}
		else if (entry->length != 0)
		{
			// cannot allocate buffer copy
			return false;
		}
	}
	else
	{
		// callback-only entries are not allowed
		return false;
	}

	EnterCriticalSection(&g_csAudioLock);

	queueEntry.callback = entry->callback;
	queueEntry.userdata = entry->userdata;
	queueEntry.numDelayBytes = 0;
	queueEntry.numPlayedBytes = 0;
	queueEntry.numWrittenBytes = 0;

	hasSpace = (channel->queueSize < ARRAYSIZE(channel->queue));
	if (hasSpace)
	{
		queueIndex = channel->queueHead + channel->queueSize;
		queueIndex = queueIndex % ARRAYSIZE(channel->queue);
		channel->queue[queueIndex] = queueEntry;
		channel->queueSize += 1;
		if (!channel->isRunningTick)
		{
			// only run the channel's tick function if it is not already running,
			// so as to not call AudioChannel_RunTick recursively
			AudioChannel_RunTick(channel);
			IDirectSoundBuffer8_Play(channel->audioBuffer, 0, 0, DSBPLAY_LOOPING);
		}
	}
	else
	{
		free(queueEntry.buffer);
	}

	LeaveCriticalSection(&g_csAudioLock);
	return hasSpace;
}

static void Audio_DestroyChannelHead(AudioChannel *channel)
{
	AudioQueueEntry *entry;

	if (channel->queueSize != 0)
	{
		entry = &channel->queue[channel->queueHead];
		free(entry->buffer);
		entry->buffer = NULL;
		entry->length = 0;
		entry->callback = NULL;
		entry->userdata = NULL;
		entry->numDelayBytes = 0;
		entry->numPlayedBytes = 0;
		entry->numWrittenBytes = 0;
		channel->queueHead = (channel->queueHead + 1) % ARRAYSIZE(channel->queue);
		channel->queueSize -= 1;
	}
}

void AudioChannel_ClearQueuedAudio(AudioChannel *channel)
{
	if (g_audioDevice == NULL)
	{
		// audio interface not initialized
		return;
	}
	EnterCriticalSection(&g_csAudioLock);

	while (channel->queueSize != 0)
	{
		Audio_DestroyChannelHead(channel);
	}
	if (!channel->isRunningTick)
	{
		AudioChannel_RunTick(channel);
	}

	LeaveCriticalSection(&g_csAudioLock);
}

int AudioChannel_IsPlaying(AudioChannel *channel)
{
	int isPlaying;

	if (g_audioDevice == NULL)
	{
		// audio interface not initialized
		return 0;
	}
	EnterCriticalSection(&g_csAudioLock);

	AudioChannel_RunTick(channel);
	isPlaying = (channel->queueSize != 0);

	LeaveCriticalSection(&g_csAudioLock);
	return isPlaying;
}

