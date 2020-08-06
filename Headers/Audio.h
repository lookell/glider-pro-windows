#ifndef AUDIO_H_
#define AUDIO_H_

#include <stddef.h>
#include <stdint.h>
#include "WinAPI.h"
#include <dsound.h>

typedef struct WaveFormat
{
	uint16_t channels;
	uint16_t bitsPerSample;
	uint32_t samplesPerSec;
} WaveFormat;

typedef struct WaveData
{
	WaveFormat format;
	size_t dataLength;
	const unsigned char *dataBytes;
} WaveData;

// Parse a WAV file from an in-memory representation, and write the data
// into the waveData output parameter. Return nonzero on success and zero
// on failure.
//
// The pointer written into waveData->dataBytes is simply a pointer into the
// given buffer, and does not need to be freed separately.
int ReadWAVFromMemory(const void *buffer, size_t length, WaveData *waveData);

// Initialize the internal audio-management structures.
//
// This function must be called only once (at the beginning of the program),
// and before calling any other Audio_Xxx functions. The behavior is undefined
// if this rule is not followed. Call Audio_KillDevice when you are finished
// with the audio interface.
HRESULT Audio_InitDevice(void);

// Release the internal audio-management structures.
//
// This function must be called only once (at the end of the program), and only
// after a successful call to Audio_InitDevice. No other Audio_Xxx functions
// should be running while Audio_KillDevice releases structures, or else the
// behavior is undefined.
void Audio_KillDevice(void);

// Create a secondary sound buffer that is attached to the audio output device.
// The parameters are similar to those of IDirectSound8::CreateSoundBuffer,
// except that LPDIRECTSOUNDBUFFER8 is used instead of LPDIRECTSOUNDBUFFER
// (without the 8).
//
// This function provides a wrapper around the QueryInterface dance required to
// obtain IDirectSoundBuffer8 (instead of IDirectSoundBuffer, without the 8),
// and also allows Audio_SetVolume to affect the buffer's volume. The interface
// pointer must be released via the Audio_ReleaseSoundBuffer function, so that
// the interface can be removed from the internal volume-changing list.
HRESULT Audio_CreateSoundBuffer(
	LPCDSBUFFERDESC pcDSBufferDesc,
	LPDIRECTSOUNDBUFFER8 *ppDSBuffer,
	LPUNKNOWN pUnkOuter);

// Create a new secondary sound buffer that shares the memory of another secondary
// sound buffer. The parameters of this function are similar to those of
// IDirectSound8::DuplicateSoundBuffer, except that LPDIRECTSOUNDBUFFER8 is used
// instead of LPDIRECTSOUNDBUFFER (without the 8). The interface pointer returned
// by this function should be released using Audio_ReleaseSoundBuffer, so that it
// can be removed from the interface list for volume control.
//
// This function provides a wrapper around the QueryInterface dance to convert from
// IDirectSoundBuffer8 into IDirectSoundBuffer and then back. It also handles the
// following known issue, quoting from Microsoft's documentation for
// IDirectSound8::DuplicateSoundBuffer:
//
//     "There is a known issue with volume levels of duplicated buffers. The
//      duplicated buffer will play at full volume unless you change the volume to
//      a different value than the original buffer's volume setting. If the volume
//      stays the same (even if you explicitly set the same volume in the duplicated
//      buffer with a IDirectSoundBuffer8::SetVolume call), the buffer will play
//      at full volume regardless. To work around this problem, immediately set the
//      volume of the duplicated buffer to something slightly different than what it
//      was, even if you change it one millibel. The volume may then be immediately
//      set back again to the original desired value."
HRESULT Audio_DuplicateSoundBuffer(
	LPDIRECTSOUNDBUFFER8 pDSBufferOriginal,
	LPDIRECTSOUNDBUFFER8 *ppDSBufferDuplicate);

// Release an IDirectSoundBuffer8 interface pointer previously allocated
// with Audio_CreateSoundBuffer or Audio_DuplicateSoundBuffer.
void Audio_ReleaseSoundBuffer(LPDIRECTSOUNDBUFFER8 pBuffer);

// Get the master volume used for buffers created by Audio_CreateSoundBuffer or
// Audio_DuplicateSoundBuffer. The value ranges between 0.0 (for complete silence)
//and 1.0 (for full volume).
HRESULT Audio_GetMasterVolume(float *pVolume);

// Set the master volume used for buffers created by Audio_CreateSoundBuffer or
// Audio_DuplicateSoundBuffer. The value ranges between 0.0 (for complete silence)
//and 1.0 (for full volume).
HRESULT Audio_SetMasterVolume(float newVolume);

#endif
