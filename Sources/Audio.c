#include "Audio.h"
#include "WinAPI.h"

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

typedef struct RiffChunk {
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
	waveData->channels = format.nChannels;
	waveData->samplesPerSec = format.nSamplesPerSec;
	waveData->bitsPerSample = format.wBitsPerSample;
	waveData->dataLength = chunk.ckSize;
	waveData->dataBytes = chunk.ckData;
	return 1;
}

int ReadWAVFromResource(void *hModule, uint16_t wavID, WaveData *waveData)
{
	HRSRC hRsrc;
	HGLOBAL hGlobal;
	DWORD resLength;
	LPVOID resPointer;

	if (waveData == NULL)
		return 0;

	hRsrc = FindResource(hModule, MAKEINTRESOURCEW(wavID), L"WAVE");
	if (hRsrc == NULL)
		return 0;
	hGlobal = LoadResource(hModule, hRsrc);
	if (hGlobal == NULL)
		return 0;
	resLength = SizeofResource(hModule, hRsrc);
	if (resLength == 0)
		return 0;
	resPointer = LockResource(hGlobal);
	if (resPointer == NULL)
		return 0;

	return ReadWAVFromMemory(resPointer, resLength, waveData);
}
