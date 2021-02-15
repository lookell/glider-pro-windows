#include "Audio.h"

#include "WinAPI.h"
#include <mmsystem.h>
#include <dsound.h>

#include <math.h>
#include <process.h>
#include <stdlib.h>

#include "ResourceIDs.h"

//===========================================================================//

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
	dataPointer = (const unsigned char *)*ppBuffer;
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

//===========================================================================//

#define AUDIO_TICK_MS  50

typedef struct AudioQueueEntry
{
	const unsigned char *buffer;
	DWORD length;
	AudioCallback endingCallback;
	AudioCallback destroyCallback;
	void *userdata;
	DWORD numDelayBytes;
	DWORD numPlayedBytes;
	DWORD numWrittenBytes;
} AudioQueueEntry;

struct AudioChannel
{
	LPDIRECTSOUNDBUFFER audioBuffer;
	WAVEFORMATEX format;
	DWORD totalByteSize;
	DWORD lastPlayCursor;
	DWORD outputCursor;
	BOOL isRunningTick;
	AudioQueueEntry queue[16];
	size_t queueHead;
	size_t queueSize;
};

typedef struct AudioDeviceState
{
	LPDIRECTSOUND8 audioDevice;
	float masterVolume;
	LONG masterAttenuation;
	AudioChannel audioChannels[8];  // enough for our purposes
} AudioDeviceState;

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
	DWORD callbackThreshold;
	DWORD bytesLeftToPlay;
	size_t i;
	AudioQueueEntry *entry;

	reservedGap = BytesInRange(channel, playCursor, writeCursor);
	callbackThreshold = AUDIO_TICK_MS * channel->format.nAvgBytesPerSec / 1000;
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
		if (entry->endingCallback != NULL)
		{
			entry->endingCallback(channel, entry->userdata);
			// make sure callback function is executed only once
			entry->endingCallback = NULL;
		}
	}
}

static void Audio_WriteSilence(AudioChannel *channel, void *outputPtr, DWORD length)
{
	if (channel->format.wBitsPerSample == 8)
	{
		memset(outputPtr, 128, length);
	}
	else
	{
		memset(outputPtr, 0, length);
	}
}

static DWORD Audio_WriteData(AudioChannel *channel, void *outputPtr, DWORD length)
{
	unsigned char *bytePtr;
	DWORD bytesWritten;
	DWORD bytesLeft;
	size_t i;
	AudioQueueEntry *entry;
	DWORD writeSize;

	bytePtr = (unsigned char *)outputPtr;
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
		memcpy(&bytePtr[bytesWritten], &entry->buffer[entry->numWrittenBytes], writeSize);
		entry->numWrittenBytes += writeSize;
		bytesWritten += writeSize;
		bytesLeft -= writeSize;
		if (bytesLeft == 0)
		{
			break;
		}
	}
	Audio_WriteSilence(channel, &bytePtr[bytesWritten], bytesLeft);
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
	hr = IDirectSoundBuffer_Lock(channel->audioBuffer,
		start, lockSize, &ptr1, &size1, &ptr2, &size2, 0);
	if (SUCCEEDED(hr))
	{
		totalSoundBytes += Audio_WriteData(channel, ptr1, size1);
		if (ptr2 != NULL)
		{
			totalSoundBytes += Audio_WriteData(channel, ptr2, size2);
		}
		IDirectSoundBuffer_Unlock(channel->audioBuffer, ptr1, size1, ptr2, size2);
	}
	return totalSoundBytes;
}

static void Audio_DestroyChannelHead(AudioChannel *channel)
{
	AudioQueueEntry *entry;

	if (channel->queueSize != 0)
	{
		entry = &channel->queue[channel->queueHead];
		if (entry->destroyCallback != NULL)
		{
			entry->destroyCallback(channel, entry->userdata);
		}
		entry->buffer = NULL;
		entry->length = 0;
		entry->endingCallback = NULL;
		entry->destroyCallback = NULL;
		entry->userdata = NULL;
		entry->numDelayBytes = 0;
		entry->numPlayedBytes = 0;
		entry->numWrittenBytes = 0;
		channel->queueHead = (channel->queueHead + 1) % ARRAYSIZE(channel->queue);
		channel->queueSize -= 1;
	}
}

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

	hr = IDirectSoundBuffer_GetCurrentPosition(
		channel->audioBuffer, &playCursor, &writeCursor);
	if (FAILED(hr))
	{
		return;
	}

	if (channel->isRunningTick)
	{
		// prevent recursive calls
		return;
	}
	channel->isRunningTick = TRUE;

	numPlayedBytes = BytesInRange(channel, channel->lastPlayCursor, playCursor);
	Audio_UpdatePlayState(channel, numPlayedBytes);
	channel->lastPlayCursor = playCursor;

	Audio_HandleDelayBytes(channel, playCursor, writeCursor);
	Audio_ExecuteCallbacks(channel, playCursor, writeCursor);
	// If callbacks added new entries, their delays must be calculated too
	Audio_HandleDelayBytes(channel, playCursor, writeCursor);
	Audio_TrimChannelQueue(channel);

	if (channel->queueSize == 0)
	{
		channel->outputCursor = writeCursor;
	}
	channel->outputCursor += Audio_UpdateBuffer(channel, channel->outputCursor, playCursor);
	channel->outputCursor %= channel->totalByteSize;

	channel->isRunningTick = FALSE;
}

static void Audio_FillBufferWithSilence(AudioChannel *channel)
{
	LPVOID outputPtr;
	DWORD outputBytes;
	HRESULT hr;

	hr = IDirectSoundBuffer_Lock(channel->audioBuffer,
		0, 0, &outputPtr, &outputBytes, NULL, NULL, DSBLOCK_ENTIREBUFFER);
	if (SUCCEEDED(hr))
	{
		Audio_WriteSilence(channel, outputPtr, outputBytes);
		IDirectSoundBuffer_Unlock(channel->audioBuffer, outputPtr, outputBytes, NULL, 0);
	}
}

static float ClampVolume(float volume)
{
	if (volume < FLT_MIN)
	{
		volume = FLT_MIN;
	}
	if (volume > 1.0f)
	{
		volume = 1.0f;
	}
	return volume;
}

static LONG ClampAttenuation(LONG attenuation)
{
	if (attenuation < DSBVOLUME_MIN)
	{
		attenuation = DSBVOLUME_MIN;
	}
	if (attenuation > DSBVOLUME_MAX)
	{
		attenuation = DSBVOLUME_MAX;
	}
	return attenuation;
}

static float VolumeToAmplitudeRatio(float volume)
{
	return powf(volume, 1.75f);
}

static float AmplitudeRatioToDecibels(float amplitude)
{
	return 20.0f * log10f(amplitude);
}

static LONG DecibelsToAttenuation(float decibels)
{
	return (LONG)(decibels * 100.0f);
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

//===========================================================================//

static BOOL AudioDevice_Init(AudioDeviceState *self, HWND hwnd)
{
	HRESULT hr;

	ZeroMemory(self, sizeof(*self));
	hr = DirectSoundCreate8(&DSDEVID_DefaultPlayback, &self->audioDevice, NULL);
	if (FAILED(hr))
	{
		return FALSE;
	}
	hr = IDirectSound8_SetCooperativeLevel(self->audioDevice, hwnd, DSSCL_PRIORITY);
	if (FAILED(hr))
	{
		IDirectSound8_Release(self->audioDevice);
		return FALSE;
	}
	self->masterVolume = 1.0f;
	self->masterAttenuation = DSBVOLUME_MAX;
	return TRUE;
}

static void AudioDevice_Kill(AudioDeviceState *self)
{
	size_t i;

	for (i = 0; i < ARRAYSIZE(self->audioChannels); ++i)
	{
		if (self->audioChannels[i].audioBuffer != NULL)
		{
			AudioChannel_Close(&self->audioChannels[i]);
		}
	}
	if (self->audioDevice != NULL)
	{
		IDirectSound8_Release(self->audioDevice);
		self->audioDevice = NULL;
	}
}

static float AudioDevice_GetMasterVolume(AudioDeviceState *self)
{
	return self->masterVolume;
}

static void AudioDevice_SetMasterVolume(AudioDeviceState *self, float volume)
{
	AudioChannel *channel;
	size_t i;

	self->masterVolume = volume;
	self->masterAttenuation = VolumeToAttenuation(volume);
	for (i = 0; i < ARRAYSIZE(self->audioChannels); ++i)
	{
		channel = &self->audioChannels[i];
		if (channel->audioBuffer != NULL)
		{
			IDirectSoundBuffer_SetVolume(channel->audioBuffer, self->masterAttenuation);
		}
	}
}

static AudioChannel *AudioDevice_FindUnusedChannel(AudioDeviceState *self)
{
	size_t i;

	for (i = 0; i < ARRAYSIZE(self->audioChannels); ++i)
	{
		if (self->audioChannels[i].audioBuffer == NULL)
		{
			return &self->audioChannels[i];
		}
	}
	return NULL;
}

static AudioChannel *AudioDevice_OpenChannel(AudioDeviceState *self, const WaveFormat *format)
{
	AudioChannel *channel;
	WAVEFORMATEX waveFormat = { 0 };
	DSBUFFERDESC bufferDesc = { 0 };
	LPDIRECTSOUNDBUFFER pBuffer;
	HRESULT hr;

	channel = AudioDevice_FindUnusedChannel(self);
	if (channel == NULL)
	{
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
	hr = IDirectSound8_CreateSoundBuffer(self->audioDevice, &bufferDesc, &pBuffer, NULL);
	if (FAILED(hr))
	{
		return NULL;
	}

	ZeroMemory(channel, sizeof(*channel));
	channel->audioBuffer = pBuffer;
	channel->format = waveFormat;
	channel->totalByteSize = bufferDesc.dwBufferBytes;
	channel->lastPlayCursor = 0;
	channel->outputCursor = 0;
	channel->isRunningTick = FALSE;
	channel->queueHead = 0;
	channel->queueSize = 0;
	Audio_FillBufferWithSilence(channel);
	IDirectSoundBuffer_SetVolume(channel->audioBuffer, self->masterAttenuation);
	IDirectSoundBuffer_Play(channel->audioBuffer, 0, 0, DSBPLAY_LOOPING);
	return channel;
}

static void AudioDevice_QueueChannelAudio(AudioChannel *channel, const AudioEntry *entry)
{
	size_t queueIndex;
	AudioQueueEntry *queueEntry;

	if (channel->queueSize >= ARRAYSIZE(channel->queue))
	{
		return;
	}
	queueIndex = channel->queueHead + channel->queueSize;
	queueEntry = &channel->queue[queueIndex % ARRAYSIZE(channel->queue)];
	ZeroMemory(queueEntry, sizeof(*queueEntry));
	queueEntry->buffer = entry->buffer;
	queueEntry->length = entry->length;
	queueEntry->endingCallback = entry->endingCallback;
	queueEntry->destroyCallback = entry->destroyCallback;
	queueEntry->userdata = entry->userdata;
	queueEntry->numDelayBytes = 0;
	queueEntry->numPlayedBytes = 0;
	queueEntry->numWrittenBytes = 0;
	channel->queueSize += 1;
	AudioChannel_RunTick(channel);
	IDirectSoundBuffer_Play(channel->audioBuffer, 0, 0, DSBPLAY_LOOPING);
}

static void AudioDevice_ClearChannelAudio(AudioChannel *channel)
{
	while (channel->queueSize != 0)
	{
		Audio_DestroyChannelHead(channel);
	}
	AudioChannel_RunTick(channel);
}

static int AudioDevice_IsChannelPlaying(AudioChannel *channel)
{
	AudioChannel_RunTick(channel);
	return (channel->queueSize != 0);
}

static void AudioDevice_CloseChannel(AudioChannel *channel)
{
	AudioDevice_ClearChannelAudio(channel);
	IDirectSoundBuffer_Release(channel->audioBuffer);
	channel->audioBuffer = NULL;
}

//==========================================================================//

#define IDT_AUDIOTICK  1000

typedef struct AudioWnd
{
	HWND hwndSelf;
	AudioDeviceState deviceState;
} AudioWnd;

static LRESULT AudioWnd_OnCreate(AudioWnd *self, LPCREATESTRUCT lpCreateStruct)
{
	(void)lpCreateStruct;
	if (!AudioDevice_Init(&self->deviceState, self->hwndSelf))
	{
		ZeroMemory(&self->deviceState, sizeof(self->deviceState));
		return -1;
	}
	if (!SetTimer(self->hwndSelf, IDT_AUDIOTICK, AUDIO_TICK_MS, NULL))
	{
		AudioDevice_Kill(&self->deviceState);
		return -1;
	}
	return 0;
}

static LRESULT AudioWnd_OnDestroy(AudioWnd *self)
{
	KillTimer(self->hwndSelf, IDT_AUDIOTICK);
	AudioDevice_Kill(&self->deviceState);
	PostQuitMessage(0);
	return 0;
}

static void AudioWnd_RunAudioTicks(AudioWnd *self)
{
	AudioChannel *channel;
	size_t i;

	for (i = 0; i < ARRAYSIZE(self->deviceState.audioChannels); ++i)
	{
		channel = &self->deviceState.audioChannels[i];
		if (channel->audioBuffer != NULL)
		{
			AudioChannel_RunTick(channel);
		}
	}
}

static LRESULT AudioWnd_OnShutdown(AudioWnd *self)
{
	DestroyWindow(self->hwndSelf);
	return 0;
}

#define AUDIOCMD_SHUTDOWN                (WM_USER + 1)
#define AUDIOCMD_GETMASTERVOLUME         (WM_USER + 2)
#define AUDIOCMD_SETMASTERVOLUME         (WM_USER + 3)
#define AUDIOCMD_OPENCHANNEL             (WM_USER + 4)
#define AUDIOCMD_CLOSECHANNEL            (WM_USER + 5)
#define AUDIOCMD_QUEUECHANNELAUDIO       (WM_USER + 6)
#define AUDIOCMD_CLEARCHANNELAUDIO       (WM_USER + 7)
#define AUDIOCMD_ISCHANNELPLAYING        (WM_USER + 8)

typedef struct AudioCmdData_GetMasterVolume
{
	float masterVolume;
} AudioCmdData_GetMasterVolume;

typedef struct AudioCmdData_SetMasterVolume
{
	float masterVolume;
} AudioCmdData_SetMasterVolume;

typedef struct AudioCmdData_OpenChannel
{
	WaveFormat format;
	AudioChannel *channel;
} AudioCmdData_OpenChannel;

typedef struct AudioCmdData_CloseChannel
{
	AudioChannel *channel;
} AudioCmdData_CloseChannel;

typedef struct AudioCmdData_QueueChannelAudio
{
	AudioChannel *channel;
	AudioEntry entry;
} AudioCmdData_QueueChannelAudio;

typedef struct AudioCmdData_ClearChannelAudio
{
	AudioChannel *channel;
} AudioCmdData_ClearChannelAudio;

typedef struct AudioCmdData_IsChannelPlaying
{
	AudioChannel *channel;
	int isPlaying;
} AudioCmdData_IsChannelPlaying;

static LRESULT AudioWnd_OnGetMasterVolume(AudioWnd *self, AudioCmdData_GetMasterVolume *cmdData)
{
	cmdData->masterVolume = AudioDevice_GetMasterVolume(&self->deviceState);
	return 0;
}

static LRESULT AudioWnd_OnSetMasterVolume(AudioWnd *self, AudioCmdData_SetMasterVolume *cmdData)
{
	AudioDevice_SetMasterVolume(&self->deviceState, cmdData->masterVolume);
	return 0;
}

static LRESULT AudioWnd_OnOpenChannel(AudioWnd *self, AudioCmdData_OpenChannel *cmdData)
{
	cmdData->channel = AudioDevice_OpenChannel(&self->deviceState, &cmdData->format);
	return 0;
}

static LRESULT AudioWnd_OnCloseChannel(AudioWnd *self, AudioCmdData_CloseChannel *cmdData)
{
	(void)self;
	AudioDevice_CloseChannel(cmdData->channel);
	return 0;
}

static LRESULT AudioWnd_OnQueueChannelAudio(AudioWnd *self, AudioCmdData_QueueChannelAudio *cmdData)
{
	(void)self;
	AudioDevice_QueueChannelAudio(cmdData->channel, &cmdData->entry);
	return 0;
}

static LRESULT AudioWnd_OnClearChannelAudio(AudioWnd *self, AudioCmdData_ClearChannelAudio *cmdData)
{
	(void)self;
	AudioDevice_ClearChannelAudio(cmdData->channel);
	return 0;
}

static LRESULT AudioWnd_OnIsChannelPlaying(AudioWnd *self, AudioCmdData_IsChannelPlaying *cmdData)
{
	(void)self;
	cmdData->isPlaying = AudioDevice_IsChannelPlaying(cmdData->channel);
	return 0;
}

static LRESULT AudioWnd_WndProc(AudioWnd *self, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		return AudioWnd_OnCreate(self, (LPCREATESTRUCT)lParam);

	case WM_DESTROY:
		return AudioWnd_OnDestroy(self);

	case WM_TIMER:
		switch (wParam)
		{
		case IDT_AUDIOTICK:
			AudioWnd_RunAudioTicks(self);
			break;
		}
		return 0;

	case AUDIOCMD_SHUTDOWN:
		return AudioWnd_OnShutdown(self);

	case AUDIOCMD_GETMASTERVOLUME:
		return AudioWnd_OnGetMasterVolume(self, (AudioCmdData_GetMasterVolume *)lParam);

	case AUDIOCMD_SETMASTERVOLUME:
		return AudioWnd_OnSetMasterVolume(self, (AudioCmdData_SetMasterVolume *)lParam);

	case AUDIOCMD_OPENCHANNEL:
		return AudioWnd_OnOpenChannel(self, (AudioCmdData_OpenChannel *)lParam);

	case AUDIOCMD_CLOSECHANNEL:
		return AudioWnd_OnCloseChannel(self, (AudioCmdData_CloseChannel *)lParam);

	case AUDIOCMD_QUEUECHANNELAUDIO:
		return AudioWnd_OnQueueChannelAudio(self, (AudioCmdData_QueueChannelAudio *)lParam);

	case AUDIOCMD_CLEARCHANNELAUDIO:
		return AudioWnd_OnClearChannelAudio(self, (AudioCmdData_ClearChannelAudio *)lParam);

	case AUDIOCMD_ISCHANNELPLAYING:
		return AudioWnd_OnIsChannelPlaying(self, (AudioCmdData_IsChannelPlaying *)lParam);

	default:
		return DefWindowProc(self->hwndSelf, message, wParam, lParam);
	}
}

static LRESULT CALLBACK
AudioWnd_StaticWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	AudioWnd *self;
	LRESULT result;

	self = (AudioWnd *)GetWindowLongPtr(hwnd, 0);
	if (self == NULL)
	{
		if (message != WM_NCCREATE)
		{
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
		self = (AudioWnd *)calloc(1, sizeof(*self));
		if (self == NULL)
		{
			SetLastError(ERROR_OUTOFMEMORY);
			return FALSE;
		}
		self->hwndSelf = hwnd;
		SetWindowLongPtr(hwnd, 0, (LONG_PTR)self);
	}
	result = AudioWnd_WndProc(self, message, wParam, lParam);
	if (message == WM_NCDESTROY)
	{
		free(self);
		SetWindowLongPtr(hwnd, 0, (LONG_PTR)NULL);
	}
	return result;
}

typedef struct AudioThreadParams
{
	HWND audioWindow;
	HANDLE hReadyEvent;
} AudioThreadParams;

static unsigned int __stdcall AudioThreadProc(void *context)
{
	AudioThreadParams *params;
	unsigned int exitCode;
	HRESULT hr;
	WNDCLASSEX wc;
	HWND hwnd;
	MSG msg;

	params = (AudioThreadParams *)context;
	exitCode = 0;
	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		ZeroMemory(&wc, sizeof(wc));
		wc.cbSize = sizeof(wc);
		wc.style = 0;
		wc.lpfnWndProc = AudioWnd_StaticWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = sizeof(LONG_PTR);
		wc.hInstance = HINST_THISCOMPONENT;
		wc.hIcon = NULL;
		wc.hCursor = NULL;
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = TEXT("AudioOwner");
		wc.hIconSm = NULL;
		RegisterClassEx(&wc);

		hwnd = CreateWindowEx(0, TEXT("AudioOwner"), TEXT(""), 0x00000000,
			0, 0, 0, 0, HWND_MESSAGE, NULL, HINST_THISCOMPONENT, NULL);
		if (hwnd != NULL)
		{
			params->audioWindow = hwnd;
			// Make sure message queue is ready before setting event
			PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
			SetEvent(params->hReadyEvent);
			while (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			exitCode = (unsigned int)msg.wParam;
		}

		CoUninitialize();
	}
	return exitCode;
}

//===========================================================================//

static HWND g_audioWindow;

static HWND Audio_GetMessageWindow(void)
{
	return (HWND)InterlockedCompareExchangePointer((PVOID *)&g_audioWindow, NULL, NULL);
}

static HWND Audio_SetMessageWindow(HWND hwnd)
{
	return (HWND)InterlockedExchangePointer((PVOID *)&g_audioWindow, (PVOID)hwnd);
}

static void Audio_SendThreadMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	SendMessage(hwnd, message, wParam, lParam);
}

int Audio_InitDevice(void)
{
	AudioThreadParams params;
	HANDLE audioThread;
	HANDLE waitHandles[2];
	DWORD waitResult;

	if (Audio_GetMessageWindow() != NULL)
	{
		return FALSE;
	}
	params.audioWindow = NULL;
	params.hReadyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (params.hReadyEvent == NULL)
	{
		return FALSE;
	}
	audioThread = (HANDLE)_beginthreadex(NULL, 0, AudioThreadProc, &params, 0, NULL);
	if (audioThread == NULL)
	{
		CloseHandle(params.hReadyEvent);
		return FALSE;
	}
	waitHandles[0] = params.hReadyEvent;
	waitHandles[1] = audioThread;
	waitResult = WaitForMultipleObjects(ARRAYSIZE(waitHandles), waitHandles, FALSE, INFINITE);
	// The event handle and thread handle aren't needed any more, so
	// we close them now. The thread object will continue to exist
	// until the thread exits (prompted by a call to Audio_KillDevice).
	CloseHandle(params.hReadyEvent);
	CloseHandle(audioThread);
	if (waitResult != (WAIT_OBJECT_0 + 0))
	{
		// thread shut down early (meaning initialization failed somewhere)
		return FALSE;
	}
	// thread is ready to go
	Audio_SetMessageWindow(params.audioWindow);
	return TRUE;
}

void Audio_KillDevice(void)
{
	HWND audioWindow;

	audioWindow = Audio_SetMessageWindow(NULL);
	if (audioWindow != NULL)
	{
		Audio_SendThreadMessage(audioWindow, AUDIOCMD_SHUTDOWN, 0, 0);
		// The audio thread itself will exit shortly after this call.
		// We hold no handles to the thread, so the underlying object
		// will destroy itself soon enough.
	}
}

float Audio_GetMasterVolume(void)
{
	AudioCmdData_GetMasterVolume cmdData;
	HWND audioWindow;

	cmdData.masterVolume = 0.0f;
	audioWindow = Audio_GetMessageWindow();
	if (audioWindow != NULL)
	{
		Audio_SendThreadMessage(audioWindow, AUDIOCMD_GETMASTERVOLUME, 0, (LPARAM)&cmdData);
	}
	return cmdData.masterVolume;
}

void Audio_SetMasterVolume(float newVolume)
{
	AudioCmdData_SetMasterVolume cmdData;
	HWND audioWindow;

	cmdData.masterVolume = newVolume;
	audioWindow = Audio_GetMessageWindow();
	if (audioWindow != NULL)
	{
		Audio_SendThreadMessage(audioWindow, AUDIOCMD_SETMASTERVOLUME, 0, (LPARAM)&cmdData);
	}
}

AudioChannel *AudioChannel_Open(const WaveFormat *format)
{
	AudioCmdData_OpenChannel cmdData;
	HWND audioWindow;

	cmdData.format = *format;
	cmdData.channel = NULL;
	audioWindow = Audio_GetMessageWindow();
	if (audioWindow != NULL)
	{
		Audio_SendThreadMessage(audioWindow, AUDIOCMD_OPENCHANNEL, 0, (LPARAM)&cmdData);
	}
	return cmdData.channel;
}

void AudioChannel_Close(AudioChannel *channel)
{
	AudioCmdData_CloseChannel cmdData;
	HWND audioWindow;

	cmdData.channel = channel;
	audioWindow = Audio_GetMessageWindow();
	if (audioWindow != NULL)
	{
		Audio_SendThreadMessage(audioWindow, AUDIOCMD_CLOSECHANNEL, 0, (LPARAM)&cmdData);
	}
}

void AudioChannel_QueueAudio(AudioChannel *channel, const AudioEntry *entry)
{
	AudioCmdData_QueueChannelAudio cmdData;
	HWND audioWindow;

	cmdData.channel = channel;
	cmdData.entry = *entry;
	audioWindow = Audio_GetMessageWindow();
	if (audioWindow != NULL)
	{
		Audio_SendThreadMessage(audioWindow, AUDIOCMD_QUEUECHANNELAUDIO, 0, (LPARAM)&cmdData);
	}
}

void AudioChannel_ClearAudio(AudioChannel *channel)
{
	AudioCmdData_ClearChannelAudio cmdData;
	HWND audioWindow;

	cmdData.channel = channel;
	audioWindow = Audio_GetMessageWindow();
	if (audioWindow != NULL)
	{
		Audio_SendThreadMessage(audioWindow, AUDIOCMD_CLEARCHANNELAUDIO, 0, (LPARAM)&cmdData);
	}
}

int AudioChannel_IsPlaying(AudioChannel *channel)
{
	AudioCmdData_IsChannelPlaying cmdData;
	HWND audioWindow;

	cmdData.channel = channel;
	cmdData.isPlaying = 0;
	audioWindow = Audio_GetMessageWindow();
	if (audioWindow != NULL)
	{
		Audio_SendThreadMessage(audioWindow, AUDIOCMD_ISCHANNELPLAYING, 0, (LPARAM)&cmdData);
	}
	return cmdData.isPlaying;
}
