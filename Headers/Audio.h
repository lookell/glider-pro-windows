#ifndef GP_AUDIO_H_
#define GP_AUDIO_H_

#include <stddef.h>
#include <stdint.h>

typedef struct WaveData {
	uint16_t channels;
	uint32_t samplesPerSec;
	uint16_t bitsPerSample;
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

// Parse a WAV file from the specified "WAVE" resource (module and ID), and
// write the data into the waveData output parameter. Return nonzero on success
// and zero on failure. (Windows only: the first parameter is really a HMODULE).
//
// The pointer written into waveData->dataBytes is a pointer into the module's
// resources, so the pointer is valid as long as the module handle is valid.
int ReadWAVFromResource(void *hModule, uint16_t wavID, WaveData *waveData);

#endif
