#define CINTERFACE
#define COBJMACROS
#include "Audio.h"

#include "ResourceIDs.h"
#include <math.h>

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

static LPDIRECTSOUND8 g_audioDevice;
static HWND g_audioWindow;
static LPDIRECTSOUNDBUFFER8 *g_audioBuffers;
static DWORD g_audioBuffersCapacity;
static LONG g_masterAttenuation;

HRESULT Audio_InitDevice(void)
{
	LPDIRECTSOUND8 newDevice;
	WNDCLASSEX wcx;
	HWND newWindow;
	HRESULT hr;

	if (g_audioDevice != NULL)
	{
		return S_FALSE;
	}
	wcx.cbSize = sizeof(wcx);
	if (!GetClassInfoEx(HINST_THISCOMPONENT, L"GliderAudioOwner", &wcx))
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
		wcx.lpszClassName = L"GliderAudioOwner";
		wcx.hIconSm = NULL;
		if (!RegisterClassEx(&wcx))
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}
	}
	newWindow = CreateWindow(
		wcx.lpszClassName, L"", 0, 0, 0, 0, 0,
		HWND_MESSAGE, NULL, wcx.hInstance, NULL
	);
	if (newWindow == NULL)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}
	hr = DirectSoundCreate8(&DSDEVID_DefaultPlayback, &newDevice, NULL);
	if (FAILED(hr))
	{
		DestroyWindow(newWindow);
		return hr;
	}
	hr = IDirectSound8_SetCooperativeLevel(newDevice, newWindow, DSSCL_PRIORITY);
	if (FAILED(hr))
	{
		IDirectSound8_Release(newDevice);
		DestroyWindow(newWindow);
		return hr;
	}

	g_audioDevice = newDevice;
	g_audioWindow = newWindow;
	g_audioBuffers = NULL;
	g_audioBuffersCapacity = 0;
	g_masterAttenuation = DSBVOLUME_MAX;
	return S_OK;
}

void Audio_KillDevice(void)
{
	free(g_audioBuffers);
	g_audioBuffersCapacity = 0;
	if (g_audioWindow != NULL)
	{
		DestroyWindow(g_audioWindow);
		g_audioWindow = NULL;
	}
	if (g_audioDevice != NULL)
	{
		IDirectSound8_Release(g_audioDevice);
		g_audioDevice = NULL;
	}
}

HRESULT Audio_GetDevice(LPDIRECTSOUND8 *ppDS8)
{
	if (ppDS8 == NULL)
	{
		return E_POINTER;
	}
	*ppDS8 = NULL;
	if (g_audioDevice == NULL)
	{
		return DSERR_UNINITIALIZED;
	}
	IDirectSound8_AddRef(g_audioDevice);
	*ppDS8 = g_audioDevice;
	return S_OK;
}

static HRESULT Audio_AllocBufferSlot(DWORD *pBufferSlot)
{
	LPDIRECTSOUNDBUFFER8 *newBuffersPtr;
	DWORD newCapacity;
	DWORD i;

	if (pBufferSlot == NULL)
	{
		return E_POINTER;
	}
	for (i = 0; i < g_audioBuffersCapacity; i++)
	{
		if (g_audioBuffers[i] == NULL)
		{
			*pBufferSlot = i;
			return S_OK;
		}
	}

	if (g_audioBuffersCapacity == 0)
	{
		newCapacity = 16;
	}
	else
	{
		newCapacity = 2 * g_audioBuffersCapacity;
	}
	newBuffersPtr = realloc(g_audioBuffers, newCapacity * sizeof(*newBuffersPtr));
	if (newBuffersPtr == NULL)
	{
		return E_OUTOFMEMORY;
	}
	g_audioBuffers = newBuffersPtr;
	for (i = g_audioBuffersCapacity; i < newCapacity; i++)
	{
		g_audioBuffers[i] = NULL;
	}
	*pBufferSlot = g_audioBuffersCapacity;
	g_audioBuffersCapacity = newCapacity;
	return S_OK;
}

HRESULT Audio_CreateSoundBuffer(
	LPCDSBUFFERDESC pcDSBufferDesc,
	LPDIRECTSOUNDBUFFER8 *ppDSBuffer,
	LPUNKNOWN pUnkOuter)
{
	LPDIRECTSOUNDBUFFER tempBuffer;
	LPDIRECTSOUNDBUFFER8 newBuffer;
	DWORD bufferSlot;
	HRESULT hr;

	if (ppDSBuffer == NULL)
	{
		return E_POINTER;
	}
	*ppDSBuffer = NULL;
	if (g_audioDevice == NULL)
	{
		return DSERR_UNINITIALIZED;
	}

	hr = Audio_AllocBufferSlot(&bufferSlot);
	if (FAILED(hr))
	{
		return hr;
	}
	hr = IDirectSound8_CreateSoundBuffer(g_audioDevice,
			pcDSBufferDesc, &tempBuffer, pUnkOuter);
	if (FAILED(hr))
	{
		return hr;
	}
	hr = IDirectSoundBuffer_QueryInterface(tempBuffer,
			&IID_IDirectSoundBuffer8, (LPVOID *)&newBuffer);
	IDirectSoundBuffer_Release(tempBuffer);
	if (FAILED(hr))
	{
		return hr;
	}
	IDirectSoundBuffer8_SetVolume(newBuffer, g_masterAttenuation);

	IDirectSoundBuffer_AddRef(newBuffer);
	g_audioBuffers[bufferSlot] = newBuffer;
	*ppDSBuffer = newBuffer;
	return S_OK;
}

HRESULT Audio_DuplicateSoundBuffer(
	LPDIRECTSOUNDBUFFER8 pDSBufferOriginal,
	LPDIRECTSOUNDBUFFER8 *ppDSBufferDuplicate)
{
	LPDIRECTSOUNDBUFFER tempOriginal, tempDuplicate;
	LPDIRECTSOUNDBUFFER8 newBuffer;
	LONG bufferVolume;
	DWORD bufferSlot;
	HRESULT hr;

	if (ppDSBufferDuplicate == NULL)
	{
		return E_POINTER;
	}
	*ppDSBufferDuplicate = NULL;
	if (g_audioDevice == NULL)
	{
		return DSERR_UNINITIALIZED;
	}
	if (pDSBufferOriginal == NULL)
	{
		return E_INVALIDARG;
	}

	hr = Audio_AllocBufferSlot(&bufferSlot);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = IDirectSoundBuffer8_QueryInterface(pDSBufferOriginal,
			&IID_IDirectSoundBuffer, (LPVOID *)&tempOriginal);
	if (FAILED(hr))
	{
		return E_UNEXPECTED;
	}

	hr = IDirectSound8_DuplicateSoundBuffer(g_audioDevice,
			tempOriginal, &tempDuplicate);
	IDirectSoundBuffer_Release(tempOriginal);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = IDirectSoundBuffer_QueryInterface(tempDuplicate,
			&IID_IDirectSoundBuffer8, (LPVOID *)&newBuffer);
	IDirectSoundBuffer_Release(tempDuplicate);
	if (FAILED(hr))
	{
		return E_UNEXPECTED;
	}

	// Work around a known issue with duplicate buffers by tweaking and untweaking
	// the buffer's volume by one millibel.
	hr = IDirectSoundBuffer8_GetVolume(newBuffer, &bufferVolume);
	if (SUCCEEDED(hr))
	{
		LONG tweakedVolume = bufferVolume - 1;
		if (tweakedVolume < DSBVOLUME_MIN)
		{
			tweakedVolume = bufferVolume + 1;
		}
		hr = IDirectSoundBuffer8_SetVolume(newBuffer, tweakedVolume);
		hr = IDirectSoundBuffer8_SetVolume(newBuffer, bufferVolume);
	}

	IDirectSoundBuffer8_AddRef(newBuffer);
	g_audioBuffers[bufferSlot] = newBuffer;
	*ppDSBufferDuplicate = newBuffer;
	return S_OK;
}

ULONG Audio_ReleaseSoundBuffer(LPDIRECTSOUNDBUFFER8 pBuffer)
{
	DWORD bufferSlot;

	for (bufferSlot = 0; bufferSlot < g_audioBuffersCapacity; bufferSlot++)
	{
		if (pBuffer == g_audioBuffers[bufferSlot])
		{
			IDirectSoundBuffer8_Release(g_audioBuffers[bufferSlot]);
			g_audioBuffers[bufferSlot] = NULL;
			break;
		}
	}
	return IDirectSoundBuffer8_Release(pBuffer);
}

//
// This factor makes it so when the desired volume is halved, then the
// attenuation goes down by 10 dB.
//
static const float VOLUME_FACTOR = 20.0f;

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

static float ClampVolume(float volume)
{
	const float MIN_VOLUME = powf(10.0f, DSBVOLUME_MIN / 100.0f / VOLUME_FACTOR);
	const float MAX_VOLUME = 1.0;

	if (volume <= MIN_VOLUME)
	{
		return 0.0;
	}
	else if (volume >= MAX_VOLUME)
	{
		return MAX_VOLUME;
	}
	else
	{
		return volume;
	}
}

static LONG VolumeToAttenuation(float volume)
{
	LONG attenuation;

	volume = ClampVolume(volume);
	attenuation = (LONG)(log10f(volume) * VOLUME_FACTOR * 100.0f);
	attenuation = ClampAttenuation(attenuation);
	return attenuation;
}

static float AttenuationToVolume(LONG attenuation)
{
	float volume;

	attenuation = ClampAttenuation(attenuation);
	volume = powf(10.0f, (float)attenuation / 100.0f / VOLUME_FACTOR);
	volume = ClampVolume(volume);
	return volume;
}

HRESULT Audio_GetMasterVolume(float *pVolume)
{
	if (pVolume == NULL)
	{
		return E_POINTER;
	}
	*pVolume = 0.0f;
	if (g_audioDevice == NULL)
	{
		return DSERR_UNINITIALIZED;
	}
	*pVolume = AttenuationToVolume(g_masterAttenuation);
	return S_OK;
}

HRESULT Audio_SetMasterVolume(float newVolume)
{
	DWORD bufferSlot;

	if (g_audioDevice == NULL)
	{
		return DSERR_UNINITIALIZED;
	}
	g_masterAttenuation = VolumeToAttenuation(newVolume);
	for (bufferSlot = 0; bufferSlot < g_audioBuffersCapacity; bufferSlot++)
	{
		if (g_audioBuffers[bufferSlot] != NULL)
		{
			IDirectSoundBuffer8_SetVolume(g_audioBuffers[bufferSlot], g_masterAttenuation);
		}
	}
	return S_OK;
}
